using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class FCurveColorRGBA
	{
		public FCurve<byte> R { get; private set; }
		public FCurve<byte> G { get; private set; }
		public FCurve<byte> B { get; private set; }
		public FCurve<byte> A { get; private set; }

		public FCurveColorRGBA()
		{
			R = new FCurve<byte>(255);
			G = new FCurve<byte>(255);
			B = new FCurve<byte>(255);
			A = new FCurve<byte>(255);
		}

		public byte[] GetBytes(float mul = 1.0f)
		{
			List<byte[]> data = new List<byte[]>();
			data.Add(R.GetBytes(mul));
			data.Add(G.GetBytes(mul));
			data.Add(B.GetBytes(mul));
			data.Add(A.GetBytes(mul));
			return data.SelectMany(_ => _).ToArray();
		}
	}
}
