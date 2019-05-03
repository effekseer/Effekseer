using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class Dummy : Control, IParameterControl
	{
		public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

		Object binding = null;

		bool[] internalValue = new bool[] { false };

		public bool EnableUndo { get; set; } = true;

		public Object Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value) return;

				binding = value;
			}
		}

		public Dummy(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}
		}

		public void SetBinding(object o)
		{
			Binding = o;
		}

		public void FixValue()
		{
		}

		public override void Update()
		{
		}
	}
}
