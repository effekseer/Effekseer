using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using Effekseer.swig;

namespace Effekseer.GUI.BindableComponent
{
	class ParameterListComponentFactory
	{
		static Dictionary<Type, Func<IParameterControl>> generators = new Dictionary<Type, Func<IParameterControl>>();

		public static void Register(Type type, Func<IParameterControl> generator)
		{
			generators.Add(type, generator);
		}

		public static IParameterControl Generate(Type type)
		{
			if (generators.ContainsKey(type))
			{
				return generators[type]();
			}
			return null;
		}
	}

	public class ParameterList : GroupControl, IControl, IDroppableControl
	{
		TypeRowCollection collection = new TypeRowCollection();

		bool isFirstUpdate = true;

		static void ChangeColumns2WithLabelWidth()
		{
			var regionAvail = Manager.NativeManager.GetContentRegionAvail();
			Manager.NativeManager.BeginTable("ParameterList", 2, TableFlags.BordersInner);
			Manager.NativeManager.TableSetupColumn("Label", TableColumnFlags.NoResize | TableColumnFlags.WidthFixed, regionAvail.X * 0.3f);
			Manager.NativeManager.TableSetupColumn("Parameter");
		}

		public ParameterList()
		{
		}

		public override void Update()
		{
			float dpiScale = Manager.DpiScale;

			Manager.NativeManager.PushStyleVar(ImGuiStyleVarFlags.CellPadding, new swig.Vec2(3.0f * dpiScale, 3.0f * dpiScale));
			ChangeColumns2WithLabelWidth();

			var indent = new IndentInformation();
			indent.Indent = 0;
			indent.IsSelecter = false;
			collection.Update(indent);

			Manager.NativeManager.EndTable();
			Manager.NativeManager.PopStyleVar();

			isFirstUpdate = false;
		}

		public void SetType(Type type)
		{
			// ignore
		}

		public void FixValues()
		{
			collection.FixValues();
		}

		public void SetValue(object value)
		{
			collection.SetValue(value, 0);
		}

		public override void DispatchDisposed()
		{
			collection.DispatchDisposed();
			OnDisposed();
		}

		public override void DispatchDropped(string path, ref bool handle)
		{
			collection.DispatchDropped(path, ref handle);
			base.DispatchDropped(path, ref handle);
		}

		struct IndentInformation
		{
			public int Indent;
			public bool IsSelecter;
		}

		class TypeRowCollection
		{
			object bindingObject = null;

			Utils.DelayedList<TypeRow> controlRows = new Utils.DelayedList<TypeRow>();

			Boolean controlEnabled = null;

			bool isControlsChanged = false;

			public TypeRowCollection()
			{
			}

			public bool IsValueChangedFromDefault
			{
				get
				{
					foreach (var row in controlRows.Internal)
					{
						if (row.Children?.IsValueChangedFromDefault ?? false)
						{
							return true;
						}

						var value = row.BindingValue as Data.IValueChangedFromDefault;
						if (value?.IsValueChangedFromDefault ?? false)
						{
							return true;
						}
					}

					return false;
				}
			}

