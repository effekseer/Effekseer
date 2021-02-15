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
			Label = Icons.PanelNetwork + Resources.GetString("Network") + "###Network";

			TabToolTip = Resources.GetString("Network");
		}

		protected override void UpdateInternal()
		{
			var target = GUIManager.Network.Target;
			var port = new int[] { GUIManager.Network.Port };
			var autoConnect = new bool[] { GUIManager.Network.AutoConnect };
			var sendOnLoad = new bool[] { GUIManager.Network.SendOnLoad };
			var sendOnEdit = new bool[] { GUIManager.Network.SendOnEdit };
			var sendOnSave = new bool[] { GUIManager.Network.SendOnSave };

			if (GUIManager.NativeManager.InputText(Resources.GetString("NetworkAddress") + "###target", target))
			{
				GUIManager.Network.Target = GUIManager.NativeManager.GetInputTextResult();
			}

			if (GUIManager.NativeManager.InputInt(Resources.GetString("Port") + "###port", port))
			{
				GUIManager.Network.Port = port[0];
			}

			if (GUIManager.NativeManager.Checkbox(Resources.GetString("AutoConnect") + "###autoConnect", autoConnect))
			{
				GUIManager.Network.AutoConnect = autoConnect[0];
			}

			if (GUIManager.NativeManager.Checkbox(Resources.GetString("TransmitDataOnLoad") + "###sendOnLoad", sendOnLoad))
			{
				GUIManager.Network.SendOnLoad = sendOnLoad[0];
			}

			if (GUIManager.NativeManager.Checkbox(Resources.GetString("TransmitDataOnEdit") + "###sendOnEdit", sendOnEdit))
			{
				GUIManager.Network.SendOnEdit = sendOnEdit[0];
			}

			if (GUIManager.NativeManager.Checkbox(Resources.GetString("TransmitDataOnSave") + "###sendOnSave", sendOnSave))
			{
				GUIManager.Network.SendOnSave = sendOnSave[0];
			}

			string state = string.Empty;
			string connect = string.Empty;
			bool enabled = false;

			if (GUIManager.Network.IsConnected())
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

			GUIManager.NativeManager.Text(state);

			if (GUIManager.NativeManager.Button(connect + "###connect"))
			{
				if (GUIManager.Network.IsConnected())
				{
					GUIManager.Network.Disconnect();
				}
				else
				{
					GUIManager.Network.Connect();

					if (!GUIManager.Network.IsConnected())
					{
						var errorMessage = MultiLanguageTextProvider.GetText("Network_FailedToConnect");
						swig.GUIManager.show(errorMessage, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
					}
				}
			}

			if (GUIManager.NativeManager.Button(Resources.GetString("SendData") + "###send"))
			{
				if (enabled)
				{
					GUIManager.Network.Send();
				}
			}
		}
	}
}
