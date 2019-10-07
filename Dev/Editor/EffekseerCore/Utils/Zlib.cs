using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Utils
{
	/// <summary>
	/// zlib compatible with python zlib level=6
	/// </summary>
	public class Zlib
	{
		public static unsafe byte[] Compress(byte[] buffer)
		{
			using (var compressStream = new System.IO.MemoryStream())
			using (var compressor = new System.IO.Compression.DeflateStream(compressStream, System.IO.Compression.CompressionLevel.Optimal))
			{
				UInt32 adder = 0;
				fixed (byte* b = buffer)
				{
					adder = CalcAdler32(b, (UInt32)buffer.Length);
				}

				compressor.Write(buffer, 0, buffer.Count());
				compressor.Close();
				var compressed = compressStream.ToArray();

				List<byte[]> dst = new List<byte[]>();
				dst.Add(new byte[] { 0x78, 0x9c });
				dst.Add(compressed);
				dst.Add(BitConverter.GetBytes(adder).Reverse().ToArray());

				return dst.SelectMany(_ => _).ToArray();
			}
		}

		public static byte[] Decompress(byte[] buffer)
		{
			var decompressBuffer = new List<byte>();
			using (var decompressStream = new System.IO.MemoryStream(buffer.Skip(2).Take(buffer.Length - 4).ToArray()))
			using (var decompressor = new System.IO.Compression.DeflateStream(decompressStream, System.IO.Compression.CompressionMode.Decompress))
			{
				while (true)
				{
					byte[] temp = new byte[1024];
					int readSize = decompressor.Read(temp, 0, temp.Length);
					if (readSize == 0) break;
					decompressBuffer.AddRange(temp.Take(readSize));
				}
			}

			return decompressBuffer.ToArray();
		}

		static unsafe UInt32 CalcAdler32(byte* data, UInt32 len)
		{
			UInt32 a = 1, b = 0;

			while (len > 0)
			{
				UInt32 rest = len > 5550 ? 5550 : len;
				len -= rest;
				do
				{
					a += *data++;
					b += a;
					rest--;
				} while (rest > 0);

				a %= 65521;
				b %= 65521;
			}

			return (b << 16) | a;
		}
	}
}
