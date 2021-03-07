using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Utl;

namespace Effekseer.GUI
{
	class RecentFiles
	{
		public const string ConfigRecentFileName = "config.recent.xml";

		static string configRecentPath;

		/// <summary>
		/// 最近使用したファイルが変更
		/// </summary>
		public static event EventHandler OnChangeRecentFiles;

		/// <summary>
		/// Recent files
		/// </summary>
		static LinkedList<string> recentFiles = new LinkedList<string>();

		static RecentFiles()
		{
			configRecentPath = System.IO.Path.Combine(Manager.GetEntryDirectory(), ConfigRecentFileName);
		}

		internal static void LoadRecentConfig()
		{
			if (System.IO.File.Exists(configRecentPath))
			{
				System.Xml.XmlDocument doc = new System.Xml.XmlDocument();
				doc.Load(configRecentPath);
				var files = doc["Recent"].GetElements("File");
				foreach (var file in files)
				{
					AddRecentFile(file.GetText());
				}
			}
		}

		internal static void SaveRecentConfig()
		{
			var rf = GetRecentFiles();

			System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

			System.Xml.XmlElement project_root = doc.CreateElement("Recent");

			foreach (var f in rf.Reverse())
			{
				project_root.AppendChild(doc.CreateTextElement("File", f));
			}

			doc.AppendChild(project_root);

			var dec = doc.CreateXmlDeclaration("1.0", "utf-8", null);
			doc.InsertBefore(dec, project_root);
			doc.Save(configRecentPath);
		}

		/// <summary>
		/// 最近使用したファイルの追加
		/// </summary>
		/// <param name="fullPath">絶対パス</param>
		public static void AddRecentFile(string fullPath)
		{
			var errorText = Resources.GetString("NotAbsolutePathError");

			if (System.IO.Path.GetFullPath(fullPath) != fullPath) throw new Exception(errorText);

			recentFiles.Remove(fullPath);
			recentFiles.AddFirst(fullPath);
			while (recentFiles.Count > 10)
			{
				recentFiles.RemoveLast();
			}
			if (OnChangeRecentFiles != null)
			{
				OnChangeRecentFiles(null, null);
			}
		}

		/// <summary>
		/// 最近使用したファイル一覧の取得
		/// </summary>
		/// <returns></returns>
		public static string[] GetRecentFiles()
		{
			return recentFiles.ToArray();
		}
	}
}
