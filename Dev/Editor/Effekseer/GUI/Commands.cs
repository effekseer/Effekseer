using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Reflection;
using System.IO;

namespace Effekseer.GUI
{
	/// <summary>
	/// ショートカット等から呼び出される命令群
	/// </summary>
	class Commands
	{
		public static void Regist()
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

        [Name(value = "InternalNew")]  // 新規
		[UniqueName(value = "Internal.New")]
		public static bool New()
		{
			if (SaveOnDisposing())
			{

			}
			else
			{
				return true;
			}

			Core.New();

			return true;
		}

        [Name(value = "InternalOpen")]  // 開く
		[UniqueName(value = "Internal.Open")]
		public static bool Open()
		{
            /*
            var filter = Properties.Resources.ProjectFilter;
            var filters = filter.Split('|');
            var result = swig.FileDialog.OpenDialog(filters[1], System.IO.Directory.GetCurrentDirectory());

            if(!string.IsNullOrEmpty(result))
            {
                Open(result);
            }
            */

			OpenFileDialog ofd = new OpenFileDialog();

			ofd.InitialDirectory = System.IO.Directory.GetCurrentDirectory();
            ofd.Filter = Properties.Resources.ProjectFilter;			
			ofd.FilterIndex = 2;
			ofd.Multiselect = false;

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				Open(ofd.FileName);
			}

            return true;
		}

		/// <summary>
		/// ファイルを開く
		/// </summary>
		/// <param name="fullPath">絶対パス</param>
		public static bool Open(string fullPath)
		{
			if (System.IO.Path.GetFullPath(fullPath) != fullPath) throw new Exception(Properties.Resources.NotAbsolutePathError);

			if (SaveOnDisposing())
			{

			}
			else
			{
				return true;
			}
			
			try
			{
				if (Core.LoadFrom(fullPath))
				{
					RecentFiles.AddRecentFile(fullPath);
				}
			}
			catch (Exception e)
			{
				MessageBox.Show(e.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}

			System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(fullPath));

			return true;
		}

        [Name(value = "InternalOverwrite")] // 上書き保存
		[UniqueName(value = "Internal.Overwrite")]
		public static bool Overwrite()
		{
			if (!System.IO.File.Exists(Core.FullPath))
			{
				SaveAs();
			}
			else
			{
				Core.SaveTo(Core.FullPath);

				if (GUIManager.Network.SendOnSave)
				{
					GUIManager.Network.Send();
				}
			}

			return true;
		}

