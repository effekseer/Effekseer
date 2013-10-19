using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Command
{
	/// <summary>
	/// Delegateで実行内容が指定可能な命令
	/// </summary>
	internal class DelegateCommand : ICommand
	{
		System.Action _execute = null;

		System.Action _unexecute = null;

		public object Identifier
		{
			get;
			private set;
		}

		public bool IsCombined
		{
			get;
			private set;
		}

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="execute">通常実行及び、Redoの際に実行されるメソッド</param>
		/// <param name="unexecute">Undoの際に実行されるメソッド</param>
		/// <param name="identifier">命令を結合する際の識別子</param>
		/// <param name="isCombined">命令を結合するか、識別子が等しいコマンドが続く時、Undo、Redoにて一括で実行される。</param>
		public DelegateCommand(System.Action execute, System.Action unexecute, object identifier = null, bool isCombined = false)
		{
			_execute = execute;
			_unexecute = unexecute;
			Identifier = identifier;
			IsCombined = isCombined;
		}

		public void Execute()
		{
			_execute();
		}

		public void Unexecute()
		{
			_unexecute();
		}
	}
}
