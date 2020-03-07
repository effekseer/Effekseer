using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.Data.Value;

namespace Effekseer.GUI.Dock
{
	public class FCurveCopiedData
	{
		public class Point
		{
			public float Key;
			public float Value;
			public float LeftKey;
			public float LeftValue;
			public float RightKey;
			public float RightValue;
			public int Interpolation;
		}

		public class Curve
		{
			public int Index;
			public int ID;
			public List<Point> Points = new List<Point>();
		}

		public List<Curve> Curves = new List<Curve>();
	}

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
		string timelineMode_Name = Resources.GetString("FCurve_TimelineMode_Name");
		string timelineMode_Desc = Resources.GetString("FCurve_TimelineMode_Desc");
		string offset_min_text = Resources.GetString("Min");
		string offset_max_text = Resources.GetString("Max");

		Utl.ParameterTreeNode paramaterTreeNode = null;

		TreeNode treeNodes = null;
		List<FCurve> flattenFcurves = new List<FCurve>();

		bool loading = false;

		bool canCurveControlPre = true;
		bool canCurveControl = true;
		bool canControl = true;

		Component.Enum startCurve = new Component.Enum();
		Component.Enum endCurve = new Component.Enum();
		Component.Enum type = new Component.Enum();
		Component.Enum timeline = new Component.Enum();

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
			startCurve.InternalLabel = start_text + "##Start";

			endCurve.Initialize(typeof(Data.Value.FCurveEdge));
			endCurve.InternalLabel = end_text + "##End";

			type.Initialize(typeof(Data.Value.FCurveInterpolation));
			type.InternalLabel = type_text + "##Type";

			timeline.Initialize(typeof(Data.Value.FCurveTimelineMode));
			timeline.InternalLabel = timelineMode_Name + "##Timeline";

			OnChanged();

			Icon = Images.GetIcon("PanelFCurve");
			TabToolTip = Resources.GetString("FCurves");

			//NoPadding = true;
			NoScrollBar = true;

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
			float dpiScale = Manager.DpiScale;
			var contentSize = Manager.NativeManager.GetContentRegionAvail();

			if (Component.Functions.CanShowTip())
			{
				Manager.NativeManager.SetTooltip(Resources.GetString("FCurve_TimelineMode_Desc"));
			}

			Manager.NativeManager.Columns(3);

			if (isFirstUpdate)
			{
				Manager.NativeManager.SetColumnWidth(0, contentSize.X * 0.24f);
				Manager.NativeManager.SetColumnWidth(1, contentSize.X * 0.5f);
				Manager.NativeManager.SetColumnWidth(2, contentSize.X * 0.26f);
			}

			{
				swig.Vec2 size = new swig.Vec2(24 * dpiScale, 24 * dpiScale);

				if (Manager.NativeManager.ImageButton(Images.GetIcon("EnlargeAnchor"), size.X, size.Y))
				{
					EnlargeAnchors();
				}

				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.SetTooltip(Resources.GetString("EnlargeAnchor") + "\n" + Resources.GetString("EnlargeAnchor_Desc"));
				}

				Manager.NativeManager.SameLine();

