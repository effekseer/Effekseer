using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;

namespace Effekseer.GUI.Widgets
{
	class Gradient
	{
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

			public CopyAndPaste<Asset.Gradient> copyAndPaste;
		}

		public static Inspector.InspectorGuiResult GuiGradient(object value, Inspector.InspectorGuiState state)
		{
			Inspector.InspectorGuiResult ret = new Inspector.InspectorGuiResult();
			var isPopupShown = false;

			var actualValue = (Asset.Gradient)value;

			unsafe void CopyStateToNative(swig.GradientHDRState dst, Asset.Gradient src)
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

			unsafe void CopyStateFromNative(Asset.Gradient dst, swig.GradientHDRState src)
			{
				dst.ColorMarkers = new Asset.Gradient.ColorMarker[src.GetColorCount()];
				dst.AlphaMarkers = new Asset.Gradient.AlphaMarker[src.GetAlphaCount()];

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

				gradientState.copyAndPaste = new CopyAndPaste<Asset.Gradient>(
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

			CopyStateToNative(gradientState.state, actualValue);

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
}