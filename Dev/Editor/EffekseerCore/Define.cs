using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer
{
	/// <summary>
	/// 値が変化したときのイベント
	/// </summary>
	/// <param name="sender"></param>
	/// <param name="e"></param>
	public delegate void ChangedValueEventHandler(object sender, ChangedValueEventArgs e);

	public enum ChangedValueType
	{ 
		Execute,
		Unexecute,
	}

	/// <summary>
	/// 値が変化したときのイベントの引数
	/// </summary>
	public class ChangedValueEventArgs : EventArgs
	{
		public object Value
		{
			get;
			private set;
		}

		public ChangedValueType Type
		{
			get;
			private set;
		}

		internal ChangedValueEventArgs(object value, ChangedValueType type)
		{
			Value = value;
			Type = type;
		}
	}

	/// <summary>
	/// 言語
	/// </summary>
	public enum Language
	{
        [Name(value = "日本語", language = Language.Japanese)]
        [Name(value = "Japanese", language = Language.English)]
		Japanese,
        [Name(value = "英語", language = Language.Japanese)]
        [Name(value = "English", language = Language.English)]
		English,
	}

	/// <summary>
	/// 名称を設定する属性
	/// </summary>
	[AttributeUsage(
	AttributeTargets.Class | AttributeTargets.Property | AttributeTargets.Field | AttributeTargets.Method,
	AllowMultiple = true,
	Inherited = false)]
	public class NameAttribute : Attribute
	{
		public NameAttribute()
		{
			language = Language.Japanese;
			value = string.Empty;
		}

		public Language language
		{
			get;
			set;
		}

		public string value
		{
			get;
			set;
		}

		/// <summary>
		/// 属性から名称を取得する。
		/// </summary>
		/// <param name="attributes"></param>
		/// <returns></returns>
		public static string GetName(object[] attributes)
		{
			if (attributes != null && attributes.Length > 0)
			{
				foreach (var attribute in attributes)
				{
					if (!(attribute is NameAttribute)) continue;

					if (((NameAttribute)attribute).language == Core.Language)
					{
						return ((NameAttribute)attribute).value;
					}
				}
			}

			return string.Empty;
		}
	}

	/// <summary>
	/// 説明を設定する属性
	/// </summary>
	[AttributeUsage(
	AttributeTargets.Class | AttributeTargets.Property | AttributeTargets.Field,
	AllowMultiple = true,
	Inherited = false)]
	public class DescriptionAttribute : Attribute
	{
		public DescriptionAttribute()
		{
			language = Language.Japanese;
			value = string.Empty;
		}

		public Language language
		{
			get;
			set;
		}

		public string value
		{
			get;
			set;
		}

		/// <summary>
		/// 属性から説明を取得する。
		/// </summary>
		/// <param name="attributes"></param>
		/// <returns></returns>
		public static string GetDescription(object[] attributes)
		{
			if (attributes != null && attributes.Length > 0)
			{
				foreach (var attribute in attributes)
				{
					if (!(attribute is DescriptionAttribute)) continue;

					if (((DescriptionAttribute)attribute).language == Core.Language)
					{
						return ((DescriptionAttribute)attribute).value;
					}
				}
			}

			return string.Empty;
		}
	}

	public class Setting
	{
		public static System.Globalization.NumberFormatInfo NFI
		{
			get;
			private set;
		}

		static Setting()
		{
			var culture = new System.Globalization.CultureInfo("ja-JP");
			NFI = culture.NumberFormat;
		}
	}
}
