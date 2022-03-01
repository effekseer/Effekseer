using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class GradientHDR
	{
		public unsafe struct ColorMarker
		{
			public float Position;
			public fixed float Color[3];
			public float Intensity;
		}

		public struct AlphaMarker
		{
			public float Position;
			public float Alpha;
		}

		public class State : ICloneable
		{
			public ColorMarker[] ColorMarkers;
			public AlphaMarker[] AlphaMarkers;

			public object Clone()
			{
				var state = new State();

				state.ColorMarkers = (ColorMarker[])ColorMarkers.Clone();
				state.AlphaMarkers = (AlphaMarker[])AlphaMarkers.Clone();

				return state;
			}
		}

		State _value;

		public State Value
		{
			get;

		}

		public State GetValue()
		{
			return _value;
		}

		public void SetValue(State value, bool isCombined = false)
		{
			value = (State)value.Clone();

			if (value == _value) return;

			var old_value = _value;
			var new_value = value;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					_value = new_value;
					CallChanged(new_value, ChangedValueType.Execute);
				},
				() =>
				{
					_value = old_value;
					CallChanged(old_value, ChangedValueType.Unexecute);
				},
				this,
				isCombined);

			Command.CommandManager.Execute(cmd);
		}

		protected void CallChanged(object value, ChangedValueType type)
		{
			if (OnChanged != null)
			{
				OnChanged(this, new ChangedValueEventArgs(value, type));
			}
		}

		public event ChangedValueEventHandler OnChanged;
	}
}