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

		public void Push(bool value)
		{
			buffers.Add(BitConverter.GetBytes(value ? 1 : 0));
		}

		public void Push(byte[] buffer)
		{
			Push(buffer.Count());
			buffers.Add((byte[])buffer.Clone());
		}

		public void Push(string value, Encoding encoding, bool zeroEnd = true, int bufLenSize = 4)
		{
			var strBuf = encoding.GetBytes(value);
			var length = strBuf.Count();

			if (zeroEnd)
			{
				if (encoding == Encoding.Unicode)
				{
					length += 2;
				}
				else if (encoding == Encoding.UTF8)
				{
					length += 1;
				}
				else
				{
					throw new NotImplementedException();
				}
			}

			if (encoding == Encoding.Unicode)
			{
				length /= 2;
			}

			if (bufLenSize == 4)
			{
				buffers.Add(BitConverter.GetBytes(length));
			}
			else if (bufLenSize == 2)
			{
				buffers.Add(BitConverter.GetBytes((UInt16)length));
			}
			else if (bufLenSize == 1)
			{
				buffers.Add(new[] { (Byte)length });
			}

			buffers.Add(strBuf);

			if (zeroEnd)
			{
				if (encoding == Encoding.Unicode)
				{
					buffers.Add(new byte[] { 0, 0 });
				}
				else if (encoding == Encoding.UTF8)
				{
					buffers.Add(new byte[] { 0 });
				}
				else
				{
					throw new NotImplementedException();
				}
			}
		}

		public void PushDirectly(byte[] buffer)
		{
			buffers.Add((byte[])buffer.Clone());
		}
	}
}
