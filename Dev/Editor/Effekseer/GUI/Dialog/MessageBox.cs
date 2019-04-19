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

			Manager.AddControl(this);
		}

        public void Update()
        {
			if(isFirstUpdate)
			{
				Manager.NativeManager.OpenPopup(id);
				isFirstUpdate = false;
			}

            if(Manager.NativeManager.BeginPopupModal(title + id, ref opened, swig.WindowFlags.AlwaysAutoResize))
            {
                Manager.NativeManager.Text(message);

                if(Manager.NativeManager.Button("OK?"))
                {
                    ShouldBeRemoved = true;
                }

                Manager.NativeManager.EndPopup();
            }
            else
            {
                ShouldBeRemoved = true;
            }
        }
    }
}
