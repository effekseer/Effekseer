﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI
{
	class DragAndDrops
	{
		static byte[] tempBuffer = new byte[512];

		public static void UpdateFileSrc(string path, FileType type, Action displayAction = null)
		{
			if (Manager.NativeManager.BeginDragDropSource())
			{
				string key = type.ToString() + "File";
				byte[] idBuf = Encoding.UTF8.GetBytes(path);
				if (Manager.NativeManager.SetDragDropPayload(key, idBuf, idBuf.Length))
				{
				}

				if (displayAction != null)
				{
					displayAction();
				}
				else
				{
					Manager.NativeManager.Text(path);
				}

				Manager.NativeManager.EndDragDropSource();
			}
		}

		public static string UpdateFileDst(FileType type)
		{
			string str = null;
			if (Manager.NativeManager.BeginDragDropTarget())
			{
				string key = type.ToString() + "File";
				int size = 0;
				if (Manager.NativeManager.AcceptDragDropPayload(key, tempBuffer, tempBuffer.Length, ref size))
				{
					str = Encoding.UTF8.GetString(tempBuffer.Take(size).ToArray());
				}

				Manager.NativeManager.EndDragDropTarget();
			}

			return str;
		}
	}
}