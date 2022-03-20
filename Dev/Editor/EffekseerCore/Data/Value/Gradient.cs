using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using Effekseer.Utl;

namespace Effekseer.Data.Value
{
	public class Gradient
	{
		public unsafe struct ColorMarker
		{
			public float Position;
			public float ColorR;
			public float ColorG;
			public float ColorB;
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

			public unsafe byte[] ToBinary()
			{
				List<byte[]> data = new List<byte[]>();
				data.Add(BitConverter.GetBytes(ColorMarkers.Length));

				for (int i = 0; i < ColorMarkers.Length; i++)
				{
					data.Add(BitConverter.GetBytes(ColorMarkers[i].Position));
					data.Add(BitConverter.GetBytes(ColorMarkers[i].ColorR));
					data.Add(BitConverter.GetBytes(ColorMarkers[i].ColorG));
					data.Add(BitConverter.GetBytes(ColorMarkers[i].ColorB));
					data.Add(BitConverter.GetBytes(ColorMarkers[i].Intensity));
				}

				data.Add(BitConverter.GetBytes(AlphaMarkers.Length));

				for (int i = 0; i < AlphaMarkers.Length; i++)
				{
					data.Add(BitConverter.GetBytes(AlphaMarkers[i].Position));
					data.Add(BitConverter.GetBytes(AlphaMarkers[i].Alpha));
				}

				return data.SelectMany(_ => _).ToArray();
			}

			public object Clone()
			{
				var state = new State();

				state.ColorMarkers = (ColorMarker[])ColorMarkers.Clone();
				state.AlphaMarkers = (AlphaMarker[])AlphaMarkers.Clone();

				return state;
			}

			public unsafe override bool Equals(object obj)
			{
				var o = (State)obj;
				if (o == null)
				{
					return false;
				}

				if (ColorMarkers.Count() != o.ColorMarkers.Count() || AlphaMarkers.Count() != o.AlphaMarkers.Count())
				{
					return false;
				}

				for (int i = 0; i < ColorMarkers.Count(); i++)
				{
					if (ColorMarkers[i].ColorR != o.ColorMarkers[i].ColorR ||
						ColorMarkers[i].ColorG != o.ColorMarkers[i].ColorG ||
						ColorMarkers[i].ColorB != o.ColorMarkers[i].ColorB ||
						ColorMarkers[i].Intensity != o.ColorMarkers[i].Intensity ||
						ColorMarkers[i].Position != o.ColorMarkers[i].Position)
					{
						return false;
					}
				}

				for (int i = 0; i < AlphaMarkers.Count(); i++)
				{
					if (
						AlphaMarkers[i].Alpha != o.AlphaMarkers[i].Alpha ||
						AlphaMarkers[i].Position != o.AlphaMarkers[i].Position)
					{
						return false;
					}
				}

				return true;
			}
		}

		State _value = null;

		public State Value
		{
			get;
		}

		public State DefaultValue
		{
			get;
			set;
		}

		public unsafe static XmlElement SaveToElement(XmlDocument doc, string element_name, object o, bool isClip)
		{
			var target = o as Gradient;

			if (target._value.Equals(target.DefaultValue))
			{
				return null;
			}

			var root = doc.CreateElement(element_name);
			var colorMarkers = doc.CreateElement("ColorMarkers");
			var alphaMarkers = doc.CreateElement("AlphaMarkers");

			foreach (var c in target._value.ColorMarkers)
			{
				var key = doc.CreateElement("Key");
				key.AppendChild(doc.CreateTextElement("ColorR", c.ColorR.ToString()));
				key.AppendChild(doc.CreateTextElement("ColorG", c.ColorG.ToString()));
				key.AppendChild(doc.CreateTextElement("ColorB", c.ColorB.ToString()));
				key.AppendChild(doc.CreateTextElement("Position", c.Position.ToString()));
				key.AppendChild(doc.CreateTextElement("Intensity", c.Intensity.ToString()));
				colorMarkers.AppendChild(key);
			}

			foreach (var a in target._value.AlphaMarkers)
			{
				var key = doc.CreateElement("Key");
				key.AppendChild(doc.CreateTextElement("Position", a.Position.ToString()));
				key.AppendChild(doc.CreateTextElement("Alpha", a.Alpha.ToString()));
				alphaMarkers.AppendChild(key);
			}

			root.AppendChild(colorMarkers);
			root.AppendChild(alphaMarkers);

			return root;
		}

