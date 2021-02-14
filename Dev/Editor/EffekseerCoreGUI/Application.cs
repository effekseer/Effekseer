using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer
{
	public abstract class Application
	{
		protected virtual void OnUpdate()
		{
		}

		protected virtual void OnTerminate()
		{
		}

		public void Run()
		{
			while (GUI.GUIManager.NativeManager.DoEvents())
			{
				OnUpdate();
			}
		}
	}
}
