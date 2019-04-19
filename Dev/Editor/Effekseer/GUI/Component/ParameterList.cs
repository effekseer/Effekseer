using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;

namespace Effekseer.GUI.Component
{
	class ParameterList : GroupControl, IControl, IDroppableControl
	{
		object bindingObject = null;
		Type bindingType = null;
		List<TypeRow> typeRows = new List<TypeRow>();
		List<TypeRow> validRows = new List<TypeRow>();

		Utils.DelayedList<TypeRow> controlRows = new Utils.DelayedList<TypeRow>();

		Dictionary<object, TypeRow> objToTypeRow = new Dictionary<object, TypeRow>();

		bool isControlsChanged = false;

		public ParameterList()
		{
		}

		public override void Update()
		{
			if(isControlsChanged)
			{
				SetValue(bindingObject);
				isControlsChanged = false;
			}

			controlRows.Lock();

			Manager.NativeManager.Columns(2);

			var columnWidth = Manager.NativeManager.GetColumnWidth(0);
			Manager.NativeManager.SetColumnWidth(0, 120);

			for(int i = 0; i < controlRows.Internal.Count; i++)
			{
				var c = controlRows.Internal[i].Control as IParameterControl;
				
				if(i > 0 && 
					(controlRows[i - 1].SelectorIndent > controlRows[i].SelectorIndent ||
					controlRows[i].IsSelector ||
					(controlRows[i - 1].SelectorIndent == controlRows[i].SelectorIndent && controlRows[i - 1].IsSelector)))
				{
					Manager.NativeManager.Separator();
				}
				//Manager.NativeManager.PushItemWidth(100);
				
				Manager.NativeManager.SetCursorPosY(Manager.NativeManager.GetCursorPosY() + Manager.TextOffsetY);
				Manager.NativeManager.Text(c.Label);

				if (Manager.NativeManager.IsItemHovered())
				{
					//Manager.NativeManager.SetTooltip(c.Description);

					Manager.NativeManager.BeginTooltip();

					Manager.NativeManager.Text(c.Label);
					Manager.NativeManager.Separator();
					Manager.NativeManager.Text(c.Description);

					Manager.NativeManager.EndTooltip();
				}

				//Manager.NativeManager.PopItemWidth();

				Manager.NativeManager.NextColumn();

				Manager.NativeManager.PushItemWidth(-1);
				c.Update();
				Manager.NativeManager.PopItemWidth();

				Manager.NativeManager.NextColumn();
			}

			Manager.NativeManager.Columns(1);

			controlRows.Unlock();

			if(isControlsChanged)
			{
				SetValue(bindingObject);
				isControlsChanged = false;
			}
		}

		public void SetType(Type type)
		{
			typeRows.Clear();

			bindingType = type;
			AppendType(type, 0);
		}

		public void FixValues()
		{
			controlRows.Lock();

			foreach (var c in controlRows.Internal.Select(_=>_.Control).OfType<IParameterControl>())
			{
				c.FixValue();
			}

			controlRows.Unlock();
		}

		void AppendType(Type type, int selectorIndent, PropertyInfo[] props = null, TypeRow parentRow = null)
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
					// visible values
					typeRows.Add(row);
					sameLayer.Add(row);

					if(row.Selector != null)
					{
						row.SelectorIndent = selectorIndent + 1;
					}
					else
					{
						row.SelectorIndent = selectorIndent;
					}
				}
				else
				{
					// value container

					// protect looping
					if (ps[i].PropertyType == typeof(Data.NodeBase)) continue;
					if (ps[i].PropertyType == typeof(Data.NodeBase.ChildrenCollection)) continue;
					if (ps[i].PropertyType == typeof(Data.Value.FCurve<float>)) continue;
					if (ps[i].PropertyType == typeof(Data.Value.FCurve<byte>)) continue;

					typeRows.Add(row);
					sameLayer.Add(row);

					var selectorIndentLocal = selectorIndent;
					if (row.Selector != null)
					{
						selectorIndentLocal = selectorIndentLocal + 1;
					}

					AppendType(ps[i].PropertyType, selectorIndentLocal, list_props.ToArray(), row);
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

			foreach (var row in newTypeRows)
			{
				if (row.Control == null) continue;

				controlRows.Add(row);

				var v = row.GetValue(value);
				row.ControlDynamic.SetBinding(v);
				objToTypeRow.Add(v, row);

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
				if( row.Control is Control)
				{
					var c = row.Control as Control;
					c.DispatchDisposed();
				}
				else
				{
					row.Control.OnDisposed();
				}
			}

			this.controlRows.Remove(row);
		}

		void ChangeSelector(object sender, ChangedValueEventArgs e)
		{
			isControlsChanged = true;
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

			public int SelectorIndent = 0;

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
					gui = new IntWithRandom(Title);
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
					gui = null;
					return;
				}
				else if (p.PropertyType == typeof(Data.Value.PathForModel))
				{
					gui = new PathForModel(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.PathForImage))
				{
					gui = new PathForImage(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.PathForSound))
				{
					gui = new PathForSound(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.FCurveVector2D))
				{
					gui = new FCurveButton(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.FCurveVector3D))
				{
					gui = new FCurveButton(Title);
				}
				else if (p.PropertyType == typeof(Data.Value.FCurveColorRGBA))
				{
					gui = new FCurveButton(Title);
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
				
				if(gui != null)
				{
					gui.Description = this.Description;
				}

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

