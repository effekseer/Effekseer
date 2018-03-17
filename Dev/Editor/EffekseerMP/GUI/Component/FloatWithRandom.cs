using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class FloatWithRandom : IParameterControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		Data.Value.FloatWithRandom binding = null;

		float[] internalValue = new float[] { 0.0f, 0.0f };

		public bool ShouldBeRemoved { get; private set; } = false;

		public bool EnableUndo { get; set; } = true;

		public Data.Value.FloatWithRandom Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value) return;

				binding = value;

				if (binding != null)
				{
					internalValue[0] = binding.GetMin();
					internalValue[1] = binding.GetMax();
				}
			}
		}

		public FloatWithRandom(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.FloatWithRandom;
			Binding = o_;
		}

		public void FixValue()
		{
		}

		public void OnDisposed()
		{
		}

		public void Update()
		{
			if (binding != null)
			{
				internalValue[0] = binding.GetMin();
				internalValue[1] = binding.GetMax();
			}

			if (Manager.NativeManager.DragFloat2(id, internalValue))
			{
				if (EnableUndo)
				{
					binding.SetMin(internalValue[0]);
					binding.SetMax(internalValue[1]);
				}
				else
				{
					Console.WriteLine("Not implemented.");
				}
			}
		}
	}
}
