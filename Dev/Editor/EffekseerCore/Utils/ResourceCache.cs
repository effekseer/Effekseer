using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Utils
{
	/// <summary>
	/// A cache class for improving loading time
	/// </summary>
	internal class ResourceCache
	{
		Dictionary<string, Utl.MaterialInformation> materials = new Dictionary<string, Utl.MaterialInformation>();
	
		public Utl.MaterialInformation LoadMaterialInformation(string path)
		{
			path = path.Replace('\\', '/');

			if (materials.ContainsKey(path)) return materials[path];

			var info = new Utl.MaterialInformation();

			if(info.Load(path))
			{
				materials.Add(path, info);
			}

			return info;
		}

		public void Remove(string path)
		{
			path = path.Replace('\\', '/');

			if (materials.ContainsKey(path))
			{
				materials.Remove(path);
			}
		}

		public void Reset()
		{
			materials.Clear();
		}
	}
}
