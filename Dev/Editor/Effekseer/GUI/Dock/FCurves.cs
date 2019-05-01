using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.Data.Value;

namespace Effekseer.GUI.Dock
{
	class FCurves : DockPanel
	{

		string frame_text = Resources.GetString("Frame");
		string value_text = Resources.GetString("Value");
		string start_text = Resources.GetString("Start");
		string end_text = Resources.GetString("End");
		string type_text = Resources.GetString("Complement");
		string sampling_text = Resources.GetString("Sampling");
		string left_text = Resources.GetString("Left");
		string right_text = Resources.GetString("Right");
		string offset_text = Resources.GetString("Offset");
		string offset_min_text = Resources.GetString("Min");
		string offset_max_text = Resources.GetString("Max");

		Utl.ParameterTreeNode paramaterTreeNode = null;

		TreeNode treeNodes = null;

		bool loading = false;

		bool canCurveControlPre = true;
		bool canCurveControl = true;
		bool canControl = true;

		Component.Enum startCurve = new Component.Enum();
		Component.Enum endCurve = new Component.Enum();
		Component.Enum type = new Component.Enum();

		bool isAutoZoomMode = false;
		float autoZoomRangeMin = float.MaxValue;
		float autoZoomRangeMax = float.MinValue;

		bool isFirstUpdate = true;

		Action<float, float> moved = null;

		public FCurves()
		{
			Label = Resources.GetString("FCurves") + "###FCurves";

			Command.CommandManager.Changed += OnChanged;
			Core.OnAfterNew += OnChanged;
			Core.OnBeforeLoad += OnBeforeLoad;
			Core.OnAfterLoad += OnAfterLoad;

			startCurve.Initialize(typeof(Data.Value.FCurveEdge));
			startCurve.Label = start_text + "##Start";

			endCurve.Initialize(typeof(Data.Value.FCurveEdge));
			endCurve.Label = end_text + "##End";

			type.Initialize(typeof(Data.Value.FCurveInterpolation));
			type.Label = type_text + "##Type";

			OnChanged();

			Icon = Images.GetIcon("PanelFCurve");
			IconSize = new swig.Vec2(24, 24);
			TabToolTip = Resources.GetString("FCurves");

			// auto zoom event
			moved += (x, y) =>
			{
				if (!isAutoZoomMode) return;

				float min_y, max_y;
				GetRange(treeNodes, out min_y, out max_y);

				if(min_y < autoZoomRangeMin && y < 0)
				{
					autoZoomRangeMin += y;
					autoZoomRangeMax += y;
				}

				if (max_y > autoZoomRangeMax && y > 0)
				{
					autoZoomRangeMin += y;
					autoZoomRangeMax += y;
				}
			};
		}

