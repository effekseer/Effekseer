using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	class GradientHDR
	{
		unsafe struct ColorMarker
		{
			public float Position;
			public fixed float Color[3];
			public float Intensity;
		}

		struct AlphaMarker
		{
			public float Position;
			public float Alpha;
		}

		class State
		{
			public ColorMarker[] ColorMarkers;
			public AlphaMarker[] AlphaMarkers;
		}


	}
}