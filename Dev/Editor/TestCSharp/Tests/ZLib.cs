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
			var decompressed3 = Effekseer.Utils.Zlib.Decompress(compressed2);
			var decompressed4 = DecompressByZLibStream(compressed2);

			TestUtils.CompareBytes(decompressed1, decompressed2, name + ": failed to decompress python zlib data.");
			TestUtils.CompareBytes(decompressed1, decompressed3, name + ": failed to round-trip csharp zlib data.");
			TestUtils.CompareBytes(decompressed1, decompressed4, name + ": failed to decompress csharp zlib data with ZLibStream.");
		}

		static byte[] DecompressByZLibStream(byte[] compressed)
		{
			using (var input = new System.IO.MemoryStream(compressed))
			using (var zlib = new System.IO.Compression.ZLibStream(input, System.IO.Compression.CompressionMode.Decompress))
			using (var output = new System.IO.MemoryStream())
			{
				zlib.CopyTo(output);
				return output.ToArray();
			}
		}
	}
}
