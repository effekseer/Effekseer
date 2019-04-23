using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Utl
{
	/// <summary>
	/// Model format
	/// </summary>
	/// <remarks>
	/// Version 0 First version
	/// Version 1 Vertex color is added
	/// Version 2 Scale is added into header (this version is only 1.30 beta2)
	/// Version 3 Scale is added into footer (compatible with Version1)
	/// </remarks>
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
                br.Close();
                return false;
			}

			var version = BitConverter.ToInt32(buf, 0);

			if (version == 2)
			{
				Scale = BitConverter.ToSingle(buf, 4);
				fs.Dispose();
                br.Close();
                return false;
			}

			if(version == 3)
			{
				fs.Seek(-4, System.IO.SeekOrigin.End);

				if (br.Read(buf, 0, 4) == 4)
				{
					Scale = BitConverter.ToSingle(buf, 0);
				}
				else
				{
					fs.Dispose();
                    br.Close();
                    return false;
				}
			}

			if (version == 5)
			{
				Scale = BitConverter.ToSingle(buf, 4);
				fs.Dispose();
                br.Close();
                return true;
			}

			fs.Dispose();
            br.Close();

            return true;
		}
	}
}