			public IndentInformation Update(IndentInformation indent, object groupLabel = null)
			{
				var currentIndent = indent;
				if (isControlsChanged)
				{
					SetValue(bindingObject, 0);
				}

				controlRows.Lock();

				float dpiScale = Manager.DpiScale;

				for (int i = 0; i < controlRows.Internal.Count; i++)
				{
					var ctrl = controlRows.Internal[i].Control;

					if (controlRows.Internal[i].Children != null)
					{
						var item = controlRows.Internal[i];

						if (string.IsNullOrEmpty(item.TreeNodeID))
						{
							// Not tree view
							currentIndent.Indent = controlRows[i].SelectorIndent;
							currentIndent.IsSelecter = controlRows[i].IsSelector;

							// TODO : Make better implemention
							// To show a group name on a first element
							currentIndent = item.Children.Update(currentIndent, item.EditableValue.IsGroup ? item.Title : null);
						}
						else
						{
							// Tree view
							var label = item.Title.ToString() + "###" + item.TreeNodeID;

							if (item.BindingValue is Data.Group.IToggleMode)
							{
								// CollapsingHeader with toggle
								Manager.NativeManager.EndTable();

								var toggleId = "###" + item.TreeNodeID + "_toggle";
								var toggleMode = item.BindingValue as Data.Group.IToggleMode;
								bool enabled = toggleMode.Enabled.GetValue();

								bool opened = Manager.NativeManager.CollapsingHeaderWithToggle(label, swig.TreeNodeFlags.None, toggleId, ref enabled);

								if (enabled != toggleMode.Enabled.GetValue())
								{
									toggleMode.Enabled.SetValue(enabled);
								}

								if (opened && !enabled)
								{
									string message = MultiLanguageTextProvider.GetText("TreeDisabled_Message");
									Manager.NativeManager.TextWrapped(message);
								}

								ChangeColumns2WithLabelWidth();

								if (opened && enabled)
								{
									currentIndent = item.Children.Update(currentIndent);
								}
							}
							else if (item.TreeNodeType == Data.TreeNodeType.Large)
							{
								Manager.NativeManager.EndTable();

								var valueChanged = item.Children.IsValueChangedFromDefault;

								if (valueChanged)
								{
									Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.Header, 0xff75ba33);
								}

								bool opened = Manager.NativeManager.CollapsingHeader(label);

								if (valueChanged)
								{
									Manager.NativeManager.PopStyleColor();
								}

								ChangeColumns2WithLabelWidth();

								if (opened)
								{
									currentIndent = item.Children.Update(currentIndent);
								}
							}
							else if (item.TreeNodeType == Data.TreeNodeType.Small)
							{
								var flag = swig.TreeNodeFlags.SpanFullWidth;

								Manager.NativeManager.EndTable();

								if (Manager.NativeManager.TreeNodeEx(label, flag))
								{
									ChangeColumns2WithLabelWidth();

									currentIndent = item.Children.Update(currentIndent);

									// Avoid Tree node bug
									Manager.NativeManager.EndTable();
									Manager.NativeManager.TreePop();
								}

								ChangeColumns2WithLabelWidth();
							}
							else
							{
								throw new NotImplementedException();
							}


						}
					}
					else if (ctrl is Dummy || ctrl == null)
					{
						continue;
					}
					else
					{
						Manager.NativeManager.TableNextColumn();

						// Show a label
						Manager.NativeManager.SetCursorPosX(Manager.NativeManager.GetCursorPosX() + 4.0f * dpiScale);
						Manager.NativeManager.SetCursorPosY(Manager.NativeManager.GetCursorPosY() + Manager.TextOffsetY);

						// TODO : Make better imeplementation
						if (groupLabel != null && i == 0)
						{
							Manager.NativeManager.Text(groupLabel.ToString());
						}
						else
						{
							Manager.NativeManager.Text(controlRows.Internal[i].Title.ToString());
						}

						if (Manager.NativeManager.IsItemHovered())
						{
							Manager.NativeManager.BeginTooltip();

							Manager.NativeManager.Text(controlRows.Internal[i].Title.ToString());
							Manager.NativeManager.Separator();
							Manager.NativeManager.Text(controlRows.Internal[i].Description.ToString());

							Manager.NativeManager.EndTooltip();
						}

						Manager.NativeManager.TableNextColumn();

						// Show a content
						Manager.NativeManager.PushItemWidth(-1);
						ctrl.Update();
						Manager.NativeManager.PopItemWidth();

						currentIndent.Indent = controlRows[i].SelectorIndent;
						currentIndent.IsSelecter = controlRows[i].IsSelector;
					}
				}

				controlRows.Unlock();

