using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Effekseer.Utl;
using WeifenLuo.WinFormsUI.Docking;

namespace Effekseer.GUI
{
	public class GUIManager
	{
        public const string ConfigGuiFileName = "config.gui.xml";
        public const string ConfigGuiPanelFileName = "config.gui.panel.xml";
        public const string ConfigRecentFileName = "config.recent.xml";
		public const string ConfigNetworkFileName = "config.network.xml";

		/// <summary>
		/// 最近使用したファイル
		/// </summary>
        static LinkedList<string> recentFiles = new LinkedList<string>();
        static string configGuiPath;
        static string configGuiPanelPath;
        static string configRecentPath;
		static string configNetworkPath;

		internal static Network Network { get; private set; }

		internal static MainForm MainForm
		{
			get;
			private set;
		}
		
		internal static AboutForm AboutForm
		{
			get;
			private set;
		}

		internal static DockViewer DockViewer
		{
			get;
			set;
		}

		internal static DockViewerController DockViewerController
		{
			get;
			set;
		}

		internal static DockViewPoint DockViewPoint
		{
			get;
			set;
		}

		internal static DockRecorder DockRecorder
		{
			get;
			set;
		}

		internal static DockNodeTreeView DockNodeTreeView
		{
			get;
			set;
		}

		internal static DockNodeCommonValues DockNodeCommonValues
		{
			get;
			set;
		}

		internal static DockNodeLocationValues DockNodeLocationValues
		{
			get;
			set;
		}

		internal static DockNodeRotationValues DockNodeRotationValues
		{
			get;
			set;
		}

		internal static DockNodeScaleValues DockNodeScalingValues
		{
			get;
			set;
		}

		internal static DockNodeLocationAbsValues DockNodeLocationAbsValues
		{
			get;
			set;
		}

		internal static DockNodeGenerationLocationValues DockNodeGenerationLocationValues
		{
			get;
			set;
		}

		internal static DockNodeRendererCommonValues DockNodeRendererCommonValues
		{
			get;
			set;
		}

		internal static DockNodeRendererValues DockNodeDrawingValues
		{
			get;
			set;
		}

        internal static DockNodeSoundValues DockNodeSoundValues
        {
            get;
            set;
        }

		internal static DockOption DockOption
		{
			get;
			set;
		}

		internal static DockEffectBehavior DockEffectBehavior
		{
			get;
			set;
		}

		internal static DockFCurves DockFCurves
		{
			get;
			set;
		}

		internal static DockNetwork DockNetwork
		{
			get;
			set;
		}

		/// <summary>
		/// 最近使用したファイルが変更
		/// </summary>
		public static event EventHandler OnChangeRecentFiles;

		public static void Initialize()
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);

            configGuiPath = System.IO.Path.Combine(GetEntryDirectory(), ConfigGuiFileName);
            configGuiPanelPath = System.IO.Path.Combine(GetEntryDirectory(), ConfigGuiPanelFileName);
            configRecentPath = System.IO.Path.Combine(GetEntryDirectory(), ConfigRecentFileName);
			configNetworkPath = System.IO.Path.Combine(GetEntryDirectory(), ConfigNetworkFileName);
            
			Shortcuts.LoadShortcuts();
			Commands.Regist();
			LoadRecentConfig();

			Network = new Network();

			MainForm = new MainForm();
			AboutForm = new AboutForm();
			DockViewer = new DockViewer();
			DockViewerController = new DockViewerController();
			DockViewPoint = new DockViewPoint();
			DockRecorder = new GUI.DockRecorder();
			DockNodeTreeView = new DockNodeTreeView();
			DockNodeCommonValues = new GUI.DockNodeCommonValues();
			DockNodeLocationValues = new GUI.DockNodeLocationValues();
			DockNodeRotationValues = new GUI.DockNodeRotationValues();
			DockNodeScalingValues = new GUI.DockNodeScaleValues();
			DockNodeLocationAbsValues = new GUI.DockNodeLocationAbsValues();
			DockNodeGenerationLocationValues = new DockNodeGenerationLocationValues();
			DockNodeRendererCommonValues = new GUI.DockNodeRendererCommonValues();
			DockNodeDrawingValues = new GUI.DockNodeRendererValues();
            DockNodeSoundValues = new GUI.DockNodeSoundValues();
			DockOption = new GUI.DockOption();
			DockEffectBehavior = new GUI.DockEffectBehavior();
			DockFCurves = new GUI.DockFCurves();
			DockNetwork = new GUI.DockNetwork();

