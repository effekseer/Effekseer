using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Reflection;

namespace Effekseer.GUI.Component
{
	public partial class LayoutPanel : System.Windows.Forms.Panel
	{
		public LayoutPanel()
		{
			InitializeComponent();

			toolTip = new ToolTip();
			toolTip.ShowAlways = true;
		}

		ToolTip toolTip = null;

		object bindingObject = null;
		Type bindingType = null;
		List<TypeRow> typeRows = new List<TypeRow>();
		List<TypeRow> validRows = new List<TypeRow>();
		Dictionary<object, TypeRow> objToTypeRow = new Dictionary<object, TypeRow>();

		public void SetType(Type type)
		{
			typeRows.Clear();
			toolTip.RemoveAll();

			bindingType = type;
			AppendType(type);

			foreach (var row in typeRows)
			{
				if (row.Control == null) continue;
				toolTip.SetToolTip(row.Control, row.Description);
				toolTip.SetToolTip(row.Label, row.Description);
			}
		}

		void AppendType(Type type, PropertyInfo[] props = null, TypeRow parentRow = null)
		{
			if (props == null)
			{
				props = new PropertyInfo[0];
			}

			var ps = type.GetProperties(BindingFlags.Public | BindingFlags.Instance | BindingFlags.DeclaredOnly);

			List<TypeRow> sameLayer = new List<TypeRow>();

			for (int i = 0; i < ps.Length; i++)
			{
				var attributes = ps[i].GetCustomAttributes(false);

				var list_props = new List<PropertyInfo>(props);
				list_props.Add(ps[i]);

				var row = new TypeRow(list_props.ToArray(), sameLayer, parentRow);

				if (row.Control != null)
				{
					// 表示可能な値
					typeRows.Add(row);
					sameLayer.Add(row);
				}
				else
				{
					// 値コンテナ

					// ループ防止
					if (ps[i].PropertyType == typeof(Data.NodeBase)) continue;
					if (ps[i].PropertyType == typeof(Data.NodeBase.ChildrenCollection)) continue;
					if (ps[i].PropertyType == typeof(Data.Value.FCurve<float>)) continue;
					if (ps[i].PropertyType == typeof(Data.Value.FCurve<byte>)) continue;

					typeRows.Add(row);
					sameLayer.Add(row);
					AppendType(ps[i].PropertyType, list_props.ToArray(), row);
				}
			}
		}

		public void SetValue(object value)
		{
			if (value != null)
			{
				RegistValue(value);
			}
			else
			{
				ResetValue();
				bindingObject = null;
			}
		}

		void RegistValue(object value)
		{
			int labelWidth = 0;

			objToTypeRow.Clear();

			var newTypeRows = new List<TypeRow>();
			var newTypeRowsHash = new HashSet<TypeRow>();
			var existsTypeRowsHash = new HashSet<TypeRow>();

			foreach (var row in typeRows)
			{
				if(row.IsShown(value))
				{
					newTypeRows.Add(row);
					newTypeRowsHash.Add(row);
				}
			}

			SuspendLayout();
			Parent.SuspendLayout();

			foreach (var row in validRows)
			{
				if (newTypeRowsHash.Contains(row))
				{
					existsTypeRowsHash.Add(row);
					RemoveRow(row, false);
				}
				else
				{
					RemoveRow(row, true);
				}
			}

			// 長さを求めるために追加
			foreach (var row in newTypeRows)
			{
				if (existsTypeRowsHash.Contains(row)) continue;
				if (row.Control == null) continue;
				Controls.Add(row.Label);
			}
			Parent.ResumeLayout(false);
			ResumeLayout(false);


			foreach (var row in newTypeRows)
			{
				if (row.Control == null) continue;
				labelWidth = Math.Max(labelWidth, row.Label.Width);
			}

			int y = 5;
			labelWidth += 5;

			SuspendLayout();
			Parent.SuspendLayout();
			AutoScrollPosition = new Point(0, 0);
			AutoScroll = false;

			foreach (var row in newTypeRows)
			{
				if (row.Control == null) continue;

				int height = Math.Max(row.Label.Height, row.Control.Height) + 4;

				row.Label.Location = new Point(0, y + (height - row.Label.Height) / 2);
				row.Control.Location = new Point(labelWidth, y + (height - row.Control.Height) / 2);
				row.Control.Anchor = AnchorStyles.Left | AnchorStyles.Top;

				if (!existsTypeRowsHash.Contains(row))
				{
					Controls.Add(row.Control);
				}

				var v = row.GetValue(value);
				row.ControlDynamic.SetBinding(v);
				objToTypeRow.Add(v, row);

				y += height;

				{
					var o0 = row.GetValue(value) as Data.Value.EnumBase;
					var o1 = row.GetValue(value) as Data.Value.PathForImage;
					if (o0 != null && row.IsSelector)
					{
						o0.OnChanged += ChangeSelector;
					}
					else if (o1 != null)
					{
						o1.OnChanged += ChangeSelector;
					}
				}
			}

			validRows.Clear();
			validRows.AddRange(newTypeRows);

			Parent.ResumeLayout(false);
			ResumeLayout(false);
			AutoScroll = true;

			bindingObject = value;
		}

