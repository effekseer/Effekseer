using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dialog
{
	class About : IRemovableControl
	{
		string id = "###Abount";
		string title = "About";
		string versionInfo = "";

		bool isFirstUpdate = true;

		bool opened = true;

		string license = @"
The MIT License

Copyright (c) 2011-2019 Effekseer Project 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the ""Software""), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED ""AS IS"", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

- [Official Website](http://effekseer.github.io/)

- [GitHub](https://github.com/effekseer/Effekseer)

- Partial icons by [Icons8](https://icons8.com)

";

		public bool ShouldBeRemoved { get; private set; } = false;

		public void Show()
		{
			versionInfo = "Effekseer Version " + Core.Version;
			title = MultiLanguageTextProvider.GetText("InternalAbout");

			Manager.AddControl(this);
		}

		public void Update()
		{
			if (isFirstUpdate)
			{
				Manager.NativeManager.OpenPopup(id);
				Manager.NativeManager.SetNextWindowSize(600 * Manager.DpiScale, 400 * Manager.DpiScale, swig.Cond.Appearing);
				isFirstUpdate = false;
			}

			if (Manager.NativeManager.BeginPopupModal(title + id, ref opened, swig.WindowFlags.None))
			{
				const float iconSize = 64;

				Manager.NativeManager.ImageData(Images.GetIcon("AppIcon"), iconSize, iconSize);

				Manager.NativeManager.SameLine();

				Manager.NativeManager.SetCursorPosY(Manager.NativeManager.GetCursorPosY() + iconSize / 2 - Manager.NativeManager.GetTextLineHeight() / 2);
				Manager.NativeManager.Text(versionInfo);

				Manager.NativeManager.Separator();

				Manager.NativeManager.Markdown(license);

				Manager.NativeManager.Separator();

				Manager.NativeManager.SetCursorPosX(Manager.NativeManager.GetContentRegionAvail().X / 2 - 100 / 2);

				if (Manager.NativeManager.Button("OK", 100))
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