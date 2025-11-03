using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum WorldCoordinateSyatemType : int
	{
		[Key(key = "WorldCoordinateSyatemType_Local")]
		Local,
		[Key(key = "WorldCoordinateSyatemType_World")]
		World,
	};

	public class CollisionsValues
	{
		[Key(key = "Enabled")]
		public Value.Boolean IsEnabled { get; set; } = new Value.Boolean(false);

		[Key(key = "Collisions_Bounce")]
		public Value.FloatWithRandom Bounce { get; set; } = new Value.FloatWithRandom(1.0f, step: 0.1f);

		[Key(key = "Collisions_Height")]
		public Value.Float Height { get; set; } = new Value.Float(0.0f);

		[Key(key = "Collisions_Friction")]
		public Value.FloatWithRandom Friction { get; set; } = new Value.FloatWithRandom(0.0f, max: 1.0f, min: 0.0f, step: 0.01f);

		[Key(key = "Collisions_CoordinateSystem")]
		public Value.Enum<WorldCoordinateSyatemType> WorldCoordinateSyatem { get; set; } = new Value.Enum<WorldCoordinateSyatemType>();
	}
}
