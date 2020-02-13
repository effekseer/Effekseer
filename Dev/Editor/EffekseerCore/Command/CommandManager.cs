using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Command
{
	/// <summary>
	/// 命令管理クラス
	/// </summary>
	public class CommandManager
	{
		public static event EventHandler Changed = null;

		/// <summary>
		/// 結合コマンドを変換してひとつのコマンドに纏める関数群
		/// </summary>
		static List<Func<IEnumerable<ICommand>, ICommand>> convertFunctions = new List<Func<IEnumerable<ICommand>, ICommand>>();

		/// <summary>
		/// 命令履歴
		/// </summary>
		static List<ICommand> cmds = new List<ICommand>();

		/// <summary>
		/// 次の命令を挿入するインデックス
		/// </summary>
		static int cmds_ind = 0;
		
		/// <summary>
		/// 命令をグループ化する際のコンテナ
		/// </summary>
		static List<ICommand> cmd_collections = new List<ICommand>();

		/// <summary>
		/// 命令をグループ化する際の階層数
		/// </summary>
		/// <remarks>
		/// 0の時はグループ化せず、
		/// 1以上の時、グループ化
		/// 0未満の時、例外
		/// </remarks>
		static int cmd_collections_count = 0;

		/// <summary>
		/// 結合中の命令コンテナ
		/// </summary>
		static List<ICommand> cmd_combined = new List<ICommand>();

		/// <summary>
		/// 結合モードか?
		/// </summary>
		static bool isCombinedMode = false;

		/// <summary>
		/// 結合コマンドを変換するメソッドを追加する。
		/// </summary>
		/// <param name="f">コマンドを受け取り、結合できたのなら結合したコマンド、そうでないならnullを返すメソッド</param>
		internal static void AddConvertFunction(Func<IEnumerable<ICommand>, ICommand> f)
		{
			convertFunctions.Add(f);
		}

		/// <summary>
		/// 結合可能なコマンドを結合し、通常のコマンドキューに追加する。
		/// </summary>
		public static void FreezeCombinedCommands()
		{
			if (isCombinedMode)
			{
				if (cmd_combined.Count == 0) throw new InvalidOperationException("List doesn't have combined commands.");

				isCombinedMode = false;

				ICommand cmdCollection = null;

				// 特殊な変換
				foreach (var f in convertFunctions)
				{
					var result = f(cmd_combined);
					if (result != null)
					{
						cmdCollection = result;
						break;
					}
				}

				// 一般的な変換
				if (cmdCollection == null)
				{
					var commandCollection = new CommandCollection();
					foreach (var c in cmd_combined)
					{
						commandCollection.Add(c);
					}
					cmdCollection = commandCollection;
				}

				// 追加処理
				cmd_combined.Clear();

				cmds.Add(cmdCollection);
				cmds_ind++;	
			}
			else
			{
				if (cmd_combined.Count > 0) throw new InvalidOperationException("List has combined commands.");
			}
		}

		/// <summary>
		/// コマンドの実行
		/// </summary>
		/// <param name="command">実行する命令</param>
		internal static void Execute(ICommand command)
		{
			Action<ICommand> addCommand = (c) =>
				{
					var cmd = c as DelegateCommand;
					if (cmd != null && cmd.IsCombined)
					{
						if (cmd_combined.Count > 0) throw new InvalidOperationException("List has combined commands.");

						isCombinedMode = true;
						cmd_combined.Add(c);
					}
					else
					{
						cmds.Add(c);
						cmds_ind++;
					}
				};

			if (command == null) throw new ArgumentNullException("command is null.");

			command.Execute();

			if (cmd_collections_count > 0)
			{
				cmd_collections.Add(command);
			}
			else
			{
				if (cmds.Count > cmds_ind)
				{
					cmds.RemoveRange(cmds_ind, cmds.Count - cmds_ind);
				}

				if (isCombinedMode)
				{
					if (cmd_combined.Count == 0) throw new InvalidOperationException("List doesn't have combined commands.");

					var cmd = command as DelegateCommand;
					var firstCmd = cmd_combined.First() as DelegateCommand;

					if (cmd != null && cmd.IsCombined && cmd.Identifier == firstCmd.Identifier)
					{
						cmd_combined.Add(command);
					}
					else
					{
						FreezeCombinedCommands();

						addCommand(command);
					}
				}
				else
				{
					addCommand(command);
				}
			}

			if (Changed != null)
			{
				Changed(null, null);
			}
		}

		/// <summary>
		/// ロールバック
		/// </summary>
		/// <returns></returns>
		public static bool Undo(bool removeCommand = false)
		{
			if (cmd_collections_count > 0)
			{
				return false;
			}
			else
			{
				FreezeCombinedCommands();

				if (cmds_ind > 0)
				{
					cmds_ind--;
					cmds[cmds_ind].Unexecute();

					if (Changed != null)
					{
						Changed(null, null);
					}

					if (removeCommand)
					{
						cmds.RemoveAt(cmds.Count - 1);
					}

					return true;
				}
			}
			
			return false;
		}

		/// <summary>
		/// 再ロールバック
		/// </summary>
		/// <returns></returns>
		public static bool Redo()
		{
			if (cmd_collections_count > 0)
			{
				return false;
			}
			else if( cmds_ind < cmds.Count)
			{
				if (isCombinedMode) throw new InvalidOperationException("CombinedMode is true.");

				cmds[cmds_ind].Execute();
				
				cmds_ind++;

				if (Changed != null)
				{
					Changed(null, null);
				}

				return true;
			}

			return false;
		}

		/// <summary>
		/// コマンドのグループ化開始
		/// </summary>
		public static void StartCollection()
		{
			FreezeCombinedCommands();
			cmd_collections_count++;
		}

		/// <summary>
		/// コマンドのグループ化終了
		/// </summary>
		public static void EndCollection()
		{
			if (cmd_collections_count == 0) throw new InvalidOperationException("Collection has not yet start.");
			cmd_collections_count--;

			if (cmd_collections_count == 0)
			{
				if (cmd_collections.Count == 0) return;

				var command = new CommandCollection();
				foreach (var cmd in cmd_collections)
				{
					command.Add(cmd);
				}

				if (cmds.Count > cmds_ind)
				{
					cmds.RemoveRange(cmds_ind, cmds.Count - cmds_ind);
				}

				cmds.Add(command);
				cmds_ind++;

				cmd_collections.Clear();
			}
		}

		/// <summary>
		/// 実行履歴の消去
		/// </summary>
		internal static bool Clear()
		{
			if (cmd_collections_count > 0)
			{
				return false;
			}
			else
			{
				cmds.Clear();
				cmds_ind = 0;
				return true;
			}
		}

		/// <summary>
		/// コマンド群コンテナ
		/// </summary>
		class CommandCollection : ICommand
		{
			List<ICommand> cmds = new List<ICommand>();

			public CommandCollection()
			{ 
			
			}

			public void Add(ICommand cmd)
			{
				cmds.Add(cmd);
			}

			public void Execute()
			{
				foreach (var cmd in cmds)
				{
					cmd.Execute();
				}
			}

			public void Unexecute()
			{
				for (int i = cmds.Count - 1; i >= 0; i--)
				{
					cmds[i].Unexecute();
				}
			}
		}
	}
}
