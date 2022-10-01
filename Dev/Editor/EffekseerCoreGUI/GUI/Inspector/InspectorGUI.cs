using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;
using Effekseer.GUI.BindableComponent;

namespace Effekseer.GUI.Inspector
{
	class InspectorGuiResult
	{
		public bool isEdited;
		public object value;
	}

	// å^èÓïÒÇ∆GuiÇï\é¶Ç∑ÇÈä÷êîÇïRÇ√ÇØÇÈÉNÉâÉX
	class InspectorGuiDictionary
	{
		private Dictionary<Type, Func<object, string, InspectorGuiResult>> FuncDictionary { get; }

		public InspectorGuiDictionary()
		{
			// å^èÓïÒÇ∆GuiÇï\é¶Ç∑ÇÈä÷êîÇïRÇ√ÇØÇÈ
			FuncDictionary = new Dictionary<Type, Func<object, string, InspectorGuiResult>>
			{
				{ typeof(int), GuiInt },
				{ typeof(float), GuiFloat },
				{ typeof(string), GuiString },
				{ typeof(Vector3D), GuiVector3D },
			};
		}

		public bool HasFunction(Type type)
		{
			return FuncDictionary.ContainsKey(type);
		}

		public Func<object, string, InspectorGuiResult> GetFunction(Type type)
		{
			return FuncDictionary[type];
		}

		private InspectorGuiResult GuiInt(object value, string label)
		{
			InspectorGuiResult ret = new InspectorGuiResult();

			if (value is int iValue)
			{
				int[] v = new[] { iValue };
				if (Manager.NativeManager.DragInt(label, v, 1))
				{
					ret.isEdited = true;
					ret.value = v[0];
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiInt");
			}
			return ret;
		}

		private InspectorGuiResult GuiFloat(object value, string label)
		{
			InspectorGuiResult ret = new InspectorGuiResult();
			if (value is float fValue)
			{
				float[] v = new[] { fValue };
				if (Manager.NativeManager.DragFloat(label, v, .1f))
				{
					ret.isEdited = true;
					ret.value = v[0];
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiFloat");
			}
			return ret;
		}
		private InspectorGuiResult GuiString(object value, string label)
		{
			InspectorGuiResult ret = new InspectorGuiResult();
			if (value is string sValue)
			{
				if (Manager.NativeManager.InputText(label, sValue))
				{
					ret.isEdited = true;
					ret.value = sValue;
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiString");
			}
			return ret;
		}

		private InspectorGuiResult GuiVector3D(object value, string label)
		{
			InspectorGuiResult ret = new InspectorGuiResult();

			if (value is Vector3D vec3Value)
			{
				FieldInfo fieldInternalValue = vec3Value.GetType().GetField("internalValue", BindingFlags.NonPublic | BindingFlags.Instance);
				float[] internalValue = (float[])fieldInternalValue.GetValue(vec3Value);

				if (Manager.NativeManager.DragFloat3EfkEx(label, internalValue, 1.0f,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					"X:" + Core.Option.GetFloatFormat(), "Y:" + Core.Option.GetFloatFormat(), "Z:" + Core.Option.GetFloatFormat()))
				{
					ret.isEdited = true;
					ret.value = vec3Value;
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiVector3D");
			}

			return ret;
		}
	}
}
