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
	public partial class String : System.Windows.Forms.TextBox
	{
		public String()
		{
			InitializeComponent();

			EnableUndo = true;

			Reading = false;
			Writing = false;
	
			Reading = true;
			Read();
			Reading = false;

			HandleCreated += new EventHandler(String_HandleCreated);
			HandleDestroyed += new EventHandler(String_HandleDestroyed);
			TextChanged += new EventHandler(String_TextChanged);
			KeyDown += new KeyEventHandler(String_KeyDown);
			Leave += new EventHandler(String_Leave);
		}

		bool changed = false;

		Color edited_color = Colors.EditedString;
		Color editing_color = Colors.EditingString;

		Data.Value.String binding = null;

		public bool EnableUndo { get; set; }

		public Data.Value.String Binding
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
			var o_ = o as Data.Value.String;
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

		void Read()
		{
			if (!Reading) throw new Exception();

			changed = false;
			BackColor = edited_color;

			if (binding != null)
			{
				Text = binding;
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
				Writing = true;
				binding.SetValue(Text);
				Writing = false;
			}
		}

		void OnChanged(object sender, ChangedValueEventArgs e)
		{
			if (Writing) return;

			Reading = true;
			Read();
			Reading = false;
		}

		void String_HandleCreated(object sender, EventArgs e)
		{

		}

		void String_HandleDestroyed(object sender, EventArgs e)
		{
			Binding = null;
		}

		void String_TextChanged(object sender, EventArgs e)
		{
			if (Reading) return;

			changed = true;
			BackColor = editing_color;
		}

		void String_KeyDown(object sender, KeyEventArgs e)
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

		void String_Leave(object sender, EventArgs e)
		{
			Write();

			Reading = true;
			Read();
			Reading = false;
		}
	}
}
