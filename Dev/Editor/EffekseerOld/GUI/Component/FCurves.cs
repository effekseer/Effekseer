using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Reactive;
using System.Reactive.Linq;

namespace Effekseer.GUI.Component
{
	public partial class FCurves : UserControl
	{
		public FCurves()
		{
			InitializeComponent();

			HandleCreated += new EventHandler(FCurves_HandleCreated);
			HandleDestroyed += new EventHandler(FCurves_HandleDestroyed);

			cb_fcurveEdgeStart.Initialize(typeof(Data.Value.FCurveEdge));
			cb_fcurveEdgeEnd.Initialize(typeof(Data.Value.FCurveEdge));
			cb_fcurveInterpolation.Initialize(typeof(Data.Value.FCurveInterpolation));


			txt_offset_min.Anchor = System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Top;
			txt_offset_max.Anchor = System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Top;
			txt_sampling.Anchor = System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Top;

			txt_frame.IsEnable = () =>
				{
					return editedFCurveKey != null;
				};

			txt_frame.WriteMethod = (value, wheel) =>
			{
				if (editedFCurveKey != null)
				{
					editedFCurveKey.SetFrame(value);
					editedFCurveKey.Commit(wheel);
					txt_frame.Reload();
				}
			};

			txt_frame.ReadMethod = () =>
			{
				if (editedFCurveKey != null)
				{
					return editedFCurveKey.Frame;
				}
				return 0;
			};

			txt_value.IsEnable = () =>
				{
					return editedFCurveKey != null;
				};

			txt_value.WriteMethod = (value,wheel) =>
				{
					if (editedFCurveKey != null)
					{
						editedFCurveKey.SetValueAsFloat(value);
						editedFCurveKey.Commit(wheel);
						txt_value.Reload();
					}
				};

			txt_value.ReadMethod = () =>
				{
					if (editedFCurveKey != null)
					{
						return editedFCurveKey.ValueAsFloat;
					}
					return 0.0f;
				};

			txt_left_x.IsEnable = () =>
			{
				return editedFCurveKey != null;
			};

			txt_left_x.WriteMethod = (value, wheel) =>
			{
				if (editedFCurveKey != null)
				{
					editedFCurveKey.SetLeftX(value);
					editedFCurveKey.Commit(wheel);
					txt_left_x.Reload();
				}
			};

			txt_left_x.ReadMethod = () =>
			{
				if (editedFCurveKey != null)
				{
					return editedFCurveKey.LeftX;
				}
				return 0.0f;
			};

			txt_left_y.IsEnable = () =>
			{
				return editedFCurveKey != null;
			};

			txt_left_y.WriteMethod = (value, wheel) =>
			{
				if (editedFCurveKey != null)
				{
					editedFCurveKey.SetLeftY(value);
					editedFCurveKey.Commit(wheel);
					txt_left_y.Reload();
				}
			};

			txt_left_y.ReadMethod = () =>
			{
				if (editedFCurveKey != null)
				{
					return editedFCurveKey.LeftY;
				}
				return 0.0f;
			};

			txt_right_x.IsEnable = () =>
			{
				return editedFCurveKey != null;
			};

			txt_right_x.WriteMethod = (value, wheel) =>
			{
				if (editedFCurveKey != null)
				{
					editedFCurveKey.SetRightX(value);
					editedFCurveKey.Commit(wheel);
					txt_right_x.Reload();
				}
			};

			txt_right_x.ReadMethod = () =>
			{
				if (editedFCurveKey != null)
				{
					return editedFCurveKey.RightX;
				}
				return 0.0f;
			};

			txt_right_y.IsEnable = () =>
			{
				return editedFCurveKey != null;
			};

			txt_right_y.WriteMethod = (value, wheel) =>
			{
				if (editedFCurveKey != null)
				{
					editedFCurveKey.SetRightY(value);
					editedFCurveKey.Commit(wheel);
					txt_right_y.Reload();
				}
			};

			txt_right_y.ReadMethod = () =>
			{
				if (editedFCurveKey != null)
				{
					return editedFCurveKey.RightY;
				}
				return 0.0f;
			};

			lbl_frame.Text = global::Effekseer.Properties.Resources.Frame;
			lbl_value.Text = global::Effekseer.Properties.Resources.Value;
			lbl_start.Text = global::Effekseer.Properties.Resources.Start;
			lbl_end.Text = global::Effekseer.Properties.Resources.End;
			lbl_type.Text = global::Effekseer.Properties.Resources.Complement;
			lbl_sampling.Text = global::Effekseer.Properties.Resources.Sampling;
			lbl_left.Text = global::Effekseer.Properties.Resources.Left;
			lbl_right.Text = global::Effekseer.Properties.Resources.Right;
			lbl_offset.Text = global::Effekseer.Properties.Resources.Offset;
			lbl_offset_min.Text = global::Effekseer.Properties.Resources.Min;
			lbl_offset_max.Text = global::Effekseer.Properties.Resources.Max;
		
		}

		Data.Value.IFCurveKey editedFCurveKey = null;

		Utl.ParameterTreeNode paramaterTreeNode = null;

		TreeNode treeNodes = null;

		float fCurvesMagXCount = 2;

		bool loading = false;

		/// <summary>
		/// グラフのX方向拡大率
		/// </summary>
		float FCurvesMagnificationX { get { return (float)Math.Pow(2.0, fCurvesMagXCount); } }

		/// <summary>
		/// グラフのX方向移動
		/// </summary>
		float fCurvesOffsetX = 0;

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

			// 初期設定
			if (treeNodes == null)
			{
				treeNodes = new TreeNode(this, paramTreeNodes);
			}

			// ノードの構造比較変更
			if (treeNodes.ParamTreeNode.Node != paramTreeNodes.Node)
			{
				treeNodes = new TreeNode(this, paramTreeNodes);
			}
			else
			{
				Action<Utl.ParameterTreeNode, TreeNode> refleshNodes = null;
				refleshNodes = (ptn, tn) =>
					{
						// 変更がないか確認
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

						// 変更があった場合
						var a = new TreeNode[ptn.Children.Count()];

						for (int i = 0; i < ptn.Children.Count(); i++)
						{
							TreeNode tn_ = tn.Children.FirstOrDefault(_ => _.ParamTreeNode.Node == ptn.Children[i].Node);
							if (tn_ != null)
							{
								// コピー
								refleshNodes(ptn.Children[i], tn_);
								a[i] = tn_;
							}
							else
							{
								// 新規
								a[i] = new TreeNode(this, ptn.Children[i]);
							}
						}
						
						tn.Children.Clear();
						tn.Children.AddRange(a);
					};

				refleshNodes(paramTreeNodes, treeNodes);
			}

