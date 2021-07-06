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

	public class FCurves : DockPanel
	{
		const string fCurveMenu = "FCurveMenu";
		const int LEFT_SHIFT = 340;
		const int RIGHT_SHIFT = 344;
		const int LEFT_ALT = 342;
		const int RIGHT_ALT = 346;

		static bool IsShiftDown()
		{
			return Manager.NativeManager.IsKeyDown(LEFT_SHIFT) || Manager.NativeManager.IsKeyDown(RIGHT_SHIFT);
		}

		static bool IsAltDown()
		{
			return Manager.NativeManager.IsKeyDown(LEFT_ALT) || Manager.NativeManager.IsKeyDown(RIGHT_ALT);
		}

		class FCurveMenuContextData
		{
			public swig.Vec2 ClickedPosition;

			public FCurve ClickedFcurve;

			public int ClickedPropIndex;

			public int ClickedPointIndex;
		}

		class Texts
		{
			public string frame = Resources.GetString("Frame");
			public string value = Resources.GetString("Value");
			public string start = Resources.GetString("Start");
			public string end = Resources.GetString("End");
			public string type = Resources.GetString("Complement");
			public string sampling = Resources.GetString("Sampling");
			public string left = Resources.GetString("Left");
			public string right = Resources.GetString("Right");
			public string offset = Resources.GetString("Offset");
			public string timelineMode_Name = Resources.GetString("FCurve_TimelineMode_Name");
			public string timelineMode_Desc = Resources.GetString("FCurve_TimelineMode_Desc");
			public string offsetMin = Resources.GetString("Min");
			public string offsetMax = Resources.GetString("Max");
		}
		Texts texts = new Texts();

		Utl.ParameterTreeNode paramaterTreeNode = null;

		TreeNode treeNodes = null;
		List<FCurve> flattenFcurves = new List<FCurve>();

		bool loading = false;

		bool canCurveControlPre = true;
		bool canCurveControl = true;
		bool canControl = true;
		bool isDetailControlling = false;

		Component.Enum startCurve = new Component.Enum();
		Component.Enum endCurve = new Component.Enum();
		Component.Enum type = new Component.Enum();
		Component.Enum timeline = new Component.Enum();

		swig.Vec2 autoZoomRangeMin = new swig.Vec2(float.MaxValue, float.MaxValue);
		swig.Vec2 autoZoomRangeMax = new swig.Vec2(float.MinValue, float.MinValue);

		bool isFirstUpdate = true;

		FCurveMenuContextData menuContext;

		string textMulti;

		public FCurves()
		{
			Label = Icons.PanelFCurve + Resources.GetString("FCurves") + "###FCurves";

			Command.CommandManager.Changed += OnChanged;
			Core.OnAfterNew += OnChanged;
			Core.OnBeforeLoad += OnBeforeLoad;
			Core.OnAfterLoad += OnAfterLoad;

			startCurve.Initialize(typeof(Data.Value.FCurveEdge));
			startCurve.InternalLabel = texts.start + "##Start";

			endCurve.Initialize(typeof(Data.Value.FCurveEdge));
			endCurve.InternalLabel = texts.end + "##End";

			type.Initialize(typeof(Data.Value.FCurveInterpolation));
			type.InternalLabel = texts.type + "##Type";

			timeline.Initialize(typeof(Data.Value.FCurveTimelineMode));
			timeline.InternalLabel = texts.timelineMode_Name + "##Timeline";

			textMulti = MultiLanguageTextProvider.GetText("MultipleValues");

			OnChanged();

			TabToolTip = Resources.GetString("FCurves");

			NoPadding = true;
			NoScrollBar = true;
		}

		protected override void UpdateInternal()
		{
			canControl = !isDetailControlling;
			isDetailControlling = false;

			float dpiScale = Manager.DpiScale;
			var contentSize = Manager.NativeManager.GetContentRegionAvail();

			Manager.NativeManager.PushItemWidth(-1);

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

			Manager.NativeManager.Spacing();

			// Toolbar
			{
				Manager.NativeManager.PushStyleVar(swig.ImGuiStyleVarFlags.ItemSpacing, new swig.Vec2(4.0f * dpiScale, 0.0f));

				swig.Vec2 size = new swig.Vec2(20 * dpiScale, 20 * dpiScale);

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
					Paste((int)Manager.Viewer.Current, false, true);
				}

				if (Component.Functions.CanShowTip())
				{
					Manager.NativeManager.SetTooltip(Resources.GetString("FCurve_Paste_Desc"));
				}

				Manager.NativeManager.SameLine();

				Manager.NativeManager.Button("?", size.X, size.Y);
				
				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.SetTooltip(Resources.GetString("FCurveCtrl_Desc"));
				}

				Manager.NativeManager.PopStyleVar();
			}

			// hot key
			if (IsDockActive() && !Manager.NativeManager.IsAnyItemActive() && canControl)
			{
				int delete_num = 261;

				if (swig.GUIManager.IsMacOSX())
				{
					delete_num = 259;
				}

				// Delete points
				if (Manager.NativeManager.IsKeyDown(delete_num))
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

			var graphSize = Manager.NativeManager.GetContentRegionAvail();
			graphSize.X = Math.Max(graphSize.X, 32);
			graphSize.Y = Math.Max(graphSize.Y, 32);

			var scale = new swig.Vec2(12, 4);

			if (Manager.NativeManager.BeginFCurve(1, graphSize, Manager.Viewer.Current, scale, autoZoomRangeMin, autoZoomRangeMax))
			{
				var clicked = Manager.NativeManager.IsMouseClicked(0, false) && Manager.NativeManager.IsWindowHovered();
				var panning = Manager.NativeManager.IsFCurvePanning();

				UpdateGraph(treeNodes, ref canControl);

				if (!IsShiftDown() && !IsAltDown())
				{
					if (canControl && clicked)
					{
						UnselectPoints();
					}
				}

				if (canControl && Manager.NativeManager.IsWindowHovered())
				{
					Manager.NativeManager.StartSelectingAreaFCurve();

					if (Manager.NativeManager.IsMouseReleased(1) && !panning)
					{
						var currentPoint = Manager.NativeManager.GetCurrentFCurveFieldPosition();
						menuContext = new FCurveMenuContextData();

						if (IsHovered(treeNodes, out var selectedFcurve, out var selectedInd, out var pointInd))
						{
							menuContext.ClickedPosition = currentPoint;
							menuContext.ClickedFcurve = selectedFcurve;
							menuContext.ClickedPropIndex = selectedInd;
							menuContext.ClickedPointIndex = pointInd;
						}

						Manager.NativeManager.OpenPopup(fCurveMenu);
					}
				}

				if(UpdateMenu())
				{
					canControl = false;
				}

				Manager.NativeManager.EndFCurve();
			}


			// Reset area
			autoZoomRangeMin.X = float.MaxValue;
			autoZoomRangeMax.X = float.MinValue;
			autoZoomRangeMin.Y = float.MaxValue;
			autoZoomRangeMax.Y = float.MinValue;

			Manager.NativeManager.NextColumn();

			UpdateDetails();

			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.Separator();

			CheckAndApplyUpdate(treeNodes);

			isFirstUpdate = false;
		}

		bool UpdateMenu()
		{
			var changed = false;

			if (Manager.NativeManager.BeginPopup(fCurveMenu))
			{
				if (menuContext.ClickedFcurve != null)
				{
					if (menuContext.ClickedPointIndex >= 0)
					{
						if (Manager.NativeManager.Selectable(MultiLanguageTextProvider.GetText("FCurve_DeletePoint")))
						{
							if(menuContext.ClickedFcurve.RemovePoint(menuContext.ClickedPropIndex, menuContext.ClickedPosition))
							{
								changed = true;
							}
						}
					}
					else
					{
						if (Manager.NativeManager.Selectable(MultiLanguageTextProvider.GetText("FCurve_AddPoint")))
						{
							if(menuContext.ClickedFcurve.AddPoint(menuContext.ClickedPropIndex, menuContext.ClickedPosition))
							{
								changed = true;
							}
						}
					}
				}

				if (Manager.NativeManager.Selectable(MultiLanguageTextProvider.GetText("FCurve_PaseteOnZero")))
				{
					Paste(0, false, false);
				}

				if (Manager.NativeManager.Selectable(MultiLanguageTextProvider.GetText("FCurve_PasteOnCurrentTime")))
				{
					Paste((int)Manager.Viewer.Current, false, true);
				}

				if (Manager.NativeManager.Selectable(MultiLanguageTextProvider.GetText("FCurve_PasteOnCursor")))
				{
					Paste((int)menuContext.ClickedPosition.X, false, true);
				}

				if (Manager.NativeManager.Selectable(MultiLanguageTextProvider.GetText("FCurve_Overwrite")))
				{
					Paste(0, true, false);
				}

				if (Manager.NativeManager.Selectable(MultiLanguageTextProvider.GetText("FCurve_AlignKey")))
				{
					AlignKeyValue(true, false);
				}

				if (Manager.NativeManager.Selectable(MultiLanguageTextProvider.GetText("FCurve_AlignValue")))
				{
					AlignKeyValue(false, true);
				}

				Manager.NativeManager.EndPopup();
			}
			else
			{
				menuContext = null;
			}

			return changed;
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
			var flag = swig.TreeNodeFlags.OpenOnArrow | swig.TreeNodeFlags.OpenOnDoubleClick | swig.TreeNodeFlags.DefaultOpen | swig.TreeNodeFlags.SpanFullWidth;

			string nodeName = treeNode.ParamTreeNode.Node.Name.Value;

			if(treeNode.FCurves.Count > 0)
			{
				if (Manager.NativeManager.TreeNodeEx(nodeName + treeNode.ID, flag))
				{
					foreach (var fcurve in treeNode.FCurves)
					{
						fcurve.UpdateTree(nodeName);
					}

					Manager.NativeManager.TreePop();
				}
			}

			for (int i = 0; i < treeNode.Children.Count; i++)
			{
				UpdateTreeNode(treeNode.Children[i]);
			}
		}

		private void CheckAndApplyUpdate(TreeNode treeNode)
		{
			canCurveControlPre = canCurveControl;
			canCurveControl = canControl;

			// Check update
			if (!canCurveControlPre && canCurveControl)
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

				if (isDirtied(treeNode))
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
			var fcurves = GetSelectedFCurve();
			var selectedPoints = fcurves.SelectMany(_=> {
				var index = _.Item2.GetSelectedIndex();
				if(index != null)
				{
					return index.Select(__ => Tuple.Create(_.Item1, _.Item2, __));
				}
				return new Tuple<IFCurve, FCurveProperty, int>[0];
			});
			var invalidValue = "/";

			Manager.NativeManager.BeginChild("##FCurveDetails", new swig.Vec2());
			Manager.NativeManager.PushItemWidth(80 * Manager.DpiScale);
			Manager.NativeManager.Spacing();

			{
				var elements = selectedPoints.Select(_ => _.Item2.Keys[_.Item3]).Distinct();
				if(elements.Count() == 1)
				{
					var target = selectedPoints.First();
					var original = target.Item2.Keys[target.Item3];
					var frameKey = new int[] { (int)original };
					if (Manager.NativeManager.DragInt(texts.frame, frameKey))
					{
						var diff = frameKey[0] - original;

						foreach (var sp in selectedPoints)
						{
							var ind = sp.Item3;
							sp.Item2.Keys[ind] = (int)frameKey[0];
							sp.Item2.LeftKeys[ind] += diff;
							sp.Item2.RightKeys[ind] += diff;
							sp.Item2.IsDirtied = true;
							sp.Item2.SolveContradiction();
						}

						canCurveControl = false;
					}

					if (Manager.NativeManager.IsItemActive())
					{
						canCurveControl = false;
						canControl = false;
						isDetailControlling = true;
					}
				}
				else
				{
					Manager.NativeManager.InputText(texts.frame, invalidValue, swig.InputTextFlags.ReadOnly);
				}
			}
			
			{
				var elements = selectedPoints.Select(_ => _.Item2.Values[_.Item3]).Distinct();
				if (elements.Count() == 1)
				{
					var target = selectedPoints.First();
					var original = target.Item2.Values[target.Item3];

					var frameValue = new float[] { original };
					if (Manager.NativeManager.DragFloat(texts.value, frameValue))
					{
						var diff = frameValue[0] - original;

						foreach (var sp in selectedPoints)
						{
							var ind = sp.Item3;
							target.Item2.Values[ind] = frameValue[0];
							target.Item2.LeftValues[ind] += diff;
							target.Item2.RightValues[ind] += diff;
							target.Item2.IsDirtied = true;

							canControl = false;
							canCurveControl = false;
						}

						if (Manager.NativeManager.IsItemActive())
						{
							canCurveControl = false;
							canControl = false;
							isDetailControlling = true;
						}
					}
				}
				else
				{
					Manager.NativeManager.InputText(texts.value, invalidValue, swig.InputTextFlags.ReadOnly);
				}
			}

			// Left key
			if (selectedPoints.Count() == 1)
			{
				var target = selectedPoints.First();
				var ind = target.Item3;

				var leftValues = new float[] { target.Item2.LeftKeys[ind], target.Item2.LeftValues[ind] };
				if (Manager.NativeManager.DragFloat2(texts.left, leftValues))
				{
					target.Item2.LeftKeys[ind] = leftValues[0];
					target.Item2.LeftValues[ind] = leftValues[1];
					target.Item2.Clip(ind);
					target.Item2.IsDirtied = true;

					canControl = false;
					canCurveControl = false;
				}

				if (Manager.NativeManager.IsItemActive())
				{
					canCurveControl = false;
					canControl = false;
					isDetailControlling = true;
				}
			}
			else
			{
				Manager.NativeManager.InputText(texts.left, invalidValue, swig.InputTextFlags.ReadOnly);
			}

			// Right key
			if (selectedPoints.Count() == 1)
			{
				var target = selectedPoints.First();
				var ind = target.Item3;

				var rightValues = new float[] { target.Item2.RightKeys[ind], target.Item2.RightValues[ind] };
				if (Manager.NativeManager.DragFloat2(texts.right, rightValues))
				{
					target.Item2.RightKeys[ind] = rightValues[0];
					target.Item2.RightValues[ind] = rightValues[1];
					target.Item2.Clip(ind);
					target.Item2.IsDirtied = true;

					canControl = false;
					canCurveControl = false;
				}

				if (Manager.NativeManager.IsItemActive())
				{
					canCurveControl = false;
					canControl = false;
					isDetailControlling = true;
				}
			}
			else
			{
				Manager.NativeManager.InputText(texts.right, invalidValue, swig.InputTextFlags.ReadOnly);
			}

			// Bézier curve
			if (selectedPoints.Count() > 0)
			{
				var elements = selectedPoints.Select(_ => _.Item2.Interpolations[_.Item3]).Distinct();

				var targetIndex = elements.Count() == 1 ? elements.First() : type.FieldNames.Count;
				var targetName = targetIndex < type.FieldNames.Count ? type.FieldNames[targetIndex].ToString() : textMulti;
				var objects = type.FieldNames;

				if (Manager.NativeManager.BeginCombo(texts.type, targetName, swig.ComboFlags.None))
				{
					for (int i = 0; i < objects.Count; i++)
					{
						bool is_selected = i == targetIndex;

						if (Manager.NativeManager.Selectable(objects[i].ToString(), is_selected, swig.SelectableFlags.None))
						{
							foreach(var target in selectedPoints)
							{
								target.Item2.Interpolations[target.Item3] = i;
								target.Item2.IsDirtied = true;
							}
							canCurveControl = false;
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
				Manager.NativeManager.InputText(texts.type, invalidValue, swig.InputTextFlags.ReadOnly);
			}

			Manager.NativeManager.Separator();

			// Start curve
			if (fcurves.Count() == 1)
			{
				var fcurve = fcurves.First();

				startCurve.SetBinding(fcurve.Item1.StartType);
				startCurve.Update();
				fcurve.Item2.StartEdge = (Data.Value.FCurveEdge)fcurve.Item1.StartType;

				if (Manager.NativeManager.IsItemActive()) canControl = false;
			}
			else
			{
				Manager.NativeManager.InputText(texts.start, invalidValue, swig.InputTextFlags.ReadOnly);
			}

			// End curve
			if (fcurves.Count() == 1)
			{
				var fcurve = fcurves.First();

				endCurve.SetBinding(fcurve.Item1.EndType);
				endCurve.Update();
				fcurve.Item2.EndEdge = (Data.Value.FCurveEdge)fcurve.Item1.EndType;

				if (Manager.NativeManager.IsItemActive()) canControl = false;
			}
			else
			{
				Manager.NativeManager.InputText(texts.end, invalidValue, swig.InputTextFlags.ReadOnly);
			}

			// Sampling
			if (fcurves.Count() == 1)
			{
				var fcurve = fcurves.First();

				var sampling = new int[] { fcurve.Item1.Sampling };

				if (Manager.NativeManager.InputInt(texts.sampling + "##SamplingText", sampling))
				{
					fcurve.Item1.Sampling.SetValue(sampling[0]);
				}

				if (Manager.NativeManager.IsItemActive()) canControl = false;
			}
			else
			{
				Manager.NativeManager.InputText(texts.sampling + "##SamplingText", invalidValue, swig.InputTextFlags.ReadOnly);
			}

			// Offset label
			if (fcurves.Count() == 1)
			{
				var fcurve = fcurves.First();

				var offset_id = texts.offset + "##OffsetMinMax";
				var offsets = new float[] { fcurve.Item1.OffsetMin, fcurve.Item1.OffsetMax };

				if (Manager.NativeManager.DragFloat2(offset_id, offsets))
				{
					fcurve.Item1.OffsetMin.SetValue(offsets[0]);
					fcurve.Item1.OffsetMax.SetValue(offsets[1]);
				}

				if (Manager.NativeManager.IsItemActive()) canControl = false;
			}
			else
			{
				Manager.NativeManager.InputText(texts.offset + "##OffsetMinMax", invalidValue, swig.InputTextFlags.ReadOnly);
			}

			if (fcurves.Count() == 0)
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
				Manager.NativeManager.InputText(texts.timelineMode_Name + "##Timeline", invalidValue, swig.InputTextFlags.ReadOnly);
				timeline.SetBinding(null);
			}

			Manager.NativeManager.Spacing();
			Manager.NativeManager.PopItemWidth();
			Manager.NativeManager.EndChild();
		}


		bool IsHovered(TreeNode treeNode, out FCurve fcurve, out int fcurveInd, out int pointInd)
		{
			fcurve = null;
			fcurveInd = -1;
			pointInd = -1;

			foreach (var f in treeNode.FCurves)
			{
				if(f.IsHovered(out fcurveInd, out pointInd))
				{
					fcurve = f;
					return true;
				}
			}

			for (int i = 0; i < treeNode.Children.Count; i++)
			{
				if(IsHovered(treeNode.Children[i], out fcurve, out fcurveInd, out pointInd))
				{
					return true;
				}
			}

			return false;
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
			var paramTreeNodes = Core.GetFCurveParameterNode(Core.Root);
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
					if (o == fcurve.GetValueAsObject())
					{
						foreach (var prop in fcurve.Properties)
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
				foreach (var fcurve in t.FCurves)
				{
					fcurve.Unselect();
				}

				foreach (var c in t.Children)
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

		public void UnselectPoints()
		{
			Action<TreeNode> recurse = null;

			recurse = (t) =>
			{
				foreach (var fcurve in t.FCurves)
				{
					fcurve.UnselectPoints();
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

			foreach (var curve in flatten)
			{
				for (int ind = 0; ind < curve.Properties.Length; ind++)
				{
					var prop = curve.Properties[ind];

					if (!prop.KVSelected.Any(_=>_ > 0))
					{
						continue;
					}

					FCurveCopiedData.Curve copiedCurve = new FCurveCopiedData.Curve();

					copiedCurve.Index = ind;
					copiedCurve.ID = curve.ID;
					
					for (int i = 0; i < prop.KVSelected.Length; i++)
					{
						if (prop.KVSelected[i] > 0)
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

			return data;
		}

		public void AlignKeyValue(bool alignKey, bool alignValue)
		{
			int pointCount = 0;
			float keySum = 0;
			float valueSum = 0;

			var flatten = flattenFcurves.ToArray();
			foreach (var curve in flatten)
			{
				for (int ind = 0; ind < curve.Properties.Length; ind++)
				{
					var prop = curve.Properties[ind];

					if (!prop.IsShown) continue;
					if (!prop.Selected) continue;

					for (int i = 0; i < prop.Keys.Length - 1; i++)
					{
						if (prop.KVSelected[i] == 0)
							continue;

						keySum += prop.Keys[i];
						valueSum += prop.Values[i];
						pointCount++;
					}

				}
			}

			if (pointCount == 0)
				return;

			float key = keySum / pointCount;
			float value = valueSum / pointCount;

			foreach (var curve in flatten)
			{
				bool curveChanged = false;

				for (int ind = 0; ind < curve.Properties.Length; ind++)
				{
					var prop = curve.Properties[ind];

					if (!prop.IsShown) continue;
					if (!prop.Selected) continue;

					bool propChanged = false;
					for (int i = 0; i < prop.Keys.Length - 1; i++)
					{
						if (prop.KVSelected[i] == 0)
							continue;

						var diffKey = key - prop.Keys[i];
						var diffValue = value - prop.Values[i];

						if(alignKey)
						{
							prop.Keys[i] = key;
							prop.LeftKeys[i] += diffKey;
							prop.RightKeys[i] += diffKey;
						}

						if(alignValue)
						{
							prop.Values[i] = value;
							prop.LeftValues[i] += diffValue;
							prop.RightValues[i] += diffValue;
						}

						propChanged = true;
						curveChanged = true;
					}

					if(propChanged)
					{
						prop.SolveContradiction();
						prop.IsDirtied = true;
					}
				}

				if(curveChanged)
				{
					curve.Commit();
				}
			}
		}

		public void Paste(int time, bool replace, bool removeSpace)
		{
			try
			{
				System.Xml.Serialization.XmlSerializer serializer = new System.Xml.Serialization.XmlSerializer(typeof(FCurveCopiedData));
				FCurveCopiedData data = serializer.Deserialize(new System.IO.StringReader(Manager.NativeManager.GetClipboardText())) as FCurveCopiedData;
				Paste(data, time, replace, removeSpace);
			}
			catch
			{
				return;
			}
		}

		public void Paste(FCurveCopiedData data, float offsetTime, bool replace, bool removeSpace)
		{
			if(removeSpace)
			{
				var xmin = data.Curves.SelectMany(_ => _.Points).Min(_ => _.Key);

				foreach (var curve in data.Curves)
				{
					foreach (var p in curve.Points)
					{
						p.Key -= xmin;
						p.LeftKey -= xmin;
						p.RightKey -= xmin;
					}
				}
			}

			var flatten = flattenFcurves.ToArray();
			foreach (var curve in flatten)
			{
				for (int ind = 0; ind < curve.Properties.Length; ind++)
				{
					var prop = curve.Properties[ind];

					if (!prop.IsShown) continue;
					if (!prop.Selected) continue;

					var candidates = data.Curves.Where(_ => _.Index == ind);

					var copiedCurve = candidates.FirstOrDefault(_ => _.ID == curve.ID);

					if (copiedCurve == null)
					{
						copiedCurve = data.Curves.FirstOrDefault(_ => _.Index == ind);
					}

					if (copiedCurve != null)
					{
					
						if(replace)
						{
							prop.KVSelected = new byte[copiedCurve.Points.Count + 1];

							prop.Keys = copiedCurve.Points.Select(_ => _.Key + offsetTime).Concat(new[] { 0.0f }).ToArray();
							prop.Values = copiedCurve.Points.Select(_ => _.Value).Concat(new[] { 0.0f }).ToArray();
							prop.LeftKeys = copiedCurve.Points.Select(_ => _.LeftKey + offsetTime).Concat(new[] { 0.0f }).ToArray();
							prop.LeftValues = copiedCurve.Points.Select(_ => _.LeftValue).Concat(new[] { 0.0f }).ToArray();
							prop.RightKeys = copiedCurve.Points.Select(_ => _.RightKey + offsetTime).Concat(new[] { 0.0f }).ToArray();
							prop.RightValues = copiedCurve.Points.Select(_ => _.RightValue).Concat(new[] { 0.0f }).ToArray();
							prop.Interpolations = copiedCurve.Points.Select(_ => _.Interpolation).Concat(new[] { 0 }).ToArray();

							prop.SolveContradiction();
							prop.IsDirtied = true;

						}
						else
						{
							for (int i = 0; i < copiedCurve.Points.Count; i++)
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
				}

				curve.Commit();
			}
		}

		List<Tuple35<Data.Value.IFCurve, FCurveProperty>> GetSelectedFCurve()
		{
			List<Tuple35<Data.Value.IFCurve, FCurveProperty>> rets = new List<Tuple35<IFCurve, FCurveProperty>>();
			Action<TreeNode> recurse = null;

			recurse = (t) =>
			{
				foreach (var fcurve in t.FCurves)
				{
					var r = fcurve.GetSelectedFCurve();
					if (r == null)
						continue;

					rets.AddRange(r);
				}

				foreach (var c in t.Children)
				{
					recurse(c);
				}
			};

			recurse(treeNodes);

			return rets;
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
			IFCurveKey CreateKey(int key, float value, float leftKey, float leftValue, float rightKey, float rightValue, FCurveInterpolation interpolation);

			void SetKeys(IFCurve fcurve, IFCurveKey[] keys);
		}

		class FloatFCurveConverter : IFCurveConverter
		{
			public IFCurveKey CreateKey(int key, float value, float leftKey, float leftValue, float rightKey, float rightValue, FCurveInterpolation interpolation)
			{
				var fcurveKey = new FCurveKey<float>(key, value);
				fcurveKey.SetLeftDirectly(leftKey, leftValue);
				fcurveKey.SetRightDirectly(rightKey, rightValue);
				fcurveKey.SetInterpolationType(interpolation);
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
			public IFCurveKey CreateKey(int key, float value, float leftKey, float leftValue, float rightKey, float rightValue, FCurveInterpolation interpolation)
			{
				var fcurveKey = new FCurveKey<int>(key, (int)value);
				fcurveKey.SetLeftDirectly(leftKey, leftValue);
				fcurveKey.SetRightDirectly(rightKey, rightValue);
				fcurveKey.SetInterpolationType(interpolation);
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

			protected FCurveProperty[] properties = null;

			public TreeNode ParentNode { get; set; }

			public string Name { get; protected set; }

			public FCurveProperty[] Properties { get { return properties; } }

			static public FCurve Create(Tuple35<string, object> v, FCurves window)
			{
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
				else if (v.Item2 is Data.Value.FCurveVector2D)
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

				for (int i = 0; i < length; i++)
				{
					properties[i] = new FCurveProperty();
					ids[i] = Manager.GetUniqueID();
					properties[i].Color = colors[i];
				}

				this.names = names;
				this.fcurves = fcurves;
			}

			public object GetValueAsObject()
			{
				return Value;
			}

			public List<Tuple35<Data.Value.IFCurve, FCurveProperty>> GetSelectedFCurve()
			{
				List<Tuple35<Data.Value.IFCurve, FCurveProperty>> ret = null;

				for (int i = 0; i < properties.Length; i++)
				{
					if (properties[i].Selected)
					{
						if (ret == null)
							ret = new List<Tuple35<IFCurve, FCurveProperty>>();

						ret.Add(Tuple35.Create(fcurves[i], properties[i]));
					}
				}

				return ret;
			}

			public void UpdateTree(string nodeName)
			{

				for (int i = 0; i < properties.Length; i++)
				{
					var value = this.fcurves[i].GetValue((int)Manager.Viewer.Current);

					string labelName = Name + " : " + names[i] + " (" + value + ")";
					string labelID = "###FCurveLabel_" + nodeName + "_" + Name + "_" + names[i];
					if (Manager.NativeManager.Selectable(labelName + labelID, properties[i].IsShown, swig.SelectableFlags.AllowDoubleClick | swig.SelectableFlags.SpanAllColumns))
					{
						if (Manager.NativeManager.IsMouseDoubleClicked(0))
						{
							properties[i].IsShown = true;

							if (this.fcurves[i].Keys.Count() > 0)
							{
								var maxKey = this.fcurves[i].Keys.Max(_ => _.Frame);
								var minKey = this.fcurves[i].Keys.Min(_ => _.Frame);
								var maxValue = this.fcurves[i].Keys.Max(_ => _.ValueAsFloat);
								var minValue = this.fcurves[i].Keys.Min(_ => _.ValueAsFloat);
								window.autoZoomRangeMax.X = maxKey + 20;
								window.autoZoomRangeMin.X = minKey - 20;
								window.autoZoomRangeMax.Y = maxValue + 10;
								window.autoZoomRangeMin.Y = minValue - 10;
							}
							else
							{
								window.autoZoomRangeMax.Y = defaultValue + 10;
								window.autoZoomRangeMin.Y = -10;
							}
						}
						else
						{
							if (IsShiftDown())
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

			public bool AddPoint(int propInd, swig.Vec2 position)
			{
				var i = propInd;
				int newCount = 0;

				if(Manager.NativeManager.AddFCurvePoint(position, properties[i].Keys,
						properties[i].Values,
						properties[i].LeftKeys,
						properties[i].LeftValues,
						properties[i].RightKeys,
						properties[i].RightValues,
						properties[i].Interpolations,
						properties[i].KVSelected,
						properties[i].Keys.Length - 1,
						ref newCount))
				{
					// TODO refactor
					properties[i].Keys = properties[i].Keys.Concat(new[] { 0.0f }).ToArray();
					properties[i].Values = properties[i].Values.Concat(new[] { 0.0f }).ToArray();
					properties[i].LeftKeys = properties[i].LeftKeys.Concat(new[] { 0.0f }).ToArray();
					properties[i].LeftValues = properties[i].LeftValues.Concat(new[] { 0.0f }).ToArray();
					properties[i].RightKeys = properties[i].RightKeys.Concat(new[] { 0.0f }).ToArray();
					properties[i].RightValues = properties[i].RightValues.Concat(new[] { 0.0f }).ToArray();
					properties[i].Interpolations = properties[i].Interpolations.Concat(new[] { 0 }).ToArray();
					properties[i].IsDirtied = true;
					return true;
				}
				return false;
			}

			public bool RemovePoint(int propInd, swig.Vec2 position)
			{
				var i = propInd;
				int newCount = 0;

				if (Manager.NativeManager.RemoveFCurvePoint(position, properties[i].Keys,
						properties[i].Values,
						properties[i].LeftKeys,
						properties[i].LeftValues,
						properties[i].RightKeys,
						properties[i].RightValues,
						properties[i].Interpolations,
						properties[i].KVSelected,
						properties[i].Keys.Length - 1,
						ref newCount))
				{
					// TODO : refactor
					properties[i].Keys = properties[i].Keys.Take(properties[i].Keys.Length - 1).ToArray();
					properties[i].Values = properties[i].Values.Take(properties[i].Values.Length - 1).ToArray();
					properties[i].LeftKeys = properties[i].LeftKeys.Take(properties[i].LeftKeys.Length - 1).ToArray();
					properties[i].LeftValues = properties[i].LeftValues.Take(properties[i].LeftValues.Length - 1).ToArray();
					properties[i].RightKeys = properties[i].RightKeys.Take(properties[i].RightKeys.Length - 1).ToArray();
					properties[i].RightValues = properties[i].RightValues.Take(properties[i].RightValues.Length - 1).ToArray();
					properties[i].Interpolations = properties[i].Interpolations.Take(properties[i].Interpolations.Length - 1).ToArray();
					properties[i].IsDirtied = true;
					return true;
				}

				return false;
			}

			public bool IsHovered(out int fcurveInd, out int pointInd)
			{
				fcurveInd = -1;
				pointInd = -1;

				for (int i = 0; i < properties.Length; i++)
				{
					if (!properties[i].IsShown) continue;

					if (Manager.NativeManager.IsHoveredOnFCurve(
						properties[i].Keys,
						properties[i].Values,
						properties[i].LeftKeys,
						properties[i].LeftValues,
						properties[i].RightKeys,
						properties[i].RightValues,
						properties[i].Interpolations,
						(swig.FCurveEdgeType)properties[i].StartEdge,
						(swig.FCurveEdgeType)properties[i].EndEdge,
						properties[i].Color,
						properties[i].Keys.Length - 1))
					{
						fcurveInd = i;

						if(Manager.NativeManager.IsHoveredOnFCurvePoint(properties[i].Keys, properties[i].Values, properties[i].Keys.Length - 1, ref pointInd))
						{

						}

						return true;
					}
				}

				return false;
			}

			public void UpdateGraph(ref bool canControl)
			{
				for (int i = 0; i < properties.Length; i++)
				{
					if (!properties[i].IsShown) continue;

					if (!properties[i].IsDirtied)
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
						true,
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
							if (IsShiftDown())
							{
								properties[i].Selected = !properties[i].Selected;
							}
							else if (IsAltDown())
							{
								properties[i].Selected = true;
							}
							else
							{
								window.UnselectAll();
								properties[i].Selected = true;
							}
						}
						
						if (properties[i].Keys.Length - 1 != newCount && newCount != -1)
						{
							if (properties[i].Keys.Length == newCount)
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

						for (int k = 0; k < properties[i].Values.Length; k++)
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
				foreach (var prop in properties)
				{
					if (!prop.IsShown) continue;
					if (!prop.Selected) continue;


					for (int i = 0; i < prop.KVSelected.Length - 1; i++)
					{
						if (prop.KVSelected[i] == 0) continue;

						var length_pre = 10.0f;
						var length_next = 10.0f;

						if (prop.KVSelected.Length - 1 == 1)
						{
							// None
						}
						else if (i == 0)
						{
							var dx = prop.Keys[i + 1] - prop.Keys[i + 0];
							length_next = Math.Min(length_next, dx / 2.0f);
						}
						else if (i == prop.KVSelected.Length - 2)
						{
							var dx = prop.Keys[i + 0] - prop.Keys[i - 1];
							length_pre = Math.Min(length_pre, dx / 2.0f);
						}
						else
						{
							length_next = Math.Min(length_next, (prop.Keys[i + 1] - prop.Keys[i + 0]) / 2.0f);
							length_pre = Math.Min(length_next, (prop.Keys[i + 0] - prop.Keys[i - 1]) / 2.0f);
						}

						prop.LeftKeys[i] = prop.Keys[i] - length_pre;
						prop.LeftValues[i] = prop.Values[i];

						prop.RightKeys[i] = prop.Keys[i] + length_next;
						prop.RightValues[i] = prop.Values[i];

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

			public void UnselectPoints()
			{
				foreach (var prop in properties)
				{
					for(int i = 0; i < prop.KVSelected.Length; i++)
					{
						prop.KVSelected[i] = 0;
					}
				}
			}

			public void Unselect()
			{
				foreach (var prop in properties)
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

				for (int i = 0; i < properties.Length; i++)
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
						var key = converter.CreateKey((int)properties[i].Keys[j], v, properties[i].LeftKeys[j], properties[i].LeftValues[j], properties[i].RightKeys[j], properties[i].RightValues[j], (FCurveInterpolation)properties[i].Interpolations[j]);
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
				return;
				if (index > 0)
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

			public List<int> GetSelectedIndex()
			{
				List<int> ret = null;

				for (int i = 0; i < KVSelected.Count() - 1; i++)
				{
					if (KVSelected[i] > 0)
					{
						if (ret == null)
							ret = new List<int>();
						ret.Add(i);
					}
				}

				return ret;
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
					Interpolations = keyFrames.Select(_ => (int)_.InterpolationType).Concat(new int[] { 0 }).ToArray();
				}
			}
		}
	}
}
