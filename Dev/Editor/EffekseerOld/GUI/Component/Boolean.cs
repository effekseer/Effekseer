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
	public partial class Boolean : System.Windows.Forms.CheckBox
	{
		public Boolean()
		{
			InitializeComponent();

			EnableUndo = true;

			Reading = false;
			Writing = false;

			Reading = true;
			Read();
			Reading = false;

			HandleCreated += new EventHandler(Boolean_HandleCreated);
			HandleDestroyed += new EventHandler(Boolean_HandleDestroyed);
			CheckedChanged += new EventHandler(Boolean_CheckedChanged);
		}

		Data.Value.Boolean binding = null;

		public bool EnableUndo { get; set; }

		public Data.Value.Boolean Binding
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
			var o_ = o as Data.Value.Boolean;
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

			if (binding != null)
			{
				Checked = binding;
				Enabled = true;
			}
			else
			{
				Checked = false;
				Enabled = false;
			}
		}

		void OnChanged(object sender, ChangedValueEventArgs e)
		{
			if (Writing) return;

			Reading = true;
			Read();
			Reading = false;
		}


		void Boolean_HandleCreated(object sender, EventArgs e)
		{
			
		}

		void Boolean_HandleDestroyed(object sender, EventArgs e)
		{
			Binding = null;
		}

		void Boolean_CheckedChanged(object sender, EventArgs e)
		{
			if (binding != null)
			{
				Writing = true;

				if (EnableUndo)
				{
					binding.SetValue(Checked);
				}
				else
				{
					binding.SetValueDirectly(Checked);
				}

				Writing = false;
			}
		}
	}
}
