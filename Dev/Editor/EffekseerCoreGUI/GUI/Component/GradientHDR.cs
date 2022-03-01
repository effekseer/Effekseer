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

		Data.Value.GradientHDR binding = null;

		swig.GradientHDRState internalState = null;

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
		}

		public void SetBinding(object o)
		{
			Binding = o as Data.Value.GradientHDR;
			internalState = new swig.GradientHDRState();
			if (Binding != null)
			{
				CopyState(internalState, Binding.GetValue());
			}
		}

		public void FixValue()
		{
			StoreValue();
		}

		public override void Update()
		{
			if (Manager.NativeManager.GradientHDR(id, internalState))
			{
				StoreValue();
			}
		}

		void StoreValue()
		{
			var state = new Data.Value.GradientHDR.State();
			CopyState(state, internalState);
			Binding.SetValue(state);
		}

		unsafe void CopyState(swig.GradientHDRState dst, Data.Value.GradientHDR.State src)
		{
			dst.SetColorCount(src.ColorMarkers.Length);
			for (int i = 0; i < src.ColorMarkers.Length; i++)
			{
				var color = new swig.ColorF();
				color.R = src.ColorMarkers[i].Color[0];
				color.G = src.ColorMarkers[i].Color[1];
				color.B = src.ColorMarkers[i].Color[2];

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
				dst.ColorMarkers[i].Color[0] = color.R;
				dst.ColorMarkers[i].Color[1] = color.G;
				dst.ColorMarkers[i].Color[2] = color.B;
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