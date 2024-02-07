using System;
using System.Reflection;
using System.IO;

namespace Effekseer.GUI
{
	/// <summary>
	/// Commands called with shortcuts
	/// </summary>
	public class Commands
	{
		public static void Register()
		{
			void register(Func<bool> f)
			{
				var attributes = f.Method.GetCustomAttributes(false);
				var uniquename = UniqueNameAttribute.GetUniqueName(attributes);
				if (uniquename != null)
				{
					Shortcuts.SetFunction(uniquename, f);

				}
			};

			register(New);
			register(Open);
			register(Overwrite);
			register(SaveAs);
			register(Exit);

			register(Play);
			register(Stop);
			register(Step);
			register(BackStep);

			register(Undo);
			register(Redo);
			register(Copy);
			register(Paste);
			register(PasteInfo);
			register(AddNode);
			register(InsertNode);
			register(RemoveNode);
			register(RenameNode);
		}

		[Name(value = "InternalNew")]
		[UniqueName(value = "Internal.New")]
		public static bool New()
		{
			if (Core.IsChanged)
			{
				var dialog = new Dialog.SaveOnDisposing(
					() =>
					{
						Core.New();
					});

			}
			else
			{
				Core.New();
			}

			return true;
		}

		[Name(value = "InternalOpen")]
		[UniqueName(value = "Internal.Open")]
		public static bool Open()
		{
			var filter = MultiLanguageTextProvider.GetText("ProjectFilterNew");
			var result = swig.FileDialog.OpenDialog(filter, System.IO.Directory.GetCurrentDirectory());

			if (!string.IsNullOrEmpty(result))
			{
				Open(result);
			}

			return true;
		}

		/// <summary>
		/// Open a file
		/// </summary>
		/// <param name="fullPath">absolute path</param>
		public static bool Open(string fullPath)
		{
			if (!Utils.Misc.IsFullPath(fullPath))
			{
				throw new Exception(MultiLanguageTextProvider.GetText("NotAbsolutePathError"));
			}
			fullPath = Utils.Misc.BackSlashToSlash(fullPath);

			if (Core.IsChanged)
			{
				var dialog = new Dialog.SaveOnDisposing(
				() =>
				{
					try
					{
						if (Core.LoadFrom(fullPath))
						{
							RecentFiles.AddRecentFile(fullPath);
							System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(fullPath));
						}
						else
						{
							swig.GUIManager.show(string.Format(MultiLanguageTextProvider.GetText("Error_NotFound"), fullPath), "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
					}
					catch (Exception e)
					{
						HandleExceptionWhileOpenning(e);
					}
				});

				return true;
			}
			else
			{
				try
				{
					if (Core.LoadFrom(fullPath))
					{
						RecentFiles.AddRecentFile(fullPath);
						System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(fullPath));
					}
					else
					{
						swig.GUIManager.show(string.Format(MultiLanguageTextProvider.GetText("Error_NotFound"), fullPath), "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
					}
				}
				catch (Exception e)
				{
					HandleExceptionWhileOpenning(e);
				}

				return true;
			}
		}

		[Name(value = "InternalSaveBackup")]
		[UniqueName(value = "Internal.SaveBackup")]
		public static bool SaveBackup()
		{
			string fileNameWithoutExtensions = Path.GetFileNameWithoutExtension(Core.Root.GetFullPath());

			// we need some kind of identifier which is as unique per project as possible
			string identifier = !string.IsNullOrEmpty(fileNameWithoutExtensions) ? fileNameWithoutExtensions
				: Core.Root.GetHashCode().ToString();

			Core.SaveBackup(Path.GetTempPath() + "/efk_autosave_" + identifier + ".efkbac");
			return true;
		}

		[Name(value = "InternalRestoreLastSession")]
		[UniqueName(value = "Internal.RestoreLastSession")]
		public static bool RestoreLastSession()
		{
			string lastSessionFile = Path.GetTempPath() + "/efk_quit.efkbac";
			if (!File.Exists(lastSessionFile))
			{
				Core.OnOutputMessage?.Invoke(MultiLanguageTextProvider.GetText("Recover_LastSession_Error"));
				return false;
			}

			RunWithUnsavedWarning(() => Core.OpenBackup(lastSessionFile));
			return true;
		}

		[Name(value = "InternalRestoreAutoSave")]
		[UniqueName(value = "Internal.RestoreAutoSave")]
		public static bool RestoreLastAutoSave()
		{
			string path = swig.FileDialog.OpenDialog("efkbac", Path.GetTempPath());
			if (!string.IsNullOrEmpty(path))
			{
				RunWithUnsavedWarning(() => Core.OpenBackup(path));
				return true;
			}
			return false;
		}

		[Name(value = "InternalOverwrite")]
		[UniqueName(value = "Internal.Overwrite")]
		public static bool Overwrite()
		{
			if (!System.IO.File.Exists(Core.Root.GetFullPath()))
			{
				return SaveAs();
			}
			else
			{
				Core.SaveTo(Core.Root.GetFullPath());

				if (Manager.Network.SendOnSave)
				{
					Manager.Network.Send();
				}
			}

			return true;
		}

		[Name(value = "InternalSaveAs")]
		[UniqueName(value = "Internal.SaveAs")]
		public static bool SaveAs()
		{
			var filter = MultiLanguageTextProvider.GetText("EffekseerParticleFilter");

			var result = swig.FileDialog.SaveDialog(filter, System.IO.Directory.GetCurrentDirectory());

			if (!string.IsNullOrEmpty(result))
			{
				var filepath = result;

				if (System.IO.Path.GetExtension(filepath) != ".efkefc")
				{
					filepath += ".efkefc";
				}

				Core.SaveTo(filepath);
				RecentFiles.AddRecentFile(filepath);

				System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));

				if (Manager.Network.SendOnSave)
				{
					Manager.Network.Send();
				}

				return true;
			}

