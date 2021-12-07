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

		public bool ShouldBeRemoved { get; private set; }

		public string GetStateIcon(EfkPkgImporter.ImportFile.PathState state)
		{
			switch (state)
			{
				case EfkPkgImporter.ImportFile.PathState.Writable: return Icons.CommonCheck;
				case EfkPkgImporter.ImportFile.PathState.Overwrite: return Icons.CommonWarning;
				case EfkPkgImporter.ImportFile.PathState.PathError: return Icons.CommonError;
			}
			return string.Empty;
		}

		EfkPkgImporter importer;

		public ImportEfkPkg()
		{
			ShouldBeRemoved = false;
		}


		public void Show(string path, EfkPkg efkpkg)
		{
			title = MultiLanguageTextProvider.GetText("ImportEfkPkgTitle");
			sourceFilePath = Utils.Misc.BackSlashToSlash(path);

			// FileViewer Path or Current Effect Path or Current Directory
			string targetDirPath = null;
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

			try
			{
				importer = new EfkPkgImporter(efkpkg, targetDirPath);
			}
			catch (Exception e)
			{
				var mb = new MessageBox();
				mb.Show("Error", e.Message);
				ShouldBeRemoved = true;
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
					Manager.NativeManager.InputText("###SourceFilePath", sourceFilePath, swig.InputTextFlags.ReadOnly);
					Manager.NativeManager.PopItemWidth();
				}

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
							importer.DestinationPath = directory;
							importer.RenewIOStatus();
						}
					}

					string icon = (importer.IsDestinationPathValid) ? Icons.CommonCheck : Icons.CommonError;
					if (Manager.NativeManager.InputText(icon + "###TargetDirPath", importer.DestinationPath))
					{
						importer.DestinationPath = Manager.NativeManager.GetInputTextResult();
						importer.RenewIOStatus();
					}

					Manager.NativeManager.PopItemWidth();
				}

				Manager.NativeManager.Spacing();

				if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("ImportRemoveDirPath") + "###ImportRemoveDirPath"))
				{
					importer.RemoveDirectoryNames();
				}

				if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("ImportAvoidOverwrite") + "###ImportAvoidOverwrite"))
				{
					importer.RenameFileNamesToAvoidOverwrite();
				}

				if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("ImportMergeSameFiles") + "###ImportMergeSameFiles"))
				{
					importer.RenameFileNamesToMergeSameFiles();
				}

				Manager.NativeManager.Spacing();

				{
					var rootDir = new[] { importer.ResourceDestination == EfkPkgImporter.ResourceDestinationType.ResourceRootDirectory };

					if (Manager.NativeManager.Checkbox(MultiLanguageTextProvider.GetText("ImportUseRootDir") + "###UseRootDir", rootDir))
					{
						importer.ResourceDestination = rootDir[0] ? EfkPkgImporter.ResourceDestinationType.ResourceRootDirectory : EfkPkgImporter.ResourceDestinationType.RelativePath;
						importer.RenewIOStatus();
					}

					if (importer.ResourceDestination == EfkPkgImporter.ResourceDestinationType.ResourceRootDirectory)
					{
						foreach (var root in importer.ResourceRoots)
						{
							if (Manager.NativeManager.InputText(root.Type.ToString(), root.RootPath))
							{
								root.RootPath = Manager.NativeManager.GetInputTextResult();
								importer.RenewIOStatus();
							}
						}
					}
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
						try
						{
							var results = importer.Import();

							if (results != null && results.Length == 1)
							{
								Commands.Open(results[0]);
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
			foreach (var import in importer.ImportedFiles)
			{
				var file = import.FileInfo;
				bool[] doesImport = new bool[] { import.DoesImport };

				// source file
				if (Manager.NativeManager.Checkbox(file.RelativePath, doesImport))
				{
					importer.SetFileImportSettings(file, doesImport[0]);
				}
				Manager.NativeManager.NextColumn();

				// destination file
				if (import.DoesImport)
				{
					Manager.NativeManager.PushItemWidth(-Manager.NativeManager.GetFrameHeight());
					if (Manager.NativeManager.InputText(GetStateIcon(import.State) + "###" +
						file.HashName, import.DestinationRelativePath))
					{
						import.DestinationRelativePath = Manager.NativeManager.GetInputTextResult();
						importer.RenewIOStatus();
					}
					Manager.NativeManager.PopItemWidth();
				}

				Manager.NativeManager.NextColumn();
			}

			Manager.NativeManager.Columns(1);

			Manager.NativeManager.EndChildFrame();
		}
	}
}
