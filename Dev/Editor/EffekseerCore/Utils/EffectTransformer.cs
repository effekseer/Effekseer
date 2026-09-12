using System;
using System.Collections.Generic;
using System.Linq;
using Effekseer.Data;
using Effekseer.Data.Value;

namespace Effekseer.Utils
{
	/// <summary>
	/// 反転する軸
	/// </summary>
	public enum TransformAxis : int
	{
		X = 0,
		Y = 1,
		Z = 2,
	}

	/// <summary>
	/// 変換結果。変換できなかったパラメーターを呼び出し側に伝える。
	/// </summary>
	public class EffectTransformResult
	{
		public int NodeCount = 0;

		/// <summary>
		/// 変換できなかったパラメーターの「分類キー|項目キー」の組。
		/// 表示側で両方を翻訳して "位置: NURBS曲線" のように並べる。
		/// </summary>
		public SortedSet<string> Skipped = new SortedSet<string>(StringComparer.Ordinal);

		/// <summary>
		/// 変換したが厳密ではないパラメーター。書式は Skipped と同じ。
		/// </summary>
		public SortedSet<string> Approximated = new SortedSet<string>(StringComparer.Ordinal);

		internal void Skip(string categoryKey, string parameterKey)
		{
			Skipped.Add(categoryKey + "|" + parameterKey);
		}

		internal void Approximate(string categoryKey, string parameterKey)
		{
			Approximated.Add(categoryKey + "|" + parameterKey);
		}
	}

	/// <summary>
	/// エフェクト全体を一括変換する。
	/// 全ノードを再帰的に処理し、ひとつの Undo 単位にまとめる。
	/// </summary>
	public static class EffectTransformer
	{
		#region primitives

		/// <summary>
		/// 値域を超えない場合のみ符号を反転する。
		/// Float.SetValue は値域で切り捨てるため、事前に確認しなければ値が壊れる。
		/// </summary>
		static bool TryNegate(Float value)
		{
			float current = value.GetValue();
			if (current == 0.0f) return true;

			float target = -current;
			if (target < value.RangeMin || target > value.RangeMax) return false;

			value.SetValue(target);
			return true;
		}

		/// <summary>
		/// 乱数範囲 [min, max] を [-max, -min] にする。
		/// SetMin と SetMax は互いを押し広げるため、この順序でなければ結果が狂う。
		/// </summary>
		static bool TryNegate(FloatWithRandom value)
		{
			float low = value.GetMin();
			float high = value.GetMax();
			if (low == 0.0f && high == 0.0f) return true;

			float targetLow = -high;
			float targetHigh = -low;
			if (targetLow < value.ValueMin || targetHigh > value.ValueMax) return false;

			value.SetMin(targetLow);
			value.SetMax(targetHigh);
			return true;
		}

		static bool TryNegateAxis(Vector3D value, TransformAxis axis)
		{
			switch (axis)
			{
				case TransformAxis.X: return TryNegate(value.X);
				case TransformAxis.Y: return TryNegate(value.Y);
				default: return TryNegate(value.Z);
			}
		}

		static bool TryNegateAxis(Vector3DWithRandom value, TransformAxis axis)
		{
			switch (axis)
			{
				case TransformAxis.X: return TryNegate(value.X);
				case TransformAxis.Y: return TryNegate(value.Y);
				default: return TryNegate(value.Z);
			}
		}

		/// <summary>
		/// 回転を鏡映する。
		/// 軸 A に垂直な平面で鏡映した場合、A まわりの回転はそのまま残り、
		/// 他の 2 軸まわりの回転は符号が反転する。
		/// 軸角表現の軸ベクトルは擬ベクトルなので、同じ規則に従う。
		/// </summary>
		static void MirrorRotation(Vector3D value, TransformAxis axis)
		{
			if (axis != TransformAxis.X) TryNegate(value.X);
			if (axis != TransformAxis.Y) TryNegate(value.Y);
			if (axis != TransformAxis.Z) TryNegate(value.Z);
		}

