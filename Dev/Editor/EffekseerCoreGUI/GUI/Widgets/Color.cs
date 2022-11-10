using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;

namespace Effekseer.GUI.Widgets
{
	class Color
	{
		public static Inspector.InspectorGuiResult Update(object value, Inspector.InspectorGuiState state)
		{
			var ret = new Inspector.InspectorGuiResult();

			if (value is EffectAsset.Color color)
			{
				var colorSpace = (color.ColorSpace == Data.ColorSpace.RGBA)
				? (swig.ColorEditFlags.DisplayRGB | swig.ColorEditFlags.InputRGB)
				: (swig.ColorEditFlags.DisplayHSV | swig.ColorEditFlags.InputHSV);

				float[] internalValue = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
				internalValue[0] = color.V1 / 255.0f;
				internalValue[1] = color.V2 / 255.0f;
				internalValue[2] = color.V3 / 255.0f;
				internalValue[3] = color.A / 255.0f;

				if (Manager.NativeManager.ColorEdit4(state.Id, internalValue, colorSpace))
				{
					EffectAsset.Color newColor = new EffectAsset.Color();
					newColor.V1 = (int)Math.Round(internalValue[0] * 255, MidpointRounding.AwayFromZero);
					newColor.V2 = (int)Math.Round(internalValue[1] * 255, MidpointRounding.AwayFromZero);
					newColor.V3 = (int)Math.Round(internalValue[2] * 255, MidpointRounding.AwayFromZero);
					newColor.A = (int)Math.Round(internalValue[3] * 255, MidpointRounding.AwayFromZero);
					newColor.ColorSpace = color.ColorSpace;

					if (color != newColor)
					{
						ret.value = newColor;
						ret.isEdited = true;
					}
				}
			}

			return ret;
		}
	}
}