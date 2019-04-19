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

		public int Count { get { return internalList.Count; } }

		public T this[int index]
		{
			get
			{
				return internalList[index];
			}
		}

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

		public void Remove(T item)
		{
			if (lockCount == 0)
			{
				internalList.Remove(item);
			}
			else
			{
				var c = new Command_Remove(item);
				commands.Add(c);
			}
		}

		public void RemoveAt(int index)
		{
			if (lockCount == 0)
			{
				internalList.RemoveAt(index);
			}
			else
			{
				var c = new Command_RemoveAt(index);
				commands.Add(c);
			}
		}

		public void Insert(int index, T item)
		{
			if (lockCount == 0)
			{
				internalList.Insert(index, item);
			}
			else
			{
				var c = new Command_Insert(index, item);
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

		class Command_Remove : Command
		{
			T removing;

			public Command_Remove(T o)
			{
				removing = o;
			}

			public override void Run(List<T> list)
			{
				list.Remove(removing);
			}
		}

		class Command_RemoveAt : Command
		{
			int index;

			public Command_RemoveAt(int index)
			{
				this.index = index;
			}

			public override void Run(List<T> list)
			{
				list.RemoveAt(index);
			}
		}

		class Command_Insert : Command
		{
			int index;
			T item;

			public Command_Insert(int index, T item)
			{
				this.index = index;
				this.item = item;
			}

			public override void Run(List<T> list)
			{
				list.Insert(index, item);
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
