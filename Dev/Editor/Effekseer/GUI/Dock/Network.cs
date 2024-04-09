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
			Label = Icons.PanelNetwork + MultiLanguageTextProvider.GetText("Network") + "###Network";
			DocPage = "network.html";

			TabToolTip = MultiLanguageTextProvider.GetText("Network");
		}

		protected override void UpdateInternal()
		{
			var target = Manager.Network.Target;
			var port = new int[] { Manager.Network.Port };
			var autoConnect = new bool[] { Manager.Network.AutoConnect };
			var sendOnLoad = new bool[] { Manager.Network.SendOnLoad };
			var sendOnEdit = new bool[] { Manager.Network.SendOnEdit };
			var sendOnSave = new bool[] { Manager.Network.SendOnSave };

			if (Manager.NativeManager.InputText(MultiLanguageTextProvider.GetText("NetworkAddress") + "###target", target))
			{
				Manager.Network.Target = Manager.NativeManager.GetInputTextResult();
			}

			if (Manager.NativeManager.InputInt(MultiLanguageTextProvider.GetText("Port") + "###port", port))
			{
				Manager.Network.Port = port[0];
			}

			if (Manager.NativeManager.Checkbox(MultiLanguageTextProvider.GetText("AutoConnect") + "###autoConnect", autoConnect))
			{
				Manager.Network.AutoConnect = autoConnect[0];
			}

			if (Manager.NativeManager.Checkbox(MultiLanguageTextProvider.GetText("TransmitDataOnLoad") + "###sendOnLoad", sendOnLoad))
			{
				Manager.Network.SendOnLoad = sendOnLoad[0];
			}

			if (Manager.NativeManager.Checkbox(MultiLanguageTextProvider.GetText("TransmitDataOnEdit") + "###sendOnEdit", sendOnEdit))
			{
				Manager.Network.SendOnEdit = sendOnEdit[0];
			}

			if (Manager.NativeManager.Checkbox(MultiLanguageTextProvider.GetText("TransmitDataOnSave") + "###sendOnSave", sendOnSave))
			{
				Manager.Network.SendOnSave = sendOnSave[0];
			}

			string state = string.Empty;
			string connect = string.Empty;
			bool enabled = false;

			if (Manager.Network.IsConnected())
			{
				state = MultiLanguageTextProvider.GetText("NetworkConnected");
				connect = MultiLanguageTextProvider.GetText("Disconnect");
				enabled = true;
			}
			else
			{
				state = MultiLanguageTextProvider.GetText("NetworkDisconnected");
				connect = MultiLanguageTextProvider.GetText("Connect");
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

			if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("SendData") + "###send"))
			{
				if (enabled)
				{
					Manager.Network.Send();
				}
			}
		}
	}
}