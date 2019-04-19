using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.GUI
{
	enum ShortcutKeys
	{
		UNKNOWN = -1,
		SPACE = 32,
		APOSTROPHE = 39, /* ' */
		COMMA = 44, /* , */
		MINUS = 45, /* - */
		PERIOD = 46, /* . */
		SLASH = 47, /* / */
		Num0 = 48,
		Num1 = 49,
		Num2 = 50,
		Num3 = 51,
		Num4 = 52,
		Num5 = 53,
		Num6 = 54,
		Num7 = 55,
		Num8 = 56,
		Num9 = 57,
		SEMICOLON = 59, /* ; */
		EQUAL = 61, /* = */
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,
		LEFT_BRACKET = 91, /* [ */
		BACKSLASH = 92, /* \ */
		RIGHT_BRACKET = 93, /* ] */
		GRAVE_ACCENT = 96, /* ` */
		WORLD_1 = 161, /* non-US #1 */
		WORLD_2 = 162, /* non-US #2 */
		ESCAPE = 256,
		ENTER = 257,
		TAB = 258,
		BACKSPACE = 259,
		INSERT = 260,
		DELETE = 261,
		RIGHT = 262,
		LEFT = 263,
		DOWN = 264,
		UP = 265,
		PAGE_UP = 266,
		PAGE_DOWN = 267,
		HOME = 268,
		END = 269,
		CAPS_LOCK = 280,
		SCROLL_LOCK = 281,
		NUM_LOCK = 282,
		PRINT_SCREEN = 283,
		PAUSE = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,
		KP_0 = 320,
		KP_1 = 321,
		KP_2 = 322,
		KP_3 = 323,
		KP_4 = 324,
		KP_5 = 325,
		KP_6 = 326,
		KP_7 = 327,
		KP_8 = 328,
		KP_9 = 329,
		KP_DECIMAL = 330,
		KP_DIVIDE = 331,
		KP_MULTIPLY = 332,
		KP_SUBTRACT = 333,
		KP_ADD = 334,
		KP_ENTER = 335,
		KP_EQUAL = 336,
		LEFT_SHIFT = 340,
		LEFT_CONTROL = 341,
		LEFT_ALT = 342,
		LEFT_SUPER = 343,
		RIGHT_SHIFT = 344,
		RIGHT_CONTROL = 345,
		RIGHT_ALT = 346,
		RIGHT_SUPER = 347,
		MENU = 348,
	}

	class Shortcuts
	{

		const string ConfigPath = "config.shortcut.xml";

		static Dictionary<string, Shortcut> shortcuts = new Dictionary<string, Shortcut>();

		static bool[] preKeys = new bool[512];
		static bool[] currentKeys = new bool[512];

		static Shortcuts()
		{
			for(int i = 0; i < 512; i++)
			{
				preKeys[i] = false;
				currentKeys[i] = false;
			}
		}

		public static void LoadShortcuts()
		{
			if (System.IO.File.Exists(ConfigPath))
			{

			}
			else
			{
				AddShortcut(new Shortcut("Internal.New", true, true, false, (int)ShortcutKeys.N, null));
				AddShortcut(new Shortcut("Internal.Open", true, true, false, (int)ShortcutKeys.O, null));
				AddShortcut(new Shortcut("Internal.Overwrite", true, false, false, (int)ShortcutKeys.S, null));
				AddShortcut(new Shortcut("Internal.SaveAs", true, false, false, 0, null));
				AddShortcut(new Shortcut("Internal.Exit", false, false, true, (int)ShortcutKeys.F4, null));

				AddShortcut(new Shortcut("Internal.PlayViewer", false, false, false, (int)ShortcutKeys.SPACE, null));
				AddShortcut(new Shortcut("Internal.StopViewer", true, false, true, (int)ShortcutKeys.LEFT, null));

				AddShortcut(new Shortcut("Internal.StepViewer", true, false, false, (int)ShortcutKeys.RIGHT, null));
				AddShortcut(new Shortcut("Internal.BackStepViewer", true, false, false, (int)ShortcutKeys.LEFT, null));

				AddShortcut(new Shortcut("Internal.Undo", true, false, false, (int)ShortcutKeys.Z, null));
				AddShortcut(new Shortcut("Internal.Redo", true, false, false, (int)ShortcutKeys.Y, null));
				AddShortcut(new Shortcut("Internal.Copy", true, false, false, (int)ShortcutKeys.C, null));
				AddShortcut(new Shortcut("Internal.Paste", true, false, false, (int)ShortcutKeys.V, null));
				AddShortcut(new Shortcut("Internal.PasteInfo", true, false, true, (int)ShortcutKeys.V, null));

				AddShortcut(new Shortcut("Internal.AddNode", true, false, false, (int)ShortcutKeys.A, null));
				AddShortcut(new Shortcut("Internal.InsertNode", true, false, false, (int)ShortcutKeys.I, null));
				AddShortcut(new Shortcut("Internal.RemoveNode", true, false, false, (int)ShortcutKeys.R, null));
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

		internal static void Update()
		{
			for (int i = 0; i < 512; i++)
			{
				preKeys[i] = currentKeys[i];
			}

			for (int i = 0; i < 512; i++)
			{
				currentKeys[i] = Manager.NativeManager.IsKeyDown(i);
			}
		}

		internal static void ProcessCmdKey(ref bool handle)
		{
			handle = true;

			foreach (var shortcut in shortcuts)
			{
				var keys = shortcut.Value;
				if (keys.IsPushed(Manager.NativeManager))
				{
					if (shortcut.Value.Function())
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

			public bool IsPushed(swig.GUIManager manager)
			{
				bool pressed = currentKeys[Key];
				bool pushed = currentKeys[Key] && !preKeys[Key];
				if(Control)
				{
					if (swig.GUIManager.IsMacOSX())
					{
						pressed = pressed && (currentKeys[(int)ShortcutKeys.LEFT_SUPER] || currentKeys[(int)ShortcutKeys.RIGHT_SUPER]);
						pushed = pushed || (
							(currentKeys[(int)ShortcutKeys.LEFT_SUPER] && !preKeys[(int)ShortcutKeys.LEFT_SUPER]) ||
							(currentKeys[(int)ShortcutKeys.RIGHT_SUPER] && !preKeys[(int)ShortcutKeys.RIGHT_SUPER]));
					}
					else
					{
						pressed = pressed && (currentKeys[(int)ShortcutKeys.LEFT_CONTROL] || currentKeys[(int)ShortcutKeys.RIGHT_CONTROL]);
						pushed = pushed || (
							(currentKeys[(int)ShortcutKeys.LEFT_CONTROL] && !preKeys[(int)ShortcutKeys.LEFT_CONTROL]) ||
							(currentKeys[(int)ShortcutKeys.RIGHT_CONTROL] && !preKeys[(int)ShortcutKeys.RIGHT_CONTROL]));
					}
				}

				if (Alt)
				{
					pressed = pressed && (currentKeys[(int)ShortcutKeys.LEFT_ALT] || currentKeys[(int)ShortcutKeys.RIGHT_ALT]);
					pushed = pushed || (
						(currentKeys[(int)ShortcutKeys.LEFT_ALT] && !preKeys[(int)ShortcutKeys.LEFT_ALT]) ||
						(currentKeys[(int)ShortcutKeys.RIGHT_ALT] && !preKeys[(int)ShortcutKeys.RIGHT_ALT]));
				}

				if (Shift)
				{
					pressed = pressed && (currentKeys[(int)ShortcutKeys.LEFT_SHIFT] || currentKeys[(int)ShortcutKeys.RIGHT_SHIFT]);
					pushed = pushed || (
						(currentKeys[(int)ShortcutKeys.LEFT_SHIFT] && !preKeys[(int)ShortcutKeys.LEFT_SHIFT]) ||
						(currentKeys[(int)ShortcutKeys.RIGHT_SHIFT] && !preKeys[(int)ShortcutKeys.RIGHT_SHIFT]));
				}

				return pressed && pushed;
			}

			public void SetFunction(Func<bool> function)
			{
				Function = function;
			}

			public string GetShortcutText()
			{
				if (Function == null) return string.Empty;
				if (Key == 0) return string.Empty;

				string r = string.Empty;

				if (Control)
				{
					if (swig.GUIManager.IsMacOSX())
					{
						r += "Command+";
					}
					else
					{
						r += "Ctrl+";
					}
				}

				if (Shift)
				{
					r += "Shift+";
				}

				if (Alt)
				{
					if(swig.GUIManager.IsMacOSX())
					{
						r += "Option+";
					}
					else
					{
						r += "Alt+";
					}
				}

				r += ((ShortcutKeys)Key).ToString();

				return r;
			}
		}
	}
}