		static void MirrorRotation(Vector3DWithRandom value, TransformAxis axis)
		{
			if (axis != TransformAxis.X) TryNegate(value.X);
			if (axis != TransformAxis.Y) TryNegate(value.Y);
			if (axis != TransformAxis.Z) TryNegate(value.Z);
		}

		static FCurveKey<float>[] ReadKeys(FCurve<float> curve)
		{
			return curve.Keys.Cast<FCurveKey<float>>().ToArray();
		}

		/// <summary>
		/// F カーブの値の符号を反転する。制御点は絶対座標なので y だけを反転する。
		/// </summary>
		static void NegateCurveValues(FCurve<float> curve)
		{
			var keys = ReadKeys(curve);
			if (keys.Length > 0)
			{
				var replaced = new FCurveKey<float>[keys.Length];
				for (int i = 0; i < keys.Length; i++)
				{
					var source = keys[i];
					var key = new FCurveKey<float>(source.Frame, -source.ValueAsFloat);
					key.SetLeftDirectly(source.LeftX, -source.LeftY);
					key.SetRightDirectly(source.RightX, -source.RightY);
					key.SetInterpolationType(source.InterpolationType);
					replaced[i] = key;
				}

				curve.SetKeys(replaced);
			}

			float offsetMin = curve.OffsetMin.GetValue();
			float offsetMax = curve.OffsetMax.GetValue();
			if (offsetMin != 0.0f || offsetMax != 0.0f)
			{
				curve.OffsetMin.SetValue(-offsetMax);
				curve.OffsetMax.SetValue(-offsetMin);
			}
		}

		/// <summary>
		/// F カーブを時間方向に反転する。制御点の左右も入れ替わる。
		/// 色カーブは FCurve&lt;int&gt; なので型引数で受ける。
		/// </summary>
		static void ReverseCurveTime<T>(FCurve<T> curve) where T : struct, IComparable<T>, IEquatable<T>
		{
			var keys = curve.Keys.Cast<FCurveKey<T>>().ToArray();
			if (keys.Length > 0)
			{
				int span = keys.Min(k => k.Frame) + keys.Max(k => k.Frame);

				var replaced = keys
					.Select(source =>
					{
						var key = new FCurveKey<T>(span - source.Frame, source.Value);
						key.SetLeftDirectly(span - source.RightX, source.RightY);
						key.SetRightDirectly(span - source.LeftX, source.LeftY);
						key.SetInterpolationType(source.InterpolationType);
						return key;
					})
					.OrderBy(k => k.Frame)
					.ToArray();

				curve.SetKeys(replaced);
			}

			var startType = curve.StartType.Value;
			var endType = curve.EndType.Value;
			if (!startType.Equals(endType))
			{
				curve.StartType.SetValue(endType);
				curve.EndType.SetValue(startType);
			}
		}

		/// <summary>
		/// イージングの加減速を入れ替える。
		/// EasingStart と EasingEnd は同じ数値を使うので、整数のまま移し替えられる。
		/// </summary>
		static void SwapEasingSpeeds(Enum<EasingStart> start, Enum<EasingEnd> end)
		{
			int startValue = start.GetValueAsInt();
			int endValue = end.GetValueAsInt();
			if (startValue == endValue) return;

			start.SetValue(endValue);
			end.SetValue(startValue);
		}

		static void Swap(ColorWithRandom first, ColorWithRandom second)
		{
			Swap(first.R, second.R);
			Swap(first.G, second.G);
			Swap(first.B, second.B);
			Swap(first.A, second.A);
		}

		static void Swap(IntWithRandom first, IntWithRandom second)
		{
			int firstMin = first.GetMin();
			int firstMax = first.GetMax();
			int secondMin = second.GetMin();
			int secondMax = second.GetMax();

			if (firstMin == secondMin && firstMax == secondMax) return;

			first.SetMin(secondMin);
			first.SetMax(secondMax);
			second.SetMin(firstMin);
			second.SetMax(firstMax);
		}

