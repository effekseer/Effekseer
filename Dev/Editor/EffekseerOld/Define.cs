using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer
{
	/// <summary>
	/// 唯一名称を設定する属性
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
		/// 属性から唯一名称を取得する。
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
