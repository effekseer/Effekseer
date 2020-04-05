using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace Effekseer.Binary
{
	enum NodeType : int
	{ 
		Root = -1,
		None = 0,
		Sprite = 2,
		Ribbon = 3,
        Ring = 4,
		Model = 5,
		Track = 6,
	};

	[StructLayout(LayoutKind.Sequential)]
	struct Vector3D
	{
		float X;
		float Y;
		float Z;

		public Vector3D(float x, float y, float z)
		{
			X = x;
			Y = y;
			Z = z;
		}
	}

	class Utils
	{
		public static void LogFileNotFound(string path)
		{
			if (Core.OnOutputLog != null)
			{
				Language language = Language.English;

				if(Core.Option != null && Core.Option.LanguageSelector != null)
				{
					language = Core.Language;
				}
				else
				{
					language = LanguageGetter.GetLanguage();
				}

				if(language == Language.Japanese)
				{
					Core.OnOutputLog(LogLevel.Warning, path + " が見つかりません。");
				}
				else
				{
					Core.OnOutputLog(LogLevel.Warning, path + " is not found.");
				}
			}
		}
	}
}
