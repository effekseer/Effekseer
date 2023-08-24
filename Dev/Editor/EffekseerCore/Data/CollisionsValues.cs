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
		public Value.Float Bounce { get; set; } = new Value.Float(1.0f);

		[Key(key = "Collisions_Height")]
		public Value.Float Height { get; set; } = new Value.Float(0.0f);

		[Key(key = "Collisions_CoordinateSystem")]
		public Value.Enum<WorldCoordinateSyatemType> WorldCoordinateSyatem { get; set; } = new Value.Enum<WorldCoordinateSyatemType>();
	}
}