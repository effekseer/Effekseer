using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace mqoToEffekseerModelConverter.mqoIO
{
	public class Data
	{
		public Scene Scene { get; set; }

		public Material[] Materials { get; set; }

		public Object[] Objects { get; set; }

		public Data()
		{
			Materials = new Material[0];
			Objects = new Object[0];
		}
	}

	public class Scene
	{
		public Vector3D Position;
		public Vector3D LookAt;
		public float Head;
		public float Pich;
		public int Ortho;
		public float Zoom2;
		public Vector3D Amb;
	}

	public class Material
	{
		public string Name { get; set; }

		public Dictionary<string, object> Paramaters = new Dictionary<string, object>();

		public Material()
		{
			Name = string.Empty;
		}
	}

	public class Object
	{
		public string Name { get; set; }

        public bool Visible = true;

		public float Facet = 0.0f;

		public Vector3D[] Vertexes { get; set; }

		public Face[] Faces { get; set; }

		public Object()
		{
			Name = string.Empty;
			Vertexes = new Vector3D[0];
			Faces = new Face[0];
		}
	}

	public class Face
	{
		public int[] Indexes;
		public float[] UV;
		public Color[] Colors;
		public int MaterialIndex = -1;
	}
}
