using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;

namespace Effekseer.GUI.Widgets
{
	class WidgetHelper
	{
		public static bool CanShowTip()
		{
			return Manager.NativeManager.IsItemHovered() && Manager.NativeManager.GetHoveredIDTimer() > 0.25f;
		}

		public static bool ShowResetButton(string id)
		{
			var ret = false;

			if (Manager.NativeManager.Button(Resources.GetString("ResetParam_Name") + id))
			{
				ret = true;
			}

			if (CanShowTip())
			{
				Manager.NativeManager.SetTooltip(Resources.GetString("ResetParam_Desc"));
			}

			return ret;
		}
	}

	public class CopyAndPaste<T>
	{
		Func<T> getter;
		Action<T> setter;

		public CopyAndPaste(Func<T> getter, Action<T> setter)
		{
			this.getter = getter;
			this.setter = setter;
		}

		public void Update()
		{
			float dpiScale = Manager.DpiScale;
			swig.Vec2 size = new swig.Vec2(18 * dpiScale, 18 * dpiScale);

			if (!Images.Icons.ContainsKey("Copy"))
			{
				ErrorUtils.ThrowFileNotfound();
			}

			if (Manager.NativeManager.ImageButton(Images.Icons["Copy"], size.X, size.Y))
			{
				var o = getter();
				if (o != null)
				{
					var data = PartsTreeSystem.JsonSerializer.Serialize(o, Effekseer.CoreContext.Environment);
					Manager.NativeManager.SetClipboardText(data);
				}
			}

			if (WidgetHelper.CanShowTip())
			{
				Manager.NativeManager.SetTooltip(Resources.GetString("Panel_Copy_Desc"));
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.ImageButton(Images.Icons["Paste"], size.X, size.Y))
			{
				var str = Manager.NativeManager.GetClipboardText();

				if (!string.IsNullOrEmpty(str))
				{
					var o = getter();
					if (o != null)
					{
						var data = PartsTreeSystem.JsonSerializer.Deserialize<T>(str, Effekseer.CoreContext.Environment);
						setter?.Invoke(data);
					}
				}
			}

			if (WidgetHelper.CanShowTip())
			{
				Manager.NativeManager.SetTooltip(Resources.GetString("Panel_Paste_Desc"));
			}
		}
	}
}