		protected override void UpdateInternal()
		{
			canControl = true;

			var selected = GetSelectedFCurve();

			var invalidValue = "/";
			var selectedInd = selected.Item1 != null ? selected.Item2.GetSelectedIndex() : -1;

			// line 1
			Manager.NativeManager.Columns(5);

			if (selectedInd >= 0)
			{
				var frameKey = new int[] { (int)selected.Item2.Keys[selectedInd] };
				if (Manager.NativeManager.DragInt(frame_text, frameKey))
				{
					var diff = frameKey[0] - selected.Item2.Keys[selectedInd];

					selected.Item2.Keys[selectedInd] = (int)frameKey[0];
					selected.Item2.LeftKeys[selectedInd] += diff;
					selected.Item2.RightKeys[selectedInd] += diff;
					selected.Item2.IsDirtied = true;
					selected.Item2.SolveContradiction();
				}

				if (Manager.NativeManager.IsItemActive()) canControl = false;
			}
			else
			{
				Manager.NativeManager.InputText(frame_text, invalidValue, swig.InputTextFlags.ReadOnly);
			}

			Manager.NativeManager.NextColumn();

			if (selectedInd >= 0)
			{
				var frameValue = new float[] { selected.Item2.Values[selectedInd] };
				if (Manager.NativeManager.DragFloat(value_text, frameValue))
				{
					var diff = frameValue[0] - selected.Item2.Values[selectedInd];

					selected.Item2.Values[selectedInd] = frameValue[0];
					selected.Item2.LeftValues[selectedInd] += diff;
					selected.Item2.RightValues[selectedInd] += diff;
					selected.Item2.IsDirtied = true;
				}

				if (Manager.NativeManager.IsItemActive()) canControl = false;
			}
			else
			{
				Manager.NativeManager.InputText(value_text, invalidValue, swig.InputTextFlags.ReadOnly);
			}

			Manager.NativeManager.NextColumn();

			// Left key
			if (selectedInd >= 0)
			{
				var leftValues = new float[] { selected.Item2.LeftKeys[selectedInd], selected.Item2.LeftValues[selectedInd] };
				if (Manager.NativeManager.DragFloat2(left_text, leftValues))
				{
					selected.Item2.LeftKeys[selectedInd] = leftValues[0];
					selected.Item2.LeftValues[selectedInd] = leftValues[1];
					selected.Item2.Clip(selectedInd);
					selected.Item2.IsDirtied = true;
				}

				if (Manager.NativeManager.IsItemActive()) canControl = false;
			}
			else
			{
				Manager.NativeManager.InputText(left_text, invalidValue, swig.InputTextFlags.ReadOnly);
			}

			Manager.NativeManager.NextColumn();

			// Right key
			if (selectedInd >= 0)
			{
				var rightValues = new float[] { selected.Item2.RightKeys[selectedInd], selected.Item2.RightValues[selectedInd] };
				if (Manager.NativeManager.DragFloat2(right_text, rightValues))
				{
					selected.Item2.RightKeys[selectedInd] = rightValues[0];
					selected.Item2.RightValues[selectedInd] = rightValues[1];
					selected.Item2.Clip(selectedInd);
					selected.Item2.IsDirtied = true;
				}

				if (Manager.NativeManager.IsItemActive()) canControl = false;
			}
			else
			{
				Manager.NativeManager.InputText(right_text, invalidValue, swig.InputTextFlags.ReadOnly);
			}

			Manager.NativeManager.NextColumn();

			if (selectedInd >= 0)
			{
				if (Manager.NativeManager.BeginCombo(type.Label, type.FieldNames[selected.Item2.Interpolations[selectedInd]], swig.ComboFlags.None))
				{
					for (int i = 0; i < type.FieldNames.Count; i++)
					{
						bool is_selected = (type.FieldNames[(int)selected.Item2.Interpolations[selectedInd]] == type.FieldNames[i]);

						if (Manager.NativeManager.Selectable(type.FieldNames[i], is_selected, swig.SelectableFlags.None))
						{
							selected.Item2.Interpolations[selectedInd] = i;
							selected.Item2.IsDirtied = true;
						}

						if (is_selected)
						{
							Manager.NativeManager.SetItemDefaultFocus();
						}
					}

					Manager.NativeManager.EndCombo();
				}

				if (Manager.NativeManager.IsItemActive()) canControl = false;
			}
			else
			{
				Manager.NativeManager.InputText(type_text, invalidValue, swig.InputTextFlags.ReadOnly);
			}

			Manager.NativeManager.Columns(1);
				
			Manager.NativeManager.Columns(4);

			// Start curve
			if(selected.Item1 != null)
			{
				startCurve.SetBinding(selected.Item1.StartType);
				startCurve.Update();
				selected.Item2.StartEdge = (Data.Value.FCurveEdge)selected.Item1.StartType;

				if (Manager.NativeManager.IsItemActive()) canControl = false;
			}
			else
			{
				Manager.NativeManager.InputText(start_text, invalidValue, swig.InputTextFlags.ReadOnly);
			}

			Manager.NativeManager.NextColumn();

			// End curve

			if (selected.Item1 != null)
			{
				endCurve.SetBinding(selected.Item1.EndType);
				endCurve.Update();
				selected.Item2.EndEdge = (Data.Value.FCurveEdge)selected.Item1.EndType;

				if (Manager.NativeManager.IsItemActive()) canControl = false;
			}
			else
			{
				Manager.NativeManager.InputText(end_text, invalidValue, swig.InputTextFlags.ReadOnly);
			}

			Manager.NativeManager.NextColumn();

			// Sampling
			if (selected.Item1 != null)
			{
				var sampling = new int[] { selected.Item1.Sampling };

				if (Manager.NativeManager.InputInt(sampling_text + "##SamplingText", sampling))
				{
					selected.Item1.Sampling.SetValue(sampling[0]);
				}

				if (Manager.NativeManager.IsItemActive()) canControl = false;
			}
			else
			{
				Manager.NativeManager.InputText(sampling_text + "##SamplingText", invalidValue, swig.InputTextFlags.ReadOnly);
			}

			Manager.NativeManager.NextColumn();

			// Offset label
			if (selected.Item1 != null)
			{
				var offset_id = offset_text + "##OffsetMinMax";
				var offsets = new float[] { selected.Item1.OffsetMin, selected.Item1.OffsetMax };

				if (Manager.NativeManager.DragFloat2(offset_id, offsets))
				{
					selected.Item1.OffsetMin.SetValue(offsets[0]);
					selected.Item1.OffsetMax.SetValue(offsets[1]);
				}

				if (Manager.NativeManager.IsItemActive()) canControl = false;
			}
			else
			{
				Manager.NativeManager.InputText(offset_text + "##OffsetMinMax", invalidValue, swig.InputTextFlags.ReadOnly);
			}

			Manager.NativeManager.Columns(1);
			
			if(selected.Item1 == null)
			{
				startCurve.SetBinding(null);
				endCurve.SetBinding(null);
			}

			{
				if (Manager.NativeManager.ImageButton(Images.GetIcon("EnlargeAnchor"), 24, 24))
				{
					EnlargeAnchors();
				}

				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.SetTooltip(Resources.GetString("EnlargeAnchor") + "\n" + Resources.GetString("EnlargeAnchor_Desc"));
				}

				Manager.NativeManager.SameLine();

				if (Manager.NativeManager.ImageButton(Images.GetIcon("ShrinkAnchor"), 24, 24))
				{
					ShrinkAnchors();
				}

				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.SetTooltip(Resources.GetString("ShrinkAnchor") + "\n" + Resources.GetString("ShrinkAnchor_Desc"));
				}
			}

			//Manager.NativeManager.BeginGroup();

			Manager.NativeManager.Columns(2);

			// hot key
			if (IsDockActive() && canControl)
			{
				int delete_num = 261;

				if(Manager.NativeManager.IsKeyDown(delete_num))
				{
					// todo canControl = false;
				}
			}


			if (isFirstUpdate)
			{
				Manager.NativeManager.SetColumnWidth(0, 200);
			}
			//Manager.NativeManager.BeginChild("##FCurveGroup_Tree");

			if (treeNodes != null)
			{
				UpdateTreeNode(treeNodes);
			}

			//Manager.NativeManager.EndChild();

			Manager.NativeManager.NextColumn();

			//Manager.NativeManager.BeginChild("##FCurveGroup_Graph");

			if(isAutoZoomMode)
			{
				if (canCurveControl)
				{
					float min_value, max_value;
					GetRange(treeNodes, out min_value, out max_value);
					var range = (max_value - min_value);
					autoZoomRangeMin = min_value - range * 0.1f;
					autoZoomRangeMax = max_value + range * 0.1f;
				}
			}
			else
			{
				//autoZoomRangeMin = float.MaxValue;
				//autoZoomRangeMax = float.MinValue;
			}

			var graphSize = Manager.NativeManager.GetContentRegionAvail();
			graphSize.X = Math.Max(graphSize.X, 32);
			graphSize.Y = Math.Max(graphSize.Y, 32);
			graphSize.Y -= 28;

			var scale = new swig.Vec2(12, 4);

			if (Manager.NativeManager.BeginFCurve(1, graphSize, Manager.Viewer.Current, scale, autoZoomRangeMin, autoZoomRangeMax))
			{
				UpdateGraph(treeNodes);
			}

			// Reset area
			autoZoomRangeMin = float.MaxValue;
			autoZoomRangeMax = float.MinValue;

			Manager.NativeManager.EndFCurve();

			//Manager.NativeManager.EndChild();

			//Manager.NativeManager.EndGroup();

			Manager.NativeManager.Columns(1);