		/// <summary>
		/// グラデーションの位置を左右反転する。位置は 0 から 1 の割合で持つ。
		/// </summary>
		static void ReverseGradient(Gradient gradient)
		{
			var state = gradient.GetValue();
			if (state == null) return;

			var reversed = (Gradient.State)state.Clone();

			if (state.ColorMarkers != null)
			{
				reversed.ColorMarkers = state.ColorMarkers
					.Select(marker => { marker.Position = 1.0f - marker.Position; return marker; })
					.OrderBy(marker => marker.Position)
					.ToArray();
			}

			if (state.AlphaMarkers != null)
			{
				reversed.AlphaMarkers = state.AlphaMarkers
					.Select(marker => { marker.Position = 1.0f - marker.Position; return marker; })
					.OrderBy(marker => marker.Position)
					.ToArray();
			}

			gradient.SetValue(reversed);
		}

		static void Swap(FloatWithRandom first, FloatWithRandom second)
		{
			float firstMin = first.GetMin();
			float firstMax = first.GetMax();
			float secondMin = second.GetMin();
			float secondMax = second.GetMax();

			if (firstMin == secondMin && firstMax == secondMax) return;

			first.SetMin(secondMin);
			first.SetMax(secondMax);
			second.SetMin(firstMin);
			second.SetMax(firstMax);
		}

		static void Swap(Vector3DWithRandom first, Vector3DWithRandom second)
		{
			Swap(first.X, second.X);
			Swap(first.Y, second.Y);
			Swap(first.Z, second.Z);
		}

		#endregion

		#region traversal

		static IEnumerable<Node> EnumerateNodes(NodeBase parent)
		{
			for (int i = 0; i < parent.Children.Count; i++)
			{
				var child = parent.Children[i];
				yield return child;

				foreach (var descendant in EnumerateNodes(child))
				{
					yield return descendant;
				}
			}
		}

		#endregion

		#region flip

		/// <summary>
		/// エフェクト全体を指定した軸で反転する。
		/// </summary>
		public static EffectTransformResult Flip(NodeRoot root, TransformAxis axis)
		{
			var result = new EffectTransformResult();
			if (root == null) return result;

			Command.CommandManager.StartCollection();
			try
			{
				foreach (var node in EnumerateNodes(root))
				{
					FlipLocation(node, axis, result);
					FlipForceFields(node, axis, result);
					FlipRotation(node, axis);
					FlipGenerationLocation(node, axis, result);
					result.NodeCount++;
				}
			}
			finally
			{
				Command.CommandManager.EndCollection();
			}

			return result;
		}

		static FCurve<float> SelectCurve(FCurveVector3D curve, TransformAxis axis)
		{
			switch (axis)
			{
				case TransformAxis.X: return curve.X;
				case TransformAxis.Y: return curve.Y;
				default: return curve.Z;
			}
		}

		/// <summary>
		/// 指定軸「以外」のカーブを返す。回転の鏡映規則に対応する。
		/// </summary>
		static IEnumerable<FCurve<float>> CurvesExcept(FCurveVector3D curve, TransformAxis axis)
		{
			if (axis != TransformAxis.X) yield return curve.X;
			if (axis != TransformAxis.Y) yield return curve.Y;
			if (axis != TransformAxis.Z) yield return curve.Z;
		}

		static void FlipLocation(Node node, TransformAxis axis, EffectTransformResult result)
		{
			var location = node.LocationValues;

			switch (location.Type.Value)
			{
				case LocationValues.ParamaterType.Fixed:
					TryNegateAxis(location.Fixed.Location, axis);
					break;

				case LocationValues.ParamaterType.PVA:
					TryNegateAxis(location.PVA.Location, axis);
					TryNegateAxis(location.PVA.Velocity, axis);
					TryNegateAxis(location.PVA.Acceleration, axis);
					break;

				case LocationValues.ParamaterType.Easing:
					TryNegateAxis(location.Easing.Start, axis);
					TryNegateAxis(location.Easing.End, axis);
					break;

				case LocationValues.ParamaterType.LocationFCurve:
					NegateCurveValues(SelectCurve(location.LocationFCurve.FCurve, axis));
					break;

				case LocationValues.ParamaterType.NurbsCurve:
					result.Skip("Position", "Position_ParameterType_NurbsCurve_Name");
					break;

				case LocationValues.ParamaterType.ViewOffset:
					result.Skip("Position", "Position_ParameterType_ViewOffset_Name");
					break;
			}
		}

