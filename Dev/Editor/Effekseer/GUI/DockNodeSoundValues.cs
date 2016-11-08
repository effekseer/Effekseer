using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Effekseer.GUI
{
	public partial class DockNodeSoundValues : DockContent
	{
        public DockNodeSoundValues()
		{
			InitializeComponent();
			HandleCreated += new EventHandler(DockNodeSoundValues_HandleCreated);
			HandleDestroyed += new EventHandler(DockNodeSoundValues_HandleDestroyed);

			lp_Sound.SetType(typeof(Data.SoundValues));

			if (Core.Language == Language.English)
			{
				Text = "Sound";
			}
			
			Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconSound).GetHicon());
		}

		void Read()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					lp_Sound.SetValue(((Data.Node)Core.SelectedNode).SoundValues);
				}
				else
				{
					lp_Sound.SetValue(null);
				}
			}
			else
			{
				lp_Sound.SetValue(null);
			}
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}

		private void DockNodeSoundValues_Load(object sender, EventArgs e)
		{
			Read();
		}

		void DockNodeSoundValues_HandleCreated(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode += OnAfterSelectNode;
		}

		void DockNodeSoundValues_HandleDestroyed(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}
	}
}
