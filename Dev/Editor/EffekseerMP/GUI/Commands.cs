using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
//using System.Windows.Forms;
using System.Diagnostics;
using System.Reflection;
using System.IO;

namespace Effekseer.GUI
{
	/// <summary>
	/// Commands called with shortcuts
	/// </summary>
	class Commands
	{
		public static void Register()
		{
			Action<Func<bool>> register = (f) =>
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
		}

		[Name(value = "InternalNew")]
		[UniqueName(value = "Internal.New")]
		public static bool New()
		{
			if(Core.IsChanged)
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

			var filter = Resources.GetString("ProjectFilter");
            var result = swig.FileDialog.OpenDialog(filter, System.IO.Directory.GetCurrentDirectory());

            if(!string.IsNullOrEmpty(result))
            {
                Open(result);
            }
            

			/*
			OpenFileDialog ofd = new OpenFileDialog();

			ofd.InitialDirectory = System.IO.Directory.GetCurrentDirectory();
			ofd.Filter = Resources.GetString("ProjectFilter");
			ofd.FilterIndex = 2;
			ofd.Multiselect = false;

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				Open(ofd.FileName);
			}
			*/

			return true;
		}

		/// <summary>
		/// Open a file
		/// </summary>
		/// <param name="fullPath">absolute path</param>
		public static bool Open(string fullPath)
		{
			if (System.IO.Path.GetFullPath(fullPath) != fullPath) throw new Exception(Resources.GetString("NotAbsolutePathError"));

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
							if (Core.Language == Language.Japanese)
							{
								swig.GUIManager.show(fullPath + "が見つかりません。", "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
							}
							else
							{
								swig.GUIManager.show(fullPath + " is not found.", "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
							}
						}
					}
					catch (Exception e)
					{
						swig.GUIManager.show(e.Message, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
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
						if (Core.Language == Language.Japanese)
						{
							swig.GUIManager.show(fullPath + "が見つかりません。", "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
						else
						{
							swig.GUIManager.show(fullPath + " is not found.", "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}
					}
				}
				catch (Exception e)
				{
					swig.GUIManager.show(e.Message, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
				}

				return true;
			}
		}

		[Name(value = "InternalOverwrite")]
		[UniqueName(value = "Internal.Overwrite")]
		public static bool Overwrite()
		{
			if (!System.IO.File.Exists(Core.FullPath))
			{
				return SaveAs();
			}
			else
			{
				Core.SaveTo(Core.FullPath);

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
			var filter = Resources.GetString("ProjectFilter");
			var result = swig.FileDialog.SaveDialog(filter, System.IO.Directory.GetCurrentDirectory());

			if (!string.IsNullOrEmpty(result))
			{
				var filepath = result;

				if(System.IO.Path.GetExtension(filepath) != ".efkproj")
				{
					filepath += ".efkproj";
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

			/*
			SaveFileDialog ofd = new SaveFileDialog();

			ofd.InitialDirectory = System.IO.Directory.GetCurrentDirectory();
			ofd.Filter = Resources.GetString("ProjectFilter");
			ofd.FilterIndex = 2;
			ofd.OverwritePrompt = true;

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				var filepath = ofd.FileName;
				Core.SaveTo(filepath);
				RecentFiles.AddRecentFile(filepath);

				System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));

				if (Manager.Network.SendOnSave)
				{
					Manager.Network.Send();
				}
			}
			*/

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

		[Name(value = "InternalStop")]  // 停止
		[UniqueName(value = "Internal.StopViewer")]
		public static bool Stop()
		{
			Manager.Viewer.StopViewer();
			return true;
		}

		[Name(value = "InternalStep")]  // ステップ
		[UniqueName(value = "Internal.StepViewer")]
		public static bool Step()
		{
			Manager.Viewer.StepViewer(false);
			return true;
		}

		[Name(value = "InternalBaskStep")] // ステップ(後)
		[UniqueName(value = "Internal.BackStepViewer")]
		public static bool BackStep()
		{
			Manager.Viewer.BackStepViewer();
			return true;
		}

		[Name(value = "InternalUndo")]  // 元に戻す
		[UniqueName(value = "Internal.Undo")]
		public static bool Undo()
		{
			Command.CommandManager.Undo();
			return true;
		}

		[Name(value = "InternalRedo")]  // やり直し
		[UniqueName(value = "Internal.Redo")]
		public static bool Redo()
		{
			Command.CommandManager.Redo();
			return true;
		}

		[Name(value = "InternalCopy")] // ノードのコピー
		[UniqueName(value = "Internal.Copy")]
		public static bool Copy()
		{
			//if (Manager.NodeTreeView == null) return false;
			//if (!Manager.NodeTreeView.NodeTreeView.Focused) return false;

			if (Core.SelectedNode != null)
			{
				var data = Core.Copy(Core.SelectedNode);
				Manager.NativeManager.SetClipboardText(data);
				return true;
			}

			return false;
		}

		[Name(value = "InternalPaste")] // ノードの貼り付け
		[UniqueName(value = "Internal.Paste")]
		public static bool Paste()
		{
			//if (Manager.NodeTreeView == null) return false;
			//if (!Manager.NodeTreeView.NodeTreeView.Focused) return false;

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
			//if (Manager.NodeTreeView == null) return false;
			//if (!Manager.NodeTreeView.NodeTreeView.Focused) return false;

			if (Core.SelectedNode != null)
			{
				var data = Manager.NativeManager.GetClipboardText();

				if(Core.IsValidXml(data))
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
				selected.AddChild();
			}

			return true;
		}

		[Name(value = "InternalInsertNode")] // ノードの挿入
		[UniqueName(value = "Internal.InsertNode")]
		public static bool InsertNode()
		{
			var selected = Core.SelectedNode;

			if (selected != null && selected.Parent != null)
			{
				selected.InsertParent();
			}

			return true;
		}

		[Name(value = "InternalRemoveNode")] // ノードの削除
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
		/// if to save an effect is required, show disposing dialog box
		/// </summary>
		/// <returns></returns>
        /*
		static public bool SaveOnDisposing()
		{
			if (Core.IsChanged)
			{
				var format = Resources.GetString("ConfirmSaveChanged");

				var result = MessageBox.Show(
					string.Format(format, System.IO.Path.GetFileName(Core.FullPath)),
					"Warning",
					MessageBoxButtons.YesNoCancel);

				if (result == System.Windows.Forms.DialogResult.Yes)
				{
					if (Core.FullPath != string.Empty)
					{
						Core.SaveTo(Core.FullPath);
					}
					else
					{
						SaveFileDialog ofd = new SaveFileDialog();

						ofd.InitialDirectory = System.IO.Directory.GetCurrentDirectory();

						ofd.Filter = Resources.GetString("ProjectFilter");
						ofd.FilterIndex = 2;
						ofd.OverwritePrompt = true;

						if (ofd.ShowDialog() == DialogResult.OK)
						{
							var filepath = ofd.FileName;
							Core.SaveTo(filepath);
						}
						else
						{
							return false;
						}
					}
				}
				else if (result == System.Windows.Forms.DialogResult.No)
				{
					return true;
				}
				else if (result == System.Windows.Forms.DialogResult.Cancel)
				{
					return false;
				}
			}

			return true;
		}
        */

		/// <summary>
		/// ヘルプを表示
		/// </summary>
		/// <returns></returns>
		[Name(value = "InternalViewHelp")] // ヘルプを表示
		[UniqueName(value = "Internal.ViewHelp")]
		static public bool ViewHelp()
		{
			string rootDir = Path.GetDirectoryName(Manager.GetEntryDirectory());
			string helpPath = Path.Combine(rootDir, @"Help\index_en.html");

			if (Core.Language == Language.Japanese)
			{
				helpPath = Path.Combine(rootDir, @"Help\index_ja.html");
			}

			if (File.Exists(helpPath))
			{
				Process.Start(helpPath);
			}

			return true;
		}

		/// <summary>
		/// サンプルを開く
		/// </summary>
		/// <returns></returns>
		[Name(value = "InternalOpenSample")] // サンプルを開く
		[UniqueName(value = "Internal.OpenSample")]
		static public bool OpenSample()
		{
			string rootDir = Path.GetDirectoryName(Manager.GetEntryDirectory());

			var filter = Resources.GetString("ProjectFilter");
			var result = swig.FileDialog.OpenDialog(filter, Path.Combine(rootDir, @"Sample"));

            if (!string.IsNullOrEmpty(result))
            {
                Open(result);
            }

            /*
			OpenFileDialog ofd = new OpenFileDialog();

			ofd.InitialDirectory = Path.Combine(rootDir, @"Sample");
			ofd.Filter = Resources.GetString("ProjectFilter");
			ofd.FilterIndex = 2;
			ofd.Multiselect = false;

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				Open(ofd.FileName);
			}
            */

			return true;
		}

		/// <summary>
		/// Effekseerについて
		/// </summary>
		/// <returns></returns>
		[Name(value = "InternalAbout")] // Effekseerについて
		[UniqueName(value = "Internal.About")]
		static public bool About()
		{
			var messageBox = new GUI.Dialog.About();
			messageBox.Show();
			return true;
		}
	}
}
