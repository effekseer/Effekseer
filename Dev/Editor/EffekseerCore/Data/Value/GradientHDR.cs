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

		State _value = null;

		public State Value
		{
			get;

		}

		static GradientHDR()
		{
			Command.CommandManager.AddConvertFunction((commands) =>
			{
				var cmds = commands.OfType<ChangeCommand>().ToArray();
				if (commands.Count() != cmds.Count())
				{
					return null;
				}

				if (!(cmds.First().Identifier is GradientHDR))
				{
					return null;
				}

				var identifers = cmds.Select(_ => _.Identifier).Distinct().ToArray();
				if (identifers.Count() != 1)
				{
					return null;
				}

				var owner = identifers.First() as GradientHDR;

				var first = cmds.First();
				var last = cmds.Last();

				var cmd = new ChangeCommand(
					first.OldValue,
					last.NewValue,
					() =>
					{
						owner._value = last.NewValue;
						owner.CallChanged(owner._value, ChangedValueType.Execute);
					},
					() =>
					{
						owner._value = first.OldValue;
						owner.CallChanged(owner._value, ChangedValueType.Unexecute);
					},
					owner,
					false);

				return cmd;
			});
		}

		public unsafe GradientHDR()
		{
			_value = new State();
			_value.ColorMarkers = new ColorMarker[2];
			_value.ColorMarkers[0].Position = 0;
			_value.ColorMarkers[0].Intensity = 1;
			_value.ColorMarkers[0].Color[0] = 1.0f;
			_value.ColorMarkers[0].Color[1] = 1.0f;
			_value.ColorMarkers[0].Color[2] = 1.0f;

			_value.ColorMarkers[1].Position = 1;
			_value.ColorMarkers[1].Intensity = 1;
			_value.ColorMarkers[1].Color[0] = 1.0f;
			_value.ColorMarkers[1].Color[1] = 1.0f;
			_value.ColorMarkers[1].Color[2] = 1.0f;

			_value.AlphaMarkers = new AlphaMarker[2];
			_value.AlphaMarkers[0].Position = 0.0f;
			_value.AlphaMarkers[0].Alpha = 1.0f;
			_value.AlphaMarkers[1].Position = 1.0f;
			_value.AlphaMarkers[1].Alpha = 1.0f;
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

			var cmd = new ChangeCommand(
				old_value,
				new_value,
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

		class ChangeCommand : Command.DelegateCommand
		{
			public State OldValue;
			public State NewValue;
			public ChangeCommand(State oldValue, State newValue, System.Action execute, System.Action unexecute, object identifier, bool isCombined)
				: base(execute, unexecute, identifier, isCombined)
			{
				OldValue = oldValue;
				NewValue = newValue;
			}
		}
	}
}