			MainForm.Show();
			
			LoadConfig();
			
			DockViewer.ShowViewer();
			
			Core.MainForm = MainForm;
		}

		internal static void LoadRecentConfig()
		{
            if (System.IO.File.Exists(configRecentPath))
			{
				System.Xml.XmlDocument doc = new System.Xml.XmlDocument();
                doc.Load(configRecentPath);
				var files = doc["Recent"].GetElements("File");
				foreach (var file in files)
				{
					AddRecentFile(file.GetText());
				}
			}
		}

		public static void CloseDockWindow()
		{
			if (DockViewerController != null) DockViewerController.Close();
			if (DockViewPoint != null) DockViewPoint.Close();
			if (DockRecorder != null) DockRecorder.Close();
			if (DockNodeTreeView != null) DockNodeTreeView.Close();
			if (DockNodeCommonValues != null) DockNodeCommonValues.Close();
			if (DockNodeLocationValues != null) DockNodeLocationValues.Close();
			if (DockNodeRotationValues != null) DockNodeRotationValues.Close();
			if (DockNodeScalingValues != null) DockNodeScalingValues.Close();
			if (DockNodeLocationAbsValues != null) DockNodeLocationAbsValues.Close();
			if (DockNodeGenerationLocationValues != null) DockNodeGenerationLocationValues.Close();
			if (DockNodeRendererCommonValues != null) DockNodeRendererCommonValues.Close();
			if (DockNodeDrawingValues != null) DockNodeDrawingValues.Close();
            if (DockNodeSoundValues != null) DockNodeSoundValues.Close();
			if (DockOption != null) DockOption.Close();
			if (DockEffectBehavior != null) DockEffectBehavior.Close();

			if (DockFCurves != null) DockFCurves.Close();
			if (DockNetwork != null) DockNetwork.Close();

			DockViewerController = null;
			DockViewPoint = null;
			DockRecorder = null;
			DockNodeTreeView = null;
			DockNodeCommonValues = null;
			DockNodeLocationValues = null;
			DockNodeRotationValues = null;
			DockNodeScalingValues = null;
			DockNodeLocationAbsValues = null;
			DockNodeGenerationLocationValues = null;
			DockNodeRendererCommonValues = null;
			DockNodeDrawingValues = null;
            DockNodeSoundValues = null;
			DockOption = null;
			DockEffectBehavior = null;
			DockFCurves = null;
			DockNetwork = null;
		}

		public static void AssignDockWindowIntoDefaultPosition()
		{
			CheckDockWindowClosed();

			if (DockViewerController == null) DockViewerController = new DockViewerController();
			if (DockViewPoint == null) DockViewPoint = new DockViewPoint();
			if (DockRecorder == null) DockRecorder = new DockRecorder();
			if (DockNodeTreeView == null) DockNodeTreeView = new DockNodeTreeView();
			if (DockNodeCommonValues == null) DockNodeCommonValues = new GUI.DockNodeCommonValues();
			if (DockNodeLocationValues == null) DockNodeLocationValues = new GUI.DockNodeLocationValues();
			if (DockNodeRotationValues == null) DockNodeRotationValues = new GUI.DockNodeRotationValues();
			if (DockNodeScalingValues == null) DockNodeScalingValues = new GUI.DockNodeScaleValues();
			if (DockNodeLocationAbsValues == null) DockNodeLocationAbsValues = new GUI.DockNodeLocationAbsValues();
			if (DockNodeGenerationLocationValues == null) DockNodeGenerationLocationValues = new DockNodeGenerationLocationValues();
			if (DockNodeRendererCommonValues == null) DockNodeRendererCommonValues = new DockNodeRendererCommonValues();
			if (DockNodeDrawingValues == null) DockNodeDrawingValues = new GUI.DockNodeRendererValues();
            if (DockNodeSoundValues == null) DockNodeSoundValues = new GUI.DockNodeSoundValues();
			if (DockOption == null) DockOption = new GUI.DockOption();
			if (DockEffectBehavior == null) DockEffectBehavior = new DockEffectBehavior();
			if (DockFCurves == null) DockFCurves = new GUI.DockFCurves();
			if (DockNetwork == null) DockNetwork = new GUI.DockNetwork();

			var rec = Screen.PrimaryScreen.Bounds;
			int _width = rec.Width;

			if (_width <= 1024)
			{
				MainForm.Panel.SuspendLayout(true);

				MainForm.Panel.DockRightPortion = 0.5f;
				MainForm.Panel.DockBottomPortion = 0.2f;

				DockViewer.Show(MainForm.Panel, DockState.Document);
				DockViewerController.Show(MainForm.Panel, DockState.DockBottom);

				DockNodeTreeView.Show(MainForm.Panel, DockState.DockRight);

				DockNodeLocationValues.Show(DockNodeTreeView.Pane, DockAlignment.Right, 0.65);

				DockNodeDrawingValues.Show(MainForm.Panel);
				DockNodeDrawingValues.DockTo(DockNodeLocationValues.Pane, DockStyle.Right, 0);

				DockNodeRotationValues.Show(MainForm.Panel);
				DockNodeRotationValues.DockTo(DockNodeLocationValues.Pane, DockStyle.Bottom, 0);

				DockNodeScalingValues.Show(MainForm.Panel);
				DockNodeScalingValues.DockTo(DockNodeRotationValues.Pane, DockStyle.Bottom, 0);

				DockNodeCommonValues.Show(DockNodeTreeView.Pane, DockAlignment.Bottom, 0.75);

				DockNodeRendererCommonValues.Show(MainForm.Panel);
				DockNodeRendererCommonValues.DockTo(DockNodeDrawingValues.Pane, DockStyle.Fill, 1);

                DockNodeSoundValues.Show(MainForm.Panel);
                DockNodeSoundValues.DockTo(DockNodeDrawingValues.Pane, DockStyle.Fill, 1);
                
				MainForm.Panel.ResumeLayout(true, true);
			}
			else if (_width <= 1600)
			{
				MainForm.Panel.SuspendLayout(true);

				MainForm.Panel.DockRightPortion = 0.5f;
				MainForm.Panel.DockBottomPortion = 0.2f;

				DockViewer.Show(MainForm.Panel, DockState.Document);
				DockViewerController.Show(MainForm.Panel, DockState.DockBottom);

				DockNodeTreeView.Show(MainForm.Panel, DockState.DockRight);

				DockNodeLocationValues.Show(DockNodeTreeView.Pane, DockAlignment.Right, 0.65);

				DockNodeDrawingValues.Show(MainForm.Panel);
				DockNodeDrawingValues.DockTo(DockNodeLocationValues.Pane, DockStyle.Right, 0);

				DockNodeRotationValues.Show(MainForm.Panel);
				DockNodeRotationValues.DockTo(DockNodeLocationValues.Pane, DockStyle.Bottom, 0);

				DockNodeScalingValues.Show(MainForm.Panel);
				DockNodeScalingValues.DockTo(DockNodeRotationValues.Pane, DockStyle.Bottom, 0);

				DockNodeCommonValues.Show(DockNodeTreeView.Pane, DockAlignment.Bottom, 0.75);

				DockNodeRendererCommonValues.Show(MainForm.Panel);
				DockNodeRendererCommonValues.DockTo(DockNodeDrawingValues.Pane, DockStyle.Fill, 1);

                DockNodeSoundValues.Show(MainForm.Panel);
                DockNodeSoundValues.DockTo(DockNodeDrawingValues.Pane, DockStyle.Fill, 1);

				MainForm.Panel.ResumeLayout(true, true);
			}
			else
			{
				MainForm.Panel.SuspendLayout(true);

				MainForm.Panel.DockRightPortion = 0.5f;
				MainForm.Panel.DockBottomPortion = 0.15f;

				DockViewer.Show(MainForm.Panel, DockState.Document);
				DockViewerController.Show(MainForm.Panel, DockState.DockBottom);

				DockNodeTreeView.Show(MainForm.Panel, DockState.DockRight);

				DockNodeLocationValues.Show(DockNodeTreeView.Pane, DockAlignment.Right, 0.65);

				DockNodeDrawingValues.Show(MainForm.Panel);
				DockNodeDrawingValues.DockTo(DockNodeLocationValues.Pane, DockStyle.Right, 0);
		
				DockNodeRotationValues.Show(MainForm.Panel);
				DockNodeRotationValues.DockTo(DockNodeLocationValues.Pane, DockStyle.Bottom, 0);

				DockNodeScalingValues.Show(MainForm.Panel);
				DockNodeScalingValues.DockTo(DockNodeRotationValues.Pane, DockStyle.Bottom, 0);

				DockNodeCommonValues.Show(DockNodeTreeView.Pane, DockAlignment.Bottom, 0.75);

				DockNodeRendererCommonValues.Show(MainForm.Panel);
				DockNodeRendererCommonValues.DockTo(DockNodeDrawingValues.Pane, DockStyle.Fill, 1);

                DockNodeSoundValues.Show(MainForm.Panel);
                DockNodeSoundValues.DockTo(DockNodeDrawingValues.Pane, DockStyle.Fill, 1);

				MainForm.Panel.ResumeLayout(true, true);
			}
		}

		public static void AssignDefaultPosition()
		{
			var rec = Screen.PrimaryScreen.Bounds;
			int _width = rec.Width;

			if (_width <= 1024)
			{
				MainForm.Location = new System.Drawing.Point((rec.Width - 800) / 2, (rec.Height - 600) / 2);
				MainForm.Width = 800;
				MainForm.Height = 600;
			}
			else if (_width <= 1600)
			{
				MainForm.Location = new System.Drawing.Point((rec.Width - 1200) / 2, (rec.Height - 600) / 2);
				MainForm.Width = 1200;
				MainForm.Height = 600;
			}
			else
			{
				MainForm.Location = new System.Drawing.Point((rec.Width - 1800) / 2, (rec.Height - 800) / 2);
				MainForm.Width = 1800;
				MainForm.Height = 800;
			}

			AssignDockWindowIntoDefaultPosition();
		}

		/// <summary>
		/// 指定した型のウインドウを開く、開いているなら選択する。
		/// </summary>
		/// <param name="type"></param>
		public static void SelectOrShowWindow(Type type)
		{
			CheckDockWindowClosed();

			var props = typeof(GUIManager).GetProperties(System.Reflection.BindingFlags.Static | System.Reflection.BindingFlags.NonPublic);

			foreach (var prop in props)
			{
				if(prop.PropertyType != type) continue;

				var value = prop.GetValue(null, null);
				if (value == null)
				{
					var constructor = type.GetConstructor(new Type[] { });
					value = constructor.Invoke(new object[] { });
					prop.SetValue(null, value, null);
					(value as DockContent).Show(MainForm.Panel, DockState.Float);
				}
				else
				{
					(value as DockContent).Select();
				}
			}
		}

		internal static void LoadConfig()
		{
            if (System.IO.File.Exists(configGuiPath) && System.IO.File.Exists(configGuiPanelPath))
			{
				// 設定ファイルが存在
				{
					System.Xml.XmlDocument doc = new System.Xml.XmlDocument();
                    doc.Load(configGuiPath);
					int x = doc["GUI"].GetTextAsInt("X");
					int y = doc["GUI"].GetTextAsInt("Y");
					int width = doc["GUI"].GetTextAsInt("Width");
					int height = doc["GUI"].GetTextAsInt("Height");
					MainForm.Location = new System.Drawing.Point(x, y);
					MainForm.Width = width;
					MainForm.Height = height;
				}

				{
					Func<string, WeifenLuo.WinFormsUI.Docking.IDockContent> get_dock = (s) =>
						{
							if (DockViewer.GetType().FullName == s)
							{
								return DockViewer;
							}

							if (DockViewerController.GetType().FullName == s)
							{
								return DockViewerController;
							}

							if (DockNodeTreeView.GetType().FullName == s)
							{
								return DockNodeTreeView;
							}

							if (DockViewPoint.GetType().FullName == s)
							{
								return DockViewPoint;
							}

							if (DockRecorder.GetType().FullName == s)
							{
								return DockRecorder;
							}

							if (DockNodeCommonValues.GetType().FullName == s)
							{
								return DockNodeCommonValues;
							}

							if (DockNodeLocationValues.GetType().FullName == s)
							{
								return DockNodeLocationValues;
							}

							if (DockNodeRotationValues.GetType().FullName == s)
							{
								return DockNodeRotationValues;
							}

							if (DockNodeRotationValues.GetType().FullName == s)
							{
								return DockNodeRotationValues;
							}

							if (DockNodeScalingValues.GetType().FullName == s)
							{
								return DockNodeScalingValues;
							}

							if (DockNodeLocationAbsValues.GetType().FullName == s)
							{
								return DockNodeLocationAbsValues;
							}

							if (DockNodeGenerationLocationValues.GetType().FullName == s)
							{
								return DockNodeGenerationLocationValues;
							}

							if (DockNodeRendererCommonValues.GetType().FullName == s)
							{
								return DockNodeRendererCommonValues;
							}

							if (DockNodeDrawingValues.GetType().FullName == s)
							{
								return DockNodeDrawingValues;
							}

                            if (DockNodeSoundValues.GetType().FullName == s)
                            {
                                return DockNodeSoundValues;
                            }

							if (DockOption.GetType().FullName == s)
							{
								return DockOption;
							}

							if (DockEffectBehavior.GetType().FullName == s)
							{
								return DockEffectBehavior;
							}


							if (DockFCurves.GetType().FullName == s)
							{
								return DockFCurves;
							}

							if (DockNetwork.GetType().FullName == s)
							{
								return DockNetwork;
							}

							return null;
						};

					var deserializeDockContent = new WeifenLuo.WinFormsUI.Docking.DeserializeDockContent(get_dock);

					MainForm.Panel.LoadFromXml(configGuiPanelPath, deserializeDockContent);
				}
			}
			else
			{
				// 設定ファイルが存在しない
				AssignDefaultPosition();
			}

			Network.Load(ConfigNetworkFileName);
		}

		internal static void SaveConfig()
		{
			{
				var rf = GetRecentFiles();

				System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

				System.Xml.XmlElement project_root = doc.CreateElement("Recent");

				foreach (var f in rf.Reverse())
				{
					project_root.AppendChild(doc.CreateTextElement("File", f));
				}

				doc.AppendChild(project_root);

				var dec = doc.CreateXmlDeclaration("1.0", "utf-8", null);
				doc.InsertBefore(dec, project_root);
                doc.Save(configRecentPath);
			}

			{
				System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

				System.Xml.XmlElement project_root = doc.CreateElement("GUI");

				project_root.AppendChild(doc.CreateTextElement("X", MainForm.Location.X.ToString()));
				project_root.AppendChild(doc.CreateTextElement("Y", MainForm.Location.Y.ToString()));
				project_root.AppendChild(doc.CreateTextElement("Width", MainForm.Width.ToString()));
				project_root.AppendChild(doc.CreateTextElement("Height", MainForm.Height.ToString()));
				doc.AppendChild(project_root);

				var dec = doc.CreateXmlDeclaration("1.0", "utf-8", null);
				doc.InsertBefore(dec, project_root);
                doc.Save(configGuiPath);
			}

            MainForm.Panel.SaveAsXml(configGuiPanelPath, Encoding.UTF8);

			Network.Save(configNetworkPath);
		}

		static void CheckDockWindowClosed()
		{
			if (DockViewerController != null && !DockViewerController.Created) DockViewerController = null;
			if (DockViewPoint != null && !DockViewPoint.Created) DockViewPoint = null;
			if (DockRecorder != null && !DockRecorder.Created) DockRecorder = null;
			if (DockNodeTreeView != null && !DockNodeTreeView.Created) DockNodeTreeView = null;
			if (DockNodeCommonValues != null && !DockNodeCommonValues.Created) DockNodeCommonValues = null;
			if (DockNodeLocationValues != null && !DockNodeLocationValues.Created) DockNodeLocationValues = null;
			if (DockNodeRotationValues != null && !DockNodeRotationValues.Created) DockNodeRotationValues = null;
			if (DockNodeScalingValues != null && !DockNodeScalingValues.Created) DockNodeScalingValues = null;
			if (DockNodeLocationAbsValues != null && !DockNodeLocationAbsValues.Created) DockNodeLocationAbsValues = null;
			if (DockNodeGenerationLocationValues != null && !DockNodeGenerationLocationValues.Created) DockNodeGenerationLocationValues = null;
			if (DockNodeRendererCommonValues != null && !DockNodeRendererCommonValues.Created) DockNodeRendererCommonValues = null;
			if (DockNodeDrawingValues != null && !DockNodeDrawingValues.Created) DockNodeDrawingValues = null;
            if (DockNodeSoundValues != null && !DockNodeSoundValues.Created) DockNodeSoundValues = null;
			if (DockOption != null && !DockOption.Created) DockOption = null;
			if (DockEffectBehavior != null && !DockEffectBehavior.Created) DockEffectBehavior = null;
			if (DockFCurves != null && !DockFCurves.Created) DockFCurves = null;
			if (DockNetwork != null && !DockNetwork.Created) DockNetwork = null;
		}

		public static bool Update()
		{
			if (!MainForm.Created) return false;

			Application.DoEvents();

			if (MainForm.Created)
			{
				MainForm.UpdateForm();
				Network.Update();

				CheckDockWindowClosed();

				if (DockViewPoint != null) DockViewPoint.UpdateViewPoint();
				if (DockRecorder != null) DockRecorder.UpdateRecorder();
				if (DockViewerController != null) DockViewerController.UpdateController();
				if (DockViewer != null) DockViewer.UpdateViewer();
				if( DockNetwork != null) DockNetwork.Update_();
			}

			return true;
		}

		public static void Close()
		{
			MainForm.Close();
		}

		/// <summary>
		/// 最近使用したファイルの追加
		/// </summary>
		/// <param name="fullPath">絶対パス</param>
		public static void AddRecentFile(string fullPath)
		{
			if (System.IO.Path.GetFullPath(fullPath) != fullPath) throw new Exception("絶対パスでありません。");

			recentFiles.Remove(fullPath);
			recentFiles.AddFirst(fullPath);
			while (recentFiles.Count > 10)
			{
				recentFiles.RemoveLast();
			}
			if (OnChangeRecentFiles != null)
			{
				OnChangeRecentFiles(null, null);
			}
		}

		/// <summary>
		/// 最近使用したファイル一覧の取得
		/// </summary>
		/// <returns></returns>
		public static string[] GetRecentFiles()
		{
			return recentFiles.ToArray();
		}

		/// <summary>
		/// アプリケーションの存在するディレクトリを取得
		/// </summary>
		/// <returns></returns>
		public static string GetEntryDirectory()
		{
			var myAssembly = System.Reflection.Assembly.GetEntryAssembly();
			string path = myAssembly.Location;

			return System.IO.Path.GetDirectoryName(path);
		}
	}
}
