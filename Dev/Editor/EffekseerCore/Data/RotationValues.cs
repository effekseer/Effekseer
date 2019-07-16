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
			[Name(language = Language.Japanese, value = "角度(度)")]
			[Description(language = Language.Japanese, value = "インスタンスの角度")]
			[Name(language = Language.English, value = "Angle")]
			[Description(language = Language.English, value = "Rotation of the instance, in degrees")]
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
			[Name(language = Language.Japanese, value = "角度(度)")]
			[Description(language = Language.Japanese, value = "インスタンスの初期角度")]
			[Name(language = Language.English, value = "Angle")]
			[Description(language = Language.English, value = "Initial rotation of the instance, in degrees")]
			public Value.Vector3DWithRandom Rotation
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "角速度(度)")]
			[Description(language = Language.Japanese, value = "インスタンスの角速度")]
			[Name(language = Language.English, value = "Angular\nVelocity")]
			[Description(language = Language.English, value = "Initial angular velocity of the instance, in degrees")]
			public Value.Vector3DWithRandom Velocity
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "角加速度(度)")]
			[Description(language = Language.Japanese, value = "インスタンスの初期角加速度")]
			[Name(language = Language.English, value = "Angular\nAccel")]
			[Description(language = Language.English, value = "Acceleration of the instance's angular velocity, in degrees")]
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
			[Name(language = Language.Japanese, value = "回転軸")]
			[Description(language = Language.Japanese, value = "インスタンスの回転軸")]
			[Name(language = Language.English, value = "Axis of\nRotation")]
			[Description(language = Language.English, value = "Axis of rotation of the instance")]
			public Value.Vector3DWithRandom Axis
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "角度(度)")]
			[Description(language = Language.Japanese, value = "インスタンスの初期角度")]
			[Name(language = Language.English, value = "Angle")]
			[Description(language = Language.English, value = "Initial rotation of the instance, in degrees")]
			public Value.FloatWithRandom Rotation
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "角速度(度)")]
			[Description(language = Language.Japanese, value = "インスタンスの角速度")]
			[Name(language = Language.English, value = "Angular\nVelocity")]
			[Description(language = Language.English, value = "Initial angular velocity of the instance, in degrees")]
			public Value.FloatWithRandom Velocity
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "角加速度(度)")]
			[Description(language = Language.Japanese, value = "インスタンスの初期角加速度")]
			[Name(language = Language.English, value = "Angular\nAccel")]
			[Description(language = Language.English, value = "Acceleration of the instance's angular velocity, in degrees")]
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
			[Name(language = Language.Japanese, value = "回転軸")]
			[Description(language = Language.Japanese, value = "インスタンスの回転軸")]
			[Name(language = Language.English, value = "Axis of\nRotation")]
			[Description(language = Language.English, value = "Axis of rotation of the instance")]
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
			[Name(language = Language.Japanese, value = "Fカーブ")]
			[Description(language = Language.Japanese, value = "Fカーブ")]
			[Name(language = Language.English, value = "F-Curve")]
			[Description(language = Language.English, value = "F-Curve")]
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
			[Name(value = "角度", language = Language.Japanese)]
			[Name(value = "Fixed Angle", language = Language.English)]
			Fixed = 0,
			[Name(value = "角度・速度・加速度", language = Language.Japanese)]
			[Name(value = "PVA", language = Language.English)]
			PVA = 1,
			[Name(value = "イージング", language = Language.Japanese)]
			[Name(value = "Easing", language = Language.English)]
			Easing = 2,
			[Name(value = "任意軸 角度・速度・加速度", language = Language.Japanese)]
			[Name(value = "PVA (Arbitrary Axis)", language = Language.English)]
			AxisPVA = 3,
			[Name(value = "任意軸 イージング", language = Language.Japanese)]
			[Name(value = "Easing (Arbitrary Axis)", language = Language.English)]
			AxisEasing = 4,
			[Name(value = "角度(Fカーブ)", language = Language.Japanese)]
			[Name(value = "Rotation (F-Curve)", language = Language.English)]
			RotationFCurve = 5,
		}
	}
}
