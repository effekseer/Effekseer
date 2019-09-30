using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Utl
{
	class BinaryReader
	{
		byte[] buffer = null;
		int offset = 0;

		public BinaryReader(byte[] buffer)
		{
			this.buffer = buffer;
			this.offset = 0;
		}

		public void Get(ref int value)
		{
			value = BitConverter.ToInt32(buffer, offset);
			offset += 4;
		}

		public void Get(ref UInt16 value)
		{
			value = BitConverter.ToUInt16(buffer, offset);
			offset += 2;
		}

		public void Get(ref Int16 value)
		{
			value = BitConverter.ToInt16(buffer, offset);
			offset += 2;
		}

		public void Get(ref Byte value)
		{
			value = buffer[offset];
			offset += 1;
		}

		public void Get(ref float value)
		{
			value = BitConverter.ToSingle(buffer, offset);
			offset += 4;
		}

		public void Get(ref bool value)
		{
			value = BitConverter.ToInt32(buffer, offset) > 0;
			offset += 4;
		}

		public void Get(ref string value, Encoding encoding, bool zeroEnd = true, int bufLenSize = 4)
		{
			int length = 0;

			if(bufLenSize == 4)
			{
				int length4 = 0;
				Get(ref length4);
				length = length4;
			}
			else if (bufLenSize == 2)
			{
				UInt16 length2 = 0;
				Get(ref length2);
				length = length2;
			}
			else if (bufLenSize == 1)
			{
				Byte length1 = 0;
				Get(ref length1);
				length = length1;
			}

			int readLength = length;

			if (zeroEnd)
			{
				if (encoding == Encoding.Unicode)
				{
					readLength -= 2;
				}
				else if (encoding == Encoding.UTF8)
				{
					readLength -= 1;
				}
				else
				{
					throw new NotImplementedException();
				}
			}

			if (encoding == Encoding.Unicode)
			{
				readLength *= 2;
			}

			value = encoding.GetString(buffer, offset, readLength);
			offset += length;
		}
	}
}
