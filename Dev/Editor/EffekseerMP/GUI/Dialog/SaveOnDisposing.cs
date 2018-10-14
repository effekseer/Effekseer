using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dialog
{
	class SaveOnDisposing : IRemovableControl
    {
		string title = string.Empty;
        string message = string.Empty;
        string id = "###saveOnDisposing";

        bool opened = true;

        bool isFirstUpdate = true;
              
		public bool ShouldBeRemoved { get; private set; } = false;

		Action disposed = null;

		public SaveOnDisposing(Action disposed)
		{
			title = "Warning";
			var format = Resources.GetString("ConfirmSaveChanged");
			message = string.Format(format, System.IO.Path.GetFileName(Core.FullPath));

			this.disposed = disposed;

			// if already show window, don't add control
			if(Manager.Controls.Internal.Any(_=>_ is SaveOnDisposing))
			{
				return;
			}

			Manager.AddControl(this);
		}

		public void Update()
		{
			if (isFirstUpdate)
            {
                Manager.NativeManager.OpenPopup(id);
                isFirstUpdate = false;
            }

            if (Manager.NativeManager.BeginPopupModal(title + id, ref opened, swig.WindowFlags.AlwaysAutoResize))
            {
                Manager.NativeManager.Text(message);

				Manager.NativeManager.Separator();

                if (Manager.NativeManager.Button("Yes", 100))
				{
					if(Commands.Overwrite())
					{
						ShouldBeRemoved = true;
                        disposed();
					}
                }

				Manager.NativeManager.SameLine();

				if (Manager.NativeManager.Button("No", 100))
                {
                    ShouldBeRemoved = true;
					disposed();        
                }
                
                Manager.NativeManager.SameLine();

				if (Manager.NativeManager.Button("Cancel", 100))
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
