using System;

namespace Effekseer.GUI.Menu
{
	internal sealed class WindowMenu
	{
		private static readonly DockSettings[] DockSettingsArray;

		static WindowMenu()
		{
			DockSettingsArray = new[]
			{
				new DockSettings("NodeTree", typeof(Dock.NodeTreeView), Icons.PanelNodeTree),
				new DockSettings("BasicSettings", typeof(Dock.CommonValues), Icons.PanelCommon),
				new DockSettings("Position", typeof(Dock.LocationValues), Icons.PanelLocation),
				new DockSettings("AttractionForces", typeof(Dock.LocationAbsValues), Icons.PanelForceField),
				new DockSettings("SpawningMethod", typeof(Dock.GenerationLocationValues), Icons.PanelGeneration),
				new DockSettings("Rotation", typeof(Dock.RotationValues), Icons.PanelRotation),
				new DockSettings("Scale", typeof(Dock.ScaleValues), Icons.PanelScale),
				new DockSettings("Depth", typeof(Dock.DepthValues), Icons.PanelDepth),
				new DockSettings("RenderSettings", typeof(Dock.RendererValues), Icons.PanelRender),
				new DockSettings("BasicRenderSettings", typeof(Dock.RendererCommonValues), Icons.PanelRenderCommon),
				new DockSettings("AdvancedRenderSettings", typeof(Dock.AdvancedRenderCommonValues), Icons.PanelDynamicParams),
				new DockSettings("AdvancedRenderSettings2", typeof(Dock.AdvancedRenderCommonValues2), Icons.PanelDynamicParams),
				new DockSettings("Sound", typeof(Dock.SoundValues), Icons.PanelSound),
				new DockSettings("FCurves", typeof(Dock.FCurves), Icons.PanelFCurve),
				new DockSettings("Global", typeof(Dock.GlobalValues), Icons.PanelGlobal),
				new DockSettings("Culling", typeof(Dock.Culling), Icons.PanelCulling),
				new DockSettings("DynamicParameter_Name", typeof(Dock.Dynamic), Icons.PanelDynamicParams),
				new DockSettings("ViewerControls", typeof(Dock.ViewerController), Icons.PanelViewerCtrl),
				new DockSettings("CameraSettings", typeof(Dock.ViewPoint), Icons.PanelViewPoint),
				new DockSettings("Environment_Name", typeof(Dock.Environement), Icons.PanelEnvironment),
				new DockSettings("Behavior", typeof(Dock.BehaviorValues), Icons.PanelBehavior),
				new DockSettings("Network", typeof(Dock.Network), Icons.PanelNetwork),
				new DockSettings("Recorder", typeof(Dock.Recorder), Icons.PanelRecorder),
				new DockSettings("FileViewer", typeof(Dock.FileViewer), Icons.PanelFileViewer),
				new DockSettings("Options", typeof(Dock.Option), Icons.PanelOptions),
			};
		}

		public static Menu SetupWindowMenu()
		{
			var menu = new Menu(new MultiLanguageString("Window"));

			{
				var item = new MenuItem();
				item.Label = new MultiLanguageString("ResetWindow");
				item.Clicked += () => { Manager.ResetWindow(); };
				menu.Controls.Add(item);
			}

			foreach (var setting in DockSettingsArray)
			{
				setting.AddTo(menu);
				if (setting.Title == "DynamicParameter_Name")
				{
					menu.Controls.Add(new MenuSeparator());
				}
			}

			return menu;
		}

		private sealed class DockSettings
		{
			public string Title { get; }
			private readonly Type _type;
			private readonly string _iconName;

			public DockSettings(string title, Type type, string iconName)
			{
				Title = title;
				_type = type;
				_iconName = iconName;
			}

			public void AddTo(Menu menu)
			{
				var item = new MenuItem();
				item.Label = new MultiLanguageString(Title);
				item.Clicked += () => { Manager.SelectOrShowWindow(_type, new swig.Vec2(300, 300), true); };
				item.Icon = _iconName;
				menu.Controls.Add(item);
			}
		}
	}
}
