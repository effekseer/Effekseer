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
	public partial class DockNetwork : DockContent
	{
		public DockNetwork()
		{
			InitializeComponent();

            Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconNetwork).GetHicon());

			HandleCreated += DockNetwork_HandleCreated;
			HandleDestroyed += DockNetwork_HandleDestroyed;
        }

		private void DockNetwork_HandleCreated(object sender, EventArgs e)
		{
			GUIManager.Network.Loaded += Reload;
		}

		private void DockNetwork_HandleDestroyed(object sender, EventArgs e)
		{
			GUIManager.Network.Loaded -= Reload;
		}

		private void DockNetwork_Load(object sender, EventArgs e)
		{
			txt_target.WriteMethod += (v,w) =>
				{
					GUIManager.Network.Target = v;
				};

			txt_target.ReadMethod += () =>
				{
					return GUIManager.Network.Target;
				};

			txt_port.WriteMethod += (v, w) =>
				{
					GUIManager.Network.Port = v;
				};

			txt_port.ReadMethod += () =>
				{
					return GUIManager.Network.Port;
				};

			txt_target.IsEnable += () => { return true; };
			txt_port.IsEnable += () => { return true; };

			txt_target.Reload();
			txt_port.Reload();

			cb_autoConnect.Checked = GUIManager.Network.AutoConnect;
			cb_sendOnLoad.Checked = GUIManager.Network.SendOnLoad;
			cb_sendOnEdit.Checked = GUIManager.Network.SendOnEdit;
			cb_sendOnSave.Checked = GUIManager.Network.SendOnSave;

			cb_autoConnect.CheckedChanged += new EventHandler(cb_autoConnect_CheckedChanged);
			cb_sendOnLoad.CheckedChanged += new EventHandler(cb_sendOnLoad_CheckedChanged);
			cb_sendOnEdit.CheckedChanged += new EventHandler(cb_sendOnEdit_CheckedChanged);
			cb_sendOnSave.CheckedChanged += new EventHandler(cb_sendOnSave_CheckedChanged);
		}

		void cb_autoConnect_CheckedChanged(object sender, EventArgs e)
		{
			GUIManager.Network.AutoConnect = cb_autoConnect.Checked;
		}

		void cb_sendOnSave_CheckedChanged(object sender, EventArgs e)
		{
			GUIManager.Network.SendOnSave = cb_sendOnSave.Checked;
		}

		void cb_sendOnEdit_CheckedChanged(object sender, EventArgs e)
		{
			GUIManager.Network.SendOnEdit = cb_sendOnEdit.Checked;
		}

		void cb_sendOnLoad_CheckedChanged(object sender, EventArgs e)
		{
			GUIManager.Network.SendOnLoad = cb_sendOnLoad.Checked;
		}

		public void Reload()
		{
			txt_target.Reload();
			txt_port.Reload();
			cb_autoConnect.Checked = GUIManager.Network.AutoConnect;
			cb_sendOnLoad.Checked = GUIManager.Network.SendOnLoad;
			cb_sendOnEdit.Checked = GUIManager.Network.SendOnEdit;
			cb_sendOnSave.Checked = GUIManager.Network.SendOnSave;
		}

		public void Update_()
		{
			if (GUIManager.Network.IsConnected())
			{
                lbl_state.Text = Properties.Resources.NetworkConnected;
				btn_connect.Text = Properties.Resources.Disconnect;
				btn_send.Enabled = true;
			}
			else
			{
                lbl_state.Text = Properties.Resources.NetworkDisconnected;
                btn_connect.Text = Properties.Resources.Connect;
				btn_send.Enabled = false;
			}
		}

		private unsafe void btn_send_Click(object sender, EventArgs e)
		{
			GUIManager.Network.Send();
		}

		private void btn_connect_Click(object sender, EventArgs e)
		{
			if (GUIManager.Network.IsConnected())
			{
				GUIManager.Network.Disconnect();
			}
			else
			{
				GUIManager.Network.Connect();
			}
		}
	}
}
