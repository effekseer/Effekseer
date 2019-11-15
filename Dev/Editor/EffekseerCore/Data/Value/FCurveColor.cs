using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class FCurveColorRGBA
	{
		public Value.Enum<FCurveTimelineType> Timeline = new Enum<FCurveTimelineType>();
		public FCurve<int> R { get; private set; }
		public FCurve<int> G { get; private set; }
		public FCurve<int> B { get; private set; }
		public FCurve<int> A { get; private set; }

		public FCurveColorRGBA()
		{
			Timeline = new Enum<FCurveTimelineType>();
			R = new FCurve<int>(255);
			G = new FCurve<int>(255);
			B = new FCurve<int>(255);
			A = new FCurve<int>(255);
		}

		public byte[] GetBytes(float mul = 1.0f)
		{
			List<byte[]> data = new List<byte[]>();
			data.Add(BitConverter.GetBytes((int)Timeline.Value));
			data.Add(R.GetBytes(mul));
			data.Add(G.GetBytes(mul));
			data.Add(B.GetBytes(mul));
			data.Add(A.GetBytes(mul));
			return data.SelectMany(_ => _).ToArray();
		}
	}
}
