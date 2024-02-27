using Effekseer.Data.Group;
using Effekseer.Data.Value;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using static Effekseer.Data.RendererCommonValues;

namespace Effekseer.Data
{
	public class GpuParticlesValues
	{
		public class BasicParams
		{
			[Key(key = "GpuParticles_Basic_EmitCount")]
			public Value.IntWithInifinite EmitCount { get; private set; } = new Value.IntWithInifinite(value: 1000, min: 1);

			[Key(key = "GpuParticles_Basic_EmitPerFrame")]
			public Value.Int EmitPerFrame { get; private set; } = new Value.Int(value: 10, min: 0);

			[Key(key = "GpuParticles_Basic_EmitOffset")]
			public Value.Float EmitOffset { get; private set; } = new Value.Float(value: 0.0f, min: 0.0f);

			[Key(key = "GpuParticles_Basic_LifeTime")]
			public Value.FloatWithRandom LifeTime { get; private set; } = new Value.FloatWithRandom(value: 100.0f, min: 1.0f);
		}

		public class EmitShapeParams
		{
			public enum ShapeType : int
			{
				[Key(key = "GpuParticles_EmitShape_ShapeType_Point")]
				Point = 0,

				[Key(key = "GpuParticles_EmitShape_ShapeType_Line")]
				Line = 1,

				[Key(key = "GpuParticles_EmitShape_ShapeType_Circle")]
				Circle = 2,

				[Key(key = "GpuParticles_EmitShape_ShapeType_Sphere")]
				Sphere = 3,

				[Key(key = "GpuParticles_EmitShape_ShapeType_Model")]
				Model = 4,
			}

			[Selector(ID = 0)]
			[Key(key = "GpuParticles_EmitShape_Shape")]
			public Value.Enum<ShapeType> Shape { get; private set; } = new Value.Enum<ShapeType>();

			[Selected(ID = 0, Value = 1)]
			[Key(key = "GpuParticles_EmitShape_LineStart")]
			public Value.Vector3D LineStart { get; private set; } = new Value.Vector3D();

			[Selected(ID = 0, Value = 1)]
			[Key(key = "GpuParticles_EmitShape_LineEnd")]
			public Value.Vector3D LineEnd { get; private set; } = new Value.Vector3D();

			[Selected(ID = 0, Value = 1)]
			[Key(key = "GpuParticles_EmitShape_LineWidth")]
			public Value.Float LineWidth { get; private set; } = new Value.Float(value: 0.0f, min: 0.0f);

			[Selected(ID = 0, Value = 2)]
			[Key(key = "GpuParticles_EmitShape_CircleAxis")]
			public Value.Vector3D CircleAxis { get; private set; } = new Value.Vector3D(0.0f, 1.0f, 0.0f);

			[Selected(ID = 0, Value = 2)]
			[Key(key = "GpuParticles_EmitShape_CircleInner")]
			public Value.Float CircleInner { get; private set; } = new Value.Float(value: 1.0f, min: 0.0f);

			[Selected(ID = 0, Value = 2)]
			[Key(key = "GpuParticles_EmitShape_CircleOuter")]
			public Value.Float CircleOuter { get; private set; } = new Value.Float(value: 1.0f, min: 0.0f);

			[Selected(ID = 0, Value = 3)]
			[Key(key = "GpuParticles_EmitShape_SphereRadius")]
			public Value.Float SphereRadius { get; private set; } = new Value.Float(value: 1.0f, min: 0.0f);

			[Selected(ID = 0, Value = 4)]
			[Key(key = "GpuParticles_RenderShape_ModelPath")]
			public Value.PathForModel ModelPath { get; private set; } = null;

			[Selected(ID = 0, Value = 4)]
			[Key(key = "GpuParticles_EmitShape_ModelSize")]
			public Value.Float ModelSize { get; private set; } = new Value.Float(value: 1.0f, min: 0.0f);

			[Key(key = "GpuParticles_EmitShape_RotationApplied")]
			public Value.Boolean RotationApplied { get; private set; } = new(value: false);

			public EmitShapeParams(Value.Path basepath)
			{
				ModelPath = new Value.PathForModel(basepath, new MultiLanguageString("ModelFilter"), true, "");
			}
		}

		public class VelocityParams
		{
			[Key(key = "GpuParticles_Velocity_Direction")]
			public Value.Vector3D Direction { get; private set; } = new Value.Vector3D(0.0f, 0.0f, 1.0f);

			[Key(key = "GpuParticles_Velocity_Spread")]
			public Value.Float Spread { get; private set; } = new Value.Float(value: 15.0f, min: 0.0f, max: 180.0f);

