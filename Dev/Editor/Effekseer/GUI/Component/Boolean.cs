using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
    class Boolean : Control, IParameterControl
    {
        string id = "";

        public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

		Data.Value.Boolean binding = null;

		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		bool[] internalValue = new bool[] { false };

		public bool EnableUndo { get; set; } = true;

        public Data.Value.Boolean Binding
        {
            get
            {
                return binding;
            }
            set
            {
                if (binding == value) return;

				binding = value;

                if(binding != null)
                {
                    internalValue[0] = binding.Value;
                }
            }
        }

        public Boolean(string label = null)
        {
			if(label != null)
			{
				Label = label;
			}

			id = "###" + Manager.GetUniqueID().ToString();
		}

        public void SetBinding(object o)
        {
            var o_ = o as Data.Value.Boolean;
            Binding = o_;
        }

		public void FixValue()
		{
		}

		public override void Update()
		{
			if (binding != null)
			{
				internalValue[0] = binding.Value;
			}

			valueChangingProp.Enable(binding);

			if (Manager.NativeManager.Checkbox(id, internalValue))
			{
				if (EnableUndo)
				{
					binding.SetValue(internalValue[0]);
				}
				else
				{
					binding.SetValueDirectly(internalValue[0]);
				}
			}

			valueChangingProp.Disable();
		}
    }
}