		void ResetValue()
		{
			objToTypeRow.Clear();

			SuspendLayout();
			Parent.SuspendLayout();

			foreach (var row in validRows)
			{
				RemoveRow(row, true);
			}

			validRows.Clear();

			Parent.ResumeLayout(false);
			ResumeLayout(false);
		}

		void RemoveRow(TypeRow row, bool removeControls)
		{
			if (row.Control == null) return;

			row.ControlDynamic.SetBinding(null);

			if (bindingObject != null)
			{
				var o0 = row.GetValue(bindingObject) as Data.Value.EnumBase;
				var o1 = row.GetValue(bindingObject) as Data.Value.PathForImage;
				if (o0 != null && row.IsSelector)
				{
					o0.OnChanged -= ChangeSelector;
				}
				else if (o1 != null)
				{
					o1.OnChanged -= ChangeSelector;
				}
			}

			if (removeControls)
			{
				this.Controls.Remove(row.Control);
				this.Controls.Remove(row.Label);
			}
		}

		void ChangeSelector(object sender, ChangedValueEventArgs e)
		{
			SetValue(bindingObject);

			// 選択位置移動
			TypeRow row = null;
			if (objToTypeRow.TryGetValue(sender, out row))
			{
				AutoScrollPosition = new Point(0, row.Control.Location.Y);
			}
		}

		class TypeRow
		{ 
			PropertyInfo[] Properties
			{
				get;
				set;
			}

			public string Title
			{
				get;
				private set;
			}

			public string Description
			{
				get;
				private set;
			}

			public bool EnableUndo
			{
				get;
				private set;
			}

			public Control Control
			{
				get;
				private set;
			}

			public dynamic ControlDynamic
			{
				get;
				private set;
			}

			public Label Label
			{
				get;
				private set;
			}

			public bool IsSelector
			{
				get;
				private set;
			}

			public int SelectorID
			{
				get;
				private set;
			}

			/// <summary>
			/// このクラスの表示非表示を決めるセレクタ
			/// </summary>
			public TypeRow Selector
			{
				get;
				private set;
			}

			/// <summary>
			/// Selectorに要求するID
			/// </summary>
			public int SelectorValue
			{
				get;
				private set;
			}

			public TypeRow Parent
			{
				get;
				private set;
			}

