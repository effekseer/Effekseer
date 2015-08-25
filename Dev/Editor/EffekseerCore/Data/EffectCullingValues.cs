using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class EffectCullingValues
	{
		[Name(language = Language.Japanese, value = "カリングの表示")]
		[Description(language = Language.Japanese, value = "カリングの表示非表示")]
		[Name(language = Language.English, value = "Enable Culling")]
		[Description(language = Language.English, value = "Whether to render hidden surfaces")]
		[Undo(Undo = false)]
		public Value.Boolean IsShown
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "カリングの種類")]
		[Name(language = Language.English, value = "Culling mode")]
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
			[Name(language = Language.Japanese, value = "半径")]
			[Description(language = Language.Japanese, value = "カリング球の半径")]
			[Name(language = Language.English, value = "Radius")]
			[Description(language = Language.English, value = "Radius of spherical culling")]
			[Undo(Undo = true)]
			public Value.Float Radius
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "位置")]
			[Description(language = Language.Japanese, value = "中心の位置")]
			[Name(language = Language.English, value = "Position")]
			[Description(language = Language.English, value = "Central location")]
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
			[Name(value = "なし", language = Language.Japanese)]
			[Name(value = "None", language = Language.English)]
			None = 0,
			[Name(value = "球", language = Language.Japanese)]
			[Name(value = "Spherical", language = Language.English)]
			Sphere = 1,
		}
	}
}
