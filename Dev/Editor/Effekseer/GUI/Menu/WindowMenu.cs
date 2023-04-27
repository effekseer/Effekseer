using System;

namespace Effekseer.GUI.Menu
{
	internal sealed class WindowMenu
	{
		private static readonly DockSettings[] DockSettingsArray = {
			new DockSettings("NodeTree", typeof(Dock.NodeTreeView), Icons.PanelNodeTree),
			new DockSettings("BasicSettings", typeof(Dock.CommonValues), Icons.PanelCommon),
			new DockSettings("Position", typeof(Dock.LocationValues), Icons.PanelLocation),
			new DockSettings("AttractionForces", typeof(Dock.LocationAbsValues), Icons.PanelForceField),
			new DockSettings("SpawningMethod", typeof(Dock.GenerationLocationValues), Icons.PanelGeneration),
			new DockSettings("KillRules", typeof(Dock.KillRules), Icons.PanelKillRules),
			new DockSettings("Rotation", typeof(Dock.RotationValues), Icons.PanelRotation),
			new DockSettings("Scale", typeof(Dock.ScaleValues), Icons.PanelScale),
			new DockSettings("Depth", typeof(Dock.DepthValues), Icons.PanelDepth),
			new DockSettings("RenderSettings", typeof(Dock.RendererValues), Icons.PanelRender),
			new DockSettings("BasicRenderSettings", typeof(Dock.RendererCommonValues), Icons.PanelRenderCommon),
			new DockSettings("AdvancedRenderSettings", typeof(Dock.AdvancedRenderCommonValues), Icons.PanelAdvancedRenderCommon),
			new DockSettings("Sound", typeof(Dock.SoundValues), Icons.PanelSound),
			new DockSettings("FCurves", typeof(Dock.FCurves), Icons.PanelFCurve),
			new DockSettings("Global", typeof(Dock.GlobalValues), Icons.PanelGlobal),
			new DockSettings("Culling", typeof(Dock.Culling), Icons.PanelCulling),
			new DockSettings("LOD_Name", typeof(Dock.LOD), Icons.PanelLOD),
			new DockSettings("DynamicParameter_Name", typeof(Dock.Dynamic), Icons.PanelDynamicParams),
			new DockSettings("ProceduralModel_Name", typeof(Dock.ProceduralModel), Icons.PanelProceduralModel),
			new DockSettings("ViewerControls", typeof(Dock.ViewerController), Icons.PanelViewerCtrl),
			new DockSettings("CameraSettings", typeof(Dock.ViewPoint), Icons.PanelViewPoint),
			new DockSettings("Environment_Name", typeof(Dock.Environement), Icons.PanelEnvironment),
			new DockSettings("Behavior", typeof(Dock.BehaviorValues), Icons.PanelBehavior),
			new DockSettings("Network", typeof(Dock.Network), Icons.PanelNetwork),
			new DockSettings("Recorder", typeof(Dock.Recorder), Icons.PanelRecorder),
			new DockSettings("FileBrowser", typeof(Dock.FileBrowser), Icons.PanelFileBrowser),
			new DockSettings("Options", typeof(Dock.Option), Icons.PanelOptions),

			new DockSettings("Inspector", typeof(Inspector.InspectorPanel), Icons.PanelOptions),
			new DockSettings("Inspector_CommonParameter", typeof(DockPanels.CommonValues), Icons.PanelOptions),
			new DockSettings("Inspector_RendererParameter", typeof(DockPanels.RendererValues), Icons.PanelOptions),
			new DockSettings("Inspector_LocationValues", typeof(DockPanels.LocationValues), Icons.PanelOptions),
			new DockSettings("Inspector_RotationValues", typeof(DockPanels.RotationValues), Icons.PanelOptions),
			new DockSettings("Inspector_DynamicParameter", typeof(DockPanels.Dymamic), Icons.PanelOptions),
			new DockSettings("Inspector_Option", typeof(DockPanels.Option), Icons.PanelOptions),
			new DockSettings("NodeTreeEditor", typeof(Dock.NodeTreeEditor), Icons.PanelOptions),

			new DockSettings("Profiler", typeof(Dock.Profiler), Icons.PanelProfiler),
		};

		public Menu SetupWindowMenu()
		{
			var menu = new Menu(new MultiLanguageString("Window"));

			{
				var item = new MenuItem();
				item.Label = new MultiLanguageString("ResetWindow");
				item.Clicked += Manager.ResetWindow;
				menu.Controls.Add(item);
			}

			foreach (var setting in DockSettingsArray)
			{
				setting.AddTo(menu);
				if (setting.Title == "ProceduralModel_Name")
				{
					menu.Controls.Add(new MenuSeparator());
				}
			}

			return menu;
		}

		private sealed class DockSettings
		{
			private readonly Type _type;
			private readonly string _iconName;

			public string Title { get; }

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
				item.Icon = _iconName;
				item.Clicked += () => Manager.SelectOrShowWindow(_type, new swig.Vec2(300, 300), true);
				item.OnUpdate = () =>
				{
					var dock = Manager.GetWindow(_type);
					item.Checked = dock != null;
				};

				menu.Controls.Add(item);
			}
		}
	}
}