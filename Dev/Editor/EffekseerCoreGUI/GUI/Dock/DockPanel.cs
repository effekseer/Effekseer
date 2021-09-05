using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
    public class DockPanel : GroupControl, IRemovableControl, IDroppableControl
    {
		public string Label { get; set; } = string.Empty;

		public string TabLabel
		{
			get
			{
				if (AllowsShortTab)
				{
					return Label.Substring(0, 1) + Label.Substring(Label.IndexOf("###"));
				}
				return Label;
			}
		}

		public string WindowID { get { return Label.Substring(Label.IndexOf("###")); } }

		internal swig.Vec2 InitialDockSize = new swig.Vec2(0, 0);

		internal bool ResetSize = false;

		bool opened = true;

		internal swig.Vec2 IconSize { get
			{
				float scale = Manager.DpiScale;
				return new swig.Vec2(18 * scale, 18 * scale);
			}
		}

		protected internal string TabToolTip = string.Empty;

		internal int IsInitialized = -1;

		protected bool NoPadding = false;
		protected bool NoScrollBar = false;
		protected bool NoCloseButton = false;
		protected bool AllowsShortTab = true;

		public bool Visibled { get; private set; }
		private bool Windowed = false;

		public DockPanel()
		{
		}

		public override void Update()
		{
			if(opened)
			{
				if (Manager.IsDockMode())
				{
					if (IsInitialized < 0)
					{
						IsInitialized++;
					}

					if (ResetSize)
					{
						Manager.NativeManager.SetNextWindowSize(InitialDockSize.X, InitialDockSize.Y, swig.Cond.Appearing);
						ResetSize = false;
					}

					swig.WindowFlags flags = swig.WindowFlags.None;

					if (NoScrollBar)
					{
						flags = swig.WindowFlags.NoScrollbar;
					}

					if (NoPadding) Manager.NativeManager.PushStyleVar(swig.ImGuiStyleVarFlags.WindowPadding, new swig.Vec2(0.0f, 0.0f));

					bool dockEnabled = Manager.NativeManager.BeginDock(
						Label, TabLabel, ref opened, Visibled && Windowed && !NoCloseButton, flags);

					Visibled = Manager.NativeManager.IsDockVisibled();
					Windowed = Manager.NativeManager.IsDockWindowed();

					if (NoPadding) Manager.NativeManager.PopStyleVar();

					if (dockEnabled)
					{
						UpdateInternal();

						Controls.Lock();

						foreach (var c in Controls.Internal)
						{
							c.Update();
						}

						Controls.Unlock();
					}
					
					Manager.NativeManager.EndDock();
				}
				else
				{
					if (Manager.NativeManager.Begin(Label, ref opened))
					{
						UpdateInternal();


						Controls.Lock();

						foreach (var c in Controls.Internal)
						{
							c.Update();
						}

						Controls.Unlock();
					}

					Manager.NativeManager.End();
				}
			}
			else
			{
				ShouldBeRemoved = true;
			}
		}

		public void Close()
		{
			opened = false;
		}

		public bool IsDockActive()
		{
			if (Manager.IsDockMode())
			{
				return Manager.NativeManager.IsDockFocused();
			}
			return false;
		}

		public void SetFocus()
		{
			if (Manager.IsDockMode())
			{
				Manager.NativeManager.SetDockFocus(WindowID);
			}
		}

		protected virtual void UpdateInternal()
        {
        }
    }
}
