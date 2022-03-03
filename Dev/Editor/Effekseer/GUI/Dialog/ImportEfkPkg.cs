using Effekseer.GUI.Dock;
using Effekseer.Utils;
using Effekseer.IO;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dialog
{
	public class ImportEfkPkg : IRemovableControl
	{
		string title = string.Empty;
		string message = string.Empty;
		string id = "###import";

		bool opened = true;
		bool isFirstUpdate = true;

		string sourceFilePath = string.Empty;
		string targetDirPath = string.Empty;
		bool targetDirPathValid = false;

		class ImportFile
		{
			public enum PathState
			{
				Writable,
				Overwrite,
				PathError,
			}

			public bool DoesImport;
			public PathState State;
			public string DestinationName;

			public void ValidationPath(string targetDirPath)
			{
				if (!Directory.Exists(targetDirPath))
				{
					State = PathState.PathError;
					return;
				}

				string fullPath = Path.Combine(targetDirPath, DestinationName);
				fullPath = Misc.BackSlashToSlash(fullPath);
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

			public string GetStateIcon()
			{
				switch (State)
				{
					case PathState.Writable: return Icons.CommonCheck;
					case PathState.Overwrite: return Icons.CommonWarning;
					case PathState.PathError: return Icons.CommonError;
				}
				return string.Empty;
			}
		}
		Dictionary<EfkPkg.FileInfo, ImportFile> importFiles = new Dictionary<EfkPkg.FileInfo, ImportFile>();

		public EfkPkg EfkPkg { get; private set; }

		public bool ShouldBeRemoved { get; private set; }

		public ImportEfkPkg()
		{
			ShouldBeRemoved = false;
		}

		public void Show(string path, EfkPkg efkpkg)
		{
			title = MultiLanguageTextProvider.GetText("ImportEfkPkgTitle");
			EfkPkg = efkpkg;
			sourceFilePath = Utils.Misc.BackSlashToSlash(path);

			// FileViewer Path or Current Effect Path or Current Directory
			var fileViewer = (Dock.FileViewer)Manager.GetWindow(typeof(Dock.FileViewer));
			if (fileViewer != null && !string.IsNullOrEmpty(fileViewer.CurrentPath))
			{
				targetDirPath = fileViewer.CurrentPath;
			}
			else if (!string.IsNullOrEmpty(Core.Root.GetFullPath()))
			{
				targetDirPath = Path.GetDirectoryName(Core.Root.GetFullPath());
			}
			else
			{
				targetDirPath = Directory.GetCurrentDirectory();
			}
			targetDirPath = Utils.Misc.BackSlashToSlash(targetDirPath);
			targetDirPathValid = Directory.Exists(targetDirPath);

			foreach (var file in efkpkg.AllFiles)
			{
				var import = new ImportFile();
				import.DestinationName = file.RelativePath;
				import.DoesImport = true;
				import.ValidationPath(targetDirPath);
				importFiles.Add(file, import);
			}

			Manager.AddControl(this);
		}

		public void Update()
		{
			if (isFirstUpdate)
			{
				Manager.NativeManager.OpenPopup(id);
				isFirstUpdate = false;
			}

			float dpiScale = Manager.DpiScale;

			Manager.NativeManager.SetNextWindowSize(500 * dpiScale, 400 * dpiScale, swig.Cond.Once);

			if (Manager.NativeManager.BeginPopupModal(title + id, ref opened))
			{
				{
					var text = MultiLanguageTextProvider.GetText("ImportSourcePackage");
					Manager.NativeManager.PushItemWidth(-1);
					Manager.NativeManager.Text(text);
					Manager.NativeManager.InputText("###SSourceFilePath", sourceFilePath, swig.InputTextFlags.ReadOnly);
					Manager.NativeManager.PopItemWidth();
				}

				Manager.NativeManager.Spacing();
				Manager.NativeManager.Spacing();

				{
					var text = MultiLanguageTextProvider.GetText("ImportDestinationDirectory");
					Manager.NativeManager.PushItemWidth(-Manager.NativeManager.GetFrameHeight());
					Manager.NativeManager.Text(text);
					Manager.NativeManager.SameLine();

					Manager.NativeManager.SetCursorPosY(Manager.NativeManager.GetCursorPosY() - Manager.NativeManager.GetFrameHeight() + Manager.NativeManager.GetTextLineHeight());
					if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("Select")))
					{
						var directory = swig.FileDialog.PickFolder(Directory.GetCurrentDirectory());
						if (!string.IsNullOrEmpty(directory))
						{
							targetDirPath = directory;
							targetDirPathValid = Directory.Exists(targetDirPath);
						}
					}

					string icon = (targetDirPathValid) ? Icons.CommonCheck : Icons.CommonError;
					if (Manager.NativeManager.InputText(icon + "###TargetDirPath", targetDirPath))
					{
						targetDirPath = Manager.NativeManager.GetInputTextResult();
						targetDirPathValid = Directory.Exists(targetDirPath);
						foreach (var file in EfkPkg.AllFiles)
						{
							importFiles[file].ValidationPath(targetDirPath);
						}
					}
					Manager.NativeManager.PopItemWidth();
				}

				Manager.NativeManager.Spacing();

				var size = Manager.NativeManager.GetContentRegionAvail();
				UpdateFileList(new swig.Vec2(0, size.Y - 32 * dpiScale));

				Manager.NativeManager.Spacing();

				{
					var textImport = MultiLanguageTextProvider.GetText("Import");
					var textCancel = MultiLanguageTextProvider.GetText("Cancel");

					float buttonWidth = 100 * dpiScale;
					Manager.NativeManager.SetCursorPosX(Manager.NativeManager.GetContentRegionAvail().X / 2 - buttonWidth + 4 * dpiScale);

					if (Manager.NativeManager.Button(textImport, buttonWidth))
					{
						var files = importFiles
							.Where(kv => kv.Value.DoesImport)
							.Select(kv => kv.Key)
							.ToArray();

						foreach (var file in files)
						{
							file.RelativePath = importFiles[file].DestinationName;
						}

						try
						{
							EfkPkg.ExtractFiles(targetDirPath, files);

							var effects = files.Where(_ => _.Type == EfkPkg.FileType.Effect);
							if(effects.Count() == 1)
							{
								string filePath = Path.Combine(targetDirPath, effects.First().RelativePath);
								filePath = filePath.Replace("\\", "/");
								Commands.Open(filePath);
							}
						}
						catch (Exception e)
						{
							var mb = new MessageBox();
							mb.Show("Error", e.Message);
						}

						ShouldBeRemoved = true;
					}

					Manager.NativeManager.SameLine();

					if (Manager.NativeManager.Button(textCancel, buttonWidth))
					{
						ShouldBeRemoved = true;
					}
				}

				Manager.NativeManager.EndPopup();
			}
			else
			{
				ShouldBeRemoved = true;
			}
		}

		// Display file list
		private void UpdateFileList(swig.Vec2 size)
		{
			var textImportSourceFile = MultiLanguageTextProvider.GetText("ImportSourceFile");
			var textImportDestinationFile = MultiLanguageTextProvider.GetText("ImportDestinationFile");

			Manager.NativeManager.BeginChildFrame(1, size);

			Manager.NativeManager.Columns(2);

			// display file table header
			Manager.NativeManager.Text(textImportSourceFile);
			Manager.NativeManager.NextColumn();
			Manager.NativeManager.Text(textImportDestinationFile);
			Manager.NativeManager.NextColumn();

			Manager.NativeManager.Separator();

			// display file table body
			foreach (var file in EfkPkg.AllFiles)
			{
				var import = importFiles[file];
				bool[] doesImport = new bool[] { import.DoesImport };

				// source file
				if (Manager.NativeManager.Checkbox(file.RelativePath, doesImport))
				{
					SetFileImportSettings(file, doesImport[0]);
				}
				Manager.NativeManager.NextColumn();

				// destination file
				if (import.DoesImport)
				{
					Manager.NativeManager.PushItemWidth(-Manager.NativeManager.GetFrameHeight());
					if (Manager.NativeManager.InputText(import.GetStateIcon() + "###" +
						file.HashName, import.DestinationName))
					{
						import.DestinationName = Manager.NativeManager.GetInputTextResult();
						import.ValidationPath(targetDirPath);
					}
					Manager.NativeManager.PopItemWidth();
				}
				Manager.NativeManager.NextColumn();
			}

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChildFrame();
		}

		// Set file import settings
		void SetFileImportSettings(EfkPkg.FileInfo file, bool doesImport)
		{
			var import = importFiles[file];
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
				foreach (var fileEfc in EfkPkg.AllFiles)
				{
					if (importFiles[fileEfc].DoesImport && 
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
	}
}
