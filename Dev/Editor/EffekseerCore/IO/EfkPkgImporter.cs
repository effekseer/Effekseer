using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Effekseer.Utils;

namespace Effekseer.IO
{
	public class EfkPkgImporter
	{
		public enum ResourceDestinationType
		{
			RelativePath,
			ResourceRootDirectory,
		}

		public class ResourceRoot
		{
			public EfkPkg.FileType Type;
			public string RootPath;
		}

		ResourceDestinationType resourceDestinationType = ResourceDestinationType.RelativePath;

		List<ResourceRoot> resourceRoots = new List<ResourceRoot>();

		public EfkPkgImporter()
		{
			resourceRoots.Add(new ResourceRoot { Type = EfkPkg.FileType.Effect, RootPath = "Effects/" });
			resourceRoots.Add(new ResourceRoot { Type = EfkPkg.FileType.Texture, RootPath = "Textures/" });
			resourceRoots.Add(new ResourceRoot { Type = EfkPkg.FileType.Material, RootPath = "Materials/" });
			resourceRoots.Add(new ResourceRoot { Type = EfkPkg.FileType.Sound, RootPath = "Sounds/" });
			resourceRoots.Add(new ResourceRoot { Type = EfkPkg.FileType.Curve, RootPath = "Curves/" });
		}

		public class ImportFile
		{
			public enum PathState
			{
				Writable,
				Overwrite,
				PathError,
			}

			public EfkPkg.FileInfo FileInfo;
			public bool DoesImport;
			public PathState State;
			public string DestinationRelativePath;

			internal EfkPkgImporter Importer;

			public string GetDestinationPath(string targetDirPath, List<ResourceRoot> resourceRoots, ResourceDestinationType resourceDestinationType)
			{
				if (resourceDestinationType == ResourceDestinationType.RelativePath)
				{
					return Misc.BackSlashToSlash(Path.Combine(targetDirPath, DestinationRelativePath));
				}
				else if (resourceDestinationType == ResourceDestinationType.ResourceRootDirectory)
				{
					var resourceRoot = resourceRoots.First(_ => _.Type == FileInfo.Type);
					return Misc.BackSlashToSlash(Path.Combine(targetDirPath, resourceRoot.RootPath, DestinationRelativePath));
				}

				throw new Exception();
			}

			public void ValidationPath()
			{
				if (!Directory.Exists(Importer.destinationPath))
				{
					State = PathState.PathError;
					return;
				}

				string fullPath = GetDestinationPath(Importer.destinationPath, Importer.resourceRoots, Importer.resourceDestinationType);

				if (!Misc.IsValidPath(fullPath))
				{
					State = PathState.PathError;
				}
				else if (File.Exists(fullPath))
				{
					State = PathState.Overwrite;
				}
				else
				{
					State = PathState.Writable;
				}
			}
		}

		string destinationPath;
		EfkPkg efkpkg;
		List<ImportFile> importedFiles = new List<ImportFile>();

		public bool IsDestinationPathValid { get; private set; }

		public IReadOnlyCollection<ImportFile> ImportedFiles
		{
			get
			{
				return importedFiles;
			}
		}

		public ResourceDestinationType ResourceDestination
		{
			get
			{
				return resourceDestinationType;
			}
			set
			{
				if (resourceDestinationType == value)
				{
					return;
				}

				resourceDestinationType = value;
			}
		}

		public EfkPkgImporter(EfkPkg efkpkg, string defaultDestinationPath)
		{
			this.destinationPath = defaultDestinationPath;
			this.efkpkg = efkpkg;

			if (destinationPath == null)
			{
				destinationPath = Directory.GetCurrentDirectory();
			}

			this.destinationPath = Misc.BackSlashToSlash(destinationPath);

			foreach (var file in efkpkg.AllFiles)
			{
				var import = new ImportFile();
				import.Importer = this;
				import.FileInfo = file;
				import.DestinationRelativePath = file.RelativePath;
				import.DoesImport = true;
				importedFiles.Add(import);
			}

			RenewIOStatus();
		}

