using System.Collections.Generic;
using Effekseer.Data;
using Effekseer.Data.Value;
using Effekseer.Utl;

namespace Effekseer.Binary.RenderData
{
	internal sealed class TextureValuesAggregator
	{
		private readonly Data.RendererCommonValues _value;
		private readonly AdvancedRenderCommonValues _advanceValue;
		private readonly TextureInformation _texInfo;
		private TextureInformation _alpha;
		private TextureInformation _uvDistortion;
		private TextureInformation _blend;
		private TextureInformation _blendAlpha;
		private TextureInformation _blendUvDistortion;
		private readonly List<byte[]> _data = new List<byte[]>();

		public TextureValuesAggregator(
			Data.RendererCommonValues value,
			AdvancedRenderCommonValues advanceValue,
			TextureInformationRepository repo)
		{
			_value = value;
			_advanceValue = advanceValue;
			_texInfo = repo.Texture;
			_alpha = repo.Alpha;
			_uvDistortion = repo.UvDistortion;
			_blend = repo.Blend;
			_blendAlpha = repo.BlendAlpha;
			_blendUvDistortion = repo.BlendUvDistortion;
		}

		public IEnumerable<byte[]> CurrentData => _data;
		public bool IsBlendTextureEnabled => _advanceValue.BlendTextureParams.Enabled;

		public void AddInt(int value)
		{
			_data.Add(value.GetBytes());
		}

		public void AddFloat(float value)
		{
			_data.Add(value.GetBytes());
		}

		public void AddTexIdAndStoreSize(PathForImage image, int number, SortedDictionary<string, int> texAndInd)
		{
			var tempTexInfo = new TextureInformation();

			if (!texAndInd.ContainsKey(image.RelativePath) || !tempTexInfo.Load(image.AbsolutePath))
			{
				AddInt(-1);
				return;
			}

			if (_value.UVTextureReferenceTarget.Value != Data.UVTextureReferenceTargetType.None
				&& number == (int)_value.UVTextureReferenceTarget.Value)
			{
				_texInfo.Load(image.AbsolutePath);
			}

			AddInt(texAndInd[image.RelativePath]);
		}

		private int GetTexIdAndInfo(PathForImage image, SortedDictionary<string, int> texAndInd, ref TextureInformation texInfoRef)
		{
			var tempTexInfo = new TextureInformation();

			if (!texAndInd.ContainsKey(image.RelativePath) || !tempTexInfo.Load(image.AbsolutePath))
			{
				return -1;
			}

			texInfoRef.Load(image.AbsolutePath);
			return texAndInd[image.RelativePath];
		}

		private void AddTexIdAndInfo(bool isEnabled, PathForImage path, SortedDictionary<string, int> texAndInd, ref TextureInformation textureInfo)
		{
			var value = isEnabled
				? GetTexIdAndInfo(path, texAndInd, ref textureInfo)
				: -1;
			AddInt(value);
		}

		public void AddAlphaTexture(SortedDictionary<string, int> texAndInd)
		{
			AddTexIdAndInfo(_advanceValue.AlphaTextureParam.Enabled,
				_advanceValue.AlphaTextureParam.Texture,
				texAndInd,
				ref _alpha);
		}

		public void AddUvDistortionTexture(SortedDictionary<string, int> texAndInd)
		{
			AddTexIdAndInfo(_advanceValue.UVDistortionTextureParam.Enabled,
				_advanceValue.UVDistortionTextureParam.Texture,
				texAndInd,
				ref _uvDistortion);
		}

		public void AddBlendAlphaTexture(SortedDictionary<string, int> texAndInd)
		{
			AddTexIdAndInfo(_advanceValue.BlendTextureParams.EnableBlendAlphaTexture,
				_advanceValue.BlendTextureParams.BlendAlphaTextureParam.Texture,
				texAndInd,
				ref _blendAlpha);
		}

		public void AddBlendUvDistortionTexture(SortedDictionary<string, int> texAndInd)
		{
			AddTexIdAndInfo(_advanceValue.BlendTextureParams.EnableBlendUVDistortionTexture,
				_advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.Texture,
				texAndInd,
				ref _blendUvDistortion);
		}

		public void AddBlendTexture(SortedDictionary<string, int> texAndInd)
		{
			var value = GetTexIdAndInfo(_advanceValue.BlendTextureParams.BlendTextureParam.Texture,
				texAndInd,
				ref _blend);
			AddInt(value);
		}
	}
}