        [Name(value = "InternalSaveAs")] // 名前をつけて保存
		[UniqueName(value = "Internal.SaveAs")]
		public static bool SaveAs()
		{
			SaveFileDialog ofd = new SaveFileDialog();

			ofd.InitialDirectory = System.IO.Directory.GetCurrentDirectory();
            ofd.Filter = Properties.Resources.ProjectFilter;
			ofd.FilterIndex = 2;
			ofd.OverwritePrompt = true;

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				var filepath = ofd.FileName;
				Core.SaveTo(filepath);
				RecentFiles.AddRecentFile(filepath);

				System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));

				if (GUIManager.Network.SendOnSave)
				{
					GUIManager.Network.Send();
				}
			}

			return true;
		}

        [Name(value = "InternalExit")] // 終了
		[UniqueName(value = "Internal.Exit")]
		public static bool Exit()
		{
			GUIManager.Close();
			return true;
		}

        [Name(value = "InternalPauseOrResume")] // 再生・一時停止
		[UniqueName(value = "Internal.PlayViewer")]
		public static bool Play()
		{
			if (GUIManager.DockViewer.Viewer.IsPlaying && !GUIManager.DockViewer.Viewer.IsPaused)
			{
				GUIManager.DockViewer.PauseAndResumeViewer();
			}
			else
			{
				if (GUIManager.DockViewer.Viewer.IsPaused)
				{
					GUIManager.DockViewer.PauseAndResumeViewer();
				}
				else
				{
					GUIManager.DockViewer.PlayViewer();
				}
			}

			return true;
		}

        [Name(value = "InternalStop")]  // 停止
		[UniqueName(value = "Internal.StopViewer")]
		public static bool Stop()
		{
			GUIManager.DockViewer.Viewer.StopViewer();
			return true;
		}

        [Name(value = "InternalStep")]  // ステップ
		[UniqueName(value = "Internal.StepViewer")]
		public static bool Step()
		{
			GUIManager.DockViewer.StepViewer(false);
			return true;
		}

        [Name(value = "InternalBaskStep")] // ステップ(後)
		[UniqueName(value = "Internal.BackStepViewer")]
		public static bool BackStep()
		{
			GUIManager.DockViewer.Viewer.BackStepViewer();
			return true;
		}

        [Name(value = "InternalUndo")]  // 元に戻す
		[UniqueName(value="Internal.Undo")]
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
			if (GUIManager.DockNodeTreeView == null) return false;
			if (!GUIManager.DockNodeTreeView.NodeTreeView.Focused) return false;

			if(Core.SelectedNode != null)
			{
				var data = Core.Copy(Core.SelectedNode);
				Clipboard.Clear();
				Clipboard.SetText(data);
				return true;
			}

			return false;
		}

        [Name(value = "InternalPaste")] // ノードの貼り付け
		[UniqueName(value = "Internal.Paste")]
		public static bool Paste()
		{
			if (GUIManager.DockNodeTreeView == null) return false;
			if (!GUIManager.DockNodeTreeView.NodeTreeView.Focused) return false;

			if (Core.SelectedNode != null)
			{
				var data = System.Windows.Forms.Clipboard.GetText();

				var selected = Core.SelectedNode;

				if (selected != null)
				{
					Command.CommandManager.StartCollection();
					var node = selected.AddChild();
					Core.Paste(node, data);
					Command.CommandManager.EndCollection();
				}

				return true;
			}

			return false;
		}

        [Name(value = "InternalPasteInfo")] // ノード情報の貼り付け
		[UniqueName(value = "Internal.PasteInfo")]
		public static bool PasteInfo()
		{
			if (GUIManager.DockNodeTreeView == null) return false;
			if (!GUIManager.DockNodeTreeView.NodeTreeView.Focused) return false;

			if (Core.SelectedNode != null)
			{
				var data = System.Windows.Forms.Clipboard.GetText();
				Core.Paste(Core.SelectedNode, data);
				return true;
			}

			return false;
		}

        [Name(value = "InternalAddNode")] // ノードの追加
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
		/// 現在のエフェクトを破棄する必要がある場合の保存
		/// </summary>
		/// <returns></returns>
		static public bool SaveOnDisposing()
		{
			if (Core.IsChanged)
			{
                var format = Properties.Resources.ConfirmSaveChanged;

				var result = MessageBox.Show(
					string.Format(format,System.IO.Path.GetFileName(Core.FullPath)),
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

                        ofd.Filter = Properties.Resources.ProjectFilter;
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
		
		/// <summary>
		/// ヘルプを表示
		/// </summary>
		/// <returns></returns>
        [Name(value = "InternalViewHelp")] // ヘルプを表示
		[UniqueName(value = "Internal.ViewHelp")]
		static public bool ViewHelp()
		{
			string rootDir = Path.GetDirectoryName(GUIManager.GetEntryDirectory());
			string helpPath = Path.Combine(rootDir, @"Help\index_en.html");

            if(Core.Language == Language.Japanese)
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
			string rootDir = Path.GetDirectoryName(GUIManager.GetEntryDirectory());

			OpenFileDialog ofd = new OpenFileDialog();

			ofd.InitialDirectory = Path.Combine(rootDir, @"Sample");
            ofd.Filter = Properties.Resources.ProjectFilter;
			ofd.FilterIndex = 2;
			ofd.Multiselect = false;

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				Open(ofd.FileName);
			}

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
			GUIManager.AboutForm.ShowDialog();
			return true;
		}
	}
}
