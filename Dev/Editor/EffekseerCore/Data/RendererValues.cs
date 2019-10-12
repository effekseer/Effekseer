using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum TextureUVType
	{
		[Name(value = "Strech", language = Language.English)]
		[Name(value = "ストレッチ", language = Language.Japanese)]
		Strech = 0,
		[Name(value = "Tile", language = Language.English)]
		[Name(value = "タイル", language = Language.Japanese)]
		Tile = 1,
	}

	public class TextureUVTypeParameter
	{
		[Name(value = "UV Type", language = Language.English)]
		[Name(value = "UV タイプ", language = Language.Japanese)]
		[Selector(ID = 0)]
		public Value.Enum<TextureUVType> Type
		{
			get;
			private set;
		}

		[Name(value = "The number of tile on Head", language = Language.English)]
		[Name(value = "頭のタイル数", language = Language.Japanese)]
		[Selected(ID = 0, Value = (int)TextureUVType.Tile)]
		public Value.Int TileEdgeHead { get; private set; }

		[Name(value = "The number of tile on Tail", language = Language.English)]
		[Name(value = "尻尾のタイル数", language = Language.Japanese)]
		[Selected(ID = 0, Value = (int)TextureUVType.Tile)]
		public Value.Int TileEdgeTail { get; private set; }

		[Name(value = "Looping area", language = Language.English)]
		[Name(value = "ループ領域", language = Language.Japanese)]
		[Selected(ID = 0, Value = (int)TextureUVType.Tile)]
		public Value.Vector2D TileLoopingArea { get; private set; }
		public TextureUVTypeParameter()
		{
			Type = new Value.Enum<TextureUVType>(TextureUVType.Strech);
			TileEdgeHead = new Value.Int(0, int.MaxValue, 0);
			TileEdgeTail = new Value.Int(0, int.MaxValue, 0);
			TileLoopingArea = new Value.Vector2D(0.0f, 1.0f);
		}
	}

	public enum RingShapeType
	{
		[Name(value = "ドーナツ", language = Language.Japanese)]
		[Name(value = "Donut", language = Language.English)]
		Donut = 0,

		[Name(value = "三日月", language = Language.Japanese)]
		[Name(value = "Crescent", language = Language.English)]
		Crescent = 1,
	}

	public enum FixedRandomEasingType : int
	{
		[Name(value = "固定", language = Language.Japanese)]
		[Name(value = "Fixed", language = Language.English)]
		Fixed = 0,
		[Name(value = "ランダム", language = Language.Japanese)]
		[Name(value = "Random", language = Language.English)]
		Random = 1,
		[Name(value = "イージング", language = Language.Japanese)]
		[Name(value = "Easing", language = Language.English)]
		Easing = 2,
	}

	public class RingShapeCrescentParameter
	{
		[Name(language = Language.Japanese, value = "開始フェード(度)")]
		[Name(language = Language.English, value = "Starting fade(Deg.)")]
		public Value.Float StartingFade { get; private set; }
		[Name(language = Language.Japanese, value = "終了フェード(度)")]
		[Name(language = Language.English, value = "Ending fade(Deg.)")]
		public Value.Float EndingFade { get; private set; }

		[Selector(ID = 0)]
		[Name(language = Language.Japanese, value = "開始角度")]
		[Name(language = Language.English, value = "Starting Angle")]
		public Value.Enum<FixedRandomEasingType> StartingAngle { get; private set; }

		[Selected(ID = 0, Value = 0)]
		public Value.Float StartingAngle_Fixed { get; private set; }

		[Selected(ID = 0, Value = 1)]
		public Value.FloatWithRandom StartingAngle_Random { get; private set; }

		[Selected(ID = 0, Value = 2)]
		[IO(Export = true)]
		public FloatEasingParamater StartingAngle_Easing { get; private set; }

		[Selector(ID = 1)]
		[Name(language = Language.Japanese, value = "終了角度")]
		[Name(language = Language.English, value = "Ending Angle")]
		public Value.Enum<FixedRandomEasingType> EndingAngle { get; private set; }

		[Selected(ID = 1, Value = 0)]
		public Value.Float EndingAngle_Fixed { get; private set; }

		[Selected(ID = 1, Value = 1)]
		public Value.FloatWithRandom EndingAngle_Random { get; private set; }

		[Selected(ID = 1, Value = 2)]
		[IO(Export = true)]
		public FloatEasingParamater EndingAngle_Easing { get; private set; }

		public RingShapeCrescentParameter()
		{
			StartingFade = new Value.Float(0, 360, 0);
			EndingFade = new Value.Float(0, 360, 0);
			StartingAngle = new Value.Enum<FixedRandomEasingType>();
			StartingAngle_Fixed = new Value.Float(0);
			StartingAngle_Random = new Value.FloatWithRandom();
			StartingAngle_Easing = new FloatEasingParamater();

			EndingAngle = new Value.Enum<FixedRandomEasingType>();
			EndingAngle_Fixed = new Value.Float(360);
			EndingAngle_Random = new Value.FloatWithRandom(360);
			EndingAngle_Easing = new FloatEasingParamater(360);
		}
	}

	public class RingShapeParameter
	{
		[Name(language = Language.Japanese, value = "形状")]
		[Name(language = Language.English, value = "Shape")]
		[Selector(ID = 0)]
		public Value.Enum<RingShapeType> Type
		{
			get;
			private set;
		}

		[IO(Export = true)]
		[Selected(ID = 0, Value=(int)RingShapeType.Crescent)]
		public RingShapeCrescentParameter Crescent { get; private set; }

		public RingShapeParameter()
		{
			Type = new Value.Enum<RingShapeType>(RingShapeType.Donut);
			Crescent = new RingShapeCrescentParameter();
		}
	}

	public class RendererValues
	{
		[Selector(ID = 0)]
		public Value.Enum<ParamaterType> Type
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 3)]
		[Selected(ID = 0, Value = 6)]
		[IO(Export = true)]
		public TextureUVTypeParameter TextureUVType { get; private set; } 

		[Selected(ID = 0, Value = 2)]
		[IO(Export = true)]
		public SpriteParamater Sprite
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 3)]
		[IO(Export = true)]
		public RibbonParamater Ribbon
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 6)]
		[IO(Export = true)]
		public TrackParameter Track
		{
			get;
			private set;
		}

        [Selected(ID = 0, Value = 4)]
        [IO(Export = true)]
        public RingParamater Ring
        {
            get;
            private set;
        }

		[Selected(ID = 0, Value = 5)]
		[IO(Export = true)]
		public ModelParamater Model
		{
			get;
			private set;
		}

		internal RendererValues()
		{
			Type = new Value.Enum<ParamaterType>(ParamaterType.Sprite);
			TextureUVType = new TextureUVTypeParameter();

			Sprite = new SpriteParamater();
            Ribbon = new RibbonParamater();
			Track = new TrackParameter();
            Ring = new RingParamater();
			Model = new ModelParamater();
		}

		public class SpriteParamater
		{
			[Name(language = Language.Japanese, value = "描画順")]
			[Name(language = Language.English, value = "Rendering Order")]
			public Value.Enum<RenderingOrder> RenderingOrder { get; private set; }

			[Name(language = Language.Japanese, value = "配置方法")]
			[Name(language = Language.English, value = "Configuration")]
			public Value.Enum<BillboardType> Billboard { get; private set; }

			[Name(language = Language.Japanese, value = "ブレンド")]
			[Name(language = Language.English, value = "Blend")]
			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<AlphaBlendType> AlphaBlend { get; private set; }

			[Selector(ID = 0)]
			[Name(language = Language.Japanese, value = "全体色")]
			[Name(language = Language.English, value = "Color All")]
			public Value.Enum<StandardColorType> ColorAll { get; private set; }

			[Selected(ID = 0, Value = 0)]
			public Value.Color ColorAll_Fixed { get; private set; }

			[Selected(ID = 0, Value = 1)]
			public Value.ColorWithRandom ColorAll_Random { get; private set; }

			[Selected(ID = 0, Value = 2)]
			[IO(Export = true)]
			public ColorEasingParamater ColorAll_Easing { get; private set; }

			[Selected(ID = 0, Value = 3)]
			[IO(Export = true)]
			public ColorFCurveParameter ColorAll_FCurve { get; private set; }

			[Selector(ID = 1)]
			[Name(language = Language.Japanese, value = "頂点色")]
			[Name(language = Language.English, value = "Vertex Color")]
			public Value.Enum<ColorType> Color { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Name(language = Language.Japanese, value = "左下色")]
			[Name(language = Language.English, value = "Lower-Left Color")]
			public Value.Color Color_Fixed_LL { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Name(language = Language.Japanese, value = "右下色")]
			[Name(language = Language.English, value = "Lower-Right Color")]
			public Value.Color Color_Fixed_LR { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Name(language = Language.Japanese, value = "左上色")]
			[Name(language = Language.English, value = "Upper-Left Color")]
			public Value.Color Color_Fixed_UL { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Name(language = Language.Japanese, value = "右上色")]
			[Name(language = Language.English, value = "Upper-Right Color")]
			public Value.Color Color_Fixed_UR { get; private set; }

			[Selector(ID = 2)]
			[Name(language = Language.Japanese, value = "頂点座標")]
			[Name(language = Language.English, value = "Vertex Coords")]
			public Value.Enum<PositionType> Position { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Name(language = Language.Japanese, value = "左下座標")]
			[Name(language = Language.English, value = "Lower-Left Coord")]
			public Value.Vector2D Position_Fixed_LL { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Name(language = Language.Japanese, value = "右下座標")]
			[Name(language = Language.English, value = "Lower-Right Coord")]
			public Value.Vector2D Position_Fixed_LR { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Name(language = Language.Japanese, value = "左上座標")]
			[Name(language = Language.English, value = "Upper-Left Coord")]
			public Value.Vector2D Position_Fixed_UL { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Name(language = Language.Japanese, value = "右上座標")]
			[Name(language = Language.English, value = "Upper-Right Coord")]
			public Value.Vector2D Position_Fixed_UR { get; private set; }

			[Name(language = Language.Japanese, value = "カラー画像")]
			[Description(language = Language.Japanese, value = "スプライトの色を表す画像")]
			[Name(language = Language.English, value = "Color Image")]
			[Description(language = Language.English, value = "Image representing the color of the sprite")]
			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Path ColorTexture
			{
				get;
				private set;
			}

			public SpriteParamater()
			{
				RenderingOrder = new Value.Enum<Data.RenderingOrder>(Data.RenderingOrder.FirstCreatedInstanceIsFirst);

				Billboard = new Value.Enum<BillboardType>(BillboardType.Billboard);
				AlphaBlend = new Value.Enum<AlphaBlendType>(AlphaBlendType.Blend);

				ColorAll = new Value.Enum<StandardColorType>(StandardColorType.Fixed);
				ColorAll_Fixed = new Value.Color(255, 255, 255, 255);
				ColorAll_Random = new Value.ColorWithRandom(255, 255, 255, 255);
				ColorAll_Easing = new ColorEasingParamater();
				ColorAll_FCurve = new ColorFCurveParameter();

				Color = new Value.Enum<ColorType>(ColorType.Default);
				Color_Fixed_LL = new Value.Color(255, 255, 255, 255);
				Color_Fixed_LR = new Value.Color(255, 255, 255, 255);
				Color_Fixed_UL = new Value.Color(255, 255, 255, 255);
				Color_Fixed_UR = new Value.Color(255, 255, 255, 255);
				

				Position = new Value.Enum<PositionType>(PositionType.Default);
				Position_Fixed_LL = new Value.Vector2D(-0.5f, -0.5f);
				Position_Fixed_LR = new Value.Vector2D(0.5f, -0.5f);
				Position_Fixed_UL = new Value.Vector2D(-0.5f, 0.5f);
				Position_Fixed_UR = new Value.Vector2D(0.5f, 0.5f);
				ColorTexture = new Value.Path("画像ファイル (*.png)|*.png", true, "");
			}

            public enum ColorType : int
            {
                [Name(value = "標準", language = Language.Japanese)]
				[Name(value = "Default", language = Language.English)]
                Default = 0,
                [Name(value = "固定", language = Language.Japanese)]
				[Name(value = "Fixed", language = Language.English)]
                Fixed = 1,
            }

            public enum PositionType : int
            {
                [Name(value = "標準", language = Language.Japanese)]
				[Name(value = "Default", language = Language.English)]
                Default = 0,
                [Name(value = "固定", language = Language.Japanese)]
				[Name(value = "Fixed", language = Language.English)]
                Fixed = 1,
            }
		}

		public class RibbonParamater
		{
			[Name(language = Language.Japanese, value = "ブレンド")]
			[Name(language = Language.English, value = "Blend")]
			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<AlphaBlendType> AlphaBlend { get; private set; }

			[Name(language = Language.Japanese, value = "視点追従")]
			[Name(language = Language.English, value = "Follow Viewpoint")]
			public Value.Boolean ViewpointDependent { get; private set; }

			[Selector(ID = 0)]
			[Name(language = Language.Japanese, value = "全体色")]
			[Name(language = Language.English, value = "Color All")]
			public Value.Enum<ColorAllType> ColorAll { get; private set; }

			[Selected(ID = 0, Value = 0)]
			public Value.Color ColorAll_Fixed { get; private set; }

			[Selected(ID = 0, Value = 1)]
			public Value.ColorWithRandom ColorAll_Random { get; private set; }

			[Selected(ID = 0, Value = 2)]
			[IO(Export = true)]
			public ColorEasingParamater ColorAll_Easing { get; private set; }

			[Selector(ID = 1)]
			[Name(language = Language.Japanese, value = "頂点色")]
			[Name(language = Language.English, value = "Vertex Color")]
			public Value.Enum<ColorType> Color { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Name(language = Language.Japanese, value = "左頂点色")]
			[Name(language = Language.English, value = "Left Vertex Color")]
			public Value.Color Color_Fixed_L { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Name(language = Language.Japanese, value = "右頂点色")]
			[Name(language = Language.English, value = "Right Vertex Color")]
			public Value.Color Color_Fixed_R { get; private set; }

			[Selector(ID = 2)]
			[Name(language = Language.Japanese, value = "座標")]
			[Name(language = Language.English, value = "Position")]
			public Value.Enum<PositionType> Position { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Name(language = Language.Japanese, value = "左座標")]
			[Name(language = Language.English, value = "Left Coord")]
			public Value.Float Position_Fixed_L { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Name(language = Language.Japanese, value = "右座標")]
			[Name(language = Language.English, value = "Right Coord")]
			public Value.Float Position_Fixed_R { get; private set; }

			[Name(language = Language.Japanese, value = "スプラインの分割数")]
			[Description(language = Language.Japanese, value = "スプラインの分割数")]
			[Name(language = Language.English, value = "The number of \nspline division")]
			[Description(language = Language.English, value = "The number of spline division")]
			public Value.Int SplineDivision
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "色画像")]
			[Description(language = Language.Japanese, value = "リボンの色を表す画像")]

			[Name(language = Language.English, value = "Color Image")]
			[Description(language = Language.English, value = "Image representing the color of the ribbon")]
			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Path ColorTexture
			{
				get;
				private set;
			}

			public RibbonParamater()
			{
				AlphaBlend = new Value.Enum<AlphaBlendType>(AlphaBlendType.Blend);
				ViewpointDependent = new Value.Boolean(false);
				ColorAll = new Value.Enum<ColorAllType>(ColorAllType.Fixed);
				ColorAll_Fixed = new Value.Color(255, 255, 255, 255);
				ColorAll_Random = new Value.ColorWithRandom(255, 255, 255, 255);
				ColorAll_Easing = new ColorEasingParamater();

				Color = new Value.Enum<ColorType>(ColorType.Default);
				Color_Fixed_L = new Value.Color(255, 255, 255, 255);
				Color_Fixed_R = new Value.Color(255, 255, 255, 255);

				Position = new Value.Enum<PositionType>(PositionType.Default);
				Position_Fixed_L = new Value.Float(-0.5f);
				Position_Fixed_R = new Value.Float(0.5f);

				SplineDivision = new Value.Int(1, int.MaxValue, 1);

				ColorTexture = new Value.Path(Resources.GetString("ImageFilter"), true, "");
			}

            public enum ColorAllType : int
            {
                [Name(value = "固定", language = Language.Japanese)]
				[Name(value = "Fixed", language = Language.English)]
                Fixed = 0,
                [Name(value = "ランダム", language = Language.Japanese)]
				[Name(value = "Random", language = Language.English)]
                Random = 1,
                [Name(value = "イージング", language = Language.Japanese)]
				[Name(value = "Easing", language = Language.English)]
                Easing = 2,
            }

            public enum ColorType : int
            {
                [Name(value = "標準", language = Language.Japanese)]
				[Name(value = "Default", language = Language.English)]
                Default = 0,
                [Name(value = "固定", language = Language.Japanese)]
				[Name(value = "Fixed", language = Language.English)]
                Fixed = 1,
            }

            public enum PositionType : int
            {
                [Name(value = "標準", language = Language.Japanese)]
				[Name(value = "Default", language = Language.English)]
                Default = 0,
                [Name(value = "固定", language = Language.Japanese)]
				[Name(value = "Fixed", language = Language.English)]
                Fixed = 1,
            }
		}

        public class RingParamater
        {
			[Name(language = Language.English, value = "Shape")]
			[IO(Export = true)]
			public RingShapeParameter RingShape { get; private set; }

            [Name(language = Language.Japanese, value = "描画順")]
			[Name(language = Language.English, value = "Rendering Order")]
            public Value.Enum<RenderingOrder> RenderingOrder { get; private set; }

            [Name(language = Language.Japanese, value = "配置方法")]
			[Name(language = Language.English, value = "Configuration")]
            public Value.Enum<BillboardType> Billboard { get; private set; }

            [Name(language = Language.Japanese, value = "ブレンド")]
			[Name(language = Language.English, value = "Blend")]
			[IO(Export = false)]
			[Shown(Shown = false)]
            public Value.Enum<AlphaBlendType> AlphaBlend { get; private set; }

            [Name(language = Language.Japanese, value = "頂点数")]
			[Name(language = Language.English, value = "Vertex Count")]
            public Value.Int VertexCount { get; private set; }

			/// <summary>
			/// for compatibility
			/// </summary>
            [Selector(ID = 0)]
			[Shown(Shown = false)]
			[IO(Export = false)]
            [Name(language = Language.Japanese, value = "表示角度")]
			[Name(language = Language.English, value = "Viewing Angle")]
            public Value.Enum<ViewingAngleType> ViewingAngle { get; private set; }

			/// <summary>
			/// for compatibility
			/// </summary>
			[Selected(ID = 0, Value = 0)]
			[Shown(Shown = false)]
			[IO(Export = false)]
			public Value.Float ViewingAngle_Fixed { get; private set; }

			/// <summary>
			/// for compatibility
			/// </summary>
			[Selected(ID = 0, Value = 1)]
			[Shown(Shown = false)]
			[IO(Export = false)]

			public Value.FloatWithRandom ViewingAngle_Random { get; private set; }

			/// <summary>
			/// for compatibility
			/// </summary>
			[Selected(ID = 0, Value = 2)]
			[Shown(Shown = false)]
			[IO(Export = false)]
			public FloatEasingParamater ViewingAngle_Easing { get; private set; }
			

            [Selector(ID = 1)]
            [Name(language = Language.Japanese, value = "外輪")]
			[Name(language = Language.English, value = "Outer")]
            public Value.Enum<LocationType> Outer { get; private set; }

            [Selected(ID = 1, Value = 0)]
			[IO(Export = true)]
            public FixedLocation Outer_Fixed { get; private set; }

            [Selected(ID = 1, Value = 1)]
			[IO(Export = true)]
            public PVALocation Outer_PVA { get; private set; }

            [Selected(ID = 1, Value = 2)]
            [IO(Export = true)]
            public Vector2DEasingParamater Outer_Easing { get; private set; }

            [Selector(ID = 2)]
            [Name(language = Language.Japanese, value = "内輪")]
			[Name(language = Language.English, value = "Inner")]
            public Value.Enum<LocationType> Inner { get; private set; }

            [Selected(ID = 2, Value = 0)]
			[IO(Export = true)]
            public FixedLocation Inner_Fixed { get; private set; }

            [Selected(ID = 2, Value = 1)]
			[IO(Export = true)]
            public PVALocation Inner_PVA { get; private set; }

            [Selected(ID = 2, Value = 2)]
            [IO(Export = true)]
            public Vector2DEasingParamater Inner_Easing { get; private set; }

            [Selector(ID = 3)]
            [Name(language = Language.Japanese, value = "中心比率")]
			[Name(language = Language.English, value = "Center Ratio")]
            public Value.Enum<CenterRatioType> CenterRatio { get; private set; }

            [Selected(ID = 3, Value = 0)]
            public Value.Float CenterRatio_Fixed { get; private set; }

            [Selected(ID = 3, Value = 1)]
            public Value.FloatWithRandom CenterRatio_Random { get; private set; }

            [Selected(ID = 3, Value = 2)]
            [IO(Export = true)]
            public FloatEasingParamater CenterRatio_Easing { get; private set; }

            [Selector(ID = 4)]
            [Name(language = Language.Japanese, value = "外輪色")]
			[Name(language = Language.English, value = "Outer Color")]
            public Value.Enum<ColorType> OuterColor { get; private set; }

            [Selected(ID = 4, Value = 0)]
            public Value.Color OuterColor_Fixed { get; private set; }

            [Selected(ID = 4, Value = 1)]
            public Value.ColorWithRandom OuterColor_Random { get; private set; }

            [Selected(ID = 4, Value = 2)]
            [IO(Export = true)]
            public ColorEasingParamater OuterColor_Easing { get; private set; }

            [Selector(ID = 5)]
            [Name(language = Language.Japanese, value = "中心色")]
			[Name(language = Language.English, value = "Center Color")]
            public Value.Enum<ColorType> CenterColor { get; private set; }

            [Selected(ID = 5, Value = 0)]
			[IO(Export = true)]
            public Value.Color CenterColor_Fixed { get; private set; }

            [Selected(ID = 5, Value = 1)]
			[IO(Export = true)]
            public Value.ColorWithRandom CenterColor_Random { get; private set; }

            [Selected(ID = 5, Value = 2)]
            [IO(Export = true)]
            public ColorEasingParamater CenterColor_Easing { get; private set; }

            [Selector(ID = 6)]
            [Name(language = Language.Japanese, value = "内輪色")]
			[Name(language = Language.English, value = "Inner Color")]
            public Value.Enum<ColorType> InnerColor { get; private set; }

            [Selected(ID = 6, Value = 0)]
            public Value.Color InnerColor_Fixed { get; private set; }

            [Selected(ID = 6, Value = 1)]
            public Value.ColorWithRandom InnerColor_Random { get; private set; }

            [Selected(ID = 6, Value = 2)]
            [IO(Export = true)]
            public ColorEasingParamater InnerColor_Easing { get; private set; }


            [Name(language = Language.Japanese, value = "色画像")]
            [Description(language = Language.Japanese, value = "リボンの色を表す画像")]
			[Name(language = Language.English, value = "Color Image")]
			[Description(language = Language.English, value = "Image representing the color of the ribbon")]
			[IO(Export = false)]
			[Shown(Shown = false)]
            public Value.Path ColorTexture
            {
                get;
                private set;
            }

            public RingParamater()
            {
				RingShape = new RingShapeParameter();
                RenderingOrder = new Value.Enum<Data.RenderingOrder>(Data.RenderingOrder.FirstCreatedInstanceIsFirst);

                Billboard = new Value.Enum<BillboardType>(BillboardType.Fixed);
                AlphaBlend = new Value.Enum<AlphaBlendType>(AlphaBlendType.Blend);

                VertexCount = new Value.Int(16, 256, 3);

				/// for compatibility
				ViewingAngle = new Value.Enum<ViewingAngleType>(ViewingAngleType.Fixed);
                ViewingAngle_Fixed = new Value.Float(360.0f, 360.0f, 0.0f);
                ViewingAngle_Random = new Value.FloatWithRandom(360.0f, 360.0f, 0.0f);
                ViewingAngle_Easing = new FloatEasingParamater(360.0f, 360.0f, 0.0f);
				

                Outer = new Value.Enum<LocationType>(LocationType.Fixed);
                Outer_Fixed = new FixedLocation(2.0f, 0.0f);
                Outer_PVA = new PVALocation(2.0f, 0.0f);
                Outer_Easing = new Vector2DEasingParamater();

                Inner = new Value.Enum<LocationType>(LocationType.Fixed);
                Inner_Fixed = new FixedLocation(1.0f, 0.0f);
                Inner_PVA = new PVALocation(1.0f, 0.0f);
                Inner_Easing = new Vector2DEasingParamater();

                CenterRatio = new Value.Enum<CenterRatioType>(CenterRatioType.Fixed);
                CenterRatio_Fixed = new Value.Float(0.5f, 1.0f, 0.0f);
                CenterRatio_Random = new Value.FloatWithRandom(0.5f, 1.0f, 0.0f);
                CenterRatio_Easing = new FloatEasingParamater(0.5f, 1.0f, 0.0f);

                OuterColor = new Value.Enum<ColorType>(ColorType.Fixed);
                OuterColor_Fixed = new Value.Color(255, 255, 255, 0);
                OuterColor_Random = new Value.ColorWithRandom(255, 255, 255, 0);
                OuterColor_Easing = new ColorEasingParamater();

                CenterColor = new Value.Enum<ColorType>(ColorType.Fixed);
                CenterColor_Fixed = new Value.Color(255, 255, 255, 255);
                CenterColor_Random = new Value.ColorWithRandom(255, 255, 255, 255);
                CenterColor_Easing = new ColorEasingParamater();

                InnerColor = new Value.Enum<ColorType>(ColorType.Fixed);
                InnerColor_Fixed = new Value.Color(255, 255, 255, 0);
                InnerColor_Random = new Value.ColorWithRandom(255, 255, 255, 0);
                InnerColor_Easing = new ColorEasingParamater();

                ColorTexture = new Value.Path(Resources.GetString("ImageFilter"), true, "");
            }

			/// <summary>
			/// for compatibility
			/// </summary>
			public enum ViewingAngleType : int
            {
                [Name(value = "位置", language = Language.Japanese)]
				[Name(value = "Fixed", language = Language.English)]
                Fixed = 0,
                [Name(value = "ランダム", language = Language.Japanese)]
				[Name(value = "Random", language = Language.English)]
                Random = 1,
                [Name(value = "イージング", language = Language.Japanese)]
				[Name(value = "Easing", language = Language.English)]
                Easing = 2,
            }
			

            public enum LocationType : int
            {
                [Name(value = "位置", language = Language.Japanese)]
				[Name(value = "Fixed", language = Language.English)]
                Fixed = 0,
                [Name(value = "位置・速度・加速度", language = Language.Japanese)]
				[Name(value = "PVA", language = Language.English)]
                PVA = 1,
                [Name(value = "イージング", language = Language.Japanese)]
				[Name(value = "Easing", language = Language.English)]
                Easing = 2,
            }

            public class FixedLocation
            {
                [Name(language = Language.Japanese, value = "位置")]
                [Description(language = Language.Japanese, value = "インスタンスの位置")]
				[Name(language = Language.English, value = "Position")]
				[Description(language = Language.English, value = "Position of the instance")]
                public Value.Vector2D Location
                {
                    get;
                    private set;
                }

                internal FixedLocation(float x = 0.0f, float y = 0.0f)
                {
                    Location = new Value.Vector2D(x, y);
                }
            }

            public enum CenterRatioType : int
            {
                [Name(value = "位置", language = Language.Japanese)]
				[Name(value = "Fixed", language = Language.English)]
                Fixed = 0,
                [Name(value = "ランダム", language = Language.Japanese)]
				[Name(value = "Random", language = Language.English)]
                Random = 1,
                [Name(value = "イージング", language = Language.Japanese)]
				[Name(value = "Easing", language = Language.English)]
                Easing = 2,
            }

            public class PVALocation
            {
                [Name(language = Language.Japanese, value = "位置")]
                [Description(language = Language.Japanese, value = "インスタンスの初期位置")]
				[Name(language = Language.English, value = "Pos")]
				[Description(language = Language.English, value = "Position of the instance")]
                public Value.Vector2DWithRandom Location
                {
                    get;
                    private set;
                }

                [Name(language = Language.Japanese, value = "速度")]
                [Description(language = Language.Japanese, value = "インスタンスの初期速度")]
				[Name(language = Language.English, value = "Speed")]
				[Description(language = Language.English, value = "Initial velocity of the instance")]
                public Value.Vector2DWithRandom Velocity
                {
                    get;
                    private set;
                }

                [Name(language = Language.Japanese, value = "加速度")]
                [Description(language = Language.Japanese, value = "インスタンスの初期加速度")]
				[Name(language = Language.English, value = "Accel")]
				[Description(language = Language.English, value = "Acceleration of the instance")]
                public Value.Vector2DWithRandom Acceleration
                {
                    get;
                    private set;
                }

                internal PVALocation(float x = 0.0f, float y = 0.0f)
                {
                    Location = new Value.Vector2DWithRandom(x, y);
                    Velocity = new Value.Vector2DWithRandom(0, 0);
                    Acceleration = new Value.Vector2DWithRandom(0, 0);
                }
            }

            public enum ColorType : int
            {
                [Name(value = "固定", language = Language.Japanese)]
				[Name(value = "Fixed", language = Language.English)]
                Fixed = 0,
                [Name(value = "ランダム", language = Language.Japanese)]
				[Name(value = "Random", language = Language.English)]
                Random = 1,
                [Name(value = "イージング", language = Language.Japanese)]
				[Name(value = "Easing", language = Language.English)]
                Easing = 2,
            }
        }

		public class ModelParamater
		{
			[Name(language = Language.Japanese, value = "モデル")]
			[Description(language = Language.Japanese, value = "モデルファイル")]
			[Name(language = Language.English, value = "Model")]
			[Description(language = Language.English, value = "Model File")]
			public Value.PathForModel Model
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "配置方法")]
			[Name(language = Language.English, value = "Configuration")]
			public Value.Enum<BillboardType> Billboard { get; private set; }

			[Name(language = Language.Japanese, value = "カリング")]
			[Name(language = Language.English, value = "Culling")]
			public Value.Enum<CullingValues> Culling { get; private set; }

			public ModelParamater()
			{
                Model = new Value.PathForModel(Resources.GetString("ModelFilter"), true, "");

				Billboard = new Value.Enum<BillboardType>(BillboardType.Fixed);

				Culling = new Value.Enum<CullingValues>(Data.CullingValues.Front);

				Color = new Value.Enum<StandardColorType>(StandardColorType.Fixed);
				Color_Fixed = new Value.Color(255, 255, 255, 255);
				Color_Random = new Value.ColorWithRandom(255, 255, 255, 255);
				Color_Easing = new ColorEasingParamater();
				Color_FCurve = new ColorFCurveParameter();
			}

			[Selector(ID = 0)]
			[Name(language = Language.Japanese, value = "全体色")]
			public Value.Enum<StandardColorType> Color { get; private set; }

			[Selected(ID = 0, Value = 0)]
			public Value.Color Color_Fixed { get; private set; }

			[Selected(ID = 0, Value = 1)]
			public Value.ColorWithRandom Color_Random { get; private set; }

			[Selected(ID = 0, Value = 2)]
			[IO(Export = true)]
			public ColorEasingParamater Color_Easing { get; private set; }

			[Selected(ID = 0, Value = 3)]
			[IO(Export = true)]
			public ColorFCurveParameter Color_FCurve { get; private set; }
		}

		public class TrackParameter
		{
			[Selector(ID = 10)]
			[Name(language = Language.Japanese, value = "幅前方")]
			[Name(language = Language.English, value = "Front Size")]
			public Value.Enum<TrackSizeType> TrackSizeFor { get; private set; }

			[Selected(ID = 10, Value = 0)]
			public Value.Float TrackSizeFor_Fixed { get; private set; }

			[Selector(ID = 11)]
			[Name(language = Language.Japanese, value = "幅中間")]
			[Name(language = Language.English, value = "Middle Size")]
			public Value.Enum<TrackSizeType> TrackSizeMiddle { get; private set; }

			[Selected(ID = 11, Value = 0)]
			public Value.Float TrackSizeMiddle_Fixed { get; private set; }

			[Selector(ID = 12)]
			[Name(language = Language.Japanese, value = "幅後方")]
			[Name(language = Language.English, value = "Back Size")]
			public Value.Enum<TrackSizeType> TrackSizeBack { get; private set; }

			[Selected(ID = 12, Value = 0)]
			public Value.Float TrackSizeBack_Fixed { get; private set; }

			[Name(language = Language.Japanese, value = "スプラインの分割数")]
			[Description(language = Language.Japanese, value = "スプラインの分割数")]
			[Name(language = Language.English, value = "The number of \nspline division")]
			[Description(language = Language.English, value = "The number of spline division")]
			public Value.Int SplineDivision
			{
				get;
				private set;
			}

			[Selector(ID = 1)]
			[Name(language = Language.Japanese, value = "色・左")]
			[Name(language = Language.English, value = "Color, Left")]
			public Value.Enum<StandardColorType> ColorLeft { get; private set; }

			[Selected(ID = 1, Value = 0)]
			public Value.Color ColorLeft_Fixed { get; private set; }

			[Selected(ID = 1, Value = 1)]
			public Value.ColorWithRandom ColorLeft_Random { get; private set; }

			[Selected(ID = 1, Value = 2)]
			[IO(Export = true)]
			public ColorEasingParamater ColorLeft_Easing { get; private set; }

			[Selected(ID = 1, Value = 3)]
			[IO(Export = true)]
			public ColorFCurveParameter ColorLeft_FCurve { get; private set; }

			[Selector(ID = 2)]
			[Name(language = Language.Japanese, value = "色・左中間")]
			[Name(language = Language.English, value = "Color, Left-Center")]
			public Value.Enum<StandardColorType> ColorLeftMiddle { get; private set; }

			[Selected(ID = 2, Value = 0)]
			public Value.Color ColorLeftMiddle_Fixed { get; private set; }

			[Selected(ID = 2, Value = 1)]
			public Value.ColorWithRandom ColorLeftMiddle_Random { get; private set; }

			[Selected(ID = 2, Value = 2)]
			[IO(Export = true)]
			public ColorEasingParamater ColorLeftMiddle_Easing { get; private set; }

			[Selected(ID = 2, Value = 3)]
			[IO(Export = true)]
			public ColorFCurveParameter ColorLeftMiddle_FCurve { get; private set; }

			[Selector(ID = 3)]
			[Name(language = Language.Japanese, value = "色・中央")]
			[Name(language = Language.English, value = "Color, Center")]
			public Value.Enum<StandardColorType> ColorCenter { get; private set; }

			[Selected(ID = 3, Value = 0)]
			public Value.Color ColorCenter_Fixed { get; private set; }

			[Selected(ID = 3, Value = 1)]
			public Value.ColorWithRandom ColorCenter_Random { get; private set; }

			[Selected(ID = 3, Value = 2)]
			[IO(Export = true)]
			public ColorEasingParamater ColorCenter_Easing { get; private set; }

			[Selected(ID = 3, Value = 3)]
			[IO(Export = true)]
			public ColorFCurveParameter ColorCenter_FCurve { get; private set; }

			[Selector(ID = 4)]
			[Name(language = Language.Japanese, value = "色・中央中間")]
			[Name(language = Language.English, value = "Color, Center-Mid.")]
			public Value.Enum<StandardColorType> ColorCenterMiddle { get; private set; }

			[Selected(ID = 4, Value = 0)]
			public Value.Color ColorCenterMiddle_Fixed { get; private set; }

			[Selected(ID = 4, Value = 1)]
			public Value.ColorWithRandom ColorCenterMiddle_Random { get; private set; }

			[Selected(ID = 4, Value = 2)]
			[IO(Export = true)]
			public ColorEasingParamater ColorCenterMiddle_Easing { get; private set; }

			[Selected(ID = 4, Value = 3)]
			[IO(Export = true)]
			public ColorFCurveParameter ColorCenterMiddle_FCurve { get; private set; }

			[Selector(ID = 5)]
			[Name(language = Language.Japanese, value = "色・右")]
			[Name(language = Language.English, value = "Color, Right")]
			public Value.Enum<StandardColorType> ColorRight { get; private set; }

			[Selected(ID = 5, Value = 0)]
			public Value.Color ColorRight_Fixed { get; private set; }

			[Selected(ID = 5, Value = 1)]
			public Value.ColorWithRandom ColorRight_Random { get; private set; }

			[Selected(ID = 5, Value = 2)]
			[IO(Export = true)]
			public ColorEasingParamater ColorRight_Easing { get; private set; }

			[Selected(ID = 5, Value = 3)]
			[IO(Export = true)]
			public ColorFCurveParameter ColorRight_FCurve { get; private set; }

			[Selector(ID = 6)]
			[Name(language = Language.Japanese, value = "色・右中間")]
			[Name(language = Language.English, value = "Color, Right-Center")]
			public Value.Enum<StandardColorType> ColorRightMiddle { get; private set; }

			[Selected(ID = 6, Value = 0)]
			public Value.Color ColorRightMiddle_Fixed { get; private set; }

			[Selected(ID = 6, Value = 1)]
			public Value.ColorWithRandom ColorRightMiddle_Random { get; private set; }

			[Selected(ID = 6, Value = 2)]
			[IO(Export = true)]
			public ColorEasingParamater ColorRightMiddle_Easing { get; private set; }

			[Selected(ID = 6, Value = 3)]
			[IO(Export = true)]
			public ColorFCurveParameter ColorRightMiddle_FCurve { get; private set; }

			
			public TrackParameter()
			{
				TrackSizeFor = new Value.Enum<TrackSizeType>(TrackSizeType.Fixed);
				TrackSizeFor_Fixed = new Value.Float(1, float.MaxValue, 0);

				TrackSizeMiddle = new Value.Enum<TrackSizeType>(TrackSizeType.Fixed);
				TrackSizeMiddle_Fixed = new Value.Float(1, float.MaxValue, 0);

				TrackSizeBack = new Value.Enum<TrackSizeType>(TrackSizeType.Fixed);
				TrackSizeBack_Fixed = new Value.Float(1, float.MaxValue, 0);

				ColorLeft = new Value.Enum<StandardColorType>(StandardColorType.Fixed);
				ColorLeft_Fixed = new Value.Color(255, 255, 255, 255);
				ColorLeft_Random = new Value.ColorWithRandom(255, 255, 255, 255);
				ColorLeft_Easing = new ColorEasingParamater();
				ColorLeft_FCurve = new ColorFCurveParameter();

				ColorLeftMiddle = new Value.Enum<StandardColorType>(StandardColorType.Fixed);
				ColorLeftMiddle_Fixed = new Value.Color(255, 255, 255, 255);
				ColorLeftMiddle_Random = new Value.ColorWithRandom(255, 255, 255, 255);
				ColorLeftMiddle_Easing = new ColorEasingParamater();
				ColorLeftMiddle_FCurve = new ColorFCurveParameter();

				ColorCenter = new Value.Enum<StandardColorType>(StandardColorType.Fixed);
				ColorCenter_Fixed = new Value.Color(255, 255, 255, 255);
				ColorCenter_Random = new Value.ColorWithRandom(255, 255, 255, 255);
				ColorCenter_Easing = new ColorEasingParamater();
				ColorCenter_FCurve = new ColorFCurveParameter();

				ColorCenterMiddle = new Value.Enum<StandardColorType>(StandardColorType.Fixed);
				ColorCenterMiddle_Fixed = new Value.Color(255, 255, 255, 255);
				ColorCenterMiddle_Random = new Value.ColorWithRandom(255, 255, 255, 255);
				ColorCenterMiddle_Easing = new ColorEasingParamater();
				ColorCenterMiddle_FCurve = new ColorFCurveParameter();

				ColorRight = new Value.Enum<StandardColorType>(StandardColorType.Fixed);
				ColorRight_Fixed = new Value.Color(255, 255, 255, 255);
				ColorRight_Random = new Value.ColorWithRandom(255, 255, 255, 255);
				ColorRight_Easing = new ColorEasingParamater();
				ColorRight_FCurve = new ColorFCurveParameter();

				ColorRightMiddle = new Value.Enum<StandardColorType>(StandardColorType.Fixed);
				ColorRightMiddle_Fixed = new Value.Color(255, 255, 255, 255);
				ColorRightMiddle_Random = new Value.ColorWithRandom(255, 255, 255, 255);
				ColorRightMiddle_Easing = new ColorEasingParamater();
				ColorRightMiddle_FCurve = new ColorFCurveParameter();

				SplineDivision = new Value.Int(1, int.MaxValue, 1);
			}
		}

		public enum BillboardType : int
		{
			[Name(value = "ビルボード", language = Language.Japanese)]
			[Name(value = "Billboard", language = Language.English)]
			Billboard = 0,
			[Name(value = "Z軸回転ビルボード", language = Language.Japanese)]
			[Name(value = "Rotated Billboard", language = Language.English)]
			RotatedBillboard = 3,
			[Name(value = "Y軸固定", language = Language.Japanese)]
			[Name(value = "Fixed Y-Axis", language = Language.English)]
			YAxisFixed = 1,
			[Name(value = "固定", language = Language.Japanese)]
			[Name(value = "Fixed", language = Language.English)]
			Fixed = 2,
		}

		public enum ParamaterType : int
		{
			[Name(value = "無し", language = Language.Japanese)]
			[Name(value = "None", language = Language.English)]
			[Icon(resourceName = "NodeEmpty")]
			None = 0,
			//Particle = 1,
			[Name(value = "スプライト", language = Language.Japanese)]
			[Name(value = "Sprite", language = Language.English)]
			[Icon(resourceName = "NodeSprite")]
			Sprite = 2,
			[Name(value = "リボン", language = Language.Japanese)]
			[Name(value = "Ribbon", language = Language.English)]
			[Icon(resourceName = "NodeRibbon")]
            Ribbon = 3,
			[Name(value = "軌跡", language = Language.Japanese)]
			[Name(value = "Track", language = Language.English)]
			[Icon(resourceName = "NodeTrack")]
			Track = 6,
            [Name(value = "リング", language = Language.Japanese)]
			[Name(value = "Ring", language = Language.English)]
			[Icon(resourceName = "NodeRing")]
            Ring = 4,
			[Name(value = "モデル", language = Language.Japanese)]
			[Name(value = "Model", language = Language.English)]
			[Icon(resourceName = "NodeModel")]
			Model = 5,
		}

	}
}
