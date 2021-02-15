using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.swig;
using Effekseer.Utl;

namespace Effekseer.GUI
{
	public class GUIManager
	{
		public static swig.GUIManager NativeManager;
		public static swig.Native Native;
		public static swig.MainWindow MainWindow;
		public static swig.IO IO;
		public static Viewer Viewer;
		internal static Network Network;


		internal static bool DoesChangeColorOnChangedValue = true;

		private static int nextID = 10;

		private static bool isFontSizeDirtied = true;

		internal static Utils.DelayedList<IRemovableControl> Controls = new Utils.DelayedList<IRemovableControl>();
		static Dock.DockManager dockManager = null;
		static Dock.DockPanel[] panels = new Dock.DockPanel[0];

		public static bool IsDockMode() { return dockManager != null; }

		public static float TextOffsetY { get; private set; }

		public static float DpiScale
		{
			get
			{
				return NativeManager.GetDpiScale();
			}
		}

		//public static bool Initialize(int width, int height, swig.DeviceType deviceType)
		//{
		//	return true;
		//}

		/// <summary>
		/// get a scale based on font size for margin, etc.
		/// </summary>
		/// <returns></returns>
		public static float GetUIScaleBasedOnFontSize()
		{
			return Core.Option.FontSize.Value / 16.0f * DpiScale;
		}

		public static void UpdateFont()
		{
			isFontSizeDirtied = true;
		}

		/// <summary>
		/// Get unique id in this aplication.
		/// </summary>
		/// <returns></returns>
		public static int GetUniqueID()
		{
			nextID++;
			return nextID;
		}

		public static void AddControl(IRemovableControl control)
		{
			Controls.Add(control);
		}

		internal static Dock.DockPanel GetWindow(Type t)
		{
			foreach (var panel in panels)
			{
				if (panel != null && panel.GetType() == t) return panel;
			}

			return null;
		}

		internal static Dock.DockPanel SelectOrShowWindow(Type t, swig.Vec2 defaultSize = null, bool resetRect = false)
		{
			for (int i = 0; i < dockTypes.Length; i++)
			{
				if (dockTypes[i] != t) continue;

				if (panels[i] != null)
				{
					panels[i].SetFocus();
					return panels[i];
				}
				else
				{
					if (defaultSize == null)
					{
						defaultSize = new swig.Vec2();
					}

					panels[i] = (Dock.DockPanel)t.GetConstructor(Type.EmptyTypes).Invoke(null);
					panels[i].InitialDockSize = defaultSize;
					panels[i].IsInitialized = -1;
					panels[i].ResetSize = resetRect;

					if (dockManager != null)
					{
						dockManager.Controls.Add(panels[i]);
					}
					else
					{
						AddControl(panels[i]);
					}

					return panels[i];
				}
			}

			return null;
		}
	}
}
