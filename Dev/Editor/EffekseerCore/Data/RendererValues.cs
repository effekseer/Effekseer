using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class RendererValues
	{
		[Selector(ID = 0)]
		public Value.Enum<ParamaterType> Type
		{
			get;
			private set;
		}

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
			Sprite = new SpriteParamater();
            Ribbon = new RibbonParamater();
			Track = new TrackParameter();
            Ring = new RingParamater();
			Model = new ModelParamater();
		}

		public class SpriteParamater
		{
			[Name(language = Language.Japanese, value = "描画順")]
			public Value.Enum<RenderingOrder> RenderingOrder { get; private set; }

			[Name(language = Language.Japanese, value = "配置方法")]
			public Value.Enum<BillboardType> Billboard { get; private set; }

			[Name(language = Language.Japanese, value = "ブレンド")]
			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<AlphaBlendType> AlphaBlend { get; private set; }

			[Selector(ID = 0)]
			[Name(language = Language.Japanese, value = "全体色")]
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
			public Value.Enum<ColorType> Color { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Name(language = Language.Japanese, value = "左下色")]
			public Value.Color Color_Fixed_LL { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Name(language = Language.Japanese, value = "右下色")]
			public Value.Color Color_Fixed_LR { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Name(language = Language.Japanese, value = "左上色")]
			public Value.Color Color_Fixed_UL { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Name(language = Language.Japanese, value = "右上色")]
			public Value.Color Color_Fixed_UR { get; private set; }

			[Selector(ID = 2)]
			[Name(language = Language.Japanese, value = "頂点座標")]
			public Value.Enum<PositionType> Position { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Name(language = Language.Japanese, value = "左下座標")]
			public Value.Vector2D Position_Fixed_LL { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Name(language = Language.Japanese, value = "右下座標")]
			public Value.Vector2D Position_Fixed_LR { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Name(language = Language.Japanese, value = "左上座標")]
			public Value.Vector2D Position_Fixed_UL { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Name(language = Language.Japanese, value = "右上座標")]
			public Value.Vector2D Position_Fixed_UR { get; private set; }

			[Name(language = Language.Japanese, value = "カラー画像")]
			[Description(language = Language.Japanese, value = "スプライトの色を表す画像")]
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

			public enum BillboardType : int
			{
				[Name(value = "ビルボード", language = Language.Japanese)]
				Billboard = 0,
				[Name(value = "Z軸回転ビルボード", language = Language.Japanese)]
				RotatedBillboard = 3,
				[Name(value = "Y軸固定", language = Language.Japanese)]
				YAxisFixed = 1,
				[Name(value = "固定", language = Language.Japanese)]
				Fixed = 2,
			}

            public enum ColorType : int
            {
                [Name(value = "標準", language = Language.Japanese)]
                Default = 0,
                [Name(value = "固定", language = Language.Japanese)]
                Fixed = 1,
            }

            public enum PositionType : int
            {
                [Name(value = "標準", language = Language.Japanese)]
                Default = 0,
                [Name(value = "固定", language = Language.Japanese)]
                Fixed = 1,
            }
		}

		public class RibbonParamater
		{
			[Name(language = Language.Japanese, value = "ブレンド")]
			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<AlphaBlendType> AlphaBlend { get; private set; }

			[Name(language = Language.Japanese, value = "視点追従")]
			public Value.Boolean ViewpointDependent { get; private set; }

			[Selector(ID = 0)]
			[Name(language = Language.Japanese, value = "全体色")]
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
			public Value.Enum<ColorType> Color { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Name(language = Language.Japanese, value = "左頂点色")]
			public Value.Color Color_Fixed_L { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Name(language = Language.Japanese, value = "右頂点色")]
			public Value.Color Color_Fixed_R { get; private set; }

			[Selector(ID = 2)]
			[Name(language = Language.Japanese, value = "座標")]
			public Value.Enum<PositionType> Position { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Name(language = Language.Japanese, value = "左座標")]
			public Value.Float Position_Fixed_L { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Name(language = Language.Japanese, value = "右座標")]
			public Value.Float Position_Fixed_R { get; private set; }


			[Name(language = Language.Japanese, value = "色画像")]
			[Description(language = Language.Japanese, value = "リボンの色を表す画像")]
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
			
				ColorTexture = new Value.Path("画像ファイル (*.png)|*.png", true, "");
			}

            public enum ColorAllType : int
            {
                [Name(value = "固定", language = Language.Japanese)]
                Fixed = 0,
                [Name(value = "ランダム", language = Language.Japanese)]
                Random = 1,
                [Name(value = "イージング", language = Language.Japanese)]
                Easing = 2,
            }

            public enum ColorType : int
            {
                [Name(value = "標準", language = Language.Japanese)]
                Default = 0,
                [Name(value = "固定", language = Language.Japanese)]
                Fixed = 1,
            }

            public enum PositionType : int
            {
                [Name(value = "標準", language = Language.Japanese)]
                Default = 0,
                [Name(value = "固定", language = Language.Japanese)]
                Fixed = 1,
            }
		}

        public class RingParamater
        {
            [Name(language = Language.Japanese, value = "描画順")]
            public Value.Enum<RenderingOrder> RenderingOrder { get; private set; }

            [Name(language = Language.Japanese, value = "配置方法")]
            public Value.Enum<BillboardType> Billboard { get; private set; }

            [Name(language = Language.Japanese, value = "ブレンド")]
			[IO(Export = false)]
			[Shown(Shown = false)]
            public Value.Enum<AlphaBlendType> AlphaBlend { get; private set; }

            [Name(language = Language.Japanese, value = "頂点数")]
            public Value.Int VertexCount { get; private set; }

            [Selector(ID = 0)]
            [Name(language = Language.Japanese, value = "表示角度")]
            public Value.Enum<ViewingAngleType> ViewingAngle { get; private set; }

            [Selected(ID = 0, Value = 0)]
            public Value.Float ViewingAngle_Fixed { get; private set; }

            [Selected(ID = 0, Value = 1)]
            public Value.FloatWithRandom ViewingAngle_Random { get; private set; }

            [Selected(ID = 0, Value = 2)]
            [IO(Export = true)]
            public FloatEasingParamater ViewingAngle_Easing { get; private set; }

            [Selector(ID = 1)]
            [Name(language = Language.Japanese, value = "外輪")]
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
			[IO(Export = false)]
			[Shown(Shown = false)]
            public Value.Path ColorTexture
            {
                get;
                private set;
            }

            public RingParamater()
            {
                RenderingOrder = new Value.Enum<Data.RenderingOrder>(Data.RenderingOrder.FirstCreatedInstanceIsFirst);

                Billboard = new Value.Enum<BillboardType>(BillboardType.Fixed);
                AlphaBlend = new Value.Enum<AlphaBlendType>(AlphaBlendType.Blend);

                VertexCount = new Value.Int(16, 256, 3);

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

                ColorTexture = new Value.Path("画像ファイル (*.png)|*.png",true, "");
            }

            public enum BillboardType : int
            {
                [Name(value = "ビルボード", language = Language.Japanese)]
                Billboard = 0,
				[Name(value = "Z軸回転ビルボード", language = Language.Japanese)]
                RotatedBillboard = 3,
                [Name(value = "Y軸固定", language = Language.Japanese)]
                YAxisFixed = 1,
                [Name(value = "固定", language = Language.Japanese)]
                Fixed = 2,
            }

            public enum ViewingAngleType : int
            {
                [Name(value = "位置", language = Language.Japanese)]
                Fixed = 0,
                [Name(value = "ランダム", language = Language.Japanese)]
                Random = 1,
                [Name(value = "イージング", language = Language.Japanese)]
                Easing = 2,
            }

            public enum LocationType : int
            {
                [Name(value = "位置", language = Language.Japanese)]
                Fixed = 0,
                [Name(value = "位置・速度・加速度", language = Language.Japanese)]
                PVA = 1,
                [Name(value = "イージング", language = Language.Japanese)]
                Easing = 2,
            }

            public class FixedLocation
            {
                [Name(language = Language.Japanese, value = "位置")]
                [Description(language = Language.Japanese, value = "インスタンスの位置")]
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
                Fixed = 0,
                [Name(value = "ランダム", language = Language.Japanese)]
                Random = 1,
                [Name(value = "イージング", language = Language.Japanese)]
                Easing = 2,
            }

            public class PVALocation
            {
                [Name(language = Language.Japanese, value = "位置")]
                [Description(language = Language.Japanese, value = "インスタンスの初期位置")]
                public Value.Vector2DWithRandom Location
                {
                    get;
                    private set;
                }

                [Name(language = Language.Japanese, value = "速度")]
                [Description(language = Language.Japanese, value = "インスタンスの初期速度")]
                public Value.Vector2DWithRandom Velocity
                {
                    get;
                    private set;
                }

                [Name(language = Language.Japanese, value = "加速度")]
                [Description(language = Language.Japanese, value = "インスタンスの初期加速度")]
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
                Fixed = 0,
                [Name(value = "ランダム", language = Language.Japanese)]
                Random = 1,
                [Name(value = "イージング", language = Language.Japanese)]
                Easing = 2,
            }
        }

		public class ModelParamater
		{
			[Name(language = Language.Japanese, value = "モデル")]
			[Description(language = Language.Japanese, value = "モデルファイル")]
			public Value.Path Model
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "法線画像")]
			[Description(language = Language.Japanese, value = "法線を表す画像")]
			public Value.PathForImage NormalTexture
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "ライティング")]
			public Value.Boolean Lighting { get; private set; }

			[Name(language = Language.Japanese, value = "カリング")]
			public Value.Enum<CullingValues> Culling { get; private set; }

			public ModelParamater()
			{
				Model = new Value.Path("モデルファイル (*.efkmodel)|*.efkmodel", true, "");
				NormalTexture = new Value.PathForImage("画像ファイル (*.png)|*.png",true,  "");
				Lighting = new Value.Boolean(true);
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
			public Value.Enum<TrackSizeType> TrackSizeFor { get; private set; }

			[Selected(ID = 10, Value = 0)]
			public Value.Float TrackSizeFor_Fixed { get; private set; }

			[Selector(ID = 11)]
			[Name(language = Language.Japanese, value = "幅中間")]
			public Value.Enum<TrackSizeType> TrackSizeMiddle { get; private set; }

			[Selected(ID = 11, Value = 0)]
			public Value.Float TrackSizeMiddle_Fixed { get; private set; }

			[Selector(ID = 12)]
			[Name(language = Language.Japanese, value = "幅後方")]
			public Value.Enum<TrackSizeType> TrackSizeBack { get; private set; }

			[Selected(ID = 12, Value = 0)]
			public Value.Float TrackSizeBack_Fixed { get; private set; }

			[Selector(ID = 1)]
			[Name(language = Language.Japanese, value = "色・左")]
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

			}
		}

		public enum ParamaterType : int
		{
			[Name(value = "無し", language = Language.Japanese)]
			None = 0,
			//Particle = 1,
			[Name(value = "スプライト", language = Language.Japanese)]
			Sprite = 2,
			[Name(value = "リボン", language = Language.Japanese)]
            Ribbon = 3,
			[Name(value = "軌跡", language = Language.Japanese)]
			Track = 6,
            [Name(value = "リング", language = Language.Japanese)]
            Ring = 4,
			[Name(value = "モデル", language = Language.Japanese)]
			Model = 5,
		}

	}
}