			/*
			if (isAutoZoomMode)
			{
				if (Manager.NativeManager.ImageButton(Images.GetIcon("AutoZoom_On"), 24, 24))
				{
					isAutoZoomMode = false;
				}
			}
			else
			{
				if (Manager.NativeManager.ImageButton(Images.GetIcon("AutoZoom_Off"), 24, 24))
				{
					isAutoZoomMode = true;
				}
			}
			*/

			//if (Manager.NativeManager.IsItemHovered())
			//{
			//	Manager.NativeManager.SetTooltip(Resources.GetString("AutoZoom") + "\n" + Resources.GetString("AutoZoom_Desc"));
			//}

			//Manager.NativeManager.SameLine();

			Manager.NativeManager.Text(Resources.GetString("FCurveCtrl_Desc"));

			isFirstUpdate = false;
		}

		public override void OnDisposed()
		{

			Action<TreeNode> recurse = null;

			recurse = (t) =>
			{
				foreach (var fcurve in t.FCurves)
				{
					fcurve.OnRemoved();
				}

				foreach (var c in t.Children)
				{
					recurse(c);
				}
			};

			recurse(treeNodes);

			Command.CommandManager.Changed -= OnChanged;
			Core.OnAfterNew -= OnChanged;
			Core.OnBeforeLoad -= OnBeforeLoad;
			Core.OnAfterLoad -= OnAfterLoad;
		}

		void UpdateTreeNode(TreeNode treeNode)
		{
			var flag = swig.TreeNodeFlags.OpenOnArrow | swig.TreeNodeFlags.OpenOnDoubleClick | swig.TreeNodeFlags.DefaultOpen;

			if (Manager.NativeManager.TreeNodeEx(treeNode.ParamTreeNode.Node.Name.Value + treeNode.ID, flag))
			{
				foreach (var fcurve in treeNode.FCurves)
				{
					fcurve.UpdateTree();
				}

				for (int i = 0; i < treeNode.Children.Count; i++)
				{
					UpdateTreeNode(treeNode.Children[i]);
				}

				Manager.NativeManager.TreePop();
			}
		}

		void UpdateGraph(TreeNode treeNode)
		{
			UpdateGraph(treeNodes, ref canControl);

			canCurveControlPre = canCurveControl;
			canCurveControl = canControl;

			// Check update
			if(!canCurveControlPre && canCurveControl)
			{
				Func<TreeNode, bool> isDirtied = null;

				isDirtied = (t) =>
				{
					foreach (var fcurve in t.FCurves)
					{
						if (fcurve.IsDirtied()) return true;
					}

					foreach (var c in t.Children)
					{
						if (isDirtied(c)) return true;
					}

					return false;
				};

				if(isDirtied(treeNode))
				{
					Command.CommandManager.StartCollection();

					Action<TreeNode> recurse = null;

					recurse = (t) =>
					{
						foreach (var fcurve in t.FCurves)
						{
							fcurve.Commit();
						}

						foreach (var c in t.Children)
						{
							recurse(c);
						}
					};

					recurse(treeNodes);

					Command.CommandManager.EndCollection();
				}
			}

			canCurveControl = canControl;
		}

		void GetRange(TreeNode treeNode, out float min_value, out float max_value)
		{
			min_value = float.MaxValue;
			max_value = float.MinValue;

			foreach (var fcurve in treeNode.FCurves)
			{
				float min_v = 0;
				float max_v = 0;

				fcurve.GetRange(out min_v, out max_v);

				min_value = Math.Min(min_value, min_v);
				max_value = Math.Max(max_value, max_v);
			}

			for (int i = 0; i < treeNode.Children.Count; i++)
			{
				float min_v = 0;
				float max_v = 0;

				GetRange(treeNode.Children[i], out min_v, out max_v);

				min_value = Math.Min(min_value, min_v);
				max_value = Math.Max(max_value, max_v);
			}

			if (min_value == float.MaxValue) min_value = 0.0f;
			if (max_value == float.MinValue) max_value = 0.0f;
		}

		void UpdateGraph(TreeNode treeNode, ref bool canControl)
		{
			
			foreach (var fcurve in treeNode.FCurves)
			{
				fcurve.UpdateGraph(ref canControl);
			}

			for (int i = 0; i < treeNode.Children.Count; i++)
			{
				UpdateGraph(treeNode.Children[i], ref canControl);
			}

		}

		void OnBeforeLoad(object sender, EventArgs e)
		{
			loading = true;
		}

		void OnAfterLoad(object sender, EventArgs e)
		{
			loading = false;
			OnChanged();
		}

		void OnChanged(object sender, EventArgs e)
		{
			if (loading) return;

			OnChanged();
		}

		void OnChanged()
		{
			var paramTreeNodes = Core.GetFCurveParameterNode();
			SetParameters(paramTreeNodes);
		}

		void SetParameters(Utl.ParameterTreeNode paramTreeNodes)
		{
			List<FCurve> befores = new List<FCurve>();
			List<FCurve> afters = new List<FCurve>();

			Action<TreeNode, List<FCurve>> getFcurves = null;
			getFcurves = (node, target) =>
			{
				if (node == null) return;
				foreach (var f in node.FCurves)
				{
					target.Add(f);
				}

				foreach (var c in node.Children)
				{
					getFcurves(c, target);
				}
			};

			getFcurves(treeNodes, befores);

			// initialize
			if (treeNodes == null)
			{
				treeNodes = new TreeNode(this, paramTreeNodes);
			}

			if (treeNodes.ParamTreeNode.Node != paramTreeNodes.Node ||
				treeNodes.ParamTreeNode.Children.Length != paramTreeNodes.Node.Children.Count)
			{
				treeNodes = new TreeNode(this, paramTreeNodes);
			}

			// compare node structures
			if (treeNodes.ParamTreeNode.Node != paramTreeNodes.Node)
			{
				treeNodes = new TreeNode(this, paramTreeNodes);
			}
			else
			{
				Action<Utl.ParameterTreeNode, TreeNode> refleshNodes = null;
				refleshNodes = (ptn, tn) =>
				{
					// check whether modification doesn't exist
					bool nochange = tn.Children.Count() == ptn.Children.Count() && 
					tn.Children.Select(_=>_.ParamTreeNode.Node).SequenceEqual(ptn.Children.Select(_=>_.Node));

					for (int i = 0; i < ptn.Children.Count() && nochange; i++)
					{
						if (tn.Children[i].ParamTreeNode.Node != ptn.Children[i].Node)
						{
							nochange = false;
							break;
						}
					}
					if (nochange)
					{
						for (int i = 0; i < ptn.Children.Count(); i++)
						{
							refleshNodes(ptn.Children[i], tn.Children[i]);
						}
						return;
					}

					// if moditications exsist
					var a = new TreeNode[ptn.Children.Count()];

					for (int i = 0; i < ptn.Children.Count(); i++)
					{
						TreeNode tn_ = tn.Children.FirstOrDefault(_ => _.ParamTreeNode.Node == ptn.Children[i].Node);
						if (tn_ != null)
						{
							// copy
							refleshNodes(ptn.Children[i], tn_);
							a[i] = tn_;
						}
						else
						{
							// new
							a[i] = new TreeNode(this, ptn.Children[i]);
						}
					}

					tn.Children.Clear();
					tn.Children.AddRange(a);
				};

				refleshNodes(paramTreeNodes, treeNodes);
			}

			// compare node structures
			{
				Action<Utl.ParameterTreeNode, TreeNode> refleshNodes = null;
				refleshNodes = (ptn, tn) =>
				{
					// check whether modification doesn't exist
					bool nochange = true;
					if (tn.FCurves.Count() == ptn.Parameters.Count())
					{
						for (int i = 0; i < tn.FCurves.Count(); i++)
						{
							if (tn.FCurves[i].GetValueAsObject() != ptn.Parameters[i].Item2)
							{
								nochange = false;
								break;
							}
						}
					}
					else
					{
						nochange = false;
					}

					if (nochange)
					{
						for (int i = 0; i < tn.Children.Count(); i++)
						{
							refleshNodes(ptn.Children[i], tn.Children[i]);
						}
						return;
					}

					// if moditications exsist
					var a = new FCurve[ptn.Parameters.Count()];

					for (int i = 0; i < ptn.Parameters.Count(); i++)
					{
						FCurve f = tn.FCurves.FirstOrDefault(_ => _.GetValueAsObject() == ptn.Parameters[i].Item2);
						if (f != null)
						{
							// copy
							a[i] = f;
						}
						else
						{
							// new
							a[i] = FCurve.Create(ptn.Parameters[i], this);
							a[i].ParentNode = tn;
						}
					}

					tn.FCurves.Clear();
					tn.FCurves.AddRange(a);

					for (int i = 0; i < tn.Children.Count(); i++)
					{
						refleshNodes(ptn.Children[i], tn.Children[i]);
					}
				};

				refleshNodes(paramTreeNodes, treeNodes);
			}

			getFcurves(treeNodes, afters);

			// event on removing
			foreach (var f in befores)
			{
				if (!afters.Contains(f))
				{
					f.OnRemoved();
				}
			}

			// event on adding
			foreach (var f in afters)
			{
				if (!befores.Contains(f))
				{
					f.OnAdded();
				}
			}

			paramaterTreeNode = paramTreeNodes;
		}

		public void UnselectAll()
		{
			Action<TreeNode> recurse = null;

			recurse = (t) =>
			{
				foreach(var fcurve in t.FCurves)
				{
					fcurve.Unselect();
				}

				foreach(var c in t.Children)
				{
					recurse(c);
				}
			};

			recurse(treeNodes);
		}

		public void HideAll()
		{
			Action<TreeNode> recurse = null;

			recurse = (t) =>
			{
				foreach (var fcurve in t.FCurves)
				{
					fcurve.Hide();
				}

				foreach (var c in t.Children)
				{
					recurse(c);
				}
			};

			recurse(treeNodes);
		}

		public void ShrinkAnchors()
		{
			Action<TreeNode> recurse = null;

			recurse = (t) =>
			{
				foreach (var fcurve in t.FCurves)
				{
					fcurve.ShrinkAnchors();
				}

				foreach (var c in t.Children)
				{
					recurse(c);
				}
			};

			recurse(treeNodes);
		}

		public void EnlargeAnchors()
		{
			Action<TreeNode> recurse = null;

			recurse = (t) =>
			{
				foreach (var fcurve in t.FCurves)
				{
					fcurve.EnlargeAnchors();
				}

				foreach (var c in t.Children)
				{
					recurse(c);
				}
			};

			recurse(treeNodes);
		}

		public void Move(float x, float y, int changedType, Data.Value.IFCurve except)
		{
			Action<TreeNode> recurse = null;

			recurse = (t) =>
			{
				foreach (var fcurve in t.FCurves)
				{
					fcurve.Move(x, y, changedType, except);
				}

				foreach (var c in t.Children)
				{
					recurse(c);
				}
			};

			recurse(treeNodes);

			if(moved != null)
			{
				moved(x, y);
			}
		}

		Tuple35<Data.Value.IFCurve, FCurveProperty> GetSelectedFCurve()
		{
			List<Tuple35<Data.Value.IFCurve, FCurveProperty>> rets = new List<Tuple35<IFCurve, FCurveProperty>>();
			Action<TreeNode> recurse = null;

			recurse = (t) =>
			{
				foreach (var fcurve in t.FCurves)
				{
					var r = fcurve.GetSelectedFCurve();
					if(r.Item1 != null)
					{
						rets.Add(r);
					}
				}

				foreach (var c in t.Children)
				{
					recurse(c);
				}
			};

			recurse(treeNodes);

			if (rets.Count != 1) return new Tuple35<Data.Value.IFCurve, FCurveProperty>(null, null);

			return rets[0];
		}

		class TreeNode
		{
			public string ID = string.Empty;

			public Utl.ParameterTreeNode ParamTreeNode { get; private set; }

			public List<TreeNode> Children { get; private set; }

			//public Math.Point Position { get; private set; }

			public int Height { get; private set; }

			public List<FCurve> FCurves { get; private set; }

			public bool IsExtended = true;

			FCurves window = null;

			public TreeNode(FCurves window, Utl.ParameterTreeNode paramTreeNode)
			{
				this.window = window;
				ID = "###" + Manager.GetUniqueID().ToString();

				Children = new List<TreeNode>();
				FCurves = new List<FCurve>();

				ParamTreeNode = paramTreeNode;

				foreach (var tn in paramTreeNode.Children)
				{
					Children.Add(new TreeNode(window, tn));
				}

				foreach (var v in paramTreeNode.Parameters)
				{
					var v_ = FCurve.Create(v, window);
					v_.ParentNode = this;

					FCurves.Add(v_);
				}
			}
		}

		abstract class FCurve
		{
			protected int LEFT_SHIFT = 340;
			protected int RIGHT_SHIFT = 344;

			public TreeNode ParentNode { get; set; }

			public string Name { get; protected set; }

			public abstract object GetValueAsObject();

			public virtual Tuple35<Data.Value.IFCurve, FCurveProperty> GetSelectedFCurve() { return new Tuple35<Data.Value.IFCurve, FCurveProperty>(null, null); }

			static public FCurve Create(Tuple35<string, object> v, FCurves window)
			{
				if (v.Item2 is Data.Value.FCurveVector2D)
				{
					Func<float, float> converter = (float pre) => { return pre; };

					var v_ = (Data.Value.FCurveVector2D)v.Item2;
					return new FCurveTemplate<float>(
						2,
						new[] { v_.X, v_.Y },
						new[] { 0xff0000ff, 0xff00ff00 },
						new string[] { "X", "Y", "Z" },
						0,
						v_,
						v.Item1,
						window,
						converter,
						float.MinValue,
						float.MaxValue);
				}
				else if (v.Item2 is Data.Value.FCurveVector3D)
				{
					Func<float, float> converter = (float pre) => { return pre; };

					var v_ = (Data.Value.FCurveVector3D)v.Item2;
					return new FCurveTemplate<float>(
						3, 
						new[] { v_.X, v_.Y, v_.Z}, 
						new[] { 0xff0000ff, 0xff00ff00, 0xffff0000 },
						new string[] { "X", "Y", "Z" },
						0,
						v_,
						v.Item1, 
						window,
						converter,
						float.MinValue,
						float.MaxValue);
				}
				else if (v.Item2 is Data.Value.FCurveColorRGBA)
				{
					Func<float, byte> converter = (float pre) => {
						return (byte)Math.Min(255, Math.Max(0, pre));
					};

					var v_ = (Data.Value.FCurveColorRGBA)v.Item2;
					return new FCurveTemplate<byte>(
						4,
						new[] { v_.R, v_.G, v_.B, v_.A },
						new[] { 0xff0000ff, 0xff00ff00, 0xffff0000, 0xffaaaaaa },
						new string[] { "R", "G", "B", "A" },
						255.0f,
						v_,
						v.Item1,
						window,
						converter,
						0,
						255);
				}
				
				return null;
			}

			public FCurve()
			{
			}

			public abstract void GetRange(out float value_min, out float value_max);

			public virtual void OnAdded() {}

			public virtual void OnRemoved() {}

			public virtual void Move(float x, float y, int changedType, Data.Value.IFCurve except) { }

			public virtual void ShrinkAnchors() { }

			public virtual void EnlargeAnchors() { }

			public virtual void Unselect() { }

			public virtual void UpdateTree() { }

			public virtual void UpdateGraph(ref bool canControl) { }

			public virtual void Hide() { }

			public virtual void Commit() { }

			public virtual bool IsDirtied() { return false; }
		}

		class FCurveTemplate<T> : FCurve where T : struct, IComparable<T>, IEquatable<T>
		{
			FCurveProperty[] properties = new FCurveProperty[3];
			Data.Value.FCurve<T>[] fcurves = new Data.Value.FCurve<T>[3];
			int[] ids = new int[3];
			string[] names = null;

			FCurves window = null;
			Func<float, T> converter = null;
			float v_min;
			float v_max;
			float defaultValue = 0;

			public object Value { get; private set; }

			public FCurveTemplate(int length, FCurve<T>[] fcurves, uint[] colors, string[] names, float defaultValue, object value, string name, FCurves window, Func<float, T> converter, float v_min, float v_max)
			{
				Name = name;
				Value = value;
				this.defaultValue = defaultValue;
				this.window = window;
				this.converter = converter;
				this.v_max = v_max;
				this.v_min = v_min;

				properties = new FCurveProperty[length];
				ids = new int[length];

				for(int i = 0; i < length; i++)
				{
					properties[i] = new FCurveProperty();
					ids[i] = Manager.GetUniqueID();
					properties[i].Color = colors[i];
				}

				this.names = names;
				this.fcurves = fcurves;
			}

			public override void GetRange(out float value_min, out float value_max)
			{
				value_min = float.MaxValue;
				value_max = float.MinValue;

				foreach(var prop in properties)
				{
					if (!prop.IsShown) continue;

					for(int i = 0; i < prop.KVSelected.Length - 1; i++)
					{
						value_min = Math.Min(prop.Values[i], value_min);
						value_max = Math.Max(prop.Values[i], value_max);
						value_min = Math.Min(prop.LeftValues[i], value_min);
						value_max = Math.Max(prop.LeftValues[i], value_max);
						value_min = Math.Min(prop.RightValues[i], value_min);
						value_max = Math.Max(prop.RightValues[i], value_max);
					}
				}

				/*
				if (value_min == float.MaxValue || value_max == float.MinValue)
				{
					// todo set from outer
					if(defaultValue == 255)
					{
						value_min = 0;
						value_max = 255;
					}
					else
					{
						value_min = -0;
						value_max = 0;
					}
				}
				*/
			}

			public override object GetValueAsObject()
			{
				return Value;
			}

			public override Tuple35<Data.Value.IFCurve, FCurveProperty> GetSelectedFCurve()
			{
				int count = properties.Count(_ => _.Selected);
				if (count != 1) return new Tuple35<Data.Value.IFCurve, FCurveProperty>(null, null);

				for(int i = 0; i < properties.Length; i++)
				{
					if(properties[i].Selected)
					{
						return new Tuple35<Data.Value.IFCurve, FCurveProperty>(fcurves[i], properties[i]);
					}
				}

				return new Tuple35<Data.Value.IFCurve, FCurveProperty>(null, null);
			}

			public override void UpdateTree()
			{

				for(int i = 0; i < properties.Length; i++)
				{
					var value = this.fcurves[i].GetValue(Manager.Viewer.Current);

					if (Manager.NativeManager.Selectable(Name + " : " + names[i] + " (" + value + ")", properties[i].IsShown, swig.SelectableFlags.AllowDoubleClick))
					{
						if(Manager.NativeManager.IsMouseDoubleClicked(0))
						{
							properties[i].IsShown = true;

							if(this.fcurves[i].Keys.Count() > 0)
							{
								var max = this.fcurves[i].Keys.Max(_ => _.ValueAsFloat);
								var min = this.fcurves[i].Keys.Min(_ => _.ValueAsFloat);
								window.autoZoomRangeMax = max + 10;
								window.autoZoomRangeMin = min - 10;
							}
							else
							{
								window.autoZoomRangeMax = defaultValue + 10;
								window.autoZoomRangeMin = -10;
							}
						}
						else
						{
							if (Manager.NativeManager.IsKeyDown(LEFT_SHIFT) || Manager.NativeManager.IsKeyDown(RIGHT_SHIFT))
							{
								properties[i].IsShown = !properties[i].IsShown;
							}
							else
							{
								window.HideAll();
								properties[i].IsShown = true;
							}
						}
					}
				}
			}

			public override void UpdateGraph(ref bool canControl)
			{
				for(int i = 0; i < properties.Length; i++)
				{
					if (!properties[i].IsShown) continue;

					if(!properties[i].IsDirtied)
					{
						properties[i].CopyValuesFromDataIfSizeDifferent(fcurves[i]);
					}

					int newCount = -1;
					float movedX = 0;
					float movedY = 0;
					int changedType = 0;

					bool isSelected = false;

					if (Manager.NativeManager.FCurve(
						ids[i],
						properties[i].Keys,
						properties[i].Values,
						properties[i].LeftKeys,
						properties[i].LeftValues,
						properties[i].RightKeys,
						properties[i].RightValues,
						properties[i].Interpolations,
						(swig.FCurveEdgeType)properties[i].StartEdge,
						(swig.FCurveEdgeType)properties[i].EndEdge,
						properties[i].KVSelected,
						properties[i].Keys.Length - 1,
						defaultValue,
						false,
						canControl,
						properties[i].Color,
						properties[i].Selected,
						v_min,
						v_max,
						ref newCount,
						ref isSelected,
						ref movedX,
						ref movedY,
						ref changedType))
					{
						canControl = false;

						if (movedX != 0 || movedY != 0)
						{
							window.Move(movedX, movedY, changedType, fcurves[i]);
							properties[i].IsDirtied = true;
						}

						if (isSelected)
						{
							if (Manager.NativeManager.IsKeyDown(LEFT_SHIFT) || Manager.NativeManager.IsKeyDown(RIGHT_SHIFT))
							{
								properties[i].Selected = true;
							}
							else
							{
								window.UnselectAll();
								properties[i].Selected = true;
							}
						}
						
						if(properties[i].Keys.Length - 1 != newCount && newCount != -1)
						{
							if(properties[i].Keys.Length == newCount)
							{
								properties[i].Keys = properties[i].Keys.Concat(new[] { 0.0f }).ToArray();
								properties[i].Values = properties[i].Values.Concat(new[] { 0.0f }).ToArray();
								properties[i].LeftKeys = properties[i].LeftKeys.Concat(new[] { 0.0f }).ToArray();
								properties[i].LeftValues = properties[i].LeftValues.Concat(new[] { 0.0f }).ToArray();
								properties[i].RightKeys = properties[i].RightKeys.Concat(new[] { 0.0f }).ToArray();
								properties[i].RightValues = properties[i].RightValues.Concat(new[] { 0.0f }).ToArray();
								properties[i].Interpolations = properties[i].Interpolations.Concat(new[] { 0 }).ToArray();
							}
							else
							{
								properties[i].Keys = properties[i].Keys.Take(properties[i].Keys.Length -1).ToArray();
								properties[i].Values = properties[i].Values.Take(properties[i].Values.Length - 1).ToArray();
								properties[i].LeftKeys = properties[i].LeftKeys.Take(properties[i].LeftKeys.Length - 1).ToArray();
								properties[i].LeftValues = properties[i].LeftValues.Take(properties[i].LeftValues.Length - 1).ToArray();
								properties[i].RightKeys = properties[i].RightKeys.Take(properties[i].RightKeys.Length - 1).ToArray();
								properties[i].RightValues = properties[i].RightValues.Take(properties[i].RightValues.Length - 1).ToArray();
								properties[i].Interpolations = properties[i].Interpolations.Take(properties[i].Interpolations.Length - 1).ToArray();
							}

							properties[i].IsDirtied = true;
						}

						for(int k = 0; k < properties[i].Values.Length; k++)
						{
							if (properties[i].Values[k] < v_min) properties[i].Values[k] = v_min;
							if (properties[i].Values[k] > v_max) properties[i].Values[k] = v_max;
						}
					}
				}
			}

			public override void Move(float x, float y, int changedType, IFCurve except)
			{
				for (int j = 0; j < properties.Length; j++)
				{
					if (fcurves[j] == except) continue;
					if (!properties[j].Selected) continue;

					properties[j].IsDirtied = true;

					for (int k = 0; k < properties[j].Keys.Length - 1; k++)
					{
						if (properties[j].KVSelected[k] == 0) continue;

						if (changedType == 0)
						{
							properties[j].Keys[k] += x;
							properties[j].Values[k] += y;
							properties[j].LeftKeys[k] += x;
							properties[j].LeftValues[k] += y;
							properties[j].RightKeys[k] += x;
							properties[j].RightValues[k] += y;
						}
						if (changedType == 1)
						{
							properties[j].LeftKeys[k] += x;
							properties[j].LeftValues[k] += y;
						}
						if (changedType == 2)
						{
							properties[j].RightKeys[k] += x;
							properties[j].RightValues[k] += y;
						}

						if (properties[j].Values[k] < v_min) properties[j].Values[k] = v_min;
						if (properties[j].Values[k] > v_max) properties[j].Values[k] = v_max;
					}


					List<Tuple<float, int>> kis = new List<Tuple<float, int>>();

					for (int i = 0; i < properties[j].Keys.Length - 1; i++)
					{
						Tuple<float, int> ki = Tuple.Create(properties[j].Keys[i], i);

						kis.Add(ki);
					}

					kis = kis.OrderBy(_ => _.Item1).ToList();

					var temp_k = properties[j].Keys.ToArray();
					var temp_v = properties[j].Values.ToArray();
					var temp_lk = properties[j].LeftKeys.ToArray();
					var temp_lv = properties[j].LeftValues.ToArray();
					var temp_rk = properties[j].RightKeys.ToArray();
					var temp_rv = properties[j].RightValues.ToArray();
					var temp_in = properties[j].Interpolations.ToArray();

					for (int k = 0; k < properties[j].Keys.Length - 1; k++)
					{
						properties[j].Keys[k] = temp_k[kis[k].Item2];
						properties[j].Values[k] = temp_v[kis[k].Item2];
						properties[j].LeftKeys[k] = temp_lk[kis[k].Item2];
						properties[j].LeftValues[k] = temp_lv[kis[k].Item2];
						properties[j].RightKeys[k] = temp_rk[kis[k].Item2];
						properties[j].RightValues[k] = temp_rv[kis[k].Item2];
						properties[j].Interpolations[k] = temp_in[kis[k].Item2];
					}
				}
			}

			public override void ShrinkAnchors()
			{
				foreach (var prop in properties)
				{
					if (!prop.IsShown) continue;
					if (!prop.Selected) continue;

					for (int i = 0; i < prop.KVSelected.Length - 1; i++)
					{
						if (prop.KVSelected[i] == 0) continue;

						prop.LeftKeys[i] = prop.Keys[i];
						prop.LeftValues[i] = prop.Values[i];

						prop.RightKeys[i] = prop.Keys[i];
						prop.RightValues[i] = prop.Values[i];

						prop.IsDirtied = true;
					}
				}
			}

			public override void EnlargeAnchors()
			{
				foreach(var prop in properties)
				{
					if (!prop.IsShown) continue;
					if (!prop.Selected) continue;

					for (int i = 0; i < prop.KVSelected.Length - 1; i++)
					{
						if (prop.KVSelected[i] == 0) continue;

						var d = 0.0f;
						var length_pre = 10.0f;
						var length_next = 10.0f;

						if (prop.KVSelected.Length -1 == 1)
						{
							d = 0.0f;
						}
						else if (i == 0)
						{
							var dx = prop.Keys[i + 1] - prop.Keys[i + 0];
							var dy = prop.Values[i + 1] - prop.Values[i + 0];
							d = dy / dx;

							length_next = Math.Min(length_next, dx / 2.0f);
						}
						else if (i == prop.KVSelected.Length - 2)
						{
							var dx = prop.Keys[i + 0] - prop.Keys[i - 1];
							var dy = prop.Values[i + 0] - prop.Values[i - 1];
							d = dy / dx;

							length_pre = Math.Min(length_pre, dx / 2.0f);
						}
						else
						{
							var dx = prop.Keys[i + 1] - prop.Keys[i - 1];
							var dy = prop.Values[i + 1] - prop.Values[i - 1];
							d = dy / dx;

							length_next = Math.Min(length_next, (prop.Keys[i + 1] - prop.Keys[i + 0]) / 2.0f);
							length_pre = Math.Min(length_next, (prop.Keys[i + 0] - prop.Keys[i - 1]) / 2.0f);
						}

						prop.LeftKeys[i] = prop.Keys[i] - length_pre;
						prop.LeftValues[i] = prop.Values[i] - d * length_pre;

						prop.RightKeys[i] = prop.Keys[i] + length_next;
						prop.RightValues[i] = prop.Values[i] + d * length_next;

						prop.IsDirtied = true;
					}
				}
			}

			public override void OnAdded()
			{
				fcurves[0].OnChanged += OnChanged_1;
				if (fcurves.Length >= 2) fcurves[1].OnChanged += OnChanged_2;
				if (fcurves.Length >= 3) fcurves[2].OnChanged += OnChanged_3;
				if (fcurves.Length >= 4) fcurves[3].OnChanged += OnChanged_4;
			}

			public override void OnRemoved()
			{
				fcurves[0].OnChanged -= OnChanged_1;
				if (fcurves.Length >= 2) fcurves[1].OnChanged -= OnChanged_2;
				if (fcurves.Length >= 3) fcurves[2].OnChanged -= OnChanged_3;
				if (fcurves.Length >= 4) fcurves[3].OnChanged -= OnChanged_4;
			}

			void OnChanged_1(object sender, ChangedValueEventArgs e)
			{
				OnChanged(0);
			}

			void OnChanged_2(object sender, ChangedValueEventArgs e)
			{
				OnChanged(1);
			}

			void OnChanged_3(object sender, ChangedValueEventArgs e)
			{
				OnChanged(2);
			}

			void OnChanged_4(object sender, ChangedValueEventArgs e)
			{
				OnChanged(3);
			}

			void OnChanged(int i)
			{
				properties[i].Keys = new float[0];
				properties[i].Values = new float[0];
				properties[i].LeftKeys = new float[0];
				properties[i].LeftValues = new float[0];
				properties[i].RightKeys = new float[0];
				properties[i].RightValues = new float[0];
				properties[i].Interpolations = new int[0];
				//properties[i].KVSelected = new byte[0];
				properties[i].CopyValuesFromDataIfSizeDifferent(fcurves[i]);
			}

			public override void Unselect()
			{
				foreach(var prop in properties)
				{
					prop.Selected = false;
				}
			}

			public override void Hide()
			{
				foreach (var prop in properties)
				{
					prop.IsShown = false;
				}
			}

			public override void Commit()
			{
				for(int i = 0; i < properties.Length; i++)
				{
					if (!properties[i].IsDirtied) continue;
					
					var keys = new List<Data.Value.FCurveKey<T>>();

					// Change value format into int
					for (int j = 0; j < properties[i].Keys.Length - 1; j++)
					{
						properties[i].Keys[j] = (int)Math.Round(properties[i].Keys[j]);
					}

					// Clamp
					for (int j = 1; j < properties[i].Keys.Length - 2; j++)
					{
						var new_left = Math.Max(properties[i].LeftKeys[j], properties[i].Keys[j - 1]);
						var new_right = Math.Min(properties[i].RightKeys[j], properties[i].Keys[j + 1]);
						properties[i].LeftKeys[j] = new_left;
						properties[i].RightKeys[j] = new_right;
					}

					for (int j = 0; j < properties[i].Keys.Length - 1; j++)
					{
						var v = properties[i].Values[j];
						var v_ = converter(v);

						Data.Value.FCurveKey<T> key = new FCurveKey<T>(
							(int)properties[i].Keys[j],
							v_);

						key.SetLeftDirectly(
							properties[i].LeftKeys[j],
							properties[i].LeftValues[j]);

						key.SetRightDirectly(
							properties[i].RightKeys[j],
							properties[i].RightValues[j]);

						keys.Add(key);
					}

					fcurves[i].SetKeys(keys.ToArray());

					properties[i].IsDirtied = false;
				}
			}

			public override bool IsDirtied()
			{
				for (int i = 0; i < properties.Length; i++)
				{
					if (properties[i].IsDirtied) return true;
				}

				return false;
			}
		}

		class FCurveProperty
		{
			public UInt32 Color = 0;

			public bool IsDirtied = false;
			public bool Selected = false;
			public bool IsShown = false;

			public float[] Keys = new float[0];
			public float[] Values = new float[0];

			public float[] LeftKeys = new float[0];
			public float[] LeftValues = new float[0];

			public float[] RightKeys = new float[0];
			public float[] RightValues = new float[0];

			public byte[] KVSelected = new byte[0];

			public int[] Interpolations = new int[0];

			public Data.Value.FCurveEdge StartEdge = Data.Value.FCurveEdge.Constant;

			public Data.Value.FCurveEdge EndEdge = Data.Value.FCurveEdge.Constant;

			public void SolveContradiction()
			{
				List<Tuple<float, int>> kis = new List<Tuple<float, int>>();

				for (int i = 0; i < Keys.Length - 1; i++)
				{
					Tuple<float, int> ki = Tuple.Create(Keys[i], i);

					kis.Add(ki);
				}

				kis = kis.OrderBy(_ => _.Item1).ToList();

				var temp_k = Keys.ToArray();
				var temp_v = Values.ToArray();
				var temp_lk = LeftKeys.ToArray();
				var temp_lv = LeftValues.ToArray();
				var temp_rk = RightKeys.ToArray();
				var temp_rv = RightValues.ToArray();
				var temp_in = Interpolations.ToArray();
				var temp_sl = KVSelected.ToArray();

				for (int k = 0; k < Keys.Length - 1; k++)
				{
					Keys[k] = temp_k[kis[k].Item2];
					Values[k] = temp_v[kis[k].Item2];
					LeftKeys[k] = temp_lk[kis[k].Item2];
					LeftValues[k] = temp_lv[kis[k].Item2];
					RightKeys[k] = temp_rk[kis[k].Item2];
					RightValues[k] = temp_rv[kis[k].Item2];
					Interpolations[k] = temp_in[kis[k].Item2];
					KVSelected[k] = temp_sl[kis[k].Item2];
				}

				for (int k = 0; k < Keys.Length - 1; k++)
				{
					Clip(k);
				}
			}

			/// <summary>
			/// Clip left and right values
			/// </summary>
			/// <param name="index"></param>
			public void Clip(int index)
			{
				if(index > 0)
				{
					LeftKeys[index] = System.Math.Max(LeftKeys[index], Keys[index - 1]);
				}

				if (index < KVSelected.Length - 2)
				{
					RightKeys[index] = System.Math.Min(RightKeys[index], Keys[index + 1]);
				}

				LeftKeys[index] = System.Math.Min(LeftKeys[index], Keys[index]);
				RightKeys[index] = System.Math.Max(RightKeys[index], Keys[index]);
			}

			public int GetSelectedIndex()
			{
				if(KVSelected.Take(KVSelected.Count() - 1).Count(_ => _ > 0) != 1)
				{
					return -1;
				}

				for(int i = 0; i < KVSelected.Count() - 1; i++)
				{
					if (KVSelected[i] > 0) return i;
				}

				return -1;
			}

			public void CopyValuesFromDataIfSizeDifferent<T>(Data.Value.FCurve<T> fcurve) where T : struct, IComparable<T>, IEquatable<T>
			{
				var plength = fcurve.Keys.Count() + 1;

				if (Keys.Length != plength)
				{
					var keyFrames = fcurve.Keys.ToArray();
					Keys = keyFrames.Select(_ => (float)_.Frame).Concat(new float[] { 0.0f }).ToArray();
				}

				if (Values.Length != plength)
				{
					var keyFrames = fcurve.Keys.ToArray();
					Values = keyFrames.Select(_ => _.ValueAsFloat).Concat(new float[] { 0.0f }).ToArray();
				}

				if (LeftKeys.Length != plength)
				{
					var keyFrames = fcurve.Keys.ToArray();
					LeftKeys = keyFrames.Select(_ => _.LeftX).Concat(new float[] { 0.0f }).ToArray();
				}

				if (LeftValues.Length != plength)
				{
					var keyFrames = fcurve.Keys.ToArray();
					LeftValues = keyFrames.Select(_ => _.LeftY).Concat(new float[] { 0.0f }).ToArray();
				}

				if (RightKeys.Length != plength)
				{
					var keyFrames = fcurve.Keys.ToArray();
					RightKeys = keyFrames.Select(_ => _.RightX).Concat(new float[] { 0.0f }).ToArray();
				}

				if (RightValues.Length != plength)
				{
					var keyFrames = fcurve.Keys.ToArray();
					RightValues = keyFrames.Select(_ => _.RightY).Concat(new float[] { 0.0f }).ToArray();
				}

				if (KVSelected.Length != plength)
				{
					var new_selected = new byte[plength];
					//KVSelected.CopyTo(new_selected, 0);
					KVSelected = new_selected;
				}

				if (Interpolations.Length != plength)
				{
					var keyFrames = fcurve.Keys.ToArray();

					var new_interpolations = new int[keyFrames.Length + 1];
					Interpolations.CopyTo(new_interpolations, 0);
					Interpolations = new_interpolations;
				}
			}
		}
	}
}
