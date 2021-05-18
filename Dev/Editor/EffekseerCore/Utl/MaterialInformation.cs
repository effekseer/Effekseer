using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Effekseer.Utl
{
	public enum CompiledMaterialPlatformType : int
	{
		DirectX9 = 0,
		// DirectX10 = 1,
		DirectX11 = 2,
		DirectX12 = 3,
		OpenGL = 10,
		Metal = 20,
		Vulkan = 30,
		PS4 = 40,
		Switch = 50,
		XBoxOne = 60,
		PS5 = 70,
	}

	public enum TextureType : int
	{
		Color,
		Value,
	}

	public enum CompiledMaterialInformationErrorCode
	{
		OK,
		TooOldFormat,
		TooNewFormat,
		NotFound,
		FailedToOpen,
		InvalidFormat,
	}

	public enum MaterialVersion : int
	{
		Version0 = 0,
		Version15 = 3,
		Version16 = 1610,
	}

	public enum CompiledMaterialVersion : int
	{
		Version0 = 0,
		Version15 = 1,
		Version16 = 1610,
	}

	/// <summary>
	/// An information of file of compiled material's header
	/// </summary>
	public class CompiledMaterialInformation
	{
		const CompiledMaterialVersion LatestSupportVersion = CompiledMaterialVersion.Version16;
		const CompiledMaterialVersion OldestSupportVersion = CompiledMaterialVersion.Version16;

		public UInt64 GUID;
		public int Version;
		public HashSet<CompiledMaterialPlatformType> Platforms = new HashSet<CompiledMaterialPlatformType>();

		public CompiledMaterialInformationErrorCode Load(string path)
		{
			if (string.IsNullOrEmpty(path))
				return CompiledMaterialInformationErrorCode.NotFound;

			System.IO.FileStream fs = null;
			if (!System.IO.File.Exists(path)) return CompiledMaterialInformationErrorCode.NotFound;

			try
			{
				fs = System.IO.File.Open(path, System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read);
			}
			catch
			{
				return CompiledMaterialInformationErrorCode.FailedToOpen;
			}


			var br = new System.IO.BinaryReader(fs);

			var buf = new byte[1024];


			if (br.Read(buf, 0, 20) != 20)
			{
				fs.Dispose();
				br.Close();
				return CompiledMaterialInformationErrorCode.InvalidFormat;
			}

			if (buf[0] != 'e' ||
				buf[1] != 'M' ||
				buf[2] != 'C' ||
				buf[3] != 'B')
			{
				fs.Dispose();
				br.Close();
				return CompiledMaterialInformationErrorCode.InvalidFormat;
			}

			Version = BitConverter.ToInt32(buf, 4);

			// bacause of camera position node, structure of uniform is changed
			if (Version < (int)OldestSupportVersion)
			{
				fs.Dispose();
				br.Close();
				return CompiledMaterialInformationErrorCode.TooOldFormat;
			}

			if (Version > (int)LatestSupportVersion)
			{
				fs.Dispose();
				br.Close();
				return CompiledMaterialInformationErrorCode.TooNewFormat;
			}

			int fversion = BitConverter.ToInt32(buf, 4);

			GUID = BitConverter.ToUInt64(buf, 8);

			var platformCount = BitConverter.ToInt32(buf, 16);

			for(int i = 0; i < platformCount; i++)
			{
				if (br.Read(buf, 0, 4) != 4)
				{
					fs.Dispose();
					br.Close();
					return CompiledMaterialInformationErrorCode.InvalidFormat;
				}

				var type = (CompiledMaterialPlatformType)BitConverter.ToInt32(buf, 0);
				Platforms.Add(type);
			}

			fs.Dispose();
			br.Close();
			return CompiledMaterialInformationErrorCode.OK;
		}
	}

	public class MaterialInformation
	{
		const MaterialVersion LatestSupportVersion = MaterialVersion.Version16;

		public MaterialVersion Version = MaterialVersion.Version16;

		public TextureInformation[] Textures = new TextureInformation[0];

		public UniformInformation[] Uniforms = new UniformInformation[0];

		public CustomDataInformation[] CustomData = new CustomDataInformation[0];

		public UInt64 GUID;

		public int CustomData1Count = 0;

		public int CustomData2Count = 0;

		public bool HasNormal = false;

		public bool HasRefraction = false;

		public Dictionary<Language, string> Names = new Dictionary<Language, string>();

		public Dictionary<Language, string> Descriptions = new Dictionary<Language, string>();

		public string Code = string.Empty;

		public JObject EditorData = null;

		public int ShadingModel = 0;

		public bool Load(string path)
		{
			if (string.IsNullOrEmpty(path))
				return false;

			byte[] file = null;

			if (Core.OnFileLoaded != null)
			{
				file = Core.OnFileLoaded(path);
				if (file == null) return false;
			}
			else
			{
				if (!System.IO.File.Exists(path)) return false;

				try
				{
					file = System.IO.File.ReadAllBytes(path);
				}
				catch
				{
					return false;
				}
			}

			return Load(file);
		}

		public bool Load(byte[] file)
		{
			var br = new System.IO.BinaryReader(new System.IO.MemoryStream(file));
			var buf = new byte[1024];

			if (br.Read(buf, 0, 16) != 16)
			{
				br.Close();
				return false;
			}

			if(buf[0] != 'E' ||
				buf[1] != 'F' ||
				buf[2] != 'K' ||
				buf[3] != 'M')
			{
				return false;
			}

			int version = BitConverter.ToInt32(buf, 4);

			if(version > (int)LatestSupportVersion)
			{
				return false;
			}

			GUID = BitConverter.ToUInt64(buf, 8);

			while (true)
			{
				if (br.Read(buf, 0, 8) != 8)
				{
					br.Close();
					break;
				}

				if (buf[0] == 'D' &&
				buf[1] == 'E' &&
				buf[2] == 'S' &&
				buf[3] == 'C')
				{
					var temp = new byte[BitConverter.ToInt32(buf, 4)];
					if (br.Read(temp, 0, temp.Length) != temp.Length) return false;

					var reader = new BinaryReader(temp);

					int count = 0;
					reader.Get(ref count);

					for (int i = 0; i < count; i++)
					{
						int lang = 0;
						string name = null;
						string desc = null;
						reader.Get(ref lang);
						reader.Get(ref name, Encoding.UTF8);
						reader.Get(ref desc, Encoding.UTF8);
						Names.Add((Language)lang, name);
						Descriptions.Add((Language)lang, desc);
					}
				}

				if (buf[0] == 'P' &&
				buf[1] == 'R' &&
				buf[2] == 'M' &&
				buf[3] == '_')
				{
					var temp = new byte[BitConverter.ToInt32(buf, 4)];
					if (br.Read(temp, 0, temp.Length) != temp.Length) return false;

					var reader = new BinaryReader(temp);

					reader.Get(ref ShadingModel);

					reader.Get(ref HasNormal);

					reader.Get(ref HasRefraction);

					reader.Get(ref CustomData1Count);

					reader.Get(ref CustomData2Count);

					int textureCount = 0;
					reader.Get(ref textureCount);

					List<TextureInformation> textures = new List<TextureInformation>();

					for (int i = 0; i < textureCount; i++)
					{
						TextureInformation info = new TextureInformation();

						reader.Get(ref info.Name, Encoding.UTF8);

						// name is for human, uniformName is a variable name after 3
						if (version >= 3)
						{
							reader.Get(ref info.UniformName, Encoding.UTF8);
						}
						else
						{
							info.UniformName = info.Name;
						}

						reader.Get(ref info.DefaultPath, Encoding.UTF8);
						reader.Get(ref info.Index);
						reader.Get(ref info.Priority);
						reader.Get(ref info.IsParam);
						int textureType = 0;
						reader.Get(ref textureType);
						info.Type = (TextureType)textureType;
						reader.Get(ref info.Sampler);

						// convert a path into absolute
						if (string.IsNullOrEmpty(info.DefaultPath))
						{
							info.DefaultPath = string.Empty;
						}

						textures.Add(info);
					}

					Textures = textures.ToArray();

					int uniformCount = 0;
					reader.Get(ref uniformCount);

					List<UniformInformation> uniforms = new List<UniformInformation>();

					for (int i = 0; i < uniformCount; i++)
					{
						UniformInformation info = new UniformInformation();

						reader.Get(ref info.Name, Encoding.UTF8);

						// name is for human, uniformName is a variable name after 3
						if (version >= 3)
						{
							reader.Get(ref info.UniformName, Encoding.UTF8);
						}
						else
						{
							info.UniformName = info.Name;
						}

						reader.Get(ref info.Offset);
						reader.Get(ref info.Priority);
						reader.Get(ref info.Type);
						reader.Get(ref info.DefaultValues[0]);
						reader.Get(ref info.DefaultValues[1]);
						reader.Get(ref info.DefaultValues[2]);
						reader.Get(ref info.DefaultValues[3]);
						uniforms.Add(info);
					}

					Uniforms = uniforms.ToArray();
				}

				if (buf[0] == 'P' &&
				buf[1] == 'R' &&
				buf[2] == 'M' &&
				buf[3] == '2')
				{
					var temp = new byte[BitConverter.ToInt32(buf, 4)];
					if (br.Read(temp, 0, temp.Length) != temp.Length) return false;

					var reader = new BinaryReader(temp);

					if (version >= 2)
					{
						int customDataCount = 0;
						reader.Get(ref customDataCount);
						AllocateCustomData(customDataCount);

						for (int j = 0; j < customDataCount; j++)
						{
							int count = 0;
							reader.Get(ref count);

							for (int i = 0; i < count; i++)
							{
								int lang = 0;
								string name = null;
								string desc = null;
								reader.Get(ref lang);
								reader.Get(ref name, Encoding.UTF8);
								reader.Get(ref desc, Encoding.UTF8);
								CustomData[j].Summaries.Add((Language)lang, name);
								CustomData[j].Descriptions.Add((Language)lang, desc);
							}
						}
					}

					int textureCount = 0;
					reader.Get(ref textureCount);

					for (int j = 0; j < textureCount; j++)
					{
						int count = 0;
						reader.Get(ref count);

						for (int i = 0; i < count; i++)
						{
							int lang = 0;
							string name = null;
							string desc = null;
							reader.Get(ref lang);
							reader.Get(ref name, Encoding.UTF8);
							reader.Get(ref desc, Encoding.UTF8);
							Textures[j].Summaries.Add((Language)lang, name);
							Textures[j].Descriptions.Add((Language)lang, desc);
						}
					}

					int uniformCount = 0;
					reader.Get(ref uniformCount);

					for (int j = 0; j < uniformCount; j++)
					{
						int count = 0;
						reader.Get(ref count);

						for (int i = 0; i < count; i++)
						{
							int lang = 0;
							string name = null;
							string desc = null;
							reader.Get(ref lang);
							reader.Get(ref name, Encoding.UTF8);
							reader.Get(ref desc, Encoding.UTF8);
							Uniforms[j].Summaries.Add((Language)lang, name);
							Uniforms[j].Descriptions.Add((Language)lang, desc);
						}
					}
				}

				if (buf[0] == 'E' &&
				buf[1] == '_' &&
				buf[2] == 'C' &&
				buf[3] == 'D')
				{
					var temp = new byte[BitConverter.ToInt32(buf, 4)];
					if (br.Read(temp, 0, temp.Length) != temp.Length) return false;

					var reader = new BinaryReader(temp);

					int customDataCount = 0;
					reader.Get(ref customDataCount);
					AllocateCustomData(customDataCount);

					for (int j = 0; j < customDataCount; j++)
					{
						reader.Get(ref CustomData[j].DefaultValues[0]);
						reader.Get(ref CustomData[j].DefaultValues[1]);
						reader.Get(ref CustomData[j].DefaultValues[2]);
						reader.Get(ref CustomData[j].DefaultValues[3]);
					}
				}
				
				if (buf[0] == 'G' &&
				buf[1] == 'E' &&
				buf[2] == 'N' &&
				buf[3] == 'E')
				{
					var temp = new byte[BitConverter.ToInt32(buf, 4)];
					if (br.Read(temp, 0, temp.Length) != temp.Length) return false;

					var reader = new BinaryReader(temp);

					reader.Get(ref Code, Encoding.UTF8);
				}

				if (buf[0] == 'D' &&
				buf[1] == 'A' &&
				buf[2] == 'T' &&
				buf[3] == 'A')
				{
					var temp = new byte[BitConverter.ToInt32(buf, 4)];
					if (br.Read(temp, 0, temp.Length) != temp.Length) return false;

					string jsonText = Encoding.UTF8.GetString(temp);
					EditorData = JsonConvert.DeserializeObject<JObject>(jsonText);
				}
			}

			return true;
		}

		public byte[] SaveToBytes()
		{
			var ms = new System.IO.MemoryStream();
			var writer = new System.IO.BinaryWriter(ms);

			writer.Write(Encoding.ASCII.GetBytes("EFKM"));
			writer.Write((int)Version);
			writer.Write(GUID);

			{
				var bw = new Utils.BinaryWriter();
				bw.Push(Names.Count);
				var keys = Names.Keys.ToArray();
				for (int i = 0; i < keys.Length; i++)
				{
					bw.Push((int)keys[i]);
					bw.Push(Names[keys[i]], Encoding.UTF8);
					bw.Push(Descriptions[keys[i]], Encoding.UTF8);
				}

				var binary = bw.GetBinary();
				writer.Write(Encoding.ASCII.GetBytes("DESC"));
				writer.Write(binary.Length);
				writer.Write(binary);
			}

			{
				var bw = new Utils.BinaryWriter();

				bw.Push(ShadingModel);
				bw.Push(HasNormal);
				bw.Push(HasRefraction);
				bw.Push(CustomData1Count);
				bw.Push(CustomData2Count);

				bw.Push(Textures.Length);
				foreach (var info in Textures)
				{
					bw.Push(info.Name, Encoding.UTF8);
					bw.Push(info.UniformName, Encoding.UTF8);
					bw.Push(info.DefaultPath, Encoding.UTF8);
					bw.Push(info.Index);
					bw.Push(info.Priority);
					bw.Push(info.IsParam);
					bw.Push((int)info.Type);
					bw.Push(info.Sampler);
				}

				bw.Push(Uniforms.Length);
				foreach (var info in Uniforms)
				{
					bw.Push(info.Name, Encoding.UTF8);
					bw.Push(info.UniformName, Encoding.UTF8);
					
					bw.Push(info.Offset);
					bw.Push(info.Priority);
					bw.Push(info.Type);
					bw.Push(info.DefaultValues[0]);
					bw.Push(info.DefaultValues[1]);
					bw.Push(info.DefaultValues[2]);
					bw.Push(info.DefaultValues[3]);
				}

				var binary = bw.GetBinary();
				writer.Write(Encoding.ASCII.GetBytes("PRM_"));
				writer.Write(binary.Length);
				writer.Write(binary);
			}

			{
				var bw = new Utils.BinaryWriter();

				bw.Push(CustomData.Length);
				foreach (var data in CustomData)
				{
					bw.Push(data.Summaries.Count);
					var keys = data.Summaries.Keys.ToArray();
					foreach (var key in keys)
					{
						bw.Push((int)key);
						bw.Push(data.Summaries[key], Encoding.UTF8);
						bw.Push(data.Descriptions[key], Encoding.UTF8);
					}
				}

				bw.Push(Textures.Length);
				foreach (var texture in Textures)
				{
					bw.Push(texture.Summaries.Count);
					var keys = texture.Summaries.Keys.ToArray();
					foreach (var key in keys)
					{
						bw.Push((int)key);
						bw.Push(texture.Summaries[key], Encoding.UTF8);
						bw.Push(texture.Descriptions[key], Encoding.UTF8);
					}
				}

				bw.Push(Uniforms.Length);
				foreach (var uniform in Uniforms)
				{
					bw.Push(uniform.Summaries.Count);
					var keys = uniform.Summaries.Keys.ToArray();
					foreach (var key in keys)
					{
						bw.Push((int)key);
						bw.Push(uniform.Summaries[key], Encoding.UTF8);
						bw.Push(uniform.Descriptions[key], Encoding.UTF8);
					}
				}

				var binary = bw.GetBinary();
				writer.Write(Encoding.ASCII.GetBytes("PRM2"));
				writer.Write(binary.Length);
				writer.Write(binary);
			}

			{
				var bw = new Utils.BinaryWriter();

				bw.Push(CustomData.Length);

				for (int j = 0; j < CustomData.Length; j++)
				{
					bw.Push(CustomData[j].DefaultValues[0]);
					bw.Push(CustomData[j].DefaultValues[1]);
					bw.Push(CustomData[j].DefaultValues[2]);
					bw.Push(CustomData[j].DefaultValues[3]);
				}

				var binary = bw.GetBinary();
				writer.Write(Encoding.ASCII.GetBytes("E_CD"));
				writer.Write(binary.Length);
				writer.Write(binary);
			}

			{
				var bw = new Utils.BinaryWriter();
				bw.Push(Code, Encoding.UTF8);

				var binary = bw.GetBinary();
				writer.Write(Encoding.ASCII.GetBytes("GENE"));
				writer.Write(binary.Length);
				writer.Write(binary);
			}

			{
				string jsonText = JsonConvert.SerializeObject(EditorData);
				jsonText = jsonText.Replace("/", "\\/");

				var bw = new Utils.BinaryWriter();
				bw.Push(jsonText, Encoding.UTF8, true, 0);

				var binary = bw.GetBinary();
				writer.Write(Encoding.ASCII.GetBytes("DATA"));
				writer.Write(binary.Length);
				writer.Write(binary);
			}

			return ms.ToArray();
		}

		private void AllocateCustomData(int customDataCount)
		{
			if (CustomData.Count() == 0)
			{
				CustomData = new CustomDataInformation[customDataCount];
				for (int j = 0; j < customDataCount; j++)
				{
					CustomData[j] = new CustomDataInformation();
				}
			}
		}

		public class CustomDataInformation
		{
			public Dictionary<Language, string> Summaries = new Dictionary<Language, string>();
			public Dictionary<Language, string> Descriptions = new Dictionary<Language, string>();
			public float[] DefaultValues = new float[4];
		}


		public class TextureInformation
		{
			public string Name;
			public string UniformName;
			public int Index;
			public string DefaultPath;
			public bool IsParam;
			public int Sampler;
			public TextureType Type = TextureType.Color;
			public int Priority = 1;

			public Dictionary<Language, string> Summaries = new Dictionary<Language, string>();
			public Dictionary<Language, string> Descriptions = new Dictionary<Language, string>();
		}

		public class UniformInformation
		{
			public string Name;
			public string UniformName;
			public int Offset;
			public int Type = 0;
			public float[] DefaultValues = new float[4];
			public int Priority = 1;

			public Dictionary<Language, string> Summaries = new Dictionary<Language, string>();
			public Dictionary<Language, string> Descriptions = new Dictionary<Language, string>();
		}
	}
}
