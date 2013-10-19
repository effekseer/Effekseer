using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Command
{
	/// <summary>
	/// 命令インターフェース
	/// </summary>
	internal interface ICommand
	{
		/// <summary>
		/// コマンドの実行
		/// </summary>
		void Execute();

		/// <summary>
		/// コマンドのロールバック
		/// </summary>
		void Unexecute();
	}
}