			[Key(key = "GpuParticles_Velocity_InitialSpeed")]
			public Value.FloatWithRandom InitialSpeed { get; private set; } = new Value.FloatWithRandom(value: 1.0f);

			[Key(key = "GpuParticles_Velocity_Damping")]
			public Value.FloatWithRandom Damping { get; private set; } = new Value.FloatWithRandom(value: 0.0f, min: 0.0f);
		}

		public class RotationParams
		{
			[Key(key = "GpuParticles_Rotation_InitialAngle")]
			public Value.Vector3DWithRandom InitialAngle { get; private set; } = new Value.Vector3DWithRandom();

			[Key(key = "GpuParticles_Rotation_AngularVelocity")]
			public Value.Vector3DWithRandom AngularVelocity { get; private set; } = new Value.Vector3DWithRandom();
		}

		public class ScaleParams
		{
			public enum ParamaterType
			{
				[Key(key = "Scale_ParamaterType_Fixed")]
				Fixed = 0,
				//[Key(key = "Scale_ParamaterType_PVA")]
				//PVA = 1,
				[Key(key = "Scale_ParamaterType_Easing")]
				Easing = 2,
			}

			[Selector(ID = 0)]
			[Key(key = "GpuParticles_Scale_Type")]
			public Value.Enum<ParamaterType> Type { get; private set; } = new();

			public class FixedParams
			{
				[Key(key = "GpuParticles_Scale_SingleScale")]
				public Value.FloatWithRandom SingleScale { get; private set; } = new(1.0f);

				[Key(key = "GpuParticles_Scale_XYZScale")]
				public Value.Vector3DWithRandom XYZScale { get; private set; } = new(1.0f, 1.0f, 1.0f);

			}
			
			[IO(Export = true)]
			[Selected(ID = 0, Value = 0)]
			public FixedParams Fixed { get; private set; } = new();

			public class EasingParams
			{
				[Key(key = "GpuParticles_Scale_InitialSingleScale")]
				public Value.FloatWithRandom InitialSingleScale { get; private set; } = new(1.0f);

				[Key(key = "GpuParticles_Scale_InitialXYZScale")]
				public Value.Vector3DWithRandom InitialXYZScale { get; private set; } = new(1.0f, 1.0f, 1.0f);

				[Key(key = "GpuParticles_Scale_TerminalSingleScale")]
				public Value.FloatWithRandom TerminalSingleScale { get; private set; } = new(1.0f);

				[Key(key = "GpuParticles_Scale_TerminalXYZScale")]
				public Value.Vector3DWithRandom TerminalXYZScale { get; private set; } = new(1.0f, 1.0f, 1.0f);

				[Key(key = "Easing_StartSpeed")]
				public Value.Enum<EasingStart> StartSpeed { get; private set; } = new();

				[Key(key = "Easing_EndSpeed")]
				public Value.Enum<EasingEnd> EndSpeed { get; private set; } = new();
			}

			[IO(Export = true)]
			[Selected(ID = 0, Value = 2)]
			public EasingParams Easing { get; private set; } = new();
		}

		public class ForceParams
		{
			[Key(key = "GpuParticles_Force_Gravity")]
			public Value.Vector3D Gravity { get; private set; } = new Value.Vector3D();

			public class VortexParams
			{
				[Key(key = "GpuParticles_Force_Vortex_Rotation")]
				public Value.Float Rotation { get; private set; } = new Value.Float(0.0f);

				[Key(key = "GpuParticles_Force_Vortex_Attraction")]
				public Value.Float Attraction { get; private set; } = new Value.Float(0.0f);

				[Key(key = "GpuParticles_Force_Vortex_Center")]
				public Value.Vector3D Center { get; private set; } = new Value.Vector3D(0.0f, 0.0f, 0.0f);

				[Key(key = "GpuParticles_Force_Vortex_Axis")]
				public Value.Vector3D Axis { get; private set; } = new Value.Vector3D(0.0f, 1.0f, 0.0f);
			}

			[IO(Export = true)]
			[TreeNode(id = "GpuParticles_Force_Vortex", key = "GpuParticles_Force_Vortex", type = TreeNodeType.Small)]
			public VortexParams Vortex { get; private set; } = new VortexParams();

			public class TurbulenceParams
			{
				[Key(key = "GpuParticles_Force_Turbulence_Power")]
				public Value.Float Power { get; private set; } = new Value.Float(0.0f, min: 0.0f);

				[Key(key = "GpuParticles_Force_Turbulence_Seed")]
				public Value.Int Seed { get; private set; } = new Value.Int(0, min: 0);

