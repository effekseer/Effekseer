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

		public void Get(ref string value, Encoding encoding)
		{
			int length = 0;
			Get(ref length);

			int readLength = length;

			if(buffer[offset + length - 1] == 0)
			{
				readLength -= 1;
			}

			if (buffer[offset + length - 2] == 0)
			{
				readLength -= 1;
			}

			value = encoding.GetString(buffer, offset, readLength);
			offset += length;
		}
	}
}
