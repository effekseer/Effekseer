using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;

namespace Effekseer.GUI.Widgets
{
	class Widgets
	{

		public static Inspector.InspectorGuiResult GuiBool(object value, Inspector.InspectorGuiState state)
		{
			Inspector.InspectorGuiResult ret = new Inspector.InspectorGuiResult();

			if (value is bool bValue)
			{
				bool[] v = new[] { bValue };
				if (Manager.NativeManager.Checkbox(state.Id, v))
				{
					ret.isEdited = true;
					ret.value = v[0];
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiBool");
			}
			return ret;
		}
		public static Inspector.InspectorGuiResult GuiInt(object value, Inspector.InspectorGuiState state)
		{
			Inspector.InspectorGuiResult ret = new Inspector.InspectorGuiResult();

			if (value is int iValue)
			{
				int[] v = new[] { iValue };
				if (Manager.NativeManager.DragInt(state.Id, v, 1))
				{
					ret.isEdited = true;
					ret.value = v[0];
					return ret;
				}
			}
			else if (value is Asset.Int iStructValue)
			{
				int[] v = new[] { iStructValue.Value };
				if (Manager.NativeManager.DragInt(state.Id, v, 1))
				{
					ret.isEdited = true;
					iStructValue.Value = v[0];
					ret.value = iStructValue;
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiInt");
			}
			return ret;
		}

		public static Inspector.InspectorGuiResult GuiFloat(object value, Inspector.InspectorGuiState state)
		{
			Inspector.InspectorGuiResult ret = new Inspector.InspectorGuiResult();
			if (value is float fValue)
			{
				float[] v = new[] { fValue };
				if (Manager.NativeManager.DragFloat(state.Id, v, .1f))
				{
					ret.isEdited = true;
					ret.value = v[0];
					return ret;
				}
			}
			else if (value is Asset.Float fStructValue)
			{
				float[] v = new[] { fStructValue.Value };
				if (Manager.NativeManager.DragFloat(state.Id, v, .1f))
				{
					ret.isEdited = true;
					fStructValue.Value = v[0];
					ret.value = fStructValue;
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiFloat");
			}
			return ret;
		}

		public static Inspector.InspectorGuiResult GuiFloatWithRange(object value, Inspector.InspectorGuiState state)
		{
			Inspector.InspectorGuiResult ret = new Inspector.InspectorGuiResult();
			Asset.FloatWithRange floatWithRange = value as Asset.FloatWithRange;

			float step = 10f;

			if (floatWithRange != null)
			{
				float[] internalValue;
				string txt_r1 = string.Empty;
				string txt_r2 = string.Empty;


				var range_1_min = float.MinValue;
				var range_1_max = float.MaxValue;
				var range_2_min = float.MinValue;
				var range_2_max = float.MaxValue;

				if (floatWithRange.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
				{
					internalValue = new float[] { floatWithRange.Center, floatWithRange.Amplitude };
					txt_r1 = Resources.GetString("Mean");
					txt_r2 = Resources.GetString("Deviation");

					range_1_min = floatWithRange.Min;
					range_1_max = floatWithRange.Max;
				}
				else
				{
					internalValue = new float[] { floatWithRange.Min, floatWithRange.Max };
					txt_r1 = Resources.GetString("Min");
					txt_r2 = Resources.GetString("Max");

					range_1_min = floatWithRange.Min;
					range_1_max = floatWithRange.Max;
					range_2_min = floatWithRange.Min;
					range_2_max = floatWithRange.Max;
				}

				if (Manager.NativeManager.DragFloat2EfkEx(state.Id, internalValue, step / 10.0f,
				range_1_min, range_1_max,
				range_2_min, range_2_max,
				txt_r1 + ":" + Core.Option.GetFloatFormat(), txt_r2 + ":" + Core.Option.GetFloatFormat()))
				{
					if (floatWithRange.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
					{
						floatWithRange.Center = internalValue[0];
						floatWithRange.Amplitude = internalValue[1];
					}
					else
					{
						floatWithRange.Min = internalValue[0];
						floatWithRange.Max = internalValue[1];
					}


					ret.isEdited = true;
					ret.value = floatWithRange;
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiFloatWithRange");
			}

			return ret;
		}

		public static Inspector.InspectorGuiResult GuiString(object value, Inspector.InspectorGuiState state)
		{
			Inspector.InspectorGuiResult ret = new Inspector.InspectorGuiResult();
			if (value is string sValue)
			{
				bool isMultiline = false;
				var textAreaAttributeWhere = state.Attriubutes.Where(_ => _.GetType() == typeof(Asset.TextAreaAttribute));
				if (textAreaAttributeWhere.Count() > 0 && textAreaAttributeWhere.First() != null)
				{
					isMultiline = true;
				}

				bool isEdited = false;
				if (isMultiline)
				{
					isEdited = Manager.NativeManager.InputTextMultiline(state.Id, sValue);
				}
				else
				{
					isEdited = Manager.NativeManager.InputText(state.Id, sValue);
				}

				if (isEdited)
				{
					ret.isEdited = true;
					ret.value = Manager.NativeManager.GetInputTextResult();
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiString");
			}
			return ret;
		}

		public static Inspector.InspectorGuiResult GuiVector3F(object value, Inspector.InspectorGuiState state)
		{
			Inspector.InspectorGuiResult ret = new Inspector.InspectorGuiResult();

			if (value is Vector3F vec3Value)
			{
				float[] guiValue = new float[] { vec3Value.X, vec3Value.Y, vec3Value.Z };

				if (Manager.NativeManager.DragFloat3EfkEx(state.Id, guiValue, 1.0f,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					"X:" + Core.Option.GetFloatFormat(), "Y:" + Core.Option.GetFloatFormat(), "Z:" + Core.Option.GetFloatFormat()))
				{
					vec3Value.X = guiValue[0];
					vec3Value.Y = guiValue[1];
					vec3Value.Z = guiValue[2];

					ret.isEdited = true;
					ret.value = vec3Value;
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiVector3F");
			}

			return ret;
		}

		public static Inspector.InspectorGuiResult GuiEnum(object value, Inspector.InspectorGuiState state)
		{
			Inspector.InspectorGuiResult ret = new Inspector.InspectorGuiResult();

			var enumType = value.GetType();
			var isEnum = enumType.IsEnum;
			if (!isEnum)
			{
				Manager.NativeManager.Text("Assert Enum");
				return ret;
			}

			// to avoid to change placesGetValues, use  GetFields
			var fieldValues = new List<int>();
			var fields = enumType.GetFields();
			List<object> fieldNames = new List<object>();

			// generate field names
			foreach (var f in fields)
			{
				if (f.FieldType != enumType) continue;

				var attributes = f.GetCustomAttributes(false);

				object name = f.ToString();

				var key = KeyAttribute.GetKey(attributes);
				var nameKey = key + "_Name";
				if (string.IsNullOrEmpty(key))
				{
					nameKey = f.FieldType.ToString() + "_" + f.ToString() + "_Name";
				}

				if (MultiLanguageTextProvider.HasKey(nameKey))
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

				fieldValues.Add((int)f.GetValue(null));
				fieldNames.Add(name);
			}

			// show gui
			int selectedValue = (int)value;
			var v = fieldValues.Select((_, i) => Tuple.Create(_, i)).Where(_ => _.Item1 == selectedValue).FirstOrDefault();
			if (Manager.NativeManager.BeginCombo(state.Id, fieldNames[v.Item2].ToString(), swig.ComboFlags.None))
			{
				for (int i = 0; i < fieldNames.Count; i++)
				{
					bool isSelected = (fieldNames[v.Item2] == fieldNames[i]);

					if (Manager.NativeManager.Selectable(fieldNames[i].ToString(), isSelected, swig.SelectableFlags.None))
					{
						selectedValue = fieldValues[i];

						ret.isEdited = true;
						ret.value = System.Enum.ToObject(enumType, selectedValue);
					}
					if (isSelected)
					{
						Manager.NativeManager.SetItemDefaultFocus();
					}
				}
				Manager.NativeManager.EndCombo();
			}

			return ret;
		}
	}
}