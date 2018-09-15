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
		[Name(language = Language.English, value = "Pos")]
		[Description(language = Language.English, value = "Based on center")]
		[Undo(Undo = false)]
		public Value.Vector3D Location
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "移動速度")]
		[Description(language = Language.Japanese, value = "中心の移動速度")]
		[Name(language = Language.English, value = "Speed")]
		[Description(language = Language.English, value = "Starting velocity")]
		[Undo(Undo = false)]
		public Value.Vector3D LocationVelocity
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "初期角度")]
		[Description(language = Language.Japanese, value = "中心の初期角度")]
		[Name(language = Language.English, value = "Angle")]
		[Description(language = Language.English, value = "Rotated about center")]
		[Undo(Undo = false)]
		public Value.Vector3D Rotation
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "回転速度")]
		[Description(language = Language.Japanese, value = "中心の回転速度")]
		[Name(language = Language.English, value = "Angular\nSpeed")]
		[Description(language = Language.English, value = "Rotated about center")]
		[Undo(Undo = false)]
		public Value.Vector3D RotationVelocity
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "初期拡大率")]
		[Description(language = Language.Japanese, value = "中心の初期拡大率")]
		[Name(language = Language.English, value = "Scale")]
		[Description(language = Language.English, value = "Scaled about center")]
		[Undo(Undo = false)]
		public Value.Vector3D Scale
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "拡大速度")]
		[Description(language = Language.Japanese, value = "中心の拡大速度")]
		[Name(language = Language.English, value = "Expansion\nRate")]
		[Description(language = Language.English, value = "Scaled about center")]
		[Undo(Undo = false)]
		public Value.Vector3D ScaleVelocity
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "破棄フレーム")]
		[Description(language = Language.Japanese, value = "中心が破棄されるフレーム")]
		[Name(language = Language.English, value = "Life")]
		[Description(language = Language.English, value = "Frame in which instance is destroyed")]
		[Undo(Undo = false)]
		public Value.IntWithInifinite RemovedTime
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "個数(X方向)")]
		[Description(language = Language.Japanese, value = "エフェクトの個数(X方向)")]
		[Name(language = Language.English, value = "X Count")]
		[Description(language = Language.English, value = "Number of instances spawned about the x-axis")]
		[Undo(Undo = false)]
		public Value.Int CountX
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "個数(Y方向)")]
		[Description(language = Language.Japanese, value = "エフェクトの個数(Y方向)")]
		[Name(language = Language.English, value = "Y Count")]
		[Description(language = Language.English, value = "Number of instances spawned about the y-axis")]
		[Undo(Undo = false)]
		public Value.Int CountY
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "個数(Z方向)")]
		[Description(language = Language.Japanese, value = "エフェクトの個数(Z方向)")]
		[Name(language = Language.English, value = "Z Count")]
		[Description(language = Language.English, value = "Number of instances spawned about the z-axis")]
		[Undo(Undo = false)]
		public Value.Int CountZ
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "距離")]
		[Description(language = Language.Japanese, value = "エフェクト間の距離")]
		[Name(language = Language.English, value = "Separation")]
		[Description(language = Language.English, value = "Distance between the spawned instances")]
		[Undo(Undo = false)]
		public Value.Float Distance
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "生成間隔(フレーム)")]
		[Description(language = Language.Japanese, value = "1以上の場合、指定された時間ごとにエフェクトを生成する。")]
		[Name(language = Language.English, value = "Generating time span(Frame)")]
		[Description(language = Language.English, value = "If the value is larger than 1, effects are generated every specified time")]
		[Undo(Undo = false)]
		public Value.Int TimeSpan
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "全体色")]
		[Name(language = Language.English, value = "Color All")]
		[Undo(Undo = false)]
		public Value.Color ColorAll
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "ターゲット位置")]
		[Description(language = Language.Japanese, value = "ターゲット位置または引力位置")]
		[Name(language = Language.English, value = "Point of\nAttraction")]
		[Description(language = Language.English, value = "Point in space the instances are drawn to")]
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

			TimeSpan = new Value.Int(0);
			ColorAll = new Value.Color(255, 255, 255, 255);

			TargetLocation = new Value.Vector3D();
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

			TargetLocation.X.SetValueDirectly(0.0f);
			TargetLocation.Y.SetValueDirectly(0.0f);
			TargetLocation.Z.SetValueDirectly(0.0f);
		}
	}
}
