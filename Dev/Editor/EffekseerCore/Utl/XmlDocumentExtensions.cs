using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Utl
{
	public static class XmlDocumentExtensions
	{
		/// <summary>
		/// ChildNodesがXmlTextのみ1つの時、そのXmlTextの値を取得する。
		/// </summary>
		/// <param name="xmlnode"></param>
		/// <returns></returns>
		public static string GetText(this System.Xml.XmlNode xmlnode)
		{
			if (xmlnode.ChildNodes.Count == 1 && xmlnode.ChildNodes[0] is System.Xml.XmlText)
			{
				return xmlnode.ChildNodes[0].Value;
			}

			return string.Empty;
		}

		public static int GetTextAsInt(this System.Xml.XmlNode xmlnode)
		{
			int ret = 0;

			if (xmlnode.ChildNodes.Count == 1 && xmlnode.ChildNodes[0] is System.Xml.XmlText)
			{
				int.TryParse(xmlnode.ChildNodes[0].Value, System.Globalization.NumberStyles.Integer, Setting.NFI, out ret);
			}

			return ret;
		}

		public static float GetTextAsFloat(this System.Xml.XmlNode xmlnode)
		{
			float ret = 0;

			if (xmlnode.ChildNodes.Count == 1 && xmlnode.ChildNodes[0] is System.Xml.XmlText)
			{
				float.TryParse(xmlnode.ChildNodes[0].Value, System.Globalization.NumberStyles.Float, Setting.NFI, out ret);
			}

			return ret;
		}

		/// <summary>
		/// ChildNodesからlocalnameのXmlNodeを探し出し、そのXmlNodeのChildNodesがXmlTextのみ1つの時、そのXmlTextの値を取得する。
		/// </summary>
		/// <param name="xmlnode"></param>
		/// <param name="localname"></param>
		/// <returns></returns>
		public static string GetText(this System.Xml.XmlNode xmlnode, string localname)
		{
			var local = xmlnode[localname];

			if (local == null) return string.Empty;

			return GetText(local);
		}

		public static int GetTextAsInt(this System.Xml.XmlNode xmlnode, string localname)
		{
			var local = xmlnode[localname];

			if (local == null) return 0;

			return GetTextAsInt(local);
		}

		public static float GetTextAsFloat(this System.Xml.XmlNode xmlnode, string localname)
		{
			var local = xmlnode[localname];

			if (local == null) return 0;

			return GetTextAsFloat(local);
		}

		public static System.Xml.XmlElement[] GetElements(this System.Xml.XmlNode xmlnode, string localname)
		{
			List<System.Xml.XmlElement> elements = new List<System.Xml.XmlElement>();

			for (int i = 0; i < xmlnode.ChildNodes.Count; i++)
			{
				if (xmlnode.ChildNodes[i].LocalName == localname && xmlnode.ChildNodes[i] is System.Xml.XmlElement)
				{
					elements.Add((System.Xml.XmlElement)xmlnode.ChildNodes[i]);
				}
			}

			return elements.ToArray();
		}

		public static bool TryGet(this System.Xml.XmlNode xmlnode, string localname, ref string ret)
		{
			var local = xmlnode[localname];

			if (local == null) return false;

			ret = GetText(local);

			return true;
		}

		public static bool TryGet(this System.Xml.XmlNode xmlnode, string localname, ref int ret)
		{
			var local = xmlnode[localname];

			if (local == null) return false;

			ret = GetTextAsInt(local);

			return true;
		}

		public static bool TryGet(this System.Xml.XmlNode xmlnode, string localname, ref float ret)
		{
			var local = xmlnode[localname];

			if (local == null) return false;

			ret = GetTextAsFloat(local);

			return true;
		}

		public static System.Xml.XmlElement CreateTextElement(this System.Xml.XmlDocument doc, string name, string text)
		{
			var element = doc.CreateElement(name);
			element.AppendChild(doc.CreateTextNode(text));
			return element;
		}

		public static System.Xml.XmlElement CreateTextElement(this System.Xml.XmlDocument doc, string name, int value)
		{
			return CreateTextElement(doc, name, value.ToString());
		}
	}
}
