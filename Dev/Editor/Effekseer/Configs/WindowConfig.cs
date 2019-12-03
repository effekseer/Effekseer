using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.Utl;

namespace Effekseer.Configs
{
	class WindowConfig
	{
		public int WindowWidth;
		public int WindowHeight;
		public int WindowPosX = -100000;
		public int WindowPosY = -100000;
		public bool WindowIsMaximumMode = false;

		public string[] OpenNodes = new string[0];
		public bool Load(string path)
		{
			if (!System.IO.File.Exists(path)) return false;

			var doc = new System.Xml.XmlDocument();

			doc.Load(path);

			if (doc.ChildNodes.Count != 2) return false;
			if (doc.ChildNodes[1].Name != "Root") return false;

			WindowWidth = doc["Root"]["WindowWidth"]?.GetTextAsInt() ?? 1280;
			WindowHeight = doc["Root"]["WindowHeight"]?.GetTextAsInt() ?? 720;
			WindowPosX = doc["Root"]["WindowPosX"]?.GetTextAsInt() ?? -100000;
			WindowPosY = doc["Root"]["WindowPosY"]?.GetTextAsInt() ?? -100000;
			WindowIsMaximumMode = (doc["Root"]["WindowIsMaximumMode"]?.GetTextAsInt() ?? 0) > 0;

			var docks = doc["Root"]["Docks"];

			if (docks != null)
			{
				List<string> openNodes = new List<string>();
				for (int i = 0; i < docks.ChildNodes.Count; i++)
				{
					var panel = docks.ChildNodes[i];
					openNodes.Add(panel.Name);
				}
			}

			return true;
		}
	}
}
