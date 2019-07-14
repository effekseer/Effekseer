using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum ZSortType
	{
		[Name(value = "なし", language = Language.Japanese)]
		[Name(value = "None", language = Language.English)]
		None,

		[Name(value = "正順", language = Language.Japanese)]
		[Name(value = "Normal order", language = Language.English)]
		NormalOrder,

		[Name(value = "逆順", language = Language.Japanese)]
		[Name(value = "Reverse order", language = Language.English)]
		ReverseOrder,

	}


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
		[Name(language = Language.English, value = "Is scaling invalid \nwith Z offset")]
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

		[Name(language = Language.Japanese, value = "深度ソート")]
		[Description(language = Language.Japanese, value = "距離による並び替え")]
		[Name(language = Language.English, value = "Depth sort")]
		[Description(language = Language.English, value = "Sorting by a distance")]
		public Value.Enum<ZSortType> ZSort
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "深度クリップ")]
		[Description(language = Language.Japanese, value = "指定された値より遠いパーティクルは描画されない。")]
		[Name(language = Language.English, value = "DepthClipping")]
		[Description(language = Language.English, value = "Particles farther than the specified value are not rendered.")]
		public Value.IntWithInifinite DepthClipping
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "深度によるスケール変化の抑制")]
		[Description(language = Language.Japanese, value = "深度が変化したとしても大きさが変わらないようにする。\n1.0だとこのパラメーターを使用せず、0.0だと変化がなくなる。")]
		[Name(language = Language.English, value = "Suppression of scaling by depth")]
		[Description(language = Language.English, value = "It makes scaling not to change even if a depth is changed.\nIf it is 1.0, it does not use this parameter, and if it is 0.0, there is no scaling.")]
		public Value.Float SuppressionOfScalingByDepth
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "描画優先度")]
		[Description(language = Language.Japanese, value = "小さい描画優先度のノードが先に描画される")]
		[Name(language = Language.English, value = "Drawing priority")]
		[Description(language = Language.English, value = "A node with Small drawing priority is drawn previously")]
		public Value.Int DrawingPriority
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
			ZSort = new Value.Enum<ZSortType>(ZSortType.None);
			DrawingPriority = new Value.Int(0, 255, -255);
			SoftParticle = new Value.Float(0, float.MaxValue, 0.0f);
			DepthClipping = new Value.IntWithInifinite(1024, true, int.MaxValue, 16);
			SuppressionOfScalingByDepth = new Value.Float(1.0f, 1.0f, 0.0f, 0.1f);
        }
    }
}
