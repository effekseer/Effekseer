using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.IO.Compression;
using System.Xml;
using System.Security.Cryptography;
using Effekseer.Data;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json;
using Effekseer.Utl;
using Microsoft.Scripting.Utils;

namespace Effekseer.IO
{
	public class EfkPkg
	{
		public const string MetaFileName = "metafile.json";

		public enum FileType
		{
			Effect,
			Texture,
			Sound,
			Model,
			Material,
			Curve,
		}

		public class FileInfo
		{
			public FileType Type;
			public string RelativePath;
			public string HashName;
			public DateTime LastWriteTime;
			public byte[] Data;
			public FileInfo[] Dependencies;
		}

		public List<FileInfo> EffectFiles = new List<FileInfo>();
		public List<FileInfo> ResourceFiles = new List<FileInfo>();

		public IEnumerable<FileInfo> AllFiles
		{ 
			get { return Enumerable.Concat(EffectFiles, ResourceFiles); }
		}

		public void AddCurrentEffect()
		{
			AddEffect(Core.Root.GetFullPath());
		}

		public FileInfo AddEffect(string path)
		{
			if (!File.Exists(path))
			{
				return null;
			}

			DateTime lastWriteTime = File.GetLastWriteTime(path);
			Data.NodeRoot rootNode = Core.LoadFromFile(path);

			var backedupDelegate = Core.OnFileLoaded;

			try
			{
				Core.OnFileLoaded = (filePath) =>
				{
					string hashName = Path.GetFileName(filePath);
					var entry = ResourceFiles.Find(r => r.HashName == hashName);
					return (entry != null) ? entry.Data : null;
				};

				// Load all dependent resource files
				var resourcePaths = Utils.Misc.FindResourcePaths(rootNode, Binary.ExporterVersion.Latest);
				var dependencies = AddEffectDependencies(resourcePaths);

				var efkefc = new EfkEfc();
				FileInfo file = new FileInfo();
				file.Type = FileType.Effect;
				file.RelativePath = Path.GetFileName(path);
				file.Data = efkefc.Save(rootNode, Core.SaveAsXmlDocument(rootNode));
				file.HashName = ComputeHashName(file.Data);
				file.LastWriteTime = lastWriteTime;
				file.Dependencies = dependencies;

				EffectFiles.Add(file);

				return file;
			}
			finally
			{
				Core.OnFileLoaded = backedupDelegate;
			}
		}

		public FileInfo AddResource(string absolutePath, string relativePath, FileType type)
		{
			if (!File.Exists(absolutePath))
			{
				return null;
			}

			DateTime lastWriteTime = File.GetLastWriteTime(absolutePath);
			byte[] resourceData = File.ReadAllBytes(absolutePath);
			if (resourceData == null)
			{
				throw new Exception("failed open file.");
			}

			string hashName = ComputeHashName(resourceData);
			FileInfo file = ResourceFiles.Find(r => r.HashName == hashName);
			if (file == null)
			{
				file = new FileInfo();
				file.Type = type;
				file.RelativePath = relativePath;
				file.Data = resourceData;
				file.HashName = hashName;
				file.LastWriteTime = lastWriteTime;
				ResourceFiles.Add(file);
			}
			return file;
		}

		private FileInfo[] AddEffectDependencies(Utils.ResourcePaths resourcePaths)
		{
			HashSet<FileInfo> dependencies = new HashSet<FileInfo>();
			for (int i = 0; i < resourcePaths.All.Length; i++)
			{
				FileType type;

				switch ((Utils.ResourcePaths.Type)i)
				{
					case Utils.ResourcePaths.Type.ColorTexture: type = FileType.Texture;  break;
					case Utils.ResourcePaths.Type.NormalTexture: type = FileType.Texture; break;
					case Utils.ResourcePaths.Type.DistortionTexture: type = FileType.Texture; break;
					case Utils.ResourcePaths.Type.Model: type = FileType.Model; break;
					case Utils.ResourcePaths.Type.Sound: type = FileType.Sound; break;
					case Utils.ResourcePaths.Type.Material: type = FileType.Material; break;
					case Utils.ResourcePaths.Type.Curve: type = FileType.Curve; break;
					default: throw new Exception("Unknown File type");
				}

				foreach (var resource in resourcePaths.All[i])
				{
					FileInfo file = AddResource(resource.AbsolutePath, resource.RelativePath, type);

					if (file != null)
					{
						file.Dependencies = AddResourceDependencies(file, resource);

						dependencies.Add(file);

						resource.SetRelativePathDirectly(file.HashName, true);
					}
				}
			}
			return dependencies.ToArray();
		}

