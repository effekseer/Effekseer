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
	public partial class Vector2D : UserControl
	{
		public Vector2D()
		{
			InitializeComponent();

			EnableUndo = true;

			this.SuspendLayout();
			Anchor = AnchorStyles.Left | AnchorStyles.Right;
			this.ResumeLayout(false);

			HandleDestroyed += new EventHandler(Vector2D_HandleDestroyed);
		}

		Data.Value.Vector2D binding = null;

		public bool EnableUndo { get; set; }

		public Data.Value.Vector2D Binding
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
					tb_x.Binding = null;
					tb_y.Binding = null;
				}

				binding = value;

				if (binding != null)
				{
					tb_x.Binding = binding.X;
					tb_y.Binding = binding.Y;
				}
			}
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Vector2D;
			Binding = o_;
		}

		void Vector2D_HandleDestroyed(object sender, EventArgs e)
		{
			Binding = null;
		}
	}
}
