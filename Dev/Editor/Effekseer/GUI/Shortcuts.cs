using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Effekseer.GUI
{
	class Shortcuts
	{

		const string ConfigPath = "config.shortcut.xml";

		static Dictionary<string, Shortcut> shortcuts = new Dictionary<string, Shortcut>();

		public static void LoadShortcuts()
		{
			if (System.IO.File.Exists(ConfigPath))
			{

			}
			else
			{
				AddShortcut(new Shortcut("Internal.New", true, true, false, (int)Keys.N, null));
				AddShortcut(new Shortcut("Internal.Open", true, true, false, (int)Keys.O, null));
				AddShortcut(new Shortcut("Internal.Overwrite", true, false, false, (int)Keys.S, null));
				AddShortcut(new Shortcut("Internal.SaveAs", true, false, false, 0, null));
				AddShortcut(new Shortcut("Internal.Exit", false, false, true, (int)Keys.F4, null));

				AddShortcut(new Shortcut("Internal.PlayViewer", false, false, false, (int)Keys.Space, null));
				AddShortcut(new Shortcut("Internal.StopViewer", true, false, true, (int)Keys.Left, null));

				AddShortcut(new Shortcut("Internal.StepViewer", true, false, false, (int)Keys.Right, null));
				AddShortcut(new Shortcut("Internal.BackStepViewer", true, false, false, (int)Keys.Left, null));

				AddShortcut(new Shortcut("Internal.Undo", true, false, false, (int)Keys.Z, null));
				AddShortcut(new Shortcut("Internal.Redo", true, false, false, (int)Keys.Y, null));
				AddShortcut(new Shortcut("Internal.Copy", true, false, false, (int)Keys.C, null));
				AddShortcut(new Shortcut("Internal.Paste", true, false, false, (int)Keys.V, null));
				AddShortcut(new Shortcut("Internal.PasteInfo", true, false, true, (int)Keys.V, null));

				AddShortcut(new Shortcut("Internal.AddNode", true, false, false, (int)Keys.A, null));
				AddShortcut(new Shortcut("Internal.InsertNode", true, false, false, (int)Keys.I, null));
				AddShortcut(new Shortcut("Internal.RemoveNode", true, false, false, (int)Keys.R, null));
			}
		}

		public static void SeveShortcuts()
		{ 
		
		}

		public static string GetShortcutText(string uniquename)
		{
			if (shortcuts.ContainsKey(uniquename))
			{
				return shortcuts[uniquename].GetShortcutText();
			}
			return string.Empty;
		}

		public static void SetFunction(string uniquename, Func<bool> function)
		{
			if (shortcuts.ContainsKey(uniquename))
			{
				shortcuts[uniquename].SetFunction(function);
			}
		}

		internal static void ProcessCmdKey(ref Message msg, Keys keyData, ref bool handle)
		{
			handle = true;

			foreach (var shortcut in shortcuts)
			{
				var keys = shortcut.Value.GetKeys();
				if (keys == (int)keyData)
				{
					if(shortcut.Value.Function())
					{
						return;
					}
				}
			}

			handle = false;
		}

		static void AddShortcut(Shortcut shortcut)
		{
			if (!shortcuts.ContainsKey(shortcut.UniqueName))
			{
				shortcuts.Add(shortcut.UniqueName, shortcut);
			}
		}

		class Shortcut
		{
			public string UniqueName
			{
				get;
				private set;
			}

			public bool Control
			{
				get;
				private set;
			}

			public bool Shift
			{
				get;
				private set;
			}

			public bool Alt
			{
				get;
				private set;
			}

			public int Key
			{
				get;
				private set;
			}

			public Func<bool> Function
			{
				get;
				private set;
			}

			public Shortcut(string uniquename, bool ctrl, bool shift, bool alt, int key, Func<bool> function)
			{
				UniqueName = uniquename;
				Control = ctrl;
				Shift = shift;
				Alt = alt;
				Key = key;
				Function = function;
			}

			public void SetFunction(Func<bool> function)
			{
				Function = function;
			}

			public int GetKeys()
			{
				int r = 0;
				
				if (Control)
				{
					r += (int)Keys.Control;
				}

				if (Shift)
				{
					r += (int)Keys.Shift;
				}

				if (Alt)
				{
					r += (int)Keys.Alt;
				}

				r += Key;

				return r;
			}

			public string GetShortcutText()
			{
				if (Function == null) return string.Empty;
				if (Key == 0) return string.Empty;

				string r = string.Empty;
				
				if (Control)
				{
					r += "Ctrl+";
				}

				if (Shift)
				{
					r += "Shift+";
				}

				if (Alt)
				{
					r += "Alt+";
				}

				r += ((System.Windows.Forms.Keys)Key).ToString();

				return r;
			}
		}
	}
}
