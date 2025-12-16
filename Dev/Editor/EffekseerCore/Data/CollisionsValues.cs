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
		private const int SelectorID = 0;

		[Selector(ID = SelectorID)]
		[Key(key = "Collisions_EnableGround")]
		public Value.Boolean IsGroundCollisionEnabled { get; private set; } = new Value.Boolean(false);

		[Key(key = "Collisions_ExternalCollision")]
		public Value.Boolean IsSceneCollisionEnabled { get; private set; } = new Value.Boolean(false);

		[Key(key = "Collisions_Bounce")]
		public Value.FloatWithRandom Bounce { get; private set; } = new Value.FloatWithRandom(1.0f, step: 0.1f);

		[Selected(ID = SelectorID, Value = 1)]
		[Key(key = "Collisions_Height")]
		public Value.Float Height { get; private set; } = new Value.Float(0.0f);

		[Key(key = "Collisions_Friction")]
		public Value.FloatWithRandom Friction { get; private set; } = new Value.FloatWithRandom(0.0f, max: 1.0f, min: 0.0f, step: 0.01f);

		[Selected(ID = SelectorID, Value = 1)]
		[Key(key = "Collisions_CoordinateSystem")]
		public Value.Enum<WorldCoordinateSyatemType> WorldCoordinateSyatem { get; private set; } = new Value.Enum<WorldCoordinateSyatemType>();

		[Key(key = "Collisions_LifetimeReduction")]
		public Value.FloatWithRandom LifetimeReductionPerCollision { get; private set; } = new Value.FloatWithRandom(0.0f, min: 0.0f, step: 0.1f);
	}
}
