using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.BindableComponent
{
	class Gradient : Control, IParameterControl
	{
		class GradientCopyPasteHolder
		{
			[Effekseer.Data.IO(Export = true)]
			public Data.Value.Gradient Gradient { get; set; }
		}

		int id = -1;
		int id_popup = -1;
		string id_c = "";
		string id_color_position = "";
		string id_color_value = "";
		string id_color_intensity = "";
		string id_color_delete = "";
		string id_alpha_position = "";
		string id_alpha_value = "";
		string id_alpha_delete = "";
		string id_reset = "";

		bool isPopupShown = false;

		Data.Value.Gradient binding = null;

		swig.GradientHDRState internalState = null;
		swig.GradientHDRGUIState guiState = null;
		swig.GradientHDRGUIState guiPopupState = null;

		CopyAndPaste copyAndPaste;

		public bool EnableUndo { get; set; } = true;

		public Data.Value.Gradient Binding
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

		public Gradient()
		{
			id = Manager.GetUniqueID();
			id_popup = Manager.GetUniqueID();
			id_c = "###" + Manager.GetUniqueID().ToString();
			id_color_position = "###" + Manager.GetUniqueID().ToString();
			id_color_value = "###" + Manager.GetUniqueID().ToString();
			id_color_intensity = "###" + Manager.GetUniqueID().ToString();
			id_color_delete = "###" + Manager.GetUniqueID().ToString();
			id_alpha_position = "###" + Manager.GetUniqueID().ToString();
			id_alpha_value = "###" + Manager.GetUniqueID().ToString();
			id_alpha_delete = "###" + Manager.GetUniqueID().ToString();
			id_reset =  "###" + Manager.GetUniqueID().ToString();

			copyAndPaste = new CopyAndPaste("Gradient", 
				() => new GradientCopyPasteHolder { Gradient = Binding }, 
				() => { CopyState(internalState, Binding.GetValue()); });
		}

		public void SetBinding(object o)
		{
			if (Binding == o)
			{
				return;
			}

			if (Binding != null)
			{
				Binding.OnChanged -= Binding_OnChanged;
			}

			Binding = o as Data.Value.Gradient;
			internalState = new swig.GradientHDRState();

			if (Binding != null)
			{
				guiState = new swig.GradientHDRGUIState();
				guiPopupState = new swig.GradientHDRGUIState();

				CopyState(internalState, Binding.GetValue());
				Binding.OnChanged += Binding_OnChanged;
			}
		}

		private void Binding_OnChanged(object sender, ChangedValueEventArgs e)
		{
			CopyState(internalState, Binding.GetValue());
		}

		public void FixValue()
		{
			StoreValue();
		}

		public override void Update()
		{
			isPopupShown = false;

			if (Manager.NativeManager.GradientHDR(id, internalState, guiState, false))
			{
				StoreValue();
			}

			if (Manager.NativeManager.IsItemClicked(0))
			{
				Manager.NativeManager.OpenPopup(id_c);
			}

			Popup();


			// TODO : move core
			if (!Manager.NativeManager.IsAnyItemActive())
			{
				Command.CommandManager.FreezeCombinedCommands();
			}
		}

		void Popup()
		{
			if (isPopupShown) return;

			var windowPos = Manager.NativeManager.GetWindowPos();
			var windowSize = Manager.NativeManager.GetWindowSize();

			// Popup below the last item
			Manager.NativeManager.SetNextWindowPos(new swig.Vec2(windowPos.X, Manager.NativeManager.GetCursorScreenPosY()), swig.Cond.Always, new swig.Vec2(0, 0));
			Manager.NativeManager.SetNextWindowSize(windowSize.X, 0.0f, swig.Cond.Always);

			if (Manager.NativeManager.BeginPopup(id_c, swig.WindowFlags.NoMove | swig.WindowFlags.AlwaysAutoResize | swig.WindowFlags.MenuBar))
			{
				if (Manager.NativeManager.BeginMenuBar())
				{
					Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.Button, 0);

					copyAndPaste.Update();

					Functions.ShowReset(binding, id_reset);

					Manager.NativeManager.PopStyleColor();
					Manager.NativeManager.EndMenuBar();
				}

				var contentSize = Manager.NativeManager.GetContentRegionAvail();
				float buttonWidth = 50.0f * Manager.GetUIScaleBasedOnFontSize();
				var itemSpacing = Manager.NativeManager.GetItemSpacing();

				int selectedIndex = guiPopupState.GetSelectedIndex();
				bool alphaMarkerSelected = selectedIndex >= 0 && guiPopupState.GetSelectedMarkerType() == swig.GradientHDRMarkerType.Alpha;
				bool colorMarkerSelected = selectedIndex >= 0 && guiPopupState.GetSelectedMarkerType() == swig.GradientHDRMarkerType.Color;

				{
					// Alpha Edit GUI
					Manager.NativeManager.BeginDisabled(!alphaMarkerSelected);

					Manager.NativeManager.Columns(2);
					Manager.NativeManager.SetColumnWidth(0, contentSize.X * 0.3f);

					Manager.NativeManager.Text("Position");
					Manager.NativeManager.NextColumn();

					float pos = alphaMarkerSelected ? internalState.GetAlphaMarkerPosition(selectedIndex) : 0.0f;
					var posArray = new float[1] { pos * 100.0f };

					Manager.NativeManager.SetNextItemWidth(contentSize.X * 0.7f - buttonWidth - itemSpacing.X);
					if (Manager.NativeManager.DragFloat(id_alpha_position, posArray, 1.0f, 0.0f, 100.0f, "%.2f %%"))
					{
						pos = posArray[0] / 100.0f;
						internalState.SetAlphaMarkerPosition(selectedIndex, pos);
					}

					Manager.NativeManager.SameLine();
					if (Manager.NativeManager.Button("Delete" + id_alpha_delete, buttonWidth))
					{
						if (internalState.RemoveAlphaMarker(selectedIndex))
						{
							StoreValue();
						}
					}
					Manager.NativeManager.NextColumn();

					Manager.NativeManager.Text("Alpha");
					Manager.NativeManager.NextColumn();

					var alpha = alphaMarkerSelected ? internalState.GetAlphaMarkerAlpha(selectedIndex) : 0.0f;
					var alphaArray = new float[1] { alpha };

					Manager.NativeManager.SetNextItemWidth(-1);
					if (Manager.NativeManager.DragFloat(id_alpha_value, alphaArray, 0.01f, 0.0f, 1.0f))
					{
						alpha = alphaArray[0];
						internalState.SetAlphaMarkerAlpha(selectedIndex, alpha);
						StoreValue();
					}
					Manager.NativeManager.NextColumn();

					Manager.NativeManager.Columns(1);
					Manager.NativeManager.EndDisabled();
				}

				if (Manager.NativeManager.GradientHDR(id_popup, internalState, guiPopupState, true))
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

					float pos = colorMarkerSelected ? internalState.GetColorMarkerPosition(selectedIndex) : 0.0f;
					var posArray = new float[1] { pos * 100.0f };

					Manager.NativeManager.SetNextItemWidth(contentSize.X * 0.7f - buttonWidth - itemSpacing.X);
					if (Manager.NativeManager.DragFloat(id_color_position, posArray, 1.0f, 0.0f, 100.0f, "%.2f %%"))
					{
						pos = posArray[0] / 100.0f;
						internalState.SetColorMarkerPosition(selectedIndex, pos);
					}

					Manager.NativeManager.SameLine();
					if (Manager.NativeManager.Button("Delete" + id_color_delete, buttonWidth))
					{
						if (internalState.RemoveColorMarker(selectedIndex))
						{
							StoreValue();
						}
					}
					Manager.NativeManager.NextColumn();

					Manager.NativeManager.Text("Color");
					Manager.NativeManager.NextColumn();

					var color = colorMarkerSelected ? internalState.GetColorMarkerColor(selectedIndex) : new swig.ColorF(0.0f, 0.0f, 0.0f, 0.0f);
					var colorArray = new float[] { color.R, color.G, color.B, 1.0f };

					Manager.NativeManager.SetNextItemWidth(-1);
					if (Manager.NativeManager.ColorEdit4(id_color_value, colorArray, swig.ColorEditFlags.NoAlpha))
					{
						color.R = colorArray[0];
						color.G = colorArray[1];
						color.B = colorArray[2];
						internalState.SetColorMarkerColor(selectedIndex, color);
						StoreValue();
					}
					Manager.NativeManager.NextColumn();

					Manager.NativeManager.Text("Intensity");
					Manager.NativeManager.NextColumn();

					var intensity = colorMarkerSelected ? internalState.GetColorMarkerIntensity(selectedIndex) : 0.0f;
					var intensityArray = new float[1] { intensity };

					Manager.NativeManager.SetNextItemWidth(-1);
					if (Manager.NativeManager.DragFloat(id_color_intensity, intensityArray, 0.01f, 0.0f, float.MaxValue))
					{
						intensity = intensityArray[0];
						internalState.SetColorMarkerIntensity(selectedIndex, intensity);
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

		void StoreValue()
		{
			var state = new Data.Value.Gradient.State();
			CopyState(state, internalState);
			Binding.SetValue(state, true);
		}

		unsafe void CopyState(swig.GradientHDRState dst, Data.Value.Gradient.State src)
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

		unsafe void CopyState(Data.Value.Gradient.State dst, swig.GradientHDRState src)
		{
			dst.ColorMarkers = new Data.Value.Gradient.ColorMarker[src.GetColorCount()];
			dst.AlphaMarkers = new Data.Value.Gradient.AlphaMarker[src.GetAlphaCount()];

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
	}
}