		static void FlipForceFields(Node node, TransformAxis axis, EffectTransformResult result)
		{
			var fields = new[]
			{
				node.LocationAbsValues.LocalForceField1,
				node.LocationAbsValues.LocalForceField2,
				node.LocationAbsValues.LocalForceField3,
				node.LocationAbsValues.LocalForceField4,
			};

			foreach (var field in fields)
			{
				switch (field.Type.Value)
				{
					case LocalForceFieldType.None:
						break;

					case LocalForceFieldType.Gravity:
						// 重力は向きをベクトルとして持つ。
						TryNegateAxis(field.Gravity.Gravity, axis);
						break;

					case LocalForceFieldType.AttractiveForce:
						// 引力は位置も向きも持たないため、反転しても変化しない。
						break;

					case LocalForceFieldType.Force:
					case LocalForceFieldType.Drag:
						// 場の中心からの等方的な力なので、中心を移せばよい。
						TryNegateAxis(field.Position, axis);
						MirrorRotation(field.Rotation, axis);
						break;

					case LocalForceFieldType.Wind:
						// 風は場のローカル +Y に吹く。ベクトルなので、
						// Y 軸で鏡映したときだけ向きが逆になる。
						TryNegateAxis(field.Position, axis);
						MirrorRotation(field.Rotation, axis);
						if (axis == TransformAxis.Y) TryNegate(field.Power);
						break;

					case LocalForceFieldType.Vortex:
						// 渦の軸は場のローカル +Y。擬ベクトルなので、
						// Y 軸「以外」で鏡映したときに回転方向が逆になる。
						TryNegateAxis(field.Position, axis);
						MirrorRotation(field.Rotation, axis);
						if (axis != TransformAxis.Y) TryNegate(field.Power);
						break;

					case LocalForceFieldType.Turbulence:
						// ノイズ場は乱数の種から生成されるため鏡映できない。
						TryNegateAxis(field.Position, axis);
						result.Skip("AttractionForces", "LFF_Type_Turbulence_Name");
						break;
				}
			}
		}

		static void FlipRotation(Node node, TransformAxis axis)
		{
			var rotation = node.RotationValues;

			switch (rotation.Type.Value)
			{
				case RotationValues.ParamaterType.Fixed:
					MirrorRotation(rotation.Fixed.Rotation, axis);
					break;

				case RotationValues.ParamaterType.PVA:
					MirrorRotation(rotation.PVA.Rotation, axis);
					MirrorRotation(rotation.PVA.Velocity, axis);
					MirrorRotation(rotation.PVA.Acceleration, axis);
					break;

				case RotationValues.ParamaterType.Easing:
					MirrorRotation(rotation.Easing.Start, axis);
					MirrorRotation(rotation.Easing.End, axis);
					break;

				case RotationValues.ParamaterType.AxisPVA:
					// 軸ベクトルだけを鏡映する。角度そのものは変わらない。
					MirrorRotation(rotation.AxisPVA.Axis, axis);
					break;

				case RotationValues.ParamaterType.AxisEasing:
					MirrorRotation(rotation.AxisEasing.Axis, axis);
					break;

				case RotationValues.ParamaterType.RotationFCurve:
					foreach (var curve in CurvesExcept(rotation.RotationFCurve.FCurve, axis))
					{
						NegateCurveValues(curve);
					}
					break;
			}
		}

