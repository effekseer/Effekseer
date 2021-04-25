using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class PrefabListDock : DockPanel
	{
		// TODO: Dummy Data
		class PrefabTest
		{
			public string title;
		}

		private List<PrefabTest> _prefabs = new List<PrefabTest>()
		{
			new PrefabTest(){ title = "�A�j���[�V�����f��-A" },
			new PrefabTest(){ title = "�A�j���[�V�����f��-B" },
			new PrefabTest(){ title = "�A�j���[�V�����f��-C" },
			new PrefabTest(){ title = "�A�j���[�V�����f��-D" },
			new PrefabTest(){ title = "�A�j���[�V�����f��-E" },
		};


		public PrefabListDock()
		{
			Label = Icons.PanelOptions + Resources.GetString("Options") + "###PrefabListDock";

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;

			TabToolTip = Resources.GetString("Options");

			NoPadding = true;
			NoScrollBar = true;
			NoCloseButton = true;
			AllowsShortTab = false;
		}

		public override void OnDisposed()
		{
			Core.OnAfterLoad -= OnAfter;
			Core.OnAfterNew -= OnAfter;
		}

		protected override void UpdateInternal()
		{
			foreach (var item in _prefabs)
			{
				if (Manager.NativeManager.TreeNodeEx(item.title, swig.TreeNodeFlags.Leaf))
				{
					// D&D Source
					if (Manager.NativeManager.BeginDragDropSource())
					{
						byte[] idBuf = new byte[5];
						if (Manager.NativeManager.SetDragDropPayload("PayloadName", idBuf, idBuf.Length))
						{
						}
						Manager.NativeManager.Text(item.title);

						Manager.NativeManager.EndDragDropSource();
					}


					Manager.NativeManager.TreePop();
				}
			}
		}

		void OnAfter(object sender, EventArgs e)
		{
		}
	}
}
