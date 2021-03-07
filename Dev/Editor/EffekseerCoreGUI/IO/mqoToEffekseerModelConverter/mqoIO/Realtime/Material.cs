using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace mqoToEffekseerModelConverter.mqoIO.Realtime
{
	public class Material
	{
		public Color Diffuse = new Color(1.0f, 1.0f, 1.0f, 1.0f);
		public Color Ambient = new Color(0.0f, 0.0f, 0.0f, 0.0f);
		public string ColorTexture = string.Empty;
		public string NormalTexture = string.Empty;
		public string AlphaTexture = string.Empty;
	}
}