				[Key(key = "GpuParticles_Force_Turbulence_Scale")]
				public Value.Float Scale { get; private set; } = new Value.Float(4.0f, min: 0.0f, step: 0.1f);

				[Key(key = "GpuParticles_Force_Turbulence_Octave")]
				public Value.Int Octave { get; private set; } = new Value.Int(1, max: 10, min: 1);
			}

			[IO(Export = true)]
			[TreeNode(id = "GpuParticles_Force_Turbulence", key = "GpuParticles_Force_Turbulence", type = TreeNodeType.Small)]
			public TurbulenceParams Turbulence { get; private set; } = new TurbulenceParams();
		}

		public class RenderBasicParams
		{
			[Key(key = "GpuParticles_RenderBasic_BlendType")]
			public Value.Enum<AlphaBlendType> BlendType { get; private set; } = new Value.Enum<AlphaBlendType>(AlphaBlendType.Blend);

			[Key(key = "GpuParticles_RenderBasic_ZWrite")]
			public Value.Boolean ZWrite { get; private set; } = new Value.Boolean(false);

			[Key(key = "GpuParticles_RenderBasic_ZTest")]
			public Value.Boolean ZTest { get; private set; } = new Value.Boolean(true);
		};

		public class RenderShapeParams
		{
			public enum ShapeType : int
			{
				[Key(key = "GpuParticles_RenderShape_ShapeType_Sprite")]
				Sprite = 0,

				[Key(key = "GpuParticles_RenderShape_ShapeType_Model")]
				Model = 1,

				[Key(key = "GpuParticles_RenderShape_ShapeType_Trail")]
				Trail = 2,
			}
			public enum BillboardType : int
			{
				[Key(key = "GpuParticles_RenderShape_BillboardType_RotatedBillboard")]
				RotatedBillboard = 0,

				[Key(key = "GpuParticles_RenderShape_BillboardType_DirectionalBillboard")]
				DirectionalBillboard = 1,

				[Key(key = "GpuParticles_RenderShape_BillboardType_YAxisFixed")]
				YAxisFixed = 2,

				[Key(key = "GpuParticles_RenderShape_BillboardType_Fixed")]
				Fixed = 3,
			}

			[Selector(ID = 0)]
			[Key(key = "GpuParticles_RenderShape_Shape")]
			public Value.Enum<ShapeType> Shape { get; private set; } = new Value.Enum<ShapeType>();

			[Selected(ID = 0, Value = 0)]
			[Key(key = "GpuParticles_RenderShape_SpriteBillboard")]
			public Value.Enum<BillboardType> SpriteBillboard { get; private set; } = new Value.Enum<BillboardType>();

			[Selected(ID = 0, Value = 1)]
			[Key(key = "GpuParticles_RenderShape_ModelPath")]
			public Value.PathForModel ModelPath { get; private set; } = null;

			[Selected(ID = 0, Value = 2)]
			[Key(key = "GpuParticles_RenderShape_TrailLength")]
			public Value.Int TrailLength { get; private set; } = new Value.Int(value: 16, max: 256, min: 2);

			[Key(key = "GpuParticles_RenderShape_Size")]
			public Value.Float ShapeSize { get; private set; } = new Value.Float(value: 1.0f, min: 0);

			public RenderShapeParams(Value.Path basepath)
			{
				ModelPath = new Value.PathForModel(basepath, new MultiLanguageString("ModelFilter"), true, "");
			}
		}

		public class RenderColorParams
		{
			[Key(key = "GpuParticles_RenderColor_ColorInherit")]
			public Value.Enum<ParentEffectType> ColorInheritType { get; private set; } = new Value.Enum<ParentEffectType>();

			[IO(Export = true)]
			[Group]
			[Key(key = "GpuParticles_RenderColor_ColorAll")]
			public StandardColor ColorAll { get; private set; } = new StandardColor();

			[Key(key = "GpuParticles_RenderColor_Emissive")]
			public Value.Float Emissive { get; private set; } = new Value.Float(value: 1.0f, min: 0);

			[Key(key = "GpuParticles_RenderColor_FadeIn")]
			public Value.Float FadeIn { get; private set; } = new Value.Float(value: 0.0f, min: 0.0f);

			[Key(key = "GpuParticles_RenderColor_FadeOut")]
			public Value.Float FadeOut { get; private set; } = new Value.Float(value: 0.0f, min: 0.0f);
		}

		public class RenderMaterialParams
		{
			public enum MaterialType : int
			{
				[Key(key = "GpuParticles_RenderMaterial_MaterialType_Unlit")]
				Unlit = 0,

				[Key(key = "GpuParticles_RenderMaterial_MaterialType_Lighting")]
				Lighting = 1,
			}

