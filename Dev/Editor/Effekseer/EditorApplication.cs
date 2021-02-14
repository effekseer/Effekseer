using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer
{
	public class App : Application
	{
		protected override void OnUpdate()
		{
			GUI.Manager.Update();
		}

		protected override void OnTerminate()
		{
			GUI.Manager.Terminate();
			Process.MaterialEditor.Terminate();
		}
	}
}
