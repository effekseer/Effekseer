using Effekseer.Data.Value;
using IronPython.Modules;

namespace Effekseer.Data
{
	public class EffectLODValues
	{
		// Not configurable value
		public Value.Boolean Lod0Enabled
		{
			get;
			private set;
		}
		
		// Not configurable value
		public Value.Float Distance0
		{
			get;
			private set;
		}

		[Key(key = "EffectLOD_Lod1Enabled")]
		public Value.Boolean Lod1Enabled
		{
			get;
			private set;
		}
		
		[Key(key = "EffectLOD_Distance1")]
		public Value.Float Distance1
		{
			get;
			private set;
		}
		
		[Key(key = "EffectLOD_Lod2Enabled")]
		public Value.Boolean Lod2Enabled
		{
			get;
			private set;
		}
		
		[Key(key = "EffectLOD_Distance2")]
		public Value.Float Distance2
		{
			get;
			private set;
		}
		
		[Key(key = "EffectLOD_Lod3Enabled")]
		public Value.Boolean Lod3Enabled
		{
			get;
			private set;
		}
		
		[Key(key = "EffectLOD_Distance3")]
		public Value.Float Distance3
		{
			get;
			private set;
		}

		public EffectLODValues()
		{
			Lod0Enabled = new Value.Boolean(true);
			Distance0 = new Value.Float(0F);
			
			Lod1Enabled = new Value.Boolean();
			Distance1 = new Value.Float(16F);
			
			Lod2Enabled = new Value.Boolean();
			Distance2 = new Value.Float(32F);
			
			Lod3Enabled = new Value.Boolean();
			Distance3 = new Value.Float(64F);
		}

		public int GetEnabledLevelsBits()
		{
			int result = 0;
			Boolean[] levels = { Lod0Enabled, Lod1Enabled, Lod2Enabled, Lod3Enabled };
			for (int i = 0; i < levels.Length; i++)
			{
				bool isEnabled = levels[i].Value;
				if (isEnabled)
				{
					result |= (1 << i);
				}
				else
				{
					break;
				}
			}

			return result;
		}
	}
}