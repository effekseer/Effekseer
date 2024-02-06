using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer
{
	public class LanguageTable
	{
		static int selectedIndex = 0;
		static List<string> languages = new List<string> { "en" };

		public static void LoadTable(string path)
		{
			var lines = System.IO.File.ReadAllLines(path);
			languages = lines.ToList();
		}

		public static void CreateTableFromDirectory(string rootDirectory)
		{
			languages = new List<string> { "en" };

			var languageDir = Utils.Misc.BackSlashToSlash(System.IO.Path.Combine(rootDirectory, "resources/languages/"));

			var directories = System.IO.Directory.GetDirectories(languageDir).Select(_ => Utils.Misc.BackSlashToSlash(_));
			foreach (var directory in directories)
			{
				var name = directory.Split('/').Last();
				if (name == "en")
				{
					continue;
				}
				languages.Add(name);
			}


		}

		public static void StoreLanguageNames(string rootDirectory)
		{
			var languageDir = Utils.Misc.BackSlashToSlash(System.IO.Path.Combine(rootDirectory, "resources/languages/"));

			foreach (var lang in languages)
			{
				var languageNameFilePath = languageDir + lang + "/language.txt";
				if (System.IO.File.Exists(languageNameFilePath))
				{
					var name = System.IO.File.ReadAllText(languageNameFilePath, Encoding.UTF8);
					MultiLanguageTextProvider.AddText("Language_" + lang, name);
				}
			}
		}

		public static void SelectLanguage(int index, bool callEvent = true)
		{
			if (selectedIndex == index) return;
			selectedIndex = index;
			if (OnLanguageChanged != null && callEvent)
			{
				OnLanguageChanged(null, null);
			}
		}

		public static void SelectLanguage(string key, bool callEvent = true)
		{
			var ind = languages.Select((i, v) => Tuple35.Create(i, v)).FirstOrDefault(_ => _.Item1 == key).Item2;
			SelectLanguage(ind, callEvent);
		}

		public static List<string> Languages { get { return languages; } }

		public static int SelectedIndex { get { return selectedIndex; } }

		public static Action<object, ChangedValueEventArgs> OnLanguageChanged;
	}

	public class MultiLanguageTextProvider
	{
		internal static int UpdateCounter { get; private set; }

		static Dictionary<string, string> texts = new Dictionary<string, string>();

		public static string Language { get; set; }

		public static string RootDirectory = string.Empty;

		static MultiLanguageTextProvider()
		{
			Language = "en";
		}

		public static void Reset()
		{
			texts.Clear();
			UpdateCounter++;
		}

		public static void LoadCSV(string path)
		{
			LoadCSV(path, "en");
			if (LanguageTable.Languages.Count > 0)
			{
				LoadCSV(path, LanguageTable.Languages[LanguageTable.SelectedIndex]);
			}
		}

		internal static void LoadCSV(string path, string language)
		{
			var csvFilePath = RootDirectory + "resources/languages/" + language + "/" + path;
			if (!System.IO.File.Exists(csvFilePath)) {
				Utils.Logger.Write(string.Format("Missing language file : {0}", csvFilePath));
				return;
			}
			using (var streamReader = new System.IO.StreamReader(csvFilePath, Encoding.UTF8))
			{
				var records = Utils.CsvReaader.Read(streamReader.ReadToEnd());

				foreach (var record in records)
				{
					if (record.Count < 2) continue;
					if (record[0] == string.Empty) continue;

					AddText(record[0], record[1]);
				}
			}
		}

		public static bool HasKey(string key)
		{
			return texts.ContainsKey(key);
		}

		public static string GetText(string key)
		{
			string ret = string.Empty;
			if (texts.TryGetValue(key, out ret))
			{
				return ret;
			}
			return key;
		}

		public static void AddText(string key, string text)
		{
			if (texts.ContainsKey(key))
			{
				texts[key] = text;
			}
			else
			{
				texts.Add(key, text);
			}
		}
	}

	public class MultiLanguageString
	{
		string cached = null;

		internal int UpdateCounter = 0;

		internal string Key = string.Empty;

		public MultiLanguageString(string key)
		{
			Key = key;
		}

		public string Value
		{
			get
			{
				if (UpdateCounter != MultiLanguageTextProvider.UpdateCounter || cached == null)
				{
					UpdateCounter = MultiLanguageTextProvider.UpdateCounter;
					cached = MultiLanguageTextProvider.GetText(Key);
				}
				return cached;
			}
		}

		public override string ToString()
		{
			return Value;
		}
	}
}