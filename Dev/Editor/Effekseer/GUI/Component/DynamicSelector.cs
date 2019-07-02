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
			if (Manager.NativeManager.RadioButton(Resources.GetString("DynamicFixed") + id + "_1", !enabled))
			{
				enabled.SetValue(false);
				reference.SetValue(null);
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.RadioButton(Resources.GetString("DynamicDynamic") + id + "_2", enabled))
			{
				enabled.SetValue(true);
			}
		}

		public static void Popup(string id, Data.Value.DynamicEquationReference reference1, Data.Value.DynamicEquationReference reference2, Data.Value.Boolean enabled)
		{
			if (Manager.NativeManager.RadioButton(Resources.GetString("DynamicFixed") + id + "_1", !enabled))
			{
				enabled.SetValue(false);
				reference1.SetValue(null);
				reference2.SetValue(null);
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.RadioButton(Resources.GetString("DynamicDynamic") + id + "_2", enabled))
			{
				enabled.SetValue(true);
			}
		}

		public static void SelectInComponent(string id, Data.Value.DynamicEquationReference reference)
		{
			Manager.NativeManager.Text(Resources.GetString("DynamicEq"));
			Manager.NativeManager.SameLine();

			var nextParam = DynamicSelector.Select("", id, reference.Value, false, false);

			if (reference.Value != nextParam)
			{
				reference.SetValue(nextParam);
			}
		}

		public static void SelectMaxInComponent(string id, Data.Value.DynamicEquationReference reference)
		{
			Manager.NativeManager.PushItemWidth(Manager.NativeManager.GetColumnWidth() - 60);

			Manager.NativeManager.Text(Resources.GetString("DynamicEq"));
			Manager.NativeManager.SameLine();

			var nextParam = DynamicSelector.Select(Resources.GetString("Max") , id, reference.Value, false, false);

			if (reference.Value != nextParam)
			{
				reference.SetValue(nextParam);
			}

			Manager.NativeManager.PopItemWidth(); ;
		}

		public static void SelectMinInComponent(string id, Data.Value.DynamicEquationReference reference)
		{
			Manager.NativeManager.PushItemWidth(Manager.NativeManager.GetColumnWidth() - 60);

			Manager.NativeManager.Text(Resources.GetString("DynamicEq"));
			Manager.NativeManager.SameLine();

			var nextParam = DynamicSelector.Select(Resources.GetString("Min"), id, reference.Value, false, false);

			if (reference.Value != nextParam)
			{
				reference.SetValue(nextParam);
			}

			Manager.NativeManager.PopItemWidth(); ;
		}
		public static Data.DynamicEquation Select(string paramName, string id, Data.DynamicEquation currentSelected, bool hasDefault, bool showInvalid)
		{
			var nextSelected = currentSelected;


			var v = Core.Dynamic.Equations.Values.Select((_, i) => Tuple.Create(_, i)).Where(_ => _.Item1 == currentSelected).FirstOrDefault();

			string selectedID = "";

			if (hasDefault)
			{
				selectedID = "Default###Dynamic";
			}

			if (v != null)
			{
				selectedID = v.Item1.Name.Value + "###DynamicEquation" + v.Item2.ToString();
			}

			if (Manager.NativeManager.BeginCombo(paramName + "###Dynamic" + id, selectedID, swig.ComboFlags.None))
			{

				if (hasDefault)
				{
					bool is_selected = v == null;

					var name = "Default###Dynamic";

					if (Manager.NativeManager.Selectable(name, is_selected, swig.SelectableFlags.None))
					{
						nextSelected = null;
					}

					if (is_selected)
					{
						Manager.NativeManager.SetItemDefaultFocus();
					}
				}

				for (int i = 0; i < Core.Dynamic.Equations.Values.Count; i++)
				{
					if (!showInvalid && !Core.Dynamic.Equations.Values[i].IsValid)
						continue;

					bool is_selected = (Core.Dynamic.Equations.Values[i] == currentSelected);

					string name = string.Empty;

					if(Core.Dynamic.Equations.Values[i].Name.Value == string.Empty)
					{
						name = "(Noname)" + "###DynamicEquation" + i.ToString();
					}
					else
					{
						name = Core.Dynamic.Equations.Values[i].Name.Value + "###DynamicEquation" + i.ToString();
					}

					if (Manager.NativeManager.Selectable(name, is_selected, swig.SelectableFlags.None))
					{
						nextSelected = Core.Dynamic.Equations.Values[i];
					}

					if (is_selected)
					{
						Manager.NativeManager.SetItemDefaultFocus();
					}
				}

				Manager.NativeManager.EndCombo();
			}

			return nextSelected;
		}
	}
}
