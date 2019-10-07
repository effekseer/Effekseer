using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestCSharp.Misc
{
	class ZLib
	{
		public static void Test()
		{
			var original = System.IO.File.ReadAllBytes("../../TestData/Misc/Zlib/data1.dat");
			var python = System.IO.File.ReadAllBytes("../../TestData/Misc/Zlib/python1.dat");
			var csharp = Effekseer.Utils.Zlib.Compress(original);
			var decompressed = Effekseer.Utils.Zlib.Decompress(python);

			if (!original.SequenceEqual(decompressed)) throw new Exception();
			if (!python.SequenceEqual(csharp)) throw new Exception();

			Compare("../../TestData/Misc/Zlib/data1.dat", "../../TestData/Misc/Zlib/python1.dat", "data1");
			Compare("../../TestData/Misc/Zlib/data2.dat", "../../TestData/Misc/Zlib/python2.dat", "data2");
			Compare("../../TestData/Misc/Zlib/data3.dat", "../../TestData/Misc/Zlib/python3.dat", "data3");
			Compare("../../TestData/Misc/Zlib/data4.dat", "../../TestData/Misc/Zlib/python4.dat", "data4");
		}

		static void Compare(string originalPath, string pythonPath, string name)
		{
			var original = System.IO.File.ReadAllBytes(originalPath);
			var python = System.IO.File.ReadAllBytes(pythonPath);
			var csharp = Effekseer.Utils.Zlib.Compress(original);
			var decompressed = Effekseer.Utils.Zlib.Decompress(python);

			if (!original.SequenceEqual(decompressed)) throw new Exception(string.Format("Zlib : {0} original is not same to decompressed.", name));
			if (!python.SequenceEqual(csharp)) throw new Exception(string.Format("Zlib : {0} python is not same to csharp.", name));
		}
	}
}