		private FileInfo[] AddResourceDependencies(FileInfo file, Data.Value.Path resource)
		{
			if (resource is Data.Value.PathForModel)
			{
				if (Path.GetExtension(resource.AbsolutePath) != ".efkmodel")
				{
					FileInfo subfile = AddResource(
						Path.ChangeExtension(resource.AbsolutePath, ".efkmodel"),
						Path.ChangeExtension(resource.RelativePath, ".efkmodel"),
						FileType.Model);

					if (subfile != null)
					{
						return new FileInfo[] { subfile };
					}
				}
			}
			else if (resource is Data.Value.PathForCurve)
			{
				if (Path.GetExtension(resource.AbsolutePath) != ".efkcurve")
				{
					FileInfo subfile = AddResource(
						Path.ChangeExtension(resource.AbsolutePath, ".efkcurve"),
						Path.ChangeExtension(resource.RelativePath, ".efkcurve"),
						FileType.Curve);

					if (subfile != null)
					{
						return new FileInfo[] { subfile };
					}
				}
			}
			else if (resource is Data.Value.PathForMaterial)
			{
				var material = new MaterialInformation();
				if (material.Load(file.Data))
				{
					HashSet<FileInfo> dependencies = new HashSet<FileInfo>();

					bool result = ReplaceMaterialPaths(material, (path) => {
						FileInfo subfile = AddResource(
							Utils.Misc.GetAbsolutePath(resource.AbsolutePath, path), 
							path, FileType.Texture);

						if (subfile != null)
						{
							dependencies.Add(subfile);
							return subfile.HashName;
						}
						return path;
					});
					if (result == false)
					{
						return null;
					}

					file.Data = material.SaveToBytes();

					return dependencies.ToArray();
				}
			}

			return null;
		}

		public bool Export(string path)
		{
			// Delete when target file exists
			if (File.Exists(path))
			{
				File.Delete(path);
			}

			using (var zip = ZipFile.Open(path, ZipArchiveMode.Create))
			{
				var files = new JObject();

				// Export effect/resource files
				foreach (var file in AllFiles)
				{
					var entry = zip.CreateEntry(file.HashName);
					entry.LastWriteTime = file.LastWriteTime;
					using (var writer = new BinaryWriter(entry.Open()))
					{
						writer.Write(file.Data);
					}
					var fileinfo = new JObject();

					fileinfo["type"] = file.Type.ToString();
					fileinfo["relative_path"] = file.RelativePath;

					if (file.Dependencies != null)
					{
						fileinfo["dependencies"] = new JArray(file.Dependencies.Select(d => d.HashName).ToArray());
					}

					files.Add(file.HashName, fileinfo);
				}

				// Write metafile.json
				{
					JObject metafile = new JObject
					{
						["version"] = Core.Version,
						["files"] = files,
					};
					string jsonMetafile = JsonConvert.SerializeObject(metafile);
					var entry = zip.CreateEntry(MetaFileName);

					using (var writer = new StreamWriter(entry.Open(), Encoding.UTF8))
					{
						writer.Write(jsonMetafile);
					}
				}
			}

			return true;
		}

		public bool Import(string path)
		{
			if (!File.Exists(path))
			{
				return false;
			}

			using (var zip = ZipFile.OpenRead(path))
			{
				// Read and parse a metafile.json
				JObject metafile = null;
				{
					var entry = zip.GetEntry(MetaFileName);
					if (entry == null)
					{
						return false;
					}

					using (var reader = new StreamReader(entry.Open()))
					{
						string jsonMetafile = reader.ReadToEnd();

						metafile = JsonConvert.DeserializeObject<JObject>(jsonMetafile);
						if (metafile == null)
						{
							return false;
						}
					}
				}

				// import files
				var files = metafile["files"].ToObject<Dictionary<string, JObject>>();
				foreach (var kv in files)
				{
					var file = new FileInfo();
					var entry = zip.GetEntry(kv.Key);
					file.HashName = kv.Key;
					file.Type = (FileType)Enum.Parse(typeof(FileType), (string)kv.Value["type"]);

					if (kv.Value.ContainsKey("name"))
					{
						// Compatiblity
						file.RelativePath = (string)kv.Value["name"];
					}
					else
					{
						file.RelativePath = (string)kv.Value["relative_path"];
					}

					file.LastWriteTime = entry.LastWriteTime.DateTime;
					file.Data = new System.IO.BinaryReader(entry.Open()).ReadBytes((int)entry.Length);

					if (file.Type == FileType.Effect)
					{
						EffectFiles.Add(file);
					}
					else
					{
						ResourceFiles.Add(file);
					}
				}

				// Resolve file dependencies
				foreach (var kv in files)
				{
					if (kv.Value.ContainsKey("dependencies"))
					{
						var file = AllFiles.First(f => f.HashName == kv.Key);

						List<FileInfo> dependencies = new List<FileInfo>();
						foreach (var token in kv.Value["dependencies"])
						{
							string hashName = (string)token;
							var resourceFile = AllFiles.First(f => f.HashName == hashName);
							if (resourceFile == null) throw new Exception("Failed to resolve dependencies.");
							dependencies.Add(resourceFile);
						}
						file.Dependencies = dependencies.ToArray();
					}
				}
			}

			return true;
		}

