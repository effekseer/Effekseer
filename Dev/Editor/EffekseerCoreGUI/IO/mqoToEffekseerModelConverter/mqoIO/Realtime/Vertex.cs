using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace mqoToEffekseerModelConverter.mqoIO.Realtime
{
	public class Vertex
	{
		public Vector3D Position;

		public Vector3D Normal;
		public Vector3D Tangent;
		public Vector3D Binormal;

		public Vector2D UV = new Vector2D(float.MaxValue, float.MaxValue);
		public Color Color = new Color(float.MaxValue, float.MaxValue, float.MaxValue, float.MaxValue);

		public Vertex Clone()
		{
			var vertex = new Vertex();
			vertex.Position = Position;
			vertex.Normal = Normal;
			vertex.UV = UV;
			vertex.Color = Color;
			return vertex;
		}
	}
}
