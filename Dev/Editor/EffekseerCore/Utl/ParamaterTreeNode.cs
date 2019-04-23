using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Utl
{
	/// <summary>
	/// 有効なパラメーター一覧を取得するためのツリーノード
	/// </summary>
	public class ParameterTreeNode
	{
		public Data.NodeBase Node { get; private set; }

		public Tuple35<string,object>[] Parameters { get; private set; }

		public ParameterTreeNode[] Children { get; private set; }

		public ParameterTreeNode(Data.NodeBase node, Tuple35<string, object>[] paramaters, ParameterTreeNode[] children)
		{
			Node = node;
			Parameters = paramaters;
			Children = children;
		}
	}
}
