using Effekseer.Data.Value;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class ScaleValues
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

		[Selected(ID = 0, Value = 5)]
		[IO(Export = true)]
		public Vector3DFCurveParameter FCurve
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 3)]
		[IO(Export = true)]
		public SinglePVAParamater SinglePVA
		{
			get;
			private set;
		}
		
		[Selected(ID = 0, Value = 4)]
		[IO(Export = true)]
		public FloatEasingParamater SingleEasing
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = (int)ParamaterType.SingleFCurve)]
		public FCurveScalar SingleFCurve
		{
			get;
			private set;
		} = new FCurveScalar();

		internal ScaleValues()
		{
			Type = new Value.Enum<ParamaterType>(ParamaterType.Fixed);
			Fixed = new FixedParamater();
			PVA = new PVAParamater();
			Easing = new Vector3DEasingParamater(1.0f, 1.0f, 1.0f);
			//Easing.Start.X.SetCenterDirectly(1.0f);
			//Easing.Start.Y.SetCenterDirectly(1.0f);
			//Easing.Start.Z.SetCenterDirectly(1.0f);
			//Easing.Start.X.SetMaxDirectly(1.0f);
			//Easing.Start.Y.SetMaxDirectly(1.0f);
			//Easing.Start.Z.SetMaxDirectly(1.0f);
			//Easing.Start.X.SetMinDirectly(1.0f);
			//Easing.Start.Y.SetMinDirectly(1.0f);
			//Easing.Start.Z.SetMinDirectly(1.0f);
			//Easing.End.X.SetCenterDirectly(1.0f);
			//Easing.End.Y.SetCenterDirectly(1.0f);
			//Easing.End.Z.SetCenterDirectly(1.0f);
			//Easing.End.X.SetMaxDirectly(1.0f);
			//Easing.End.Y.SetMaxDirectly(1.0f);
			//Easing.End.Z.SetMaxDirectly(1.0f);
			//Easing.End.X.SetMinDirectly(1.0f);
			//Easing.End.Y.SetMinDirectly(1.0f);
			//Easing.End.Z.SetMinDirectly(1.0f);
			SinglePVA = new SinglePVAParamater();

			SingleEasing = new FloatEasingParamater(1.0f);
			//SingleEasing.Start.SetCenterDirectly(1.0f);
			//SingleEasing.Start.SetMaxDirectly(1.0f);
			//SingleEasing.Start.SetMinDirectly(1.0f);
			//SingleEasing.End.SetCenterDirectly(1.0f);
			//SingleEasing.End.SetMaxDirectly(1.0f);
			//SingleEasing.End.SetMinDirectly(1.0f);

			FCurve = new Vector3DFCurveParameter();
			FCurve.FCurve.X.DefaultValue = 1.0f;
			FCurve.FCurve.Y.DefaultValue = 1.0f;
			FCurve.FCurve.Z.DefaultValue = 1.0f;

			SingleFCurve.S.DefaultValue = 1.0f;

			// dynamic parameter
			Fixed.Scale.CanSelectDynamicEquation = true;
			PVA.Scale.CanSelectDynamicEquation = true;
			PVA.Velocity.CanSelectDynamicEquation = true;
			PVA.Acceleration.CanSelectDynamicEquation = true;
			Easing.Start.CanSelectDynamicEquation = true;
			Easing.End.CanSelectDynamicEquation = true;
		}

		public class FixedParamater
		{
			[Key(key = "Scale_FixedParamater_Scale")]
			public Value.Vector3D Scale
			{
				get;
				private set;
			}

			internal FixedParamater()
			{
				Scale = new Value.Vector3D(1, 1, 1);
			}
		}

		public class PVAParamater
		{
			[Key(key = "Scale_PVAParamater_Scale")]
			public Value.Vector3DWithRandom Scale
			{
				get;
				private set;
			}

			[Key(key = "Scale_PVAParamater_Velocity")]
			public Value.Vector3DWithRandom Velocity
			{
				get;
				private set;
			}

			[Key(key = "Scale_PVAParamater_Acceleration")]

			public Value.Vector3DWithRandom Acceleration
			{
				get;
				private set;
			}

			internal PVAParamater()
			{
				Scale = new Value.Vector3DWithRandom(1, 1, 1);
				Velocity = new Value.Vector3DWithRandom(0, 0, 0);
				Acceleration = new Value.Vector3DWithRandom(0, 0, 0);
			}
		}

		public class SinglePVAParamater
		{
			[Key(key = "Scale_SinglePVAParamater_Scale")]
			public Value.FloatWithRandom Scale
			{
				get;
				private set;
			}

			[Key(key = "Scale_SinglePVAParamater_Velocity")]
			public Value.FloatWithRandom Velocity
			{
				get;
				private set;
			}

			[Key(key = "Scale_SinglePVAParamater_Acceleration")]
			public Value.FloatWithRandom Acceleration
			{
				get;
				private set;
			}

			internal SinglePVAParamater()
			{
				Scale = new Value.FloatWithRandom(1);
				Velocity = new Value.FloatWithRandom(0);
				Acceleration = new Value.FloatWithRandom(0);
			}
		}

		public enum ParamaterType : int
		{
			[Key(key = "Scale_ParamaterType_Fixed")]
			Fixed = 0,
			[Key(key = "Scale_ParamaterType_PVA")]
			PVA = 1,
			[Key(key = "Scale_ParamaterType_Easing")]
			Easing = 2,
			[Key(key = "Scale_ParamaterType_FCurve")]
			FCurve = 5,
			[Key(key = "Scale_ParamaterType_SinglePVA")]
			SinglePVA = 3,
			[Key(key = "Scale_ParamaterType_SingleEasing")]
			SingleEasing = 4,
			[Key(key = "Scale_ParamaterType_SingleFCurve")]
			SingleFCurve = 6,
		}
	}
}
