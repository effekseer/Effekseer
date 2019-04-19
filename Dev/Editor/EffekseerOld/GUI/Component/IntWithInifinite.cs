using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Effekseer.GUI.Component
{
	public partial class IntWithInifinite : UserControl
	{
		public IntWithInifinite()
		{
			InitializeComponent();

			EnableUndo = true;

			this.SuspendLayout();
			Anchor = AnchorStyles.Left | AnchorStyles.Right;
			this.ResumeLayout(false);

			Func<bool> get_enable = () =>
				{
					if (binding != null)
					{
						return !binding.Infinite;
					}
					return false;
				};

			tb_int.IsEnable += get_enable;
			HandleDestroyed += new EventHandler(IntWithInifinite_HandleDestroyed);
		}

		Data.Value.IntWithInifinite binding = null;

		public bool EnableUndo { get; set; }

		public Data.Value.IntWithInifinite Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value) return;

				if (binding != null)
				{
					binding.Infinite.OnChanged -= OnChanged;
					tb_int.Binding = null;
					cb_infinite.Binding = null;
				}

				binding = value;

				if (binding != null)
				{
					binding.Infinite.OnChanged += OnChanged;
					tb_int.Binding = binding.Value;
					cb_infinite.Binding = binding.Infinite;
				}
			}
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.IntWithInifinite;
			Binding = o_;
		}

		void OnChanged(object sender, ChangedValueEventArgs e)
		{
			tb_int.Reload();
		}

		void IntWithInifinite_HandleDestroyed(object sender, EventArgs e)
		{
			Binding = null;
		}
	}
}
