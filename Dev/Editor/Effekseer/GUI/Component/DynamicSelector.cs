using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class DynamicSelector
	{
		public static Data.DynamicVector Select(string paramName, string id, Data.DynamicVector currentSelected, bool hasDefault, bool showInvalid)
		{
			var nextSelected = currentSelected;


			var v = Core.Dynamic.Vectors.Values.Select((_, i) => Tuple.Create(_, i)).Where(_ => _.Item1 == currentSelected).FirstOrDefault();

			string selectedID = "";

			if (hasDefault)
			{
				selectedID = "Default###Dynamic";
			}

			if (v != null)
			{
				selectedID = v.Item1.Name.Value + "###DynamicVector" + v.Item2.ToString();
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

				for (int i = 0; i < Core.Dynamic.Vectors.Values.Count; i++)
				{
					if (!showInvalid && !Core.Dynamic.Vectors.Values[i].IsValid)
						continue;

					bool is_selected = (Core.Dynamic.Vectors.Values[i] == currentSelected);

					string name = string.Empty;

					if(Core.Dynamic.Vectors.Values[i].Name.Value == string.Empty)
					{
						name = "(Noname)" + "###DynamicVector" + i.ToString();
					}
					else
					{
						name = Core.Dynamic.Vectors.Values[i].Name.Value + "###DynamicVector" + i.ToString();
					}

					if (Manager.NativeManager.Selectable(name, is_selected, swig.SelectableFlags.None))
					{
						nextSelected = Core.Dynamic.Vectors.Values[i];
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