		static void FlipGenerationLocation(Node node, TransformAxis axis, EffectTransformResult result)
		{
			var generation = node.GenerationLocationValues;

			switch (generation.Type.Value)
			{
				case GenerationLocationValues.ParameterType.Point:
					TryNegateAxis(generation.Point.Location, axis);
					break;

				case GenerationLocationValues.ParameterType.Line:
					TryNegateAxis(generation.Line.PositionStart, axis);
					TryNegateAxis(generation.Line.PositionEnd, axis);
					break;

				case GenerationLocationValues.ParameterType.Sphere:
					// 球は X, Y まわりの回転を持つ。回転の鏡映規則に従う。
					if (axis != TransformAxis.X) TryNegate(generation.Sphere.RotationX);
					if (axis != TransformAxis.Y) TryNegate(generation.Sphere.RotationY);
					break;

				case GenerationLocationValues.ParameterType.Circle:
					FlipCircle(generation.Circle, axis);
					break;

				case GenerationLocationValues.ParameterType.Model:
					result.Skip("SpawningMethod", "SpawnMethod_ParameterType_Model_Name");
					break;
			}
		}

		/// <summary>
		/// 円の軸が反転軸と一致する場合、円は鏡映面に平行なので角度は変わらない。
		/// 一致しない場合は掃引方向が逆転する。
		/// </summary>
		static void FlipCircle(GenerationLocationValues.CircleParameter circle, TransformAxis axis)
		{
			if (CircleAxisOf(circle.AxisDirection.Value) == axis) return;

			TryNegate(circle.AngleStart);
			TryNegate(circle.AngleEnd);
			Swap(circle.AngleStart, circle.AngleEnd);
		}

		static TransformAxis CircleAxisOf(AxisType axisType)
		{
			switch (axisType)
			{
				case AxisType.XAxis: return TransformAxis.X;
				case AxisType.YAxis: return TransformAxis.Y;
				default: return TransformAxis.Z;
			}
		}

		#endregion

		#region reverse timeline

		/// <summary>
		/// エフェクト全体の時間方向を反転する。
		/// F カーブ、イージング、色や大きさの推移など、時間に沿って定義された
		/// パラメーターだけが対象になる。発生数や寿命は時間反転できない。
		/// </summary>
		public static EffectTransformResult ReverseTimeline(NodeRoot root)
		{
			var result = new EffectTransformResult();
			if (root == null) return result;

			Command.CommandManager.StartCollection();
			try
			{
				foreach (var node in EnumerateNodes(root))
				{
					ReverseLocation(node, result);
					ReverseRotation(node, result);
					ReverseScale(node, result);
					ReverseColors(node);
					ReverseFade(node, result);
					ReportEmission(node, result);
					result.NodeCount++;
				}
			}
			finally
			{
				Command.CommandManager.EndCollection();
			}

			return result;
		}

		/// <summary>
		/// 等加速度運動を時間反転する。
		/// p(t) = p0 + v0*t + a*t^2/2 に対して q(t) = p(T - t) を展開すると
		///   q0 = p0 + v0*T + a*T^2/2,  w0 = -(v0 + a*T),  b = a
		/// になる。中心値だけを移し、乱数の振れ幅はそのまま残す。
		/// こうすると 2 回適用したとき元に戻る。
		/// </summary>
		static void ReversePVA(FloatWithRandom position, FloatWithRandom velocity, FloatWithRandom acceleration, float life)
		{
			// v*T と a*T^2/2 は打ち消し合うことが多く、float のままでは桁落ちする。
			// 倍精度で計算してから戻すと誤差が一桁小さくなる。
			double p = position.GetCenter();
			double v = velocity.GetCenter();
			double a = acceleration.GetCenter();
			double t = life;

			double travel = v * t;
			double drift = a * t * t * 0.5;
			double gained = a * t;

			TrySetCenter(position, (float)SnapToZero(p + travel + drift, p, travel, drift));
			TrySetCenter(velocity, (float)SnapToZero(-(v + gained), v, gained));

			// 加速度は向きも大きさも変わらない。
		}

		/// <summary>
		/// 打ち消し合う項の和は、入力の精度から見ればゼロのことがある。
		/// そのまま残すと 1e-9 のような値がファイルに書き込まれてしまう。
		/// </summary>
		static double SnapToZero(double sum, params double[] terms)
		{
			double scale = 0.0;
			foreach (var term in terms)
			{
				scale = Math.Max(scale, Math.Abs(term));
			}

			return Math.Abs(sum) <= scale * 1e-6 ? 0.0 : sum;
		}

