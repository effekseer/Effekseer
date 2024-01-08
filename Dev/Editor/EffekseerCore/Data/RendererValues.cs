using Effekseer.Data.Value;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum TextureUVType
	{
		[Key(key = "TextureUVTypeParameter_Type_Strech")]
		Strech = 0,
		[Key(key = "TextureUVTypeParameter_Type_Tile")]
		Tile = 2,
		[Key(key = "TextureUVTypeParameter_Type_TilePerParticle")]
		TilePerParticle = 1,
	}

	public enum TrailSmoothingType : int
	{
		[Key(key = "Disabled")]
		Disabled = 0,
		[Key(key = "Enabled")]
		Enabled = 1,
	}

	public enum TrailTimeSourceType : int
	{
		[Key(key = "RS_TrailTimeSource_FirstParticle")]
		FirstParticle = 0,
		[Key(key = "RS_TrailTimeSource_ParticleGroup")]
		ParticleGroup = 1,
	}

	public class TextureUVTypeParameter
	{
		[Key(key = "TextureUVTypeParameter_Type")]
		[Selector(ID = 0)]
		public Value.Enum<TextureUVType> Type
		{
			get;
			private set;
		}

		[Key(key = "TextureUVTypeParameter_TileLength")]
		[Selected(ID = 0, Value = (int)TextureUVType.Tile)]
		public Value.Float TileLength { get; private set; } = new Value.Float(1, float.MaxValue, 0.1f);

		[Key(key = "TextureUVTypeParameter_TileEdgeHead")]
		[Selected(ID = 0, Value = (int)TextureUVType.TilePerParticle)]
		public Value.Int TileEdgeHead { get; private set; }

		[Key(key = "TextureUVTypeParameter_TileEdgeTail")]
		[Selected(ID = 0, Value = (int)TextureUVType.TilePerParticle)]
		public Value.Int TileEdgeTail { get; private set; }

		[Key(key = "TextureUVTypeParameter_TileLoopingArea")]
		[Selected(ID = 0, Value = (int)TextureUVType.TilePerParticle)]
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
		[Key(key = "RingShapeType_Donut")]
		Donut = 0,

		[Key(key = "RingShapeType_Crescent")]
		Crescent = 1,
	}

	public enum FixedRandomEasingType : int
	{
		[Key(key = "FixedRandomEasingType_Fixed")]
		Fixed = 0,
		[Key(key = "FixedRandomEasingType_Random")]
		Random = 1,
		[Key(key = "FixedRandomEasingType_Easing")]
		Easing = 2,
	}

	public class RingShapeCrescentParameter
	{
		[Key(key = "RS_RingShapeCrescent_StartingFade")]
		public Value.Float StartingFade { get; private set; }

		[Key(key = "RS_RingShapeCrescent_EndingFade")]
		public Value.Float EndingFade { get; private set; }

		[Selector(ID = 0)]
		[Key(key = "RS_RingShapeCrescent_StartingAngle")]
		public Value.Enum<FixedRandomEasingType> StartingAngle { get; private set; }

		[Selected(ID = 0, Value = 0)]
		public Value.Float StartingAngle_Fixed { get; private set; }

		[Selected(ID = 0, Value = 1)]
		public Value.FloatWithRandom StartingAngle_Random { get; private set; }

		[Selected(ID = 0, Value = 2)]
		[IO(Export = true)]
		[TreeNode(id = "StartingAngle_Easing", key = "Easing_Parameter", type = TreeNodeType.Small)]
		public FloatEasingParamater StartingAngle_Easing { get; private set; }

		[Selector(ID = 1)]
		[Key(key = "RS_RingShapeCrescent_EndingAngle")]
		public Value.Enum<FixedRandomEasingType> EndingAngle { get; private set; }

		[Selected(ID = 1, Value = 0)]
		public Value.Float EndingAngle_Fixed { get; private set; }

		[Selected(ID = 1, Value = 1)]
		public Value.FloatWithRandom EndingAngle_Random { get; private set; }

		[Selected(ID = 1, Value = 2)]
		[IO(Export = true)]
		[TreeNode(id = "EndingAngle_Easing", key = "Easing_Parameter", type = TreeNodeType.Small)]
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
		[Selector(ID = 0)]
		[Key(key = "RS_RingShape_Type")]
		public Value.Enum<RingShapeType> Type
		{
			get;
			private set;
		}

		[IO(Export = true)]
		[Selected(ID = 0, Value = (int)RingShapeType.Crescent)]
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

		[Key(key = "RS_TrailSmoothing")]
		[Selected(ID = 0, Value = 6)]
		[IO(Export = true)]
		public Value.Enum<TrailSmoothingType> TrailSmoothing { get; private set; } = new Enum<TrailSmoothingType>(TrailSmoothingType.Enabled);

		[Key(key = "RS_TrailTimeSource")]
		[Selected(ID = 0, Value = 3)]
		[Selected(ID = 0, Value = 6)]
		[IO(Export = true)]
		public Value.Enum<TrailTimeSourceType> TrailTimeSource { get; private set; } = new Enum<TrailTimeSourceType>(TrailTimeSourceType.ParticleGroup);


		[Selected(ID = 0, Value = (int)ParamaterType.Sprite)]
		[Selected(ID = 0, Value = (int)ParamaterType.Model)]
		[IO(Export = true)]
		[Key(key = "RS_ColorAll")]
		[Group]
		public StandardColor ColorAll { get; private set; } = new StandardColor();

		[Selected(ID = 0, Value = (int)ParamaterType.Track)]
		[IO(Export = true)]
		[Key(key = "RS_Track_ColorLeft")]
		[Group]
		public StandardColor TrailColorLeft { get; private set; } = new StandardColor();

		[Selected(ID = 0, Value = (int)ParamaterType.Track)]
		[IO(Export = true)]
		[Key(key = "RS_Track_ColorLeftMiddle")]
		[Group]
		public StandardColor TrailColorLeftMiddle { get; private set; } = new StandardColor();

		[Selected(ID = 0, Value = (int)ParamaterType.Track)]
		[IO(Export = true)]
		[Key(key = "RS_Track_ColorCenter")]
		[Group]
		public StandardColor TrailColorCenter { get; private set; } = new StandardColor();

		[Selected(ID = 0, Value = (int)ParamaterType.Track)]
		[IO(Export = true)]
		[Key(key = "RS_Track_ColorCenterMiddle")]
		[Group]
		public StandardColor TrailColorCenterMiddle { get; private set; } = new StandardColor();

		[Selected(ID = 0, Value = (int)ParamaterType.Track)]
		[IO(Export = true)]
		[Key(key = "RS_Track_ColorRight")]
		[Group]
		public StandardColor TrailColorRight { get; private set; } = new StandardColor();


		[Selected(ID = 0, Value = (int)ParamaterType.Track)]
		[IO(Export = true)]
		[Key(key = "RS_Track_ColorRightMiddle")]
		[Group]
		public StandardColor TrailColorRightMiddle { get; private set; } = new StandardColor();

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

		/*
		[Selector(ID = 100)]
		[IO(Export = true)]
		[Key(key = "ModelParameter_EnableFalloff")]
		public Value.Boolean EnableFalloff { get; private set; }

		[Selected(ID = 100, Value = 0)]
		[IO(Export = true)]
		public FalloffParameter FalloffParam { get; private set; }
		*/

		internal RendererValues(Path basepath)
		{
			Type = new Value.Enum<ParamaterType>(ParamaterType.Sprite);
			TextureUVType = new TextureUVTypeParameter();

			Sprite = new SpriteParamater(basepath);
			Ribbon = new RibbonParamater(basepath);
			Track = new TrackParameter(basepath);
			Ring = new RingParamater(basepath);
			Model = new ModelParamater(basepath);

			// EnableFalloff = new Value.Boolean(false);
			// FalloffParam = new FalloffParameter();
		}

		public class SpriteParamater
		{
			[Key(key = "RS_Sprite_RenderingOrder")]
			public Value.Enum<RenderingOrder> RenderingOrder { get; private set; }

			[Key(key = "RS_Sprite_Billboard")]
			public Value.Enum<BillboardType> Billboard { get; private set; }

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<AlphaBlendType> AlphaBlend { get; private set; }

			//[Key(key = "RS_Sprite_ColorAll")]
			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<StandardColorType> ColorAll { get; private set; } = new Value.Enum<StandardColorType>(StandardColorType.Fixed);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Color ColorAll_Fixed { get; private set; } = new Value.Color(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.ColorWithRandom ColorAll_Random { get; private set; } = new Value.ColorWithRandom(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorEasingParamater ColorAll_Easing { get; private set; } = new ColorEasingParamater();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorFCurveParameter ColorAll_FCurve { get; private set; } = new ColorFCurveParameter();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Gradient ColorAll_Gradient { get; private set; } = new Gradient();

			[Selector(ID = 1)]
			[Key(key = "RS_Sprite_Color")]
			public Value.Enum<ColorType> Color { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Key(key = "RS_Sprite_Color_Fixed_LL")]
			public Value.Color Color_Fixed_LL { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Key(key = "RS_Sprite_Color_Fixed_LR")]
			public Value.Color Color_Fixed_LR { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Key(key = "RS_Sprite_Color_Fixed_UL")]
			public Value.Color Color_Fixed_UL { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Key(key = "RS_Sprite_Color_Fixed_UR")]
			public Value.Color Color_Fixed_UR { get; private set; }

			[Selector(ID = 2)]
			[Key(key = "RS_Sprite_Position")]
			public Value.Enum<PositionType> Position { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Key(key = "RS_Sprite_Position_Fixed_LL")]
			public Value.Vector2D Position_Fixed_LL { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Key(key = "RS_Sprite_Position_Fixed_LR")]
			public Value.Vector2D Position_Fixed_LR { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Key(key = "RS_Sprite_Position_Fixed_UL")]
			public Value.Vector2D Position_Fixed_UL { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Key(key = "RS_Sprite_Position_Fixed_UR")]
			public Value.Vector2D Position_Fixed_UR { get; private set; }

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Path ColorTexture
			{
				get;
				private set;
			}

			public SpriteParamater(Path basepath)
			{
				RenderingOrder = new Value.Enum<Data.RenderingOrder>(Data.RenderingOrder.FirstCreatedInstanceIsFirst);

				Billboard = new Value.Enum<BillboardType>(BillboardType.Billboard);
				AlphaBlend = new Value.Enum<AlphaBlendType>(AlphaBlendType.Blend);

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
				ColorTexture = new Value.Path(basepath, new MultiLanguageString(""), true, "");
			}
			public enum ColorType : int
			{
				[Key(key = "RS_Sprite_ColorType_Default")]
				Default = 0,
				[Key(key = "RS_Sprite_ColorType_Fixed")]
				Fixed = 1,
			}

			public enum PositionType : int
			{
				[Key(key = "RS_Sprite_PositionType_Default")]
				Default = 0,
				[Key(key = "RS_Sprite_PositionType_Fixed")]
				Fixed = 1,
			}
		}

		public class RibbonParamater
		{
			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<AlphaBlendType> AlphaBlend { get; private set; }

			[Key(key = "RS_Ribbon_ViewpointDependent")]
			public Value.Boolean ViewpointDependent { get; private set; }

			[Selector(ID = 0)]
			[Key(key = "RS_Ribbon_ColorAll")]
			public Value.Enum<ColorAllType> ColorAll { get; private set; }

			[Selected(ID = 0, Value = 0)]
			public Value.Color ColorAll_Fixed { get; private set; }

			[Selected(ID = 0, Value = 1)]
			public Value.ColorWithRandom ColorAll_Random { get; private set; }

			[Selected(ID = 0, Value = 2)]
			[IO(Export = true)]
			public ColorEasingParamater ColorAll_Easing { get; private set; }

			[Selector(ID = 1)]
			[Key(key = "RS_Ribbon_Color")]
			public Value.Enum<ColorType> Color { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Key(key = "RS_Ribbon_Color_Fixed_L")]
			public Value.Color Color_Fixed_L { get; private set; }

			[Selected(ID = 1, Value = 1)]
			[Key(key = "RS_Ribbon_Color_Fixed_R")]
			public Value.Color Color_Fixed_R { get; private set; }

			[Selector(ID = 2)]
			[Key(key = "RS_Ribbon_Position")]
			public Value.Enum<PositionType> Position { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Key(key = "RS_Ribbon_Position_Fixed_L")]
			public Value.Float Position_Fixed_L { get; private set; }

			[Selected(ID = 2, Value = 1)]
			[Key(key = "RS_Ribbon_Position_Fixed_R")]
			public Value.Float Position_Fixed_R { get; private set; }

			[Key(key = "RS_Ribbon_SplineDivision")]
			public Value.Int SplineDivision
			{
				get;
				private set;
			}

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Path ColorTexture
			{
				get;
				private set;
			}

			public RibbonParamater(Path basepath)
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

				ColorTexture = new Value.Path(basepath, new MultiLanguageString("ImageFilter"), true, "");
			}

			public enum ColorAllType : int
			{
				[Key(key = "RS_Ribbon_ColorAllType_Fixed")]
				Fixed = 0,
				[Key(key = "RS_Ribbon_ColorAllType_Random")]
				Random = 1,
				[Key(key = "RS_Ribbon_ColorAllType_Easing")]
				Easing = 2,
			}

			public enum ColorType : int
			{
				[Key(key = "RS_Ribbon_ColorType_Default")]
				Default = 0,
				[Key(key = "RS_Ribbon_ColorType_Fixed")]
				Fixed = 1,
			}

			public enum PositionType : int
			{
				[Key(key = "RS_Ribbon_PositionType_Default")]
				Default = 0,
				[Key(key = "RS_Ribbon_PositionType_Fixed")]
				Fixed = 1,
			}
		}

		public class RingParamater
		{
			[Key(key = "RS_Ring_RingShape")]
			[IO(Export = true)]
			public RingShapeParameter RingShape { get; private set; }

			[Key(key = "RS_Ring_RenderingOrder")]
			public Value.Enum<RenderingOrder> RenderingOrder { get; private set; }

			[Key(key = "RS_Ring_Billboard")]
			public Value.Enum<BillboardType> Billboard { get; private set; }

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<AlphaBlendType> AlphaBlend { get; private set; }

			[Key(key = "RS_Ring_VertexCount")]
			public Value.Int VertexCount { get; private set; }

			/// <summary>
			/// for compatibility
			/// </summary>
			[Selector(ID = 0)]
			[Shown(Shown = false)]
			[IO(Export = false)]
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
			[Key(key = "RS_Ring_Outer")]
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
			[Key(key = "RS_Ring_Inner")]
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
			[Key(key = "RS_Ring_CenterRatio")]
			public Value.Enum<CenterRatioType> CenterRatio { get; private set; }

			[Selected(ID = 3, Value = 0)]
			public Value.Float CenterRatio_Fixed { get; private set; }

			[Selected(ID = 3, Value = 1)]
			public Value.FloatWithRandom CenterRatio_Random { get; private set; }

			[Selected(ID = 3, Value = 2)]
			[IO(Export = true)]
			[TreeNode(id = "CenterRatio_Easing", key = "Easing_Parameter", type = TreeNodeType.Small)]
			public FloatEasingParamater CenterRatio_Easing { get; private set; }

			[Selector(ID = 4)]
			[Key(key = "RS_Ring_OuterColor")]
			public Value.Enum<ColorType> OuterColor { get; private set; }

			[Selected(ID = 4, Value = 0)]
			public Value.Color OuterColor_Fixed { get; private set; }

			[Selected(ID = 4, Value = 1)]
			public Value.ColorWithRandom OuterColor_Random { get; private set; }

			[Selected(ID = 4, Value = 2)]
			[IO(Export = true)]
			public ColorEasingParamater OuterColor_Easing { get; private set; }

			[Selector(ID = 5)]
			[Key(key = "RS_Ring_CenterColor")]
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
			[Key(key = "RS_Ring_InnerColor")]
			public Value.Enum<ColorType> InnerColor { get; private set; }

			[Selected(ID = 6, Value = 0)]
			public Value.Color InnerColor_Fixed { get; private set; }

			[Selected(ID = 6, Value = 1)]
			public Value.ColorWithRandom InnerColor_Random { get; private set; }

			[Selected(ID = 6, Value = 2)]
			[IO(Export = true)]
			public ColorEasingParamater InnerColor_Easing { get; private set; }

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Path ColorTexture
			{
				get;
				private set;
			}

			public RingParamater(Path basepath)
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

				ColorTexture = new Value.Path(basepath, new MultiLanguageString("ImageFilter"), true, "");
			}

			/// <summary>
			/// for compatibility
			/// </summary>
			public enum ViewingAngleType : int
			{
				Fixed = 0,
				Random = 1,
				Easing = 2,
			}


			public enum LocationType : int
			{
				[Key(key = "RS_Ring_LocationType_Fixed")]
				Fixed = 0,
				[Key(key = "RS_Ring_LocationType_PVA")]
				PVA = 1,
				[Key(key = "RS_Ring_LocationType_Easing")]
				Easing = 2,
			}

			public class FixedLocation
			{
				[Key(key = "FixedLocation_Location")]
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
				[Key(key = "RS_Ring_CenterRatioType_Fixed")]
				Fixed = 0,
				[Key(key = "RS_Ring_CenterRatioType_Random")]
				Random = 1,
				[Key(key = "RS_Ring_CenterRatioType_Easing")]
				Easing = 2,
			}

			public class PVALocation
			{
				[Key(key = "RS_Ring_PVALocation_Location")]
				public Value.Vector2DWithRandom Location
				{
					get;
					private set;
				}

				[Key(key = "RS_Ring_PVALocation_Velocity")]
				public Value.Vector2DWithRandom Velocity
				{
					get;
					private set;
				}

				[Key(key = "RS_Ring_PVALocation_Acceleration")]
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
				[Key(key = "RS_Ring_ColorType_Fixed")]
				Fixed = 0,
				[Key(key = "RS_Ring_ColorType_Random")]
				Random = 1,
				[Key(key = "RS_Ring_ColorType_Easing")]
				Easing = 2,
			}
		}

		public class ModelParamater
		{
			const int ModelReferenceTypeID = 100;

			[Selector(ID = ModelReferenceTypeID)]
			public Value.Enum<ModelReferenceType> ModelReference
			{
				get;
				private set;
			} = new Enum<ModelReferenceType>(ModelReferenceType.File);

			[Selected(ID = 100, Value = (int)ModelReferenceType.File)]
			[Key(key = "RS_Model_Model")]
			public Value.PathForModel Model
			{
				get;
				private set;
			}

			[Selected(ID = 100, Value = (int)ModelReferenceType.ProceduralModel)]
			public ProceduralModelReference Reference { get; private set; } = new ProceduralModelReference();

			[Key(key = "RS_Model_Billboard")]
			public Value.Enum<BillboardType> Billboard { get; private set; }

			[Key(key = "RS_Model_Culling")]
			public Value.Enum<CullingValues> Culling { get; private set; }

			public ModelParamater(Value.Path basepath)
			{
				Model = new Value.PathForModel(basepath, new MultiLanguageString("ModelFilter"), true, "");

				Billboard = new Value.Enum<BillboardType>(BillboardType.Fixed);

				Culling = new Value.Enum<CullingValues>(Data.CullingValues.Front);
			}

			[IO(Export = false)]
			[Shown(Shown = false)]
			//[Key(key = "RS_Model_Color")]
			public Value.Enum<StandardColorType> Color { get; private set; } = new Value.Enum<StandardColorType>(StandardColorType.Fixed);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Color Color_Fixed { get; private set; } = new Value.Color(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.ColorWithRandom Color_Random { get; private set; } = new Value.ColorWithRandom(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorEasingParamater Color_Easing { get; private set; } = new ColorEasingParamater();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorFCurveParameter Color_FCurve { get; private set; } = new ColorFCurveParameter();
		}

		public class TrackParameter
		{
			[Selector(ID = 10)]
			[Key(key = "RS_Track_TrackSizeFor")]
			public Value.Enum<TrackSizeType> TrackSizeFor { get; private set; }

			[Selected(ID = 10, Value = 0)]
			public Value.Float TrackSizeFor_Fixed { get; private set; }

			[Selector(ID = 11)]
			[Key(key = "RS_Track_TrackSizeMiddle")]
			public Value.Enum<TrackSizeType> TrackSizeMiddle { get; private set; }

			[Selected(ID = 11, Value = 0)]
			public Value.Float TrackSizeMiddle_Fixed { get; private set; }

			[Selector(ID = 12)]
			[Key(key = "RS_Track_TrackSizeBack")]
			public Value.Enum<TrackSizeType> TrackSizeBack { get; private set; }

			[Selected(ID = 12, Value = 0)]
			public Value.Float TrackSizeBack_Fixed { get; private set; }

			[Key(key = "RS_Track_SplineDivision")]
			public Value.Int SplineDivision
			{
				get;
				private set;
			}

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<StandardColorType> ColorLeft { get; private set; } = new Value.Enum<StandardColorType>(StandardColorType.Fixed);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Color ColorLeft_Fixed { get; private set; } = new Value.Color(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.ColorWithRandom ColorLeft_Random { get; private set; } = new Value.ColorWithRandom(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorEasingParamater ColorLeft_Easing { get; private set; } = new ColorEasingParamater();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorFCurveParameter ColorLeft_FCurve { get; private set; } = new ColorFCurveParameter();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<StandardColorType> ColorLeftMiddle { get; private set; } = new Value.Enum<StandardColorType>(StandardColorType.Fixed);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Color ColorLeftMiddle_Fixed { get; private set; } = new Value.Color(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.ColorWithRandom ColorLeftMiddle_Random { get; private set; } = new Value.ColorWithRandom(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorEasingParamater ColorLeftMiddle_Easing { get; private set; } = new ColorEasingParamater();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorFCurveParameter ColorLeftMiddle_FCurve { get; private set; } = new ColorFCurveParameter();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<StandardColorType> ColorCenter { get; private set; } = new Value.Enum<StandardColorType>(StandardColorType.Fixed);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Color ColorCenter_Fixed { get; private set; } = new Value.Color(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.ColorWithRandom ColorCenter_Random { get; private set; } = new Value.ColorWithRandom(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorEasingParamater ColorCenter_Easing { get; private set; } = new ColorEasingParamater();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorFCurveParameter ColorCenter_FCurve { get; private set; } = new ColorFCurveParameter();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<StandardColorType> ColorCenterMiddle { get; private set; } = new Value.Enum<StandardColorType>(StandardColorType.Fixed);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Color ColorCenterMiddle_Fixed { get; private set; } = new Value.Color(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.ColorWithRandom ColorCenterMiddle_Random { get; private set; } = new Value.ColorWithRandom(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorEasingParamater ColorCenterMiddle_Easing { get; private set; } = new ColorEasingParamater();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorFCurveParameter ColorCenterMiddle_FCurve { get; private set; } = new ColorFCurveParameter();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<StandardColorType> ColorRight { get; private set; } = new Value.Enum<StandardColorType>(StandardColorType.Fixed);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Color ColorRight_Fixed { get; private set; } = new Value.Color(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.ColorWithRandom ColorRight_Random { get; private set; } = new Value.ColorWithRandom(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorEasingParamater ColorRight_Easing { get; private set; } = new ColorEasingParamater();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorFCurveParameter ColorRight_FCurve { get; private set; } = new ColorFCurveParameter();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Enum<StandardColorType> ColorRightMiddle { get; private set; } = new Value.Enum<StandardColorType>(StandardColorType.Fixed);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.Color ColorRightMiddle_Fixed { get; private set; } = new Value.Color(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public Value.ColorWithRandom ColorRightMiddle_Random { get; private set; } = new Value.ColorWithRandom(255, 255, 255, 255);

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorEasingParamater ColorRightMiddle_Easing { get; private set; } = new ColorEasingParamater();

			[IO(Export = false)]
			[Shown(Shown = false)]
			public ColorFCurveParameter ColorRightMiddle_FCurve { get; private set; } = new ColorFCurveParameter();


			public TrackParameter(Path basepath)
			{
				TrackSizeFor = new Value.Enum<TrackSizeType>(TrackSizeType.Fixed);
				TrackSizeFor_Fixed = new Value.Float(1, float.MaxValue, 0);

				TrackSizeMiddle = new Value.Enum<TrackSizeType>(TrackSizeType.Fixed);
				TrackSizeMiddle_Fixed = new Value.Float(1, float.MaxValue, 0);

				TrackSizeBack = new Value.Enum<TrackSizeType>(TrackSizeType.Fixed);
				TrackSizeBack_Fixed = new Value.Float(1, float.MaxValue, 0);

				SplineDivision = new Value.Int(1, int.MaxValue, 1);
			}
		}

		public enum BillboardType : int
		{
			[Key(key = "BillboardType_Billboard")]
			Billboard = 0,
			[Key(key = "BillboardType_RotatedBillboard")]
			RotatedBillboard = 3,
			[Key(key = "BillboardType_DirectionalBillboard")]
			DirectionalBillboard = 4,
			[Key(key = "BillboardType_YAxisFixed")]
			YAxisFixed = 1,
			[Key(key = "BillboardType_Fixed")]
			Fixed = 2,
		}

		public enum ParamaterType : int
		{
			[Key(key = "RS_ParameterType_None")]
			[Icon(code = "\xec20")]
			None = 0,

			[Key(key = "RS_ParameterType_Sprite")]
			[Icon(code = "\xec21")]
			Sprite = 2,

			[Key(key = "RS_ParameterType_Ribbon")]
			[Icon(code = "\xec22")]
			Ribbon = 3,

			[Key(key = "RS_ParameterType_Track")]
			[Icon(code = "\xec23")]
			Track = 6,

			[Key(key = "RS_ParameterType_Ring")]
			[Icon(code = "\xec24")]
			Ring = 4,

			[Key(key = "RS_ParameterType_Model")]
			[Icon(code = "\xec25")]
			Model = 5,
		}

	}
}