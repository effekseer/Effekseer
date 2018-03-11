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

		string license = @"
The MIT License

Copyright (c) 2011-2015 Effekseer Project 

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

------------------------------------------------------------
Dock Panel Suite

 The MIT License

 Copyright (c) 2007 Weifen Luo (email: weifenluo @yahoo.com) 

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the ""Software""), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. 
 THE SOFTWARE IS PROVIDED ""AS IS"", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
";

        public bool ShouldBeRemoved { get; private set; } = false;

        public void Show()
        {
            versionInfo = "Effekseer Version " + Core.Version;
			title = Resources.GetString("InternalAbout");

			Manager.AddControl(this);
        }

        public void Update()
        {
			if (isFirstUpdate)
			{
				Manager.NativeManager.OpenPopup(id);
				Manager.NativeManager.SetNextWindowSize(600, 400, swig.Cond.Appearing);
				isFirstUpdate = false;
			}

			if (Manager.NativeManager.BeginPopupModal(title + id, null, swig.WindowFlags.None))
            {
				Manager.NativeManager.Image(Images.Icon, 32, 32);

				Manager.NativeManager.SameLine();

                Manager.NativeManager.Text(versionInfo);

                Manager.NativeManager.TextWrapped(license);

                if(Manager.NativeManager.Button("http://effekseer.github.io/"))
                {
                    System.Diagnostics.Process.Start("http://effekseer.github.io/");
                }

                if (Manager.NativeManager.Button("https://github.com/effekseer/Effekseer"))
                {
                    System.Diagnostics.Process.Start("https://github.com/effekseer/Effekseer");
                }

                if (Manager.NativeManager.Button("OK"))
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