			/// <summary>
			/// プロパティから生成
			/// </summary>
			/// <param name="properties"></param>
			public TypeRow(PropertyInfo[] properties, List<TypeRow> sameLayer, TypeRow parent)
			{
				Parent = parent;

				var p = properties.LastOrDefault();
				var attributes = p.GetCustomAttributes(false);

				Control gui = null;

				var undo = attributes.Where(_ => _.GetType() == typeof(Effekseer.Data.UndoAttribute)).FirstOrDefault() as Effekseer.Data.UndoAttribute;
				if (undo != null && !undo.Undo)
				{
					EnableUndo = false;
				}
				else
				{
					EnableUndo = true;
				}

				var shown = attributes.Where(_ => _.GetType() == typeof(Effekseer.Data.ShownAttribute)).FirstOrDefault() as Effekseer.Data.ShownAttribute;

				if (shown != null && !shown.Shown)
				{
					gui = null;
					return;
				}
				else if (p.PropertyType == typeof(Data.Value.String))
				{
					gui = new String();
				}
				else if (p.PropertyType == typeof(Data.Value.Boolean))
				{
					gui = new Boolean();
				}
				else if (p.PropertyType == typeof(Data.Value.Int))
				{
					gui = new Int();
				}
				else if (p.PropertyType == typeof(Data.Value.IntWithInifinite))
				{
					gui = new IntWithInifinite();
				}
				else if (p.PropertyType == typeof(Data.Value.IntWithRandom))
				{
					gui = new IntWithRandom();
				}
				else if (p.PropertyType == typeof(Data.Value.Float))
				{
					gui = new Float();
				}
				else if (p.PropertyType == typeof(Data.Value.FloatWithRandom))
				{
					gui = new FloatWithRandom();
				}
				else if (p.PropertyType == typeof(Data.Value.Vector2D))
				{
					gui = new Vector2D();
				}
				else if (p.PropertyType == typeof(Data.Value.Vector2DWithRandom))
				{
					gui = new Vector2DWithRandom();
				}
				else if (p.PropertyType == typeof(Data.Value.Vector3D))
				{
					gui = new Vector3D();
				}
				else if (p.PropertyType == typeof(Data.Value.Vector3DWithRandom))
				{
					gui = new Vector3DWithRandom();
				}
				else if (p.PropertyType == typeof(Data.Value.Color))
				{
					gui = new ColorCtrl();
				}
				else if (p.PropertyType == typeof(Data.Value.ColorWithRandom))
				{
					gui = new ColorWithRandom();
				}
				else if (p.PropertyType == typeof(Data.Value.Path))
				{
					gui = new Path();
				}
				else if (p.PropertyType == typeof(Data.Value.PathForModel))
				{
					gui = new PathForModel();
				}
				else if (p.PropertyType == typeof(Data.Value.PathForImage))
				{
					gui = new PathForImage();
				}
				else if (p.PropertyType == typeof(Data.Value.PathForSound))
				{
					gui = new PathForSound();
				}
				else if (p.PropertyType == typeof(Data.Value.FCurveVector2D))
				{
					FCurveButton button = new FCurveButton();
					gui = button;
				}
				else if (p.PropertyType == typeof(Data.Value.FCurveVector3D))
				{
					FCurveButton button = new FCurveButton();
					gui = button;
				}
				else if (p.PropertyType == typeof(Data.Value.FCurveColorRGBA))
				{
					FCurveButton button = new FCurveButton();
					gui = button;
				}
				else if (p.PropertyType == typeof(Data.Value.FCurve<float>))
				{
					gui = null;
					return;
				}
				else if (p.PropertyType == typeof(Data.Value.FCurve<byte>))
				{
					gui = null;
					return;
				}
				else if (p.PropertyType == typeof(Data.Value.IFCurveKey))
				{
					gui = null;
					return;
				}
                else if (p.PropertyType == typeof(Data.Value.Enum<Language>))
                {
                    gui = new GuiLanguage();
                }
				else if (p.PropertyType.IsGenericType)
				{
					var types = p.PropertyType.GetGenericArguments();
					gui = new Enum();
				
					var dgui = (dynamic)gui;
					dgui.Initialize(types[0]);
				}

				var selector_attribute = (from a in attributes where a is Data.SelectorAttribute select a).FirstOrDefault() as Data.SelectorAttribute;
				if (selector_attribute != null)
				{
					IsSelector = true;
					SelectorID = selector_attribute.ID;
				}
				else
				{
					IsSelector = false;
					SelectorID = -1;
				}

				Properties = properties.Clone() as PropertyInfo[];
				Title = NameAttribute.GetName(attributes);
				Description = DescriptionAttribute.GetDescription(attributes);
				Control = gui;

				// Selector
				var selected_attribute = (from a in attributes where a is Data.SelectedAttribute select a).FirstOrDefault() as Data.SelectedAttribute;
				if (selected_attribute != null)
				{
					var selector = sameLayer.Where(_ => _.IsSelector && _.SelectorID == selected_attribute.ID).LastOrDefault();

					if (selector != null)
					{
						Selector = selector;
						SelectorValue = selected_attribute.Value;
					}
				}

				Label = new Label();
				Label.Width = 0;
				Label.AutoSize = true;
				Label.Text = Title;
				Label.TextAlign = ContentAlignment.MiddleCenter;
				Label.Font = new Font(Label.Font.FontFamily, 9);
				ControlDynamic = Control;

				if (Control != null && !(Control is Button))
				{
					ControlDynamic.EnableUndo = EnableUndo;
				}
			}

			public object GetValue(object obj)
			{
				var value = obj;
				foreach (var p in Properties)
				{
					value = p.GetValue(value, null);
				}

				return value;
			}

			public bool IsShown(object obj)
			{
				if (Parent != null && !Parent.IsShown(obj)) return false;
				if (Selector == null) return true;
				if (!Selector.IsShown(obj)) return false;

				var value = Selector.GetValue(obj) as Data.Value.EnumBase;

				if (value == null) return false;

				return SelectorValue == value.GetValueAsInt();
			}
		}
	}
}
