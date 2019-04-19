using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Effekseer.GUI.Component
{
	public partial class DelegateStringTextBox : System.Windows.Forms.TextBox
	{
		public DelegateStringTextBox()
		{
			Reading = false;
			Writing = false;

			Reading = true;
			Read();
			Reading = false;

			HandleCreated += new EventHandler(_HandleCreated);
			HandleDestroyed += new EventHandler(_HandleDestroyed);
			TextChanged += new EventHandler(_TextChanged);
			KeyDown += new KeyEventHandler(_KeyDown);
			Leave += new EventHandler(_Leave);
			Enter += new EventHandler(_Enter);

			Changed = false;
			WheelStep = 1;
		}

		public bool Changed
		{
			get;
			private set;
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

		/// <summary>
		/// 読み込み関数
		/// </summary>
		public Func<string> ReadMethod
		{
			get;
			set;
		}

		/// <summary>
		/// 書き込み関数
		/// </summary>
		public Action<string,bool> WriteMethod
		{
			get;
			set;
		}
		
		/// <summary>
		/// ホイール回転単位の変化量
		/// </summary>
		public int WheelStep
		{
			get;
			set;
		}

		void Read()
		{
			if (!Reading) throw new Exception();

			Changed = false;
			BackColor = Colors.EditedString;

			bool enable = ReadMethod != null;
			if (IsEnable != null && enable)
			{
				enable = IsEnable();
			}

			if (enable)
			{
				Text = ReadMethod();
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
			if (WriteMethod != null && Changed)
			{
				Writing = true;
				WriteMethod(Text, false);
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

		void _HandleCreated(object sender, EventArgs e)
		{

		}

		void _HandleDestroyed(object sender, EventArgs e)
		{

		}

		void _TextChanged(object sender, EventArgs e)
		{
			if (Reading) return;

			Changed = true;
			BackColor = Colors.EditingString;
		}

		void _KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter && Changed)
			{
				// ビープ音防止
				e.SuppressKeyPress = true;

				Write();

				Reading = true;
				Read();
				Reading = false;
			}
		}
		
		void _Enter(object sender, EventArgs e)
		{
			this.BeginInvoke(new MethodInvoker(() => this.SelectAll()));
		}

		void _Leave(object sender, EventArgs e)
		{
			Write();

			Reading = true;
			Read();
			Reading = false;
		}
	}
}
