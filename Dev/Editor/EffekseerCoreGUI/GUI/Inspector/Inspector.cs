using System;
using System.Reflection;
using System.IO;

namespace Effekseer.GUI.Inspector
{
	public class InspectorPanel : Dock.DockPanel
	{
		object target;

		public InspectorPanel()
		{
			Label = "Inspector###Inspector";
			target = new TestData();
		}

		protected override void UpdateInternal()
		{
			Inspector.Update(target);
		}

		struct TestData
		{
			public int Int1;
			public int Int2;
		}
	}

	class Inspector
	{
		public static void Update(object target)
		{
			var fields = target.GetType().GetFields();

			foreach (var field in fields)
			{
				var value = field.GetValue(target);

				if (value is int i)
				{
					var iarray = new[] { i };

					if (Manager.NativeManager.DragInt(field.Name, iarray, 1))
					{
						field.SetValue(target, iarray[0]);
					}
				}
			}
		}
	}
}
