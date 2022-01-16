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
			public FileType Type { get; private set; }
			public string RootPath;

			internal ResourceRoot(FileType type)
			{
				Type = type;
			}
		}

		public class ImportFile
		{
			public enum PathState
			{
				Writable,
				Overwrite,
				PathError,
			}

			public EfkPkg.FileInfo FileInfo { get; private set; }
			public bool DoesImport;
			public PathState State { get; private set; }
			public string DestinationRelativePath;

			EfkPkgImporter Importer;

			internal ImportFile(EfkPkgImporter importer, EfkPkg.FileInfo fileInfo)
			{
				Importer = importer;
				FileInfo = fileInfo;
			}

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
				if (!Directory.Exists(Importer.DestinationPath))
				{
					State = PathState.PathError;
					return;
				}

				string fullPath = GetDestinationPath(Importer.DestinationPath, Importer.resourceRoots, Importer.resourceDestinationType);

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

		public string DestinationPath { get; set; }
		EfkPkg efkpkg;
		List<ImportFile> importedFiles = new List<ImportFile>();
		ResourceDestinationType resourceDestinationType = ResourceDestinationType.RelativePath;
		List<ResourceRoot> resourceRoots = new List<ResourceRoot>();

		public bool IsDestinationPathValid { get; private set; }


		public IReadOnlyCollection<ImportFile> ImportedFiles
		{
			get
			{
				return importedFiles;
			}
		}

		public IReadOnlyCollection<ResourceRoot> ResourceRoots
		{
			get
			{
				return resourceRoots;
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
			resourceRoots.Add(new ResourceRoot(FileType.Effect) { RootPath = "Effects/" });
			resourceRoots.Add(new ResourceRoot(FileType.Texture) { RootPath = "Textures/" });
			resourceRoots.Add(new ResourceRoot(FileType.Model) { RootPath = "Models/" });
			resourceRoots.Add(new ResourceRoot(FileType.Material) { RootPath = "Materials/" });
			resourceRoots.Add(new ResourceRoot(FileType.Sound) { RootPath = "Sounds/" });
			resourceRoots.Add(new ResourceRoot(FileType.Curve) { RootPath = "Curves/" });

			DestinationPath = defaultDestinationPath;
			this.efkpkg = efkpkg;

			if (DestinationPath == null)
			{
				DestinationPath = Directory.GetCurrentDirectory();
			}

			DestinationPath = Misc.BackSlashToSlash(DestinationPath);

			foreach (var file in efkpkg.AllFiles)
			{
				var import = new ImportFile(this, file);
				import.DestinationRelativePath = file.RelativePath;
				import.DoesImport = true;
				importedFiles.Add(import);
			}

			RenewIOStatus();
		}

		public void RenewIOStatus()
		{
			IsDestinationPathValid = Directory.Exists(DestinationPath);

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
				file.FileInfo.RelativePath = file.GetDestinationPath(string.Empty, resourceRoots, resourceDestinationType);
			}

			efkpkg.ExtractFiles(DestinationPath, files.Select(_ => _.FileInfo).ToArray());

			var effects = files.Where(_ => _.FileInfo.Type == FileType.Effect);
			return effects.Select(_ => Misc.BackSlashToSlash(Path.Combine(DestinationPath, _.FileInfo.RelativePath))).ToArray();
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

		public void RemoveDirectoryNames()
		{
			foreach (var imported in importedFiles)
			{
				imported.DestinationRelativePath = Path.GetFileName(imported.DestinationRelativePath);
			}
		}

		public void RenameFileNamesToAvoidOverwrite()
		{
			var pathToHashes = GetRelatedFilePathToHashes();

			foreach (var imported in importedFiles)
			{
				var path = imported.GetDestinationPath(DestinationPath, resourceRoots, resourceDestinationType);
				var dirPath = Path.GetDirectoryName(path);

				var relatedPathToHashes = pathToHashes.Where(_ => Path.GetDirectoryName(_.Key) == dirPath);

				if (relatedPathToHashes.Any(_ => _.Key == path && _.Value != imported.FileInfo.HashName))
				{
					while (relatedPathToHashes.Any(_ => _.Key == path))
					{
						var dn = Path.GetDirectoryName(imported.DestinationRelativePath);
						var fn = Path.GetFileNameWithoutExtension(imported.DestinationRelativePath);
						fn += "_" + Path.GetExtension(imported.DestinationRelativePath);
						imported.DestinationRelativePath = Misc.BackSlashToSlash(Path.Combine(dn, fn));

						path = imported.GetDestinationPath(DestinationPath, resourceRoots, resourceDestinationType);
					}
				}
			}
		}

		public void RenameFileNamesToMergeSameFiles()
		{
			var pathToHashes = GetRelatedFilePathToHashes();

			foreach (var imported in importedFiles)
			{
				var path = imported.GetDestinationPath(DestinationPath, resourceRoots, resourceDestinationType);
				var dirPath = Path.GetDirectoryName(path);

				var relatedPathToHashes = pathToHashes.Where(_ => Path.GetDirectoryName(_.Key) == dirPath);

				if (relatedPathToHashes.Any(_ => _.Value == imported.FileInfo.HashName))
				{
					var p = relatedPathToHashes.First(_ => _.Value == imported.FileInfo.HashName);
					var dn = Path.GetDirectoryName(imported.DestinationRelativePath);
					imported.DestinationRelativePath = Misc.BackSlashToSlash(Path.Combine(dn, Path.GetFileName(p.Key)));
				}
			}
		}

		Dictionary<string, string> GetRelatedFilePathToHashes()
		{
			var pathToHashes = new Dictionary<string, string>();

			foreach (var imported in importedFiles)
			{
				var path = imported.GetDestinationPath(DestinationPath, resourceRoots, resourceDestinationType);
				var dirPath = Path.GetDirectoryName(path);

				var files = new string[0];

				if (Directory.Exists(dirPath))
				{
					files = Directory.GetFiles(dirPath).Select(_ => Misc.BackSlashToSlash(_)).ToArray();
				}

				foreach (var f in files)
				{
					if (pathToHashes.ContainsKey(f))
					{
						continue;
					}

					try
					{
						var hash = EfkPkg.ComputeHashName(File.ReadAllBytes(f));
						pathToHashes.Add(f, hash);
					}
					catch
					{
					}
				}
			}

			return pathToHashes;
		}
	}
}