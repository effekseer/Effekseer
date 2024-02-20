using System;
using System.Collections.Generic;
using System.Linq;

namespace Effekseer.GUI.BindableComponent
{
	class PathForImage : PathBase
	{
		string infoText = string.Empty;

		public PathForImage()
		{
			fileType = FileType.Texture;
			filter = MultiLanguageTextProvider.GetText("ImageFilter");
		}

		protected override void UpdateSubParts(float width)
		{
			Manager.NativeManager.SetNextItemWidth(width);
			Manager.NativeManager.InputText(id2, infoText, swig.InputTextFlags.ReadOnly);
		}

		protected override void UpdateInfo()
		{
			base.UpdateInfo();

			if (thumbnail != null)
			{
				var image = thumbnail.Image;
				infoText = "" + image.GetWidth() + "x" + image.GetHeight();
			}
			else
			{
				infoText = "";
			}
		}
	}
}