using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class Vector2D : IParameterControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		Data.Value.Vector2D binding = null;

		float[] internalValue = new float[] { 0.0f, 0.0f };

		public bool EnableUndo { get; set; } = true;

		public Data.Value.Vector2D Binding
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
					internalValue[0] = binding.X.Value;
					internalValue[1] = binding.Y.Value;
				}
			}
		}

		public Vector2D(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Vector2D;
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
				internalValue[0] = binding.X.Value;
				internalValue[1] = binding.Y.Value;
			}

			if (Manager.NativeManager.DragFloat2(id, internalValue))
			{
				if (EnableUndo)
				{
					binding.X.SetValue(internalValue[0]);
					binding.Y.SetValue(internalValue[1]);
				}
				else
				{
					binding.X.SetValueDirectly(internalValue[0]);
					binding.Y.SetValueDirectly(internalValue[1]);
				}
			}
		}
	}
}
