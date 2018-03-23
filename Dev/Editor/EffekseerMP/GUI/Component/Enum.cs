using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class Enum : Control, IParameterControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		Data.Value.EnumBase binding = null;

		int[] enums = null;

		List<string> names = new List<string>();

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
				var name = NameAttribute.GetName(attributes);
				if (name == string.Empty)
				{
					name = f.ToString();
				}

				//Bitmap icon = null;
				//var iconAttribute = IconAttribute.GetIcon(attributes);
				//if (iconAttribute != null)
				//{
				//	icon = (Bitmap)Properties.Resources.ResourceManager.GetObject(iconAttribute.resourceName);
				//	hasIcon = true;
				//}
				//
				//Items.Add(name);
				list.Add((int)f.GetValue(null));
				names.Add(name);
				//iconBitmaps.Add(icon);
			}
			enums = list.ToArray();

			//if (hasIcon)
			//{
			//	// アイコンが存在するときはカスタム描画に切り替える
			//	icons = iconBitmaps.ToArray();
			//	DrawMode = DrawMode.OwnerDrawFixed;
			//	DrawItem += new DrawItemEventHandler(Enum_OnDrawItem);
			//	ItemHeight += icons[0].Height / 2;
			//}
		}

		public Enum(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

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

			var v = enums.Select((_, i) => Tuple.Create(_, i)).Where(_ => _.Item1 == selectedValues).FirstOrDefault();

			if(Manager.NativeManager.BeginCombo(id, names[v.Item2], swig.ComboFlags.None))
			{
				for(int i = 0; i < names.Count; i++)
				{
					bool is_selected = (names[v.Item2] == names[i]);
					if (Manager.NativeManager.Selectable(names[i], is_selected))
					{
						selectedValues = enums[i];
						binding.SetValue(selectedValues);
					}
						
					if (is_selected)
					{
						Manager.NativeManager.SetItemDefaultFocus();
					}

				}

				Manager.NativeManager.EndCombo();
			}

			Console.WriteLine("Not implemented.");
		}
	}
}