		static void ReversePVA(Vector3DWithRandom position, Vector3DWithRandom velocity, Vector3DWithRandom acceleration, float life)
		{
			ReversePVA(position.X, velocity.X, acceleration.X, life);
			ReversePVA(position.Y, velocity.Y, acceleration.Y, life);
			ReversePVA(position.Z, velocity.Z, acceleration.Z, life);
		}

		/// <summary>
		/// 振れ幅を保ったまま中心を動かす。値域を超える場合は何もしない。
		/// </summary>
		/// <summary>
		/// 振れ幅を保ったまま中心を動かす。値域を超える場合は何もしない。
		/// SetCenter に任せると端が中心と振れ幅の足し算で決まり、
		/// 本来ゼロの端に 1e-9 程度の残りが出るため、端を直接指定する。
		/// </summary>
		static bool TrySetCenter(FloatWithRandom value, float center)
		{
			float amplitude = value.GetAmplitude();

			float low = (float)SnapToZero(center - amplitude, center, amplitude);
			float high = (float)SnapToZero(center + amplitude, center, amplitude);

			if (low < value.ValueMin || high > value.ValueMax) return false;

			value.SetMin(low);
			value.SetMax(high);
			return true;
		}

		/// <summary>
		/// 反転が厳密になるのは、寿命も各値も乱数を持たない場合だけ。
		/// 乱数があると粒子ごとの対応まではたどれない。
		/// </summary>
		static bool IsExact(IntWithRandom life, params FloatWithRandom[] values)
		{
			if (life.GetMin() != life.GetMax()) return false;

			return values.All(v => v.GetAmplitude() == 0.0f);
		}

		static FloatWithRandom[] Components(params Vector3DWithRandom[] vectors)
		{
			return vectors.SelectMany(v => new[] { v.X, v.Y, v.Z }).ToArray();
		}

		/// <summary>
		/// 寿命の中心値をフレーム数として使う。
		/// </summary>
		static float LifeOf(Node node)
		{
			return node.CommonValues.Life.GetCenter();
		}

		/// <summary>
		/// 寿命に沿って変化する色を反転する。
		/// 位置だけを反転しても、明滅や退色が順方向のままだと逆再生には見えない。
		/// </summary>
		static void ReverseColors(Node node)
		{
			var drawing = node.DrawingValues;

			// スプライト、モデル、軌跡が使う新しい色パラメーター。
			ReverseStandardColor(drawing.ColorAll);
			ReverseStandardColor(drawing.TrailColorLeft);
			ReverseStandardColor(drawing.TrailColorLeftMiddle);
			ReverseStandardColor(drawing.TrailColorCenter);
			ReverseStandardColor(drawing.TrailColorCenterMiddle);
			ReverseStandardColor(drawing.TrailColorRight);
			ReverseStandardColor(drawing.TrailColorRightMiddle);

			// リボンとリングは従来の色パラメーターのままなので個別に扱う。
			ReverseColorEasing(drawing.Ribbon.ColorAll_Easing);
			ReverseColorEasing(drawing.Ring.OuterColor_Easing);
			ReverseColorEasing(drawing.Ring.CenterColor_Easing);
			ReverseColorEasing(drawing.Ring.InnerColor_Easing);
		}

		/// <summary>
		/// 粒子が時間をかけて発生するノードは、パラメーターをどう書き換えても
		/// 逆再生にはならない。発生した順番は保存されていないため、
		/// 後から出た粒子を先に出すことができない。
		/// 1 個しか出さないノードには当てはまらないので、その場合は黙っている。
		/// </summary>
		static void ReportEmission(Node node, EffectTransformResult result)
		{
			var common = node.CommonValues;

			bool emitsOverTime =
				common.MaxGeneration.Infinite.Value ||
				common.MaxGeneration.Value.GetValue() > 1;

			if (emitsOverTime)
			{
				result.Skip("BasicSettings", "BasicSettings_GenerationTime_Name");
			}
		}

