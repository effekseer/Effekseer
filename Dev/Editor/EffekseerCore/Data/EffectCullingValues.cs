using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class EffectCullingValues
	{
		[Key(key = "Culling_IsShown")]
		[Undo(Undo = false)]
		public Value.Boolean IsShown
		{
			get;
			private set;
		}

		[Key(key = "Culling_Type")]
		[Selector(ID = 0)]
		public Value.Enum<ParamaterType> Type
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 1)]
		[IO(Export = true)]
		public SphereParamater Sphere
		{
			get;
			private set;
		}

		public class SphereParamater
		{
			[Key(key = "Culling_Sphere_Radius")]
			[Undo(Undo = true)]
			public Value.Float Radius
			{
				get;
				private set;
			}

			[Key(key = "Culling_Sphere_Location")]
			[Undo(Undo = false)]
			public Value.Vector3D Location
			{
				get;
				private set;
			}

			public SphereParamater()
			{
				Radius = new Value.Float(1);
				Location = new Value.Vector3D();
			}
		}

		public EffectCullingValues()
		{
			IsShown = new Value.Boolean(false);
			Type = new Value.Enum<ParamaterType>(ParamaterType.None);
			Sphere = new SphereParamater();
		}

		public enum ParamaterType : int
		{
			[Key(key = "Culling_ParamaterType_None")]
			None = 0,
			[Key(key = "Culling_ParamaterType_Sphere")]
			Sphere = 1,
		}
	}
}
