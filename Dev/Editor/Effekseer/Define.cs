using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer
{
	public class ErrorUtils
	{
		public static void ThrowFileNotfound()
		{
			throw new Exception(Resources.GetString("MismatchResourceError"));
		}

		public static void ShowErrorByNodeLayerLimit()
		{
			var mb = new GUI.Dialog.MessageBox();
			mb.Show("Error", String.Format(Resources.GetString("LayerLimitError"), Constant.NodeLayerLimit));
		}
	}

}