			[IO(Export = true)]
			[Selector(ID = 0)]
			[Key(key = "GpuParticles_RenderMaterial_Material")]
			public Value.Enum<MaterialType> Material { get; private set; } = new Value.Enum<MaterialType>();

			public class Texture
			{
				[Key(key = "GpuParticles_RenderMaterial_TexturePath")]
				public Value.PathForImage Path { get; private set; } = null;

				[Key(key = "GpuParticles_RenderMaterial_TextureFilter")]
				public Value.Enum<FilterType> Filter { get; private set; } = new Value.Enum<FilterType>(FilterType.Linear);

				[Key(key = "GpuParticles_RenderMaterial_TextureWrap")]
				public Value.Enum<WrapType> Wrap { get; private set; } = new Value.Enum<WrapType>(WrapType.Repeat);

				public Texture(Value.Path basepath)
				{
					Path = new Value.PathForImage(basepath, new MultiLanguageString("ModelFilter"), true, "");
				}
			}

			[IO(Export = true)]
			[Group]
			[Key(key = "GpuParticles_RenderMaterial_ColorTexture")]
			public Texture ColorTexture { get; private set; } = null;

			[IO(Export = true)]
			[Group]
			[Selected(ID = 0, Value = 1)]
			[Key(key = "GpuParticles_RenderMaterial_NormalTexture")]
			public Texture NormalTexture { get; private set; } = null;

			public RenderMaterialParams(Value.Path basepath)
			{
				ColorTexture = new Texture(basepath);
				NormalTexture = new Texture(basepath);
			}
		}

		[IO(Export = true)]
		[Selector(ID = 0)]
		[Key(key = "GpuParticles_Enabled")]
		public Value.Boolean Enabled { get; private set; } = new Value.Boolean();

		[IO(Export = true)]
		[Selected(ID = 0, Value = 1)]
		[TreeNode(id = "GpuParticles_Basic", key = "GpuParticles_Basic")]
		public BasicParams Basic { get; private set; } = new BasicParams();

		[IO(Export = true)]
		[Selected(ID = 0, Value = 1)]
		[TreeNode(id = "GpuParticles_EmitShape", key = "GpuParticles_EmitShape")]
		public EmitShapeParams EmitShape { get; private set; } = null;

		[IO(Export = true)]
		[Selected(ID = 0, Value = 1)]
		[TreeNode(id = "GpuParticles_Velocity", key = "GpuParticles_Velocity")]
		public VelocityParams Velocity { get; private set; } = new VelocityParams();

		[IO(Export = true)]
		[Selected(ID = 0, Value = 1)]
		[TreeNode(id = "GpuParticles_Rotation", key = "GpuParticles_Rotation")]
		public RotationParams Rotation { get; private set; } = new RotationParams();

		[IO(Export = true)]
		[Selected(ID = 0, Value = 1)]
		[TreeNode(id = "GpuParticles_Scale", key = "GpuParticles_Scale")]
		public ScaleParams Scale { get; private set; } = new ScaleParams();

		[IO(Export = true)]
		[Selected(ID = 0, Value = 1)]
		[TreeNode(id = "GpuParticles_Force", key = "GpuParticles_Force")]
		public ForceParams Force { get; private set; } = new ForceParams();

		[IO(Export = true)]
		[Selected(ID = 0, Value = 1)]
		[TreeNode(id = "GpuParticles_RenderBasic", key = "GpuParticles_RenderBasic")]
		public RenderBasicParams RenderBasic { get; private set; } = new RenderBasicParams();
		
		[IO(Export = true)]
		[Selected(ID = 0, Value = 1)]
		[TreeNode(id = "GpuParticles_RenderShape", key = "GpuParticles_RenderShape")]
		public RenderShapeParams RenderShape { get; private set; } = null;

		[IO(Export = true)]
		[Selected(ID = 0, Value = 1)]
		[TreeNode(id = "GpuParticles_RenderColor", key = "GpuParticles_RenderColor")]
		public RenderColorParams RenderColor { get; private set; } = new RenderColorParams();

		[IO(Export = true)]
		[Selected(ID = 0, Value = 1)]
		[TreeNode(id = "GpuParticles_RenderMaterial", key = "GpuParticles_RenderMaterial")]
		public RenderMaterialParams RenderMaterial { get; private set; } = null;

		public GpuParticlesValues(Value.Path basepath)
		{
			EmitShape = new EmitShapeParams(basepath);
			RenderShape = new RenderShapeParams(basepath);
			RenderMaterial = new RenderMaterialParams(basepath);
		}
	}
}
