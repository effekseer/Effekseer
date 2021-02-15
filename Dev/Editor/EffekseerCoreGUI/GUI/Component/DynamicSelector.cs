using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class DynamicSelector
	{
		public static void Popup(string id, Data.Value.DynamicEquationReference reference, Data.Value.Boolean enabled)
		{
			if (GUIManager.NativeManager.RadioButton(Resources.GetString("DynamicFixed") + id + "_1", !enabled))
			{
				enabled.SetValue(false);
				reference.SetValue(null);
			}

			GUIManager.NativeManager.SameLine();

			if (GUIManager.NativeManager.RadioButton(Resources.GetString("DynamicDynamic") + id + "_2", enabled))
			{
				enabled.SetValue(true);
			}
		}

		public static void Popup(string id, Data.Value.DynamicEquationReference reference1, Data.Value.DynamicEquationReference reference2, Data.Value.Boolean enabled)
		{
			if (GUIManager.NativeManager.RadioButton(Resources.GetString("DynamicFixed") + id + "_1", !enabled))
			{
				enabled.SetValue(false);
				reference1.SetValue(null);
				reference2.SetValue(null);
			}

			GUIManager.NativeManager.SameLine();

			if (GUIManager.NativeManager.RadioButton(Resources.GetString("DynamicDynamic") + id + "_2", enabled))
			{
				enabled.SetValue(true);
			}
		}

		public static void SelectInComponent(string id, Data.Value.DynamicEquationReference reference)
		{
			GUIManager.NativeManager.Text(Resources.GetString("DynamicEq"));
			GUIManager.NativeManager.SameLine();

			var nextParam = ObjectCollection.Select("", id, reference.Value, false, Core.Dynamic.Equations);

			if (GUIManager.NativeManager.IsItemHovered())
			{
				DynamicEquationCodeTooltip.Render(reference);
			}

			if (reference.Value != nextParam)
			{
				reference.SetValue(nextParam);
			}
		}

		public static void SelectMaxInComponent(string id, Data.Value.DynamicEquationReference reference)
		{
			GUIManager.NativeManager.PushItemWidth(GUIManager.NativeManager.GetColumnWidth() - 48 * GUIManager.DpiScale);

			GUIManager.NativeManager.Text(Resources.GetString("DynamicEq"));
			GUIManager.NativeManager.SameLine();

			var nextParam = ObjectCollection.Select(Resources.GetString("Max") , id, reference.Value, false, Core.Dynamic.Equations);

			if (GUIManager.NativeManager.IsItemHovered())
			{
				DynamicEquationCodeTooltip.Render(reference);
			}

			if (reference.Value != nextParam)
			{
				reference.SetValue(nextParam);
			}

			GUIManager.NativeManager.PopItemWidth(); ;
		}

		public static void SelectMinInComponent(string id, Data.Value.DynamicEquationReference reference)
		{
			GUIManager.NativeManager.PushItemWidth(GUIManager.NativeManager.GetColumnWidth() - 48 * GUIManager.DpiScale);

			GUIManager.NativeManager.Text(Resources.GetString("DynamicEq"));
			GUIManager.NativeManager.SameLine();

			var nextParam = ObjectCollection.Select(Resources.GetString("Min"), id, reference.Value, false,  Core.Dynamic.Equations);

			if (GUIManager.NativeManager.IsItemHovered())
			{
				DynamicEquationCodeTooltip.Render(reference);
			}

			if (reference.Value != nextParam)
			{
				reference.SetValue(nextParam);
			}

			GUIManager.NativeManager.PopItemWidth(); ;
		}
	}
}
