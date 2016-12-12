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
			Action<Func<bool>> regist = (f) =>
				{
					var attributes = f.Method.GetCustomAttributes(false);
					var uniquename = UniqueNameAttribute.GetUniqueName(attributes);
					if (uniquename != null)
					{
						Shortcuts.SetFunction(uniquename, f);

					}
				};

			regist(New);
			regist(Open);
			regist(Overwrite);
			regist(SaveAs);
			regist(Exit);

			regist(Play);
			regist(Stop);
			regist(Step);
			regist(BackStep);

			regist(Undo);
			regist(Redo);
			regist(Copy);
			regist(Paste);
			regist(PasteInfo);
			regist(AddNode);
			regist(InsertNode);
			regist(RemoveNode);
		}

		[Name(value = "新規", language = Language.Japanese)]
		[Name(value = "New Project", language = Language.English)]
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

		[Name(value = "開く", language = Language.Japanese)]
		[Name(value = "Open", language = Language.English)]
		[UniqueName(value = "Internal.Open")]
		public static bool Open()
		{
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
					GUIManager.AddRecentFile(fullPath);
				}
			}
			catch (Exception e)
			{
				MessageBox.Show(e.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}

			System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(fullPath));

			return true;
		}

		[Name(value = "上書き保存", language = Language.Japanese)]
		[Name(value = "Save", language = Language.English)]
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

		[Name(value = "名前をつけて保存", language = Language.Japanese)]
		[Name(value = "Save as...", language = Language.English)]
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
				GUIManager.AddRecentFile(filepath);

				System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));

				if (GUIManager.Network.SendOnSave)
				{
					GUIManager.Network.Send();
				}
			}

			return true;
		}

		[Name(value = "終了", language = Language.Japanese)]
		[Name(value = "Exit", language = Language.English)]
		[UniqueName(value = "Internal.Exit")]
		public static bool Exit()
		{
			GUIManager.Close();
			return true;
		}

		[Name(value = "再生・一時停止", language = Language.Japanese)]
		[Name(value = "Pause/Resume", language = Language.English)]
		[UniqueName(value = "Internal.PlayViewer")]
		public static bool Play()
		{
			if (GUIManager.DockViewer.IsPlaying && !GUIManager.DockViewer.IsPaused)
			{
				GUIManager.DockViewer.PauseAndResumeViewer();
			}
			else
			{
				if (GUIManager.DockViewer.IsPaused)
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

		[Name(value = "停止", language = Language.Japanese)]
		[Name(value = "Stop", language = Language.English)]
		[UniqueName(value = "Internal.StopViewer")]
		public static bool Stop()
		{
			GUIManager.DockViewer.StopViewer();
			return true;
		}

		[Name(value = "ステップ", language = Language.Japanese)]
		[Name(value = "Step", language = Language.English)]
		[UniqueName(value = "Internal.StepViewer")]
		public static bool Step()
		{
			GUIManager.DockViewer.StepViewer(false);
			return true;
		}

		[Name(value = "ステップ(後)", language = Language.Japanese)]
		[Name(value = "Step(Back)", language = Language.English)]
		[UniqueName(value = "Internal.BackStepViewer")]
		public static bool BackStep()
		{
			GUIManager.DockViewer.BackStepViewer();
			return true;
		}

		[Name(value="元に戻す", language= Language.Japanese)]
		[Name(value = "Undo", language = Language.English)]
		[UniqueName(value="Internal.Undo")]
		public static bool Undo()
		{
			Command.CommandManager.Undo();
			return true;
		}

		[Name(value = "やり直し", language = Language.Japanese)]
		[Name(value = "Redo", language = Language.English)]
		[UniqueName(value = "Internal.Redo")]
		public static bool Redo()
		{
			Command.CommandManager.Redo();
			return true;
		}

		[Name(value = "ノードのコピー", language = Language.Japanese)]
		[Name(value = "Copy Node", language = Language.English)]
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

		[Name(value = "ノードの貼り付け", language = Language.Japanese)]
		[Name(value = "Paste Node", language = Language.English)]
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

		[Name(value = "ノード情報の貼り付け", language = Language.Japanese)]
		[Name(value = "Paste Node Info", language = Language.English)]
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

		[Name(value = "ノードの追加", language = Language.Japanese)]
		[Name(value = "Add Node", language = Language.English)]
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

		[Name(value = "ノードの挿入", language = Language.Japanese)]
		[Name(value = "Insert Node", language = Language.English)]
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

		[Name(value = "ノードの削除", language = Language.Japanese)]
		[Name(value = "Delete Node", language = Language.English)]
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
		[Name(value = "ヘルプを表示", language = Language.Japanese)]
		[Name(value = "Manual", language = Language.English)]
		[UniqueName(value = "Internal.ViewHelp")]
		static public bool ViewHelp()
		{
			string rootDir = Path.GetDirectoryName(GUIManager.GetEntryDirectory());
			string helpPath = Path.Combine(rootDir, @"Help\index.html");
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
		[Name(value = "サンプルを開く", language = Language.Japanese)]
		[Name(value = "Open Sample Project", language = Language.English)]
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
		[Name(value = "Effekseerについて", language = Language.Japanese)]
		[Name(value = "About Effekseer", language = Language.English)]
		[UniqueName(value = "Internal.About")]
		static public bool About()
		{
			GUIManager.AboutForm.ShowDialog();
			return true;
		}
	}
}