		public unsafe static void LoadFromElement(XmlElement e, object o, bool isClip)
		{
			var target = o as Gradient;

			var colorMarkers = e["ColorMarkers"];
			var alphaMarkers = e["AlphaMarkers"];

			var _value = new State();
			_value.ColorMarkers = new ColorMarker[colorMarkers.ChildNodes.Count];
			for (int i = 0; i < colorMarkers.ChildNodes.Count; i++)
			{
				_value.ColorMarkers[i].ColorR = colorMarkers.ChildNodes[i]["ColorR"].GetTextAsFloat();
				_value.ColorMarkers[i].ColorG = colorMarkers.ChildNodes[i]["ColorG"].GetTextAsFloat();
				_value.ColorMarkers[i].ColorB = colorMarkers.ChildNodes[i]["ColorB"].GetTextAsFloat();
				_value.ColorMarkers[i].Position = colorMarkers.ChildNodes[i]["Position"].GetTextAsFloat();
				_value.ColorMarkers[i].Intensity = colorMarkers.ChildNodes[i]["Intensity"].GetTextAsFloat();
			}

			_value.AlphaMarkers = new AlphaMarker[alphaMarkers.ChildNodes.Count];
			for (int i = 0; i < alphaMarkers.ChildNodes.Count; i++)
			{
				_value.AlphaMarkers[i].Position = alphaMarkers.ChildNodes[i]["Position"].GetTextAsFloat();
				_value.AlphaMarkers[i].Alpha = alphaMarkers.ChildNodes[i]["Alpha"].GetTextAsFloat();
			}

			target._value = _value;
		}

		public unsafe byte[] ToBinary()
		{
			return _value.ToBinary();
		}

		static Gradient()
		{
			IO.ExtendSupportedType(typeof(Gradient), SaveToElement, LoadFromElement);

			Command.CommandManager.AddConvertFunction((commands) =>
			{
				var cmds = commands.OfType<ChangeCommand>().ToArray();
				if (commands.Count() != cmds.Count())
				{
					return null;
				}

				if (!(cmds.First().Identifier is Gradient))
				{
					return null;
				}

				var identifers = cmds.Select(_ => _.Identifier).Distinct().ToArray();
				if (identifers.Count() != 1)
				{
					return null;
				}

				var owner = identifers.First() as Gradient;

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

		public unsafe static State CreateDefault()
		{
			var value = new State();
			value.ColorMarkers = new ColorMarker[2];
			value.ColorMarkers[0].Position = 0;
			value.ColorMarkers[0].Intensity = 1;
			value.ColorMarkers[0].ColorR = 1.0f;
			value.ColorMarkers[0].ColorG = 1.0f;
			value.ColorMarkers[0].ColorB = 1.0f;

			value.ColorMarkers[1].Position = 1;
			value.ColorMarkers[1].Intensity = 1;
			value.ColorMarkers[1].ColorR = 1.0f;
			value.ColorMarkers[1].ColorG = 1.0f;
			value.ColorMarkers[1].ColorB = 1.0f;

			value.AlphaMarkers = new AlphaMarker[2];
			value.AlphaMarkers[0].Position = 0.0f;
			value.AlphaMarkers[0].Alpha = 1.0f;
			value.AlphaMarkers[1].Position = 1.0f;
			value.AlphaMarkers[1].Alpha = 1.0f;
			return value;
		}
		public unsafe Gradient()
		{
			_value = CreateDefault();
			DefaultValue = CreateDefault();
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

		public void SetValueDirectly(State value)
		{
			_value = (State)value.Clone();
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