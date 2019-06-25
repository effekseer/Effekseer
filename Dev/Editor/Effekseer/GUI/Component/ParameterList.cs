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

		Utils.DelayedList<TypeRow> controlRows = new Utils.DelayedList<TypeRow>();

		Dictionary<object, TypeRow> objToTypeRow = new Dictionary<object, TypeRow>();

		bool isControlsChanged = false;

		public ParameterList()
		{
		}

		public override void Update()
		{
			if (isControlsChanged)
			{
				SetValue(bindingObject);
				isControlsChanged = false;
			}

			controlRows.Lock();

			Manager.NativeManager.Columns(2);

			var columnWidth = Manager.NativeManager.GetColumnWidth(0);
			Manager.NativeManager.SetColumnWidth(0, 120);

			for (int i = 0; i < controlRows.Internal.Count; i++)
			{
				var c = controlRows.Internal[i].Control as IParameterControl;

				if (c is Dummy) continue;

				if (i > 0 &&
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

			if (isControlsChanged)
			{
				SetValue(bindingObject);
				isControlsChanged = false;
			}
		}

		public void SetType(Type type)
		{
			// Ignore
			/*
			typeRows.Clear();

			bindingType = type;
			AppendType(type, 0);
			*/
		}

		public void FixValues()
		{
			controlRows.Lock();

			foreach (var c in controlRows.Internal.Select(_ => _.Control).OfType<IParameterControl>())
			{
				c.FixValue();
			}

			controlRows.Unlock();
		}

		/*
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

					if (row.Selector != null)
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
		*/

		public void SetValue(object value)
		{
			if (value != null)
			{
				RegisterValue(value);
			}
			else
			{
				ResetValue();

				if (bindingObject is Data.IEditableValueCollection)
				{
					var o2 = bindingObject as Data.IEditableValueCollection;
					o2.OnChanged -= ChangeSelector;
				}

				bindingObject = null;
			}
		}

		void RegisterValue(object value)
		{
			List<TypeRow> newRows = new List<TypeRow>();

			Action<object,int> parseObject = null;

			parseObject = (objValue, indent) =>
				{
					Data.EditableValue[] editableValues = null;

					if(objValue is Data.IEditableValueCollection)
					{
						editableValues = (objValue as Data.IEditableValueCollection).GetValues();
					}
					else
					{
						var props = objValue.GetType().GetProperties(BindingFlags.Public | BindingFlags.Instance | BindingFlags.DeclaredOnly);
						editableValues = props.Select(_ => Data.EditableValue.Create(_.GetValue(objValue), _)).ToArray();
					}

					List<TypeRow> localRows = new List<TypeRow>();

					foreach (var editableValue in editableValues)
					{
						var propValue = editableValue.Value;
						var prop = editableValue;

						TypeRow row = null;

						if (!editableValue.IsShown) continue;

						if (objToTypeRow.ContainsKey(propValue))
						{
							row = objToTypeRow[propValue];
							row.SetSelector(localRows);
							row.SelectorIndent = indent;
							if (row.Selector != null) row.SelectorIndent++;
#if DEBUG
							if(row.BindingValue != propValue) throw new Exception();
#endif

							if (!row.IsShown()) continue;
						}
						else
						{
							row = new TypeRow(prop);
							row.SetSelector(localRows);
							row.BindingValue = propValue;
							row.SelectorIndent = indent;
							if (row.Selector != null) row.SelectorIndent++;

							if (!row.IsShown()) continue;

							if (row.Control == null)
							{
								if (prop.Value.GetType() == typeof(Data.NodeBase)) continue;
								if (prop.Value.GetType() == typeof(Data.NodeBase.ChildrenCollection)) continue;
								if (prop.Value.GetType() == typeof(Data.Value.FCurve<float>)) continue;
								if (prop.Value.GetType() == typeof(Data.Value.FCurve<byte>)) continue;

								parseObject(propValue, row.SelectorIndent);
								continue;
							}
							else
							{
								row.ControlDynamic.SetBinding(propValue);
								objToTypeRow.Add(propValue, row);
							}
						}

						newRows.Add(row);
						localRows.Add(row);

						if (propValue is Data.IEditableValueCollection)
						{
							parseObject(propValue, row.SelectorIndent);
						}

						{
							var o0 = row.BindingValue as Data.Value.EnumBase;
							var o1 = row.BindingValue as Data.Value.PathForImage;
							var o2 = row.BindingValue as Data.IEditableValueCollection;
							if (o0 != null && row.IsSelector)
							{
								o0.OnChanged += ChangeSelector;
							}
							else if (o1 != null)
							{
								o1.OnChanged += ChangeSelector;
							}
							else if (o2 != null)
							{
								o2.OnChanged += ChangeSelector;
							}
						}
					}
				};

			parseObject(value, 0);

			foreach(var row in controlRows.Internal.ToArray())
			{
				if (newRows.Contains(row)) continue;
				RemoveRow(row, true);
				objToTypeRow.Remove(row.BindingValue);
			}

			controlRows.Clear();

			foreach(var n in newRows)
			{
				controlRows.Add(n);
			}

			bindingObject = value;

			if(bindingObject is Data.IEditableValueCollection)
			{
				var o2 = bindingObject as Data.IEditableValueCollection;
				o2.OnChanged += ChangeSelector;
			}
		}

		void ResetValue()
		{
			objToTypeRow.Clear();

			controlRows.Lock();
			foreach (var row in controlRows.Internal)
			{
				RemoveRow(row, true);
			}
			controlRows.Unlock();
		}

		void RemoveRow(TypeRow row, bool removeControls)
		{
			if (row.Control == null) return;

			row.ControlDynamic.SetBinding(null);

			if (bindingObject != null)
			{
				var o0 = row.BindingValue as Data.Value.EnumBase;
				var o1 = row.BindingValue as Data.Value.PathForImage;
				var o2 = row.BindingValue as Data.IEditableValueCollection;
				if (o0 != null && row.IsSelector)
				{
					o0.OnChanged -= ChangeSelector;
				}
				else if (o1 != null)
				{
					o1.OnChanged -= ChangeSelector;
				}
				else if (o2 != null)
				{
					o2.OnChanged -= ChangeSelector;
				}
			}

			if (removeControls)
			{
				if (row.Control is Control)
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
			Data.EditableValue editableValue;

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

			public object BindingValue { get; set; }

			public int SelectorIndent = 0;

			/// <summary>
			/// Generate based on property
			/// </summary>
			/// <param name="propInfo"></param>
			public TypeRow(Data.EditableValue propInfo)
			{
				editableValue = propInfo;

				Title = editableValue.Title;
				Description = editableValue.Description;
				EnableUndo = editableValue.IsUndoEnabled;
				var shown = editableValue.IsShown;

				IParameterControl gui = null;
				var type = editableValue.Value.GetType();

				if (!shown)
				{
					gui = null;
					return;
				}
				else if (type == typeof(Data.Value.String))
				{
					gui = new String(Title);
				}
				else if (type == typeof(Data.Value.Boolean))
				{
					gui = new Boolean(Title);
				}
				else if (type == typeof(Data.Value.Int))
				{
					gui = new Int(Title);
				}
				else if (type == typeof(Data.Value.IntWithInifinite))
				{
					gui = new IntWithInifinite(Title);
				}
				else if (type == typeof(Data.Value.IntWithRandom))
				{
					gui = new IntWithRandom(Title);
				}
				else if (type == typeof(Data.Value.Float))
				{
					gui = new Float(Title);
				}
				else if (type == typeof(Data.Value.FloatWithRandom))
				{
					gui = new FloatWithRandom(Title);
				}
				else if (type == typeof(Data.Value.Vector2D))
				{
					gui = new Vector2D(Title);
				}
				else if (type == typeof(Data.Value.Vector2DWithRandom))
				{
					gui = new Vector2DWithRandom(Title);
				}
				else if (type == typeof(Data.Value.Vector3D))
				{
					gui = new Vector3D(Title);
				}
				else if (type == typeof(Data.Value.Vector3DWithRandom))
				{
					gui = new Vector3DWithRandom(Title);
				}
				else if (type == typeof(Data.Value.Vector4D))
				{
					gui = new Vector4D(Title);
				}
				else if (type == typeof(Data.Value.Color))
				{
					gui = new ColorCtrl(Title);
				}
				else if (type == typeof(Data.Value.ColorWithRandom))
				{
					gui = new ColorWithRandom(Title);
				}
				else if (type == typeof(Data.Value.Path))
				{
					gui = null;
					return;
				}
				else if (type == typeof(Data.Value.PathForModel))
				{
					gui = new PathForModel(Title);
				}
				else if (type == typeof(Data.Value.PathForImage))
				{
					gui = new PathForImage(Title);
				}
				else if (type == typeof(Data.Value.PathForSound))
				{
					gui = new PathForSound(Title);
				}
#if MATERIAL_ENABLED
				else if (type == typeof(Data.Value.PathForMaterial))
				{
					gui = new PathForMaterial(Title);
				}
#endif
				else if (type == typeof(Data.Value.FCurveVector2D))
				{
					gui = new FCurveButton(Title);
				}
				else if (type == typeof(Data.Value.FCurveVector3D))
				{
					gui = new FCurveButton(Title);
				}
				else if (type == typeof(Data.Value.FCurveColorRGBA))
				{
					gui = new FCurveButton(Title);
				}
				else if (editableValue.Value is Data.IEditableValueCollection)
				{
					gui = new Dummy(Title);
				}
				else if (type == typeof(Data.Value.FCurve<float>))
				{
					gui = null;
					return;
				}
				else if (type == typeof(Data.Value.FCurve<byte>))
				{
					gui = null;
					return;
				}
				else if (type == typeof(Data.Value.IFCurveKey))
				{
					gui = null;
					return;
				}
				else if (type.IsGenericType)
				{
					var types = type.GetGenericArguments();
					gui = new Enum(Title);

					var dgui = (dynamic)gui;
					dgui.Initialize(types[0]);
				}

				if (editableValue.SelectorID >= 0)
				{
					IsSelector = true;
					SelectorID = editableValue.SelectorID;
				}
				else
				{
					IsSelector = false;
					SelectorID = -1;
				}

				Control = gui;

				if (gui != null)
				{
					gui.Description = this.Description;
				}

				Label = Title;

				ControlDynamic = Control;

				if (Control != null)
				{
					Control.EnableUndo = EnableUndo;
				}
			}

			public void SetSelector(List<TypeRow> sameLayerRows)
			{
				// Selector
				if(editableValue.SelectedID >= 0)
				{
					var selector = sameLayerRows.Where(_ => _.IsSelector && _.SelectorID == editableValue.SelectedID).LastOrDefault();

					if (selector != null)
					{
						Selector = selector;
						SelectorValue = editableValue.SelectedValue;
					}
				}
			}

			public bool IsShown()
			{
				if (Parent != null && !Parent.IsShown()) return false;
				if (!editableValue.IsShown) return false;

				if (Selector == null) return true;
				if (!Selector.IsShown()) return false;

				var value = Selector.BindingValue as Data.Value.EnumBase;

				if (value == null) return false;


				return SelectorValue == value.GetValueAsInt();
			}
		}
	}
}

