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
			message = string.Format(format, System.IO.Path.GetFileName(Core.Root.GetFullPath()));

			this.disposed = disposed;

			// if already show window, don't add control
			if(GUIManager.Controls.Internal.Any(_=>_ is SaveOnDisposing))
			{
				return;
			}

			GUIManager.AddControl(this);
		}

		public void Update()
		{
			float buttonSizeX = 100 * GUIManager.DpiScale;

			if (isFirstUpdate)
            {
				GUIManager.NativeManager.OpenPopup(id);
                isFirstUpdate = false;
            }

            if (GUIManager.NativeManager.BeginPopupModal(title + id, ref opened, swig.WindowFlags.AlwaysAutoResize))
            {
				GUIManager.NativeManager.Text(message);

				GUIManager.NativeManager.Separator();

                if (GUIManager.NativeManager.Button("Yes", buttonSizeX))
				{
					if(Commands.Overwrite())
					{
						ShouldBeRemoved = true;
                        disposed();
					}
                }

				GUIManager.NativeManager.SameLine();

				if (GUIManager.NativeManager.Button("No", buttonSizeX))
                {
                    ShouldBeRemoved = true;
					disposed();        
                }

				GUIManager.NativeManager.SameLine();

				if (GUIManager.NativeManager.Button("Cancel", buttonSizeX))
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
