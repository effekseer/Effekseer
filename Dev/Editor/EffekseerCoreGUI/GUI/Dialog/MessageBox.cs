using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dialog
{
    class MessageBox : IRemovableControl
    {
        string title = string.Empty;
        string message = string.Empty;
        string id = "###messageBox";

		bool opened = true;

		bool isFirstUpdate = true;

        public bool ShouldBeRemoved { get; private set; } = false;

        public void Show(string title, string message)
        {
            this.title = title;
            this.message = message;

			GUIManager.AddControl(this);
		}

        public void Update()
        {
			if(isFirstUpdate)
			{
				GUIManager.NativeManager.OpenPopup(id);
				isFirstUpdate = false;
			}

            if(GUIManager.NativeManager.BeginPopupModal(title + id, ref opened, swig.WindowFlags.AlwaysAutoResize))
            {
				GUIManager.NativeManager.Text(message);

				GUIManager.NativeManager.Separator();

				GUIManager.NativeManager.SetCursorPosX(GUIManager.NativeManager.GetContentRegionAvail().X / 2 - 100 / 2);

				if (GUIManager.NativeManager.Button("OK", 100))
				{
					ShouldBeRemoved = true;
				}

				GUIManager.NativeManager.EndPopup();
            }
            else
            {
                ShouldBeRemoved = true;
            }
        }
    }
}
