using Effekseer.Data.Value;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum AxisType
	{
		[Name(value = "X軸", language = Language.Japanese)]
		[Name(value = "X-Axis", language = Language.English)]
		XAxis,

		[Name(value = "Y軸", language = Language.Japanese)]
		[Name(value = "Y-Axis", language = Language.English)]
		YAxis,

		[Name(value = "Z軸", language = Language.Japanese)]
		[Name(value = "Z-Axis", language = Language.English)]
		ZAxis,
	}

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

		[Selected(ID = 0, Value = 4)]
		[IO(Export = true)]
		public LineParameter Line
		{
			get;
			private set;
		}

		internal GenerationLocationValues(Value.Path basepath)
		{
			EffectsRotation = new Value.Boolean(false);
			Type = new Value.Enum<ParameterType>();
			Point = new PointParameter();
			Sphere = new SphereParameter();
			Model = new ModelParameter(basepath);
			Circle = new CircleParameter();
			Line = new LineParameter();
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

		public class LineParameter
		{
			[Name(value = "分割数", language = Language.Japanese)]
			[Name(value = "Verticies", language = Language.English)]
			public Value.Int Division
			{
				get;
				private set;
			}

			[Name(value = "開始位置", language = Language.Japanese)]
			[Name(value = "Init Position", language = Language.English)]
			public Value.Vector3DWithRandom PositionStart
			{
				get;
				private set;
			}

			[Name(value = "終了位置", language = Language.Japanese)]
			[Name(value = "Final Position", language = Language.English)]
			public Value.Vector3DWithRandom PositionEnd
			{
				get;
				private set;
			}

			[Name(value = "位置ノイズ", language = Language.Japanese)]
			[Name(value = "Position Noize", language = Language.English)]
			public Value.FloatWithRandom PositionNoize
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "生成位置種類")]
			[Name(language = Language.English, value = "Spawn Mode")]
			public Value.Enum<LineType> Type
			{
				get;
				private set;
			}

			public LineParameter()
			{
				Division = new Value.Int(8, int.MaxValue, 1);
				PositionStart = new Value.Vector3DWithRandom();
				PositionEnd = new Value.Vector3DWithRandom();
				PositionNoize = new Value.FloatWithRandom();
				Type = new Value.Enum<LineType>(LineType.Random);
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
			public Value.PathForModel Model
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

			public ModelParameter(Path basepath)
			{
                Model = new Value.PathForModel(basepath, Resources.GetString("ModelFilter"), true, "");				
				Type = new Value.Enum<ModelType>(ModelType.Random);
			}
		}

		public class CircleParameter
		{
			[Name(language = Language.Japanese, value = "軸方向")]
			[Name(language = Language.English, value = "Axis Direction")]
			public Value.Enum<AxisType> AxisDirection
			{
				get;
				private set;
			}

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

			[Name(value = "角度ノイズ", language = Language.Japanese)]
			[Name(value = "Angle Noize", language = Language.English)]
			public Value.FloatWithRandom AngleNoize
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
				AxisDirection = new Value.Enum<AxisType>(AxisType.ZAxis);
				Division = new Value.Int(8, int.MaxValue, 1);
				Radius = new Value.FloatWithRandom();
				AngleStart = new Value.FloatWithRandom(0, float.MaxValue, float.MinValue);
				AngleEnd = new Value.FloatWithRandom(360, float.MaxValue, float.MinValue);
				AngleNoize = new Value.FloatWithRandom(0);
				Type = new Value.Enum<CircleType>(CircleType.Random);
			}
		}

		public enum ParameterType : int
		{
			[Name(value = "点", language = Language.Japanese)]
			[Name(value = "Point", language = Language.English)]
			Point = 0,
			[Name(value = "線", language = Language.Japanese)]
			[Name(value = "Line", language = Language.English)]
			Line = 4,
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

		public enum LineType : int
		{
			[Name(value = "ランダム", language = Language.Japanese)]
			[Name(value = "Random", language = Language.English)]
			Random = 0,
			[Name(value = "順番", language = Language.Japanese)]
			[Name(value = "Order", language = Language.English)]
			Order = 1,
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
