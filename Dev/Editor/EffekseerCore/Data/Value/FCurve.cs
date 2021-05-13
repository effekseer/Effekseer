using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public enum FCurveTimelineMode : int
	{
		[Key(key = "FcurveTimelineMode_Time")]
		Time = 0,

		[Key(key = "FcurveTimelineMode_Percent")]
		Percent = 1,
	}

	public enum FCurveEdge
	{
		[Key(key = "FcurveEdge_Constant")]
		Constant = 0,
		[Key(key = "FcurveEdge_Loop")]
		Loop = 1,
		[Key(key = "FcurveEdge_LoopInversely")]
		LoopInversely = 2,
	}

	public enum FCurveInterpolation
	{
		[Key(key = "FcurveInterpolation_Bezier")]
		Bezier = 0,
		[Key(key = "FcurveInterpolation_Linear")]
		Linear = 1,
	}

	public interface IFCurve
	{
		Enum<FCurveEdge> StartType { get; }
		Enum<FCurveEdge> EndType { get; }
		void RemoveKey(IFCurveKey key);
		float GetValue(int frame);
		IEnumerable<IFCurveKey> Keys { get; }
		Float OffsetMax { get; }
		Float OffsetMin { get; }
		Int Sampling { get; }

		event ChangedValueEventHandler OnChanged;
	}

	public class FCurve<T> : IFCurve where T : struct, IComparable<T>, IEquatable<T>
	{
        float ToFloat(T v)
        {
            var o = (object)v;

            if (o is float) return (float)o;

            if (v is int)
            {
                var b = (int)o;
                return (float)b;
            }

            throw new NotImplementedException();
        }

        public event ChangedValueEventHandler OnChanged;

		List<FCurveKey<T>> keys = new List<FCurveKey<T>>();

		public IEnumerable<IFCurveKey> Keys
		{
			get
			{
				return (IEnumerable<IFCurveKey>)keys;
			}
		}

		public Enum<FCurveEdge> StartType { get; private set; }

		public Enum<FCurveEdge> EndType { get; private set; }

		public Float OffsetMax { get; private set; }
		public Float OffsetMin { get; private set; }

		public Int Sampling { get; private set; }

		public float DefaultValueRangeMax { get; set; }
		public float DefaultValueRangeMin { get; set; }

		public T DefaultValue { get; set; }

		public FCurve(T defaultValue)
		{
			DefaultValueRangeMax = 100.0f;
			DefaultValueRangeMin = -100.0f;

			this.DefaultValue = defaultValue;
			StartType = new Enum<FCurveEdge>(FCurveEdge.Constant);
			EndType = new Enum<FCurveEdge>(FCurveEdge.Constant);

			OffsetMax = new Float();
			OffsetMin = new Float();

			Sampling = new Int(10, int.MaxValue, 1, 1);
		}

		public void SetKeys(FCurveKey<T>[] keys)
		{
			var new_value = keys.ToList();
			var old_value = this.keys.ToList();

			var cmd = new Command.DelegateCommand(
			() =>
			{
				this.keys = new_value;

				foreach (var key in keys)
				{
					key.OnChangedKey += OnChangedKey;
				}

				if (OnChanged != null)
				{
					OnChanged(this, new ChangedValueEventArgs(new_value, ChangedValueType.Execute));
				}
			},
			() =>
			{
				this.keys = old_value;

				foreach (var key in keys)
				{
					key.OnChangedKey -= OnChangedKey;
				}

				if (OnChanged != null)
				{
					OnChanged(this, new ChangedValueEventArgs(old_value, ChangedValueType.Unexecute));
				}
			});

			Command.CommandManager.Execute(cmd);
		}

		public void AddKey(FCurveKey<T> key)
		{
			if (keys.Contains(key)) return;

			var old_value = keys;
			var new_value = new List<FCurveKey<T>>();
			new_value.AddRange(keys.Concat( new[] {key}));
			SortKeys(new_value);
			
			var cmd = new Command.DelegateCommand(
				() =>
				{
					keys = new_value;
					key.OnChangedKey += OnChangedKey;

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(new_value, ChangedValueType.Execute));
					}
				},
				() =>
				{
					keys = old_value;
					key.OnChangedKey -= OnChangedKey;

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(old_value, ChangedValueType.Unexecute));
					}
				});

			Command.CommandManager.Execute(cmd);
		}

		public void RemoveKey(IFCurveKey key)
		{
			var k = key as FCurveKey<T>;
			if (k != null)
			{
				RemoveKey(k);
			}
		}

		public void RemoveKey(FCurveKey<T> key)
		{
			if (!keys.Contains(key)) return;

			var old_value = keys;
			var new_value = new List<FCurveKey<T>>();
			new_value.AddRange(keys.Except(new[] { key }));

			var cmd = new Command.DelegateCommand(
				() =>
				{
					keys = new_value;
					key.OnChangedKey -= OnChangedKey;

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(new_value, ChangedValueType.Execute));
					}
				},
				() =>
				{
					keys = old_value;
					key.OnChangedKey += OnChangedKey;

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(old_value, ChangedValueType.Unexecute));
					}
				});

			Command.CommandManager.Execute(cmd);
		}

		void OnChangedKey(FCurveKey<T> key)
		{
			keys.Sort(new KeyComparer());
		}

		public byte[] GetBytes(float mul = 1.0f)
		{
			var freq = Sampling.Value;
			List<byte[]> data = new List<byte[]>();

			data.Add(BitConverter.GetBytes((int)StartType.Value));
			data.Add(BitConverter.GetBytes((int)EndType.Value));
			data.Add(BitConverter.GetBytes(OffsetMax.Value));
			data.Add(BitConverter.GetBytes(OffsetMin.Value));

			if (keys.Count > 0)
			{
				var len = keys.Last().Frame - keys.First().Frame;
				data.Add(BitConverter.GetBytes(keys.First().Frame));
				data.Add(BitConverter.GetBytes(len));
				data.Add(BitConverter.GetBytes(freq));

				int count = 0;
				if (len % freq > 0)
				{
					count = len / freq + 2;
				}
				else
				{
					count = len / freq + 1;
				}

				data.Add(BitConverter.GetBytes(count));

				if (typeof(T) == typeof(float))
				{
					for (int f = keys.First().Frame; f < keys.Last().Frame; f += freq)
					{
						var v = GetValue(f) * mul;
						data.Add(BitConverter.GetBytes(v));
					}

					{
						var v = GetValue(keys.Last().Frame) * mul;
						data.Add(BitConverter.GetBytes(v));
					}
				}
				else if (typeof(T) == typeof(int))
				{
					for (int f = keys.First().Frame; f < keys.Last().Frame; f += freq)
					{
						var v = GetValue(f) * mul;
						data.Add(BitConverter.GetBytes(v));
					}

					{
						var v = GetValue(keys.Last().Frame) * mul;
						data.Add(BitConverter.GetBytes(v));
					}
				}
				
			}
			else
			{
				data.Add(BitConverter.GetBytes(0));
				data.Add(BitConverter.GetBytes(0));
				data.Add(BitConverter.GetBytes(0));
				data.Add(BitConverter.GetBytes(0));
			}

			return data.SelectMany(_ => _).ToArray();
		}

		public void AddKeyDirectly(FCurveKey<T> key)
		{
			if (keys.Contains(key)) return;
			keys.Add(key);
			key.OnChangedKey += OnChangedKey;
			SortKeys(keys);
		}

		public float GetValue(int frame)
		{
			var begin = StartType.Value;
			var end = EndType.Value;

			int lInd = 0;
			int rInd = (int)keys.Count- 1;

			if (keys.Count == 0)
            {
                return ToFloat(DefaultValue);
            }

            int length = keys[rInd].Frame - keys[lInd].Frame;

			if(length == 0)
			{
				return ToFloat(keys[lInd].Value);
			}

			if (keys[rInd].Frame <= frame)
			{
				if (end == FCurveEdge.Constant)
				{
					return ToFloat(keys[rInd].Value);
				}
				else if (end == FCurveEdge.Loop)
				{
					frame = (frame - keys[rInd].Frame) % length + keys[lInd].Frame;
				}
				else if (end == FCurveEdge.LoopInversely)
				{
					frame = (length - (frame - keys[rInd].Frame) % length) + keys[lInd].Frame;
				}
			}

			if (frame <= keys[lInd].Frame)
			{
				if (begin == FCurveEdge.Constant)
				{
					return ToFloat(keys[lInd].Value);
				}
				else if (begin == FCurveEdge.Loop)
				{
					frame = (keys[lInd].Frame - frame) % length + keys[lInd].Frame;
				}
				else if (begin == FCurveEdge.LoopInversely)
				{
					frame = (length - (keys[lInd].Frame - frame) % length) + keys[lInd].Frame;
				}
			}

			while (true)
			{
				int mInd = (lInd + rInd) / 2;

				/* lIndとrIndの間 */
				if (mInd == lInd)
				{
					if (keys[lInd].InterpolationType == FCurveInterpolation.Linear)
					{
						float subF = (float)(keys[rInd].Frame - keys[lInd].Frame);
						var subV = ToFloat(keys[rInd].Value) - ToFloat(keys[lInd].Value);

						if (subF == 0) return ToFloat(keys[lInd].Value);

						return subV / (float)(subF) * (float)(frame - keys[lInd].Frame) + ToFloat(keys[lInd].Value);
					}
					else if (keys[lInd].InterpolationType == FCurveInterpolation.Bezier)
					{
						if (keys[lInd].Frame == frame) return ToFloat(keys[lInd].Value);

						float[] k1 = new float[2];
						float[] k1rh = new float[2];
						float[] k2lh = new float[2];
						float[] k2 = new float[2];

						k1[0] = keys[lInd].Frame;
						k1[1] = ToFloat(keys[lInd].Value);

						k1rh[0] = keys[lInd].RightX;
						k1rh[1] = keys[lInd].RightY;

						k2lh[0] = keys[rInd].LeftX;
						k2lh[1] = keys[rInd].LeftY;

						k2[0] = keys[rInd].Frame;
						k2[1] = ToFloat(keys[rInd].Value);

						NormalizeValues(k1, k1rh, k2lh, k2);
						float t = 0;
						var getT = CalcT(frame, k1[0], k1rh[0], k2lh[0], k2[0], out t);
						if (getT)
						{
							return CalcBezierValue(k1[1], k1rh[1], k2lh[1], k2[1], t);
						}
						else
						{
							return 0;
						}
					}
				}

				if (keys[mInd].Frame <= frame)
				{
					lInd = mInd;
				}
				else if (frame <= keys[mInd].Frame)
				{
					rInd = mInd;
				}
			}
		}

		/// <summary>
		/// キーをソートする。
		/// </summary>
		/// <param name="keys"></param>
		/// <returns></returns>
		public static bool SortKeys(ICollection<FCurveKey<T>> keys)
		{
			if (keys is List<FCurveKey<T>>)
			{
				var ks = keys as List<FCurveKey<T>>;
				ks.Sort(new KeyComparer());
				return true;
			}

			return false;
		}

		/// <summary>
		/// 三乗根を求める。
		/// </summary>
		/// <param name="value"></param>
		/// <returns></returns>
		static float Sqrt3(float value)
		{
			if (value == 0)
			{
				return 0;
			}
			else if (value > 0)
			{
				return (float)Math.Pow(value, 1.0 / 3.0);
			}
			else
			{
				return -(float)Math.Pow(-value, 1.0 / 3.0);
			}
		}

		/// <summary>
		/// 平方根を求める。
		/// </summary>
		/// <param name="value"></param>
		/// <returns></returns>
		static float Sqrt(float value)
		{
			return (float)Math.Sqrt(value);
		}

		/// <summary>
		/// 正しく値を求めれるように補正を行う。
		/// </summary>
		/// <param name="k1"></param>
		/// <param name="k1rh"></param>
		/// <param name="k2lh"></param>
		/// <param name="k2"></param>
		static void NormalizeValues(float[] k1, float[] k1rh, float[] k2lh, float[] k2)
		{
			var h1 = new float[2];
			var h2 = new float[2];

			// 傾きを求める
			h1[0] = k1[0] - k1rh[0];
			h1[1] = k1[1] - k1rh[1];

			h2[0] = k2[0] - k2lh[0];
			h2[1] = k2[1] - k2lh[1];

			// キーフレーム間の長さ
			var len = k2[0] - k1[0];

			// キー1の右手の長さ
			var lenR = Math.Abs(h1[0]);

			// キー2の左手の長さ
			var lenL = Math.Abs(h2[0]);

			// 長さがない
			if (lenR == 0 && lenL == 0) return;

			// 手が重なっている場合、補正
			if ((lenR + lenL) > len)
			{
				var f = len / (lenR + lenL);

				k1rh[0] = (k1[0] - f * h1[0]);
				k1rh[1] = (k1[1] - f * h1[1]);

				k2lh[0] = (k2[0] - f * h2[0]);
				k2lh[1] = (k2[1] - f * h2[1]);
			}
		}

		/// <summary>
		/// 二次方程式の解を求める。
		/// </summary>
		/// <param name="a"></param>
		/// <param name="b"></param>
		/// <param name="c"></param>
		/// <param name="r1"></param>
		/// <param name="r2"></param>
		static void QuadraticFormula(float a, float b, float c, out float r1, out float r2)
		{
			r1 = float.NaN;
			r2 = float.NaN;

			if (a != 0.0)
			{
				float p = b * b - 4 * a * c;

				if (p > 0)
				{
					p = Sqrt(p);
					r1 = (-b - p) / (2 * a);
					r2 = (-b + p) / (2 * a);
				}
				else if (p == 0)
				{
					r1 = -b / (2 * a);
				}
			}
			else if (b != 0.0)
			{
				r1 = -c / b;
			}
			else if (c == 0.0f)
			{
				r1 = 0.0f;
			}
		}

		/// <summary>
		/// 三次方程式の解を求める。
		/// </summary>
		/// <param name="frame"></param>
		/// <param name="k1X"></param>
		/// <param name="k1rhX"></param>
		/// <param name="k2lhX"></param>
		/// <param name="k2X"></param>
		/// <param name="r"></param>
		/// <returns></returns>
		static bool CalcT(float frame, float k1X, float k1rhX, float k2lhX, float k2X, out float r)
		{
			Func<float, bool> isValid = (v) =>
				{
					if (float.IsNaN(v)) return false;

					const float small = -0.00000001f;
					const float big = 1.000001f;
					return (v >= small) && (v <= big);
				};

			var c3_ = k2X - k1X + 3.0f * (k1rhX - k2lhX);
			var c2_ = 3.0f * (k1X - 2.0f * k1rhX + k2lhX);
			var c1_ = 3.0f * (k1rhX - k1X);
			var c0_ = k1X - frame;
			
			if (c3_ != 0.0)
			{
				var c2 = c2_ / c3_;
				var c1 = c1_ / c3_;
				var c0 = c0_ / c3_;

				var p = c1 / 3.0f - c2 * c2 / (3.0f * 3.0f);
				var q = (2.0f * c2 * c2 * c2 / (3.0f*3.0f*3.0f) - c2 / 3.0f * c1 + c0) / 2.0f;
				var p3q2 = q * q + p * p * p;

				if (p3q2 > 0.0)
				{
					var t = Sqrt(p3q2);
					var u = Sqrt3(-q + t);
					var v = Sqrt3(-q - t);
					r = u + v - c2 / 3.0f;

					if (isValid(r)) return true;
					return false;
				}
				else if (p3q2 == 0.0)
				{
					var u = Sqrt3(-q);
					var v = Sqrt3(-q);
					r = u + v - c2 / 3.0f;
					if (isValid(r)) return true;

					u = -Sqrt3(-q);
					v = -Sqrt3(-q);
					r = u + v - c2 / 3.0f;
					if (isValid(r)) return true;

					return false;
				}
				else
				{
					// ビエタの解
					var phi = (float)Math.Acos(-q / Sqrt(-(p * p * p)));
					var pd3 = (float)Math.Cos(phi / 3);
					var rmp = Sqrt(-p);

					r = 2.0f * rmp * (float)Math.Cos(phi / 3) - c2 / 3.0f;
					if (isValid(r)) return true;

					var q2 = Sqrt(3 - 3 * pd3 * pd3);

					//r = -rmp * (pd3 + q2) - a;
					r = 2.0f * rmp * (float)Math.Cos(phi / 3 + 2.0 * Math.PI / 3.0) - c2 / 3.0f;
					if (isValid(r)) return true;

					//r = -rmp * (pd3 - q2) - a;
					r = 2.0f * rmp * (float)Math.Cos(phi / 3 + 4.0 * Math.PI / 3.0) - c2 / 3.0f;
					if (isValid(r)) return true;
					return false;
				}
			}
			else
			{
				// 2次方程式のケース
				float r1;
				float r2;
				QuadraticFormula(c2_, c1_, c0_, out r1, out r2);

				r = r1;
				if (isValid(r)) return true;

				r = r2;
				if (isValid(r)) return true;

				r = 0.0f;
				return false;
			}
		}

		/// <summary>
		/// 三次方程式の解から値を求める。
		/// </summary>
		/// <param name="k1Y"></param>
		/// <param name="k1rhY"></param>
		/// <param name="k2lhY"></param>
		/// <param name="k2Y"></param>
		/// <param name="t"></param>
		/// <returns></returns>
		static float CalcBezierValue(float k1Y, float k1rhY, float k2lhY, float k2Y, float t)
		{
			var c0 = k1Y;
			var c1 = 3.0f * (k1rhY - k1Y);
			var c2 = 3.0f * (k1Y - 2.0f * k1rhY + k2lhY);
			var c3 = k2Y - k1Y + 3.0f * (k1rhY - k2lhY);

			return c0 + t * c1 + t * t * c2 + t * t * t * c3;
		}



		public class KeyComparer : System.Collections.Generic.IComparer<FCurveKey<T>>
		{
			public int Compare(FCurveKey<T> key1, FCurveKey<T> key2)
			{
				if (key1.Frame > key2.Frame)
				{
					return 1;
				}
				else if (key1.Frame < key2.Frame)
				{
					return -1;
				}

				return key1.Value.CompareTo(key2.Value);
			}
		}
	}

	public interface IFCurveKey
	{
		int Frame { get; }
		float ValueAsFloat { get; }
		float LeftX { get; }
		float LeftY { get; }

		float RightX { get; }
		float RightY { get; }

		int FramePrevious { get; }
		float ValuePreviousAsFloat { get; }
		float LeftXPrevious { get; }
		float LeftYPrevious { get; }

		float RightXPrevious { get; }
		float RightYPrevious { get; }

		FCurveInterpolation InterpolationType { get; }

		void Commit(bool isCombined = false);

		void SetFrame(int frame);
		void SetValueAsFloat(float value);
		void SetLeftX(float value);
		void SetLeftY(float value);
		void SetRightX(float value);
		void SetRightY(float value);

		void SetLeft(float x, float y);
		void SetRight(float x, float y);

		event ChangedValueEventHandler OnChanged;
	}

	public class FCurveKey<T> : IFCurveKey where T : struct, IComparable<T>, IEquatable<T>
	{
		int _frame = 0;
		T _value = default(T);
		float _left_x = 0;
		float _left_y = 0;
		float _right_x = 0;
		float _right_y = 0;
		FCurveInterpolation _interpolation_type = FCurveInterpolation.Bezier;


		int _frame_temp = 0;
		T _value_temp = default(T);
		float _left_x_temp = 0;
		float _left_y_temp = 0;
		float _right_x_temp = 0;
		float _right_y_temp = 0;

        float ToFloat(T v)
        {
            var o = (object)v;

            if (o is float) return (float)o;

            if (v is int)
            {
                var b = (int)o;
                return (float)b;
            }

            throw new NotImplementedException();
        }

        public event Action<FCurveKey<T>> OnChangedKey;

		public int Frame
		{
			get
			{
				return _frame_temp;
			}
		}

		public T Value
		{
			get
			{
				return _value_temp;
			}
		}

		public float ValueAsFloat
		{
			get
			{
                return ToFloat(_value_temp);
            }
		}

		public float LeftX
		{
			get
			{
				return _left_x_temp;
			}
		}

		public float LeftY
		{
			get
			{
				return _left_y_temp;
			}
		}

		public float RightX
		{
			get
			{
				return _right_x_temp;
			}
		}

		public float RightY
		{
			get
			{
				return _right_y_temp;
			}
		}

		public int FramePrevious
		{
			get
			{
				return _frame;
			}
		}

		public T ValuePrevious
		{
			get
			{
				return _value;
			}
		}

		public float ValuePreviousAsFloat
		{
			get
			{
                return ToFloat(_value);

            }
		}

		public float LeftXPrevious
		{
			get
			{
				return _left_x;
			}
		}

		public float LeftYPrevious
		{
			get
			{
				return _left_y;
			}
		}

		public float RightXPrevious
		{
			get
			{
				return _right_x;
			}
		}

		public float RightYPrevious
		{
			get
			{
				return _right_y;
			}
		}

		public FCurveInterpolation InterpolationType
		{
			get
			{
				return _interpolation_type;
			}
		}

		public void Commit(bool isCombined = false)
		{
			if (_frame == _frame_temp && 
				_value.Equals(_value_temp) &&
				_left_x.Equals(_left_x_temp) &&
				_left_y.Equals(_left_y_temp) &&
				_right_x.Equals(_right_x_temp) &&
				_right_y.Equals(_right_y_temp)) return;
		
			var old_frame = _frame;
			var new_frame = _frame_temp;

			var old_value = _value;
			var new_value = _value_temp;

			var old_left_x = _left_x;
			var new_left_x = _left_x_temp;
			var old_left_y = _left_y;
			var new_left_y = _left_y_temp;

			var old_right_x = _right_x;
			var new_right_x = _right_x_temp;
			var old_right_y = _right_y;
			var new_right_y = _right_y_temp;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					_frame = new_frame;
					_value = new_value;

					_left_x = new_left_x;
					_left_y = new_left_y;
					_right_x = new_right_x;
					_right_y = new_right_y;
					
					_frame_temp = new_frame;
					_value_temp = new_value;

					_left_x_temp = new_left_x;
					_left_y_temp = new_left_y;
					_right_x_temp = new_right_x;
					_right_y_temp = new_right_y;
					

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(new_value, ChangedValueType.Execute));
					}

					if (OnChangedKey != null)
					{
						OnChangedKey(this);
					}
				},
				() =>
				{
					_frame = old_frame;
					_value = old_value;

					_left_x = old_left_x;
					_left_y = old_left_y;
					_right_x = old_right_x;
					_right_y = old_right_y;

					_frame_temp = old_frame;
					_value_temp = old_value;

					_left_x_temp = old_left_x;
					_left_y_temp = old_left_y;
					_right_x_temp = old_right_x;
					_right_y_temp = old_right_y;

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(old_value, ChangedValueType.Unexecute));
					}

					if (OnChangedKey != null)
					{
						OnChangedKey(this);
					}
				},
				this,
				isCombined);

			Command.CommandManager.Execute(cmd);
		}

		public void SetFrame(int frame)
		{
			if (frame == _frame_temp) return;
			var dif = frame - _frame_temp;
			_frame_temp = frame;

			_left_x_temp += dif;
			_right_x_temp += dif;

			if (OnChangedKey != null)
			{
				OnChangedKey(this);
			}
		}

		public void SetValue(T value)
		{
			if (value.CompareTo(_value_temp) == 0) return;
			var dif = ToFloat(value) - ToFloat(_value_temp);
			_value_temp = value;

			_left_y_temp += dif;
			_right_y_temp += dif;

			if (OnChangedKey != null)
			{
				OnChangedKey(this);
			}
		}

		public void SetValueAsFloat(float value)
		{
			if (typeof(T) == typeof(int))
			{
				value = (float)Math.Round((double)value);
				value = Math.Max(0, value);
				value = Math.Min(255, value);
			}

			var valNow = ToFloat(_value_temp);
			if (valNow == value) return;
			
			var dif = value - valNow;
			_value_temp = (T)((object)value);

			_left_y_temp += dif;
			_right_y_temp += dif;

			if (OnChangedKey != null)
			{
				OnChangedKey(this);
			}
		}

		public void SetLeftX(float value)
		{
			if (value.CompareTo(_left_x_temp) == 0) return;

			_left_x_temp = value;
			if (_left_x_temp > _frame_temp)
			{
				_left_x_temp = _frame_temp;
			}

			// 傾き調整
			var _valueF = ToFloat(_value_temp);
			var dxR = _right_x_temp - _frame_temp;
			var dyR = _right_y_temp - ToFloat(_value_temp);

			var dxL = _left_x_temp - _frame_temp;
			var dyL = _left_y_temp - ToFloat(_value_temp);

			if (dxR != 0)
			{ 
				var h = dyR / dxR;
				_left_y_temp = _valueF + dxL * h;
			}
			else if (dxR == 0 && dyR != 0.0f)
			{
				_left_x_temp = _frame_temp;
			}
			
			if (OnChangedKey != null)
			{
				OnChangedKey(this);
			}
		}

		public void SetLeftY(float value)
		{
			if (value.CompareTo(_left_y_temp) == 0) return;

			_left_y_temp = value;

			// 傾き調整
			var _valueF = ToFloat(_value_temp);
			var dxR = _right_x_temp - _frame_temp;
			var dyR = _right_y_temp - ToFloat(_value_temp);

			var dxL = _left_x_temp - _frame_temp;
			var dyL = _left_y_temp - ToFloat(_value_temp);

			if (dxR != 0)
			{
				var h = dyR / dxR;
				_left_x_temp = _frame_temp + dyL / h;
			}
			else if (dxR == 0 && dyR != 0.0f)
			{
				_left_x_temp = _frame_temp;
			}

			if (OnChangedKey != null)
			{
				OnChangedKey(this);
			}
		}

		public void SetRightX(float value)
		{
			if (value.CompareTo(_right_x_temp) == 0) return;
			_right_x_temp = value;
			if (_right_x_temp < _frame_temp) _right_x_temp = _frame_temp;

			CorrectGradientRight();

			if (OnChangedKey != null)
			{
				OnChangedKey(this);
			}
		}

		public void SetRightY(float value)
		{
			if (value.CompareTo(_right_y_temp) == 0) return;
			_right_y_temp = value;

			CorrectGradientRight();

			if (OnChangedKey != null)
			{
				OnChangedKey(this);
			}
		}

		public void SetLeft(float x, float y)
		{
			if (x.CompareTo(_left_x_temp) == 0 &&
				y.CompareTo(_left_y_temp) == 0) return;

			_left_x_temp = x;
			if (_left_x_temp > _frame_temp)
			{
				_left_x_temp = _frame_temp;
			}
			_left_y_temp = y;

			var _valueF = ToFloat(_value_temp);
			var dxR = _right_x_temp - _frame_temp;
			var dyR = _right_y_temp - ToFloat(_value_temp);

			var dxL = _left_x_temp - _frame_temp;
			var dyL = _left_y_temp - ToFloat(_value_temp);

			var lenL = (float)Math.Sqrt(dxL * dxL + dyL * dyL);
			var lenR = (float)Math.Sqrt(dxR * dxR + dyR * dyR);

			if (lenL > 0 && lenR > 0)
			{
				_right_x_temp = _frame_temp - dxL / lenL * lenR;
				_right_y_temp = _valueF - dyL / lenL * lenR;
			}

			if (OnChangedKey != null)
			{
				OnChangedKey(this);
			}
		}

		public void SetRight(float x, float y)
		{
			if (x.CompareTo(_right_x_temp) == 0 &&
				y.CompareTo(_right_y_temp) == 0) return;

			_right_x_temp = x;
			if (_right_x_temp < _frame_temp) _right_x_temp = _frame_temp;
			_right_y_temp = y;

			CorrectGradientRight();

			if (OnChangedKey != null)
			{
				OnChangedKey(this);
			}
		}

		public void SetLeftDirectly(float x, float y)
		{
			_left_x = x;
			_left_x_temp = x;
			_left_y = y;
			_left_y_temp = y;
		}

		public void SetRightDirectly(float x, float y)
		{
			_right_x = x;
			_right_x_temp = x;
			_right_y = y;
			_right_y_temp = y;
		}

		/// <summary>
		/// 傾き調整
		/// </summary>
		void CorrectGradientRight()
		{
			var _valueF = ToFloat(_value_temp);
			var dxR = _right_x_temp - _frame_temp;
			var dyR = _right_y_temp - ToFloat(_value_temp);

			var dxL = _left_x_temp - _frame_temp;
			var dyL = _left_y_temp - ToFloat(_value_temp);

			var lenL = (float)Math.Sqrt(dxL * dxL + dyL * dyL);
			var lenR = (float)Math.Sqrt(dxR * dxR + dyR * dyR);

			if (lenL > 0 && lenR > 0)
			{
				_left_x_temp = _frame_temp - dxR / lenR * lenL;
				_left_y_temp = _valueF - dyR / lenR * lenL;
			}
		}

		public void SetInterpolationType(FCurveInterpolation interpolation_type)
		{
			_interpolation_type = interpolation_type;
		}

		public FCurveKey(int frame = 0, T value = default(T))
		{
			_frame = frame;
			_value = value;
			_frame_temp = frame;
			_value_temp = value;

			_left_x = frame;
			_left_x_temp = frame;
			_right_x = frame;
			_right_x_temp = frame;

			_left_y = ToFloat(value);
			_left_y_temp = ToFloat(value);
			_right_y = ToFloat(value);
			_right_y_temp = ToFloat(value);
		}

		public event ChangedValueEventHandler OnChanged;
	}
}
