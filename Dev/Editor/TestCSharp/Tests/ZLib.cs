using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TestCSharp.Misc;

namespace TestCSharp
{
	class ZLib
	{
		public static void Test()
		{
			Compare("../../TestData/Misc/Zlib/data1.dat", "../../TestData/Misc/Zlib/python1.dat", "data1");
			Compare("../../TestData/Misc/Zlib/data2.dat", "../../TestData/Misc/Zlib/python2.dat", "data2");
			Compare("../../TestData/Misc/Zlib/data3.dat", "../../TestData/Misc/Zlib/python3.dat", "data3");
			Compare("../../TestData/Misc/Zlib/data4.dat", "../../TestData/Misc/Zlib/python4.dat", "data4");
		}

		static void Compare(string decompressedPath, string compressedPath, string name)
		{
			var decompressed1 = System.IO.File.ReadAllBytes(decompressedPath);
			var compressed1 = System.IO.File.ReadAllBytes(compressedPath);
			
			var compressed2 = Effekseer.Utils.Zlib.Compress(decompressed1);
			var decompressed2 = Effekseer.Utils.Zlib.Decompress(compressed1);

			TestUtils.CompareBytes(decompressed1, decompressed2);
			TestUtils.CompareBytes(compressed1, compressed2);
		}
	}
}
