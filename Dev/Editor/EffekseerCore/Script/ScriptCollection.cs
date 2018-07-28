using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Script
{
	public class ScriptCollection<T> where T : ScriptBase
	{
		List<T> scripts = new List<T>();

		internal ScriptCollection()
		{
	
		}

		public void Add(T script)
		{
			scripts.Add(script);
		}

		public T this[int ind]
		{
			get
			{
				return scripts[ind];
			}
		}

		public int Count
		{
			get
			{
				return scripts.Count;
			}
		}

		public T GetScript(string uniquename)
		{
			var ss = (from s in scripts where s.UniqueName == uniquename select s).FirstOrDefault() as T;
			return ss;
		}

		public T[] GetScripts()
		{
			return scripts.ToArray();
		}
	}
}