			return false;
		}

		[Name(value = "InternalExit")]
		[UniqueName(value = "Internal.Exit")]
		public static bool Exit()
		{
			Manager.NativeManager.Close();
			return true;
		}

		[Name(value = "InternalPauseOrResume")]
		[UniqueName(value = "Internal.PlayViewer")]
		public static bool Play()
		{
			if (Manager.Viewer.IsPlaying && !Manager.Viewer.IsPaused)
			{
				Manager.Viewer.PauseAndResumeViewer();
			}
			else
			{
				if (Manager.Viewer.IsPaused)
				{
					Manager.Viewer.PauseAndResumeViewer();
				}
				else
				{
					Manager.Viewer.PlayViewer();
				}
			}

			return true;
		}

		[Name(value = "InternalStop")]
		[UniqueName(value = "Internal.StopViewer")]
		public static bool Stop()
		{
			Manager.Viewer.StopViewer();
			return true;
		}

		[Name(value = "InternalStep")]
		[UniqueName(value = "Internal.StepViewer")]
		public static bool Step()
		{
			Manager.Viewer.StepViewer(1.0f, false);
			return true;
		}

		[Name(value = "InternalBaskStep")]
		[UniqueName(value = "Internal.BackStepViewer")]
		public static bool BackStep()
		{
			Manager.Viewer.BackStepViewer();
			return true;
		}

		[Name(value = "InternalUndo")]
		[UniqueName(value = "Internal.Undo")]
		public static bool Undo()
		{
			Command.CommandManager.Undo();
			return true;
		}

		[Name(value = "InternalRedo")]
		[UniqueName(value = "Internal.Redo")]
		public static bool Redo()
		{
			Command.CommandManager.Redo();
			return true;
		}

		[Name(value = "InternalCopy")]
		[UniqueName(value = "Internal.Copy")]
		public static bool Copy()
		{
			if (Core.SelectedNode != null)
			{
				var data = Core.Copy(Core.SelectedNode);
				Manager.NativeManager.SetClipboardText(data);
				return true;
			}

			return false;
		}

		[Name(value = "InternalPaste")]
		[UniqueName(value = "Internal.Paste")]
		public static bool Paste()
		{
			if (Core.SelectedNode != null)
			{
				var data = Manager.NativeManager.GetClipboardText();

				var selected = Core.SelectedNode;

				if (selected != null)
				{
					if (Core.IsValidXml(data))
					{
						Command.CommandManager.StartCollection();
						var node = selected.AddChild();
						Core.Paste(node, data);
						Command.CommandManager.EndCollection();


						if (Core.Root.GetDeepestLayerNumberInChildren() > Constant.NodeLayerLimit)
						{
							Command.CommandManager.Undo(true);
							ErrorUtils.ShowErrorByNodeLayerLimit();
						}
					}
				}

				return true;
			}

			return false;
		}

		[Name(value = "InternalPasteInfo")]
		[UniqueName(value = "Internal.PasteInfo")]
		public static bool PasteInfo()
		{
			if (Core.SelectedNode != null)
			{
				var data = Manager.NativeManager.GetClipboardText();

				if (Core.IsValidXml(data))
				{
					Core.Paste(Core.SelectedNode, data);
				}

				return true;
			}

			return false;
		}

		[Name(value = "InternalAddNode")]
		[UniqueName(value = "Internal.AddNode")]
		public static bool AddNode()
		{
			var selected = Core.SelectedNode;

			if (selected != null)
			{
				if (selected.GetLayerNumber() < Constant.NodeLayerLimit)
				{
					selected.AddChild();
				}
				else
				{
					ErrorUtils.ShowErrorByNodeLayerLimit();
				}
			}

			return true;
		}

