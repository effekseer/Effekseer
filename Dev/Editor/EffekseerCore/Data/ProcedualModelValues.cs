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

	public class ProcedualModelParameter : INamedObject
	{
		const int SelecterType = 100;
		const int SelecterPrimitive = 200;

		public Value.String Name { get; private set; } = new Value.String("New Model");

		[Selector(ID = SelecterType)]
		[Key(key = "PM_Type")]
		public Value.Enum<ProcedualModelType> Type { get; private set; } = new Value.Enum<ProcedualModelType>(ProcedualModelType.Mesh);

		[Selected(ID = SelecterType, Value = (int)ProcedualModelType.Mesh)]
		[Key(key = "PM_AngleBeginEnd")]
		public Value.Vector2D AngleBeginEnd { get; private set; } = new Value.Vector2D(0.0f, 360.0f);

		[Selected(ID = SelecterType, Value = (int)ProcedualModelType.Mesh)]
		[Key(key = "PM_Divisions")]
		public Value.Int2 Divisions { get; private set; } = new Value.Int2(10, 10, x_min: 2, y_min: 2);

		[Selected(ID = SelecterType, Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_CrossSection")]
		public Value.Enum<ProcedualModelCrossSectionType> CrossSection { get; private set; } = new Value.Enum<ProcedualModelCrossSectionType>(ProcedualModelCrossSectionType.Plane);

		[Selected(ID = SelecterType, Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_Rotate")]
		public Value.Float Rotate { get; private set; } = new Value.Float(1.0f);

		[Selected(ID = SelecterType, Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_Vertices")]
		public Value.Int Vertices { get; private set; } = new Value.Int(10, int.MaxValue, 2, 1);


		[Selected(ID = SelecterType, Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_RibbonScales")]
		public Value.Vector2D RibbonScales { get; private set; } = new Value.Vector2D(0.2f, 0.2f, x_step: 0.01f, y_step: 0.01f);

		[Selected(ID = SelecterType, Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_RibbonAngles")]
		public Value.Vector2D RibbonAngles { get; private set; } = new Value.Vector2D(0.2f, 0.2f);

		[Selected(ID = SelecterType, Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_RibbonNoises")]
		public Value.Vector2D RibbonNoises { get; private set; } = new Value.Vector2D(0.0f, 0.0f);

		[Selected(ID = SelecterType, Value = (int)ProcedualModelType.Ribbon)]
		[Key(key = "PM_Count")]
		public Value.Int Count { get; private set; } = new Value.Int(2, int.MaxValue, 1);

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
		[Key(key = "PM_WaveNoisePower")]
		public Value.Vector3D CurlNoisePower { get; private set; } = new Value.Vector3D(0.0f, 0.0f, 0.0f, x_step: 0.01f, y_step: 0.01f, z_step: 0.01f);

		[Key(key = "PM_ColorLeft")]
		public Value.Color ColorLeft { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorCenter")]
		public Value.Color ColorCenter { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorRight")]
		public Value.Color ColorRight { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorLeftMiddle")]
		public Value.Color ColorLeftMiddle { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorCenterMiddle")]
		public Value.Color ColorCenterMiddle { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorRightMiddle")]
		public Value.Color ColorRightMiddle { get; private set; } = new Value.Color(255, 255, 255, 255);

		[Key(key = "PM_ColorCenterArea")]
		public Value.Vector2D ColorCenterArea { get; private set; } = new Value.Vector2D(0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, x_step: 0.01f, y_step: 0.01f);

		public override bool Equals(object obj)
		{
			var param = obj as ProcedualModelParameter;
			if (param == null)
				return false;

			if (Type.Value != param.Type.Value)
				return false;

			if (Type.Value == ProcedualModelType.Mesh)
			{
				if (!AngleBeginEnd.ValueEquals(param.AngleBeginEnd))
					return false;
				if (!Divisions.ValueEquals(param.Divisions))
					return false;
			}
			else if (Type.Value == ProcedualModelType.Ribbon)
			{
				if (CrossSection.Value != param.CrossSection.Value)
					return false;
				if (Rotate.Value != param.Rotate.Value)
					return false;
				if (Vertices.Value != param.Vertices.Value)
					return false;
				if (!RibbonScales.ValueEquals(param.RibbonScales))
					return false;
				if (!RibbonAngles.ValueEquals(param.RibbonAngles))
					return false;
				if (!RibbonNoises.ValueEquals(param.RibbonNoises))
					return false;
				if (Count.Value != param.Count.Value)
					return false;
			}
			else
			{
				throw new Exception();
			}


			if (PrimitiveType.Value != param.PrimitiveType.Value)
				return false;

			if (PrimitiveType.Value == ProcedualModelPrimitiveType.Sphere)
			{
				if (Radius.Value != param.Radius.Value)
					return false;
				if (DepthMin.Value != param.DepthMin.Value)
					return false;
				if (DepthMax.Value != param.DepthMax.Value)
					return false;
			}
			else if (PrimitiveType.Value == ProcedualModelPrimitiveType.Cone)
			{
				if (Radius.Value != param.Radius.Value)
					return false;
				if (Depth.Value != param.Depth.Value)
					return false;
			}
			else if (PrimitiveType.Value == ProcedualModelPrimitiveType.Cylinder)
			{
				if (Radius.Value != param.Radius.Value)
					return false;
				if (Radius2.Value != param.Radius2.Value)
					return false;
				if (Depth.Value != param.Depth.Value)
					return false;
			}
			else if (PrimitiveType.Value == ProcedualModelPrimitiveType.Spline4)
			{
				if (!Point1.ValueEquals(param.Point1))
					return false;
				if (!Point2.ValueEquals(param.Point2))
					return false;
				if (!Point3.ValueEquals(param.Point3))
					return false;
				if (!Point4.ValueEquals(param.Point4))
					return false;
			}
			else
			{
				throw new Exception();
			}

			if (AxisType.Value != param.AxisType.Value)
				return false;

			if (!TiltNoiseFrequency.ValueEquals(param.TiltNoiseFrequency))
				return false;
			if (!TiltNoiseOffset.ValueEquals(param.TiltNoiseOffset))
				return false;
			if (!TiltNoisePower.ValueEquals(param.TiltNoisePower))
				return false;

			if (!WaveNoiseFrequency.ValueEquals(param.WaveNoiseFrequency))
				return false;
			if (!WaveNoiseOffset.ValueEquals(param.WaveNoiseOffset))
				return false;
			if (!WaveNoisePower.ValueEquals(param.WaveNoisePower))
				return false;

			if (!CurlNoiseFrequency.ValueEquals(param.CurlNoiseFrequency))
				return false;
			if (!CurlNoiseOffset.ValueEquals(param.CurlNoiseOffset))
				return false;
			if (!CurlNoisePower.ValueEquals(param.CurlNoisePower))
				return false;

			if (!ColorLeft.ValueEquals(param.ColorLeft))
				return false;
			if (!ColorCenter.ValueEquals(param.ColorCenter))
				return false;
			if (!ColorRight.ValueEquals(param.ColorRight))
				return false;
			if (!ColorLeftMiddle.ValueEquals(param.ColorLeftMiddle))
				return false;
			if (!ColorCenterMiddle.ValueEquals(param.ColorCenterMiddle))
				return false;
			if (!ColorRightMiddle.ValueEquals(param.ColorRightMiddle))
				return false;

			if (!ColorCenterArea.ValueEquals(param.ColorCenterArea))
				return false;

			return true;
		}

		public override int GetHashCode()
		{
			var hash = 0;

			hash = Utils.Misc.CombineHashCodes(new[] { hash, Type.Value.GetHashCode() });

			if (Type.Value == ProcedualModelType.Mesh)
			{
				hash = Utils.Misc.CombineHashCodes(new[] { hash, AngleBeginEnd.X.Value.GetHashCode(), AngleBeginEnd.Y.Value.GetHashCode(), Divisions.GetValueHashCode() });
			}
			else if (Type.Value == ProcedualModelType.Ribbon)
			{
				hash = Utils.Misc.CombineHashCodes(new[] { hash, CrossSection.Value.GetHashCode(), Rotate.Value.GetHashCode(), Vertices.Value.GetHashCode(), RibbonScales.GetValueHashCode(), RibbonAngles.GetValueHashCode(), RibbonNoises.GetValueHashCode(), Count.Value.GetHashCode() });
			}
			else
			{
				throw new Exception();
			}

			hash = Utils.Misc.CombineHashCodes(new[] { hash, PrimitiveType.Value.GetHashCode() });

			if (PrimitiveType.Value == ProcedualModelPrimitiveType.Sphere)
			{
				hash = Utils.Misc.CombineHashCodes(new[] { hash, DepthMin.Value.GetHashCode(), DepthMax.Value.GetHashCode(), Radius.Value.GetHashCode() });
			}
			else if (PrimitiveType.Value == ProcedualModelPrimitiveType.Cone)
			{
				hash = Utils.Misc.CombineHashCodes(new[] { hash, Depth.Value.GetHashCode(), Radius.Value.GetHashCode() });
			}
			else if (PrimitiveType.Value == ProcedualModelPrimitiveType.Cylinder)
			{
				hash = Utils.Misc.CombineHashCodes(new[] { hash, Depth.Value.GetHashCode(), Radius.Value.GetHashCode(), Radius2.Value.GetHashCode() });
			}
			else if (PrimitiveType.Value == ProcedualModelPrimitiveType.Spline4)
			{
				hash = Utils.Misc.CombineHashCodes(new[] { hash, Point1.GetValueHashCode(), Point2.GetValueHashCode(), Point3.GetValueHashCode(), Point4.GetValueHashCode() });
			}
			else
			{
				throw new Exception();
			}

			hash = Utils.Misc.CombineHashCodes(new[] { hash, AxisType.Value.GetHashCode() });

			hash = Utils.Misc.CombineHashCodes(new[] { hash, TiltNoiseFrequency.GetValueHashCode(), TiltNoiseOffset.GetValueHashCode(), TiltNoisePower.GetValueHashCode() });
			hash = Utils.Misc.CombineHashCodes(new[] { hash, WaveNoiseFrequency.GetValueHashCode(), WaveNoiseOffset.GetValueHashCode(), WaveNoisePower.GetValueHashCode() });
			hash = Utils.Misc.CombineHashCodes(new[] { hash, CurlNoiseFrequency.GetValueHashCode(), CurlNoiseOffset.GetValueHashCode(), CurlNoisePower.GetValueHashCode() });

			hash = Utils.Misc.CombineHashCodes(new[] { hash, ColorLeft.GetValueHashCode(), ColorCenter.GetValueHashCode(), ColorRight.GetValueHashCode() });
			hash = Utils.Misc.CombineHashCodes(new[] { hash, ColorLeftMiddle.GetValueHashCode(), ColorCenterMiddle.GetValueHashCode(), ColorRightMiddle.GetValueHashCode() });

			hash = Utils.Misc.CombineHashCodes(new[] { hash, ColorCenterArea.GetValueHashCode() });

			return hash;
		}
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