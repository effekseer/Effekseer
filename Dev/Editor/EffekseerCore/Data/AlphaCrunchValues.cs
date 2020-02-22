using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
    public class AlphaCrunchValues
    {
        [Selector(ID = 0)]
        [IO(Export = true)]
        public Value.Enum<ParameterType> Type { get; private set; }

        [Selected(ID = 0, Value = 0)]
        [IO(Export = true)]
        public FixedParameter Fixed { get; private set; }

        [Selected(ID = 0, Value = 1)]
        [IO(Export = true)]
        public FourPointInterpolationParameter FourPointInterpolation { get; private set; }

        [Selected(ID = 0, Value = 2)]
        [IO(Export = true)]
        public FloatEasingParamater Easing { get; private set; }

        [Selected(ID = 0, Value = 3)]
        [Name(language = Language.Japanese, value = "Fカーブ")]
        [IO(Export = true)]
        public Value.FCurveScalar FCurve { get; private set; }

        public AlphaCrunchValues()
        {
            Type = new Value.Enum<ParameterType>(ParameterType.Fixed);
            Fixed = new FixedParameter();
            FourPointInterpolation = new FourPointInterpolationParameter();
            Easing = new FloatEasingParamater(0.0f, 1.0f, 0.0f);
            FCurve = new Value.FCurveScalar(0.0f, 100.0f);

            Fixed.Threshold.CanSelectDynamicEquation = true;
            Easing.Start.CanSelectDynamicEquation = true;
            Easing.End.CanSelectDynamicEquation = true;
        }

        public class FixedParameter
        {
            [Name(language = Language.Japanese, value = "アルファ閾値")]
            [Name(language = Language.English, value = "Alpha Threshold")]
            public Value.Float Threshold { get; private set; }

            internal FixedParameter()
            {
                Threshold = new Value.Float(0.0f, 1.0f, 0.0f, 0.05f);
            }
        }

        public class FourPointInterpolationParameter
        {
            [Name(language = Language.Japanese, value = "生成時アルファ閾値")]
            [Name(language = Language.English, value = "Begin Alpha Threshold")]
            public Value.FloatWithRandom BeginThreshold { get; private set; }

            [Name(language = Language.Japanese, value = "遷移フレーム(生成時 -> 第2)")]
            [Name(language = Language.English, value = "Sequence Frame Num")]
            public Value.IntWithRandom TransitionFrameNum { get; private set; }

            [Name(language = Language.Japanese, value = "第2アルファ閾値")]
            [Name(language = Language.English, value = "Second Alpha Threshold")]
            public Value.FloatWithRandom No2Threshold { get; private set; }

            [Name(language = Language.Japanese, value = "第3アルファ閾値")]
            [Name(language = Language.English, value = "Third Alpha Threshold")]
            public Value.FloatWithRandom No3Threshold { get; private set; }

            [Name(language = Language.Japanese, value = "遷移フレーム(第3 -> 消滅時)")]
            [Name(language = Language.English, value = "Sequence Frame Num")]
            public Value.IntWithRandom TransitionFrameNum2 { get; private set; }

            [Name(language = Language.Japanese, value = "消滅時アルファ閾値")]
            [Name(language = Language.English, value = "End Alpha Threshold")]
            public Value.FloatWithRandom EndThreshold { get; private set; }


            internal FourPointInterpolationParameter()
            {
                BeginThreshold = new Value.FloatWithRandom(0.0f, 1.0f, 0.0f, DrawnAs.CenterAndAmplitude, 0.05f);
                TransitionFrameNum = new Value.IntWithRandom(0, int.MaxValue, 0);
                No2Threshold = new Value.FloatWithRandom(0.0f, 1.0f, 0.0f, DrawnAs.CenterAndAmplitude, 0.05f);
                No3Threshold = new Value.FloatWithRandom(0.0f, 1.0f, 0.0f, DrawnAs.CenterAndAmplitude, 0.05f);
                TransitionFrameNum2 = new Value.IntWithRandom(0, int.MaxValue, 0);
                EndThreshold = new Value.FloatWithRandom(0.0f, 1.0f, 0.0f, DrawnAs.CenterAndAmplitude, 0.05f);
            }
        }

        public enum ParameterType : int
        {
            [Name(value = "アルファ閾値", language = Language.Japanese)]
            [Name(value = "Set Alpha Threshold", language = Language.English)]
            Fixed = 0,

            [Name(value = "4点補間", language = Language.Japanese)]
            [Name(value = "Four Point Interpolation", language = Language.English)]
            FourPointInterpolation = 1,

            [Name(value = "イージング", language = Language.Japanese)]
            [Name(value = "Easing", language = Language.English)]
            Easing = 2,

            [Name(value = "アルファ閾値(Fカーブ)", language = Language.Japanese)]
            [Name(value = "F-Curve", language = Language.English)]
            FCurve = 3,
        }
    }
}
