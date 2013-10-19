using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace Effekseer.Utl
{
	public static class BitConverterExtensions
	{
		public static byte[] GetBytes(this int value)
		{
			return BitConverter.GetBytes(value);
		}

		public static byte[] GetBytes<T>(this T obj) where T : struct
		{
			int size = Marshal.SizeOf(obj);
			byte[] bytes = new byte[size];
			GCHandle gch = GCHandle.Alloc(bytes, GCHandleType.Pinned);
			Marshal.StructureToPtr(obj, gch.AddrOfPinnedObject(), false);
			gch.Free();
			return bytes;
		}
	}
}
