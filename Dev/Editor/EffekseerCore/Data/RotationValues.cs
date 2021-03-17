using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class RotationValues
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
		public AxisPVAParamater AxisPVA
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 4)]
		[IO(Export = true)]
		public AxisEasingParamater AxisEasing
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 5)]
		[IO(Export = true)]
		public RotationFCurveParamater RotationFCurve
		{
			get;
			private set;
		}

		internal RotationValues()
		{
			Type = new Value.Enum<ParamaterType>(ParamaterType.Fixed);
			Fixed = new FixedParamater();
			PVA = new PVAParamater();
			Easing = new Vector3DEasingParamater();
			AxisPVA = new AxisPVAParamater();
			AxisEasing = new AxisEasingParamater();
			RotationFCurve = new RotationFCurveParamater();

			// dynamic parameter
			Fixed.Rotation.CanSelectDynamicEquation = true;
			PVA.Rotation.CanSelectDynamicEquation = true;
			PVA.Velocity.CanSelectDynamicEquation = true;
			PVA.Acceleration.CanSelectDynamicEquation = true;
			Easing.Start.CanSelectDynamicEquation = true;
			Easing.End.CanSelectDynamicEquation = true;
		}

		public class FixedParamater
		{
			[Key(key = "Rotation_FixedParamater_Rotation")]
			public Value.Vector3D Rotation
			{
				get;
				private set;
			}

			internal FixedParamater()
			{
				Rotation = new Value.Vector3D(0, 0, 0);
			}
		}

		public class PVAParamater
		{
			[Key(key = "Rotation_PVAParamater_Rotation")]
			public Value.Vector3DWithRandom Rotation
			{
				get;
				private set;
			}

			[Key(key = "Rotation_PVAParamater_Velocity")]
			public Value.Vector3DWithRandom Velocity
			{
				get;
				private set;
			}

			[Key(key = "Rotation_PVAParamater_Acceleration")]
			public Value.Vector3DWithRandom Acceleration
			{
				get;
				private set;
			}

			internal PVAParamater()
			{
				Rotation = new Value.Vector3DWithRandom(0, 0, 0);
				Velocity = new Value.Vector3DWithRandom(0, 0, 0);
				Acceleration = new Value.Vector3DWithRandom(0, 0, 0);
			}
		}

		public class AxisPVAParamater
		{
			[Key(key = "Rotation_AxisPVAParamater_Axis")]
			public Value.Vector3DWithRandom Axis
			{
				get;
				private set;
			}

			[Key(key = "Rotation_AxisPVAParamater_Rotation")]
			public Value.FloatWithRandom Rotation
			{
				get;
				private set;
			}

			[Key(key = "Rotation_AxisPVAParamater_Velocity")]
			public Value.FloatWithRandom Velocity
			{
				get;
				private set;
			}

			[Key(key = "Rotation_AxisPVAParamater_Acceleration")]
			public Value.FloatWithRandom Acceleration
			{
				get;
				private set;
			}

			internal AxisPVAParamater()
			{
				Axis = new Value.Vector3DWithRandom(0, 1, 0);
				Rotation = new Value.FloatWithRandom(0);
				Velocity = new Value.FloatWithRandom(0);
				Acceleration = new Value.FloatWithRandom(0);
			}
		}
		

		public class AxisEasingParamater
		{
			[Key(key = "Rotation_AxisEasingParamater_Axis")]
			public Value.Vector3DWithRandom Axis
			{
				get;
				private set;
			}

			[IO(Export = true)]
			public FloatEasingParamater Easing
			{
				get;
				private set;
			}

			internal AxisEasingParamater()
			{
				Axis = new Value.Vector3DWithRandom(0, 1, 0);
				Easing = new FloatEasingParamater();
			}
		}

		public class RotationFCurveParamater
		{
			[Key(key = "FCurve")]
			[Shown(Shown = true)]
			public Value.FCurveVector3D FCurve
			{
				get;
				private set;
			}

			public RotationFCurveParamater()
			{
				FCurve = new Value.FCurveVector3D();

				FCurve.X.DefaultValueRangeMax = 180.0f;
				FCurve.X.DefaultValueRangeMin = -180.0f;
				FCurve.Y.DefaultValueRangeMax = 180.0f;
				FCurve.Y.DefaultValueRangeMin = -180.0f;
				FCurve.Z.DefaultValueRangeMax = 180.0f;
				FCurve.Z.DefaultValueRangeMin = -180.0f;
			}
		}

		public enum ParamaterType : int
		{
			[Key(key = "Rotation_ParamaterType_Fixed")]
			Fixed = 0,
			[Key(key = "Rotation_ParamaterType_PVA")]
			PVA = 1,
			[Key(key = "Rotation_ParamaterType_Easing")]
			Easing = 2,
			[Key(key = "Rotation_ParamaterType_AxisPVA")]
			AxisPVA = 3,
			[Key(key = "Rotation_ParamaterType_AxisEasing")]
			AxisEasing = 4,
			[Key(key = "Rotation_ParamaterType_RotationFCurve")]
			RotationFCurve = 5,
		}
	}
}
