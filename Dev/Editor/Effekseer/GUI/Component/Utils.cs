using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class ValueChangingProperty : IDisposable
	{
		bool flag = false;

		public void Enable(dynamic value)
		{
			if (Manager.DoesChangeColorOnChangedValue && (value?.IsValueChangedFromDefault ?? false))
			{
				flag = true;
				Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.Border, 0xff1111ff);
				Manager.NativeManager.PushStyleVar(swig.ImGuiStyleVarFlags.FrameBorderSize, 1);
			}
		}

		public void Disable()
		{
			if (flag)
			{
				Manager.NativeManager.PopStyleColor();
				Manager.NativeManager.PopStyleVar();
			}
			flag = false;
		}

		public void Dispose()
		{
			Disable();
		}
	}

	class DynamicEquationCodeTooltip
	{
		public static void render(Data.Value.DynamicEquationReference dynamicEquation)
		{
			if (dynamicEquation.Index != -1)
			{
				Manager.NativeManager.BeginTooltip();
				Manager.NativeManager.Text(dynamicEquation.Value.Code.Value);
				Manager.NativeManager.EndTooltip();
			}
		}
 	}
}
