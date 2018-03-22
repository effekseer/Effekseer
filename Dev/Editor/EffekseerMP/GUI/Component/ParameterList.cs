using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;

namespace Effekseer.GUI.Component
{
	class ParameterList : IControl, IDroppableControl
	{
		// Not implemented
		// ToolTip toolTip = null;

		object bindingObject = null;
		Type bindingType = null;
		List<TypeRow> typeRows = new List<TypeRow>();
		List<TypeRow> validRows = new List<TypeRow>();
		Dictionary<object, TypeRow> objToTypeRow = new Dictionary<object, TypeRow>();

		internal Utils.DelayedList<IParameterControl> Controls = new Utils.DelayedList<IParameterControl>();

		public ParameterList()
		{
			// Not implemented
			//toolTip = new ToolTip();
			//toolTip.ShowAlways = true;
		}

		public void OnDropped(string path, ref bool handle)
		{
			Controls.Lock();

			foreach (var c in Controls.Internal)
			{
				var dc = c as IDroppableControl;
				if(dc != null)
				{
					dc.OnDropped(path, ref handle);
					if (handle) break;
				}
			}

			Controls.Unlock();
		}

		public void Update()
		{
			Controls.Lock();

			Manager.NativeManager.Columns(2);

			var columnWidth = Manager.NativeManager.GetColumnWidth(0);
			Manager.NativeManager.SetColumnWidth(0, 200);

			foreach (var c in Controls.Internal)
			{
				Manager.NativeManager.PushItemWidth(180);
				c.Update();
				Manager.NativeManager.PopItemWidth();

				Manager.NativeManager.NextColumn();
				Manager.NativeManager.Text(c.Label);
				Manager.NativeManager.NextColumn();
			}

			Manager.NativeManager.Columns(1);
			
			Controls.Unlock();
		}

		public void SetType(Type type)
		{
			typeRows.Clear();
			// Not implemented
			//toolTip.RemoveAll();

			bindingType = type;
			AppendType(type);

			foreach (var row in typeRows)
			{
				if (row.Control == null) continue;

				// Not implemented
				//toolTip.SetToolTip(row.Control, row.Description);
				//toolTip.SetToolTip(row.Label, row.Description);
			}
		}

		public void FixValues()
		{
			Controls.Lock();

			foreach (var c in Controls.Internal)
			{
				c.FixValue();
			}

			Controls.Unlock();
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
				RegisterValue(value);
			}
			else
			{
				ResetValue();
				bindingObject = null;
			}
		}

		void RegisterValue(object value)
		{
			int labelWidth = 0;

			objToTypeRow.Clear();

			var newTypeRows = new List<TypeRow>();
			var newTypeRowsHash = new HashSet<TypeRow>();
			var existsTypeRowsHash = new HashSet<TypeRow>();

			foreach (var row in typeRows)
			{
				if (row.IsShown(value))
				{
					newTypeRows.Add(row);
					newTypeRowsHash.Add(row);
				}
			}

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
				Console.WriteLine("Not implemented.");
				//Controls.Add(row.Label);
			}

			foreach (var row in newTypeRows)
			{
				if (row.Control == null) continue;
				Console.WriteLine("Not implemented.");
				//labelWidth = Math.Max(labelWidth, row.Label.Width);
			}

			int y = 5;
			labelWidth += 5;

			Console.WriteLine("Not implemented.");
			//AutoScrollPosition = new Point(0, 0);
			//AutoScroll = false;

			foreach (var row in newTypeRows)
			{
				if (row.Control == null) continue;

				Console.WriteLine("Not implemented.");
				//int height = Math.Max(row.Label.Height, row.Control.Height) + 4;
				//
				//row.Label.Location = new Point(0, y + (height - row.Label.Height) / 2);
				//row.Control.Location = new Point(labelWidth, y + (height - row.Control.Height) / 2);
				//row.Control.Anchor = AnchorStyles.Left | AnchorStyles.Top;

				if (!existsTypeRowsHash.Contains(row))
				{
					Controls.Add(row.Control);
				}

				var v = row.GetValue(value);
				row.ControlDynamic.SetBinding(v);
				objToTypeRow.Add(v, row);

				Console.WriteLine("Not implemented.");
				//y += height;

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

			Console.WriteLine("Not implemented.");
			//AutoScroll = true;

			bindingObject = value;
		}

