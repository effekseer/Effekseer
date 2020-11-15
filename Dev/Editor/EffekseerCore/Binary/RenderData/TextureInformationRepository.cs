using Effekseer.Utl;

namespace Effekseer.Binary.RenderData
{
	internal sealed class TextureInformationRepository
	{
		public TextureInformation Texture { get; } = new TextureInformation();
		public TextureInformation Alpha { get; } = new TextureInformation();
		public TextureInformation UvDistortion { get; } = new TextureInformation();
		public TextureInformation Blend { get; } = new TextureInformation();
		public TextureInformation BlendAlpha { get; } = new TextureInformation();
		public TextureInformation BlendUvDistortion { get; } = new TextureInformation();
	}
}