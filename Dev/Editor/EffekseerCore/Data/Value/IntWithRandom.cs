using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class IntWithRandom : IValueChangedFromDefault
	{
		int _value_center = 0;
		int _value_max = 0;
		int _value_min = 0;

		int _max = int.MaxValue;
		int _min = int.MinValue;

		public int Center
		{
			get
			{
				return GetCenter();
			}

			set
			{
				SetCenter(value);
			}
		}

		public int Max
		{
			get
			{
				return GetMax();
			}

			set
			{
				SetMax(value);
			}
		}

		public int Min
		{
			get
			{
				return GetMin();
			}

			set
			{
				SetMin(value);
			}
		}

		public int Amplitude
		{
			get
			{
				return GetAmplitude();
			}

			set
			{
				SetAmplitude(value);
			}
		}

		public DrawnAs DrawnAs
		{
			get;
			set;
		}

		public int Step
		{
			get;
			set;
		}

		public int ValueMin
		{
			get { return _min; }
		}

		public int ValueMax
		{
			get { return _max; }
		}

		internal int DefaultValueCenter { get; private set; }
		internal int DefaultValueMax { get; private set; }
		internal int DefaultValueMin { get; private set; }
		internal DrawnAs DefaultDrawnAs { get; private set; }

		public event ChangedValueEventHandler OnChanged;

		public bool CanSelectDynamicEquation = false;

		public Boolean IsDynamicEquationEnabled
		{
			get;
			private set;
		}

		public DynamicEquationReference DynamicEquationMin
		{
			get;
			private set;
		}

		public DynamicEquationReference DynamicEquationMax
		{
			get;
			private set;
		}

		public bool IsValueChangedFromDefault
		{
			get { return Center != DefaultValueCenter || Min != DefaultValueMin || Max != DefaultValueMax || DrawnAs != DefaultDrawnAs; }
		}

		internal IntWithRandom(int value = 0, int max = int.MaxValue, int min = int.MinValue, DrawnAs drawnas = Data.DrawnAs.CenterAndAmplitude, int step = 1)
		{
			_value_center = value;
			_value_max = value;
			_value_min = value;
			_max = max;
			_min = min;
			DrawnAs = drawnas;
			Step = step;

			IsDynamicEquationEnabled = new Boolean();
			DynamicEquationMin = new DynamicEquationReference();
			DynamicEquationMax = new DynamicEquationReference();

			DefaultValueCenter = _value_center;
			DefaultValueMax = _value_max;
			DefaultValueMin = _value_min;
			DefaultDrawnAs = DrawnAs;
		}

		internal void CallChanged(object value, ChangedValueType type)
		{
			if (OnChanged != null)
			{
				OnChanged(this, new ChangedValueEventArgs(value, type));
			}
		}

		public int GetCenter()
		{
			return _value_center;
		}

		public void SetCenter(int value, bool isCombined = false)
		{
			value = Math.Min(value, _max);
			value = Math.Max(value, _min);

			if (value == _value_center) return;

			int a = Amplitude;

			int old_center = _value_center;
			int new_center = value;
			int old_max = _value_max;
			int new_max = Math.Min(value + a, _max);
			int old_min = _value_min;
			int new_min = Math.Max(value - a, _min);

			var cmd = new Command.DelegateCommand(
				() =>
				{
					_value_center = new_center;
					_value_max = new_max;
					_value_min = new_min;

					CallChanged(new_center, ChangedValueType.Execute);
				},
				() =>
				{
					_value_center = old_center;
					_value_max = old_max;
					_value_min = old_min;

					CallChanged(old_center, ChangedValueType.Unexecute);
				},
				this,
				isCombined);

			Command.CommandManager.Execute(cmd);
		}

		internal void SetCenterDirectly(int value)
		{
			_value_center = value;
		}

		public int GetMax()
		{
			return _value_max;
		}

		public void SetMax(int value, bool isCombined = false)
		{
			value = Math.Min(value, _max);
			value = Math.Max(value, _min);

			if (value == _value_max) return;

			int old_max = _value_max;
			int new_max = value;
			int old_min = _value_min;
			int new_min = Math.Min(value, _value_min);
			int old_center = _value_center;
			int new_center = (new_max + new_min) / 2;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					_value_center = new_center;
					_value_max = new_max;
					_value_min = new_min;

					CallChanged(new_max, ChangedValueType.Execute);
				},
				() =>
				{
					_value_center = old_center;
					_value_max = old_max;
					_value_min = old_min;

					CallChanged(old_max, ChangedValueType.Unexecute);
				},
				this,
				isCombined);

			Command.CommandManager.Execute(cmd);
		}

		internal void SetMaxDirectly(int value)
		{
			value = Math.Min(value, _max);

			_value_max = value;
		}

		public int GetMin()
		{
			return _value_min;
		}

		public void SetMin(int value, bool isCombined = false)
		{
			value = Math.Min(value, _max);
			value = Math.Max(value, _min);

			if (value == _value_min) return;

			int old_max = _value_max;
			int new_max = Math.Max(value, _value_max);
			int old_min = _value_min;
			int new_min = value;
			int old_center = _value_center;
			int new_center = (new_max + new_min) / 2;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					_value_center = new_center;
					_value_max = new_max;
					_value_min = new_min;

					CallChanged(new_min, ChangedValueType.Execute);
				},
				() =>
				{
					_value_center = old_center;
					_value_max = old_max;
					_value_min = old_min;

					CallChanged(old_min, ChangedValueType.Unexecute);
				},
				this,
				isCombined);

			Command.CommandManager.Execute(cmd);
		}

		internal void SetMinDirectly(int value)
		{
			value = Math.Max(value, _min);

			_value_min = value;
		}

		public int GetAmplitude()
		{
			return Math.Max(_value_max - _value_center, _value_center - _value_min);
		}

		public void SetAmplitude(int value, bool isCombined = false)
		{
			value = Math.Max(value, 0);

			int old_center = _value_center;
			int new_center = _value_center;
			int old_max = _value_max;
			int new_max = Math.Min(_value_center + value, _max);
			int old_min = _value_min;
			int new_min = Math.Max(_value_center - value, _min);

			if (new_max == old_max && new_min == old_min) return;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					_value_center = new_center;
					_value_max = new_max;
					_value_min = new_min;

					CallChanged(new_min, ChangedValueType.Execute);
				},
				() =>
				{
					_value_center = old_center;
					_value_max = old_max;
					_value_min = old_min;

					CallChanged(old_min, ChangedValueType.Unexecute);
				},
				this,
				isCombined);

			Command.CommandManager.Execute(cmd);
		}

		public byte[] GetBytes()
		{
			byte[] values = new byte[sizeof(float) * 2];
			BitConverter.GetBytes(Max).CopyTo(values, sizeof(float) * 0);
			BitConverter.GetBytes(Min).CopyTo(values, sizeof(float) * 1);
			return values;
		}
	}
}