		void ResetValue()
		{
			objToTypeRow.Clear();

			foreach (var row in validRows)
			{
				RemoveRow(row, true);
			}

			validRows.Clear();
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
				row.Control.OnDisposed();
				this.Controls.Remove(row.Control);
				Console.WriteLine("Not implemented.");
				//this.Controls.Remove(row.Label);
			}
		}

		void ChangeSelector(object sender, ChangedValueEventArgs e)
		{
			SetValue(bindingObject);

			// 選択位置移動
			TypeRow row = null;
			if (objToTypeRow.TryGetValue(sender, out row))
			{
				Console.WriteLine("Not implemented.");
				//AutoScrollPosition = new Point(0, row.Control.Location.Y);
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

			public IParameterControl Control
			{
				get;
				private set;
			}

			public dynamic ControlDynamic
			{
				get;
				private set;
			}

			public string Label
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
			/// Generate based on property
			/// </summary>
			/// <param name="properties"></param>
			public TypeRow(PropertyInfo[] properties, List<TypeRow> sameLayer, TypeRow parent)
			{
				Parent = parent;

				var p = properties.LastOrDefault();
				var attributes = p.GetCustomAttributes(false);

				Title = NameAttribute.GetName(attributes);
				Description = DescriptionAttribute.GetDescription(attributes);
				
				IParameterControl gui = null;

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
					gui = new String(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.Boolean))
				{
					gui = new Boolean(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.Int))
				{
					gui = new Int(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.IntWithInifinite))
				{
					gui = new IntWithInifinite(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.IntWithRandom))
				{
					gui = new IntWithRandom();
				}
				else if (p.PropertyType == typeof(Data.Value.Float))
				{
					gui = new Float(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.FloatWithRandom))
				{
					gui = new FloatWithRandom(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.Vector2D))
				{
					gui = new Vector2D(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.Vector2DWithRandom))
				{
					gui = new Vector2DWithRandom(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.Vector3D))
				{
					gui = new Vector3D(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.Vector3DWithRandom))
				{
					gui = new Vector3DWithRandom(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.Color))
				{
					gui = new ColorCtrl(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.ColorWithRandom))
				{
					gui = new ColorWithRandom(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.Path))
				{
					Console.WriteLine("Not implemented.");
					//gui = new Path();
				}
				else if (p.PropertyType == typeof(Data.Value.PathForModel))
				{
					Console.WriteLine("Not implemented.");
					//gui = new PathForModel();
				}
				else if (p.PropertyType == typeof(Data.Value.PathForImage))
				{
					Console.WriteLine("Not implemented.");
					//gui = new PathForImage();
				}
				else if (p.PropertyType == typeof(Data.Value.PathForSound))
				{
					gui = new PathForSound(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.FCurveVector2D))
				{
					Console.WriteLine("Not implemented.");
					//FCurveButton button = new FCurveButton();
					//gui = button;
				}
				else if (p.PropertyType == typeof(Data.Value.FCurveVector3D))
				{
					Console.WriteLine("Not implemented.");
					//FCurveButton button = new FCurveButton();
					//gui = button;
				}
				else if (p.PropertyType == typeof(Data.Value.FCurveColorRGBA))
				{
					Console.WriteLine("Not implemented.");
					//FCurveButton button = new FCurveButton();
					//gui = button;
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
				else if (p.PropertyType.IsGenericType)
				{
					var types = p.PropertyType.GetGenericArguments();
					gui = new Enum(Title);
					
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

				Label = Title;

				Console.WriteLine("Not implemented.");
				//Label = new Label();
				//Label.Width = 0;
				//Label.AutoSize = true;
				//Label.Text = Title;
				//Label.TextAlign = ContentAlignment.MiddleCenter;
				//Label.Font = new Font(Label.Font.FontFamily, 9);
				
				ControlDynamic = Control;

				if (Control != null)
				{
					Control.EnableUndo = EnableUndo;
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

