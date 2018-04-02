using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
			Manager.NativeManager.Text(value_text);
			Manager.NativeManager.Text(start_text);
			Manager.NativeManager.Text(end_text);
			Manager.NativeManager.Text(type_text);
			Manager.NativeManager.Text(sampling_text);
			Manager.NativeManager.Text(left_text);
			Manager.NativeManager.Text(right_text);
			Manager.NativeManager.Text(offset_text);
			Manager.NativeManager.Text(offset_min_text);
			Manager.NativeManager.Text(offset_max_text);


			Manager.NativeManager.BeginGroup();

			Manager.NativeManager.BeginChild("##FCurveGroup");

			if(treeNodes != null)
			{
				DrawTreeNode(treeNodes);
			}

			Manager.NativeManager.EndChild();

			Manager.NativeManager.EndGroup();
		}

		public override void OnDisposed()
		{
			Command.CommandManager.Changed -= OnChanged;
			Core.OnAfterNew -= OnChanged;
			Core.OnBeforeLoad -= OnBeforeLoad;
			Core.OnAfterLoad -= OnAfterLoad;
		}

		void DrawTreeNode(TreeNode treeNode)
		{
			var flag = swig.TreeNodeFlags.OpenOnArrow | swig.TreeNodeFlags.OpenOnDoubleClick | swig.TreeNodeFlags.DefaultOpen;

			if (Manager.NativeManager.TreeNodeEx(treeNode.ParamTreeNode.Node.Name.Value + treeNode.ID, flag))
			{
				foreach (var fcurve in treeNode.FCurves)
				{
					if(Manager.NativeManager.BeginChildFrame((uint)fcurve.ID, new swig.Vec2(200,100)))
					{
						Manager.NativeManager.Text(fcurve.Name);
					}
					Manager.NativeManager.EndChildFrame();
				}

				for (int i = 0; i < treeNode.Children.Count; i++)
				{
					DrawTreeNode(treeNode.Children[i]);
				}

				Manager.NativeManager.TreePop();
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
						if (tn.Children[i].ParamTreeNode != ptn.Children[i])
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

			public string IDStr { get; private set; }

			public int ID { get; private set; }

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
				ID = Manager.GetUniqueID();
				IDStr = "##" + ID.ToString();
			}

			public virtual void OnAdded()
			{
				/*
				panelTree.Controls.Add(GraphPanel);
				GraphPanel.BackColor = ColorBackground;

				added = true;
				*/
			}

			public virtual void OnRemoved()
			{
				/*
				panelTree.Controls.Remove(GraphPanel);
				if (!added) throw new Exception("already added.");
				added = false;
				*/
			}
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
			public Data.Value.FCurveVector3D Value { get; private set; }

			public FCurveVector3D(string name, Data.Value.FCurveVector3D value, FCurves window)
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
	}
}
