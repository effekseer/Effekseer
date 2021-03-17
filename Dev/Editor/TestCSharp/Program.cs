using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Reflection;
using System.Runtime.ExceptionServices;

namespace TestCSharp
{
	class Program
	{
		[STAThread]
		[HandleProcessCorruptedStateExceptions]
		static void Main(string[] args)
		{
			ZLib.Test();
			MaterialInformation.Test();

			var script = new InternalScriptTest();
			script.Test();

			ChunkTest.Test();
		}
	}

	public class ChunkTest
	{
		public static void Test()
		{
			// a function to generate dummy data for testing
			Func<int, byte[]> generateDummyData = (int n) =>
			{
				var ret = new byte[n];
				var rand = new Random();

				for (int i = 0; i < n; i++)
				{
					ret[i] = (byte)rand.Next(255);
				}

				return ret;
			};

			var src = new Effekseer.IO.Chunk();
			src.AddChunk("tes1", generateDummyData(25));
			src.AddChunk("tes2", generateDummyData(255));
			src.AddChunk("tes3", generateDummyData(256));
			var saved = src.Save();

			var dst = new Effekseer.IO.Chunk();
			dst.Load(saved);

			if (!(src.Blocks[0].Chunk == dst.Blocks[0].Chunk)) throw new Exception();
			if (!(src.Blocks[1].Chunk == dst.Blocks[1].Chunk)) throw new Exception();
			if (!(src.Blocks[2].Chunk == dst.Blocks[2].Chunk)) throw new Exception();

			if (!(src.Blocks[0].Buffer.Length == dst.Blocks[0].Buffer.Length)) throw new Exception();
			if (!(src.Blocks[1].Buffer.Length == dst.Blocks[1].Buffer.Length)) throw new Exception();
			if (!(src.Blocks[2].Buffer.Length == dst.Blocks[2].Buffer.Length)) throw new Exception();

			if (!src.Blocks[0].Buffer.SequenceEqual(dst.Blocks[0].Buffer)) throw new Exception();
			if (!src.Blocks[1].Buffer.SequenceEqual(dst.Blocks[1].Buffer)) throw new Exception();
			if (!src.Blocks[2].Buffer.SequenceEqual(dst.Blocks[2].Buffer)) throw new Exception();
		}
	}

	class InternalScriptTest
	{
		public void Test()
		{

			Effekseer.InternalScript.Compiler compiler = new Effekseer.InternalScript.Compiler();
			Validate(compiler.Compile("1 + 2 + 3"), null);
			Validate(compiler.Compile("1 * 2 + 3"), null);
			Validate(compiler.Compile("1 * (2 + 3)"), null);

			Validate(compiler.Compile("@O.x = 1 + 2 + 3"), null);
			Validate(compiler.Compile("@O.x = 1 + 2 + 3\n@O.y = 1 + 2 + 3"), null);

			Validate(compiler.Compile("@1"), null);
			Validate(compiler.Compile("sin(1.0)"), null);
			Validate(compiler.Compile("cos(2.0)"), null);

			Validate(compiler.Compile("rand()"), null);
			Validate(compiler.Compile("rand(2.0)"), null);
			Validate(compiler.Compile(".1"), null);

			Validate(compiler.Compile("randaaa()"), typeof(Effekseer.InternalScript.UnknownFunctionException));

		}

		public bool Validate(Effekseer.InternalScript.CompileResult result, Type type)
		{
			if (type == null)
			{
				if (result.Error != null) throw result.Error;
			}
			else
			{
				if (result.Error == null) throw new Exception();
				if (result.Error.GetType() != type) throw result.Error;
			}
			return true;
		}
	}
}
