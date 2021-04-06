using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class SoundValues
	{
		[Selector(ID = 0)]
		public Value.Enum<ParamaterType> Type
		{
			get;
			private set;
		}

        [Selected(ID = 0, Value = 0)]
        [IO(Export = true)]
        public NoneParamater None
        {
            get;
            private set;
        }

		[Selected(ID = 0, Value = 1)]
		[IO(Export = true)]
		public SoundParamater Sound
		{
			get;
			private set;
		}

		internal SoundValues(Value.Path basepath)
		{
            Type = new Value.Enum<ParamaterType>(ParamaterType.None);
            None = new NoneParamater();
            Sound = new SoundParamater(basepath);
		}

        public class NoneParamater
        {
            internal NoneParamater()
            {
            }
        }

        public class SoundParamater
        {
			[Key(key = "Sound_Wave")]
			public Value.PathForSound Wave
            {
                get;
                private set;
            }

			[Key(key = "Sound_Volume")]
			public Value.FloatWithRandom Volume
            {
                get;
                private set;
            }

			[Key(key = "Sound_Pitch")]
			public Value.FloatWithRandom Pitch
            {
                get;
                private set;
            }

            [Selector(ID = 0)]
			[Key(key = "Sound_PanType")]
			public Value.Enum<ParamaterPanType> PanType
            {
                get;
                private set;
            }

            [Selected(ID = 0, Value = 0)]
			[Key(key = "Sound_Pan")]
			public Value.FloatWithRandom Pan
            {
                get;
                private set;
            }

            [Selected(ID = 0, Value = 1)]
			[Key(key = "Sound_Distance")]
			public Value.Float Distance
            {
                get;
                private set;
            }

			[Key(key = "Sound_Delay")]
			public Value.IntWithRandom Delay
            {
                get;
                private set;
            }

            internal SoundParamater(Value.Path basepath)
			{
				Wave = new Value.PathForSound(basepath, Resources.GetString("SoundFilter"), true, "");
                Volume = new Value.FloatWithRandom(1.0f, 1.0f, 0.0f, DrawnAs.CenterAndAmplitude, 0.1f);
                Pitch = new Value.FloatWithRandom(0.0f, 2.0f, -2.0f, DrawnAs.CenterAndAmplitude, 0.1f);
                PanType = new Value.Enum<ParamaterPanType>(ParamaterPanType.Sound2D);
                Pan = new Value.FloatWithRandom(0.0f, 1.0f, -1.0f, DrawnAs.CenterAndAmplitude, 0.1f);
                Distance = new Value.Float(10.0f);
                Delay = new Value.IntWithRandom(0, int.MaxValue, 0);
			}
        }

        public enum ParamaterPanType : int
        {
			[Key(key = "Sound_PanType_Sound2D")]
			Sound2D = 0,
			[Key(key = "Sound_PanType_Sound3D")]
			Sound3D = 1,
        }

        public enum ParamaterType : int
        {
			[Key(key = "Sound_ParamaterType_None")]
			None = 0,
			[Key(key = "Sound_ParamaterType_Use")]
			Use = 1,
        }
	}
}
