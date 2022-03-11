using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class GradientHDR : Control, IParameterControl
	{
		int id = -1;
		int id_popup = -1;
		string id_c = "";
		string id_color = "";
		string id_intensity = "";
		string id_alpha = "";

		bool isPopupShown = false;

		Data.Value.GradientHDR binding = null;

		swig.GradientHDRState internalState = null;
		swig.GradientHDRGUIState guiState = null;
		swig.GradientHDRGUIState guiPopupState = null;

		public bool EnableUndo { get; set; } = true;

		public Data.Value.GradientHDR Binding
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

		public GradientHDR()
		{
			id = Manager.GetUniqueID();
			id_popup = Manager.GetUniqueID();
			id_c = "###" + Manager.GetUniqueID().ToString();
			id_color = "###" + Manager.GetUniqueID().ToString();
			id_intensity = "###" + Manager.GetUniqueID().ToString();
			id_alpha = "###" + Manager.GetUniqueID().ToString();
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

			Binding = o as Data.Value.GradientHDR;
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

			if (Manager.NativeManager.GradientHDR(id, internalState, guiState))
			{
				StoreValue();
			}

			if (Manager.NativeManager.Button("Edit"))
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

			if (Manager.NativeManager.BeginPopup(id_c, swig.WindowFlags.AlwaysAutoResize))
			{
				Manager.NativeManager.Dummy(new swig.Vector2I(200, 1));

				if (Manager.NativeManager.GradientHDR(id_popup, internalState, guiPopupState))
				{
					StoreValue();
				}

				var selectedIndex = guiPopupState.GetSelectedIndex();

				if (selectedIndex >= 0)
				{
					if (guiPopupState.GetSelectedMarkerType() == swig.GradientHDRMarkerType.Color)
					{
						var color = internalState.GetColorMarkerColor(selectedIndex);
						var intensity = internalState.GetColorMarkerIntensity(selectedIndex);

						var colors = new float[] { color.R, color.G, color.B, 1.0f };

						if (Manager.NativeManager.ColorEdit4("Color" + id_color, colors, swig.ColorEditFlags.NoAlpha))
						{
							color.R = colors[0];
							color.G = colors[1];
							color.B = colors[2];
							internalState.SetColorMarkerColor(selectedIndex, color);
							StoreValue();
						}

						var intensityArray = new float[1];
						intensityArray[0] = intensity;

						if (Manager.NativeManager.DragFloat("Intensity" + id_intensity, intensityArray, 0.1f, 0.0f, float.MaxValue))
						{
							intensity = intensityArray[0];
							internalState.SetColorMarkerIntensity(selectedIndex, intensity);
							StoreValue();
						}
					}
					else if (guiPopupState.GetSelectedMarkerType() == swig.GradientHDRMarkerType.Alpha)
					{
						var alpha = internalState.GetAlphaMarkerAlpha(selectedIndex);
						var alphaArray = new float[1];
						alphaArray[0] = alpha;

						if (Manager.NativeManager.DragFloat("Alpha" + id_alpha, alphaArray, 0.1f, 0.0f, 1.0f))
						{
							alpha = alphaArray[0];
							internalState.SetAlphaMarkerAlpha(selectedIndex, alpha);
							StoreValue();
						}
					}

					if (Manager.NativeManager.Button("Delete"))
					{
						var selectedType = guiPopupState.GetSelectedMarkerType();

						if (selectedType == swig.GradientHDRMarkerType.Color)
						{
							if (internalState.RemoveColorMarker(selectedIndex))
							{
								StoreValue();
							}
						}
						else if (selectedType == swig.GradientHDRMarkerType.Alpha)
						{
							if (internalState.RemoveAlphaMarker(selectedIndex))
							{
								StoreValue();
							}
						}
					}
				}

				Manager.NativeManager.EndPopup();

				isPopupShown = true;
			}
		}
		void StoreValue()
		{
			var state = new Data.Value.GradientHDR.State();
			CopyState(state, internalState);
			Binding.SetValue(state, true);
		}

		unsafe void CopyState(swig.GradientHDRState dst, Data.Value.GradientHDR.State src)
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

		unsafe void CopyState(Data.Value.GradientHDR.State dst, swig.GradientHDRState src)
		{
			dst.ColorMarkers = new Data.Value.GradientHDR.ColorMarker[src.GetColorCount()];
			dst.AlphaMarkers = new Data.Value.GradientHDR.AlphaMarker[src.GetAlphaCount()];

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