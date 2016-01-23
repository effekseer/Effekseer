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

			if(Core.Language == Language.English)
			{
				Text = "Network";

				btn_connect.Text = "Connect";
				btn_send.Text = "Send Data";
				lbl_state.Text = "State：";
				cb_sendOnSave.Text = "Transmit data on save";
				cb_sendOnEdit.Text = "Transmit data on edit";
				cb_sendOnLoad.Text = "Transmit data on load";
				cb_autoConnect.Text = "Auto-connect";
				lbl_port.Text = "Port";
				lbl_target.Text = "Address";
			}
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
			if(Core.Language == Language.Japanese)
			{
				if (Effekseer.Core.Viewer.IsConnected())
				{
					lbl_state.Text = "状態：接続済";
					btn_connect.Text = "切断";
					btn_send.Enabled = true;
				}
				else
				{
					lbl_state.Text = "状態：未接続";
					btn_connect.Text = "接続";
					btn_send.Enabled = false;
				}
			}
			else if(Core.Language == Language.English)
			{
				if (Effekseer.Core.Viewer.IsConnected())
				{
					lbl_state.Text = "State: Connected";
					btn_connect.Text = "Disconnect";
					btn_send.Enabled = true;
				}
				else
				{
					lbl_state.Text = "State: Disconnected";
					btn_connect.Text = "Connect";
					btn_send.Enabled = false;
				}
			}

		}

		private unsafe void btn_send_Click(object sender, EventArgs e)
		{
			GUIManager.Network.Send();
		}

		private void btn_connect_Click(object sender, EventArgs e)
		{
			if (Effekseer.Core.Viewer.IsConnected())
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
