using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Utils
{
	class BinaryWriter
	{
		List<byte[]> buffers = new List<byte[]>();

		public BinaryWriter()
		{
		}

		public byte[] GetBinary()
		{
			return buffers.SelectMany(_ => _).ToArray();
		}

		public void Push(int value)
		{
			buffers.Add(BitConverter.GetBytes(value));
		}

		public void Push(UInt32 value)
		{
			buffers.Add(BitConverter.GetBytes(value));
		}
		public void Push(Int16 value)
		{
			buffers.Add(BitConverter.GetBytes(value));
		}

		public void Push(UInt16 value)
		{
			buffers.Add(BitConverter.GetBytes(value));
		}

		public void Push(byte[] buffer)
		{
			Push(buffer.Count());
			buffers.Add((byte[])buffer.Clone());
		}

		public void Push(string value, Encoding encoding)
		{
			var strBuf = encoding.GetBytes(value);
			buffers.Add(BitConverter.GetBytes((strBuf.Count() + 2) / 2));
			buffers.Add(strBuf);

			if(encoding == Encoding.Unicode)
			{
				buffers.Add(new byte[] { 0, 0 });
			}
			else if (encoding == Encoding.UTF8)
			{
				buffers.Add(new byte[] { 0, 0 });
			}
			else
			{
				throw new NotImplementedException();
			}
		}

		public void PushDirectly(byte[] buffer)
		{
			buffers.Add((byte[])buffer.Clone());
		}
	}
}