				if (isControlsChanged)
				{
					SetValue(bindingObject, 0);
				}

				return currentIndent;
			}

			public void FixValues()
			{
				controlRows.Lock();

				foreach (var c in controlRows.Internal.Select(_ => _.Control).OfType<IParameterControl>())
				{
					c.FixValue();
				}

				foreach (var c in controlRows.Internal)
				{
					c.Children?.FixValues();
				}

				controlRows.Unlock();
			}

			public void SetValue(object value, int indent)
			{
				if (value != null)
				{
					RegisterValue(value, indent);
				}
				else
				{
					controlRows.Lock();
					foreach (var row in controlRows.Internal)
					{
						RemoveRow(row, true);
					}
					controlRows.Unlock();

					if (bindingObject is Data.IEditableValueCollection)
					{
						var o2 = bindingObject as Data.IEditableValueCollection;
						o2.OnChanged -= ChangeSelector;
					}

					bindingObject = null;
				}

				isControlsChanged = false;
			}

			void RegisterValue(object value, int indent)
			{
				List<TypeRow> newRows = new List<TypeRow>();

				Dictionary<object, TypeRow> objToTypeRow = new Dictionary<object, TypeRow>();

				foreach (var row in controlRows.Internal)
				{
					objToTypeRow.Add(row.BindingValue, row);
				}

				{
					Data.EditableValue[] editableValues = null;

					if (value is Data.IEditableValueCollection)
					{
						editableValues = (value as Data.IEditableValueCollection).GetValues();
					}
					else
					{
						var props = value.GetType().GetProperties(BindingFlags.Public | BindingFlags.Instance | BindingFlags.DeclaredOnly).OrderBy(x => x.MetadataToken);
						editableValues = props.Select(_ => Data.EditableValue.Create(_.GetValue(value), _)).ToArray();
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
							row.UpdateTitleAndDesc(prop);
							row.SetSelector(localRows);
							row.SelectorIndent = indent;
							if (row.Selector != null) row.SelectorIndent++;
#if DEBUG
							if (row.BindingValue != propValue) throw new Exception();
#endif

							if (!row.IsShown()) continue;

							if (row.Control == null)
							{
								// It is possible that children are changed even if a parent is not changed.
								row.Children.RegisterValue(propValue, row.SelectorIndent);
							}
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
								row.Children = new TypeRowCollection();
								row.Children.RegisterValue(propValue, row.SelectorIndent);
							}
							else
							{
								row.ControlDynamic.SetBinding(propValue);
							}
							objToTypeRow.Add(propValue, row);
						}

						newRows.Add(row);
						localRows.Add(row);

						if (propValue is Data.IEditableValueCollection)
						{
							row.Children = new TypeRowCollection();
							row.Children.RegisterValue(propValue, row.SelectorIndent);
						}

						{
							var o0 = row.BindingValue as Data.Value.EnumBase;
							var o1 = row.BindingValue as Data.Value.PathForImage;
							var o2 = row.BindingValue as Data.IEditableValueCollection;
							var o3 = row.BindingValue as Data.Value.Boolean;

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
							else if (o3 != null)
							{
								o3.OnChanged += ChangeSelector;
							}
						}
					}
				}

				foreach (var row in controlRows.Internal.ToArray())
				{
					if (newRows.Contains(row)) continue;
					RemoveRow(row, true);
					objToTypeRow.Remove(row.BindingValue);
				}

				controlRows.Clear();

				foreach (var n in newRows)
				{
					controlRows.Add(n);
				}

				bindingObject = value;

