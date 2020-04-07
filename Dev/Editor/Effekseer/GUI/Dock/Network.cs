using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class Network : DockPanel
	{
		public Network()
		{
			Label = Resources.GetString("Network") + "###Network";

			Icon = Images.GetIcon("PanelNetwork");
			TabToolTip = Resources.GetString("Network");
		}

		protected override void UpdateInternal()
		{
			var target = Manager.Network.Target;
			var port = new int[] { Manager.Network.Port };
			var autoConnect = new bool[] { Manager.Network.AutoConnect };
			var sendOnLoad = new bool[] { Manager.Network.SendOnLoad };
			var sendOnEdit = new bool[] { Manager.Network.SendOnEdit };
			var sendOnSave = new bool[] { Manager.Network.SendOnSave };

			if (Manager.NativeManager.InputText(Resources.GetString("NetworkAddress") + "###target", target))
			{
				Manager.Network.Target = Manager.NativeManager.GetInputTextResult();
			}

			if (Manager.NativeManager.InputInt(Resources.GetString("Port") + "###port", port))
			{
				Manager.Network.Port = port[0];
			}

			if (Manager.NativeManager.Checkbox(Resources.GetString("AutoConnect") + "###autoConnect", autoConnect))
			{
				Manager.Network.AutoConnect = autoConnect[0];
			}

			if (Manager.NativeManager.Checkbox(Resources.GetString("TransmitDataOnLoad") + "###sendOnLoad", sendOnLoad))
			{
				Manager.Network.SendOnLoad = sendOnLoad[0];
			}

			if (Manager.NativeManager.Checkbox(Resources.GetString("TransmitDataOnEdit") + "###sendOnEdit", sendOnEdit))
			{
				Manager.Network.SendOnEdit = sendOnEdit[0];
			}

			if (Manager.NativeManager.Checkbox(Resources.GetString("TransmitDataOnSave") + "###sendOnSave", sendOnSave))
			{
				Manager.Network.SendOnSave = sendOnSave[0];
			}

			string state = string.Empty;
			string connect = string.Empty;
			bool enabled = false;

			if (Manager.Network.IsConnected())
			{
				state = Resources.GetString("NetworkConnected");
				connect = Resources.GetString("Disconnect");
				enabled = true;
			}
			else
			{
				state = Resources.GetString("NetworkDisconnected");
				connect = Resources.GetString("Connect");
				enabled = false;
			}

			Manager.NativeManager.Text(state);

			if (Manager.NativeManager.Button(connect + "###connect"))
			{
				if (Manager.Network.IsConnected())
				{
					Manager.Network.Disconnect();
				}
				else
				{
					Manager.Network.Connect();

					if (!Manager.Network.IsConnected())
					{
						var errorMessage = MultiLanguageTextProvider.GetText("Network_FailedToConnect");
						swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
					}
				}
			}

			if (Manager.NativeManager.Button(Resources.GetString("SendData") + "###send"))
			{
				if (enabled)
				{
					Manager.Network.Send();
				}
			}
		}
	}
}
