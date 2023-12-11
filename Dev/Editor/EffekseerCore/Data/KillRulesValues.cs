using System.Collections.Generic;
using Effekseer.Data.Value;

namespace Effekseer.Data
{
	public enum PlaneAxisType : int
	{
		[Key(key = "PlaneAxisType_XPositive")]
		XPositive,
		[Key(key = "PlaneAxisType_XNegative")]
		XNegative,
		[Key(key = "PlaneAxisType_YPositive")]
		YPositive,
		[Key(key = "PlaneAxisType_YNegative")]
		YNegative,
		[Key(key = "PlaneAxisType_ZPositive")]
		ZPositive,
		[Key(key = "PlaneAxisType_ZNegative")]
		ZNegative
	}

	public class KillRulesValues
	{

		public static Dictionary<PlaneAxisType, PlaneAxisSpace> PlaneAxisNormal = new Dictionary<PlaneAxisType, PlaneAxisSpace>()
		{
			{
				PlaneAxisType.XPositive,
				new PlaneAxisSpace(new Vector3D(1F, 0F, 0F), new Vector3D(0F, 1F, 0F), new Vector3D(0F, 0F, 1F))
			},
			{
				PlaneAxisType.XNegative,
				new PlaneAxisSpace(new Vector3D(-1F, 0F, 0F), new Vector3D(0F, 1F, 0F), new Vector3D(0F, 0F, 1F))
			},
			{
				PlaneAxisType.YPositive,
				new PlaneAxisSpace(new Vector3D(0F, 1F, 0F), new Vector3D(1F, 0F, 0F), new Vector3D(0F, 0F, 1F))
			},
			{
				PlaneAxisType.YNegative,
				new PlaneAxisSpace(new Vector3D(0F, -1F, 0F), new Vector3D(1F, 0F, 0F), new Vector3D(0F, 0F, 1F))
			},
			{
				PlaneAxisType.ZPositive,
				new PlaneAxisSpace(new Vector3D(0F, 0F, 1F), new Vector3D(1F, 0F, 0F), new Vector3D(0F, 1F, 0F))
			},
			{
				PlaneAxisType.ZNegative,
				new PlaneAxisSpace(new Vector3D(0F, 0F, -1F), new Vector3D(1F, 0F, 0F), new Vector3D(0F, 1F, 0F))
			},
		};



		[Key(key = "KillRules_Type")]
		[Selector(ID = 1)]
		public Value.Enum<KillType> Type
		{
			get;
			set;
		}


		[Key(key = "KillRules_Box_Center")]
		[Selected(ID = 1, Value = 1)]
		public Value.Vector3D BoxCenter
		{
			get;
			set;
		}

		[Key(key = "KillRules_Box_Size")]
		[Selected(ID = 1, Value = 1)]
		public Value.Vector3D BoxSize
		{
			get;
			set;
		}

		[Key(key = "KillRules_Box_IsKillInside")]
		[Selected(ID = 1, Value = 1)]
		public Value.Boolean BoxIsKillInside
		{
			get;
			set;
		}

		[Key(key = "KillRules_Plane_Axis")]
		[Selected(ID = 1, Value = 2)]
		public Value.Enum<PlaneAxisType> PlaneAxis
		{
			get;
			set;
		}

		[Key(key = "KillRules_Plane_PlaneOffset")]
		[Selected(ID = 1, Value = 2)]
		public Value.Float PlaneOffset
		{
			get;
			set;
		}

		[Key(key = "KillRules_Sphere_Center")]
		[Selected(ID = 1, Value = 3)]
		public Value.Vector3D SphereCenter
		{
			get;
			set;
		}

		[Key(key = "KillRules_Sphere_Radius")]
		[Selected(ID = 1, Value = 3)]
		public Value.Float SphereRadius
		{
			get;
			set;
		}


		[Key(key = "KillRules_Sphere_IsKillInside")]
		[Selected(ID = 1, Value = 3)]
		public Value.Boolean SphereIsKillInside
		{
			get;
			set;
		}


		[Key(key = "KillRules_IsScaleAndRotationApplied")]
		[Selected(ID = 1, Value = 1)]
		[Selected(ID = 1, Value = 2)]
		[Selected(ID = 1, Value = 3)]
		public Value.Boolean IsScaleAndRotationApplied
		{
			get;
			set;
		}

		public KillRulesValues()
		{
			Type = new Enum<KillType>(KillType.None);
			IsScaleAndRotationApplied = new Boolean(true);

			BoxCenter = new Vector3D(0F, 0F, 0F);
			BoxSize = new Vector3D(0.5F, 0.5F, 0.5F,
				float.MaxValue, 0,
				float.MaxValue, 0,
				float.MaxValue, 0);
			BoxIsKillInside = new Boolean(false);

			PlaneAxis = new Enum<PlaneAxisType>(PlaneAxisType.YPositive);
			PlaneOffset = new Float(1.0F);

			SphereCenter = new Vector3D(0.0F, 0.0F, 0.0F);
			SphereRadius = new Float(1F, float.MaxValue, 0F);
			SphereIsKillInside = new Boolean(false);
		}

		public enum KillType : int
		{
			[Key(key = "KillType_None")]
			None,
			[Key(key = "KillType_Box")]
			Box,
			[Key(key = "KillType_Plane")]
			Height,
			[Key(key = "KillType_Sphere")]
			Sphere
		}

		public class PlaneAxisSpace
		{
			public readonly Vector3D Normal;
			public readonly Vector3D Tangent;
			public readonly Vector3D Bitangent;

			public PlaneAxisSpace(Vector3D normal, Vector3D tangent, Vector3D bitangent)
			{
				Normal = normal;
				Tangent = tangent;
				Bitangent = bitangent;
			}
		}
	}
}