			// パラメーターの構造比較変更
			{
				Action<Utl.ParameterTreeNode, TreeNode> refleshNodes = null;
				refleshNodes = (ptn, tn) =>
				{
					// 変更がないか確認
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

					// 変更があった場合
					var a = new FCurve[ptn.Parameters.Count()];

					for (int i = 0; i < ptn.Parameters.Count(); i++)
					{
						FCurve f = tn.FCurves.FirstOrDefault(_ => _.GetValueAsObject() == ptn.Parameters[i].Item2);
						if (f != null)
						{
							// コピー
							a[i] = f;
						}
						else
						{
							// 新規
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

			// 消滅イベント
			foreach (var f in befores)
			{
				if (!afters.Contains(f))
				{
					f.OnRemoved(this.splitContainer.Panel2);
				}
			}

			// 追加イベント
			foreach (var f in afters)
			{
				if (!befores.Contains(f))
				{
					f.OnAdded(this.splitContainer.Panel2);
				}
			}

			treeNodes.CalculatePosition(Math.Max(0, splitContainer.Panel2.Width - 20));

			paramaterTreeNode = paramTreeNodes;
		}

		void FCurves_HandleCreated(object sender, EventArgs e)
		{
			Command.CommandManager.Changed += OnChanged;
			Core.OnAfterNew += OnChanged;
			Core.OnBeforeLoad += OnBeforeLoad;
			Core.OnAfterLoad += OnAfterLoad;
			OnChanged();
		}

		void FCurves_HandleDestroyed(object sender, EventArgs e)
		{
			Command.CommandManager.Changed -= OnChanged;
			Core.OnAfterNew -= OnChanged;
			Core.OnBeforeLoad -= OnBeforeLoad;
			Core.OnAfterLoad -= OnAfterLoad;
		}

		public void ScrollPosition(object o)
		{
			if (treeNodes == null) return;

			int y = 0;

			Action<TreeNode> getX = null;
			getX = (t) =>
				{
					var v = t.FCurves.Where(_=>_.GetValueAsObject() == o).FirstOrDefault();
					if(v != null)
					{
						y = v.Position.Y;
						return;
					}

					foreach (var t_ in t.Children)
					{
						getX(t_);
					}
				};

			getX(treeNodes);

			splitContainer.Panel2.AutoScrollPosition = new Point(0,y);

			Refresh();
		}

		/// <summary>
		/// 設定可能なグラフを指定する。
		/// </summary>
		/// <param name="graph"></param>
		public void SetGprah()
		{
			cb_fcurveEdgeStart.SetBinding(null);
			cb_fcurveEdgeEnd.SetBinding(null);
			txt_offset_max.SetBinding(null);
			txt_offset_min.SetBinding(null);
			txt_sampling.SetBinding(null);
		}

		/// <summary>
		/// 設定可能なグラフを指定する。
		/// </summary>
		/// <param name="graph"></param>
		public void SetGprah(Data.Value.IFCurve graph)
		{
			cb_fcurveEdgeStart.SetBinding(graph.StartType);
			cb_fcurveEdgeEnd.SetBinding(graph.EndType);
			txt_offset_max.SetBinding(graph.OffsetMax);
			txt_offset_min.SetBinding(graph.OffsetMin);
			txt_sampling.SetBinding(graph.Sampling);
		}

		public void SetFCurveKey()
		{
			editedFCurveKey = null;
			cb_fcurveInterpolation.SetBinding(null);
			txt_frame.Reload();
			txt_value.Reload();
			txt_offset_max.Reload();
			txt_offset_min.Reload();

			txt_left_x.Reload();
			txt_left_y.Reload();
			txt_right_x.Reload();
			txt_right_y.Reload();
		}

		public void SetFCurveKey(Data.Value.IFCurveKey fcurve)
		{
			editedFCurveKey = fcurve;
			cb_fcurveInterpolation.SetBinding(fcurve.InterpolationType);

			txt_frame.Reload();
			txt_value.Reload();
			txt_offset_max.Reload();
			txt_offset_min.Reload();

			txt_left_x.Reload();
			txt_left_y.Reload();
			txt_right_x.Reload();
			txt_right_y.Reload();
		}

		public void UpdateFCurveKey()
		{
			txt_frame.Reload();
			txt_value.Reload();
			txt_offset_max.Reload();
			txt_offset_min.Reload();

			txt_left_x.Reload();
			txt_left_y.Reload();
			txt_right_x.Reload();
			txt_right_y.Reload();
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
			Refresh();
		}

		private void splitContainer1_Panel1_Paint(object sender, PaintEventArgs e)
		{
			
			if (treeNodes != null)
			{
				treeNodes.RenderNames(e.Graphics, -splitContainer.Panel2.AutoScrollPosition.Y);
			}
		}


		private void splitContainer_Panel2_SizeChanged(object sender, EventArgs e)
		{
			if (treeNodes != null)
			{
				treeNodes.CalculatePosition(Math.Max(0, splitContainer.Panel2.Width - 20));
			}
		}

		private void splitContainer_Panel2_Scroll(object sender, ScrollEventArgs e)
		{
			splitContainer.Panel1.Refresh();
		}

		private void splitContainer_Panel1_MouseDown(object sender, MouseEventArgs e)
		{
			if (treeNodes != null)
			{
				if (treeNodes.ClickOnPanel(e.X, e.Y - splitContainer.Panel2.AutoScrollPosition.Y))
				{
					treeNodes.CalculatePosition(Math.Max(0, splitContainer.Panel2.Width - 20));
					splitContainer.Panel1.Refresh();
				}
			}
		}

		private void FCurves_MouseDown(object sender, MouseEventArgs e)
		{

		}

		private void FCurves_MouseUp(object sender, MouseEventArgs e)
		{

		}

		private void FCurves_MouseMove(object sender, MouseEventArgs e)
		{

		}

		class TreeNode
		{
			public Utl.ParameterTreeNode ParamTreeNode { get; private set; }

			public List<TreeNode> Children { get; private set; }

			public Point Position { get; private set; }

			public int Height { get; private set; }

			public List<FCurve> FCurves { get; private set; }

			public bool IsExtended = true;

			FCurves window = null;

			public TreeNode(FCurves window, Utl.ParameterTreeNode paramTreeNode)
			{
				this.window = window;
				
				Children = new List<TreeNode>();
				FCurves = new List<FCurve>();

				ParamTreeNode = paramTreeNode;

				foreach (var tn in paramTreeNode.Children)
				{ 
					Children.Add(new TreeNode(window,tn));
				}

				foreach (var v in paramTreeNode.Parameters)
				{
					var v_ = FCurve.Create(v, window);
					v_.ParentNode = this;

					FCurves.Add(v_);
				}
			}

			const int upperleftX = 5;
			const int upperleftY = 5;
			const int fontSize = 12;
			const int mergin = 2;
			const int indent = 10;
			const int paramOffset = 25;

			public void CalculatePosition(int width)
			{
				int indentCount = 1;

				// 初期位置
				Position = new Point(upperleftX + indent * indentCount, upperleftY);
				Height = fontSize + mergin * 2;

				int childY = Position.Y + Height;

				foreach (var child in Children)
				{
					child.CalculatePosition(childY, indentCount + 1, IsExtended, width);
					childY += child.Height;
				}
				
			}

			void CalculatePosition(int y, int indentCount, bool isParentExtended, int width)
			{
				Position = new Point(indent * indentCount, y);

				if (isParentExtended)
				{
					Height = fontSize + mergin * 2;
				}
				else
				{
					Height = 0;
				}

				int childY = Position.Y + Height;

				foreach (var f in FCurves)
				{
					f.Position = new Point(paramOffset + indent * indentCount, childY);
					f.SetSize(IsExtended && isParentExtended, width);

					if (IsExtended && isParentExtended)
					{
						Height += Math.Max(f.Height, fontSize + mergin);
					}
					childY = Position.Y + Height;
				}
				
				foreach (var child in Children)
				{
					child.CalculatePosition(childY, indentCount + 1, IsExtended && isParentExtended, width);
					Height += child.Height;
					childY = Position.Y + Height;
				}
			}

			/// <summary>
			/// 文字部分をクリックした時のイベントを発生させる。
			/// </summary>
			/// <param name="x"></param>
			/// <param name="y"></param>
			/// <returns>何かが変更されたか</returns>
			public bool ClickOnPanel(int x, int y)
			{
				bool changed = false;

				// 拡張クリック
				int offsetX = Position.X - 12;
				int offsetY = Position.Y + 2;
				if (IsArrowClicked(offsetX, offsetY, x, y))
				{
					IsExtended = !IsExtended;
					changed = changed || true;
				}

				foreach (var f in FCurves)
				{
					if (f.IsAllowClicked(x, y))
					{
						f.IsExtended = !f.IsExtended;
						changed = changed || true;
					}
				}

				foreach (var child in Children)
				{
					changed =  changed || child.ClickOnPanel(x, y);
				}

				return changed;
			}

			public void RenderNames(Graphics g, int offset)
			{
				// 拡張描画
				int offsetX = Position.X - 12;
				int offsetY = Position.Y + 2 - offset;
				DrawArrow(g, offsetX, offsetY, IsExtended);

				// ノード名描画
				TextRenderer.DrawText(
					g,
					ParamTreeNode.Node.Name,
					window.Font,
					new Point(Position.X,Position.Y + mergin - offset),
					Color.Black);

				if (IsExtended)
				{
					foreach (var f in FCurves)
					{
						f.RenderNames(g, offset);
					}

					foreach (var child in Children)
					{
						child.RenderNames(g, offset);
					}
				}
			}

			public void RenderFCurves(Graphics g)
			{ 
			
			}
		}

		static bool IsArrowClicked(int x, int y, int clickedX,int clickedY)
		{
			if (x - 2 < clickedX &&
				 clickedX < x -2 + 12 &&
				y < clickedY &&
				clickedY < y + 12)
			{
				return true;
			}
			return false;
		}

		static void DrawArrow(Graphics g, int x, int y, bool isExtended)
		{
			int longEdge = 9;
			if (isExtended)
			{
				g.FillPolygon(
					System.Drawing.Brushes.White,
					new Point[] { 
					new Point(x, y),
					new Point(x + longEdge, y),
					new Point(x + longEdge / 2, y + 5),});
			}
			else
			{
				g.FillPolygon(
						System.Drawing.Brushes.White,
						new Point[] { 
						new Point(x, y),
						new Point(x, y + longEdge),
						new Point(x + 5, y + longEdge / 2),});
			}
		}


		abstract class FCurve
		{
			public abstract object GetValueAsObject();

			public Color ColorBackground = Color.FromArgb(255, 144, 144, 144);
			public Color ColorBackgroundInvalid = Color.FromArgb(255, 68, 68,68);
			public Color ColorGridLine = Color.FromArgb(255, 91, 91, 91);
			public Color ColorGridCenterLine = Color.FromArgb(255, 0, 0, 0);

			public string Name { get; protected set; }

			public TreeNode ParentNode { get; set; }

			public bool IsExtended { get; set; }
			bool isParentExtended = false;

			public int Height
			{
				get
				{
					if (IsExtended && isParentExtended) return 150;
					return 0;
				}
			}
			public int Width = 400;

			public Point Position;

			protected FCurves window = null;

			public DrawnPanel GraphPanel { get; private set; }
			bool added = false;

			public IObservable<System.Windows.Forms.MouseEventArgs> MouseDownObservable { get; private set; }
			public IObservable<System.Windows.Forms.MouseEventArgs> MouseMoveObservable { get; private set; }
			public IObservable<System.Windows.Forms.MouseEventArgs> MouseUpObservable { get; private set; }
			public IObservable<System.Windows.Forms.MouseEventArgs> MouseWheelObservable { get; private set; }
			public bool EventFlag = false;
			public List<IDisposable> DisposingEvents = new List<IDisposable>();

			public Point MousePosition { get; private set; }

			/// <summary>
			/// 選択されているポイント
			/// </summary>
			protected HashSet<SelectablePoint> selectedPoints = new HashSet<SelectablePoint>();


			protected HashSet<SelectablePoint> preSelectedPoints = new HashSet<SelectablePoint>();

			/// <summary>
			/// グラフのY方向拡大率のカウンタ
			/// </summary>
			protected float fCurvesMagYCount = 0;

			/// <summary>
			/// グラフのY方向拡大率
			/// </summary>
			protected float FCurvesMagnificationY { get { return (float)Math.Pow(2.0f, fCurvesMagYCount); } }

			/// <summary>
			/// グラフのY方向移動
			/// </summary>
			protected float fCurvesOffsetY = 0;

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

			protected FCurve()
			{
				GraphPanel = new DrawnPanel();
				GraphPanel.MouseDown += new MouseEventHandler(GraphPanel_MouseDown);
				GraphPanel.MouseWheel += new MouseEventHandler(GraphPanel_MouseWheel);
				GraphPanel.MouseHover += new EventHandler(GraphPanel_MouseHover);
				GraphPanel.MouseMove += new MouseEventHandler(GraphPanel_MouseMove);
				GraphPanel.KeyDown += new KeyEventHandler(GraphPanel_KeyDown);
				GraphPanel.KeyUp += new KeyEventHandler(GraphPanel_KeyUp);
				GraphPanel.Paint += new PaintEventHandler(GraphPanel_Paint);

				MouseDownObservable = System.Reactive.Linq.Observable.FromEvent<
					System.Windows.Forms.MouseEventHandler,
					System.Windows.Forms.MouseEventArgs>(
					h => (s, e) => h(e),
					h => GraphPanel.MouseDown += h,
					h => GraphPanel.MouseDown -= h);

				MouseUpObservable = System.Reactive.Linq.Observable.FromEvent<
					System.Windows.Forms.MouseEventHandler,
					System.Windows.Forms.MouseEventArgs>(
					h => (s, e) => h(e),
					h => GraphPanel.MouseUp += h,
					h => GraphPanel.MouseUp -= h);

				MouseMoveObservable = System.Reactive.Linq.Observable.FromEvent<
					System.Windows.Forms.MouseEventHandler,
					System.Windows.Forms.MouseEventArgs>(
					h => (s, e) => h(e),
					h => GraphPanel.MouseMove += h,
					h => GraphPanel.MouseMove -= h);

				MouseWheelObservable = System.Reactive.Linq.Observable.FromEvent<
					System.Windows.Forms.MouseEventHandler,
					System.Windows.Forms.MouseEventArgs>(
					h => (s, e) => h(e),
					h => GraphPanel.MouseWheel += h,
					h => GraphPanel.MouseWheel -= h);

				GraphPanel.MouseWheel_ = (delta) =>
					{
						if (EventFlag) return;
						MagnifiyScreen(delta);
					};

				IsExtended = true;

				IsShiftHold = false;
			}

			public bool IsAllowClicked(int x, int y)
			{ 
				int offsetX = Position.X - 12;
				int offsetY = Position.Y + 2;
				return FCurves.IsArrowClicked(offsetX, offsetY, x, y);
			}

			public void RenderNames(Graphics g,int offset)
			{
				int offsetX = Position.X - 12;
				int offsetY = Position.Y + 2 - offset;

				DrawArrow(g, offsetX, offsetY, IsExtended);

				TextRenderer.DrawText(
					g,
					Name,
					window.Font,
					new Point(Position.X, Position.Y - offset),
					Color.Black);
			}

			public virtual void OnAdded(Panel panelTree)
			{
				panelTree.Controls.Add(GraphPanel);
				GraphPanel.BackColor = ColorBackground;

				added = true;
			}

			public virtual void OnRemoved(Panel panelTree)
			{
				panelTree.Controls.Remove(GraphPanel);
				if (!added) throw new Exception("already added.");
				added = false;
			}

			public void SetSize(bool isParentExtended, int width)
			{
				this.isParentExtended = isParentExtended;
				Width = width;
				GraphPanel.Size = new Size(Width, Height);

				if (Height > 0)
				{
					GraphPanel.Location = new Point(0, Position.Y + ((Panel)GraphPanel.Parent).AutoScrollPosition.Y);
				}
				else
				{ 
					// スクロールバー対策
					GraphPanel.Location = new Point(0, Position.Y + ((Panel)GraphPanel.Parent).AutoScrollPosition.Y + 14);
				}
			}

			protected virtual void Paint(Graphics g)
			{ 
			}

			void GraphPanel_Paint(object sender, PaintEventArgs e)
			{
				Paint(e.Graphics);
			}

			public bool IsShiftHold { get; private set; }
			bool isCtrlHold = false;

			protected virtual void KeyDown(Keys e)
			{ 
			}

			void GraphPanel_KeyDown(object sender, KeyEventArgs e)
			{
				if (e.Shift) IsShiftHold = true;
				if (e.Control) isCtrlHold = true;
				KeyDown(e.KeyCode);
			}

			void GraphPanel_KeyUp(object sender, KeyEventArgs e)
			{
				if (e.KeyCode== Keys.ShiftKey) IsShiftHold = false;
				if (e.KeyCode == Keys.ControlKey) isCtrlHold = false;
			}

			/// <summary>
			/// 中央ボタンのドラッグでスクリーンを移動する処理を追加する。
			/// </summary>
			/// <param name="p"></param>
			void AddScreenMoveEvent(Point p)
			{
				EventFlag = true;

				int mousePosX = p.X;
				int mousePosY = p.Y;

				var e1 = MouseMoveObservable.
					Repeat().
					Subscribe( _ =>
					{
						window.fCurvesOffsetX -= (float)((_.X - mousePosX)) / window.FCurvesMagnificationX;
						fCurvesOffsetY += (float)((_.Y - mousePosY)) / FCurvesMagnificationY;
						GraphPanel.Parent.Refresh();

						mousePosX = _.X;
						mousePosY = _.Y;
					});

				var e2 = MouseUpObservable.
					Subscribe(_ =>
					{
						foreach (var e in DisposingEvents) e.Dispose();
						EventFlag = false;
					});

				DisposingEvents.Add(e1);
				DisposingEvents.Add(e2);
			}

			void GraphPanel_MouseDown(object sender, MouseEventArgs e)
			{
				if (e.Button == System.Windows.Forms.MouseButtons.Left)
				{
					if (!EventFlag)
					{
						MouseDown(e.X, e.Y);
					}
				}

				if (e.Button == System.Windows.Forms.MouseButtons.Middle)
				{
					if (!EventFlag)
					{
						AddScreenMoveEvent(e.Location);
					}
				}
			}

			void GraphPanel_MouseWheel(object sender, MouseEventArgs e)
			{
				//if (EventFlag) return;
				//
				//if (e.X >= this.GraphPanel.Width) return;
				//if (e.X <= 0) return;
				//if (e.Y >= this.GraphPanel.Height) return;
				//if (e.Y <= 0) return;
				//
				//MagnifiyScreen(e.Delta);
			}

			private void MagnifiyScreen(int delta)
			{
				if (delta > 0)
				{	
					if ((!IsShiftHold && !isCtrlHold) || (IsShiftHold && !isCtrlHold))
					{
						var v = ScreenToValue(MousePosition.X, MousePosition.Y);
						window.fCurvesMagXCount += 0.1f;

						var dx = v.X * window.FCurvesMagnificationX - MousePosition.X;
						window.fCurvesOffsetX = dx / window.FCurvesMagnificationX;
					}

					if ((!IsShiftHold && !isCtrlHold) || (!IsShiftHold && isCtrlHold))
					{
						var v = ScreenToValue(MousePosition.X, MousePosition.Y);
						fCurvesMagYCount += 0.1f;

						var y_ = -((v.Y) * FCurvesMagnificationY) + GraphPanel.Height / 2;
						var dy = y_ - MousePosition.Y;
						fCurvesOffsetY = - dy / FCurvesMagnificationY;
					}
				}
				else
				{
					if ((!IsShiftHold && !isCtrlHold) || (IsShiftHold && !isCtrlHold))
					{
						var v = ScreenToValue(MousePosition.X, MousePosition.Y);
						window.fCurvesMagXCount -= 0.1f;

						var dx = v.X * window.FCurvesMagnificationX - MousePosition.X;
						window.fCurvesOffsetX = dx / window.FCurvesMagnificationX;
					}

					if ((!IsShiftHold && !isCtrlHold) || (!IsShiftHold && isCtrlHold))
					{
						var v = ScreenToValue(MousePosition.X, MousePosition.Y);
						fCurvesMagYCount -= 0.1f;

						var y_ = -((v.Y) * FCurvesMagnificationY) + GraphPanel.Height / 2;
						var dy = y_ - MousePosition.Y;
						fCurvesOffsetY = - dy / FCurvesMagnificationY;
					}
				}

				GraphPanel.Parent.Refresh();
			}

			protected void DrawLine(Graphics g, Data.Value.IFCurve keys, Color color)
			{
				int penSize = selectedPoints.Any(_ => _.FCurve == keys) ? 2 : 1;
				var pen = new Pen(color, penSize);

				Action<int, int> drawLine = (s, e) =>
				{
					var p0 = new PointF(s, 0);
					var p1 = new PointF(s + 2, 0);

					var fv0 = p0;
					fv0.Y = keys.GetValue((int)p0.X);
					p0 = ValueToScreen(fv0.X, fv0.Y);

					for (float x = s + 2; x < e; x += 2)
					{
						p1.X = x;
						var fv1 = p1;
						fv1.Y = keys.GetValue((int)p1.X);
						p1 = ValueToScreen(fv1.X, fv1.Y);

						g.DrawLine(
							pen,
							p0,
							p1);

						p0 = p1;
					}

					{
						p1.X = e;
						var fv1 = p1;
						fv1.Y = keys.GetValue((int)p1.X);
						p1 = ValueToScreen(fv1.X, fv1.Y);

						g.DrawLine(
							pen,
							p0,
							p1);
					}
				};

				var leftXonV = ScreenToValue(0, 0).X;
				var rightXonV = ScreenToValue(GraphPanel.Width, 0).X;



				Data.Value.IFCurveKey kv = null;
				foreach (var key in keys.Keys)
				{
					if (kv == null)
					{
						drawLine((int)leftXonV, key.Frame);
					}
					else
					{
						drawLine(kv.Frame, key.Frame);
					}

					kv = key;
				}

				if (kv != null)
				{
					drawLine(kv.Frame, (int)rightXonV);
				}

				if (keys.Keys.Count() == 0)
				{
					drawLine((int)leftXonV, (int)rightXonV);
				}

				// 点の描画
				foreach (var key in keys.Keys)
				{
					var c = selectedPoints.Where(_ => _.Key == key && _.Type == SelectablePoint.PointType.Center).FirstOrDefault();
					var l = selectedPoints.Where(_ => _.Key == key && _.Type == SelectablePoint.PointType.Left).FirstOrDefault();
					var r = selectedPoints.Where(_ => _.Key == key && _.Type == SelectablePoint.PointType.Right).FirstOrDefault();

					var _c = preSelectedPoints.Where(_ => _.Key == key && _.Type == SelectablePoint.PointType.Center).FirstOrDefault();
					var _l = preSelectedPoints.Where(_ => _.Key == key && _.Type == SelectablePoint.PointType.Left).FirstOrDefault();
					var _r = preSelectedPoints.Where(_ => _.Key == key && _.Type == SelectablePoint.PointType.Right).FirstOrDefault();

					var p = ValueToScreen(key.Frame, key.ValueAsFloat);
					var pl = ValueToScreen(key.LeftX, key.LeftY);
					var pr = ValueToScreen(key.RightX, key.RightY);

					if (l != null || r != null || c != null)
					{
						g.DrawLine(
							Pens.Black,
							p,
							pl);

						g.DrawLine(
							Pens.Black,
							p,
							pr);
					}


					if (l != null)
					{
						g.FillEllipse(
							Brushes.LightPink,
							new Rectangle(pl.X - 4, pl.Y - 4, 9, 9));
					}
					else if (_l != null)
					{
						g.FillEllipse(
							Brushes.DeepSkyBlue,
							new Rectangle(pl.X - 4, pl.Y - 4, 9, 9));
					}
					else if (l != null || r != null || c != null)
					{
						
						g.FillEllipse(
							Brushes.DeepPink,
							new Rectangle(pl.X - 4, pl.Y - 4, 9, 9));
					}
					

					if (r != null)
					{
						g.FillEllipse(
							Brushes.LightPink,
							new Rectangle(pr.X - 4, pr.Y - 4, 9, 9));
					}
					else if (_r != null)
					{
						g.FillEllipse(
							Brushes.DeepSkyBlue,
							new Rectangle(pr.X - 4, pr.Y - 4, 9, 9));
					}
					else if (l != null || r != null || c != null)
					{
						g.FillEllipse(
							Brushes.DeepPink,
							new Rectangle(pr.X - 4, pr.Y - 4, 9, 9));
					}
					

					if (c != null)
					{
						g.FillEllipse(
							Brushes.LightPink,
							new Rectangle(p.X - 2, p.Y - 2, 5, 5));
					}
					else if (_c != null)
					{
						g.FillEllipse(
							Brushes.DeepSkyBlue,
							new Rectangle(p.X - 2, p.Y - 2, 5, 5));
					}
					else
					{
						g.FillEllipse(
							Brushes.DeepPink,
							new Rectangle(p.X - 2, p.Y - 2, 5, 5));
					}
					
				}
			}

			/// <summary>
			/// グリッドを描画する。
			/// </summary>
			/// <param name="g"></param>
			protected void DrawGrids(Graphics g)
			{
				var ul = ScreenToValue(0, 0);
				var dr = ScreenToValue(GraphPanel.Width, GraphPanel.Height);

				var frameLenByPixel = Math.Abs(dr.X - ul.X) / GraphPanel.Width;
				var gridFrame = (int)(frameLenByPixel * 30.0f * 10000) / 10000;

				if (gridFrame > 4)
				{
					gridFrame = ((int)(gridFrame + 4.0f) / 5) * 5;
				}
				else if (gridFrame > 1)
				{
					gridFrame = ((int)(gridFrame + 1.0f) / 2) * 2;
				}
				else
				{
					gridFrame = 1;
				}


				var valueLenByPixel = Math.Abs(dr.Y - ul.Y) / GraphPanel.Height;
				var gridValue = (valueLenByPixel * 30.0f);

				if (gridValue > 4)
				{
					gridValue = ((int)(gridValue + 4.0f) / 5) * 5;
				}
				else if (gridValue > 1)
				{
					gridValue = ((int)(gridValue + 1.0f) / 2) * 2;
				}
				else if (gridValue > 0.1f)
				{
					gridValue = ((int)(gridValue * 10)) / 10.0f;
				}
				else if (gridValue > 0.01f)
				{
					gridValue = ((int)(gridValue * 100)) / 100.0f;
				}

				for (var x = ul.X - (ul.X % gridFrame); x <= dr.X; x += gridFrame)
				{
					Pen pen = new Pen(ColorGridLine);
					if (x == 0.0f) pen = new Pen(ColorGridCenterLine);
					g.DrawLine(pen, ValueToScreen(x, ul.Y), ValueToScreen(x, dr.Y));
				}

				for (var y = dr.Y - (dr.Y % gridValue); y <= ul.Y; y += gridValue)
				{
					Pen pen = new Pen(ColorGridLine);
					if (y == 0.0f) pen = new Pen(ColorGridCenterLine);
					g.DrawLine(pen, ValueToScreen(ul.X, y), ValueToScreen(dr.X, y));
				}

				for (var x = ul.X - (ul.X % gridFrame); x <= dr.X; x += gridFrame)
				{
					var x_ = ValueToScreen(x, dr.Y);
					var s = x.ToString();

					var size = TextRenderer.MeasureText(s, window.Font);

					TextRenderer.DrawText(
						g,
						s,
						window.Font,
						new Point(x_.X - size.Width / 2, GraphPanel.Height - size.Height),
						Color.Black);
				}

				for (var y = dr.Y - (dr.Y % gridValue); y <= ul.Y; y += gridValue)
				{
					var y_ = ValueToScreen(ul.X, y);

					var s = string.Empty;

					if (gridValue >= 1)
					{
						s = ((int)y).ToString();
					}
					else if (gridValue >= 0.1)
					{
						s = y.ToString("F1");
					}
					else if (gridValue >= 0.01)
					{
						s = y.ToString("F2");
					}

					var size = TextRenderer.MeasureText(s, window.Font);

					TextRenderer.DrawText(
						g,
						s,
						window.Font,
						new Point(0, y_.Y - size.Height / 2),
						Color.Black);
				}
			}

			void GraphPanel_MouseHover(object sender, EventArgs e)
			{
				GraphPanel.Focus();
			}

			void GraphPanel_MouseMove(object sender, MouseEventArgs e)
			{
				MousePosition = e.Location;

				var all = GetAllSelectablePoints();

				preSelectedPoints.Clear();
				
				var selected = GetSelectablePoints(e.X, e.Y).ToArray();
				foreach(var s in selected)
				{
					preSelectedPoints.Add(s);
				}

				GraphPanel.Refresh();
			}

			protected void MouseDown(int x, int y)
			{
				var fv = ScreenToValue(x, y);

				// 選択
				var selected = GetSelectablePoints(x, y).ToArray();

				if (selected.Any())
				{
					// 点を選択

					if (IsShiftHold)
					{
						// 追加選択
						foreach (var s in selected)
						{
							if (!selectedPoints.Any(_ => _.Equals(s)))
							{
								selectedPoints.Add(s);
							}
						}
					}
					else
					{
						// 新規選択(クリックするごとに選択がループするようにする)
						if (selectedPoints.Count >= 2 || selectedPoints.Count == 0)
						{
							selectedPoints.Clear();

							foreach (var s in selected)
							{
								selectedPoints.Add(s);
								break;
							}
						}
						else
						{
							var selectedPoint = selectedPoints.First();
							selectedPoints.Clear();

							bool found = false;

							foreach (var s in selected)
							{
								if (found)
								{
									selectedPoints.Add(s);
									break;
								}
								else
								{
									if (selectedPoint.Equals(s)) found = true;
								}
							}

							if (selectedPoints.Count == 0)
							{
								foreach (var s in selected)
								{
									selectedPoints.Add(s);
									break;
								}
							}
						}
					}

					// グラフ単体に反映
					GraphPanel.Refresh();

					EventFlag = true;

					int mousePosX = x;
					int mousePosY = y;

					var leftPoint = selected.FirstOrDefault(_ => _.Type == SelectablePoint.PointType.Left);
					var rightPoint = selected.FirstOrDefault(_ => _.Type == SelectablePoint.PointType.Right);

					// 移動イベントを追加
					if (!selectedPoints.Any(_ => _.Type != SelectablePoint.PointType.Center))
					{
						var e1 = MouseMoveObservable.
							Repeat().
							Subscribe(_ =>
							{
								var fv_ = ScreenToValue(_.X, _.Y);
								foreach (var s in selectedPoints)
								{
									s.Key.SetFrame(s.Key.FramePrevious + (int)(fv_.X - fv.X));
									s.Key.SetValueAsFloat(s.Key.ValuePreviousAsFloat + (fv_.Y - fv.Y));

								}

								// グラフ全体に反映
								GraphPanel.Parent.Refresh();

								// エフェクトに反映
								GUIManager.DockViewer.Viewer.IsChanged = true;

								// 数値に反映
								window.UpdateFCurveKey();
							});

						var e2 = MouseUpObservable.
							Subscribe(_ =>
							{
								foreach (var s in selectedPoints)
								{
									s.Key.Commit();
								}

								foreach (var e in DisposingEvents) e.Dispose();
								EventFlag = false;

								// グラフ全体に反映
								GraphPanel.Parent.Refresh();
							});

						DisposingEvents.Add(e1);
						DisposingEvents.Add(e2);
					}
					else if (selectedPoints.Contains(leftPoint))
					{
						var e1 = MouseMoveObservable.
								Repeat().
								Subscribe(_ =>
								{
									var fv_ = ScreenToValue(_.X, _.Y);

									leftPoint.Key.SetLeft(
											leftPoint.Key.LeftXPrevious + (fv_.X - fv.X),
											leftPoint.Key.LeftYPrevious + (fv_.Y - fv.Y));

									// グラフ全体に反映
									GraphPanel.Parent.Refresh();

									// エフェクトに反映
									GUIManager.DockViewer.Viewer.IsChanged = true;

									// 数値に反映
									window.UpdateFCurveKey();
								});

						var e2 = MouseUpObservable.
							Subscribe(_ =>
							{
								foreach (var s in selectedPoints)
								{
									s.Key.Commit();
								}

								foreach (var e in DisposingEvents) e.Dispose();
								EventFlag = false;

								// グラフ全体に反映
								GraphPanel.Parent.Refresh();
							});

						DisposingEvents.Add(e1);
						DisposingEvents.Add(e2);
					}
					else if (selectedPoints.Contains(rightPoint))
					{
						var e1 = MouseMoveObservable.
									Repeat().
									Subscribe(_ =>
									{
										var fv_ = ScreenToValue(_.X, _.Y);

										rightPoint.Key.SetRight(
											rightPoint.Key.RightXPrevious + (fv_.X - fv.X),
											rightPoint.Key.RightYPrevious + (fv_.Y - fv.Y));

										// グラフ全体に反映
										GraphPanel.Parent.Refresh();

										// エフェクトに反映
										GUIManager.DockViewer.Viewer.IsChanged = true;

										// 数値に反映
										window.UpdateFCurveKey();
									});

						var e2 = MouseUpObservable.
							Subscribe(_ =>
							{
								foreach (var s in selectedPoints)
								{
									s.Key.Commit();
								}

								foreach (var e in DisposingEvents) e.Dispose();
								EventFlag = false;

								// グラフ全体に反映
								GraphPanel.Parent.Refresh();
							});

						DisposingEvents.Add(e1);
						DisposingEvents.Add(e2);
					}


					// 設定可能グラフ設定
					SetGraphToPanel();

					SetFCurveKey();
				}
				else
				{
					selectedPoints.Clear();

					// 設定可能グラフ設定
					SetGraphToPanel();

					SetFCurveKey();

					GraphPanel.Refresh();
				}
			}

			protected void SetFCurveKey()
			{
				if (selectedPoints.Count() == 1)
				{
					window.SetFCurveKey(selectedPoints.First().Key);
				}
				else
				{
					window.SetFCurveKey();
				}
			}

			protected void SetGraphToPanel()
			{
				var selectedGraphs = selectedPoints.Select(_ => _.FCurve);
				if (selectedGraphs.Count() == 1)
				{
					window.SetGprah(selectedGraphs.First());
				}
				else
				{
					window.SetGprah();
				}
			}

			protected void DrawFrame(Graphics g)
			{
				Pen pen = Pens.Gray;
				g.DrawLine(pen, new Point(0, 0), new Point(GraphPanel.Width - 1, 0));
				g.DrawLine(pen, new Point(0, GraphPanel.Height - 1), new Point(GraphPanel.Width - 1, GraphPanel.Height - 1));
				g.DrawLine(pen, new Point(0, 0), new Point(0, GraphPanel.Height - 1));
				g.DrawLine(pen, new Point(GraphPanel.Width - 1, 0), new Point(GraphPanel.Width - 1, GraphPanel.Height - 1));
			}

			protected void DrawBackground(Graphics g)
			{
				var node = ParentNode.ParamTreeNode.Node as Data.Node;
				var life = node.CommonValues.Life.GetCenter();
				var width = GraphPanel.Width;

				var left = ValueToScreen(0, 0);
				var right = ValueToScreen(life, 0);
				var rightX = Math.Min(right.X, width);
				g.FillRectangle(
					new SolidBrush(ColorBackgroundInvalid),
					new Rectangle(Math.Max(0, rightX), 0, Math.Max(rightX, width), Height));
			}

			protected PointF ScreenToValue(int x, int y)
			{
				float x_ = 0.0f;
				float y_ = 0.0f;

				y_ = -y + GraphPanel.Height / 2;
				x_ = x / window.FCurvesMagnificationX;
				y_ = y_ / FCurvesMagnificationY;
				x_ += window.fCurvesOffsetX;
				y_ += fCurvesOffsetY;

				return new PointF(x_, y_);
			}

			protected Point ValueToScreen(float x, float y)
			{
				x = ((x - window.fCurvesOffsetX) * window.FCurvesMagnificationX);
				y = -((y - fCurvesOffsetY) * FCurvesMagnificationY) + GraphPanel.Height / 2;

				return new Point((int)x, (int)y);
			}

			virtual protected IEnumerable<SelectablePoint> GetSelectablePoints(int x, int y)
			{
				return null;
			}

			virtual protected IEnumerable<SelectablePoint> GetAllSelectablePoints()
			{
				return null;
			}


			/// <summary>
			/// 選択可能ポイント
			/// </summary>
			protected class SelectablePoint
			{
				public PointType Type = PointType.Center;

				public Data.Value.IFCurve FCurve = null;
				public Data.Value.IFCurveKey Key = null;

				public enum PointType
				{
					Center,
					Left,
					Right,
				}

				public SelectablePoint()
				{
				}

				public SelectablePoint(Data.Value.IFCurve fcurve, Data.Value.IFCurveKey key, PointType type)
				{
					FCurve = fcurve;
					Key = key;
					Type = type;
				}

				public override bool Equals(object obj)
				{
					var p = obj as SelectablePoint;

					return p.Type == Type && p.Key == Key;
				}
			}

			internal class DrawnPanel : Panel
			{
				public Action<int> MouseWheel_;

				public DrawnPanel()
				{
					SetStyle(ControlStyles.ResizeRedraw, true);
					SetStyle(ControlStyles.DoubleBuffer, true);
					SetStyle(ControlStyles.UserPaint, true);
					SetStyle(ControlStyles.AllPaintingInWmPaint, true);
				}

				protected override void WndProc(ref Message m)
				{
					if (m.Msg == 0x020A)
					{
						var pos = this.PointToClient(Cursor.Position);
						if (0 <= pos.X &&
							pos.X <= Size.Width &&
							0 <= pos.Y &&
							pos.Y <= Size.Height)
						{
							if ((int)m.WParam > 0)
							{
								MouseWheel_(120);
							}
							else if ((int)m.WParam < 0)
							{
								MouseWheel_(-120);
							}
						}
						else
						{
							base.WndProc(ref m);
						}
					}
					else
					{
						base.WndProc(ref m);
					}
				}
			}
		}

		class FCurveColor : FCurve
		{
			public Data.Value.FCurveColorRGBA Value { get; private set; }

			public FCurveColor(string name, Data.Value.FCurveColorRGBA value, FCurves window)
			{
				Name = name;
				Value = value;
				this.window = window;

				fCurvesMagYCount = (float)Math.Log((float)GraphPanel.Height / (value.R.DefaultValueRangeMax - value.R.DefaultValueRangeMin), 2.0);
				fCurvesOffsetY = (value.R.DefaultValueRangeMax + value.R.DefaultValueRangeMin) / 2.0f;
			}

			public override object GetValueAsObject()
			{
				return Value;
			}

			protected override IEnumerable<SelectablePoint> GetAllSelectablePoints()
			{
				var selectedX = Value.R.Keys.Select(_ => new SelectablePoint(Value.R, _, SelectablePoint.PointType.Center));

				var selectedY = Value.G.Keys.Select(_ => new SelectablePoint(Value.G, _, SelectablePoint.PointType.Center));

				var selectedZ = Value.B.Keys.Select(_ => new SelectablePoint(Value.B, _, SelectablePoint.PointType.Center));

				var selectedA = Value.A.Keys.Select(_ => new SelectablePoint(Value.A, _, SelectablePoint.PointType.Center));

				var selectedLeft = selectedPoints.Select(_ => new SelectablePoint(_.FCurve, _.Key, SelectablePoint.PointType.Left));

				var selectedRight = selectedPoints.Select(_ => new SelectablePoint(_.FCurve, _.Key, SelectablePoint.PointType.Right));

				return selectedX.Concat(selectedY.Concat(selectedZ.Concat(selectedA.Concat(selectedLeft.Concat(selectedRight)))));
			}

			protected override IEnumerable<SelectablePoint> GetSelectablePoints(int x, int y)
			{
				var fv = ScreenToValue(x, y);

				var selectableArea = new PointF(
						5.0f / window.FCurvesMagnificationX,
						5.0f / FCurvesMagnificationY);

				var selectableAreaAncor = new PointF(
						9.0f / window.FCurvesMagnificationX,
						9.0f / FCurvesMagnificationY);

				var selectedX = Value.R.Keys.Where(_ =>
					fv.X - selectableArea.X < _.Frame &&
					_.Frame < fv.X + selectableArea.X &&
					fv.Y - selectableArea.Y < _.ValueAsFloat &&
					_.ValueAsFloat < fv.Y + selectableArea.Y).Select(_ => new SelectablePoint(Value.R, _, SelectablePoint.PointType.Center));

				var selectedY = Value.G.Keys.Where(_ =>
					fv.X - selectableArea.X < _.Frame &&
					_.Frame < fv.X + selectableArea.X &&
					fv.Y - selectableArea.Y < _.ValueAsFloat &&
					_.ValueAsFloat < fv.Y + selectableArea.Y).Select(_ => new SelectablePoint(Value.G, _, SelectablePoint.PointType.Center));

				var selectedZ = Value.B.Keys.Where(_ =>
					fv.X - selectableArea.X < _.Frame &&
					_.Frame < fv.X + selectableArea.X &&
					fv.Y - selectableArea.Y < _.ValueAsFloat &&
					_.ValueAsFloat < fv.Y + selectableArea.Y).Select(_ => new SelectablePoint(Value.B, _, SelectablePoint.PointType.Center));

				var selectedA = Value.A.Keys.Where(_ =>
					fv.X - selectableArea.X < _.Frame &&
					_.Frame < fv.X + selectableArea.X &&
					fv.Y - selectableArea.Y < _.ValueAsFloat &&
					_.ValueAsFloat < fv.Y + selectableArea.Y).Select(_ => new SelectablePoint(Value.A, _, SelectablePoint.PointType.Center));

				var selectedLeft = selectedPoints.Where(_ =>
				{
					var p1 = new PointF(_.Key.LeftX, _.Key.LeftY);
					return fv.X - selectableAreaAncor.X < p1.X &&
						p1.X < fv.X + selectableAreaAncor.X &&
						fv.Y - selectableAreaAncor.Y < p1.Y &&
						p1.Y < fv.Y + selectableAreaAncor.Y;
				}).Select(_ => new SelectablePoint(_.FCurve, _.Key, SelectablePoint.PointType.Left));

				var selectedRight = selectedPoints.Where(_ =>
				{
					var p1 = new PointF(_.Key.RightX, _.Key.RightY);
					return fv.X - selectableAreaAncor.X < p1.X &&
						p1.X < fv.X + selectableAreaAncor.X &&
						fv.Y - selectableAreaAncor.Y < p1.Y &&
						p1.Y < fv.Y + selectableAreaAncor.Y;
				}).Select(_ => new SelectablePoint(_.FCurve, _.Key, SelectablePoint.PointType.Right));

				return selectedX.Concat(selectedY.Concat(selectedZ.Concat(selectedA.Concat(selectedLeft.Concat(selectedRight)))));
			}

			protected override void Paint(Graphics g)
			{
				DrawBackground(g);
				DrawFrame(g);

				DrawGrids(g);

				DrawColor(g);

				DrawLine(g, Value.R, Color.Red);
				DrawLine(g, Value.G, Color.LightGreen);
				DrawLine(g, Value.B, Color.Blue);
				DrawLine(g, Value.A, Color.White);

			}

			void DrawColor(Graphics g)
			{
				for (int x = 0; x < GraphPanel.Width; x += 5)
				{ 
					var v = ScreenToValue(x,0);
					var r = Math.Max(0,Math.Min(255,(int)Value.R.GetValue((int)v.X)));
					var gr = Math.Max(0,Math.Min(255,(int)Value.G.GetValue((int)v.X)));
					var b = Math.Max(0,Math.Min(255,(int)Value.B.GetValue((int)v.X)));
					var a = Math.Max(0,Math.Min(255,(int)Value.A.GetValue((int)v.X)));

					g.FillRectangle(
						new SolidBrush(Color.FromArgb(a, r, gr, b)),
						new Rectangle(x, 5, 5, 10));
				}
			}

			protected override void KeyDown(Keys e)
			{
				if (!EventFlag && e == Keys.R)
				{
					selectedPoints.Clear();

					var key = PointToKey(MousePosition.X, MousePosition.Y);

					var p = new SelectablePoint();
					p.FCurve = Value.R;
					p.Key = key;
					selectedPoints.Add(p);

					Value.R.AddKey(key);
				}

				if (!EventFlag && e == Keys.G)
				{
					selectedPoints.Clear();

					var key = PointToKey(MousePosition.X, MousePosition.Y);

					var p = new SelectablePoint();
					p.FCurve = Value.G;
					p.Key = key;
					selectedPoints.Add(p);

					Value.G.AddKey(key);
				}

				if (!EventFlag && e == Keys.B)
				{
					selectedPoints.Clear();

					var key = PointToKey(MousePosition.X, MousePosition.Y);

					var p = new SelectablePoint();
					p.FCurve = Value.B;
					p.Key = key;
					selectedPoints.Add(p);

					Value.B.AddKey(key);
				}

				if (!EventFlag && e == Keys.A)
				{
					selectedPoints.Clear();

					var key = PointToKey(MousePosition.X, MousePosition.Y);

					var p = new SelectablePoint();
					p.FCurve = Value.A;
					p.Key = key;
					selectedPoints.Add(p);

					Value.A.AddKey(key);
				}

				if (!EventFlag && e == Keys.Delete)
				{
					foreach (var s in selectedPoints)
					{
						s.FCurve.RemoveKey(s.Key);
					}
					selectedPoints.Clear();
				}

				SetGraphToPanel();
			}

			Data.Value.FCurveKey<byte> PointToKey(int x, int y)
			{
				var p = ScreenToValue(x, y);

				return new Data.Value.FCurveKey<byte>((int)p.X, (byte)Math.Max(0, Math.Min(255, p.Y)));
			}

			Point FCurveKeyToPoint(Data.Value.FCurveKey<float> key)
			{
				var p = ValueToScreen(key.Frame, key.Value);
				return new Point(p.X, p.Y);
			}
		}


		class FCurveVector2D : FCurve
		{
			public Data.Value.FCurveVector2D Value { get; private set; }

			public FCurveVector2D(string name, Data.Value.FCurveVector2D value, FCurves window)
			{
				Name = name;
				Value = value;
				this.window = window;

				fCurvesMagYCount = (float)Math.Log((float)GraphPanel.Height / (value.X.DefaultValueRangeMax - value.X.DefaultValueRangeMin), 2.0);
				fCurvesOffsetY = (value.X.DefaultValueRangeMax + value.X.DefaultValueRangeMin) / 2.0f;
			}

			public override object GetValueAsObject()
			{
				return Value;
			}

			protected override IEnumerable<SelectablePoint> GetAllSelectablePoints()
			{
				var selectedX = Value.X.Keys.Select(_ => new SelectablePoint(Value.X, _, SelectablePoint.PointType.Center));

				var selectedY = Value.Y.Keys.Select(_ => new SelectablePoint(Value.Y, _, SelectablePoint.PointType.Center));

				var selectedLeft = selectedPoints.Select(_ => new SelectablePoint(_.FCurve, _.Key, SelectablePoint.PointType.Left));

				var selectedRight = selectedPoints.Select(_ => new SelectablePoint(_.FCurve, _.Key, SelectablePoint.PointType.Right));

				return selectedX.Concat(selectedY.Concat(selectedLeft.Concat(selectedRight)));
			}

			protected override IEnumerable<SelectablePoint> GetSelectablePoints(int x, int y)
			{
				var fv = ScreenToValue(x, y);

				var selectableArea = new PointF(
						5.0f / window.FCurvesMagnificationX,
						5.0f / FCurvesMagnificationY);

				var selectableAreaAncor = new PointF(
					9.0f / window.FCurvesMagnificationX,
					9.0f / FCurvesMagnificationY);

				var selectedX = Value.X.Keys.Where(_ =>
					fv.X - selectableArea.X < _.Frame &&
					_.Frame < fv.X + selectableArea.X &&
					fv.Y - selectableArea.Y < _.ValueAsFloat &&
					_.ValueAsFloat < fv.Y + selectableArea.Y).Select(_ => new SelectablePoint(Value.X, _, SelectablePoint.PointType.Center));

				var selectedY = Value.Y.Keys.Where(_ =>
					fv.X - selectableArea.X < _.Frame &&
					_.Frame < fv.X + selectableArea.X &&
					fv.Y - selectableArea.Y < _.ValueAsFloat &&
					_.ValueAsFloat < fv.Y + selectableArea.Y).Select(_ => new SelectablePoint(Value.Y, _, SelectablePoint.PointType.Center));

				var selectedLeft = selectedPoints.Where(_ =>
				{
					var p1 = new PointF(_.Key.LeftX, _.Key.LeftY);
					return fv.X - selectableAreaAncor.X < p1.X &&
						p1.X < fv.X + selectableAreaAncor.X &&
						fv.Y - selectableAreaAncor.Y < p1.Y &&
						p1.Y < fv.Y + selectableAreaAncor.Y;
				}).Select(_ => new SelectablePoint(_.FCurve, _.Key, SelectablePoint.PointType.Left));

				var selectedRight = selectedPoints.Where(_ =>
				{
					var p1 = new PointF(_.Key.RightX, _.Key.RightY);
					return fv.X - selectableAreaAncor.X < p1.X &&
						p1.X < fv.X + selectableAreaAncor.X &&
						fv.Y - selectableAreaAncor.Y < p1.Y &&
						p1.Y < fv.Y + selectableAreaAncor.Y;
				}).Select(_ => new SelectablePoint(_.FCurve, _.Key, SelectablePoint.PointType.Right));

				return selectedX.Concat(selectedY.Concat(selectedLeft.Concat(selectedRight)));
			}

			protected override void Paint(Graphics g)
			{
				DrawBackground(g);
				DrawFrame(g);

				DrawGrids(g);

				DrawLine(g, Value.X, Color.Red);
				DrawLine(g, Value.Y, Color.LightGreen);
			}

			protected override void KeyDown(Keys e)
			{
				if (!EventFlag && e == Keys.X)
				{
					selectedPoints.Clear();

					var key = PointToKey(MousePosition.X, MousePosition.Y);

					var p = new SelectablePoint();
					p.FCurve = Value.X;
					p.Key = key;
					selectedPoints.Add(p);

					Value.X.AddKey(key);
				}

				if (!EventFlag && e == Keys.Y)
				{
					selectedPoints.Clear();

					var key = PointToKey(MousePosition.X, MousePosition.Y);

					var p = new SelectablePoint();
					p.FCurve = Value.Y;
					p.Key = key;
					selectedPoints.Add(p);

					Value.Y.AddKey(key);
				}

				if (!EventFlag && e == Keys.Delete)
				{
					foreach (var s in selectedPoints)
					{
						s.FCurve.RemoveKey(s.Key);
					}
					selectedPoints.Clear();
				}

				SetGraphToPanel();
			}

			Data.Value.FCurveKey<float> PointToKey(int x, int y)
			{
				var p = ScreenToValue(x, y);

				return new Data.Value.FCurveKey<float>((int)p.X, p.Y);
			}


			Point FCurveKeyToPoint(Data.Value.FCurveKey<float> key)
			{
				var p = ValueToScreen(key.Frame, key.Value);
				return new Point(p.X, p.Y);
			}
		}

		class FCurveVector3D : FCurve
		{
			public Data.Value.FCurveVector3D Value { get; private set; }

			public FCurveVector3D(string name, Data.Value.FCurveVector3D value, FCurves window)
			{
				Name = name;
				Value = value;
				this.window = window;

				fCurvesMagYCount = (float)Math.Log((float)GraphPanel.Height / (value.X.DefaultValueRangeMax - value.X.DefaultValueRangeMin), 2.0);
				fCurvesOffsetY = (value.X.DefaultValueRangeMax + value.X.DefaultValueRangeMin) / 2.0f;
			}

			public override object GetValueAsObject()
			{
				return Value;
			}

			protected override IEnumerable<SelectablePoint> GetAllSelectablePoints()
			{
				var selectedX = Value.X.Keys.Select(_ => new SelectablePoint(Value.X, _, SelectablePoint.PointType.Center));

				var selectedY = Value.Y.Keys.Select(_ => new SelectablePoint(Value.Y, _, SelectablePoint.PointType.Center));

				var selectedZ = Value.Z.Keys.Select(_ => new SelectablePoint(Value.Z, _, SelectablePoint.PointType.Center));

				var selectedLeft = selectedPoints.Select(_ => new SelectablePoint(_.FCurve, _.Key, SelectablePoint.PointType.Left));

				var selectedRight = selectedPoints.Select(_ => new SelectablePoint(_.FCurve, _.Key, SelectablePoint.PointType.Right));

				return selectedX.Concat(selectedY.Concat(selectedZ.Concat(selectedLeft.Concat(selectedRight))));
			}

			protected override IEnumerable<SelectablePoint>  GetSelectablePoints(int x, int y)
			{
				var fv = ScreenToValue(x, y);

				var selectableArea = new PointF(
						5.0f / window.FCurvesMagnificationX,
						5.0f / FCurvesMagnificationY);

				var selectableAreaAncor = new PointF(
					9.0f / window.FCurvesMagnificationX,
					9.0f / FCurvesMagnificationY);

				var selectedX = Value.X.Keys.Where(_ =>
					fv.X - selectableArea.X < _.Frame &&
					_.Frame < fv.X + selectableArea.X &&
					fv.Y - selectableArea.Y < _.ValueAsFloat &&
					_.ValueAsFloat < fv.Y + selectableArea.Y).Select(_ => new SelectablePoint(Value.X, _, SelectablePoint.PointType.Center));

				var selectedY = Value.Y.Keys.Where(_ =>
					fv.X - selectableArea.X < _.Frame &&
					_.Frame < fv.X + selectableArea.X &&
					fv.Y - selectableArea.Y < _.ValueAsFloat &&
					_.ValueAsFloat < fv.Y + selectableArea.Y).Select(_ => new SelectablePoint(Value.Y, _, SelectablePoint.PointType.Center));

				var selectedZ = Value.Z.Keys.Where(_ =>
					fv.X - selectableArea.X < _.Frame &&
					_.Frame < fv.X + selectableArea.X &&
					fv.Y - selectableArea.Y < _.ValueAsFloat &&
					_.ValueAsFloat < fv.Y + selectableArea.Y).Select(_ => new SelectablePoint(Value.Z, _, SelectablePoint.PointType.Center));

				var selectedLeft = selectedPoints.Where(_ =>
					{
						var p1 = new PointF(_.Key.LeftX, _.Key.LeftY);
						return fv.X - selectableAreaAncor.X < p1.X &&
							p1.X < fv.X + selectableAreaAncor.X &&
							fv.Y - selectableAreaAncor.Y < p1.Y &&
							p1.Y < fv.Y + selectableAreaAncor.Y;
					}).Select(_ => new SelectablePoint(_.FCurve, _.Key, SelectablePoint.PointType.Left));

				var selectedRight = selectedPoints.Where(_ =>
				{
					var p1 = new PointF(_.Key.RightX, _.Key.RightY);
					return fv.X - selectableAreaAncor.X < p1.X &&
						p1.X < fv.X + selectableAreaAncor.X &&
						fv.Y - selectableAreaAncor.Y < p1.Y &&
						p1.Y < fv.Y + selectableAreaAncor.Y;
				}).Select(_ => new SelectablePoint(_.FCurve, _.Key, SelectablePoint.PointType.Right));

				return selectedX.Concat(selectedY.Concat(selectedZ.Concat(selectedLeft.Concat(selectedRight))));
			}

			protected override void Paint(Graphics g)
			{
				DrawBackground(g);
				DrawFrame(g);

				DrawGrids(g);

				DrawLine(g, Value.X, Color.Red);
				DrawLine(g, Value.Y, Color.LightGreen);
				DrawLine(g, Value.Z, Color.Blue);
			}

			protected override void KeyDown(Keys e)
			{
				if (!EventFlag && e == Keys.X)
				{
					selectedPoints.Clear();

					var key = PointToKey(MousePosition.X, MousePosition.Y);

					var p = new SelectablePoint();
					p.FCurve = Value.X;
					p.Key = key;
					selectedPoints.Add(p);

					Value.X.AddKey(key);
				}

				if (!EventFlag && e == Keys.Y)
				{
					selectedPoints.Clear();

					var key = PointToKey(MousePosition.X, MousePosition.Y);

					var p = new SelectablePoint();
					p.FCurve = Value.Y;
					p.Key = key;
					selectedPoints.Add(p);

					Value.Y.AddKey(key);
				}

				if (!EventFlag && e == Keys.Z)
				{
					selectedPoints.Clear();

					var key = PointToKey(MousePosition.X, MousePosition.Y);

					var p = new SelectablePoint();
					p.FCurve = Value.Z;
					p.Key = key;
					selectedPoints.Add(p);

					Value.Z.AddKey(key);
				}

				if (!EventFlag && e == Keys.Delete)
				{
					foreach (var s in selectedPoints)
					{
						s.FCurve.RemoveKey(s.Key);
					}
					selectedPoints.Clear();
				}

				SetGraphToPanel();
			}

			Data.Value.FCurveKey<float> PointToKey(int x, int y)
			{
				var p = ScreenToValue(x, y);

				return new Data.Value.FCurveKey<float>((int)p.X, p.Y);
			}


			Point FCurveKeyToPoint(Data.Value.FCurveKey<float> key)
			{
				var p = ValueToScreen(key.Frame, key.Value);
				return new Point(p.X, p.Y);
			}
		}

	}
}
