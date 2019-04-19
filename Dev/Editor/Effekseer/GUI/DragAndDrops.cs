using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI
{
	class DragAndDrops
	{
		static byte[] tempBuffer = new byte[512];

		public static string ImageKey = "__IMAGE__";

		public static void UpdateImageSrc(string path)
		{
			if (Manager.NativeManager.BeginDragDropSource())
			{
				byte[] idBuf = Encoding.UTF8.GetBytes(path);
				if (Manager.NativeManager.SetDragDropPayload(ImageKey, idBuf, idBuf.Length))
				{
				}
				Manager.NativeManager.Text(path);

				Manager.NativeManager.EndDragDropSource();
			}
		}

		public static string UpdateImageDst()
		{
			string str = null;
			if (Manager.NativeManager.BeginDragDropTarget())
			{
				int size = 0;
				if (Manager.NativeManager.AcceptDragDropPayload(ImageKey, tempBuffer, tempBuffer.Length, ref size))
				{
					str = Encoding.UTF8.GetString(tempBuffer.Take(size).ToArray());
				}

				Manager.NativeManager.EndDragDropTarget();
			}

			return str;
		}
	}
}
