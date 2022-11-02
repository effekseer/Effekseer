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
		private Dictionary<Type, Func<object, InspectorGuiState, InspectorGuiResult>> FuncDictionary { get; }

		public InspectorGuiDictionary()
		{
			// å^èÓïÒÇ∆GuiÇï\é¶Ç∑ÇÈä÷êîÇïRÇ√ÇØÇÈ
			FuncDictionary = new Dictionary<Type, Func<object, InspectorGuiState, InspectorGuiResult>>
			{
				{ typeof(bool), GuiBool },
				{ typeof(int), GuiInt },
				{ typeof(float), GuiFloat },
				{ typeof(EffectAsset.FloatWithRange), GuiFloatWithRange },
				{ typeof(string), GuiString },
				{ typeof(Vector3F), GuiVector3F },
				{ typeof(System.Enum), GuiEnum },
				{ typeof(EffectAsset.Gradient), GuiGradient },
			};
		}

		public bool HasFunction(Type type)
		{
			return FuncDictionary.ContainsKey(type);
		}

		public Func<object, InspectorGuiState, InspectorGuiResult> GetFunction(Type type)
		{
			return FuncDictionary[type];
		}

		private InspectorGuiResult GuiBool(object value, InspectorGuiState state)
		{
			InspectorGuiResult ret = new InspectorGuiResult();

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
		private InspectorGuiResult GuiInt(object value, InspectorGuiState state)
		{
			InspectorGuiResult ret = new InspectorGuiResult();

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
			else
			{
				Manager.NativeManager.Text("Assert GuiInt");
			}
			return ret;
		}

		private InspectorGuiResult GuiFloat(object value, InspectorGuiState state)
		{
			InspectorGuiResult ret = new InspectorGuiResult();
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
			else
			{
				Manager.NativeManager.Text("Assert GuiFloat");
			}
			return ret;
		}

		private InspectorGuiResult GuiFloatWithRange(object value, InspectorGuiState state)
		{
			InspectorGuiResult ret = new InspectorGuiResult();
			EffectAsset.FloatWithRange floatWithRange = value as EffectAsset.FloatWithRange;

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

		private InspectorGuiResult GuiString(object value, InspectorGuiState state)
		{
			InspectorGuiResult ret = new InspectorGuiResult();
			if (value is string sValue)
			{
				if (Manager.NativeManager.InputText(state.Id, sValue))
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

		private InspectorGuiResult GuiVector3F(object value, InspectorGuiState state)
		{
			InspectorGuiResult ret = new InspectorGuiResult();

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

		private InspectorGuiResult GuiEnum(object value, InspectorGuiState state)
		{
			InspectorGuiResult ret = new InspectorGuiResult();

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

		class GradientState
		{
			public swig.GradientHDRState state = new swig.GradientHDRState();
			public swig.GradientHDRGUIState guiState = new swig.GradientHDRGUIState();
			public swig.GradientHDRGUIState guiPopupState = new swig.GradientHDRGUIState();

			public int id = Manager.GetUniqueID();
			public int id_popup = Manager.GetUniqueID();
			public string id_c = "###" + Manager.GetUniqueID().ToString();
			public string id_color_position = "###" + Manager.GetUniqueID().ToString();
			public string id_color_value = "###" + Manager.GetUniqueID().ToString();
			public string id_color_intensity = "###" + Manager.GetUniqueID().ToString();
			public string id_color_delete = "###" + Manager.GetUniqueID().ToString();
			public string id_alpha_position = "###" + Manager.GetUniqueID().ToString();
			public string id_alpha_value = "###" + Manager.GetUniqueID().ToString();
			public string id_alpha_delete = "###" + Manager.GetUniqueID().ToString();

			public CopyAndPaste<EffectAsset.Gradient> copyAndPaste;
		}

		private InspectorGuiResult GuiGradient(object value, InspectorGuiState state)
		{
			InspectorGuiResult ret = new InspectorGuiResult();
			var isPopupShown = false;

			var actualValue = (EffectAsset.Gradient)value;

			unsafe void CopyStateToNative(swig.GradientHDRState dst, EffectAsset.Gradient src)
			{
				dst.SetColorCount(src.ColorMarkers.Length);
				for (int i = 0; i < src.ColorMarkers.Length; i++)
				{
					var color = new swig.ColorF();
					color.R = src.ColorMarkers[i].ColorR;
					color.G = src.ColorMarkers[i].ColorG;
					color.B = src.ColorMarkers[i].ColorB;

					dst.SetColorMarkerPosition(i, src.ColorMarkers[i].Position);
					dst.SetColorMarkerColor(i, color);
					dst.SetColorMarkerIntensity(i, src.ColorMarkers[i].Intensity);
				}

				dst.SetAlphaCount(src.AlphaMarkers.Length);
				for (int i = 0; i < src.AlphaMarkers.Length; i++)
				{
					dst.SetAlphaMarkerPosition(i, src.AlphaMarkers[i].Position);
					dst.SetAlphaMarkerAlpha(i, src.AlphaMarkers[i].Alpha);
				}
			}

			unsafe void CopyStateFromNative(EffectAsset.Gradient dst, swig.GradientHDRState src)
			{
				dst.ColorMarkers = new EffectAsset.Gradient.ColorMarker[src.GetColorCount()];
				dst.AlphaMarkers = new EffectAsset.Gradient.AlphaMarker[src.GetAlphaCount()];

				for (int i = 0; i < src.GetColorCount(); i++)
				{
					var color = src.GetColorMarkerColor(i);
					dst.ColorMarkers[i].ColorR = color.R;
					dst.ColorMarkers[i].ColorG = color.G;
					dst.ColorMarkers[i].ColorB = color.B;
					dst.ColorMarkers[i].Position = src.GetColorMarkerPosition(i);
					dst.ColorMarkers[i].Intensity = src.GetColorMarkerIntensity(i);
				}

				for (int i = 0; i < src.GetAlphaCount(); i++)
				{
					dst.AlphaMarkers[i].Alpha = src.GetAlphaMarkerAlpha(i);
					dst.AlphaMarkers[i].Position = src.GetAlphaMarkerPosition(i);
				}
			}

			GradientState gradientState = null;
			if (state.UserData != null)
			{
				gradientState = (GradientState)state.UserData;
			}
			else
			{
				gradientState = new GradientState();
				state.UserData = gradientState;
				CopyStateToNative(gradientState.state, actualValue);

				gradientState.copyAndPaste = new CopyAndPaste<EffectAsset.Gradient>(
				() => actualValue,
				(value) => { actualValue = value; });

			}

			void StoreValue()
			{
				CopyStateFromNative(actualValue, gradientState.state);
				ret.value = actualValue;
				ret.isEdited = true;
			}

			void Popup()
			{
				if (isPopupShown) return;

				var windowPos = Manager.NativeManager.GetWindowPos();
				var windowSize = Manager.NativeManager.GetWindowSize();

				// Popup below the last item
				Manager.NativeManager.SetNextWindowPos(new swig.Vec2(windowPos.X, Manager.NativeManager.GetCursorScreenPosY()), swig.Cond.Always, new swig.Vec2(0, 0));
				Manager.NativeManager.SetNextWindowSize(windowSize.X, 0.0f, swig.Cond.Always);

				if (Manager.NativeManager.BeginPopup(gradientState.id_c, swig.WindowFlags.NoMove | swig.WindowFlags.AlwaysAutoResize))
				{
					gradientState.copyAndPaste.Update();

					var contentSize = Manager.NativeManager.GetContentRegionAvail();
					float buttonWidth = 50.0f * Manager.GetUIScaleBasedOnFontSize();
					var itemSpacing = Manager.NativeManager.GetItemSpacing();

					int selectedIndex = gradientState.guiPopupState.GetSelectedIndex();
					bool alphaMarkerSelected = selectedIndex >= 0 && gradientState.guiPopupState.GetSelectedMarkerType() == swig.GradientHDRMarkerType.Alpha;
					bool colorMarkerSelected = selectedIndex >= 0 && gradientState.guiPopupState.GetSelectedMarkerType() == swig.GradientHDRMarkerType.Color;

					{
						// Alpha Edit GUI
						Manager.NativeManager.BeginDisabled(!alphaMarkerSelected);

						Manager.NativeManager.Columns(2);
						Manager.NativeManager.SetColumnWidth(0, contentSize.X * 0.3f);

						Manager.NativeManager.Text("Position");
						Manager.NativeManager.NextColumn();

						float pos = alphaMarkerSelected ? gradientState.state.GetAlphaMarkerPosition(selectedIndex) : 0.0f;
						var posArray = new float[1] { pos * 100.0f };

						Manager.NativeManager.SetNextItemWidth(contentSize.X * 0.7f - buttonWidth - itemSpacing.X);
						if (Manager.NativeManager.DragFloat(gradientState.id_alpha_position, posArray, 1.0f, 0.0f, 100.0f, "%.2f %%"))
						{
							pos = posArray[0] / 100.0f;
							gradientState.state.SetAlphaMarkerPosition(selectedIndex, pos);
						}

						Manager.NativeManager.SameLine();
						if (Manager.NativeManager.Button("Delete" + gradientState.id_alpha_delete, buttonWidth))
						{
							if (gradientState.state.RemoveAlphaMarker(selectedIndex))
							{
								StoreValue();
							}
						}
						Manager.NativeManager.NextColumn();

						Manager.NativeManager.Text("Alpha");
						Manager.NativeManager.NextColumn();

						var alpha = alphaMarkerSelected ? gradientState.state.GetAlphaMarkerAlpha(selectedIndex) : 0.0f;
						var alphaArray = new float[1] { alpha };

						Manager.NativeManager.SetNextItemWidth(-1);
						if (Manager.NativeManager.DragFloat(gradientState.id_alpha_value, alphaArray, 0.01f, 0.0f, 1.0f))
						{
							alpha = alphaArray[0];
							gradientState.state.SetAlphaMarkerAlpha(selectedIndex, alpha);
							StoreValue();
						}
						Manager.NativeManager.NextColumn();

						Manager.NativeManager.Columns(1);
						Manager.NativeManager.EndDisabled();
					}

					if (Manager.NativeManager.GradientHDR(gradientState.id_popup, gradientState.state, gradientState.guiPopupState, true))
					{
						StoreValue();
					}

					{
						// Color Edit GUI
						Manager.NativeManager.BeginDisabled(!colorMarkerSelected);

						Manager.NativeManager.Columns(2);
						Manager.NativeManager.SetColumnWidth(0, contentSize.X * 0.3f);

						Manager.NativeManager.Text("Position");
						Manager.NativeManager.NextColumn();

						float pos = colorMarkerSelected ? gradientState.state.GetColorMarkerPosition(selectedIndex) : 0.0f;
						var posArray = new float[1] { pos * 100.0f };

						Manager.NativeManager.SetNextItemWidth(contentSize.X * 0.7f - buttonWidth - itemSpacing.X);
						if (Manager.NativeManager.DragFloat(gradientState.id_color_position, posArray, 1.0f, 0.0f, 100.0f, "%.2f %%"))
						{
							pos = posArray[0] / 100.0f;
							gradientState.state.SetColorMarkerPosition(selectedIndex, pos);
						}

						Manager.NativeManager.SameLine();
						if (Manager.NativeManager.Button("Delete" + gradientState.id_color_delete, buttonWidth))
						{
							if (gradientState.state.RemoveColorMarker(selectedIndex))
							{
								StoreValue();
							}
						}
						Manager.NativeManager.NextColumn();

						Manager.NativeManager.Text("Color");
						Manager.NativeManager.NextColumn();

						var color = colorMarkerSelected ? gradientState.state.GetColorMarkerColor(selectedIndex) : new swig.ColorF(0.0f, 0.0f, 0.0f, 0.0f);
						var colorArray = new float[] { color.R, color.G, color.B, 1.0f };

						Manager.NativeManager.SetNextItemWidth(-1);
						if (Manager.NativeManager.ColorEdit4(gradientState.id_color_value, colorArray, swig.ColorEditFlags.NoAlpha))
						{
							color.R = colorArray[0];
							color.G = colorArray[1];
							color.B = colorArray[2];
							gradientState.state.SetColorMarkerColor(selectedIndex, color);
							StoreValue();
						}
						Manager.NativeManager.NextColumn();

						Manager.NativeManager.Text("Intensity");
						Manager.NativeManager.NextColumn();

						var intensity = colorMarkerSelected ? gradientState.state.GetColorMarkerIntensity(selectedIndex) : 0.0f;
						var intensityArray = new float[1] { intensity };

						Manager.NativeManager.SetNextItemWidth(-1);
						if (Manager.NativeManager.DragFloat(gradientState.id_color_intensity, intensityArray, 0.01f, 0.0f, float.MaxValue))
						{
							intensity = intensityArray[0];
							gradientState.state.SetColorMarkerIntensity(selectedIndex, intensity);
							StoreValue();
						}
						Manager.NativeManager.NextColumn();

						Manager.NativeManager.Columns(1);
						Manager.NativeManager.EndDisabled();
					}

					Manager.NativeManager.EndPopup();

					isPopupShown = true;
				}
			}


			if (Manager.NativeManager.GradientHDR(gradientState.id, gradientState.state, gradientState.guiState, false))
			{
				StoreValue();
			}

			if (Manager.NativeManager.IsItemClicked(0))
			{
				Manager.NativeManager.OpenPopup(gradientState.id_c);
			}

			Popup();

			return ret;
		}
	}

	public class CopyAndPaste<T>
	{
		Func<T> getter;
		Action<T> setter;

		public CopyAndPaste(Func<T> getter, Action<T> setter)
		{
			this.getter = getter;
			this.setter = setter;
		}

		public void Update()
		{
			float dpiScale = Manager.DpiScale;
			swig.Vec2 size = new swig.Vec2(18 * dpiScale, 18 * dpiScale);

			if (!Images.Icons.ContainsKey("Copy"))
			{
				ErrorUtils.ThrowFileNotfound();
			}

			if (Manager.NativeManager.ImageButton(Images.Icons["Copy"], size.X, size.Y))
			{
				var o = getter();
				if (o != null)
				{
					var data = PartsTreeSystem.JsonSerializer.Serialize(o, Effekseer.CoreContext.Environment);
					Manager.NativeManager.SetClipboardText(data);
				}
			}

			if (Functions.CanShowTip())
			{
				Manager.NativeManager.SetTooltip(Resources.GetString("Panel_Copy_Desc"));
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.ImageButton(Images.Icons["Paste"], size.X, size.Y))
			{
				var str = Manager.NativeManager.GetClipboardText();

				if (!string.IsNullOrEmpty(str))
				{
					var o = getter();
					if (o != null)
					{
						var data = PartsTreeSystem.JsonSerializer.Deserialize<T>(str, Effekseer.CoreContext.Environment);
						setter?.Invoke(data);
					}
				}
			}

			if (Functions.CanShowTip())
			{
				Manager.NativeManager.SetTooltip(Resources.GetString("Panel_Paste_Desc"));
			}
		}
	}
}
