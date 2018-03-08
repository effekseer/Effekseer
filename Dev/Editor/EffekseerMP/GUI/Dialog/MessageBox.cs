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

        public bool ShouldBeRemoved { get; private set; } = false;

        public void Show(string title, string message)
        {
            this.title = title;
            this.message = message;
            Manager.NativeManager.OpenPopup(id);

            Manager.Controls.Add(this);
        }

        public void Update()
        {
            if(Manager.NativeManager.BeginPopupModal(title + id, null, swig.WindowFlags.AlwaysAutoResize))
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
