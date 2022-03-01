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
		}

		public void FixValue()
		{
		}

		public override void Update()
		{
			if (Manager.NativeManager.GradientHDR(id, internalState))
			{

			}
		}

		void CopyState(swig.GradientHDRState dst, Data.Value.GradientHDR.State src)
		{
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