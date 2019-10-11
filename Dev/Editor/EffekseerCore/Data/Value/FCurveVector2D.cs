using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class FCurveVector2D
	{
		public Value.Enum<FCurveTimelineType> Timeline = new Enum<FCurveTimelineType>();
		public FCurve<float> X { get; private set; }
		public FCurve<float> Y { get; private set; }

		public FCurveVector2D()
		{
			Timeline = new Enum<FCurveTimelineType>();
			X = new FCurve<float>(0);
			Y = new FCurve<float>(0);
		}

		public byte[] GetBytes(float mul = 1.0f)
		{
			List<byte[]> data = new List<byte[]>();
			data.Add(BitConverter.GetBytes((int)Timeline.Value));
			data.Add(X.GetBytes(mul));
			data.Add(Y.GetBytes(mul));
			return data.SelectMany(_ => _).ToArray();
		}
	}
}
