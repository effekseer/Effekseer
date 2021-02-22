using Effekseer.Data.Value;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum ProcedualModelType : int
	{
		[Key(key = "ProcedualModelType_Mesh")]
		Mesh,
		[Key(key = "ProcedualModelType_Ribbon")]
		Ribbon,
	}
	public enum ProcedualModelPrimitiveType : int
	{
		[Key(key = "ProcedualModelPrimitiveType_Sphere")]
		Sphere,
		[Key(key = "ProcedualModelPrimitiveType_Cone")]
		Cone,
		[Key(key = "ProcedualModelPrimitiveType_Cylinder")]
		Cylinder,
		[Key(key = "ProcedualModelPrimitiveType_Spline")]
		Spline4,
	}

	public enum ProcedualModelCrossSectionType : int
	{
		[Key(key = "ProcedualModelCrossSectionType_Plane")]
		Plane,
		[Key(key = "ProcedualModelCrossSectionType_Cross")]
		Cross,
		[Key(key = "ProcedualModelCrossSectionType_Point")]
		Point,
	}

	public enum ProcedualModelAxisType : int
	{
		[Key(key = "ProcedualModelAxisType_X")]
		X,
		[Key(key = "ProcedualModelAxisType_Y")]
		Y,
		[Key(key = "ProcedualModelAxisType_Z")]
		Z,
	}

	public class ProceduralModelMeshParameter
	{
		[Selected(Value = (int)ProcedualModelType.Mesh)]
		[Key(key = "PM_AngleBeginEnd")]
		public Value.Vector2D AngleBeginEnd { get; private set; } = new Value.Vector2D(0.0f, 360.0f);

		[Selected(Value = (int)ProcedualModelType.Mesh)]
		[Key(key = "PM_Divisions")]
		public Value.Int2 Divisions { get; private set; } = new Value.Int2(10, 10, x_min: 2, y_min: 2);

		[Selected(Value = (int)ProcedualModelType.Mesh)]
		[Key(key = "PM_Rotate")]
		public Value.Float Rotate { get; private set; } = new Value.Float(0.0f);

	}

	public class ProceduralModelRibbonParameter
	{
		[Selected(Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_CrossSection")]
		public Value.Enum<ProcedualModelCrossSectionType> CrossSection { get; private set; } = new Value.Enum<ProcedualModelCrossSectionType>(ProcedualModelCrossSectionType.Plane);

		[Selected(Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_Rotate")]
		public Value.Float Rotate { get; private set; } = new Value.Float(1.0f);

		[Selected(Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_Vertices")]
		public Value.Int Vertices { get; private set; } = new Value.Int(10, int.MaxValue, 2, 1);


		[Selected(Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_RibbonScales")]
		public Value.Vector2D RibbonScales { get; private set; } = new Value.Vector2D(0.2f, 0.2f, x_step: 0.01f, y_step: 0.01f);

		[Selected(Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_RibbonAngles")]
		public Value.Vector2D RibbonAngles { get; private set; } = new Value.Vector2D(0.0f, 0.0f);

		[Selected(Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_RibbonNoises")]
		public Value.Vector2D RibbonNoises { get; private set; } = new Value.Vector2D(0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, x_step: 0.1f, y_step: 0.1f);

		[Selected(Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_Count")]
		public Value.Int Count { get; private set; } = new Value.Int(2, int.MaxValue, 1);
	}

	public class ProceduralModelShapeParameter
	{
		const int SelecterPrimitive = 200;

		[Selector(ID = SelecterPrimitive)]
		[Key(key = "PM_PrimitiveType")]
		public Value.Enum<ProcedualModelPrimitiveType> PrimitiveType { get; private set; } = new Value.Enum<ProcedualModelPrimitiveType>(ProcedualModelPrimitiveType.Sphere);

		[Selected(ID = SelecterPrimitive, Value = (int)ProcedualModelPrimitiveType.Sphere)]
		[Selected(ID = SelecterPrimitive, Value = (int)ProcedualModelPrimitiveType.Cone)]
		[Selected(ID = SelecterPrimitive, Value = (int)ProcedualModelPrimitiveType.Cylinder)]
		[Key(key = "PM_Radius")]
		public Value.Float Radius { get; private set; } = new Value.Float(1.0f);

		[Selected(ID = SelecterPrimitive, Value = (int)ProcedualModelPrimitiveType.Cylinder)]
		[Key(key = "PM_Radius2")]
		public Value.Float Radius2 { get; private set; } = new Value.Float(1.0f);

		[Selected(ID = SelecterPrimitive, Value = (int)ProcedualModelPrimitiveType.Cone)]
		[Selected(ID = SelecterPrimitive, Value = (int)ProcedualModelPrimitiveType.Cylinder)]
		[Key(key = "PM_Depth")]
		public Value.Float Depth { get; private set; } = new Value.Float(1.0f);

		[Selected(ID = SelecterPrimitive, Value = (int)ProcedualModelPrimitiveType.Sphere)]
		[Key(key = "PM_DepthMin")]
		public Value.Float DepthMin { get; private set; } = new Value.Float(-1.0f);

		[Selected(ID = SelecterPrimitive, Value = (int)ProcedualModelPrimitiveType.Sphere)]
		[Key(key = "PM_DepthMax")]
		public Value.Float DepthMax { get; private set; } = new Value.Float(1.0f);

		[Key(key = "PM_Point1")]
		[Selected(ID = SelecterPrimitive, Value = (int)ProcedualModelPrimitiveType.Spline4)]
		public Value.Vector2D Point1 { get; private set; } = new Value.Vector2D(1.0f, 0.0f);

		[Key(key = "PM_Point2")]
		[Selected(ID = SelecterPrimitive, Value = (int)ProcedualModelPrimitiveType.Spline4)]
		public Value.Vector2D Point2 { get; private set; } = new Value.Vector2D(1.0f, 0.5f);

		[Key(key = "PM_Point3")]
		[Selected(ID = SelecterPrimitive, Value = (int)ProcedualModelPrimitiveType.Spline4)]
		public Value.Vector2D Point3 { get; private set; } = new Value.Vector2D(1.0f, 1.0f);

		[Key(key = "PM_Point4")]
		[Selected(ID = SelecterPrimitive, Value = (int)ProcedualModelPrimitiveType.Spline4)]
		public Value.Vector2D Point4 { get; private set; } = new Value.Vector2D(1.0f, 2.0f);

		[Key(key = "PM_AxisType")]
		public Enum<AxisType> AxisType { get; private set; } = new Enum<AxisType>(Data.AxisType.YAxis);
	}

	public class ProceduralModelShapeNoiseParameter
	{
		[Key(key = "PM_TiltNoiseFreq")]
		public Value.Vector2D TiltNoiseFrequency { get; private set; } = new Value.Vector2D(1.0f, 1.0f, x_step: 0.01f, y_step: 0.01f);

		[Key(key = "PM_TiltNoiseOffset")]
		public Value.Vector2D TiltNoiseOffset { get; private set; } = new Value.Vector2D(0.0f, 0.0f, x_step: 0.01f, y_step: 0.01f);

		[Key(key = "PM_TiltNoisePower")]
		public Value.Vector2D TiltNoisePower { get; private set; } = new Value.Vector2D(0.0f, 0.0f, x_step: 0.01f, y_step: 0.01f);

		[Key(key = "PM_WaveNoiseFreq")]
		public Value.Vector3D WaveNoiseFrequency { get; private set; } = new Value.Vector3D(1.0f, 1.0f, 1.0f, x_step: 0.01f, y_step: 0.01f, z_step: 0.01f);

		[Key(key = "PM_WaveNoiseOffset")]
		public Value.Vector3D WaveNoiseOffset { get; private set; } = new Value.Vector3D(0.0f, 0.0f, 0.0f, x_step: 0.01f, y_step: 0.01f, z_step: 0.01f);

		[Key(key = "PM_WaveNoisePower")]
		public Value.Vector3D WaveNoisePower { get; private set; } = new Value.Vector3D(0.0f, 0.0f, 0.0f, x_step: 0.01f, y_step: 0.01f, z_step: 0.01f);

		[Key(key = "PM_CurlNoiseFreq")]
		public Value.Vector3D CurlNoiseFrequency { get; private set; } = new Value.Vector3D(1.0f, 1.0f, 1.0f, x_step: 0.01f, y_step: 0.01f, z_step: 0.01f);

		[Key(key = "PM_CurlNoiseOffset")]
		public Value.Vector3D CurlNoiseOffset { get; private set; } = new Value.Vector3D(0.0f, 0.0f, 0.0f, x_step: 0.01f, y_step: 0.01f, z_step: 0.01f);
		[Key(key = "PM_CurlNoisePower")]
		public Value.Vector3D CurlNoisePower { get; private set; } = new Value.Vector3D(0.0f, 0.0f, 0.0f, x_step: 0.01f, y_step: 0.01f, z_step: 0.01f);
	}

	public class ProceduralModelVertexColorParameter
	{
		[Key(key = "PM_ColorUpperLeft")]
		public Value.Color ColorUpperLeft { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorUpperCenter")]
		public Value.Color ColorUpperCenter { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorUpperRight")]
		public Value.Color ColorUpperRight { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorMiddleLeft")]
		public Value.Color ColorMiddleLeft { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorMiddleCenter")]
		public Value.Color ColorMiddleCenter { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorMiddleRight")]
		public Value.Color ColorMiddleRight { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorLowerLeft")]
		public Value.Color ColorLowerLeft { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorLowerCenter")]
		public Value.Color ColorLowerCenter { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorLowerRight")]
		public Value.Color ColorLowerRight { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorCenterPosition")]
		public Value.Vector2D ColorCenterPosition { get; private set; } = new Value.Vector2D(0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, x_step: 0.01f, y_step: 0.01f);

		[Key(key = "PM_ColorCenterArea")]
		public Value.Vector2D ColorCenterArea { get; private set; } = new Value.Vector2D(0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, x_step: 0.01f, y_step: 0.01f);
	}

	public class ProceduralModelVertexColorNoiseParameter
	{
		[Key(key = "PM_VertexColorNoiseFreq")]
		public Value.Vector3D NoiseFrequency { get; private set; } = new Value.Vector3D(1.0f, 1.0f, 1.0f, x_step: 0.01f, y_step: 0.01f, z_step: 0.01f);

		[Key(key = "PM_VertexColorNoiseOffset")]
		public Value.Vector3D NoiseOffset { get; private set; } = new Value.Vector3D(0.0f, 0.0f, 0.0f, x_step: 0.01f, y_step: 0.01f, z_step: 0.01f);
		[Key(key = "PM_VertexColorNoisePower")]
		public Value.Vector3D NoisePower { get; private set; } = new Value.Vector3D(0.0f, 0.0f, 0.0f, x_step: 0.01f, y_step: 0.01f, z_step: 0.01f);
	}

	public class ProcedualModelParameter : INamedObject
	{
		const int SelecterType = 100;
		const int SelecterPrimitive = 200;

		public Value.String Name { get; private set; } = new Value.String("New Model");

		[Selector(ID = SelecterType)]
		[Key(key = "PM_Type")]
		public Value.Enum<ProcedualModelType> Type { get; private set; } = new Value.Enum<ProcedualModelType>(ProcedualModelType.Mesh);

		[IO(Export = true)]
		[Selected(ID = SelecterType, Value = (int)ProcedualModelType.Mesh)]
		public ProceduralModelMeshParameter Mesh { get; private set; } = new ProceduralModelMeshParameter();

		[IO(Export = true)]
		[Selected(ID = SelecterType, Value = (int)ProcedualModelType.Ribbon)]
		public ProceduralModelRibbonParameter Ribbon { get; private set; } = new ProceduralModelRibbonParameter();

		[IO(Export = true)]
		[TreeNode(key = "PM_Shape", id = "PM_Shape")]
		public ProceduralModelShapeParameter Shape { get; private set; } = new ProceduralModelShapeParameter();

		[IO(Export = true)]
		[TreeNode(key = "PM_ShapeNoise", id = "PM_ShapeNoise")]
		public ProceduralModelShapeNoiseParameter ShapeNoise { get; private set; } = new ProceduralModelShapeNoiseParameter();

		[IO(Export = true)]
		[TreeNode(key = "PM_VertexColor", id = "PM_VertexColor")]
		public ProceduralModelVertexColorParameter VertexColor { get; private set; } = new ProceduralModelVertexColorParameter();

		[IO(Export = true)]
		[TreeNode(key = "PM_VertexColorNoise", id = "PM_VertexColorNoise")]
		public ProceduralModelVertexColorNoiseParameter VertexColorNoise { get; private set; } = new ProceduralModelVertexColorNoiseParameter();
	}

	public class ProcedualModelCollection : Value.ObjectCollection<ProcedualModelParameter>
	{
		public ProcedualModelCollection()
		{
			CreateValue += () =>
			{
				return new ProcedualModelParameter();
			};

			GetEditableValues += (v) =>
			{
				List<EditableValue> ret = new List<EditableValue>();

				EditableValue value = new EditableValue();
				value.Value = v;
				value.Title = Resources.GetString("ProcedualModel");
				value.IsUndoEnabled = true;
				ret.Add(value);

				return ret.ToArray();
			};
		}
	}

	public class ProcedualModelValues
	{
		public ProcedualModelCollection ProcedualModels { get; private set; } = new ProcedualModelCollection();

		public ProcedualModelValues()
		{
		}
	}

	public class ProcedualModelReference : ObjectReference<ProcedualModelParameter>
	{
		public ProcedualModelReference()
		{
			GetHolder += (ProcedualModelParameter e) =>
			{
				return Core.ProcedualModel.ProcedualModels.GetHolder(e);
			};

			GetValueWithIndex += (int ind) =>
			{
				if (0 <= ind && ind < Core.ProcedualModel.ProcedualModels.Values.Count)
				{
					return Core.ProcedualModel.ProcedualModels.Values[ind];
				}
				return null;
			};
		}
	}
}