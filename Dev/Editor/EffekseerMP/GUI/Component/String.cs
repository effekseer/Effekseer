using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class String : IControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		Data.Value.String binding = null;

		string internalValue = string.Empty;

		public bool ShouldBeRemoved { get; private set; } = false;

		public bool EnableUndo { get; set; } = true;

		public Data.Value.String Binding
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
					internalValue = binding.Value;
				}
			}
		}

		public String(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.String;
			Binding = o_;
		}

		public void Update()
		{
			if (binding != null)
			{
				internalValue = binding.Value;
			}

			if(Manager.NativeManager.InputText(Label + id, internalValue))
			{
				var v = Manager.NativeManager.GetInputTextResult();

				if (EnableUndo)
				{
					binding.SetValue(v);
				}
				else
				{
					throw new Exception();
				}
			}
		}
	}
}
