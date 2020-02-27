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

	/// <summary>
	/// Attribute for unique name
	/// </summary>
	[AttributeUsage(
	AttributeTargets.Class | AttributeTargets.Property | AttributeTargets.Field | AttributeTargets.Method,
	AllowMultiple = false,
	Inherited = false)]
	public class UniqueNameAttribute : Attribute
	{
		public UniqueNameAttribute()
		{
			value = string.Empty;
		}

		public string value
		{
			get;
			set;
		}

		/// <summary>
		/// Get unique name from attribute
		/// </summary>
		/// <param name="attributes"></param>
		/// <returns></returns>
		public static string GetUniqueName(object[] attributes)
		{
			if (attributes != null && attributes.Length > 0)
			{
				foreach (var attribute in attributes)
				{
					if (!(attribute is UniqueNameAttribute)) continue;

					return ((UniqueNameAttribute)attribute).value;
				}
			}

			return string.Empty;
		}
	}
}
