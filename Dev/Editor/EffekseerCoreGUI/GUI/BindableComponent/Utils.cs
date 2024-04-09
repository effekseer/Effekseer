using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.BindableComponent
{
	public class ObjectCollection
	{
		public static T Select<T>(string paramName, string id, T currentSelected, bool hasDefault, Data.Value.ObjectCollection<T> collection) where T : class, Data.Value.INamedObject, new()
		{
			var nextSelected = currentSelected;

			var v = collection.Values.Select((_, i) => Tuple.Create(_, i)).Where(_ => _.Item1 == currentSelected).FirstOrDefault();

			string selectedID = "";

			if (hasDefault)
			{
				selectedID = "Default###Selected";
			}

			if (v != null)
			{
				selectedID = v.Item1.Name.Value + "###SelectedObj" + v.Item2.ToString();
			}

			if (Manager.NativeManager.BeginCombo(paramName + "###Selected" + id, selectedID, swig.ComboFlags.None))
			{

				if (hasDefault)
				{
					bool is_selected = v == null;

					var name = "Default###Selected";

					if (Manager.NativeManager.Selectable(name, is_selected, swig.SelectableFlags.None))
					{
						nextSelected = null;
					}

					if (is_selected)
					{
						Manager.NativeManager.SetItemDefaultFocus();
					}
				}

				for (int i = 0; i < collection.Values.Count; i++)
				{
					bool is_selected = (collection.Values[i] == currentSelected);

					string name = string.Empty;

					if (collection.Values[i].Name.Value == string.Empty)
					{
						name = "(Noname)" + "###ObjName" + i.ToString();
					}
					else
					{
						name = collection.Values[i].Name.Value + "###ObjName" + i.ToString();
					}

					if (Manager.NativeManager.Selectable(name, is_selected, swig.SelectableFlags.None))
					{
						nextSelected = collection.Values[i];
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

	public class Functions
	{
		public static string GetFilepathWithExtentions(string filepath, string extention)
		{
			if (System.IO.Path.GetExtension(filepath) != extention)
			{
				filepath += extention;
			}

			return filepath;
		}

		public static bool CanShowTip()
		{
			return Manager.NativeManager.IsItemHovered() && Manager.NativeManager.GetHoveredIDTimer() > 0.25f;
		}

		public static void ShowReset(Data.IResettableValue value, string id)
		{
			if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("ResetParam_Name") + id))
			{
				value.ResetValue();
			}

			if (CanShowTip())
			{
				Manager.NativeManager.SetTooltip(MultiLanguageTextProvider.GetText("ResetParam_Desc"));
			}
		}

	}
	public class CopyAndPaste : IControl
	{
		string elementName = string.Empty;
		Func<object> getter;
		Action valueChanged;

		public CopyAndPaste(string elementName, Func<object> getter, Action valueChanged)
		{
			this.elementName = elementName;
			this.getter = getter;
			this.valueChanged = valueChanged;
		}

		public void Update()
		{
			float dpiScale = Manager.DpiScale;
			swig.Vec2 size = new swig.Vec2(20 * dpiScale, 20 * dpiScale);

			if (!Images.Icons.ContainsKey("Copy"))
			{
				ErrorUtils.ThrowFileNotfound();
			}

			if (Manager.NativeManager.IconButton(Icons.Copy))
			{
				var o = getter();
				if (o != null)
				{
					Manager.NativeManager.SetClipboardText(Core.Copy(elementName, o));
				}
			}

			if (Functions.CanShowTip())
			{
				Manager.NativeManager.SetTooltip(MultiLanguageTextProvider.GetText("Panel_Copy_Desc"));
			}

			if (Manager.NativeManager.IconButton(Icons.Paste))
			{
				var str = Manager.NativeManager.GetClipboardText();

				if (!string.IsNullOrEmpty(str))
				{
					var o = getter();
					if (o != null)
					{
						Core.Paste(elementName, o, str);
						valueChanged?.Invoke();
					}
				}
			}

			if (Functions.CanShowTip())
			{
				Manager.NativeManager.SetTooltip(MultiLanguageTextProvider.GetText("Panel_Paste_Desc"));
			}
		}
	}
	class ValueChangingProperty : IDisposable
	{
		bool flag = false;

		public void Enable(dynamic value)
		{
			if (Manager.DoesChangeColorOnChangedValue && (value?.IsValueChangedFromDefault ?? false))
			{
				flag = true;
				Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.Border, 0x77ffff11);
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
		public static void Render(Data.Value.DynamicEquationReference dynamicEquation)
		{
			if (dynamicEquation.Index != -1)
			{
				Manager.NativeManager.BeginTooltip();
				Manager.NativeManager.Text(dynamicEquation.Value.Code.Value);
				Manager.NativeManager.EndTooltip();
			}
		}
	}

	class LanguageSelector : Control, IParameterControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

		Data.LanguageSelector binding = null;

		bool isActive = false;

		public List<string> FieldNames = new List<string>();

		public bool EnableUndo { get; set; } = true;

		public Data.LanguageSelector Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value) return;

				binding = value;
			}
		}

		public LanguageSelector()
		{
			id = "###" + Manager.GetUniqueID().ToString();

			FieldNames.AddRange(LanguageTable.Languages.Select(_ => MultiLanguageTextProvider.GetText("Language_" + _)));
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.LanguageSelector;
			Binding = o_;

			if (Binding != null)
			{

			}
		}

		public void FixValue()
		{
			if (binding == null) return;
		}

		public override void OnDisposed()
		{
			FixValue();
		}

		public override void Update()
		{
			if (binding != null)
			{
			}

			if (Manager.NativeManager.BeginCombo(id, FieldNames[LanguageTable.SelectedIndex], swig.ComboFlags.None, null))
			{
				for (int i = 0; i < FieldNames.Count; i++)
				{
					bool is_selected = (FieldNames[LanguageTable.SelectedIndex] == FieldNames[i]);

					if (Manager.NativeManager.Selectable(FieldNames[i], is_selected, swig.SelectableFlags.None))
					{
						LanguageTable.SelectLanguage(i);
					}

					if (is_selected)
					{
						Manager.NativeManager.SetItemDefaultFocus();
					}

				}

				Manager.NativeManager.EndCombo();
			}


			var isActive_Current = Manager.NativeManager.IsItemActive();

			if (isActive && !isActive_Current)
			{
				FixValue();
			}

			isActive = isActive_Current;
		}
	}
}