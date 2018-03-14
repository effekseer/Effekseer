using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Utils
{
	class DelayedList<T>
	{
		int lockCount = 0;

		List<T> internalList = new List<T>();

		List<Command> commands = new List<Command>();

		public List<T> Internal { get { return internalList; } }

		public void Add(T item)
		{
			if(lockCount == 0)
			{
				internalList.Add(item);
			}
			else
			{
				var c = new Command_Add(item);
				commands.Add(c);
			}
		}

		public void Clear()
		{
			if (lockCount == 0)
			{
				internalList.Clear();
			}
			else
			{
				var c = new Command_Clear();
				commands.Add(c);
			}
		}

		public void Lock()
		{
			lockCount++;
		}

		public void Unlock()
		{
			lockCount--;

			if(lockCount == 0)
			{
				foreach(var c in commands)
				{
					c.Run(internalList);
				}

				commands.Clear();
			}
		}

		class Command
		{
			public virtual void Run(List<T> list) { }
		}

		class Command_Add : Command
		{
			T adding;

			public Command_Add(T o)
			{
				adding = o;
			}

			public override void Run(List<T> list)
			{
				list.Add(adding);
			}
		}

		class Command_Clear : Command
		{
			public Command_Clear()
			{
			}

			public override void Run(List<T> list)
			{
				list.Clear();
			}
		}
	}
}