				if (Manager.NativeManager.ImageButton(Images.GetIcon("ShrinkAnchor"), size.X, size.Y))
				{
					ShrinkAnchors();
				}

				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.SetTooltip(Resources.GetString("ShrinkAnchor") + "\n" + Resources.GetString("ShrinkAnchor_Desc"));
				}

				Manager.NativeManager.SameLine();

				if (Manager.NativeManager.ImageButton(Images.Icons["Copy"], size.X, size.Y))
				{
					Copy();
				}

				if (Component.Functions.CanShowTip())
				{
					Manager.NativeManager.SetTooltip(Resources.GetString("FCurve_Copy_Desc"));
				}

				Manager.NativeManager.SameLine();

				if (Manager.NativeManager.ImageButton(Images.Icons["Paste"], size.X, size.Y))
				{
					Paste();
				}

				if (Component.Functions.CanShowTip())
				{
					Manager.NativeManager.SetTooltip(Resources.GetString("FCurve_Paste_Desc"));
				}

				Manager.NativeManager.SameLine();

				Manager.NativeManager.Button("?");
				
				// if (Component.Functions.CanShowTip())
				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.SetTooltip(Resources.GetString("FCurveCtrl_Desc"));
				}
			}

			// hot key
			if (IsDockActive() && canControl)
			{
				int delete_num = 261;

				// Delete points
				if(Manager.NativeManager.IsKeyDown(delete_num))
				{
					DeleteSelectedPoints();
					canControl = false;
				}
			}

			Manager.NativeManager.BeginChild("##FCurveTree", new swig.Vec2());
			if (treeNodes != null)
			{
				UpdateTreeNode(treeNodes);
			}
			Manager.NativeManager.EndChild();

			Manager.NativeManager.NextColumn();

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

			var scale = new swig.Vec2(12, 4);

			if (Manager.NativeManager.BeginFCurve(1, graphSize, Manager.Viewer.Current, scale, autoZoomRangeMin, autoZoomRangeMax))
			{
				UpdateGraph(treeNodes);
			}

			// Reset area
			autoZoomRangeMin = float.MaxValue;
			autoZoomRangeMax = float.MinValue;

			if(canControl)
			{
				Manager.NativeManager.StartSelectingAreaFCurve();
			}

			Manager.NativeManager.EndFCurve();

			Manager.NativeManager.NextColumn();

			UpdateDetails();

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

			string nodeName = treeNode.ParamTreeNode.Node.Name.Value;
			if (Manager.NativeManager.TreeNodeEx(nodeName + treeNode.ID, flag))
			{
				foreach (var fcurve in treeNode.FCurves)
				{
					fcurve.UpdateTree(nodeName);
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

		void UpdateDetails()
		{
			var selected = GetSelectedFCurve();

			var invalidValue = "/";
			var selectedInd = selected.Item1 != null ? selected.Item2.GetSelectedIndex() : -1;

			Manager.NativeManager.BeginChild("##FCurveDetails", new swig.Vec2());
			Manager.NativeManager.PushItemWidth(80 * Manager.DpiScale);

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

			if (selectedInd >= 0)
			{
				if (Manager.NativeManager.BeginCombo(type_text, type.FieldNames[selected.Item2.Interpolations[selectedInd]], swig.ComboFlags.None))
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

			Manager.NativeManager.Separator();

			// Start curve
			if (selected.Item1 != null)
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

			if (selected.Item1 == null)
			{
				startCurve.SetBinding(null);
				endCurve.SetBinding(null);
			}

			var fcurveGroups = flattenFcurves.Where(_ => _.Properties.Any(__ => __.IsShown)).ToArray();

			if (fcurveGroups.Any())
			{
				// TODO : implement multi select
				timeline.SetBinding(fcurveGroups.First().GetTimeLineType());
				timeline.Update();
			}
			else
			{
				Manager.NativeManager.InputText(timelineMode_Name + "##Timeline", invalidValue, swig.InputTextFlags.ReadOnly);
				timeline.SetBinding(null);
			}

			Manager.NativeManager.PopItemWidth();
			Manager.NativeManager.EndChild();
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
			flattenFcurves.Clear();

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

			// collect fcurves
			{
				Action<TreeNode> recurse = null;

				recurse = (t) =>
				{
					foreach (var fcurve in t.FCurves)
					{
						flattenFcurves.Add(fcurve);
					}

					foreach (var c in t.Children)
					{
						recurse(c);
					}
				};

				recurse(treeNodes);
			}
		}

		public void SelectFCurve(object o)
		{
			Action<TreeNode> recurse = null;

			recurse = (t) =>
			{
				foreach (var fcurve in t.FCurves)
				{
					if(o == fcurve.GetValueAsObject())
					{
						foreach(var prop in fcurve.Properties)
						{
							prop.Selected = true;
							prop.IsShown = true;
						}
					}
				}

				foreach (var c in t.Children)
				{
					recurse(c);
				}
			};

			recurse(treeNodes);
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

		public void DeleteSelectedPoints()
		{
			Action<TreeNode> recurse = null;

			recurse = (t) =>
			{
				foreach (var fcurve in t.FCurves)
				{
					fcurve.DeleteSelectedPoints();
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

		public void Copy()
		{
			var copiedData = CopyAsClass();
			if (copiedData == null)
			{
				return;
			}

			System.Xml.Serialization.XmlSerializer serializer = new System.Xml.Serialization.XmlSerializer(typeof(FCurveCopiedData));
			System.IO.StringWriter writer = new System.IO.StringWriter();
			serializer.Serialize(writer, copiedData);
			Manager.NativeManager.SetClipboardText(writer.ToString());
		}

		public FCurveCopiedData CopyAsClass()
		{
			FCurveCopiedData data = new FCurveCopiedData();

			var flatten = flattenFcurves;

			foreach(var curve in flatten)
			{
				for(int ind = 0; ind < curve.Properties.Length; ind++)
				{
					var prop = curve.Properties[ind];

					if(!prop.KVSelected.Any(_=>_ > 0))
					{
						continue;
					}

					FCurveCopiedData.Curve copiedCurve = new FCurveCopiedData.Curve();

					copiedCurve.Index = ind;
					copiedCurve.ID = curve.ID;
					
					for(int i = 0; i < prop.KVSelected.Length; i++)
					{
						if(prop.KVSelected[i] > 0)
						{
							var p = new FCurveCopiedData.Point();
							p.Key = prop.Keys[i];
							p.Value = prop.Values[i];
							p.LeftKey = prop.LeftKeys[i];
							p.LeftValue = prop.LeftValues[i];
							p.RightKey = prop.RightKeys[i];
							p.RightValue = prop.RightValues[i];
							p.Interpolation = prop.Interpolations[i];
							copiedCurve.Points.Add(p);
						}
					}

					data.Curves.Add(copiedCurve);
				}
			}

			if (data.Curves.Count == 0)
			{
				return null;
			}

			var xmin = data.Curves.SelectMany(_ => _.Points).Min(_ => _.Key);

			foreach(var curve in data.Curves)
			{
				foreach(var p in curve.Points)
				{
					p.Key -= xmin;
					p.LeftKey -= xmin;
					p.RightKey -= xmin;
				}
			}

			return data;
		}

		public void Paste()
		{
			try
			{
				System.Xml.Serialization.XmlSerializer serializer = new System.Xml.Serialization.XmlSerializer(typeof(FCurveCopiedData));
				FCurveCopiedData data = serializer.Deserialize(new System.IO.StringReader(Manager.NativeManager.GetClipboardText())) as FCurveCopiedData;
				Paste(data);
			}
			catch
			{
				return;
			}
		}

		public void Paste(FCurveCopiedData data)
		{
			var flatten = flattenFcurves.ToArray();
			var offsetTime = Manager.Viewer.Current;

			foreach (var curve in flatten)
			{
				for (int ind = 0; ind < curve.Properties.Length; ind++)
				{
					var prop = curve.Properties[ind];

					if (!prop.IsShown) continue;
					if (!prop.Selected) continue;

					var candidates = data.Curves.Where(_ => _.Index == ind);

					var copiedCurve = candidates.FirstOrDefault(_ => _.ID == curve.ID);

					if(copiedCurve == null)
					{
						copiedCurve = data.Curves.FirstOrDefault(_ => _.Index == ind);
					}

					if (copiedCurve != null)
					{
						
						for(int i = 0; i < copiedCurve.Points.Count; i++)
						{
							prop.KVSelected = new byte[] { 0 }.Concat(prop.KVSelected).ToArray();
						}

						prop.Keys = copiedCurve.Points.Select(_ => _.Key + offsetTime).Concat(prop.Keys).ToArray();
						prop.Values = copiedCurve.Points.Select(_ => _.Value).Concat(prop.Values).ToArray();
						prop.LeftKeys = copiedCurve.Points.Select(_ => _.LeftKey + offsetTime).Concat(prop.LeftKeys).ToArray();
						prop.LeftValues = copiedCurve.Points.Select(_ => _.LeftValue).Concat(prop.LeftValues).ToArray();
						prop.RightKeys = copiedCurve.Points.Select(_ => _.RightKey + offsetTime).Concat(prop.RightKeys).ToArray();
						prop.RightValues = copiedCurve.Points.Select(_ => _.RightValue).Concat(prop.RightValues).ToArray();
						prop.Interpolations = copiedCurve.Points.Select(_ => _.Interpolation).Concat(prop.Interpolations).ToArray();

						prop.SolveContradiction();
						prop.IsDirtied = true;
					}
				}

				curve.Commit();
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


		interface IFCurveConverter
		{
			IFCurveKey CreateKey(int key, float value, float leftKey, float leftValue, float rightKey, float rightValue);

			void SetKeys(IFCurve fcurve, IFCurveKey[] keys);
		}

		class FloatFCurveConverter : IFCurveConverter
		{
			public IFCurveKey CreateKey(int key, float value, float leftKey, float leftValue, float rightKey, float rightValue)
			{
				var fcurveKey = new FCurveKey<float>(key, value);
				fcurveKey.SetLeftDirectly(leftKey, leftValue);
				fcurveKey.SetRightDirectly(rightKey, rightValue);
				return fcurveKey;
			}

			public void SetKeys(IFCurve fcurve, IFCurveKey[] keys)
			{
				var fc = fcurve as FCurve<float>;
				fc.SetKeys(keys.OfType<FCurveKey<float>>().ToArray());
			}
		}

		class IntFCurveConverter : IFCurveConverter
		{
			public IFCurveKey CreateKey(int key, float value, float leftKey, float leftValue, float rightKey, float rightValue)
			{
				var fcurveKey = new FCurveKey<int>(key, (int)value);
				fcurveKey.SetLeftDirectly(leftKey, leftValue);
				fcurveKey.SetRightDirectly(rightKey, rightValue);
				return fcurveKey;
			}

			public void SetKeys(IFCurve fcurve, IFCurveKey[] keys)
			{
				var fc = fcurve as FCurve<int>;
				fc.SetKeys(keys.OfType<FCurveKey<int>>().ToArray());
			}
		}

		class FCurve
		{
			/// <summary>
			/// next id for copy and paste
			/// </summary>
			static int nextID = 0;

			/// <summary>
			/// for copy and paste
			/// </summary>
			public int ID { get; private set; }
			protected int LEFT_SHIFT = 340;
			protected int RIGHT_SHIFT = 344;
			protected int LEFT_ALT = 342;
			protected int RIGHT_ALT = 346;

			protected FCurveProperty[] properties = null;

			public TreeNode ParentNode { get; set; }

			public string Name { get; protected set; }

			public FCurveProperty[] Properties { get { return properties; } }

			static public FCurve Create(Tuple35<string, object> v, FCurves window)
			{
#if __EFFEKSEER_BUILD_VERSION16__
				if (v.Item2 is Data.Value.FCurveScalar)
				{
					var v_ = (Data.Value.FCurveScalar)v.Item2;
					return new FCurve(
						1,
						new[] { v_.S },
						new[] { 0xffffffff },
						new string[] { "S" },
						0,
						v_,
						v.Item1,
						window,
						new FloatFCurveConverter(),
						v_.S.DefaultValueRangeMin,
						v_.S.DefaultValueRangeMax,
						v_.Timeline);
				}
				else
#endif
				if (v.Item2 is Data.Value.FCurveVector2D)
				{
					var v_ = (Data.Value.FCurveVector2D)v.Item2;
					return new FCurve(
						2,
						new[] { v_.X, v_.Y },
						new[] { 0xff5234ff, 0xff00e682 },
						new string[] { "X", "Y" },
						0,
						v_,
						v.Item1,
						window,
						new FloatFCurveConverter(),
						float.MinValue,
						float.MaxValue,
						v_.Timeline);
				}
				else if (v.Item2 is Data.Value.FCurveVector3D)
				{
					Func<float, float> converter = (float pre) => { return pre; };

					var v_ = (Data.Value.FCurveVector3D)v.Item2;
					return new FCurve(
						3,
						new[] { v_.X, v_.Y, v_.Z },
						new[] { 0xff5234ff, 0xff00e682, 0xffff9028 },
						new string[] { "X", "Y", "Z" },
						0,
						v_,
						v.Item1,
						window,
						new FloatFCurveConverter(),
						float.MinValue,
						float.MaxValue,
						v_.Timeline);
				}
				else if (v.Item2 is Data.Value.FCurveColorRGBA)
				{
					var v_ = (Data.Value.FCurveColorRGBA)v.Item2;
					return new FCurve(
						4,
						new[] { v_.R, v_.G, v_.B, v_.A },
						new[] { 0xff5234ff, 0xff00e682, 0xffff9028, 0xffc0c0c0 },
						new string[] { "R", "G", "B", "A" },
						255.0f,
						v_,
						v.Item1,
						window,
						new IntFCurveConverter(),
						0,
						int.MaxValue,
						v_.Timeline);
				}

				return null;
			}


			Data.Value.IFCurve[] fcurves = null;
			int[] ids = new int[3];
			string[] names = null;
			Data.Value.Enum<FCurveTimelineMode> timelineType = null;

			FCurves window = null;
			IFCurveConverter converter = null;
			float v_min;
			float v_max;
			float defaultValue = 0;

			public object Value { get; private set; }

			public FCurve(int length, IFCurve[] fcurves, uint[] colors, string[] names, float defaultValue, object value, string name, FCurves window, IFCurveConverter converter, float v_min, float v_max, Data.Value.Enum<FCurveTimelineMode> timelineType)
			{
				ID = nextID;
				nextID++;

				Name = name;
				Value = value;
				this.defaultValue = defaultValue;
				this.window = window;
				this.converter = converter;
				this.v_max = v_max;
				this.v_min = v_min;
				this.timelineType = timelineType;

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

			public void GetRange(out float value_min, out float value_max)
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

			public object GetValueAsObject()
			{
				return Value;
			}

			public Tuple35<Data.Value.IFCurve, FCurveProperty> GetSelectedFCurve()
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

			public void UpdateTree(string nodeName)
			{

				for(int i = 0; i < properties.Length; i++)
				{
					var value = this.fcurves[i].GetValue(Manager.Viewer.Current);

					string labelName = Name + " : " + names[i] + " (" + value + ")";
					string labelID = "###FCurveLabel_" + nodeName + "_" + Name + "_" + names[i];
					if (Manager.NativeManager.Selectable(labelName + labelID, properties[i].IsShown, swig.SelectableFlags.AllowDoubleClick))
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

			public void UpdateGraph(ref bool canControl)
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
								properties[i].Selected = !properties[i].Selected;
							}
							else if (Manager.NativeManager.IsKeyDown(LEFT_ALT) || Manager.NativeManager.IsKeyDown(RIGHT_ALT))
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

			public void Move(float x, float y, int changedType, IFCurve except)
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

			public void DeleteSelectedPoints()
			{
				foreach (var prop in properties)
				{
					if (!prop.IsShown) continue;
					if (!prop.Selected) continue;

					for (int i = 0; i < prop.KVSelected.Length - 1;)
					{
						if (prop.KVSelected[i] == 0)
						{
							i++;
							continue;
						}

						prop.LeftKeys = prop.LeftKeys.Where((v, ind) => ind != i).ToArray();
						prop.LeftValues = prop.LeftValues.Where((v, ind) => ind != i).ToArray();
						prop.RightKeys = prop.RightKeys.Where((v, ind) => ind != i).ToArray();
						prop.RightValues = prop.RightValues.Where((v, ind) => ind != i).ToArray();
						prop.Keys = prop.Keys.Where((v, ind) => ind != i).ToArray();
						prop.Values = prop.Values.Where((v, ind) => ind != i).ToArray();
						prop.KVSelected = prop.KVSelected.Where((v, ind) => ind != i).ToArray();

						prop.IsDirtied = true;
					}
				}
			}

			public void ShrinkAnchors()
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

			public void EnlargeAnchors()
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

			public void OnAdded()
			{
				fcurves[0].OnChanged += OnChanged_1;
				if (fcurves.Length >= 2) fcurves[1].OnChanged += OnChanged_2;
				if (fcurves.Length >= 3) fcurves[2].OnChanged += OnChanged_3;
				if (fcurves.Length >= 4) fcurves[3].OnChanged += OnChanged_4;
			}

			public void OnRemoved()
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

			public void Unselect()
			{
				foreach(var prop in properties)
				{
					prop.Selected = false;
				}
			}

			public void Hide()
			{
				foreach (var prop in properties)
				{
					prop.IsShown = false;
				}
			}

			public void Commit()
			{
				Command.CommandManager.StartCollection();

				for(int i = 0; i < properties.Length; i++)
				{
					if (!properties[i].IsDirtied) continue;
					
					var keys = new List<IFCurveKey>();

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
						var key = converter.CreateKey((int)properties[i].Keys[j], v, properties[i].LeftKeys[j], properties[i].LeftValues[j], properties[i].RightKeys[j], properties[i].RightValues[j]);
						keys.Add(key);
					}

					converter.SetKeys(fcurves[i], keys.ToArray());
					properties[i].IsDirtied = false;
				}

				Command.CommandManager.EndCollection();
			}

			public bool IsDirtied()
			{
				for (int i = 0; i < properties.Length; i++)
				{
					if (properties[i].IsDirtied) return true;
				}

				return false;
			}

			public Enum<FCurveTimelineMode> GetTimeLineType()
			{
				return timelineType;
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

			public void CopyValuesFromDataIfSizeDifferent(Data.Value.IFCurve fcurve)
			{
				var plength = fcurve.Keys.Count() + 1;
				var keyFrames = fcurve.Keys.ToArray();

				if (Keys.Length != plength)
				{
					Keys = keyFrames.Select(_ => (float)_.Frame).Concat(new float[] { 0.0f }).ToArray();
				}

				if (Values.Length != plength)
				{
					Values = keyFrames.Select(_ => _.ValueAsFloat).Concat(new float[] { 0.0f }).ToArray();
				}

				if (LeftKeys.Length != plength)
				{
					LeftKeys = keyFrames.Select(_ => _.LeftX).Concat(new float[] { 0.0f }).ToArray();
				}

				if (LeftValues.Length != plength)
				{
					LeftValues = keyFrames.Select(_ => _.LeftY).Concat(new float[] { 0.0f }).ToArray();
				}

				if (RightKeys.Length != plength)
				{
					RightKeys = keyFrames.Select(_ => _.RightX).Concat(new float[] { 0.0f }).ToArray();
				}

				if (RightValues.Length != plength)
				{
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
					var new_interpolations = new int[keyFrames.Length + 1];
					Interpolations.CopyTo(new_interpolations, 0);
					Interpolations = new_interpolations;
				}
			}
		}
	}
}
