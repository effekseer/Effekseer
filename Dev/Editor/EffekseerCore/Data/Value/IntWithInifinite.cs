using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class IntWithInifinite
	{
		public Int Value
		{
			get;
			private set;
		}
		public Boolean Infinite
		{
			get;
			private set;
		}
		public int Step
		{
			get;
			private set;
		}

		public bool IsValueChangedFromDefault
		{
			get { return Value.IsValueChangedFromDefault || Infinite.IsValueChangedFromDefault; }
		}

		public event ChangedValueEventHandler OnChanged;

		public bool CanSelectDynamicEquation = false;

		public Boolean IsDynamicEquationEnabled
		{
			get;
			private set;
		}

		public DynamicEquationReference DynamicEquation
		{
			get;
			private set;
		}

		internal IntWithInifinite(int value = 0, bool infinite = false, int max = int.MaxValue, int min = int.MinValue, int step = 1)
		{
			Value = new Int(value, max, min, step);
			Infinite = new Boolean(infinite);

			IsDynamicEquationEnabled = new Boolean();
			DynamicEquation = new DynamicEquationReference();
		}
	}
}
