using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;
using Effekseer.Asset;
using Effekseer.GUI.BindableComponent;
using Effekseer.Data;

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
			else if (value is Asset.Int intWithDynamicValue)
			{
				bool isEdited = false;
				int[] v = new[] { intWithDynamicValue.Value };

				bool isPopupShown = false;

				var canSelectDynamicEquationAttribute = state.Attriubutes.Where(
					_ => _.GetType() == typeof(Asset.CanSelectDynamicEquationAttribute)
					).FirstOrDefault() as Asset.CanSelectDynamicEquationAttribute;
				bool canSelectDynamicEquation = canSelectDynamicEquationAttribute != null ? canSelectDynamicEquationAttribute.CanSelect : false;


				// Action that show popup
				Action popup = () => {
					if (isPopupShown) return;

					if (Manager.NativeManager.BeginPopupContextItem(state.Id))
					{
						if (canSelectDynamicEquation)
						{
							if (DynamicSelector.Popup(
								state.Id,
								intWithDynamicValue.DynamicEquation,
								ref intWithDynamicValue.IsDynamicEquationEnabled))
							{
								isEdited = true;
							}
						}

						Manager.NativeManager.EndPopup();

						isPopupShown = true;
					}
				};


				if (Manager.NativeManager.DragInt(state.Id, v, 1))
				{
					intWithDynamicValue.Value = v[0];
					isEdited = true;
				}
				popup();

				if (canSelectDynamicEquation && intWithDynamicValue.IsDynamicEquationEnabled)
				{
					var equation = DynamicSelector.SelectInComponent(state.Id + "_Equation", intWithDynamicValue.DynamicEquation);
					if (equation != null)
					{
						intWithDynamicValue.DynamicEquation = equation;
						isEdited = true;
					}
					popup();
				}

				if (isEdited)
				{
					ret.isEdited = true;
					ret.value = intWithDynamicValue;
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiInt");
			}
			return ret;
		}


		public static Inspector.InspectorGuiResult GuiIntWithInfinite(object value, Inspector.InspectorGuiState state)
		{
			Inspector.InspectorGuiResult ret = new Inspector.InspectorGuiResult();

			if (value is Asset.IntWithInfinite iValue)
			{
				bool isEdited = false;

				int[] v = new[] { iValue.Value };
				Manager.NativeManager.PushItemWidth(60);
				if (Manager.NativeManager.DragInt(state.Id + "_Value", v, 1))
				{
					iValue.Value = v[0];
					isEdited = true;
				}

				Manager.NativeManager.SameLine();

				bool[] v_isInfinite = new[] { iValue.Infinite };
				var infLabel = Resources.GetString("Infinite");
				if (Manager.NativeManager.Checkbox(infLabel + state.Id + "_IsInfinite", v_isInfinite))
				{
					iValue.Infinite = v_isInfinite[0];
					isEdited = true;
				}

				Manager.NativeManager.PopItemWidth();

				if (isEdited) 
				{
					ret.isEdited = true;
					ret.value = iValue;
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiIntWithInfinite");
			}
			return ret;
		}


		public static Inspector.InspectorGuiResult GuiIntWithRange(object value, Inspector.InspectorGuiState state)
		{
			Inspector.InspectorGuiResult ret = new Inspector.InspectorGuiResult();
			Asset.IntWithRange intWithRange = value as Asset.IntWithRange;

			int step = 1;

			if (intWithRange != null)
			{
				Asset.DynamicEquation dynamicEquationMin = intWithRange.DynamicEquationMin;
				Asset.DynamicEquation dynamicEquationMax = intWithRange.DynamicEquationMax;

				int[] internalValue;
				bool isEdited = false;
				bool isPopupShown = false;

				var canSelectDynamicEquationAttribute = state.Attriubutes.Where(
					_ => _.GetType() == typeof(Asset.CanSelectDynamicEquationAttribute)
					).FirstOrDefault()
					as Asset.CanSelectDynamicEquationAttribute;
				bool canSelectDynamicEquation = canSelectDynamicEquationAttribute != null ? canSelectDynamicEquationAttribute.CanSelect : false;

				Action popup = () =>
				{
					if (isPopupShown) { return; }

					if (Manager.NativeManager.BeginPopupContextItem(state.Id))
					{
						//Functions.ShowReset(binding, state.Id);

						if (canSelectDynamicEquation)
						{
							DynamicSelector.Popup(state.Id, dynamicEquationMin, dynamicEquationMax, ref intWithRange.IsDynamicEquationEnabled);
						}


						if (intWithRange.IsDynamicEquationEnabled)
						{
							// None
						}
						else
						{
							var txt_r_r1 = Resources.GetString("Gauss");
							var txt_r_r2 = Resources.GetString("Range");

							if (Manager.NativeManager.RadioButton(txt_r_r1 + state.Id + "_Gauss", intWithRange.DrawnAs == Data.DrawnAs.CenterAndAmplitude))
							{
								intWithRange.DrawnAs = Data.DrawnAs.CenterAndAmplitude;
							}

							Manager.NativeManager.SameLine();

							if (Manager.NativeManager.RadioButton(txt_r_r2 + state.Id + "_Range", intWithRange.DrawnAs == Data.DrawnAs.MaxAndMin))
							{
								intWithRange.DrawnAs = Data.DrawnAs.MaxAndMin;
							}
						}

						Manager.NativeManager.EndPopup();

						isPopupShown = true;
					}
				};

				string txt_r1 = string.Empty;
				string txt_r2 = string.Empty;


				var range_1_min = int.MinValue;
				var range_1_max = int.MaxValue;
				var range_2_min = int.MinValue;
				var range_2_max = int.MaxValue;

				if (intWithRange.DrawnAs == Data.DrawnAs.CenterAndAmplitude && !intWithRange.IsDynamicEquationEnabled)
				{
					internalValue = new int[] { intWithRange.Center, intWithRange.Amplitude };
					txt_r1 = Resources.GetString("Mean");
					txt_r2 = Resources.GetString("Deviation");

					range_1_min = intWithRange.Min;
					range_1_max = intWithRange.Max;
				}
				else
				{
					internalValue = new int[] { intWithRange.Min, intWithRange.Max };
					txt_r1 = Resources.GetString("Min");
					txt_r2 = Resources.GetString("Max");

					range_1_min = intWithRange.Min;
					range_1_max = intWithRange.Max;
					range_2_min = intWithRange.Min;
					range_2_max = intWithRange.Max;
				}

				if (Manager.NativeManager.DragInt2EfkEx(state.Id, internalValue, step,
					range_1_min, range_1_max,
					range_2_min, range_2_max,
					txt_r1 + ":" + "%.0f", txt_r2 + ":" + "%.0f"))
				{
					if (intWithRange.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
					{
						intWithRange.Center = internalValue[0];
						intWithRange.Amplitude = internalValue[1];
					}
					else
					{
						intWithRange.Min = internalValue[0];
						intWithRange.Max = internalValue[1];
					}

					isEdited = true;
				}
				popup();


				// 
				if (canSelectDynamicEquation && intWithRange.IsDynamicEquationEnabled)
				{
					// min
					var equationMin = DynamicSelector.SelectMinInComponent(state.Id + "_EquationMin", intWithRange.DynamicEquationMin);
					if (equationMin != null)
					{
						intWithRange.DynamicEquationMax = equationMin;
						isEdited = true;
					}
					popup();

					// max
					var equationMax = DynamicSelector.SelectMaxInComponent(state.Id + "_EquationMax", intWithRange.DynamicEquationMax);
					if (equationMax != null)
					{
						intWithRange.DynamicEquationMax = equationMax;
						isEdited = true;
					}
					popup();
				}

				if (isEdited)
				{
					ret.isEdited = true;
					ret.value = intWithRange;
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert IntWithRange");
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
			else if (value is Asset.Float floatWithDynamicValue)
			{
				bool isEdited = false;
				float[] v = new[] { floatWithDynamicValue.Value };

				bool isPopupShown = false;

				var canSelectDynamicEquationAttribute = state.Attriubutes.Where(
					_ => _.GetType() == typeof(Asset.CanSelectDynamicEquationAttribute)
					).FirstOrDefault() as Asset.CanSelectDynamicEquationAttribute;
				bool canSelectDynamicEquation = canSelectDynamicEquationAttribute != null ? canSelectDynamicEquationAttribute.CanSelect : false;


				// Action that show popup
				Action popup = () => {
					if (isPopupShown) return;

					if (Manager.NativeManager.BeginPopupContextItem(state.Id))
					{
						if (canSelectDynamicEquation)
						{
							if (DynamicSelector.Popup(
								state.Id,
								floatWithDynamicValue.DynamicEquation,
								ref floatWithDynamicValue.IsDynamicEquationEnabled))
							{
								isEdited = true;
							}
						}

						Manager.NativeManager.EndPopup();

						isPopupShown = true;
					}
				};


				if (Manager.NativeManager.DragFloat(state.Id, v, .1f))
				{
					floatWithDynamicValue.Value = v[0];
					isEdited = true;
				}
				popup();

				if (canSelectDynamicEquation && floatWithDynamicValue.IsDynamicEquationEnabled)
				{
					var equation = DynamicSelector.SelectInComponent(state.Id + "_Equation", floatWithDynamicValue.DynamicEquation);
					if (equation != null)
					{
						floatWithDynamicValue.DynamicEquation = equation;
						isEdited = true;
					}
					popup();
				}

				if (isEdited)
				{
					ret.isEdited = true;
					ret.value = floatWithDynamicValue;
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
				Asset.DynamicEquation dynamicEquationMin = floatWithRange.DynamicEquationMin;
				Asset.DynamicEquation dynamicEquationMax = floatWithRange.DynamicEquationMax;

				float[] internalValue;
				bool isEdited = false;
				bool isPopupShown = false;

				var canSelectDynamicEquationAttribute = state.Attriubutes.Where(
					_ => _.GetType() == typeof(Asset.CanSelectDynamicEquationAttribute)
					).FirstOrDefault()
					as Asset.CanSelectDynamicEquationAttribute;
				bool canSelectDynamicEquation = canSelectDynamicEquationAttribute != null ? canSelectDynamicEquationAttribute.CanSelect : false;

				Action popup = () =>
				{
					if (isPopupShown) { return; }

					if (Manager.NativeManager.BeginPopupContextItem(state.Id))
					{
						//Functions.ShowReset(binding, state.Id);

						if (canSelectDynamicEquation)
						{
							DynamicSelector.Popup(state.Id, dynamicEquationMin, dynamicEquationMax, ref floatWithRange.IsDynamicEquationEnabled);
						}


						if (floatWithRange.IsDynamicEquationEnabled)
						{
							// None
						}
						else
						{
							var txt_r_r1 = Resources.GetString("Gauss");
							var txt_r_r2 = Resources.GetString("Range");

							if (Manager.NativeManager.RadioButton(txt_r_r1 + state.Id + "_Gauss", floatWithRange.DrawnAs == Data.DrawnAs.CenterAndAmplitude))
							{
								floatWithRange.DrawnAs = Data.DrawnAs.CenterAndAmplitude;
							}

							Manager.NativeManager.SameLine();

							if (Manager.NativeManager.RadioButton(txt_r_r2 + state.Id + "_Range", floatWithRange.DrawnAs == Data.DrawnAs.MaxAndMin))
							{
								floatWithRange.DrawnAs = Data.DrawnAs.MaxAndMin;
							}
						}
						
						Manager.NativeManager.EndPopup();
						
						isPopupShown = true;
					}
				};

				string txt_r1 = string.Empty;
				string txt_r2 = string.Empty;


				var range_1_min = float.MinValue;
				var range_1_max = float.MaxValue;
				var range_2_min = float.MinValue;
				var range_2_max = float.MaxValue;

				if (floatWithRange.DrawnAs == Data.DrawnAs.CenterAndAmplitude && !floatWithRange.IsDynamicEquationEnabled)
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

					isEdited = true;
				}
				popup();


				// 
				if (canSelectDynamicEquation && floatWithRange.IsDynamicEquationEnabled)
				{
					// min
					var equationMin = DynamicSelector.SelectMinInComponent(state.Id + "_EquationMin", floatWithRange.DynamicEquationMin);
					if (equationMin != null)
					{
						floatWithRange.DynamicEquationMax = equationMin;
						isEdited = true;
					}
					popup();

					// max
					var equationMax = DynamicSelector.SelectMaxInComponent(state.Id + "_EquationMax", floatWithRange.DynamicEquationMax);
					if (equationMax != null)
					{
						floatWithRange.DynamicEquationMax = equationMax;
						isEdited = true;
					}
					popup();
				}

				if (isEdited)
				{
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
				bool isEdited = false;
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

					isEdited = true;
				}
				if (isEdited)
				{
					ret.isEdited = true;
					ret.value = vec3Value;
					return ret;
				}
			}
			else if(value is Asset.Vector3 vec3WithDynamicValue)
			{
				bool isEdited = false;
				float[] guiValue = new float[] { vec3WithDynamicValue.Value.X, vec3WithDynamicValue.Value.Y, vec3WithDynamicValue.Value.Z };

				bool isPopupShown = false;

				var canSelectDynamicEquationAttribute = state.Attriubutes.Where(
					_ => _.GetType() == typeof(Asset.CanSelectDynamicEquationAttribute)
					).FirstOrDefault() as Asset.CanSelectDynamicEquationAttribute;
				bool canSelectDynamicEquation = canSelectDynamicEquationAttribute != null ? canSelectDynamicEquationAttribute.CanSelect : false;


				// Action that show popup
				Action popup = () => {
					if (isPopupShown) return;

					if (Manager.NativeManager.BeginPopupContextItem(state.Id))
					{
						if (canSelectDynamicEquation)
						{
							if (DynamicSelector.Popup(
								state.Id,
								vec3WithDynamicValue.DynamicEquation,
								ref vec3WithDynamicValue.IsDynamicEquationEnabled))
							{
								isEdited = true;
							}
						}

						Manager.NativeManager.EndPopup();

						isPopupShown = true;
					}
				};

				if (Manager.NativeManager.DragFloat3EfkEx(state.Id, guiValue, 1.0f,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					"X:" + Core.Option.GetFloatFormat(), "Y:" + Core.Option.GetFloatFormat(), "Z:" + Core.Option.GetFloatFormat()))
				{
					vec3Value.X = guiValue[0];
					vec3Value.Y = guiValue[1];
					vec3Value.Z = guiValue[2];

					isEdited = true;
				}
				popup();


				if (canSelectDynamicEquation && vec3WithDynamicValue.IsDynamicEquationEnabled)
				{
					var equation = DynamicSelector.SelectMinInComponent(state.Id + "_Equation", vec3WithDynamicValue.DynamicEquation);
					if (equation != null)
					{
						vec3WithDynamicValue.DynamicEquation = equation;
						isEdited = true;
					}
					popup();
				}

				if (isEdited)
				{
					ret.isEdited = true;
					ret.value = vec3WithDynamicValue;
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiVector3F");
			}
			return ret;
		}


		public static Inspector.InspectorGuiResult GuiVector3WithRange(object value, Inspector.InspectorGuiState state)
		{
			Inspector.InspectorGuiResult ret = new Inspector.InspectorGuiResult();

			bool isEdited = false;
			bool isPopupShown = false;

			if (value is Vector3WithRange vec3Value)
			{
				float[] guiValue1;
				float[] guiValue2;

				var canSelectDynamicEquationAttribute = state.Attriubutes.Where(
					_ => _.GetType() == typeof(Asset.CanSelectDynamicEquationAttribute)
					).FirstOrDefault()
					as Asset.CanSelectDynamicEquationAttribute;
				bool canSelectDynamicEquation = canSelectDynamicEquationAttribute != null ? canSelectDynamicEquationAttribute.CanSelect : false;

				// Action that show popup
				Action popup = () => {
					if (isPopupShown) return;

					if (Manager.NativeManager.BeginPopupContextItem(state.Id))
					{

						if (canSelectDynamicEquation)
						{
							if (DynamicSelector.Popup(
								state.Id, 
								vec3Value.DynamicEquationMin, vec3Value.DynamicEquationMax, 
								ref vec3Value.IsDynamicEquationEnabled))
							{
								isEdited = true;
							}
						}

						if (vec3Value.IsDynamicEquationEnabled)
						{
							// None
						}
						else
						{
							var txt_r_r1 = Resources.GetString("Gauss");
							var txt_r_r2 = Resources.GetString("Range");

							if (Manager.NativeManager.RadioButton(txt_r_r1 + state.Id + "_Gauss", vec3Value.DrawnAs == Data.DrawnAs.CenterAndAmplitude))
							{
								vec3Value.DrawnAs = Data.DrawnAs.CenterAndAmplitude;
								isEdited = true;
							}

							Manager.NativeManager.SameLine();

							if (Manager.NativeManager.RadioButton(txt_r_r2 + state.Id + "_Range", vec3Value.DrawnAs == Data.DrawnAs.MaxAndMin))
							{
								vec3Value.DrawnAs = Data.DrawnAs.MaxAndMin;
								isEdited = true;
							}
						}

						Manager.NativeManager.EndPopup();

						isPopupShown = true;
					}
				};

				string txt_r1 = string.Empty;
				string txt_r2 = string.Empty;

				if (vec3Value.DrawnAs == DrawnAs.CenterAndAmplitude && !vec3Value.IsDynamicEquationEnabled)
				{
					guiValue1 = new float[] { vec3Value.X.Center, vec3Value.Y.Center, vec3Value.Z.Center };
					guiValue2 = new float[] { vec3Value.X.Amplitude, vec3Value.Y.Amplitude, vec3Value.Z.Amplitude };

					txt_r1 = Resources.GetString("Mean");
					txt_r2 = Resources.GetString("Deviation");
				}
				else
				{
					guiValue1 = new float[] { vec3Value.X.Min, vec3Value.Y.Min, vec3Value.Z.Min };
					guiValue2 = new float[] { vec3Value.X.Max, vec3Value.Y.Max, vec3Value.Z.Max };

					txt_r1 = Resources.GetString("Min");
					txt_r2 = Resources.GetString("Max");
				}

				Manager.NativeManager.PushItemWidth(Manager.NativeManager.GetColumnWidth() - 48 * Manager.DpiScale);

				if (Manager.NativeManager.DragFloat3EfkEx(state.Id + "_1", guiValue1, 1.0f,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					"X:" + Core.Option.GetFloatFormat(), "Y:" + Core.Option.GetFloatFormat(), "Z:" + Core.Option.GetFloatFormat()))
				{
					if (vec3Value.DrawnAs == DrawnAs.CenterAndAmplitude && !vec3Value.IsDynamicEquationEnabled)
					{
						vec3Value.X.Center = guiValue1[0];
						vec3Value.Y.Center = guiValue1[1];
						vec3Value.Z.Center = guiValue1[2];
					}
					else
					{
						vec3Value.X.Min = guiValue1[0];
						vec3Value.Y.Min = guiValue1[1];
						vec3Value.Z.Min = guiValue1[2];
					}

					isEdited = true;
				}

				popup();

				Manager.NativeManager.SameLine();
				Manager.NativeManager.Text(txt_r1);

				if (canSelectDynamicEquation && vec3Value.IsDynamicEquationEnabled)
				{
					var newEquation = DynamicSelector.SelectMaxInComponent(state.Id + "_EquationMin", vec3Value.DynamicEquationMin);
					if (newEquation != null)
					{
						vec3Value.DynamicEquationMin = newEquation;
						isEdited = true;
					}

					popup();
				}

				if (Manager.NativeManager.DragFloat3EfkEx(state.Id + "_2", guiValue2, 1.0f,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					float.MinValue, float.MaxValue,
					"X:" + Core.Option.GetFloatFormat(), "Y:" + Core.Option.GetFloatFormat(), "Z:" + Core.Option.GetFloatFormat()))
				{
					if (vec3Value.DrawnAs == DrawnAs.CenterAndAmplitude && !vec3Value.IsDynamicEquationEnabled)
					{
						vec3Value.X.Amplitude = guiValue2[0];
						vec3Value.Y.Amplitude = guiValue2[1];
						vec3Value.Z.Amplitude = guiValue2[2];
					}
					else
					{
						vec3Value.X.Max = guiValue2[0];
						vec3Value.Y.Max = guiValue2[1];
						vec3Value.Z.Max = guiValue2[2];
					}

					isEdited = true;
				}

				popup();

				Manager.NativeManager.SameLine();
				Manager.NativeManager.Text(txt_r2);


				if (canSelectDynamicEquation && vec3Value.IsDynamicEquationEnabled)
				{
					var newEquation = DynamicSelector.SelectMaxInComponent(state.Id + "_EquationMax", vec3Value.DynamicEquationMax);
					if (newEquation != null)
					{
						vec3Value.DynamicEquationMax = newEquation;
						isEdited = true;
					}
					popup();
				}

				Manager.NativeManager.PopItemWidth();

				if (isEdited)
				{
					ret.isEdited = true;
					ret.value = vec3Value;
					return ret;
				}
			}
			else
			{
				Manager.NativeManager.Text("Assert GuiVector3WithRange");
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