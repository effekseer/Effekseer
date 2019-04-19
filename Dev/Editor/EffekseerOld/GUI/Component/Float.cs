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
	partial class Float : System.Windows.Forms.TextBox
	{
		public Float()
		{
			InitializeComponent();

			EnableUndo = true;

			Reading = false;
			Writing = false;
	
			Reading = true;
			Read();
			Reading = false;

			HandleCreated += new EventHandler(Float_HandleCreated);
			HandleDestroyed += new EventHandler(Float_HandleDestroyed);
			TextChanged += new EventHandler(Float_TextChanged);
			KeyDown += new KeyEventHandler(Float_KeyDown);
			Enter += new EventHandler(Float_Enter);
			Leave += new EventHandler(Float_Leave);
		}

		bool changed = false;

		Data.Value.Float binding = null;

		public bool EnableUndo { get; set; }

		public Data.Value.Float Binding
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
					binding.OnChanged -= OnChanged;
				}

				binding = value;

				if (binding != null)
				{
					binding.OnChanged += OnChanged;
				}

				Reading = true;
				Read();
				Reading = false;
			}
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Float;
			Binding = o_;
		}

		/// <summary>
		/// 他のクラスからデータ読み込み中
		/// </summary>
		public bool Reading
		{
			get;
			private set;
		}

		/// <summary>
		/// 他のクラスにデータ書き込み中
		/// </summary>
		public bool Writing
		{
			get;
			private set;
		}

		/// <summary>
		/// 再読込
		/// </summary>
		public void Reload()
		{
			if (Writing) return;

			Reading = true;
			Read();
			Reading = false;
		}

		/// <summary>
		/// 外部からの有効指定
		/// </summary>
		public Func<bool> IsEnable
		{
			get;
			set;
		}

		

		void Read()
		{
			if (!Reading) throw new Exception();

			changed = false;
			BackColor = Colors.EditedFloat;

			bool enable = binding != null;
			if (IsEnable != null && enable)
			{
				enable = IsEnable();
			}

			if (enable)
			{
				Text = binding.Value.ToString();
				Enabled = true;
			}
			else
			{
				Text = string.Empty;
				Enabled = false;
				BackColor = Colors.Disable;
			}
		}

		void Write()
		{
			if (binding != null && changed)
			{
				float parsed = 0;
				if (float.TryParse(Text, System.Globalization.NumberStyles.Float, Setting.NFI, out parsed))
				{
					Writing = true;

					if (EnableUndo)
					{
						binding.SetValue(parsed);
					}
					else
					{
						binding.SetValueDirectly(parsed);
					}

					Writing = false;
				}
			}
		}

		void OnChanged(object sender, ChangedValueEventArgs e)
		{
			if (Writing) return;

			Reading = true;
			Read();
			Reading = false;
		}

		void Float_HandleCreated(object sender, EventArgs e)
		{

		}

		void Float_HandleDestroyed(object sender, EventArgs e)
		{
			Binding = null;
		}

		void Float_TextChanged(object sender, EventArgs e)
		{
			if (Reading) return;

			changed = true;
			BackColor = Colors.EditingFloat;
		}

		void Float_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter && changed)
			{
				// ビープ音防止
				e.SuppressKeyPress = true;

				Write();

				Reading = true;
				Read();
				Reading = false;
			}
		}
		
		void Float_Enter(object sender, EventArgs e)
		{
			this.BeginInvoke(new MethodInvoker(() => this.SelectAll()));
		}

		void Float_Leave(object sender, EventArgs e)
		{
			Write();

			Reading = true;
			Read();
			Reading = false;
		}
		
		void _MouseWheel(int delta)
		{
			int wheel = (delta / 120);

			if (EnableUndo)
			{
				Binding.SetValue(Binding.Value + wheel * Binding.Step, true);
			}
			else
			{
				Binding.SetValueDirectly(Binding.Value + wheel * Binding.Step);
			}
			Reload();
			SelectAll();
			changed = true;
		}

		protected override void WndProc(ref Message m)
		{
			if (m.Msg == 0x020A)
			{
				var pos = this.PointToClient(Cursor.Position);
				if (0 <= pos.X &&
					pos.X <= Size.Width &&
					0 <= pos.Y &&
					pos.Y <= Size.Height)
				{
					if ((int)m.WParam > 0)
					{
						_MouseWheel(120);
					}
					else if ((int)m.WParam < 0)
					{
						_MouseWheel(-120);
					}
				}
				else
				{
					base.WndProc(ref m);
				}
			}
			else
			{
				base.WndProc(ref m);
			}
		}
	}
}
