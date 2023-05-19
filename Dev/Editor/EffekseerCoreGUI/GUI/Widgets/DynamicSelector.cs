using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;
using Effekseer.Asset;
using Effekseer.GUI.BindableComponent;

namespace Effekseer.GUI.Widgets
{

	/// <summary>
	/// Parts of widgets for DynamicEquation
	/// </summary>
	public class DynamicSelector
	{
		public static bool Popup(string id, DynamicEquation equation, ref bool enabled)
		{
			if (Manager.NativeManager.RadioButton(Resources.GetString("DynamicFixed") + id + "_1", !enabled))
			{
				enabled = false;
				equation = null;
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.RadioButton(Resources.GetString("DynamicDynamic") + id + "_2", enabled))
			{
				enabled = true;

				return true;
			}

			return false;
		}



		public static bool Popup(string id, DynamicEquation equation1, DynamicEquation equation2, ref bool enabled)
		{
			if (Manager.NativeManager.RadioButton(Resources.GetString("DynamicFixed") + id + "_1", !enabled))
			{
				enabled = false;
				equation1 = null;
				equation2 = null;
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.RadioButton(Resources.GetString("DynamicDynamic") + id + "_2", enabled))
			{
				enabled = true;

				return true;
			}

			return false;
		}

		public static bool EquationsCombo(string paramName, string id, ref int currentIndex, List<Asset.DynamicEquation> equations)
		{
			bool isChanged = false;

			// combo of DynamicEquations
			if (equations.Count <= currentIndex || currentIndex < 0)
			{
				currentIndex = Math.Max(0, equations.Count - 1);
			}
			var selectedEquation = equations.Count > currentIndex ? equations[currentIndex] : null;
			string comboLabel = selectedEquation != null ? selectedEquation.Name : string.Empty;
			if (Manager.NativeManager.BeginCombo(paramName + "###" + id + "Selected", comboLabel, swig.ComboFlags.None))
			{
				int i = 0;
				foreach (var equation in CoreContext.SelectedEffect.Asset.DynamicEquations)
				{
					bool isSelected = ReferenceEquals(equation, selectedEquation);

					string name = string.Empty;
					if (equation.Name != string.Empty)
					{
						name = equation.Name + "###" + id + "ObjName" + i.ToString();
					}
					else
					{
						name = "(Noname)" + "###" + id + "ObjName" + i.ToString();
					}

					if (Manager.NativeManager.Selectable(name, isSelected))
					{
						if (currentIndex != i)
						{
							isChanged = true;
						}
						currentIndex = i;
					}
					if (isSelected)
					{
						Manager.NativeManager.SetItemDefaultFocus();
					}
					++i;
				}

				Manager.NativeManager.EndCombo();
			}

			return isChanged;
		}

		public static void EquationCodeTooltip(DynamicEquation equation)
		{
			if (equation != null)
			{
				Manager.NativeManager.BeginTooltip();
				Manager.NativeManager.Text(equation.Code);
				Manager.NativeManager.EndTooltip();
			}
		}

		public static DynamicEquation SelectMinInComponent(string id, DynamicEquation equation)
		{
			Manager.NativeManager.PushItemWidth(Manager.NativeManager.GetColumnWidth() - 48 * Manager.DpiScale);

			Manager.NativeManager.Text(Resources.GetString("DynamicEq"));
			Manager.NativeManager.SameLine();


			var equations = CoreContext.SelectedEffect.Asset.DynamicEquations;
			int currentIndex = equations.FindIndex(item => ReferenceEquals(item, equation));

			bool isChanged = false;

			isChanged = EquationsCombo(Resources.GetString("Min"), id, ref currentIndex, equations);

			var selectedEquation = equations.Count > currentIndex ? equations[currentIndex] : null;

			if (Manager.NativeManager.IsItemHovered())
			{
				EquationCodeTooltip(selectedEquation);
			}

			Manager.NativeManager.PopItemWidth();

			return isChanged ? selectedEquation : null;
		}

		public static DynamicEquation SelectMaxInComponent(string id, DynamicEquation equation)
		{
			Manager.NativeManager.PushItemWidth(Manager.NativeManager.GetColumnWidth() - 48 * Manager.DpiScale);

			Manager.NativeManager.Text(Resources.GetString("DynamicEq"));
			Manager.NativeManager.SameLine();

			var equations = CoreContext.SelectedEffect.Asset.DynamicEquations;
			int currentIndex = equations.FindIndex(item => ReferenceEquals(item, equation));

			bool isChanged = false;

			isChanged = EquationsCombo(Resources.GetString("Max"), id, ref currentIndex, equations);

			var selectedEquation = equations.Count > currentIndex ? equations[currentIndex] : null;

			if (Manager.NativeManager.IsItemHovered())
			{
				EquationCodeTooltip(selectedEquation);
			}

			Manager.NativeManager.PopItemWidth();
			
			return isChanged ? selectedEquation : null;
		}
	}
}
