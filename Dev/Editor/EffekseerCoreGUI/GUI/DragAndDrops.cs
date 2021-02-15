using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI
{
	class DragAndDrops
	{
		public enum FileType
		{
			Image,
			Sound,
			Model,
			Material,
			Curve,
			Other,
		}

		static byte[] tempBuffer = new byte[512];

		public static void UpdateFileSrc(string path, FileType type)
		{
			if (GUIManager.NativeManager.BeginDragDropSource())
			{
				string key = type.ToString() + "File";
				byte[] idBuf = Encoding.UTF8.GetBytes(path);
				if (GUIManager.NativeManager.SetDragDropPayload(key, idBuf, idBuf.Length))
				{
				}
				GUIManager.NativeManager.Text(path);

				GUIManager.NativeManager.EndDragDropSource();
			}
		}

		public static string UpdateFileDst(FileType type)
		{
			string str = null;
			if (GUIManager.NativeManager.BeginDragDropTarget())
			{
				string key = type.ToString() + "File";
				int size = 0;
				if (GUIManager.NativeManager.AcceptDragDropPayload(key, tempBuffer, tempBuffer.Length, ref size))
				{
					str = Encoding.UTF8.GetString(tempBuffer.Take(size).ToArray());
				}

				GUIManager.NativeManager.EndDragDropTarget();
			}

			return str;
		}
	}
}
