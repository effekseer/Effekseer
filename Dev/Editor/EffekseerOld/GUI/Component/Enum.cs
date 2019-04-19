using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.VisualStyles;

namespace Effekseer.GUI.Component
{
	public partial class Enum : System.Windows.Forms.ComboBox
	{
		public Enum()
		{
			InitializeComponent();

			EnableUndo = true;

			Reading = false;
			Writing = false;

			Reading = true;
			Read();
			Reading = false;

			HandleCreated += new EventHandler(Enum_HandleCreated);
			HandleDestroyed += new EventHandler(Enum_HandleDestroyed);
			SelectedIndexChanged += new EventHandler(Enum_SelectedIndexChanged);
		}

		public void Initialize(Type enumType)
		{
			if (isInitialized) return;
			isInitialized = true;

			// GetValuesだと順番が狂うため、GetFieldsで取得
			var list = new List<int>();
			var fields = enumType.GetFields();
			var iconBitmaps = new List<Bitmap>();
			bool hasIcon = false;

			foreach (var f in fields)
			{
				if (f.FieldType != enumType) continue;

				var attributes = f.GetCustomAttributes(false);
				var name = NameAttribute.GetName(attributes);
				if (name == string.Empty)
				{
					name = f.ToString();
				}

				Bitmap icon = null;
				var iconAttribute = IconAttribute.GetIcon(attributes);
				if (iconAttribute != null)
				{
					icon = (Bitmap)Properties.Resources.ResourceManager.GetObject(iconAttribute.resourceName);
					hasIcon = true;
				}

				Items.Add(name);
				list.Add((int)f.GetValue(null));
				iconBitmaps.Add(icon);
			}
			enums = list.ToArray();

			if (hasIcon)
			{
				// アイコンが存在するときはカスタム描画に切り替える
				icons = iconBitmaps.ToArray();
				DrawMode = DrawMode.OwnerDrawFixed;
				DrawItem += new DrawItemEventHandler(Enum_OnDrawItem);
				ItemHeight += icons[0].Height / 2;
			}
		}

		bool isInitialized = false;

		int[] enums = null;
		Bitmap[] icons = null;
		Data.Value.EnumBase binding = null;

		public bool EnableUndo { get; set; }

		public Data.Value.EnumBase Binding
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
			var o_ = o as Data.Value.EnumBase;
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
					if (enums[i] == binding.GetValueAsInt())
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

		void Enum_OnDrawItem(object sender, DrawItemEventArgs e)
		{
			if (e.Index == -1) return;
			
			e.DrawBackground();
			
			var icon = icons[e.Index];
			e.Graphics.DrawImage(icon, e.Bounds.X, e.Bounds.Y);
			
			var font = this.Font;
			using (var brush = new SolidBrush(e.ForeColor))
			{
				e.Graphics.DrawString(Items[e.Index].ToString(), font, brush, 
					e.Bounds.X + icon.Width + 2, 
					e.Bounds.Y + (icon.Height - font.Height) / 2 + 1);
			}

			e.DrawFocusRectangle();
			

		}
	}
}
