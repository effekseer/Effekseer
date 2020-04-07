using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class FCurveButton : Control, IParameterControl
	{
		string id = "";

		object boundObject = null;
		
		public bool EnableUndo { get; set; } = true;

		public FCurveButton()
		{
			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void FixValue()
		{
		}

		public void SetBinding(object o)
		{
			boundObject = o;
		}

		public override void Update()
		{
			
			if(Manager.NativeManager.Button(Resources.GetString("FCurves") + id))
			{
				var panel = Manager.SelectOrShowWindow(typeof(Dock.FCurves)) as Dock.FCurves;
				panel?.UnselectAll();
				panel?.SelectFCurve(boundObject);
			}	
		}
	}
}
