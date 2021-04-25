using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	public class Enum : Control, IParameterControl
	{
		string id = "";

		/// <summary>
		/// A label without parameter list
		/// </summary>
		internal string InternalLabel = string.Empty;

		Data.Value.EnumBase binding = null;

		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		int[] enums = null;

		public List<object> FieldNames = new List<object>();
		
		public bool EnableUndo { get; set; } = true;

		int selectedValues = -1;
		string preview_value = string.Empty;
		bool isInitialized = false;

		public Data.Value.EnumBase Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value) return;

				binding = value;

				if (binding != null)
				{
					selectedValues = binding.GetValueAsInt();
				}
			}
		}

		public void Initialize(Type enumType)
		{
			if (isInitialized) return;
			isInitialized = true;

			// to avoid to change placesGetValues, use  GetFields
			var list = new List<int>();
			var fields = enumType.GetFields();
			//var iconBitmaps = new List<Bitmap>();
			//bool hasIcon = false;

			foreach (var f in fields)
			{
				if (f.FieldType != enumType) continue;

				var attributes = f.GetCustomAttributes(false);

				object name = f.ToString();

				
				 var key = KeyAttribute.GetKey(attributes);
				var nameKey = key + "_Name";
				if(string.IsNullOrEmpty(key))
				{
					nameKey = f.FieldType.ToString() + "_" + f.ToString() + "_Name";
				}

				if(MultiLanguageTextProvider.HasKey(nameKey))
				{
					name = new MultiLanguageString(nameKey);
				}
				else
				{
					name = NameAttribute.GetName(attributes);
					if (name.ToString() == string.Empty)
					{
						name = f.ToString();
					}
				}

				var iconAttribute = IconAttribute.GetIcon(attributes);
				if (iconAttribute != null)
				{
					name = iconAttribute.code + name;
				}

				list.Add((int)f.GetValue(null));
				FieldNames.Add(name);
			}
			enums = list.ToArray();
		}

		public Enum()
		{
			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.EnumBase;
			Binding = o_;
		}

		public void FixValue()
		{
		}

		public override void Update()
		{
			if (binding != null)
			{
				selectedValues = binding.GetValueAsInt();
			}
			else
			{
				selectedValues = -1;
			}

			if (EnableUndo)
			{
				valueChangingProp.Enable(binding);
			}

			var v = enums.Select((_, i) => Tuple.Create(_, i)).Where(_ => _.Item1 == selectedValues).FirstOrDefault();

			if(Manager.NativeManager.BeginCombo(InternalLabel + id, FieldNames[v.Item2].ToString(), swig.ComboFlags.None))
			{
				for(int i = 0; i < FieldNames.Count; i++)
				{
					bool is_selected = (FieldNames[v.Item2] == FieldNames[i]);

					if (Manager.NativeManager.Selectable(FieldNames[i].ToString(), is_selected, swig.SelectableFlags.None))
					{
						selectedValues = enums[i];

						if (EnableUndo)
						{
							binding.SetValue(selectedValues);
						}
						else
						{
							binding.SetValueDirectly(selectedValues);
						}
					}
						
					if (is_selected)
					{
						Manager.NativeManager.SetItemDefaultFocus();
					}

				}

				Manager.NativeManager.EndCombo();
			}

			if (EnableUndo)
			{
				valueChangingProp.Disable();
			}
		}
	}
}
