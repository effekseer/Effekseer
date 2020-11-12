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
		private readonly AdvancedRenderCommonValues2 _advanceValue2;
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
			AdvancedRenderCommonValues2 advanceValue2,
			TextureInformation texInfo,
			TextureInformation alpha,
			TextureInformation uvDistortion,
			TextureInformation blend,
			TextureInformation blendAlpha,
			TextureInformation blendUvDistortion)
		{
			_value = value;
			_advanceValue = advanceValue;
			_advanceValue2 = advanceValue2;
			_texInfo = texInfo;
			_alpha = alpha;
			_uvDistortion = uvDistortion;
			_blend = blend;
			_blendAlpha = blendAlpha;
			_blendUvDistortion = blendUvDistortion;
		}

		public IEnumerable<byte[]> CurrentData => _data;
		public bool IsBlendTextureEnabled => _advanceValue2.EnableBlendTexture;

		public void AddInt(int value)
		{
			_data.Add(value.GetBytes());
		}

		public void AddFloat(float value)
		{
			_data.Add(value.GetBytes());
		}

		public void AddTexIdAndStoreSize(PathForImage image, int number, Dictionary<string, int> texAndInd)
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

		private int GetTexIdAndInfo(PathForImage image, Dictionary<string, int> texAndInd, ref TextureInformation texInfoRef)
		{
			var tempTexInfo = new TextureInformation();

			if (!texAndInd.ContainsKey(image.RelativePath) || !tempTexInfo.Load(image.AbsolutePath))
			{
				return -1;
			}

			texInfoRef.Load(image.AbsolutePath);
			return texAndInd[image.RelativePath];
		}

		private void AddTexIdAndInfo(bool isEnabled, PathForImage path, Dictionary<string, int> texAndInd, ref TextureInformation textureInfo)
		{
			var value = isEnabled
				? GetTexIdAndInfo(path, texAndInd, ref textureInfo)
				: -1;
			AddInt(value);
		}

		public void AddAlphaTexture(Dictionary<string, int> texAndInd)
		{
			AddTexIdAndInfo(_advanceValue.EnableAlphaTexture,
				_advanceValue.AlphaTextureParam.Texture,
				texAndInd,
				ref _alpha);
		}

		public void AddUvDistortionTexture(Dictionary<string, int> texAndInd)
		{
			AddTexIdAndInfo(_advanceValue.EnableUVDistortionTexture,
				_advanceValue.UVDistortionTextureParam.Texture,
				texAndInd,
				ref _uvDistortion);
		}

		public void AddBlendAlphaTexture(Dictionary<string, int> texAndInd)
		{
			AddTexIdAndInfo(_advanceValue2.BlendTextureParams.EnableBlendAlphaTexture,
				_advanceValue2.BlendTextureParams.BlendAlphaTextureParam.Texture,
				texAndInd,
				ref _blendAlpha);
		}

		public void AddBlendUvDistortionTexture(Dictionary<string, int> texAndInd)
		{
			AddTexIdAndInfo(_advanceValue2.BlendTextureParams.EnableBlendUVDistortionTexture,
				_advanceValue2.BlendTextureParams.BlendUVDistortionTextureParam.Texture,
				texAndInd,
				ref _blendUvDistortion);
		}

		public void AddBlendTexture(Dictionary<string, int> texAndInd)
		{
			GetTexIdAndInfo(_advanceValue2.BlendTextureParams.BlendTextureParam.Texture,
				texAndInd,
				ref _blend);
		}
	}
}
