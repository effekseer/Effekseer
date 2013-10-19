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
	public partial class Enum<T> : System.Windows.Forms.ComboBox where T : struct, IComparable, IFormattable, IConvertible
	{
		public Enum()
		{
			InitializeComponent();

			EnableUndo = true;
			
			// GetValuesだと順番が狂うため、GetFieldsで取得
			var list = new List<T>();
			var fields = typeof(T).GetFields();

			foreach (var f in fields)
			{
				if (f.FieldType != typeof(T)) continue;

				var name = NameAttribute.GetName(f.GetCustomAttributes(false));
				if (name == string.Empty)
				{
					name = f.ToString();
				}
				Items.Add(name);
				list.Add((T)f.GetValue(null));
			}
			enums = list.ToArray();

			Reading = false;
			Writing = false;

			Reading = true;
			Read();
			Reading = false;

			HandleCreated += new EventHandler(Enum_HandleCreated);
			HandleDestroyed += new EventHandler(Enum_HandleDestroyed);
			SelectedIndexChanged += new EventHandler(Enum_SelectedIndexChanged);
		}

		T[] enums = null;
		Data.Value.Enum<T> binding = null;

		public bool EnableUndo { get; set; }

		public Data.Value.Enum<T> Binding
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
			var o_ = o as Data.Value.Enum<T>;
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
				for (int i = 0; i < enums.Length; i++)
				{
					if (enums[i].ToString() == binding.GetValue().ToString())
					{
						SelectedIndex = i;
						break;
					}
				}

				Enabled = true;
			}
			else
			{
				SelectedIndex = -1;
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

		void Enum_HandleCreated(object sender, EventArgs e)
		{
		
		}

		void Enum_HandleDestroyed(object sender, EventArgs e)
		{
			Binding = null;
		}

		void Enum_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (binding != null)
			{
				Writing = true;
				if (SelectedIndex >= 0)
				{
					if (EnableUndo)
					{
						binding.SetValue(enums[SelectedIndex]);
					}
					else
					{
						binding.SetValueDirectly(enums[SelectedIndex]);
					}
				}
				Writing = false;
			}
		}
	}
}