		public bool ExtractFiles(string dirPath, FileInfo[] extractFiles = null)
		{
			if (!Directory.Exists(dirPath))
			{
				return false;
			}

			if (extractFiles == null)
			{
				extractFiles = AllFiles.ToArray();
			}

			var backedupDelegate = Core.OnFileLoaded;

			try
			{
				// Extract resource files
				foreach (var file in ResourceFiles)
				{
					if (!extractFiles.Contains(file))
					{
						continue;
					}

					string filePath = Path.Combine(dirPath, file.RelativePath);
					string resourceDirPath = Path.GetDirectoryName(filePath);
					if (!Directory.Exists(resourceDirPath))
					{
						Directory.CreateDirectory(resourceDirPath);
					}

					byte[] data = ApplyResourceDependencies(file, dirPath, filePath);
					if (data != null)
					{
						File.WriteAllBytes(filePath, data);
						File.SetLastWriteTime(filePath, file.LastWriteTime);
					}
				}

				// Extract effect files
				foreach (var file in EffectFiles)
				{
					if (!extractFiles.Contains(file))
					{
						continue;
					}

					string filePath = Path.Combine(dirPath, file.RelativePath);

					EfkEfc efkefc = new EfkEfc();
					var doc = efkefc.Load(file.Data);
					if (doc == null) return false;

					Core.OnFileLoaded = (path) =>
					{
						string hashName = Path.GetFileName(path);
						var entry = ResourceFiles.Find(r => r.HashName == hashName);
						return (entry != null) ? entry.Data : null;
					};

					NodeRoot root = Core.LoadFromXml(doc, filePath);
					if (root == null) return false;

					// Resolve the path of dependent resources
					var resourcePaths = Utils.Misc.FindResourcePaths(root, Binary.ExporterVersion.Latest);
					foreach (var list in resourcePaths.All)
					{
						ApplyEffectDependencies(list);
					}

					// Write effect file
					Core.OnFileLoaded = backedupDelegate;
					byte[] data = efkefc.Save(root, Core.SaveAsXmlDocument(root));
					File.WriteAllBytes(filePath, data);
					File.SetLastWriteTime(filePath, file.LastWriteTime);
				}
			}
			finally
			{
				Core.OnFileLoaded = backedupDelegate;
			}

			return true;
		}

		private void ApplyEffectDependencies(List<Data.Value.Path> resourceList)
		{
			foreach (var resource in resourceList)
			{
				string hashName = resource.GetRelativePath();
				var entry = ResourceFiles.Find(r => r.HashName == hashName);
				if (entry != null)
				{
					resource.SetRelativePathDirectly(entry.RelativePath, true);
				}
			}
		}

		private byte[] ApplyResourceDependencies(FileInfo file, string exportDirPath, string resourcePath)
		{
			if (file.Type == FileType.Material)
			{
				var material = new MaterialInformation();
				if (material.Load(file.Data))
				{
					bool result = ReplaceMaterialPaths(material, (path) =>
					{
						var resfile = AllFiles.First(f => f.HashName == path);
						var fullPath = Path.Combine(exportDirPath, resfile.RelativePath);
						fullPath = Utils.Misc.BackSlashToSlash(fullPath);
						path = Utils.Misc.GetRelativePath(resourcePath, fullPath);
						return path;
					});

					return material.SaveToBytes();
				}
				return null;
			}
			else
			{
				return file.Data;
			}
		}

		static public string ComputeHashName(byte[] data)
		{
			var hashMethod = new MD5CryptoServiceProvider();
			byte[] hash = hashMethod.ComputeHash(data);
			string hashStr = string.Join("", hash.Select(v => v.ToString("X2")).ToArray());
			hashStr += "-" + data.Length.ToString("X8");
			return hashStr;
		}

		private bool ReplaceMaterialPaths(MaterialInformation material, Func<string, string> callback)
		{
			foreach (var texture in material.Textures)
			{
				if (texture.DefaultPath != string.Empty)
				{
					texture.DefaultPath = callback(texture.DefaultPath);
				}
			}

			try
			{
				var json = material.EditorData;

				foreach (var texture in (JArray)json["Textures"])
				{
					string path = (string)texture["Path"];
					if (!string.IsNullOrEmpty(path))
					{
						texture["Path"] = callback(path);
					}
				}

				foreach (var node in (JArray)json["Nodes"])
				{
					string type = (string)node["Type"];
					if (type == "SampleTexture")
					{
						string path = (string)node["Props"][0]["Value"];
						if (!string.IsNullOrEmpty(path))
						{
							node["Props"][0]["Value"] = callback(path);
						}
					}
					else if (type == "TextureObjectParameter")
					{
						string path = (string)node["Props"][2]["Value"];
						if (!string.IsNullOrEmpty(path))
						{
							node["Props"][2]["Value"] = callback(path);
						}
					}
				}
			}
			catch (Exception e)
			{
				Console.WriteLine("Material Error: " + e.ToString());
				return false;
			}

			return true;
		}
	}
}