				if (bindingObject is Data.IEditableValueCollection)
				{
					var o2 = bindingObject as Data.IEditableValueCollection;
					o2.OnChanged += ChangeSelector;
				}
			}

			void RemoveRow(TypeRow row, bool removeControls)
			{
				if (row.Children != null)
				{
					row.Children.SetValue(null, 0);
				}

				if (row.ControlDynamic != null)
				{
					row.ControlDynamic.SetBinding(null);
				}

				if (bindingObject != null)
				{
					var o0 = row.BindingValue as Data.Value.EnumBase;
					var o1 = row.BindingValue as Data.Value.PathForImage;
					var o2 = row.BindingValue as Data.IEditableValueCollection;
					var o3 = row.BindingValue as Data.Value.Boolean;
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
					else if (o3 != null)
					{
						o3.OnChanged += ChangeSelector;
					}
				}

				if (removeControls && row.Control != null)
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


			private void InternalDispatchDisposed(Utils.DelayedList<TypeRow> rows)
			{
				foreach (var row in rows.Internal)
				{
					if (row.Control != null)
					{
						var c = row.Control as Control;
						c.DispatchDisposed();
					}
					else
					{
						if (row.Children == null) continue;
						InternalDispatchDisposed(row.Children.controlRows);
					}
				}
			}

			public void DispatchDisposed()
			{
				InternalDispatchDisposed(controlRows);
			}

			private void InternalDispatchDropped(Utils.DelayedList<TypeRow> rows, string path, ref bool handle)
			{
				if (handle) return;

				foreach (var row in rows.Internal)
				{
					if (row.Control != null)
					{
						var c = row.Control as Control;
						c.DispatchDropped(path, ref handle);
						if (handle) break;
					}
					if (row.Children != null)
					{
						InternalDispatchDropped(row.Children.controlRows, path, ref handle);
					}
				}
			}

			public void DispatchDropped(string path, ref bool handle)
			{
				InternalDispatchDropped(controlRows, path, ref handle);
			}
		}

		class TypeRow
		{
			public string TreeNodeID = null;

			public Data.TreeNodeType TreeNodeType = Data.TreeNodeType.Large;

			public TypeRowCollection Children;

			public Data.EditableValue EditableValue { get; private set; }

			public object Title
			{
				get;
				private set;
			}

			public object Description
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

			public bool IsSelector
			{
				get;
				private set;
			}

			public int SelfSelectorID
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
			/// A value which is required to show
			/// </summary>
			public int[] RequiredSelectorValues
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
				EditableValue = propInfo;

				Title = EditableValue.Title;
				Description = EditableValue.Description;
				EnableUndo = EditableValue.IsUndoEnabled;
				var shown = EditableValue.IsShown;

				IParameterControl gui = null;
				var type = EditableValue.Value.GetType();

				if (!shown)
				{
					gui = null;
					return;
				}

				gui = ParameterListComponentFactory.Generate(type);

				if (gui != null)
				{
					// already generated
				}
				else if (type == typeof(Data.NodeBase) || type == typeof(Data.NodeBase.ChildrenCollection))
				{
					// Ignore
					gui = new Dummy();
					return;
				}
				else if (type == typeof(Data.Value.String))
				{
					gui = new String();
				}
				else if (type == typeof(Data.Value.Boolean))
				{
					gui = new Boolean();
				}
				else if (type == typeof(Data.Value.Int))
				{
					gui = new Int();
				}
				else if (type == typeof(Data.Value.Int2))
				{
					gui = new Int2();
				}
				else if (type == typeof(Data.Value.IntWithInifinite))
				{
					gui = new IntWithInifinite();
				}
				else if (type == typeof(Data.Value.IntWithRandom))
				{
					gui = new IntWithRandom();
				}
				else if (type == typeof(Data.Value.Float))
				{
					gui = new Float();
				}
				else if (type == typeof(Data.Value.FloatWithRandom))
				{
					gui = new FloatWithRandom();
				}
				else if (type == typeof(Data.Value.Vector2D))
				{
					gui = new Vector2D();
				}
				else if (type == typeof(Data.Value.Vector2DWithRandom))
				{
					gui = new Vector2DWithRandom();
				}
				else if (type == typeof(Data.Value.Vector3D))
				{
					gui = new Vector3D();
				}
				else if (type == typeof(Data.Value.Vector3DWithRandom))
				{
					gui = new Vector3DWithRandom();
				}
				else if (type == typeof(Data.Value.Vector4D))
				{
					gui = new Vector4D();
				}
				else if (type == typeof(Data.Value.Color))
				{
					gui = new ColorCtrl();
				}
				else if (type == typeof(Data.Value.ColorWithRandom))
				{
					gui = new ColorWithRandom();
				}
				else if (type == typeof(Data.Value.Path))
				{
					// Ignore
					gui = new Dummy();
					return;
				}
				else if (type == typeof(Data.Value.PathForModel))
				{
					gui = new PathForModel();
				}
				else if (type == typeof(Data.Value.PathForImage))
				{
					gui = new PathForImage();
				}
				else if (type == typeof(Data.Value.PathForSound))
				{
					gui = new PathForSound();
				}
				else if (type == typeof(Data.Value.PathForMaterial))
				{
					gui = new PathForMaterial();
				}
				else if (type == typeof(Data.Value.PathForCurve))
				{
					gui = new PathForCurve();
				}
				else if (type == typeof(Data.Value.FCurveScalar))
				{
					gui = new FCurveButton();
				}
				else if (type == typeof(Data.Value.FCurveVector2D))
				{
					gui = new FCurveButton();
				}
				else if (type == typeof(Data.Value.FCurveVector3D))
				{
					gui = new FCurveButton();
				}
				else if (type == typeof(Data.Value.FCurveColorRGBA))
				{
					gui = new FCurveButton();
				}
				else if (EditableValue.Value is Data.IEditableValueCollection)
				{
					gui = new Dummy();
				}
				else if (type == typeof(Data.Value.FCurve<float>) || type == typeof(Data.Value.FCurve<byte>) || type == typeof(Data.Value.IFCurveKey))
				{
					// Ignore
					gui = new Dummy();
					return;
				}
				else if (type.IsGenericType)
				{
					var types = type.GetGenericArguments();
					gui = new Enum();

					var dgui = (dynamic)gui;
					dgui.Initialize(types[0]);
				}

				if (EditableValue.SelfSelectorID >= 0)
				{
					IsSelector = true;
					SelfSelectorID = EditableValue.SelfSelectorID;
				}
				else
				{
					IsSelector = false;
					SelfSelectorID = -1;
				}

				Control = gui;

				TreeNodeID = propInfo.TreeNodeID;
				TreeNodeType = propInfo.TreeNodeType;

				ControlDynamic = Control;

				if (Control != null)
				{
					Control.EnableUndo = EnableUndo;
				}
			}

			public void UpdateTitleAndDesc(Data.EditableValue propInfo)
			{
				Title = propInfo.Title;
				Description = propInfo.Description;
			}
			public void SetSelector(List<TypeRow> sameLayerRows)
			{
				// Selector
				if (EditableValue.TargetSelectorID >= 0)
				{
					var selector = sameLayerRows.Where(_ => _.IsSelector && _.SelfSelectorID == EditableValue.TargetSelectorID).LastOrDefault();

					if (selector != null)
					{
						Selector = selector;
						RequiredSelectorValues = EditableValue.RequiredSelectorValues;
					}
				}
			}

			public bool IsShown()
			{
				if (Parent != null && !Parent.IsShown()) return false;
				if (!EditableValue.IsShown) return false;

				if (Selector == null) return true;
				if (!Selector.IsShown()) return false;

				var value = Selector.BindingValue as Data.Value.EnumBase;

				if (value == null)
				{
					if (Selector.BindingValue.GetType() == typeof(Data.Value.Boolean))
					{
						var v = Selector.BindingValue as Data.Value.Boolean;
						return v.Value;
					}

					return false;
				}

				return RequiredSelectorValues.Contains(value.GetValueAsInt());
			}
		}
	}
}