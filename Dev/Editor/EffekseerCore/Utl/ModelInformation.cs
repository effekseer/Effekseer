using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Utl
{
	public class ModelInformation
	{
		public float Scale = 1.0f;

		public ModelInformation()
		{

		}

		public bool Load(string path)
		{
			System.IO.FileStream fs = null;
			try
			{
				fs = System.IO.File.Open(path, System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read);
			}
			catch (System.IO.FileNotFoundException e)
			{
				return false;
			}

			var br = new System.IO.BinaryReader(fs);

			var buf = new byte[1024];

			if (br.Read(buf, 0, 8) != 8)
			{
				fs.Dispose();
				br.Dispose();
				return false;
			}

			var version = BitConverter.ToInt32(buf, 0);

			if (version > 1)
			{
				Scale = BitConverter.ToSingle(buf, 4);
			}

			fs.Dispose();
			br.Dispose();

			return true;
		}
	}
}
