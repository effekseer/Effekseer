using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dialog
{
	class OpenModel : IRemovableControl
	{
		public float Magnification = 1.0f;
		public bool OK = false;

		string title = string.Empty;
		string message = string.Empty;
		string id = "###openModel";

		bool opened = true;

		bool isFirstUpdate = true;

		public Action OnOK = null;
		public Action OnCancel = null;

		public bool ShouldBeRemoved { get; private set; } = false;

		public OpenModel(float mag)
		{
			Magnification = mag;
		}

		public void Show(string title)
		{
			this.title = title;
			Manager.AddControl(this);
		}

		public void Update()
		{
			if (isFirstUpdate)
			{
				Manager.NativeManager.OpenPopup(id);
				isFirstUpdate = false;

				if (Core.Language == Language.Japanese)
				{
					this.message = "拡大";
				}
				else
				{
					this.message = "Scale";
				}
			}

			if (Manager.NativeManager.BeginPopupModal(title + id, ref opened, swig.WindowFlags.AlwaysAutoResize))
			{
				Manager.NativeManager.Text(message);

				var fs = new float[] { Magnification };

				if(Manager.NativeManager.DragFloat("###mag", fs))
				{
					Magnification = fs[0];
				}

				var okText = MultiLanguageTextProvider.GetText("Save");
				var cancelText = MultiLanguageTextProvider.GetText("Cancel");

				if (Manager.NativeManager.Button(okText))
				{
					OK = true;
					ShouldBeRemoved = true;
					if(OnOK != null)
					{
						OnOK();
					}
				}

				if (Manager.NativeManager.Button(cancelText))
				{
					ShouldBeRemoved = true;
					if (OnCancel != null)
					{
						OnCancel();
					}
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
