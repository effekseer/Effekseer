
#ifndef	__EFFEKSEER_EFFECTNODE_H__
#define	__EFFEKSEER_EFFECTNODE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Color.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.RectF.h"
#include "Effekseer.InternalStruct.h"
#include "Effekseer.FCurves.h"
#include "Sound/Effekseer.SoundPlayer.h"

#include "Effekseer.Effect.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum class BindType : int32_t
{
	NotBind = 0,
	NotBind_Root = 3,
	WhenCreating = 1,
	Always = 2,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class StandardColorParameter
{
public:
	enum
	{
		Fixed = 0,
		Random = 1,
		Easing = 2,
		FCurve_RGBA = 3,
		Parameter_DWORD = 0x7fffffff,
	} type;

	union
	{
		struct
		{
			Color all;
		} fixed;

		struct
		{
			random_color all;
		} random;

		struct
		{
			easing_color all;
		} easing;

		struct
		{
			FCurveVectorColor* FCurve;
		} fcurve_rgba;
	};

	StandardColorParameter()
	{
		type = Fixed;
	}

	~StandardColorParameter()
	{
		if (type == FCurve_RGBA)
		{
			ES_SAFE_DELETE(fcurve_rgba.FCurve);
		}
	}

	void load(uint8_t*& pos, int32_t version)
	{
		memcpy(&type, pos, sizeof(int));
		pos += sizeof(int);

		if (type == Fixed)
		{
			memcpy(&fixed, pos, sizeof(fixed));
			pos += sizeof(fixed);
		}
		else if (type == Random)
		{
			random.all.load(version, pos);
		}
		else if (type == Easing)
		{
			easing.all.load(version, pos);
		}
		else if (type == FCurve_RGBA)
		{
			fcurve_rgba.FCurve = new FCurveVectorColor();
			int32_t size = fcurve_rgba.FCurve->Load(pos, version);
			pos += size;
		}
	}
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParameterCommonValues_8
{
	int MaxGeneration;
	BindType TranslationBindType;
	BindType RotationBindType;
	BindType ScalingBindType;
	int		RemoveWhenLifeIsExtinct;
	int		RemoveWhenParentIsRemoved;
	int		RemoveWhenChildrenIsExtinct;
	random_int	life;
	float GenerationTime;
	float GenerationTimeOffset;
};

struct ParameterCommonValues
{
	int MaxGeneration;
	BindType TranslationBindType;
	BindType RotationBindType;
	BindType ScalingBindType;
	int		RemoveWhenLifeIsExtinct;
	int		RemoveWhenParentIsRemoved;
	int		RemoveWhenChildrenIsExtinct;
	random_int	life;
	random_float GenerationTime;
	random_float GenerationTimeOffset;
};

struct ParameterDepthValues
{
	float	DepthOffset;
	bool	IsDepthOffsetScaledWithCamera;
	bool	IsDepthOffsetScaledWithParticleScale;
	ZSortType	ZSort;
	int32_t	DrawingPriority;
	float	SoftParticle;

	ParameterDepthValues()
	{
		DepthOffset = 0;
		IsDepthOffsetScaledWithCamera = false;
		IsDepthOffsetScaledWithParticleScale = false;
		ZSort = ZSortType::None;
		DrawingPriority = 0;
		SoftParticle = 0.0f;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ParameterTranslationType
{
	ParameterTranslationType_Fixed = 0,
	ParameterTranslationType_PVA = 1,
	ParameterTranslationType_Easing = 2,
	ParameterTranslationType_FCurve = 3,

	ParameterTranslationType_None = 0x7fffffff - 1,

	ParameterTranslationType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParameterTranslationFixed
{
	Vector3D Position;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParameterTranslationPVA
{
	random_vector3d	location;
	random_vector3d	velocity;
	random_vector3d	acceleration;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum class LocationAbsType : int32_t
{
	None = 0,
	Gravity = 1,
	AttractiveForce = 2,
};

struct LocationAbsParameter
{
	LocationAbsType type = LocationAbsType::None;

	union
	{
		struct
		{

		} none;

		vector3d	gravity;

		struct {
			float	force;
			float	control;
			float	minRange;
			float	maxRange;
		} attractiveForce;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ParameterRotationType
{
	ParameterRotationType_Fixed = 0,
	ParameterRotationType_PVA = 1,
	ParameterRotationType_Easing = 2,
	ParameterRotationType_AxisPVA = 3,
	ParameterRotationType_AxisEasing = 4,

	ParameterRotationType_FCurve = 5,

	ParameterRotationType_None = 0x7fffffff - 1,

	ParameterRotationType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParameterRotationFixed
{
	Vector3D Position;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParameterRotationPVA
{
	random_vector3d	rotation;
	random_vector3d	velocity;
	random_vector3d	acceleration;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParameterRotationAxisPVA
{
	random_vector3d	axis;
	random_float	rotation;
	random_float	velocity;
	random_float	acceleration;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParameterRotationAxisEasing
{
	random_vector3d	axis;
	easing_float	easing;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ParameterScalingType
{
	ParameterScalingType_Fixed = 0,
	ParameterScalingType_PVA = 1,
	ParameterScalingType_Easing = 2,
	ParameterScalingType_SinglePVA = 3,
	ParameterScalingType_SingleEasing = 4,
	ParameterScalingType_FCurve = 5,

	ParameterScalingType_None = 0x7fffffff - 1,

	ParameterScalingType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParameterScalingFixed
{
	Vector3D Position;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParameterScalingPVA
{
	random_vector3d Position;
	random_vector3d Velocity;
	random_vector3d Acceleration;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParameterScalingSinglePVA
{
	random_float Position;
	random_float Velocity;
	random_float Acceleration;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParameterGenerationLocation
{
	int	EffectsRotation;

	enum class AxisType : int32_t
	{
		X,
		Y,
		Z,
	};

	enum
	{
		TYPE_POINT = 0,
		TYPE_SPHERE = 1,
		TYPE_MODEL = 2,
		TYPE_CIRCLE = 3,
		TYPE_LINE = 4,

		TYPE_DWORD = 0x7fffffff,
	} type;

	enum eModelType
	{
		MODELTYPE_RANDOM = 0,
		MODELTYPE_VERTEX = 1,
		MODELTYPE_VERTEX_RANDOM = 2,
		MODELTYPE_FACE = 3,
		MODELTYPE_FACE_RANDOM = 4,

		MODELTYPE_DWORD = 0x7fffffff,
	};

	enum eCircleType
	{
		CIRCLE_TYPE_RANDOM = 0,
		CIRCLE_TYPE_ORDER = 1,
		CIRCLE_TYPE_REVERSE_ORDER = 2,
	};

	enum class LineType : int32_t
	{
		Random = 0,
		Order = 1,
	};

	union
	{
		struct
		{
			random_vector3d location;
		} point;

		struct
		{
			random_float radius;
			random_float rotation_x;
			random_float rotation_y;
		} sphere;

		struct
		{
			int32_t		index;
			eModelType	type;
		} model;

		struct
		{
			int32_t			division;
			random_float	radius;
			random_float	angle_start;
			random_float	angle_end;
			eCircleType		type;
			AxisType		axisDirection;
			random_float	angle_noize;
		} circle;

		struct
		{
			int32_t			division;
			random_vector3d	position_start;
			random_vector3d	position_end;
			random_float	position_noize;
			LineType		type;
		} line;
	};

	void load(uint8_t*& pos, int32_t version)
	{
		memcpy(&EffectsRotation, pos, sizeof(int));
		pos += sizeof(int);

		memcpy(&type, pos, sizeof(int));
		pos += sizeof(int);

		if (type == TYPE_POINT)
		{
			memcpy(&point, pos, sizeof(point));
			pos += sizeof(point);
		}
		else if (type == TYPE_SPHERE)
		{
			memcpy(&sphere, pos, sizeof(sphere));
			pos += sizeof(sphere);
		}
		else if (type == TYPE_MODEL)
		{
			memcpy(&model, pos, sizeof(model));
			pos += sizeof(model);
		}
		else if (type == TYPE_CIRCLE)
		{
			if (version < 10)
			{
				memcpy(&circle, pos, sizeof(circle) - sizeof(circle.axisDirection) - sizeof(circle.angle_noize));
				pos += sizeof(circle) - sizeof(circle.axisDirection) - sizeof(circle.angle_noize);
				circle.axisDirection = AxisType::Z;
				circle.angle_noize.max = 0;
				circle.angle_noize.min = 0;
			}
			else
			{
				memcpy(&circle, pos, sizeof(circle));
				pos += sizeof(circle);
			}
		}
		else if (type == TYPE_LINE)
		{
			memcpy(&line, pos, sizeof(line));
			pos += sizeof(line);
		}
	}
};

struct ParameterRendererCommon
{
	int32_t				ColorTextureIndex;
	AlphaBlendType		AlphaBlend;

	TextureFilterType	FilterType;

	TextureWrapType		WrapType;

	bool				ZWrite;

	bool				ZTest;

	bool				Distortion;

	float				DistortionIntensity;

	BindType			ColorBindType;

	enum
	{
		FADEIN_ON = 1,
		FADEIN_OFF = 0,

		FADEIN_DWORD = 0x7fffffff,
	} FadeInType;

	struct
	{
		float	Frame;
		easing_float_without_random	Value;
	} FadeIn;

	enum
	{
		FADEOUT_ON = 1,
		FADEOUT_OFF = 0,

		FADEOUT_DWORD = 0x7fffffff,
	} FadeOutType;

	struct
	{
		float	Frame;
		easing_float_without_random	Value;
	} FadeOut;

	enum
	{
		UV_DEFAULT = 0,
		UV_FIXED = 1,
		UV_ANIMATION = 2,
		UV_SCROLL = 3,
		UV_FCURVE = 4,

		UV_DWORD = 0x7fffffff,
	} UVType;


	/**
	@brief	UV Parameter
	@note
	for Compatibility
	*/
	struct UVScroll_09
	{
		rectf		Position;
		vector2d	Speed;
	};

	union
	{
		struct
		{
		} Default;

		struct
		{
			rectf	Position;
		} Fixed;

		struct
		{
			rectf	Position;
			int32_t	FrameLength;
			int32_t	FrameCountX;
			int32_t	FrameCountY;

			enum
			{
				LOOPTYPE_ONCE = 0,
				LOOPTYPE_LOOP = 1,
				LOOPTYPE_REVERSELOOP = 2,

				LOOPTYPE_DWORD = 0x7fffffff,
			} LoopType;

			random_int	StartFrame;

		} Animation;

		struct
		{
			random_vector2d	Position;
			random_vector2d	Size;
			random_vector2d	Speed;
		} Scroll;

		struct
		{
			FCurveVector2D* Position;
			FCurveVector2D* Size;
		} FCurve;

	} UV;

	void reset()
	{
		memset(this, 0, sizeof(ParameterRendererCommon));
	}

	void load(uint8_t*& pos, int32_t version)
	{
		memset(this, 0, sizeof(ParameterRendererCommon));

		memcpy(&ColorTextureIndex, pos, sizeof(int));
		pos += sizeof(int);

		memcpy(&AlphaBlend, pos, sizeof(int));
		pos += sizeof(int);

		memcpy(&FilterType, pos, sizeof(int));
		pos += sizeof(int);

		memcpy(&WrapType, pos, sizeof(int));
		pos += sizeof(int);

		if (version >= 5)
		{
			int32_t zwrite, ztest = 0;

			memcpy(&ztest, pos, sizeof(int32_t));
			pos += sizeof(int32_t);

			memcpy(&zwrite, pos, sizeof(int32_t));
			pos += sizeof(int32_t);

			ZWrite = zwrite != 0;
			ZTest = ztest != 0;
		}
		else
		{
			ZWrite = false;
			ZTest = true;
		}

		memcpy(&FadeInType, pos, sizeof(int));
		pos += sizeof(int);

		if (FadeInType == FADEIN_ON)
		{
			memcpy(&FadeIn, pos, sizeof(FadeIn));
			pos += sizeof(FadeIn);
		}

		memcpy(&FadeOutType, pos, sizeof(int));
		pos += sizeof(int);

		if (FadeOutType == FADEOUT_ON)
		{
			memcpy(&FadeOut, pos, sizeof(FadeOut));
			pos += sizeof(FadeOut);
		}

		memcpy(&UVType, pos, sizeof(int));
		pos += sizeof(int);

		if (UVType == UV_DEFAULT)
		{
		}
		else if (UVType == UV_FIXED)
		{
			memcpy(&UV.Fixed, pos, sizeof(UV.Fixed));
			pos += sizeof(UV.Fixed);
		}
		else if (UVType == UV_ANIMATION)
		{
			if (version < 10)
			{
				// without start frame
				memcpy(&UV.Animation, pos, sizeof(UV.Animation) - sizeof(UV.Animation.StartFrame));
				pos += sizeof(UV.Animation) - sizeof(UV.Animation.StartFrame);
				UV.Animation.StartFrame.max = 0;
				UV.Animation.StartFrame.min = 0;
			}
			else
			{
				memcpy(&UV.Animation, pos, sizeof(UV.Animation));
				pos += sizeof(UV.Animation);
			}
		}
		else if (UVType == UV_SCROLL)
		{
			if (version < 10)
			{
				// compatibility
				UVScroll_09 values;
				memcpy(&values, pos, sizeof(values));
				pos += sizeof(values);
				UV.Scroll.Position.max.x = values.Position.x;
				UV.Scroll.Position.max.y = values.Position.y;
				UV.Scroll.Position.min = UV.Scroll.Position.max;

				UV.Scroll.Size.max.x = values.Position.w;
				UV.Scroll.Size.max.y = values.Position.h;
				UV.Scroll.Size.min = UV.Scroll.Size.max;

				UV.Scroll.Speed.max.x = values.Speed.x;
				UV.Scroll.Speed.max.y = values.Speed.y;
				UV.Scroll.Speed.min = UV.Scroll.Speed.max;

			}
			else
			{
				memcpy(&UV.Scroll, pos, sizeof(UV.Scroll));
				pos += sizeof(UV.Scroll);
			}
		}
		else if (UVType == UV_FCURVE)
		{
			UV.FCurve.Position = new FCurveVector2D();
			UV.FCurve.Size = new FCurveVector2D();
			pos += UV.FCurve.Position->Load(pos, version);
			pos += UV.FCurve.Size->Load(pos, version);
		}

		if (version >= 10)
		{
			memcpy(&ColorBindType, pos, sizeof(int32_t));
			pos += sizeof(int32_t);
		}
		else
		{
			ColorBindType = BindType::NotBind;
		}

		if (version >= 9)
		{
			int32_t distortion = 0;

			memcpy(&distortion, pos, sizeof(int32_t));
			pos += sizeof(int32_t);

			Distortion = distortion > 0;

			memcpy(&DistortionIntensity, pos, sizeof(float));
			pos += sizeof(float);

		}
	}

	void destroy()
	{
		if (UVType == UV_FCURVE)
		{
			ES_SAFE_DELETE(UV.FCurve.Position);
			ES_SAFE_DELETE(UV.FCurve.Size);
		}
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ParameterSoundType
{
	ParameterSoundType_None = 0,
	ParameterSoundType_Use = 1,

	ParameterSoundType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ParameterSoundPanType
{
	ParameterSoundPanType_2D = 0,
	ParameterSoundPanType_3D = 1,

	ParameterSoundPanType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParameterSound
{
	int32_t			WaveId;
	random_float	Volume;
	random_float	Pitch;
	ParameterSoundPanType PanType;
	random_float	Pan;
	float			Distance;
	random_int		Delay;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum eRenderingOrder
{
	RenderingOrder_FirstCreatedInstanceIsFirst = 0,
	RenderingOrder_FirstCreatedInstanceIsLast = 1,

	RenderingOrder_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
@brief	ノードインスタンス生成クラス
@note
エフェクトのノードの実体を生成する。
*/
class EffectNodeImplemented
	: public EffectNode
{
	friend class Manager;
	friend class EffectImplemented;
	friend class Instance;

protected:
	// 所属しているパラメーター
	Effect*	m_effect;

	// 子ノード
	std::vector<EffectNodeImplemented*>	m_Nodes;

	// ユーザーデータ
	void* m_userData;

	// コンストラクタ
	EffectNodeImplemented(Effect* effect, unsigned char*& pos);

	// デストラクタ
	virtual ~EffectNodeImplemented();

	// 読込
	void LoadParameter(unsigned char*& pos, EffectNode* parent, Setting* setting);

	// 初期化
	void Initialize();

public:

	/**
	@brief	\~english Whether to draw the node.
	\~japanese このノードを描画するか?

	@note
	\~english 普通は描画されないノードは、描画の種類が変更されて、描画しないノードになる。ただし、色の継承をする場合、描画のみを行わないノードになる。
	\~japanese For nodes that are not normally rendered, the rendering type is changed to become a node that does not render. However, when color inheritance is done, it becomes a node which does not perform drawing only.
	*/
	bool IsRendered;

	ParameterCommonValues		CommonValues;

	ParameterTranslationType	TranslationType;
	ParameterTranslationFixed	TranslationFixed;
	ParameterTranslationPVA		TranslationPVA;
	easing_vector3d				TranslationEasing;
	FCurveVector3D*				TranslationFCurve;

	LocationAbsParameter		LocationAbs;

	ParameterRotationType		RotationType;
	ParameterRotationFixed		RotationFixed;
	ParameterRotationPVA		RotationPVA;
	easing_vector3d				RotationEasing;
	FCurveVector3D*				RotationFCurve;

	ParameterRotationAxisPVA	RotationAxisPVA;
	ParameterRotationAxisEasing	RotationAxisEasing;

	ParameterScalingType		ScalingType;
	ParameterScalingFixed		ScalingFixed;
	ParameterScalingPVA			ScalingPVA;
	easing_vector3d				ScalingEasing;
	ParameterScalingSinglePVA	ScalingSinglePVA;
	easing_float				ScalingSingleEasing;
	FCurveVector3D*				ScalingFCurve;

	ParameterGenerationLocation	GenerationLocation;

	ParameterDepthValues		DepthValues;

	ParameterRendererCommon		RendererCommon;

	ParameterSoundType			SoundType;
	ParameterSound				Sound;

	eRenderingOrder				RenderingOrder;

	int32_t						RenderingPriority = -1;

	Effect* GetEffect() const override;

	int GetChildrenCount() const override;

	EffectNode* GetChild(int index) const override;

	EffectBasicRenderParameter GetBasicRenderParameter() override;

	void SetBasicRenderParameter(EffectBasicRenderParameter param) override;

	EffectModelParameter GetEffectModelParameter() override;

	/**
	@brief	描画部分の読込
	*/
	virtual void LoadRendererParameter(unsigned char*& pos, Setting* setting);

	/**
	@brief	描画開始
	*/
	virtual void BeginRendering(int32_t count, Manager* manager);

	/**
	@brief	グループ描画開始
	*/
	virtual void BeginRenderingGroup(InstanceGroup* group, Manager* manager);

	virtual void EndRenderingGroup(InstanceGroup* group, Manager* manager);

	/**
	@brief	描画
	*/
	virtual void Rendering(const Instance& instance, const Instance* next_instance, Manager* manager);

	/**
	@brief	描画終了
	*/
	virtual void EndRendering(Manager* manager);

	/**
	@brief	インスタンスグループ描画時初期化
	*/
	virtual void InitializeRenderedInstanceGroup(InstanceGroup& instanceGroup, Manager* manager);

	/**
	@brief	描画部分初期化
	*/
	virtual void InitializeRenderedInstance(Instance& instance, Manager* manager);

	/**
	@brief	描画部分更新
	*/
	virtual void UpdateRenderedInstance(Instance& instance, Manager* manager);

	/**
	@brief	描画部分更新
	*/
	virtual float GetFadeAlpha(const Instance& instance);

	/**
	@brief	サウンド再生
	*/
	virtual void PlaySound_(Instance& instance, SoundTag tag, Manager* manager);

	/**
	@brief	エフェクトノード生成
	*/
	static EffectNodeImplemented* Create(Effect* effect, EffectNode* parent, unsigned char*& pos);

	/**
	@brief	ノードの種類取得
	*/
	virtual eEffectNodeType GetType() const { return EFFECT_NODE_TYPE_NONE; }
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECTNODE_H__