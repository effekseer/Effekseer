using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Utl
{
	public struct RGBHSVColor
	{
		public int RH;
		public int GS;
		public int BV;

		public static RGBHSVColor RGBToHSV(RGBHSVColor rgb)
		{
			double max;
			double min;
			double R, G, B, H, S, V;

			R = (double)rgb.RH / 255.0;
			G = (double)rgb.GS / 255.0;
			B = (double)rgb.BV / 255.0;

			if (R >= G && R >= B)
			{
				max = R;
				min = (G < B) ? G : B;
			}
			else if (G >= R && G >= B)
			{
				max = G;
				min = (R < B) ? R : B;
			}
			else
			{
				max = B;
				min = (R < G) ? R : G;
			}
			if (R == G && G == B)
			{
				H = 0.0f;
			}
			else if (max == R)
			{
				H = 60 * (G - B) / (max - min);
			}
			else if (max == G)
			{
				H = 60 * (B - R) / (max - min) + 120;
			}
			else
			{
				H = 60 * (R - G) / (max - min) + 240;
			}
			if (H < 0.0f)
			{
				H += 360.0f;
			}
			if (max == 0.0f)
			{
				S = 0.0f;
			}
			else
			{
				S = (max - min) / max;
			}
			V = max;

			RGBHSVColor ret = new RGBHSVColor();
			ret.RH = (int)Math.Round(H / 360 * 252, MidpointRounding.AwayFromZero);
			ret.GS = (int)Math.Round(S * 255, MidpointRounding.AwayFromZero);
			ret.BV = (int)Math.Round(V * 255, MidpointRounding.AwayFromZero);
			return ret;
		}

		public static RGBHSVColor HSVToRGB(RGBHSVColor hsv)
		{
			int H = hsv.RH, S = hsv.GS, V = hsv.BV;
			int i, R = 0, G = 0, B = 0, f, p, q, t;

			i = H / 42 % 6;
			f = H % 42 * 6;
			p = (V * (256 - S)) >> 8;
			q = (V * (256 - ((S * f) >> 8))) >> 8;
			t = (V * (256 - ((S * (252 - f)) >> 8))) >> 8;
			if (p < 0) p = 0;
			if (p > 255) p = 255;
			if (q < 0) q = 0;
			if (q > 255) q = 255;
			if (t < 0) t = 0;
			if (t > 255) t = 255;

			switch (i)
			{
				case 0: R = V; G = t; B = p; break;
				case 1: R = q; G = V; B = p; break;
				case 2: R = p; G = V; B = t; break;
				case 3: R = p; G = q; B = V; break;
				case 4: R = t; G = p; B = V; break;
				case 5: R = V; G = p; B = q; break;
			}

			RGBHSVColor ret = new RGBHSVColor();
			ret.RH = R;
			ret.GS = G;
			ret.BV = B;
			return ret;
		}
	}
}
