using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.BindableComponent
{
	class Int2 : Control, IParameterControl
	{
		string id = "";
		string id_c = "";
		string id_reset = "";

		Data.Value.Int2 binding = null;

		bool isPopupShown = false;

		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		bool isActive = false;

		int[] internalValue = new int[] { 0, 0 };

		public bool EnableUndo { get; set; } = true;

		public Data.Value.Int2 Binding
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

		public Int2()
		{
			id = "###" + Manager.GetUniqueID().ToString();
			id_c = "###" + Manager.GetUniqueID().ToString();
			id_reset = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Int2;
			Binding = o_;
		}

		public void FixValue()
		{
			FixValueInternal(false);
		}

		void FixValueInternal(bool combined)
		{
			if (binding == null) return;

			if (EnableUndo)
			{
				binding.X.SetValue(internalValue[0], combined);
				binding.Y.SetValue(internalValue[1], combined);
			}
			else
			{
				binding.X.SetValueDirectly(internalValue[0]);
				binding.Y.SetValueDirectly(internalValue[1]);
			}
		}

		public override void OnDisposed()
		{
			FixValueInternal(false);
		}

		public override void Update()
		{
			isPopupShown = false;
			if (binding == null) return;

			valueChangingProp.Enable(binding);

			int step = 1;
			if (binding != null)
			{
				internalValue[0] = binding.X.Value;
				internalValue[1] = binding.Y.Value;
				step = binding.X.Step;
			}

			if (Manager.NativeManager.DragInt2EfkEx(id, internalValue, step,
				int.MinValue, int.MaxValue,
				int.MinValue, int.MaxValue,
				"X:%d", "Y:%d"))
			{
				FixValueInternal(isActive);
			}

			Popup();

			var isActive_Current = Manager.NativeManager.IsItemActive();

			if (isActive && !isActive_Current)
			{
				FixValue();
			}

			isActive = isActive_Current;

			valueChangingProp.Disable();
		}

		void Popup()
		{
			if (isPopupShown) return;

			if (Manager.NativeManager.BeginPopupContextItem(id_c))
			{
				Functions.ShowReset(binding, id_reset);

				Manager.NativeManager.EndPopup();

				isPopupShown = true;
			}
		}
	}
}