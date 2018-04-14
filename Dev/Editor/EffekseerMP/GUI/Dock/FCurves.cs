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

		public FCurves()
		{
			Label = Resources.GetString("FCurves");

			Command.CommandManager.Changed += OnChanged;
			Core.OnAfterNew += OnChanged;
			Core.OnBeforeLoad += OnBeforeLoad;
			Core.OnAfterLoad += OnAfterLoad;
			OnChanged();
		}

		protected override void UpdateInternal()
		{


			Manager.NativeManager.Text(frame_text);
			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(value_text);
			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(start_text);
			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(end_text);
			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(type_text);

			Manager.NativeManager.Text(sampling_text);
			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(left_text);
			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(right_text);
			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(offset_text);
			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(offset_min_text);
			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(offset_max_text);


			Manager.NativeManager.BeginGroup();

			Manager.NativeManager.Columns(2);

			Manager.NativeManager.SetColumnWidth(0, 200);
			Manager.NativeManager.BeginChild("##FCurveGroup_Tree");

			if(treeNodes != null)
			{
				UpdateTreeNode(treeNodes);
			}

			Manager.NativeManager.EndChild();

			Manager.NativeManager.NextColumn();

			Manager.NativeManager.BeginChild("##FCurveGroup_Graph");

			if(Manager.NativeManager.BeginFCurve(1))
			{
				UpdateGraph(treeNodes);
			}

			Manager.NativeManager.EndFCurve();

			Manager.NativeManager.EndChild();


			Manager.NativeManager.EndGroup();
		}

		public override void OnDisposed()
		{
			Console.WriteLine("Not Implemented (Dispose fcurve event)");
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
			bool canControl = true;

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
					bool nochange = tn.Children.Count() == ptn.Children.Count();
					for (int i = 0; i < ptn.Children.Count() && nochange; i++)
					{
						if (tn.Children[i].ParamTreeNode.Node != ptn.Children[i].Node)
						{
							nochange = false;
							break;
						}
					}
					if (nochange) return;

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

		class TreeNode
		{
			public string ID = string.Empty;

			public Utl.ParameterTreeNode ParamTreeNode { get; private set; }

			public List<TreeNode> Children { get; private set; }

			public Math.Point Position { get; private set; }

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
			public TreeNode ParentNode { get; set; }

			public string Name { get; protected set; }

			public abstract object GetValueAsObject();

			static public FCurve Create(Tuple<string, object> v, FCurves window)
			{
				if (v.Item2 is Data.Value.FCurveVector2D)
				{
					var v_ = (Data.Value.FCurveVector2D)v.Item2;
					return new FCurveVector2D(v.Item1, v_, window);
				}
				else if (v.Item2 is Data.Value.FCurveVector3D)
				{
					var v_ = (Data.Value.FCurveVector3D)v.Item2;
					return new FCurveVector3D(v.Item1, v_, window);
				}
				else if (v.Item2 is Data.Value.FCurveColorRGBA)
				{
					var v_ = (Data.Value.FCurveColorRGBA)v.Item2;
					return new FCurveColor(v.Item1, v_, window);
				}
				
				return null;
			}

			public FCurve()
			{
			}

			public virtual void OnAdded() {}

			public virtual void OnRemoved() {}

			public virtual void Move(float x, float y, int changedType, Data.Value.IFCurve except) { }

			public virtual void Unselect() { }

			public virtual void UpdateTree() { }

			public virtual void UpdateGraph(ref bool canControl) { }

			public virtual void Hide() { }

			public virtual void Commit() { }

			public virtual bool IsDirtied() { return false; }
		}

		class FCurveColor : FCurve
		{
			public Data.Value.FCurveColorRGBA Value { get; private set; }

			public FCurveColor(string name, Data.Value.FCurveColorRGBA value, FCurves window)
			{
				Name = name;
				Value = value;
				//this.window = window;
				//
				//fCurvesMagYCount = (float)Math.Log((float)GraphPanel.Height / (value.R.DefaultValueRangeMax - value.R.DefaultValueRangeMin), 2.0);
				//fCurvesOffsetY = (value.R.DefaultValueRangeMax + value.R.DefaultValueRangeMin) / 2.0f;
			}

			public override object GetValueAsObject()
			{
				return Value;
			}
		}

		class FCurveVector2D : FCurve
		{
			public Data.Value.FCurveVector2D Value { get; private set; }

			public FCurveVector2D(string name, Data.Value.FCurveVector2D value, FCurves window)
			{
				Name = name;
				Value = value;
				//this.window = window;
				//
				//fCurvesMagYCount = (float)Math.Log((float)GraphPanel.Height / (value.X.DefaultValueRangeMax - value.X.DefaultValueRangeMin), 2.0);
				//fCurvesOffsetY = (value.X.DefaultValueRangeMax + value.X.DefaultValueRangeMin) / 2.0f;
			}

			public override object GetValueAsObject()
			{
				return Value;
			}
		}

		class FCurveVector3D : FCurve
		{
			FCurveProperty[] properties = new FCurveProperty[3];
			Data.Value.FCurve<float>[] fcurves = new Data.Value.FCurve<float>[3];
			int[] ids = new int[3];
			bool[] isDirtied = new bool[3];

			FCurves window = null;

			public Data.Value.FCurveVector3D Value { get; private set; }

			public FCurveVector3D(string name, Data.Value.FCurveVector3D value, FCurves window)
			{
				Name = name;
				Value = value;
				this.window = window;
				//
				//fCurvesMagYCount = (float)Math.Log((float)GraphPanel.Height / (value.X.DefaultValueRangeMax - value.X.DefaultValueRangeMin), 2.0);
				//fCurvesOffsetY = (value.X.DefaultValueRangeMax + value.X.DefaultValueRangeMin) / 2.0f;

				properties[0] = new FCurveProperty();
				properties[1] = new FCurveProperty();
				properties[2] = new FCurveProperty();

				fcurves[0] = Value.X;
				fcurves[1] = Value.Y;
				fcurves[2] = Value.Z;

				ids[0] = Manager.GetUniqueID();
				ids[1] = Manager.GetUniqueID();
				ids[2] = Manager.GetUniqueID();

				properties[0].Color = 0xff0000ff;
				properties[1].Color = 0xff00ff00;
				properties[2].Color = 0xffff0000;

				isDirtied[0] = false;
				isDirtied[1] = false;
				isDirtied[2] = false;
			}

			public override object GetValueAsObject()
			{
				return Value;
			}

			public override void UpdateTree()
			{
				if (Manager.NativeManager.Selectable("X", properties[0].IsShown))
				{
					int LEFT_SHIFT = 340;
					int RIGHT_SHIFT = 344;

					if (Manager.NativeManager.IsKeyDown(LEFT_SHIFT) || Manager.NativeManager.IsKeyDown(RIGHT_SHIFT))
					{
					}
					else
					{
						window.HideAll();
					}

					properties[0].IsShown = true;
				}

				if (Manager.NativeManager.Selectable("Y", properties[1].IsShown))
				{
					int LEFT_SHIFT = 340;
					int RIGHT_SHIFT = 344;

					if (Manager.NativeManager.IsKeyDown(LEFT_SHIFT) || Manager.NativeManager.IsKeyDown(RIGHT_SHIFT))
					{
					}
					else
					{
						window.HideAll();
					}

					properties[1].IsShown = true;
				}

				if (Manager.NativeManager.Selectable("Z", properties[2].IsShown))
				{
					int LEFT_SHIFT = 340;
					int RIGHT_SHIFT = 344;

					if (Manager.NativeManager.IsKeyDown(LEFT_SHIFT) || Manager.NativeManager.IsKeyDown(RIGHT_SHIFT))
					{
					}
					else
					{
						window.HideAll();
					}

					properties[2].IsShown = true;
				}
			}

			public override void UpdateGraph(ref bool canControl)
			{
				for(int i = 0; i < 3; i++)
				{
					if (!properties[i].IsShown) continue;

					properties[i].Update(fcurves[i]);

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
						swig.FCurveEdgeType.Constant,
						swig.FCurveEdgeType.Constant,
						properties[i].KVSelected,
						properties[i].Keys.Length - 1,
						0.0f,
						false,
						canControl,
						properties[i].Color,
						properties[i].Selected,
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
							isDirtied[i] = true;
						}

						if (isSelected)
						{
							int LEFT_SHIFT = 340;
							int RIGHT_SHIFT = 344;

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

					isDirtied[j] = true;

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

			public override void OnAdded()
			{
				fcurves[0].OnChanged += OnChanged_1;
				fcurves[1].OnChanged += OnChanged_2;
				fcurves[2].OnChanged += OnChanged_3;
			}

			public override void OnRemoved()
			{
				fcurves[0].OnChanged -= OnChanged_1;
				fcurves[1].OnChanged -= OnChanged_2;
				fcurves[2].OnChanged -= OnChanged_3;
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

			void OnChanged(int i)
			{
				properties[i].Keys = new float[0];
				properties[i].Values = new float[0];
				properties[i].LeftKeys = new float[0];
				properties[i].LeftValues = new float[0];
				properties[i].RightKeys = new float[0];
				properties[i].RightValues = new float[0];
				properties[i].Interpolations = new int[0];
				properties[i].Update(fcurves[i]);
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
					if (!isDirtied[i]) continue;
					
					var keys = new List<Data.Value.FCurveKey<float>>();

					for(int j = 0; j < properties[i].Keys.Length - 1; j++)
					{
						Data.Value.FCurveKey<float> key = new FCurveKey<float>(
							(int)properties[i].Keys[j],
							properties[i].Values[j]);

						key.SetLeftDirectly(
							properties[i].LeftKeys[j],
							properties[i].LeftValues[j]);

						key.SetLeftDirectly(
							properties[i].RightKeys[j],
							properties[i].RightValues[j]);

						keys.Add(key);
					}

					fcurves[i].SetKeys(keys.ToArray());
					
					isDirtied[i] = false;
				}
			}

			public override bool IsDirtied()
			{
				for (int i = 0; i < properties.Length; i++)
				{
					if (isDirtied[i]) return true;
				}

				return false;
			}
		}

		class FCurveProperty
		{
			public UInt32 Color = 0;

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

			public void Update(Data.Value.FCurve<float> fcurve)
			{
				var plength = fcurve.Keys.Count() + 1;

				if (Keys.Length < plength)
				{
					var keyFrames = fcurve.Keys.ToArray();
					Keys = keyFrames.Select(_ => (float)_.Frame).Concat(new float[] { 0.0f }).ToArray();
				}

				if (Values.Length < plength)
				{
					var keyFrames = fcurve.Keys.ToArray();
					Values = keyFrames.Select(_ => _.ValueAsFloat).Concat(new float[] { 0.0f }).ToArray();
				}

				if (LeftKeys.Length < plength)
				{
					var keyFrames = fcurve.Keys.ToArray();
					LeftKeys = keyFrames.Select(_ => _.LeftX).Concat(new float[] { 0.0f }).ToArray();
				}

				if (LeftValues.Length < plength)
				{
					var keyFrames = fcurve.Keys.ToArray();
					LeftValues = keyFrames.Select(_ => _.LeftY).Concat(new float[] { 0.0f }).ToArray();
				}

				if (RightKeys.Length < plength)
				{
					var keyFrames = fcurve.Keys.ToArray();
					RightKeys = keyFrames.Select(_ => _.RightX).Concat(new float[] { 0.0f }).ToArray();
				}

				if (RightValues.Length < plength)
				{
					var keyFrames = fcurve.Keys.ToArray();
					RightValues = keyFrames.Select(_ => _.RightY).Concat(new float[] { 0.0f }).ToArray();
				}

				if (KVSelected.Length < plength)
				{
					var keyFrames = fcurve.Keys.ToArray();

					var new_selected = new byte[keyFrames.Length + 1];
					KVSelected.CopyTo(new_selected, 0);
					KVSelected = new_selected;
				}

				if (Interpolations.Length < plength)
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