		[Name(value = "InternalInsertNode")]
		[UniqueName(value = "Internal.InsertNode")]
		public static bool InsertNode()
		{
			var selected = Core.SelectedNode;

			if (selected != null && selected.Parent != null)
			{
				if (Core.Root.GetDeepestLayerNumberInChildren() < Constant.NodeLayerLimit)
				{
					selected.InsertParent();
				}
				else
				{
					ErrorUtils.ShowErrorByNodeLayerLimit();
				}
			}

			return true;
		}

		[Name(value = "InternalRemoveNode")]
		[UniqueName(value = "Internal.RemoveNode")]
		public static bool RemoveNode()
		{
			var selected = Core.SelectedNode;

			if (selected != null && selected.Parent != null)
			{
				selected.Parent.RemoveChild(selected as Data.Node);
			}

			return true;
		}

		/// <summary>
		/// Rename node with a dialog
		/// </summary>
		/// <returns></returns>
		[Name(value = "InternalRenameNode")]
		[UniqueName(value = "Internal.RenameNode")]
		public static bool RenameNode()
		{
			var selected = Core.SelectedNode;
			if (selected != null)
			{
				var renameNode = new GUI.Dialog.RenameNode();
				renameNode.Show(selected);
				return true;
			}
			return false;
		}

		/// <summary>
		/// show a help
		/// </summary>
		/// <returns></returns>
		[Name(value = "InternalViewHelp")]
		[UniqueName(value = "Internal.ViewHelp")]
		static public bool ViewHelp()
		{
			ShowURL(Core.GetToolHelpURL());
			return true;
		}

		/*
		/// <summary>
		/// Open sample
		/// </summary>
		/// <returns></returns>
		[Name(value = "InternalOpenSample")]
		[UniqueName(value = "Internal.OpenSample")]
		static public bool OpenSample()
		{
			string rootDir = Path.GetDirectoryName(Manager.GetEntryDirectory());

			var filter = MultiLanguageTextProvider.GetText("ProjectFilterNew");

			var result = swig.FileDialog.OpenDialog(filter, Path.Combine(rootDir, @"Sample"));

            if (!string.IsNullOrEmpty(result))
            {
                Open(result);
            }

			return true;
		}
		*/

		/// <summary>
		/// About effekseer
		/// </summary>
		/// <returns></returns>
		[Name(value = "InternalAbout")]
		[UniqueName(value = "Internal.About")]
		static public bool About()
		{
			var messageBox = new GUI.Dialog.About();
			messageBox.Show();
			return true;
		}

		[Name(value = "InternalImportPackage")]
		[UniqueName(value = "Internal.ImportPackage")]
		static public void ImportPackage(string packagePath, string targetDirPath)
		{
			if (string.IsNullOrEmpty(packagePath))
			{
				packagePath = swig.FileDialog.OpenDialog("efkpkg", string.Empty);
			}
			if (string.IsNullOrEmpty(packagePath))
			{
				return;
			}

			var efkpkg = new IO.EfkPkg();
			if (!efkpkg.Import(packagePath))
			{
				return;
			}

			var dialog = new GUI.Dialog.ImportEfkPkg();
			dialog.Show(packagePath, efkpkg);
		}

		[Name(value = "InternalExportPackage")]
		[UniqueName(value = "Internal.ExportPackage")]
		static public void ExportPackage(string packagePath, string[] exportFilePaths)
		{
			if (string.IsNullOrEmpty(packagePath))
			{
				packagePath = swig.FileDialog.SaveDialog("efkpkg", string.Empty);
			}
			if (string.IsNullOrEmpty(packagePath))
			{
				return;
			}

			var efkpkg = new IO.EfkPkg();
			foreach (string path in exportFilePaths)
			{
				efkpkg.AddEffect(path);
			}
			efkpkg.Export(packagePath);
		}

		private static void RunWithUnsavedWarning(Action command)
		{
			if (Core.IsChanged)
			{
				new Dialog.SaveOnDisposing(command);
			}
			else
			{
				command();
			}
		}

		static public void HandleExceptionWhileOpenning(Exception e)
		{
			if (e is TargetInvocationException)
			{
				HandleExceptionWhileOpenning(e.InnerException);
				return;
			}

			swig.GUIManager.show(e.Message, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
			Core.New();
		}

		static public bool ShowURL(string url)
		{
			try
			{
				var info = new System.Diagnostics.ProcessStartInfo
				{
					FileName = url,
					UseShellExecute = true
				};
				System.Diagnostics.Process.Start(info);
			}
			catch
			{
				swig.GUIManager.show(MultiLanguageTextProvider.GetText("Error_FailedToOpenHelp"), "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
			}

			return true;
		}
	}
}