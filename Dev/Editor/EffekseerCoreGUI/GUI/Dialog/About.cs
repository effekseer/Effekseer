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

[Official Website](http://effekseer.github.io/)

[Github](https://github.com/effekseer/Effekseer)

";

        public bool ShouldBeRemoved { get; private set; } = false;

        public void Show()
        {
            versionInfo = "Effekseer Version " + Core.Version;
			title = Resources.GetString("InternalAbout");

			GUIManager.AddControl(this);
        }

        public void Update()
        {
			if (isFirstUpdate)
			{
				GUIManager.NativeManager.OpenPopup(id);
				GUIManager.NativeManager.SetNextWindowSize(600 * GUIManager.DpiScale, 400 * GUIManager.DpiScale, swig.Cond.Appearing);
				isFirstUpdate = false;
			}

			if (GUIManager.NativeManager.BeginPopupModal(title + id, ref opened, swig.WindowFlags.None))
            {
				const float iconSize = 64;

				GUIManager.NativeManager.Image(Images.GetIcon("AppIcon"), iconSize, iconSize);

				GUIManager.NativeManager.SameLine();

				GUIManager.NativeManager.SetCursorPosY(GUIManager.NativeManager.GetCursorPosY() + iconSize / 2 - GUIManager.NativeManager.GetTextLineHeight() / 2);
				GUIManager.NativeManager.Text(versionInfo);

				GUIManager.NativeManager.Separator();

				GUIManager.NativeManager.Markdown(license);

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
