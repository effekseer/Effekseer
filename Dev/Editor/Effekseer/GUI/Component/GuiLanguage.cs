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
    public partial class GuiLanguage : System.Windows.Forms.ComboBox
    { 
        public GuiLanguage()
        {
            InitializeComponent();

            EnableUndo = true;

            var list = new List<Language>();
            var fields = typeof(Language).GetFields();

            foreach (var f in fields)
            {
                if (f.FieldType != typeof(Language)) continue;

                var name = NameAttribute.GetName(f.GetCustomAttributes(false));
                Items.Add(name);
                list.Add((Language)f.GetValue(null));
            }
            enums = list.ToArray();

            Reading = false;
            Writing = false;

            Reading = true;
            Read();
            Reading = false;

            HandleCreated += new EventHandler(GuiLanguage_HandleCreated);
            HandleDestroyed += new EventHandler(GuiLanguage_HandleDestroyed);
            SelectedIndexChanged += new EventHandler(GuiLanguage_SelectedIndexChanged);
            SelectionChangeCommitted += new EventHandler(GuiLanguage_SelectionChangeCommitted);
        }

        Language[] enums = null;
        Data.Value.Enum<Language> binding = null;

        public bool EnableUndo { get; set; }

        public Data.Value.Enum<Language> Binding
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
            var o_ = o as Data.Value.Enum<Language>;
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

        void GuiLanguage_HandleCreated(object sender, EventArgs e)
        {

        }

        void GuiLanguage_HandleDestroyed(object sender, EventArgs e)
        {
            Binding = null;
        }

        void GuiLanguage_SelectedIndexChanged(object sender, EventArgs e)
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

        void GuiLanguage_SelectionChangeCommitted(object sender, EventArgs e)
        {
            if (enums[SelectedIndex] == Language.Japanese)
            {
                MessageBox.Show("次回のEffekseer起動時に適応します。");
            }
            else if (enums[SelectedIndex] == Language.English)
            {
                MessageBox.Show("It adapts to the next startup");
            }
        }
    }
}
