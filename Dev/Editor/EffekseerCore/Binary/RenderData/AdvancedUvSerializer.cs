using Effekseer.Data.Group;
using Effekseer.Utl;

namespace Effekseer.Binary.RenderData
{
	internal sealed class AdvancedUvSerializer : UvSerializer
	{
		public AdvancedUvSerializer(IUvCommandValues values)
			: base(values.UV, values.UVFixed, values.UVAnimation, values.UVScroll, values.UVFCurve)
		{
		}

		protected override void GetSize(TextureInformation texInfo, out float width, out float height)
		{
			// specification change(1.5)
			width = 128.0f;
			height = 128.0f;

			if (texInfo.Width > 0 && texInfo.Height > 0)
			{
				width = texInfo.Width;
				height = texInfo.Height;
			}
		}
	}
}