		static void ReverseStandardColor(StandardColor color)
		{
			switch (color.Type.Value)
			{
				case StandardColorType.Easing:
					ReverseColorEasing(color.Easing);
					break;

				case StandardColorType.FCurve:
					ReverseCurveTime(color.FCurve.FCurve.R);
					ReverseCurveTime(color.FCurve.FCurve.G);
					ReverseCurveTime(color.FCurve.FCurve.B);
					ReverseCurveTime(color.FCurve.FCurve.A);
					break;

				case StandardColorType.Gradient:
					ReverseGradient(color.Gradient);
					break;

				default:
					// 固定色と乱数色は時間で変わらない。
					break;
			}
		}

		static void ReverseColorEasing(ColorEasingParamater easing)
		{
			Swap(easing.Start, easing.End);
			SwapEasingSpeeds(easing.StartSpeed, easing.EndSpeed);
		}

		/// <summary>
		/// フェードインとフェードアウトを入れ替える。
		/// 「消滅後にフェード」は対応するフェードインがないため、そのまま残す。
		/// </summary>
		static void ReverseFade(Node node, EffectTransformResult result)
		{
			var render = node.RendererCommonValues;

			var fadeInType = render.FadeInType.Value;
			var fadeOutType = render.FadeOutType.Value;

			if (fadeOutType == RendererCommonValues.FadeOutMethod.AfterRemoved)
			{
				result.Skip("BasicRenderSettings", "BRS_FadeOutType_Name");
				return;
			}

			if (fadeInType == RendererCommonValues.FadeInMethod.None &&
				fadeOutType == RendererCommonValues.FadeOutMethod.None)
			{
				return;
			}

			render.FadeInType.SetValue(fadeOutType == RendererCommonValues.FadeOutMethod.None
				? RendererCommonValues.FadeInMethod.None
				: RendererCommonValues.FadeInMethod.Use);

			render.FadeOutType.SetValue(fadeInType == RendererCommonValues.FadeInMethod.None
				? RendererCommonValues.FadeOutMethod.None
				: RendererCommonValues.FadeOutMethod.WithinLifetime);

			float inFrame = render.FadeIn.Frame.GetValue();
			float outFrame = render.FadeOut.Frame.GetValue();
			render.FadeIn.Frame.SetValue(outFrame);
			render.FadeOut.Frame.SetValue(inFrame);

			int inStart = render.FadeIn.StartSpeed.GetValueAsInt();
			int inEnd = render.FadeIn.EndSpeed.GetValueAsInt();
			render.FadeIn.StartSpeed.SetValue(render.FadeOut.StartSpeed.GetValueAsInt());
			render.FadeIn.EndSpeed.SetValue(render.FadeOut.EndSpeed.GetValueAsInt());
			render.FadeOut.StartSpeed.SetValue(inStart);
			render.FadeOut.EndSpeed.SetValue(inEnd);
		}

		static void ReverseVector3DCurve(FCurveVector3D curve)
		{
			ReverseCurveTime(curve.X);
			ReverseCurveTime(curve.Y);
			ReverseCurveTime(curve.Z);
		}

		static void ReverseLocation(Node node, EffectTransformResult result)
		{
			var location = node.LocationValues;

			switch (location.Type.Value)
			{
				case LocationValues.ParamaterType.Fixed:
					// 時間変化しないので反転しても同じ。
					break;

				case LocationValues.ParamaterType.Easing:
					Swap(location.Easing.Start, location.Easing.End);
					SwapEasingSpeeds(location.Easing.StartSpeed, location.Easing.EndSpeed);
					break;

				case LocationValues.ParamaterType.LocationFCurve:
					ReverseVector3DCurve(location.LocationFCurve.FCurve);
					break;

				case LocationValues.ParamaterType.PVA:
				{
					var pva = location.PVA;
					ReversePVA(pva.Location, pva.Velocity, pva.Acceleration, LifeOf(node));

					if (!IsExact(node.CommonValues.Life, Components(pva.Location, pva.Velocity, pva.Acceleration)))
					{
						result.Approximate("Position", "Position_ParamaterType_PVA_Name");
					}
					break;
				}

				case LocationValues.ParamaterType.NurbsCurve:
					result.Skip("Position", "Position_ParameterType_NurbsCurve_Name");
					break;

				case LocationValues.ParamaterType.ViewOffset:
					break;
			}
		}

