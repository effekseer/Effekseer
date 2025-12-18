using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class EffectBehaviorValues
	{
		[Key(key = "EffectBehavior_Location")]
		[Undo(Undo = false)]
		public Value.Vector3D Location
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_LocationVelocity")]
		[Undo(Undo = false)]
		public Value.Vector3D LocationVelocity
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_Rotation")]
		[Undo(Undo = false)]
		public Value.Vector3D Rotation
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_RotationVelocity")]
		[Undo(Undo = false)]
		public Value.Vector3D RotationVelocity
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_Scale")]
		[Undo(Undo = false)]
		public Value.Vector3D Scale
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_ScaleVelocity")]
		[Undo(Undo = false)]
		public Value.Vector3D ScaleVelocity
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_RemovedTime")]
		[Undo(Undo = false)]
		public Value.IntWithInifinite RemovedTime
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_CountX")]
		[Undo(Undo = false)]
		public Value.Int CountX
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_CountY")]
		[Undo(Undo = false)]
		public Value.Int CountY
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_CountZ")]
		[Undo(Undo = false)]
		public Value.Int CountZ
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_Distance")]
		[Undo(Undo = false)]
		public Value.Float Distance
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_TimeSpan")]
		[Undo(Undo = false)]
		public Value.Int TimeSpan
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_ColorAll")]
		[Undo(Undo = false)]
		public Value.Color ColorAll
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_PlaybackSpeed")]
		[Undo(Undo = false)]
		public Value.Float PlaybackSpeed
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_TargetLocation")]
		[Undo(Undo = false)]
		public Value.Vector3D TargetLocation
		{
			get;
			private set;
		}

		[Key(key = "EffectBehavior_ExternalModels")]
		[TreeNode(key = "EffectBehavior_ExternalModels", id = "EffectBehavior_ExternalModels")]
		[IO(Export = true)]
		[Undo(Undo = false)]
		public ExternalModelValues ExternalModels
		{
			get;
			private set;
		}

		public EffectBehaviorValues()
		{
			Location = new Value.Vector3D();
			Rotation = new Value.Vector3D();
			Scale = new Value.Vector3D(1.0f, 1.0f, 1.0f);

			LocationVelocity = new Value.Vector3D();
			RotationVelocity = new Value.Vector3D();
			ScaleVelocity = new Value.Vector3D();

			RemovedTime = new Value.IntWithInifinite(0, true, int.MaxValue, 0);

			CountX = new Value.Int(1, int.MaxValue, 1);
			CountY = new Value.Int(1, int.MaxValue, 1);
			CountZ = new Value.Int(1, int.MaxValue, 1);

			Distance = new Value.Float(5.0f, float.MaxValue, 0.0f);

			TimeSpan = new Value.Int(0);
			ColorAll = new Value.Color(255, 255, 255, 255);

			TargetLocation = new Value.Vector3D();

			PlaybackSpeed = new Value.Float(1.0f, float.MaxValue, 0.0f, 0.1f);

			ExternalModels = new ExternalModelValues();
		}

		/// <summary>
		/// Adhoc code
		/// </summary>
		public void Reset()
		{
			Location.X.SetValueDirectly(0.0f);
			Location.Y.SetValueDirectly(0.0f);
			Location.Z.SetValueDirectly(0.0f);

			Rotation.X.SetValueDirectly(0.0f);
			Rotation.Y.SetValueDirectly(0.0f);
			Rotation.Z.SetValueDirectly(0.0f);

			Scale.X.SetValueDirectly(1.0f);
			Scale.Y.SetValueDirectly(1.0f);
			Scale.Z.SetValueDirectly(1.0f);

			LocationVelocity.X.SetValueDirectly(0.0f);
			LocationVelocity.Y.SetValueDirectly(0.0f);
			LocationVelocity.Z.SetValueDirectly(0.0f);

			RotationVelocity.X.SetValueDirectly(0.0f);
			RotationVelocity.Y.SetValueDirectly(0.0f);
			RotationVelocity.Z.SetValueDirectly(0.0f);

			ScaleVelocity.X.SetValueDirectly(0.0f);
			ScaleVelocity.Y.SetValueDirectly(0.0f);
			ScaleVelocity.Z.SetValueDirectly(0.0f);

			RemovedTime.Infinite.SetValueDirectly(true);
			RemovedTime.Value.SetValueDirectly(0);

			CountX.SetValueDirectly(1);
			CountY.SetValueDirectly(1);
			CountZ.SetValueDirectly(1);

			Distance.SetValueDirectly(5);

			TimeSpan.SetValueDirectly(0);

			ColorAll.R.SetValueDirectly(255);
			ColorAll.G.SetValueDirectly(255);
			ColorAll.B.SetValueDirectly(255);
			ColorAll.A.SetValueDirectly(255);

			PlaybackSpeed.SetValueDirectly(1);

			TargetLocation.X.SetValueDirectly(0.0f);
			TargetLocation.Y.SetValueDirectly(0.0f);
			TargetLocation.Z.SetValueDirectly(0.0f);

			ExternalModels.Reset();
		}
	}
}
