using Effekseer.Data.Value;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum AxisType
	{
		[Key(key = "SpawnMethod_AxisType_Xaxis")]
		XAxis,

		[Key(key = "SpawnMethod_AxisType_Yaxis")]
		YAxis,

		[Key(key = "SpawnMethod_AxisType_Zaxis")]
		ZAxis,
	}

	public class GenerationLocationValues
	{
		public enum ModelCoordinateType
		{
			[Key(key = "SpawnMethod_ModelCoordinate_Local")]
			Local = 0,
			[Key(key = "SpawnMethod_ModelCoordinate_World")]
			World = 1,
		}

		[Key(key = "SpawnMethod_EffectsRotation")]
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
			[Key(key = "SpawnMethod_Point_Location")]
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
			[Key(key = "SpawnMethod_Line_Division")]
			public Value.Int Division
			{
				get;
				private set;
			}

			[Key(key = "SpawnMethod_Line_PositionStart")]
			public Value.Vector3DWithRandom PositionStart
			{
				get;
				private set;
			}

			[Key(key = "SpawnMethod_Line_PositionEnd")]
			public Value.Vector3DWithRandom PositionEnd
			{
				get;
				private set;
			}

			[Key(key = "SpawnMethod_Line_PositionNoize")]
			public Value.FloatWithRandom PositionNoize
			{
				get;
				private set;
			}

			[Key(key = "SpawnMethod_Line_Type")]
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
			[Key(key = "SpawnMethod_Sphere_Radius")]
			public Value.FloatWithRandom Radius
			{
				get;
				private set;
			}

			[Key(key = "SpawnMethod_Sphere_RotationX")]
			public Value.FloatWithRandom RotationX
			{
				get;
				private set;
			}

			[Key(key = "SpawnMethod_Sphere_RotationY")]
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
			const int ModelReferenceTypeID = 100;

			[Selector(ID = ModelReferenceTypeID)]
			public Value.Enum<ModelReferenceType> ModelReference
			{
				get;
				private set;
			} = new Enum<ModelReferenceType>(ModelReferenceType.File);

			[Selected(ID = 100, Value = (int)ModelReferenceType.File)]
			[Key(key = "SpawnMethod_Model_Model")]

			public Value.PathForModel Model
			{
				get;
				private set;
			}

			[Selected(ID = ModelReferenceTypeID, Value = (int)ModelReferenceType.ProceduralModel)]
			public ProceduralModelReference Reference
			{
				get;
				private set;
			} = new ProceduralModelReference();

			[Selected(ID = ModelReferenceTypeID, Value = (int)ModelReferenceType.ExternalModel)]
			[Key(key = "SpawnMethod_Model_ExternalIndex")]
			public Value.Int ExternalModelIndex
			{
				get;
				private set;
			} = new Value.Int(0, ExternalModelValues.SlotCount - 1, 0);

			[Selected(ID = ModelReferenceTypeID, Value = (int)ModelReferenceType.ExternalModel)]
			[Key(key = "SpawnMethod_Model_Coordinate")]
			public Value.Enum<ModelCoordinateType> Coordinate
			{
				get;
				private set;
			} = new Value.Enum<ModelCoordinateType>(ModelCoordinateType.Local);

			[Key(key = "SpawnMethod_Model_Type")]
			public Value.Enum<ModelType> Type
			{
				get;
				private set;
			}

			public ModelParameter(Path basepath)
			{
				Model = new Value.PathForModel(basepath, new MultiLanguageString("ModelFilter"), true, "");
				Type = new Value.Enum<ModelType>(ModelType.Random);
			}
		}

		public class CircleParameter
		{
			[Key(key = "SpawnMethod_Circle_AxisDirection")]
			public Value.Enum<AxisType> AxisDirection
			{
				get;
				private set;
			}

			[Key(key = "SpawnMethod_Circle_Division")]
			public Value.Int Division
			{
				get;
				private set;
			}

			[Key(key = "SpawnMethod_Circle_Radius")]
			public Value.FloatWithRandom Radius
			{
				get;
				private set;
			}

			[Key(key = "SpawnMethod_Circle_AngleStart")]
			public Value.FloatWithRandom AngleStart
			{
				get;
				private set;
			}

			[Key(key = "SpawnMethod_Circle_AngleEnd")]
			public Value.FloatWithRandom AngleEnd
			{
				get;
				private set;
			}

			[Key(key = "SpawnMethod_Circle_AngleNoize")]
			public Value.FloatWithRandom AngleNoize
			{
				get;
				private set;
			}

			[Key(key = "SpawnMethod_Circle_Type")]
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
			[Key(key = "SpawnMethod_ParameterType_Point")]
			Point = 0,
			[Key(key = "SpawnMethod_ParameterType_Line")]
			Line = 4,
			[Key(key = "SpawnMethod_ParameterType_Circle")]
			Circle = 3,
			[Key(key = "SpawnMethod_ParameterType_Sphere")]
			Sphere = 1,
			[Key(key = "SpawnMethod_ParameterType_Model")]
			Model = 2,
		}

		public enum ModelType : int
		{
			[Key(key = "SpawnMethod_ModelType_Random")]
			Random = 0,
			[Key(key = "SpawnMethod_ModelType_Vertex")]
			Vertex = 1,
			[Key(key = "SpawnMethod_ModelType_RandomVertex")]
			RandomVertex = 2,
			[Key(key = "SpawnMethod_ModelType_Face")]
			Face = 3,
			[Key(key = "SpawnMethod_ModelType_RandomFace")]
			RandomFace = 4,
		}

		public enum LineType : int
		{
			[Key(key = "SpawnMethod_LineType_Random")]
			Random = 0,
			[Key(key = "SpawnMethod_LineType_Order")]
			Order = 1,
		}

		public enum CircleType : int
		{
			[Key(key = "SpawnMethod_CircleType_Random")]
			Random = 0,
			[Key(key = "SpawnMethod_CircleType_Order")]
			Order = 1,
			[Key(key = "SpawnMethod_CircleType_ReverseOrder")]
			ReverseOrder = 2,
		}
	}
}
