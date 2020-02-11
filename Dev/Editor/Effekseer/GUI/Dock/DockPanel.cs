using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
    class DockPanel : GroupControl, IRemovableControl, IDroppableControl
    {
		public string Label { get; set; } = string.Empty;

		bool opened = true;

		internal swig.DockSlot InitialDockSlot = swig.DockSlot.Float;

		internal swig.Vec2 InitialDockSize = new swig.Vec2(0, 0);

		internal float InitialDockRate = 0.5f;

		internal bool InitialDockReset = false;

		internal int InitialDockActive = 0;

		internal swig.ImageResource Icon;

		internal swig.Vec2 IconSize { get
			{
				float scale = Manager.DpiScale;
				return new swig.Vec2(18 * scale, 18 * scale);
			}
		}

		internal string TabToolTip = string.Empty;

		internal int IsInitialized = -1;

		protected bool NoPadding = false;
		protected bool NoScrollBar = false;

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
						Manager.NativeManager.SetNextDock(InitialDockSlot);
						Manager.NativeManager.SetNextDockRate(InitialDockRate);
						if (InitialDockReset)
						{
							Manager.NativeManager.ResetNextParentDock();
						}

						IsInitialized++;
					}

					if(Icon != null)
					{
						Manager.NativeManager.SetNextDockIcon(Icon, IconSize);
					}

					if(!String.IsNullOrEmpty(TabToolTip))
					{
						Manager.NativeManager.SetNextDockTabToolTip(TabToolTip);
					}

					
					swig.WindowFlags flags = swig.WindowFlags.None;

					if (NoScrollBar)
					{
						flags = swig.WindowFlags.NoScrollbar;
					}

					if (NoPadding) Manager.NativeManager.PushStyleVar(swig.ImGuiStyleVarFlags.WindowPadding, new swig.Vec2(0.0f, 0.0f));

					bool dockEnabled = Manager.NativeManager.BeginDock(Label, ref opened, flags, InitialDockSize);

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
					
					if (InitialDockActive > 0)
					{
						Manager.NativeManager.SetDockActive();
						InitialDockActive--;
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
				return Manager.NativeManager.GetDockActive();
			}
			return false;
		}

        protected virtual void UpdateInternal()
        {
        }
    }
}