		public void RenewIOStatus()
		{
			IsDestinationPathValid = Directory.Exists(destinationPath);

			foreach (var imported in importedFiles)
			{
				imported.ValidationPath();
			}
		}

		public string[] Import()
		{
			var files = importedFiles.Where(_ => _.DoesImport);

			foreach (var file in files)
			{
				file.FileInfo.RelativePath = file.DestinationRelativePath;
			}


			efkpkg.ExtractFiles(destinationPath, files.Select(_ => _.FileInfo).ToArray());

			var effects = files.Where(_ => _.FileInfo.Type == EfkPkg.FileType.Effect);
			return effects.Select(_ => Utils.Misc.BackSlashToSlash(Path.Combine(destinationPath, _.FileInfo.RelativePath))).ToArray();
		}

		/// <summary>
		/// Set file import settings
		/// </summary>
		/// <param name="file"></param>
		/// <param name="doesImport"></param>
		public void SetFileImportSettings(EfkPkg.FileInfo file, bool doesImport)
		{
			var import = importedFiles.FirstOrDefault(_ => _.FileInfo == file);
			import.DoesImport = doesImport;

			if (doesImport)
			{
				// Enable dependency files
				if (file.Dependencies != null)
				{
					foreach (var fileDep in file.Dependencies)
					{
						SetFileImportSettings(fileDep, true);
					}
				}
			}
			else
			{
				// Disable when nowhere dependent
				foreach (var fileEfc in efkpkg.AllFiles)
				{
					if (importedFiles.FirstOrDefault(_ => _.FileInfo == fileEfc).DoesImport &&
						fileEfc.Dependencies != null &&
						fileEfc.Dependencies.Contains(file))
					{
						import.DoesImport = true;
					}
				}

				// Disable dependency files
				if (file.Dependencies != null)
				{
					foreach (var fileDep in file.Dependencies)
					{
						SetFileImportSettings(fileDep, false);
					}
				}
			}
		}

		public void RemoveDirectoryInfos()
		{
			foreach (var imported in importedFiles)
			{
				imported.DestinationRelativePath = Path.GetFileName(imported.DestinationRelativePath);
			}
		}

		public void RenameFiles()
		{
			var pathToHashes = new Dictionary<string, string>();

			foreach (var imported in importedFiles)
			{
				var path = imported.GetDestinationPath(destinationPath, resourceRoots, resourceDestinationType);
				var dirPath = Path.GetDirectoryName(path);
				var files = Directory.GetFiles(dirPath);

				foreach (var f in files)
				{
					if (pathToHashes.ContainsKey(f))
					{
						continue;
					}

					var hash = EfkPkg.ComputeHashName(File.ReadAllBytes(f));
					pathToHashes.Add(f, hash);
				}
			}

			foreach (var imported in importedFiles)
			{
				var path = imported.GetDestinationPath(destinationPath, resourceRoots, resourceDestinationType);
				var dirPath = Path.GetDirectoryName(path);

				var relatedPathToHashes = pathToHashes.Where(_ => Path.GetDirectoryName(_.Key) == dirPath);

				if (relatedPathToHashes.Any(_ => _.Value == imported.FileInfo.HashName))
				{
					var p = relatedPathToHashes.First(_ => _.Value == imported.FileInfo.HashName);
					imported.DestinationRelativePath = Misc.BackSlashToSlash(Path.Combine(dirPath, Path.GetFileName(p.Key)));
				}
				else if (relatedPathToHashes.Any(_ => _.Key == path))
				{
					while (relatedPathToHashes.Any(_ => _.Key == path))
					{
						var dn = Path.GetDirectoryName(imported.DestinationRelativePath);
						var fn = Path.GetFileNameWithoutExtension(imported.DestinationRelativePath);
						fn += "_" + Path.GetExtension(imported.DestinationRelativePath);
						imported.DestinationRelativePath = Misc.BackSlashToSlash(Path.Combine(dn, fn));

						path = imported.GetDestinationPath(destinationPath, resourceRoots, resourceDestinationType);
					}
				}
			}
		}
	}
}