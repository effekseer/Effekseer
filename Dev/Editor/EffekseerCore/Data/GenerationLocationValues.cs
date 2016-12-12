using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class GenerationLocationValues
	{
		[Name(value = "生成角度に影響", language = Language.Japanese)]
		[Name(value = "Set angle on spawn", language = Language.English)]
		public Value.Boolean EffectsRotation
		{
			get;
			private set;
		}

		[Selector(ID = 0)]
		public Value.Enum<ParameterType> Type
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 0)]
		[IO(Export = true)]
		public PointParameter Point
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 1)]
		[IO(Export = true)]
		public SphereParameter Sphere
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 2)]
		[IO(Export = true)]
		public ModelParameter Model
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 3)]
		[IO(Export = true)]
		public CircleParameter Circle
		{
			get;
			private set;
		}

		internal GenerationLocationValues()
		{
			EffectsRotation = new Value.Boolean(false);
			Type = new Value.Enum<ParameterType>();
			Point = new PointParameter();
			Sphere = new SphereParameter();
			Model = new ModelParameter();
			Circle = new CircleParameter();
		}

		public class PointParameter
		{
			[Name(value = "生成位置", language = Language.Japanese)]
			[Name(value = "Spawn location", language = Language.English)]
			public Value.Vector3DWithRandom Location
			{
				get;
				private set;
			}

			internal PointParameter()
			{
				Location = new Value.Vector3DWithRandom();
			}
		}

		public class SphereParameter
		{
			[Name(value = "半径", language = Language.Japanese)]
			[Name(value = "Radius", language = Language.English)]
			public Value.FloatWithRandom Radius
			{
				get;
				private set;
			}

			[Name(value = "X軸角度(度)", language = Language.Japanese)]
			[Name(value = "X Rotation", language = Language.English)]
			public Value.FloatWithRandom RotationX
			{
				get;
				private set;
			}

			[Name(value = "Y軸角度(度)", language = Language.Japanese)]
			[Name(value = "Y Rotation", language = Language.English)]
			public Value.FloatWithRandom RotationY
			{
				get;
				private set;
			}

			internal SphereParameter()
			{
				Radius = new Value.FloatWithRandom(0.0f, float.MaxValue, 0.0f);
				RotationX = new Value.FloatWithRandom(0.0f);
				RotationY = new Value.FloatWithRandom(0.0f);
			}
		}

		public class ModelParameter
		{
			[Name(language = Language.Japanese, value = "モデル")]
			[Description(language = Language.Japanese, value = "モデルファイル")]
			[Name(language = Language.English, value = "Model")]
			[Description(language = Language.English, value = "Model File")]
			public Value.Path Model
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "生成位置種類")]
			[Name(language = Language.English, value = "Method of Spawning")]
			public Value.Enum<ModelType> Type
			{
				get;
				private set;
			}

			public ModelParameter()
			{
                Model = new Value.Path(Properties.Resources.ModelFilter, true, "");				
				Type = new Value.Enum<ModelType>(ModelType.Random);
			}
		}

		public class CircleParameter
		{
			[Name(value = "分割数", language = Language.Japanese)]
			[Name(value = "Verticies", language = Language.English)]
			public Value.Int Division
			{
				get;
				private set;
			}

			[Name(value = "半径", language = Language.Japanese)]
			[Name(value = "Radius", language = Language.English)]
			public Value.FloatWithRandom Radius
			{
				get;
				private set;
			}

			[Name(value = "開始角度", language = Language.Japanese)]
			[Name(value = "Init Angle", language = Language.English)]
			public Value.FloatWithRandom AngleStart
			{
				get;
				private set;
			}

			[Name(value = "終了角度", language = Language.Japanese)]
			[Name(value = "Final Angle", language = Language.English)]
			public Value.FloatWithRandom AngleEnd
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "生成位置種類")]
			[Name(language = Language.English, value = "Spawn Mode")]
			public Value.Enum<CircleType> Type
			{
				get;
				private set;
			}

			public CircleParameter()
			{
				Division = new Value.Int(8, int.MaxValue, 1);
				Radius = new Value.FloatWithRandom();
				AngleStart = new Value.FloatWithRandom(0, float.MaxValue, float.MinValue);
				AngleEnd = new Value.FloatWithRandom(360, float.MaxValue, float.MinValue);
				Type = new Value.Enum<CircleType>(CircleType.Random);
			}
		}

		public enum ParameterType : int
		{
			[Name(value = "点", language = Language.Japanese)]
			[Name(value = "Point", language = Language.English)]
			Point = 0,
			[Name(value = "円", language = Language.Japanese)]
			[Name(value = "Circle", language = Language.English)]
			Circle = 3,
			[Name(value = "球", language = Language.Japanese)]
			[Name(value = "Sphere", language = Language.English)]
			Sphere = 1,
			[Name(value = "モデル", language = Language.Japanese)]
			[Name(value = "Model", language = Language.English)]
			Model = 2,
			
		}

		public enum ModelType : int
		{
			[Name(value = "ランダム", language = Language.Japanese)]
			[Name(value = "Random", language = Language.English)]
			Random = 0,
			[Name(value = "頂点", language = Language.Japanese)]
			[Name(value = "Vertex", language = Language.English)]
			Vertex = 1,
			[Name(value = "頂点(ランダム)", language = Language.Japanese)]
			[Name(value = "Random Vertex", language = Language.English)]
			RandomVertex = 2,
			[Name(value = "面", language = Language.Japanese)]
			[Name(value = "Surface", language = Language.English)]
			Face = 3,
			[Name(value = "面(ランダム)", language = Language.Japanese)]
			[Name(value = "Random Surface", language = Language.English)]
			RandomFace = 4,
		}

		public enum CircleType : int
		{
			[Name(value = "ランダム", language = Language.Japanese)]
			[Name(value = "Random", language = Language.English)]
			Random = 0,
			[Name(value = "正順", language = Language.Japanese)]
			[Name(value = "Clockwise", language = Language.English)]
			Order = 1,
			[Name(value = "逆順", language = Language.Japanese)]
			[Name(value = "Counter", language = Language.English)]
			ReverseOrder = 2,
		}
	}
}
