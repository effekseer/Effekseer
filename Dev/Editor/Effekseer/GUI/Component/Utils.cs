using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class Functions
	{
		public static string GetFilepathWithExtentions(string filepath, string extention)
		{
			if (System.IO.Path.GetExtension(filepath) != extention)
			{
				filepath += extention;
			}

			return filepath;
		}
	}
	class CopyAndPaste : IControl
	{
		string elementName = string.Empty;
		Func<object> getter;
		
		public CopyAndPaste(string elementName, Func<object> getter)
		{
			this.elementName = elementName;
			this.getter = getter;
		}

		public void Update()
		{
			if (Manager.NativeManager.Button("Copy"))
			{
				var o = getter();
				if(o != null)
				{
					Manager.NativeManager.SetClipboardText(Core.Copy(elementName, o));
				}
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button("Paste"))
			{
				var str = Manager.NativeManager.GetClipboardText();

				if (!string.IsNullOrEmpty(str))
				{
					var o = getter();
					if (o != null)
					{
						Core.Paste(elementName, o, str);
					}
				}
			}
		}
	}
	class ValueChangingProperty : IDisposable
	{
		bool flag = false;

		public void Enable(dynamic value)
		{
			if (Manager.DoesChangeColorOnChangedValue && (value?.IsValueChangedFromDefault ?? false))
			{
				flag = true;
				Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.Border, 0x77ffff11);
				Manager.NativeManager.PushStyleVar(swig.ImGuiStyleVarFlags.FrameBorderSize, 1);
			}
		}

		public void Disable()
		{
			if (flag)
			{
				Manager.NativeManager.PopStyleColor();
				Manager.NativeManager.PopStyleVar();
			}
			flag = false;
		}

		public void Dispose()
		{
			Disable();
		}
	}

	class DynamicEquationCodeTooltip
	{
		public static void Render(Data.Value.DynamicEquationReference dynamicEquation)
		{
			if (dynamicEquation.Index != -1)
			{
				Manager.NativeManager.BeginTooltip();
				Manager.NativeManager.Text(dynamicEquation.Value.Code.Value);
				Manager.NativeManager.EndTooltip();
			}
		}
 	}
}
