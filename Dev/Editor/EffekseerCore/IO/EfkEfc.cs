using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.IO
{
	/// <summary>
	/// It should be made as unit test
	/// </summary>
	public class ChunkTest
	{
		public static void Test()
		{
			Func<int, byte[]> generator = (int n) => {
				var ret = new byte[n];
				var rand = new Random();

				for(int i = 0; i < n; i++)
				{
					ret[i] = (byte)rand.Next(255);
				}

				return ret;
			};
			
			var src = new Chunk();
			src.AddChunk("tes1", generator(25));
			src.AddChunk("tes2", generator(255));
			src.AddChunk("tes3", generator(256));
			var saved = src.Save();

			var dst = new Chunk();
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

	/// <summary>
	/// Chunck to save or load
	/// TODO create new file
	/// </summary>
	class Chunk
	{
		public List<ChunkBlock> Blocks = new List<ChunkBlock>();

		public void AddChunk(string chunk, byte[] buffer)
		{
			var block = new ChunkBlock();
			block.Chunk = chunk;
			block.Buffer = buffer;
			Blocks.Add(block);
		}

		public byte[] Save()
		{
			List<byte[]> data = new List<byte[]>();

			foreach(var binary in Blocks)
			{
				var chunk = new byte[4] { 0, 0, 0, 0 };
				var chararray = Encoding.UTF8.GetBytes(binary.Chunk);
				for(int i = 0; i < Math.Min(chunk.Length, chararray.Length); i++)
				{
					chunk[i] = chararray[i];
				}

				data.Add(chunk);
				data.Add(BitConverter.GetBytes((UInt32)binary.Buffer.Length));
				data.Add(binary.Buffer);
			}

			return data.SelectMany(_ => _).ToArray();
		}

		public bool Load(byte[] data)
		{
			Blocks.Clear();

			int pos = 0;

			while(pos < data.Length)
			{
				var chunkdata = data.Skip(pos).Take(4).ToArray();
				var chunk = Encoding.UTF8.GetString(chunkdata, 0, 4);
				pos += 4;

				var size = BitConverter.ToInt32(data, pos);
				pos += 4;

				var buffer = data.Skip(pos).Take(size).ToArray();
				pos += size;

				var binary = new ChunkBlock();
				binary.Chunk = chunk;
				binary.Buffer = buffer;
				Blocks.Add(binary);
			}

			if (pos != data.Length) return false;

			return true;
		}
	}

	class ChunkBlock
	{
		public string Chunk;

		public byte[] Buffer;
	}

	class EfkEfc
	{
		public bool Save()
		{
			return false;
		}

		public bool Load()
		{
			return false;
		}
	}
}
