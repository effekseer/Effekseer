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
			Action<Action> regist = (f) =>
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
		public static void New()
		{
			if (SaveOnDisposing())
			{

			}
			else
			{
				return;
			}

			Core.New();
		}

		[Name(value = "開く", language = Language.Japanese)]
		[Name(value = "Open", language = Language.English)]
		[UniqueName(value = "Internal.Open")]
		public static void Open()
		{
			OpenFileDialog ofd = new OpenFileDialog();

			ofd.InitialDirectory = System.IO.Directory.GetCurrentDirectory();
			ofd.Filter = "エフェクトプロジェクト (*.efkproj)|*.efkproj";
			ofd.FilterIndex = 2;
			ofd.Multiselect = false;

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				Open(ofd.FileName);
			}
			else
			{
				return;
			}
		}

		/// <summary>
		/// ファイルを開く
		/// </summary>
		/// <param name="fullPath">絶対パス</param>
		public static void Open(string fullPath)
		{
			var errorText = string.Empty;
			if(Core.Language == Language.Japanese)
			{
				errorText = "絶対パスでありません。";
			}
			else if (Core.Language == Language.English)
			{
				errorText = "This is not an absolute path.";
			}

			if (System.IO.Path.GetFullPath(fullPath) != fullPath) throw new Exception(errorText);

			if (SaveOnDisposing())
			{

			}
			else
			{
				return;
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
		}

		[Name(value = "上書き保存", language = Language.Japanese)]
		[Name(value = "Save", language = Language.English)]
		[UniqueName(value = "Internal.Overwrite")]
		public static void Overwrite()
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
		}

		[Name(value = "名前をつけて保存", language = Language.Japanese)]
		[Name(value = "Save as...", language = Language.English)]
		[UniqueName(value = "Internal.SaveAs")]
		public static void SaveAs()
		{
			SaveFileDialog ofd = new SaveFileDialog();

			ofd.InitialDirectory = System.IO.Directory.GetCurrentDirectory();
			ofd.Filter = "エフェクトプロジェクト (*.efkproj)|*.efkproj";
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
			else
			{
				return;
			}

			
		}

		[Name(value = "終了", language = Language.Japanese)]
		[Name(value = "Exit", language = Language.English)]
		[UniqueName(value = "Internal.Exit")]
		public static void Exit()
		{
			GUIManager.Close();
		}

		[Name(value = "再生・一時停止", language = Language.Japanese)]
		[Name(value = "Pause/Resume", language = Language.English)]
		[UniqueName(value = "Internal.PlayViewer")]
		public static void Play()
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
		}

		[Name(value = "停止", language = Language.Japanese)]
		[Name(value = "Stop", language = Language.English)]
		[UniqueName(value = "Internal.StopViewer")]
		public static void Stop()
		{
			GUIManager.DockViewer.StopViewer();
		}

		[Name(value = "ステップ", language = Language.Japanese)]
		[Name(value = "Step", language = Language.English)]
		[UniqueName(value = "Internal.StepViewer")]
		public static void Step()
		{
			GUIManager.DockViewer.StepViewer(false);
		}

		[Name(value = "ステップ(後)", language = Language.Japanese)]
		[Name(value = "Step(Back)", language = Language.English)]
		[UniqueName(value = "Internal.BackStepViewer")]
		public static void BackStep()
		{
			GUIManager.DockViewer.BackStepViewer();
		}

		[Name(value="元に戻す", language= Language.Japanese)]
		[Name(value = "Undo", language = Language.English)]
		[UniqueName(value="Internal.Undo")]
		public static void Undo()
		{
			Command.CommandManager.Undo();
		}

		[Name(value = "やり直し", language = Language.Japanese)]
		[Name(value = "Redo", language = Language.English)]
		[UniqueName(value = "Internal.Redo")]
		public static void Redo()
		{
			Command.CommandManager.Redo();
		}

		[Name(value = "ノードのコピー", language = Language.Japanese)]
		[Name(value = "Copy Node", language = Language.English)]
		[UniqueName(value = "Internal.Copy")]
		public static void Copy()
		{
			if(Core.SelectedNode != null)
			{
				var data = Core.Copy(Core.SelectedNode);
				Clipboard.Clear();
				Clipboard.SetText(data);
			}
		}

		[Name(value = "ノードの貼り付け", language = Language.Japanese)]
		[Name(value = "Paste Node", language = Language.English)]
		[UniqueName(value = "Internal.Paste")]
		public static void Paste()
		{
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
			}
		}

		[Name(value = "ノード情報の貼り付け", language = Language.Japanese)]
		[Name(value = "Paste Node Info", language = Language.English)]
		[UniqueName(value = "Internal.PasteInfo")]
		public static void PasteInfo()
		{
			if (Core.SelectedNode != null)
			{
				var data = System.Windows.Forms.Clipboard.GetText();
				Core.Paste(Core.SelectedNode, data);
			}
		}

		[Name(value = "ノードの追加", language = Language.Japanese)]
		[Name(value = "Add Node", language = Language.English)]
		[UniqueName(value = "Internal.AddNode")]
		public static void AddNode()
		{
			var selected = Core.SelectedNode;

			if (selected != null)
			{
				selected.AddChild();
			}
		}

		[Name(value = "ノードの挿入", language = Language.Japanese)]
		[Name(value = "Insert Node", language = Language.English)]
		[UniqueName(value = "Internal.InsertNode")]
		public static void InsertNode()
		{
			var selected = Core.SelectedNode;

			if (selected != null && selected.Parent != null)
			{
				selected.InsertParent();
			}
		}

		[Name(value = "ノードの削除", language = Language.Japanese)]
		[Name(value = "Delete Node", language = Language.English)]
		[UniqueName(value = "Internal.RemoveNode")]
		public static void RemoveNode()
		{
			var selected = Core.SelectedNode;

			if (selected != null && selected.Parent != null)
			{
				selected.Parent.RemoveChild(selected as Data.Node);
			}
		}

		/// <summary>
		/// 現在のエフェクトを破棄する必要がある場合の保存
		/// </summary>
		/// <returns></returns>
		static public bool SaveOnDisposing()
		{
			if (Core.IsChanged)
			{
				var format = "[{0}] は変更されています。保存しますか？";
				if(Core.Language == Language.English)
				{
					format = "[{0}] has been changed. Do you want to save?";
				}

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

						string filter = string.Empty;
						if (Core.Language == Language.Japanese)
						{
							filter = "エフェクトプロジェクト (*.efkproj)|*.efkproj";
						}
						else if (Core.Language == Language.English)
						{
							filter = "Effekseer Project (*.efkproj)|*.efkproj";
						}

						ofd.Filter = filter;
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
		static public void ViewHelp()
		{
			string rootDir = Path.GetDirectoryName(GUIManager.GetEntryDirectory());
			string helpPath = Path.Combine(rootDir, @"Help\index.html");
			if (File.Exists(helpPath))
			{
				Process.Start(helpPath);
			}
		}
		
		/// <summary>
		/// サンプルを開く
		/// </summary>
		/// <returns></returns>
		[Name(value = "サンプルを開く", language = Language.Japanese)]
		[Name(value = "Open Sample Project", language = Language.English)]
		[UniqueName(value = "Internal.OpenSample")]
		static public void OpenSample()
		{
			string rootDir = Path.GetDirectoryName(GUIManager.GetEntryDirectory());

			OpenFileDialog ofd = new OpenFileDialog();

			ofd.InitialDirectory = Path.Combine(rootDir, @"Sample");
			ofd.Filter = "エフェクトプロジェクト (*.efkproj)|*.efkproj";
			ofd.FilterIndex = 2;
			ofd.Multiselect = false;

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				Open(ofd.FileName);
			}
			else
			{
				return;
			}
		}

		/// <summary>
		/// Effekseerについて
		/// </summary>
		/// <returns></returns>
		[Name(value = "Effekseerについて", language = Language.Japanese)]
		[Name(value = "About Effekseer", language = Language.English)]
		[UniqueName(value = "Internal.About")]
		static public void About()
		{
			GUIManager.AboutForm.ShowDialog();
		}
	}
}
