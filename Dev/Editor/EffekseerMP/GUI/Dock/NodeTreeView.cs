using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
    class NodeTreeView : DockPanel
    {
        internal List<NodeTreeViewNode> Children = new List<NodeTreeViewNode>();

        public NodeTreeView()
        {
			Label = Resources.GetString("NodeTree");
        }

        override protected void UpdateInternal()
        {
            foreach (var child in Children)
            {
                child.Update();
            }
        }

        /// <summary>
        /// Renew all view
        /// </summary>
        public void Renew()
        {
            // Reset all
            if(Children.Count != 1 || Children[0].node != Core.Root)
            {
                foreach(var child in Children)
                {
                    child.RemoveEvent(true);
                }
                Children.Clear();

                Children.Add(new NodeTreeViewNode(Core.Root));
            }


            Action<NodeTreeViewNode, Data.NodeBase> set_nodes = null;
            set_nodes = (treenode, node) =>
            {
                var tns = treenode.Children;
                var ns = node.Children;

                for (int ind = 0; ind < ns.Count;)
                {
                    // not need to change
                    if (ind < tns.Count && ((NodeTreeViewNode)tns[ind]).node == ns[ind])
                    {
                        ind++;
                        continue;
                    }

                    // need to change
                    while (tns.Count > ind)
                    {
                        ((NodeTreeViewNode)tns[ind]).RemoveEvent(true);
                        tns.RemoveAt(ind);
                    }

                    while (ns.Count > ind)
                    {
                        tns.Add(new NodeTreeViewNode(ns[ind]));
                        ind++;
                    }
                }

                for (int i = 0; i < ns.Count; i++)
                {
                    set_nodes(tns[i], ns[i]);
                }
            };

            set_nodes(Children[0], Core.Root);

            ExpandAll();
        }

        public void ExpandAll()
        {
            Console.WriteLine("Not implemented");
        }

        /// <summary>
        /// When renew, it is called.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void OnRenew(object sender, EventArgs e)
        {
            Renew();
        }
    }

    class NodeTreeViewNode : IControl
    {
        string id = "";

        public Data.NodeBase node { get; private set; } = null;

        internal List<NodeTreeViewNode> Children = new List<NodeTreeViewNode>();

        public NodeTreeViewNode(Data.NodeBase node, bool createChildren = false)
        {
            var rand = new Random();
            id = "###" + rand.Next(0xffff).ToString();

            this.node = node;

            node.OnAfterAddNode += OnAfterAddNode;
            node.OnAfterRemoveNode += OnAfterRemoveNode;
            node.OnAfterExchangeNodes += OnAfterExchangeNodes;

            if (createChildren)
            {
                for(int i = 0; i < node.Children.Count; i++)
                {
                    var newNode = new NodeTreeViewNode(node.Children[i], true);
                    Children.Add(newNode);
                }
            }
        }

        public void RemoveEvent(bool recursion)
        {
            if (node is Data.Node)
            {
                var realNode = (Data.Node)node;
            }

            node.OnAfterAddNode -= OnAfterAddNode;
            node.OnAfterRemoveNode -= OnAfterRemoveNode;
            node.OnAfterExchangeNodes -= OnAfterExchangeNodes;
            if (recursion)
            {
                for (int i = 0; i < Children.Count; i++)
                {
                    (Children[i]).RemoveEvent(true);
                }
            }
        }


        public void Update()
        {
            if (Manager.NativeManager.TreeNode(node.Name + id))
            {
                foreach (var child in Children)
                {
                    child.Update();
                }

                Manager.NativeManager.TreePop();
            }
        }

        public void ExpandAll()
        {
            Console.WriteLine("Not implemented.");
        }

        void OnAfterAddNode(object sender, ChangedValueEventArgs e)
        {
            var node = e.Value as Data.NodeBase;

            int ind = 0;
            for (; ind < node.Children.Count; ind++)
            {
                if (node == node.Children[ind]) break;
            }

            if (ind == node.Children.Count)
            {
                Children.Add(new NodeTreeViewNode(node, true));
            }
            else
            {
                Children.Insert(ind, new NodeTreeViewNode(node, true));
            }

            ExpandAll();
        }

        void OnAfterRemoveNode(object sender, ChangedValueEventArgs e)
        {
            var node = e.Value as Data.NodeBase;

            for (int i = 0; i < Children.Count; i++)
            {
                var treenode = Children[i];
                if (treenode.node == node)
                {
                    treenode.RemoveEvent(true);
                    Children.Remove(treenode);
                    return;
                }
            }

            throw new Exception();
        }

        void OnAfterExchangeNodes(object sender, ChangedValueEventArgs e)
        {
            var node1 = (e.Value as Tuple<Data.Node, Data.Node>).Item1;
            var node2 = (e.Value as Tuple<Data.Node, Data.Node>).Item2;

            int ind1 = 0;
            int ind2 = 0;

            for (int i = 0; i < Children.Count; i++)
            {
                var treenode = Children[i];
                if (treenode.node == node1)
                {
                    ind1 = i;
                }
                if (treenode.node == node2)
                {
                    ind2 = i;
                }
            }

            if (ind1 > ind2)
            {
                var ind_temp = ind1;
                var node_temp = node1;
                ind1 = ind2;
                node1 = node2;
                ind2 = ind_temp;
                node2 = node_temp;
            }

            Children.Insert(ind2, new NodeTreeViewNode(node1, true));
            Children.RemoveAt(ind2 + 1);

            Children.Insert(ind1, new NodeTreeViewNode(node2, true));
            Children.RemoveAt(ind1 + 1);

            ExpandAll();
        }
    }
}
