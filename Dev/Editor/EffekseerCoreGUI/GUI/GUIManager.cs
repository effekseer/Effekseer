using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.swig;
using Effekseer.Utl;

namespace Effekseer.GUI
{
	public class GUIManager
	{
		public static swig.GUIManager NativeManager;
		public static swig.Native Native;
		public static swig.MainWindow MainWindow;
		public static swig.IO IO;

		//public static bool Initialize(int width, int height, swig.DeviceType deviceType)
		//{
		//	return true;
		//}
	}
}
