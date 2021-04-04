using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using TestCSharp.Misc;

namespace TestCSharp
{
	class MaterialInformation
	{
		public static void Test()
		{
			TestMaterialFile("../../TestData/Effects/Materials_Serialized/CustomData1.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials_Serialized/DepthFade.efkmat");
			TestMaterialFile("../../TestData/Effects/Materials_Serialized/Parameter23.efkmat");
		}

		static void TestMaterialFile(string path)
		{
			string fullpath = Path.Combine(Directory.GetCurrentDirectory(), path);
			var material = new Effekseer.Utl.MaterialInformation();

			byte[] bytes = File.ReadAllBytes(fullpath);
			if (bytes == null)
			{
				TestUtils.Assert(false, "Failed to load material file: " + path);
			}

			if (!material.Load(bytes))
			{
				TestUtils.Assert(false, "Failed to load material file: " + path);
			}

			byte[] savedBytes = material.SaveToBytes();
			//File.WriteAllBytes(fullpath + ".testout.efkmat", savedBytes);
			TestUtils.CompareBytes(RemoveDataChunk(bytes), RemoveDataChunk(savedBytes));
		}

		static byte[] RemoveDataChunk(byte[] bytes)
		{
			var stream = new MemoryStream(bytes);
			var reader = new BinaryReader(stream);
			var buf = new byte[16];

			reader.Read(buf, 0, 16);

			while (stream.Position < stream.Length)
			{
				reader.Read(buf, 0, 4);
				uint size = reader.ReadUInt32();

				if (buf[0] == 'D' &&
					buf[1] == 'A' &&
					buf[2] == 'T' &&
					buf[3] == 'A')
				{
					return bytes.Take((int)stream.Position - 8).ToArray();
				}

				stream.Seek(stream.Position + size, SeekOrigin.Begin);
			}
			return bytes;
		}
	}
}
