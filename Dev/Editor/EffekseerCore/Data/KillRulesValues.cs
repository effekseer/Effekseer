using Effekseer.Data.Value;

namespace Effekseer.Data
{
	public class KillRulesValues
	{
		
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
		
		[Key(key = "KillRules_Box_IsScaleAndRotationApplied")]
		[Selected(ID = 1, Value = 1)]
		public Value.Boolean BoxIsScaleAndRotationApplied
		{
			get;
			set;
		}

		[Key(key = "KillRules_Height_Height")]
		[Selected(ID = 1, Value = 2)]
		public Value.Float Height
		{
			get;
			set;
		}
		
		[Key(key = "KillRules_Height_IsFloor")]
		[Selected(ID = 1, Value = 2)]
		public Value.Boolean HeightIsFloor
		{
			get;
			set;
		}
		
		[Key(key = "KillRules_Height_IsScaleAndRotationApplied")]
		[Selected(ID = 1, Value = 2)]
		public Value.Boolean HeightIsScaleAndRotationApplied
		{
			get;
			set;
		}

		public KillRulesValues()
		{
			Type = new Enum<KillType>(KillType.None);
			
			BoxCenter = new Vector3D(0F, 0F, 0F);
			BoxSize = new Vector3D(0.5F, 0.5F, 0.5F,
				float.MaxValue, 0, 
				float.MaxValue, 0, 
				float.MaxValue, 0);
			
			BoxIsKillInside = new Boolean(false);
			BoxIsScaleAndRotationApplied = new Boolean(true);

			Height = new Float(0F);
			HeightIsFloor = new Boolean(false);
			HeightIsScaleAndRotationApplied = new Boolean(true);
		}

		public enum KillType : int
		{
			[Key(key = "KillType_None")]
			None,
			[Key(key = "KillType_Box")]
			Box,
			[Key(key = "KillType_Height")]
			Height
		}
	}
}