		static void ReverseRotation(Node node, EffectTransformResult result)
		{
			var rotation = node.RotationValues;

			switch (rotation.Type.Value)
			{
				case RotationValues.ParamaterType.Fixed:
					break;

				case RotationValues.ParamaterType.Easing:
					Swap(rotation.Easing.Start, rotation.Easing.End);
					SwapEasingSpeeds(rotation.Easing.StartSpeed, rotation.Easing.EndSpeed);
					break;

				case RotationValues.ParamaterType.AxisEasing:
					Swap(rotation.AxisEasing.Easing.Start, rotation.AxisEasing.Easing.End);
					SwapEasingSpeeds(rotation.AxisEasing.Easing.StartSpeed, rotation.AxisEasing.Easing.EndSpeed);
					break;

				case RotationValues.ParamaterType.RotationFCurve:
					ReverseVector3DCurve(rotation.RotationFCurve.FCurve);
					break;

				case RotationValues.ParamaterType.PVA:
				{
					var pva = rotation.PVA;
					ReversePVA(pva.Rotation, pva.Velocity, pva.Acceleration, LifeOf(node));

					if (!IsExact(node.CommonValues.Life, Components(pva.Rotation, pva.Velocity, pva.Acceleration)))
					{
						result.Approximate("Rotation", "Rotation_ParamaterType_PVA_Name");
					}
					break;
				}

				case RotationValues.ParamaterType.AxisPVA:
				{
					// 軸はそのまま、軸まわりの角度だけを時間反転する。
					var pva = rotation.AxisPVA;
					ReversePVA(pva.Rotation, pva.Velocity, pva.Acceleration, LifeOf(node));

					if (!IsExact(node.CommonValues.Life, pva.Rotation, pva.Velocity, pva.Acceleration))
					{
						result.Approximate("Rotation", "Rotation_ParamaterType_AxisPVA_Name");
					}
					break;
				}
			}
		}

		static void ReverseScale(Node node, EffectTransformResult result)
		{
			var scale = node.ScalingValues;

			switch (scale.Type.Value)
			{
				case ScaleValues.ParamaterType.Fixed:
					break;

				case ScaleValues.ParamaterType.Easing:
					Swap(scale.Easing.Start, scale.Easing.End);
					SwapEasingSpeeds(scale.Easing.StartSpeed, scale.Easing.EndSpeed);
					break;

				case ScaleValues.ParamaterType.SingleEasing:
					Swap(scale.SingleEasing.Start, scale.SingleEasing.End);
					SwapEasingSpeeds(scale.SingleEasing.StartSpeed, scale.SingleEasing.EndSpeed);
					break;

				case ScaleValues.ParamaterType.FCurve:
					ReverseVector3DCurve(scale.FCurve.FCurve);
					break;

				case ScaleValues.ParamaterType.SingleFCurve:
					ReverseCurveTime(scale.SingleFCurve.S);
					break;

				case ScaleValues.ParamaterType.PVA:
				{
					var pva = scale.PVA;
					ReversePVA(pva.Scale, pva.Velocity, pva.Acceleration, LifeOf(node));

					if (!IsExact(node.CommonValues.Life, Components(pva.Scale, pva.Velocity, pva.Acceleration)))
					{
						result.Approximate("Scale", "Scale_ParamaterType_PVA_Name");
					}
					break;
				}

				case ScaleValues.ParamaterType.SinglePVA:
				{
					var pva = scale.SinglePVA;
					ReversePVA(pva.Scale, pva.Velocity, pva.Acceleration, LifeOf(node));

					if (!IsExact(node.CommonValues.Life, pva.Scale, pva.Velocity, pva.Acceleration))
					{
						result.Approximate("Scale", "Scale_ParamaterType_SinglePVA_Name");
					}
					break;
				}
			}
		}

		#endregion
	}
}
