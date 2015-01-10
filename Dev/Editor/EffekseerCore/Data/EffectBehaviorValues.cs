using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class EffectBehaviorValues
	{
		[Name(language = Language.Japanese, value = "初期位置")]
		[Description(language = Language.Japanese, value = "中心の初期位置")]
		[Undo(Undo = false)]
		public Value.Vector3D Location
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "移動速度")]
		[Description(language = Language.Japanese, value = "中心の移動速度")]
		[Undo(Undo = false)]
		public Value.Vector3D LocationVelocity
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "初期角度")]
		[Description(language = Language.Japanese, value = "中心の初期角度")]
		[Undo(Undo = false)]
		public Value.Vector3D Rotation
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "回転速度")]
		[Description(language = Language.Japanese, value = "中心の回転速度")]
		[Undo(Undo = false)]
		public Value.Vector3D RotationVelocity
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "初期拡大率")]
		[Description(language = Language.Japanese, value = "中心の初期拡大率")]
		[Undo(Undo = false)]
		public Value.Vector3D Scale
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "拡大速度")]
		[Description(language = Language.Japanese, value = "中心の拡大速度")]
		[Undo(Undo = false)]
		public Value.Vector3D ScaleVelocity
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "破棄フレーム")]
		[Description(language = Language.Japanese, value = "中心が破棄されるフレーム")]
		[Undo(Undo = false)]
		public Value.IntWithInifinite RemovedTime
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "個数(X方向)")]
		[Description(language = Language.Japanese, value = "エフェクトの個数(X方向)")]
		[Undo(Undo = false)]
		public Value.Int CountX
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "個数(Y方向)")]
		[Description(language = Language.Japanese, value = "エフェクトの個数(Y方向)")]
		[Undo(Undo = false)]
		public Value.Int CountY
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "個数(Z方向)")]
		[Description(language = Language.Japanese, value = "エフェクトの個数(Z方向)")]
		[Undo(Undo = false)]
		public Value.Int CountZ
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "距離")]
		[Description(language = Language.Japanese, value = "エフェクト間の距離")]
		[Undo(Undo = false)]
		public Value.Float Distance
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "ターゲット位置")]
		[Description(language = Language.Japanese, value = "ターゲット位置または引力位置")]
		[Undo(Undo = false)]
		public Value.Vector3D TargetLocation
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
			TargetLocation = new Value.Vector3D();
		}
	}
}
