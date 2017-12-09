using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
    public class DepthValues
    {

        [Name(language = Language.Japanese, value = "Zオフセット")]
        [Description(language = Language.Japanese, value = "描画時に奥行方向に移動されるオフセット")]
        [Name(language = Language.English, value = "Z-Offset")]
        [Description(language = Language.English, value = "An offset which shift Z direction when the particle is rendered.")]
        public Value.Float DepthOffset
        {
            get;
            private set;
        }

		[Name(language = Language.Japanese, value = "Zオフセットによる拡大無効化")]
		[Description(language = Language.Japanese, value = "Zオフセットにより大きさが変化しないようにするか")]
		[Name(language = Language.English, value = "Is scaling invalid with Z offset")]
		[Description(language = Language.English, value = "Whether Scaling is not changed with Z offset")]
		public Value.Boolean IsScaleChangedDependingOnDepthOffset
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "Zオフセットの拡大")]
		[Description(language = Language.Japanese, value = "Zオフセットがパーティクルの拡大で大きくなるか")]
		[Name(language = Language.English, value = "Scaling Z-Offset")]
		[Description(language = Language.English, value = "Whether Z offset is enlarged with a scaling of particles")]
		public Value.Boolean IsDepthOffsetChangedDependingOnParticleScale
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "ソフトパーティクル")]
		[Description(language = Language.Japanese, value = "ソフトパーティクル")]
		[Name(language = Language.English, value = "Soft particle")]
		[Description(language = Language.English, value = "Soft particle")]
		public Value.Float SoftParticle
		{
			get;
			private set;
		}

        public DepthValues()
        {
            DepthOffset = new Value.Float();
			IsScaleChangedDependingOnDepthOffset = new Value.Boolean();
			IsDepthOffsetChangedDependingOnParticleScale = new Value.Boolean();
			SoftParticle = new Value.Float(0, float.MaxValue, 0.0f);
        }
    }
}
