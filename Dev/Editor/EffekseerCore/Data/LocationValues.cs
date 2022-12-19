using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class LocationValues
	{
		[Selector(ID = 0)]
		public Value.Enum<ParamaterType> Type
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 0)]
		[IO(Export = true)]
		public FixedParamater Fixed
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 1)]
		[IO(Export = true)]
		public PVAParamater PVA
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 2)]
		[IO(Export = true)]
		public Vector3DEasingParamater Easing
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 3)]
		[IO(Export = true)]
		public Vector3DFCurveParameter LocationFCurve
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 4)]
		[IO(Export = true)]
		public NurbsCurveParameter NurbsCurve
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 5)]
		[IO(Export = true)]
		public ViewOffsetParameter ViewOffset
		{
			get;
			private set;
		}

		/// <summary>
		/// Dummy
		/// </summary>
		internal LocationValues()
			 : this(null)
		{
		}

		internal LocationValues(Value.Path basepath)
		{
			Type = new Value.Enum<ParamaterType>(ParamaterType.Fixed);
			Fixed = new FixedParamater();
			PVA = new PVAParamater();
			Easing = new Vector3DEasingParamater();
			LocationFCurve = new Vector3DFCurveParameter();
			NurbsCurve = new NurbsCurveParameter(basepath);
			ViewOffset = new ViewOffsetParameter();

			// dynamic parameter
			Fixed.Location.CanSelectDynamicEquation = true;
			PVA.Location.CanSelectDynamicEquation = true;
			PVA.Velocity.CanSelectDynamicEquation = true;
			PVA.Acceleration.CanSelectDynamicEquation = true;
			Easing.Start.CanSelectDynamicEquation = true;
			Easing.Middle.CanSelectDynamicEquation = true;
			Easing.End.CanSelectDynamicEquation = true;
		}

		public class FixedParamater
		{
			[Key(key = "Position_FixedParamater_Location")]
			public Value.Vector3D Location
			{
				get;
				private set;
			}

			internal FixedParamater()
			{
				Location = new Value.Vector3D(0, 0, 0);
			}
		}

		public class PVAParamater
		{
			[Key(key = "Position_PVAParamater_Location")]
			public Value.Vector3DWithRandom Location
			{
				get;
				private set;
			}

			[Key(key = "Position_PVAParamater_Velocity")]
			public Value.Vector3DWithRandom Velocity
			{
				get;
				private set;
			}

			[Key(key = "Position_PVAParamater_Acceleration")]
			public Value.Vector3DWithRandom Acceleration
			{
				get;
				private set;
			}

			internal PVAParamater()
			{
				Location = new Value.Vector3DWithRandom(0, 0, 0);
				Velocity = new Value.Vector3DWithRandom(0, 0, 0);
				Acceleration = new Value.Vector3DWithRandom(0, 0, 0);
			}
		}

		public class NurbsCurveParameter
		{
			public enum NurbsLoopType : int
			{
				[Key(key = "Position_NurbsCurveParameter_NurbsLoopType_Repeat")]
				Repeat = 0,

				[Key(key = "Position_NurbsCurveParameter_NurbsLoopType_Stop")]
				Stop = 1,
			}

			[IO(Export = true)]
			[Key(key = "Position_NurbsCurveParameter_FileParh")]
			public Value.PathForCurve FilePath
			{
				get;
				private set;
			}

			[IO(Export = true)]
			[Key(key = "Position_NurbsCurveParameter_Scale")]
			public Value.Float Scale
			{
				get;
				private set;
			}

			[IO(Export = true)]
			[Key(key = "Position_NurbsCurveParameter_MoveSpeed")]
			public Value.Float MoveSpeed
			{
				get;
				private set;
			}

			[IO(Export = true)]
			[Key(key = "Position_NurbsCurveParameter_LoopType")]
			public Value.Enum<NurbsLoopType> LoopType
			{
				get;
				private set;
			}

			public NurbsCurveParameter(Value.Path basepath)
			{
				FilePath = new Value.PathForCurve(basepath, Resources.GetString("CurveFilter"), true, "");
				Scale = new Value.Float(1.0f);
				MoveSpeed = new Value.Float(1.0f, float.MaxValue, 0.0f);
				LoopType = new Value.Enum<NurbsLoopType>(NurbsLoopType.Repeat);
			}
		}

		public class ViewOffsetParameter
		{
			[IO(Export = true)]
			[Key(key = "Position_ViewOffsetParameter_Distance")]
			public Value.FloatWithRandom Distance
			{
				get;
				private set;
			}

			public ViewOffsetParameter()
			{
				Distance = new Value.FloatWithRandom(3.0f, float.MaxValue, 0.0f, DrawnAs.CenterAndAmplitude, 0.5f);
			}
		}

		public enum ParamaterType : int
		{
			[Key(key = "Position_ParamaterType_Fixed")]
			Fixed = 0,
			[Key(key = "Position_ParamaterType_PVA")]
			PVA = 1,
			[Key(key = "Position_ParamaterType_Easing")]
			Easing = 2,
			[Key(key = "Position_ParamaterType_LocationFCurve")]
			LocationFCurve = 3,
			[Key(key = "Position_ParameterType_NurbsCurve")]
			NurbsCurve = 4,
			[Key(key = "Position_ParameterType_ViewOffset")]
			ViewOffset = 5,
		}
	}
}