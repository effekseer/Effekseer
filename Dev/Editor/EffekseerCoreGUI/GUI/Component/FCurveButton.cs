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
			id = "###" + GUIManager.GetUniqueID().ToString();
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
			
			if(GUIManager.NativeManager.Button(Resources.GetString("FCurves") + id))
			{
				var windowSize = GUIManager.NativeManager.GetWindowSize();
				var panel = GUIManager.SelectOrShowWindow(typeof(Dock.FCurves), new swig.Vec2(windowSize.X *0.75f, windowSize.Y * 0.5f)) as Dock.FCurves;
				panel?.UnselectAll();
				panel?.SelectFCurve(boundObject);
			}	
		}
	}
}
