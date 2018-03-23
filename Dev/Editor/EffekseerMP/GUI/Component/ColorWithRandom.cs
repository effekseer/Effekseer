using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class ColorWithRandom : Control, IParameterControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		Data.Value.ColorWithRandom binding = null;

		float[] internalValueMax = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
		float[] internalValueMin = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };

		public bool ShouldBeRemoved { get; private set; } = false;

		/// <summary>
		/// This parameter is unused.
		/// </summary>
		public bool EnableUndo { get; set; } = true;

		public Data.Value.ColorWithRandom Binding
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
					internalValueMax[0] = binding.R.Max / 255.0f;
					internalValueMax[1] = binding.G.Max / 255.0f;
					internalValueMax[2] = binding.B.Max / 255.0f;
					internalValueMax[3] = binding.A.Max / 255.0f;

					internalValueMin[0] = binding.R.Min / 255.0f;
					internalValueMin[1] = binding.G.Min / 255.0f;
					internalValueMin[2] = binding.B.Min / 255.0f;
					internalValueMin[3] = binding.A.Min / 255.0f;

				}
			}
		}

		public ColorWithRandom(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.ColorWithRandom;
			Binding = o_;
		}

		public void FixValue()
		{
		}

		public override void Update()
		{
			if (binding != null)
			{
				internalValueMax[0] = binding.R.Max / 255.0f;
				internalValueMax[1] = binding.G.Max / 255.0f;
				internalValueMax[2] = binding.B.Max / 255.0f;
				internalValueMax[3] = binding.A.Max / 255.0f;

				internalValueMax[0] = binding.R.Min / 255.0f;
				internalValueMax[1] = binding.G.Min / 255.0f;
				internalValueMax[2] = binding.B.Min / 255.0f;
				internalValueMax[3] = binding.A.Min / 255.0f;
			}

			Console.WriteLine("Not implemented. (HSV, gauss, range)");

			if (Manager.NativeManager.ColorEdit4(id, internalValueMax, swig.ColorEditFlags.None))
			{
				binding.SetMax(
					(int)(internalValueMax[0] * 255),
					(int)(internalValueMax[1] * 255),
					(int)(internalValueMax[2] * 255),
					(int)(internalValueMax[3] * 255));
			}

			if (Manager.NativeManager.ColorEdit4(id, internalValueMin, swig.ColorEditFlags.None))
			{
				binding.SetMin(
					(int)(internalValueMin[0] * 255),
					(int)(internalValueMin[1] * 255),
					(int)(internalValueMin[2] * 255),
					(int)(internalValueMin[3] * 255));
			}
		}
	}
}
