

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Effect.h"
#include "Effekseer.EffectImplemented.h"

#include "Effekseer.EffectNode.h"
#include "Effekseer.Instance.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"
#include "Effekseer.InstanceGroup.h"
#include "Effekseer.Manager.h"
#include "Effekseer.ManagerImplemented.h"

#include "Effekseer.DefaultEffectLoader.h"
#include "Effekseer.TextureLoader.h"

#include "Effekseer.Setting.h"

#include "Renderer/Effekseer.SpriteRenderer.h"
#include "Renderer/Effekseer.RibbonRenderer.h"
#include "Renderer/Effekseer.RingRenderer.h"
#include "Renderer/Effekseer.ModelRenderer.h"
#include "Renderer/Effekseer.TrackRenderer.h"

#include "Effekseer.SoundLoader.h"
#include "Sound/Effekseer.SoundPlayer.h"

#include "Effekseer.ModelLoader.h"

#include <iostream>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static int64_t GetTime(void)
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

Manager::DrawParameter::DrawParameter()
{ 
	CameraCullingMask = 1; 
}

Manager* Manager::Create( int instance_max, bool autoFlip )
{
	return new ManagerImplemented( instance_max, autoFlip );
}

Matrix43* ManagerImplemented::DrawSet::GetEnabledGlobalMatrix()
{
	if (IsPreupdated)
	{
		InstanceContainer* pContainer = InstanceContainerPointer;
		if (pContainer == nullptr) return nullptr;

		auto firstGroup = pContainer->GetFirstGroup();
		if (firstGroup == nullptr) return nullptr;

		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();
		if (pInstance == nullptr) return nullptr;

		return &(pInstance->m_GlobalMatrix43);
	}
	else
	{
		return &(GlobalMatrix);
	}
}


void ManagerImplemented::DrawSet::CopyMatrixFromInstanceToRoot()
{
	if (IsPreupdated)
	{
		InstanceContainer* pContainer = InstanceContainerPointer;
		if (pContainer == nullptr) return;

		auto firstGroup = pContainer->GetFirstGroup();
		if (firstGroup == nullptr) return;

		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();
		if (pInstance == nullptr) return;

		GlobalMatrix = pInstance->m_GlobalMatrix43;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Handle ManagerImplemented::AddDrawSet( Effect* effect, InstanceContainer* pInstanceContainer, InstanceGlobal* pGlobalPointer )
{
	Handle Temp = m_NextHandle;

	if( ++m_NextHandle < 0 )
	{
		// オーバーフローしたらリセットする
		m_NextHandle = 0;
	}

	DrawSet drawset( effect, pInstanceContainer, pGlobalPointer );
	drawset.Self = Temp;

	ES_SAFE_ADDREF( effect );

	m_DrawSets[Temp] = drawset;

	return Temp;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::GCDrawSet( bool isRemovingManager )
{
	// インスタンスグループ自体の削除処理
	{
		std::map<Handle,DrawSet>::iterator it = m_RemovingDrawSets[1].begin();
		while( it != m_RemovingDrawSets[1].end() )
		{
			DrawSet& drawset = (*it).second;

			// dispose all instances
			if (drawset.InstanceContainerPointer != nullptr)
			{
				drawset.InstanceContainerPointer->RemoveForcibly(true);
				drawset.InstanceContainerPointer->~InstanceContainer();
				InstanceContainer::operator delete(drawset.InstanceContainerPointer, this);
			}

			ES_SAFE_RELEASE( drawset.ParameterPointer );
			ES_SAFE_DELETE( drawset.GlobalPointer );

			if(m_cullingWorld != NULL)
			{
				m_cullingWorld->RemoveObject(drawset.CullingObjectPointer);
				Culling3D::SafeRelease(drawset.CullingObjectPointer);
			}

			m_RemovingDrawSets[1].erase( it++ );
		}
		m_RemovingDrawSets[1].clear();
	}

	{
		std::map<Handle,DrawSet>::iterator it = m_RemovingDrawSets[0].begin();
		while( it != m_RemovingDrawSets[0].end() )
		{
			m_RemovingDrawSets[1][ (*it).first ] = (*it).second;
			m_RemovingDrawSets[0].erase( it++ );
		}
		m_RemovingDrawSets[0].clear();
	}

	{
		std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
		while( it != m_DrawSets.end() )
		{
			DrawSet& draw_set = (*it).second;

			// 削除フラグが立っている時
			bool isRemoving = draw_set.IsRemoving;

			// 何も存在しない時
			if( !isRemoving && draw_set.GlobalPointer->GetInstanceCount() == 0 )
			{
				isRemoving = true;
			}

			// ルートのみ存在し、既に新しく生成する見込みがないとき
			if( !isRemoving && draw_set.GlobalPointer->GetInstanceCount() == 1 )
			{
				InstanceContainer* pRootContainer = draw_set.InstanceContainerPointer;
				InstanceGroup* group = pRootContainer->GetFirstGroup();

				if( group )
				{
					Instance* pRootInstance = group->GetFirst();

					if( pRootInstance && pRootInstance->GetState() == INSTANCE_STATE_ACTIVE )
					{
						int maxcreate_count = 0;
						bool canRemoved = true;
						for( int i = 0; i < pRootInstance->m_pEffectNode->GetChildrenCount(); i++ )
						{
							auto child = (EffectNodeImplemented*) pRootInstance->m_pEffectNode->GetChild(i);

							if (pRootInstance->maxGenerationChildrenCount[i] > pRootInstance->m_generatedChildrenCount[i])
							{
								canRemoved = false;
								break;
							}
						}
					
						if (canRemoved)
						{
							// when a sound is not playing.
							if (!GetSoundPlayer() || !GetSoundPlayer()->CheckPlayingTag(draw_set.GlobalPointer))
							{
								isRemoving = true;
							}
						}
					}
				}
			}

			if( isRemoving )
			{
				// 消去処理
				StopEffect( (*it).first );

				if( (*it).second.RemovingCallback != NULL )
				{
					(*it).second.RemovingCallback( this, (*it).first, isRemovingManager );
				}

				m_RemovingDrawSets[0][ (*it).first ] = (*it).second;
				m_DrawSets.erase( it++ );
			}
			else
			{
				++it;
			}			
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceContainer* ManagerImplemented::CreateInstanceContainer( EffectNode* pEffectNode, InstanceGlobal* pGlobal, bool isRoot, const Matrix43& rootMatrix, Instance* pParent )
{
	InstanceContainer* pContainer = new(this) InstanceContainer(
		this,
		pEffectNode,
		pGlobal,
		pEffectNode->GetChildrenCount() );
	
	for (int i = 0; i < pEffectNode->GetChildrenCount(); i++)
	{
		pContainer->SetChild(i, CreateInstanceContainer(pEffectNode->GetChild(i), pGlobal, false, Matrix43(), nullptr));
	}

	if( isRoot )
	{
		pGlobal->SetRootContainer(pContainer);

		InstanceGroup* group = pContainer->CreateGroup();
		Instance* instance = group->CreateInstance();
		instance->Initialize( NULL, 0, 0, rootMatrix);

		// This group is not generated by an instance, so changed a flag
		group->IsReferencedFromInstance = false;
	}

	return pContainer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* EFK_STDCALL ManagerImplemented::Malloc( unsigned int size )
{
	return (void*)new char*[size];
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EFK_STDCALL ManagerImplemented::Free( void* p, unsigned int size )
{
	char* pData = (char*)p;
	delete [] pData;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int EFK_STDCALL ManagerImplemented::Rand()
{
	return rand();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::ExecuteEvents()
{	
	for (auto& ds : m_DrawSets)
	{
		if( ds.second.GoingToStop )
		{
			InstanceContainer* pContainer = ds.second.InstanceContainerPointer;

			if (pContainer != nullptr)
			{
				pContainer->KillAllInstances(true);
			}

			ds.second.IsRemoving = true;
			if (GetSoundPlayer() != NULL)
			{
				GetSoundPlayer()->StopTag(ds.second.GlobalPointer);
			}
		}

		if(ds.second.GoingToStopRoot )
		{
			InstanceContainer* pContainer = ds.second.InstanceContainerPointer;

			if (pContainer != nullptr)
			{
				pContainer->KillAllInstances(false);
			}
		}
	}
}

ManagerImplemented::ManagerImplemented( int instance_max, bool autoFlip )
	: m_autoFlip	( autoFlip )
	, m_NextHandle	( 0 )
	, m_instance_max	( instance_max )
	, m_setting			( NULL )
	, m_sequenceNumber	( 0 )

	, m_cullingWorld	(NULL)
	, m_culled(false)

	, m_spriteRenderer(NULL)
	, m_ribbonRenderer(NULL)
	, m_ringRenderer(NULL)
	, m_modelRenderer(NULL)
	, m_trackRenderer(NULL)

	, m_soundPlayer(NULL)

	, m_MallocFunc(NULL)
	, m_FreeFunc(NULL)
	, m_randFunc(NULL)
	, m_randMax(0)
{
	m_setting = Setting::Create();

	SetMallocFunc( Malloc );
	SetFreeFunc( Free );
	SetRandFunc( Rand );
	SetRandMax(RAND_MAX);

	m_renderingDrawSets.reserve( 64 );

	m_reserved_instances_buffer = new uint8_t[ sizeof(Instance) * m_instance_max ];

	for( int i = 0; i < m_instance_max; i++ )
	{
		Instance* instances = (Instance*)m_reserved_instances_buffer;
		m_reserved_instances.push( &instances[i] );
	}

	m_setting->SetEffectLoader(new DefaultEffectLoader());
	EffekseerPrintDebug("*** Create : Manager\n");
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ManagerImplemented::~ManagerImplemented()
{
	StopAllEffects();

	ExecuteEvents();

	for( int i = 0; i < 5; i++ )
	{
		GCDrawSet( true );
	}

	assert( m_reserved_instances.size() == m_instance_max ); 
	ES_SAFE_DELETE_ARRAY( m_reserved_instances_buffer );

	Culling3D::SafeRelease(m_cullingWorld);

	ES_SAFE_DELETE(m_spriteRenderer);
	ES_SAFE_DELETE(m_ribbonRenderer);
	ES_SAFE_DELETE(m_modelRenderer);
	ES_SAFE_DELETE(m_trackRenderer);
	ES_SAFE_DELETE(m_ringRenderer);

	ES_SAFE_DELETE(m_soundPlayer);

	ES_SAFE_RELEASE( m_setting );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::PushInstance( Instance* instance )
{
	m_reserved_instances.push( instance );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance* ManagerImplemented::PopInstance()
{
	if( m_reserved_instances.empty() )
	{
		return NULL;
	}

	Instance* ret = m_reserved_instances.front();
	m_reserved_instances.pop();
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::Destroy()
{
	StopAllEffects();

	ExecuteEvents();

	for( int i = 0; i < 5; i++ )
	{
		GCDrawSet( true );
	}

	Release();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
uint32_t ManagerImplemented::GetSequenceNumber() const
{
	return m_sequenceNumber;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
MallocFunc ManagerImplemented::GetMallocFunc() const
{
	return m_MallocFunc;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetMallocFunc(MallocFunc func)
{
	m_MallocFunc = func;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
FreeFunc ManagerImplemented::GetFreeFunc() const
{
	return m_FreeFunc;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetFreeFunc(FreeFunc func)
{
	m_FreeFunc = func;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RandFunc ManagerImplemented::GetRandFunc() const
{
	return m_randFunc;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRandFunc(RandFunc func)
{
	m_randFunc = func;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int ManagerImplemented::GetRandMax() const
{
	return m_randMax;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRandMax(int max_)
{
	m_randMax = max_;
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
CoordinateSystem ManagerImplemented::GetCoordinateSystem() const
{
	return m_setting->GetCoordinateSystem();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetCoordinateSystem( CoordinateSystem coordinateSystem )
{
	m_setting->SetCoordinateSystem(coordinateSystem);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SpriteRenderer* ManagerImplemented::GetSpriteRenderer()
{
	return m_spriteRenderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetSpriteRenderer(SpriteRenderer* renderer)
{
	ES_SAFE_DELETE(m_spriteRenderer);
	m_spriteRenderer = renderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer* ManagerImplemented::GetRibbonRenderer()
{
	return m_ribbonRenderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRibbonRenderer(RibbonRenderer* renderer)
{
	ES_SAFE_DELETE(m_ribbonRenderer);
	m_ribbonRenderer = renderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer* ManagerImplemented::GetRingRenderer()
{
	return m_ringRenderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRingRenderer(RingRenderer* renderer)
{
	ES_SAFE_DELETE(m_ringRenderer);
	m_ringRenderer = renderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer* ManagerImplemented::GetModelRenderer()
{
	return m_modelRenderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetModelRenderer(ModelRenderer* renderer)
{
	ES_SAFE_DELETE(m_modelRenderer);
	m_modelRenderer = renderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TrackRenderer* ManagerImplemented::GetTrackRenderer()
{
	return m_trackRenderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetTrackRenderer(TrackRenderer* renderer)
{
	ES_SAFE_DELETE(m_trackRenderer);
	m_trackRenderer = renderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundPlayer* ManagerImplemented::GetSoundPlayer()
{
	return m_soundPlayer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetSoundPlayer(SoundPlayer* soundPlayer)
{
	ES_SAFE_DELETE(m_soundPlayer);
	m_soundPlayer = soundPlayer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Setting* ManagerImplemented::GetSetting()
{
	return m_setting;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetSetting(Setting* setting)
{
	ES_SAFE_RELEASE(m_setting);
	m_setting = setting;
	ES_SAFE_ADDREF(m_setting);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectLoader* ManagerImplemented::GetEffectLoader()
{
	return m_setting->GetEffectLoader();
}
	
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetEffectLoader( EffectLoader* effectLoader )
{
	m_setting->SetEffectLoader(effectLoader);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader* ManagerImplemented::GetTextureLoader()
{
	return m_setting->GetTextureLoader();
}
	
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetTextureLoader( TextureLoader* textureLoader )
{
	m_setting->SetTextureLoader(textureLoader);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundLoader* ManagerImplemented::GetSoundLoader()
{
	return m_setting->GetSoundLoader();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetSoundLoader( SoundLoader* soundLoader )
{
	m_setting->SetSoundLoader(soundLoader);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader* ManagerImplemented::GetModelLoader()
{
	return m_setting->GetModelLoader();
}
	
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetModelLoader( ModelLoader* modelLoader )
{
	m_setting->SetModelLoader(modelLoader);
}

MaterialLoader* ManagerImplemented::GetMaterialLoader() { return m_setting->GetMaterialLoader(); }

void ManagerImplemented::SetMaterialLoader(MaterialLoader* loader) { m_setting->SetMaterialLoader(loader); }

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::StopEffect( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];
		drawSet.GoingToStop = true;
		drawSet.IsRemoving = true;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::StopAllEffects()
{
	std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
	while( it != m_DrawSets.end() )
	{
		(*it).second.GoingToStop = true;
		(*it).second.IsRemoving = true;
		++it;		
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::StopRoot( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].GoingToStopRoot = true;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::StopRoot( Effect* effect )
{
	std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
	std::map<Handle,DrawSet>::iterator it_end = m_DrawSets.end();

	while( it != it_end )
	{
		if( (*it).second.ParameterPointer == effect )
		{
			(*it).second.GoingToStopRoot = true;
		}
		++it;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool ManagerImplemented::Exists( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		// always exists before update
		if (!m_DrawSets[handle].IsPreupdated) return true;

		if( m_DrawSets[handle].IsRemoving ) return false;
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int32_t ManagerImplemented::GetInstanceCount( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		return m_DrawSets[handle].GlobalPointer->GetInstanceCount();
	}
	return 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Matrix43 ManagerImplemented::GetMatrix( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];

		auto mat = drawSet.GetEnabledGlobalMatrix();

		if (mat != nullptr)
		{
			return *mat;
		}
	}

	return Matrix43();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetMatrix( Handle handle, const Matrix43& mat )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];
		auto mat_ = drawSet.GetEnabledGlobalMatrix();

		if (mat_ != nullptr)
		{
			(*mat_) = mat;
			drawSet.CopyMatrixFromInstanceToRoot();
			drawSet.IsParameterChanged = true;
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Vector3D ManagerImplemented::GetLocation( Handle handle )
{
	Vector3D location;

	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];
		auto mat_ = drawSet.GetEnabledGlobalMatrix();

		if (mat_ != nullptr)
		{
			location.X = mat_->Value[3][0];
			location.Y = mat_->Value[3][1];
			location.Z = mat_->Value[3][2];
		}
	}

	return location;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetLocation( Handle handle, float x, float y, float z )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];
		auto mat_ = drawSet.GetEnabledGlobalMatrix();

		if (mat_ != nullptr)
		{
			mat_->Value[3][0] = x;
			mat_->Value[3][1] = y;
			mat_->Value[3][2] = z;

			drawSet.CopyMatrixFromInstanceToRoot();
			drawSet.IsParameterChanged = true;
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetLocation( Handle handle, const Vector3D& location )
{
	SetLocation( handle, location.X, location.Y, location.Z );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::AddLocation( Handle handle, const Vector3D& location )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];
		auto mat_ = drawSet.GetEnabledGlobalMatrix();

		if (mat_ != nullptr)
		{
			mat_->Value[3][0] += location.X;
			mat_->Value[3][1] += location.Y;
			mat_->Value[3][2] += location.Z;
			drawSet.CopyMatrixFromInstanceToRoot();
			drawSet.IsParameterChanged = true;
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRotation( Handle handle, float x, float y, float z )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];

		auto mat_ = drawSet.GetEnabledGlobalMatrix();

		if (mat_ != nullptr)
		{
			Matrix43 MatRotX, MatRotY, MatRotZ;

			MatRotX.RotationX(x);
			MatRotY.RotationY(y);
			MatRotZ.RotationZ(z);

			Matrix43 r;
			Vector3D s, t;

			mat_->GetSRT(s, r, t);

			r.Indentity();
			Matrix43::Multiple(r, r, MatRotZ);
			Matrix43::Multiple(r, r, MatRotX);
			Matrix43::Multiple(r, r, MatRotY);

			mat_->SetSRT(s, r, t);

			drawSet.CopyMatrixFromInstanceToRoot();
			drawSet.IsParameterChanged = true;
		}		
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRotation( Handle handle, const Vector3D& axis, float angle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];

		auto mat_ = drawSet.GetEnabledGlobalMatrix();

		if (mat_ != nullptr)
		{
			Matrix43 r;
			Vector3D s, t;

			mat_->GetSRT(s, r, t);

			r.RotationAxis(axis, angle);

			mat_->SetSRT(s, r, t);

			drawSet.CopyMatrixFromInstanceToRoot();
			drawSet.IsParameterChanged = true;
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetScale( Handle handle, float x, float y, float z )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];

		auto mat_ = drawSet.GetEnabledGlobalMatrix();

		if (mat_ != nullptr)
		{
			Matrix43 r;
			Vector3D s, t;

			mat_->GetSRT(s, r, t);

			s.X = x;
			s.Y = y;
			s.Z = z;

			mat_->SetSRT(s, r, t);

			drawSet.CopyMatrixFromInstanceToRoot();
			drawSet.IsParameterChanged = true;
		}
	}
}

void ManagerImplemented::SetAllColor(Handle handle, Color color)
{
	if (m_DrawSets.count(handle) > 0)
	{
		auto& drawSet = m_DrawSets[handle];

		drawSet.GlobalPointer->IsGlobalColorSet = true;
		drawSet.GlobalPointer->GlobalColor = color;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetTargetLocation( Handle handle, float x, float y, float z )
{
	SetTargetLocation( handle, Vector3D( x, y, z ) );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetTargetLocation( Handle handle, const Vector3D& location )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];

		InstanceGlobal* instanceGlobal = drawSet.GlobalPointer;
		instanceGlobal->SetTargetLocation( location );

		drawSet.IsParameterChanged = true;
	}
}

float ManagerImplemented::GetDynamicInput(Handle handle, int32_t index) 
{
	auto it = m_DrawSets.find(handle);
	if (it != m_DrawSets.end())
	{
		auto globalPtr = it->second.GlobalPointer;
		if (index < 0 || globalPtr->dynamicInputParameters.size() <= index)
			return 0.0f;

		return globalPtr->dynamicInputParameters[index];
	}

	return 0.0f;
}

void ManagerImplemented::SetDynamicInput(Handle handle, int32_t index, float value) {
	if (m_DrawSets.count(handle) > 0)
	{
		DrawSet& drawSet = m_DrawSets[handle];

		InstanceGlobal* instanceGlobal = drawSet.GlobalPointer;

		if (index < 0 || instanceGlobal->dynamicInputParameters.size() <= index)
			return;

		instanceGlobal->dynamicInputParameters[index] = value;

		drawSet.IsParameterChanged = true;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Matrix43 ManagerImplemented::GetBaseMatrix( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		return m_DrawSets[handle].BaseMatrix;
	}

	return Matrix43();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetBaseMatrix( Handle handle, const Matrix43& mat )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].BaseMatrix = mat;
		m_DrawSets[handle].DoUseBaseMatrix = true;
		m_DrawSets[handle].IsParameterChanged = true;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRemovingCallback( Handle handle, EffectInstanceRemovingCallback callback )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].RemovingCallback = callback;
	}
}

bool ManagerImplemented::GetShown(Handle handle)
{
	if (m_DrawSets.count(handle) > 0)
	{
		return m_DrawSets[handle].IsShown;
	}

	return false;
}

void ManagerImplemented::SetShown( Handle handle, bool shown )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].IsShown = shown;
	}
}

bool ManagerImplemented::GetPaused(Handle handle)
{
	if (m_DrawSets.count(handle) > 0)
	{
		return m_DrawSets[handle].IsPaused;
	}

	return false;
}

void ManagerImplemented::SetPaused( Handle handle, bool paused )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].IsPaused = paused;
	}
}

void ManagerImplemented::SetPausedToAllEffects(bool paused)
{
	std::map<Handle, DrawSet>::iterator it = m_DrawSets.begin();
	while (it != m_DrawSets.end())
	{
		(*it).second.IsPaused = paused;
		++it;
	}
}

int ManagerImplemented::GetLayer(Handle handle)
{
	if (m_DrawSets.count(handle) > 0)
	{
		return m_DrawSets[handle].Layer;
	}
	return 0;
}

void ManagerImplemented::SetLayer(Handle handle, int32_t layer)
{
	if (m_DrawSets.count(handle) > 0)
	{
		m_DrawSets[handle].Layer = layer;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float ManagerImplemented::GetSpeed(Handle handle) const
{
	auto it = m_DrawSets.find(handle);
	if (it == m_DrawSets.end()) return 0.0f;
	return it->second.Speed;
}


void ManagerImplemented::SetSpeed( Handle handle, float speed )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].Speed = speed;
		m_DrawSets[handle].IsParameterChanged = true;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetAutoDrawing( Handle handle, bool autoDraw )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].IsAutoDrawing = autoDraw;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::Flip()
{
	if( !m_autoFlip )
	{
		m_renderingMutex.lock();
	}

	// execute preupdate
	for (auto& drawSet : m_DrawSets)
	{
		Preupdate(drawSet.second);
	}

	ExecuteEvents();

	// DrawSet削除処理
	GCDrawSet( false );

	m_renderingDrawSets.clear();
	m_renderingDrawSetMaps.clear();

	/* カリング生成 */
	if( cullingNext.SizeX != cullingCurrent.SizeX ||
		cullingNext.SizeY != cullingCurrent.SizeY ||
		cullingNext.SizeZ != cullingCurrent.SizeZ ||
		cullingNext.LayerCount != cullingCurrent.LayerCount)
	{
		Culling3D::SafeRelease(m_cullingWorld);
		
		std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
		std::map<Handle,DrawSet>::iterator it_end = m_DrawSets.end();
		while( it != it_end )
		{
			DrawSet& ds = (*it).second;
			Culling3D::SafeRelease(ds.CullingObjectPointer);
			++it;
		}

		m_cullingWorld = Culling3D::World::Create(
			cullingNext.SizeX,
			cullingNext.SizeY,
			cullingNext.SizeZ,
			cullingNext.LayerCount);

		cullingCurrent = cullingNext;
	}

	{
		std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
		std::map<Handle,DrawSet>::iterator it_end = m_DrawSets.end();
		
		while( it != it_end )
		{
			DrawSet& ds = (*it).second;
			EffectImplemented* effect = (EffectImplemented*)ds.ParameterPointer;

			if(ds.IsParameterChanged)
			{
				if(m_cullingWorld != NULL)
				{
					if(ds.CullingObjectPointer == NULL)
					{
						ds.CullingObjectPointer = Culling3D::Object::Create();
						if(effect->Culling.Shape == CullingShape::Sphere)
						{
							ds.CullingObjectPointer->ChangeIntoSphere(0.0f);
						}

						if (effect->Culling.Shape == CullingShape::NoneShape)
						{
							ds.CullingObjectPointer->ChangeIntoAll();
						}
					}

					InstanceContainer* pContainer = ds.InstanceContainerPointer;
					Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

					Vector3D pos(
						ds.CullingObjectPointer->GetPosition().X,
						ds.CullingObjectPointer->GetPosition().Y,
						ds.CullingObjectPointer->GetPosition().Z);

					Matrix43 pos_;
					pos_.Translation(pos.X, pos.Y, pos.Z);

					Matrix43::Multiple(pos_, pos_,  pInstance->m_GlobalMatrix43);

					if(ds.DoUseBaseMatrix)
					{
						Matrix43::Multiple(pos_, pos_,  ds.BaseMatrix);
					}

					Culling3D::Vector3DF position;
					position.X = pos_.Value[3][0];
					position.Y = pos_.Value[3][1];
					position.Z = pos_.Value[3][2];
					ds.CullingObjectPointer->SetPosition(position);

					if(effect->Culling.Shape == CullingShape::Sphere)
					{
						float radius = effect->Culling.Sphere.Radius;

						{
							Vector3D s;
							pInstance->GetGlobalMatrix43().GetScale(s);
						
							radius = radius * sqrt(s.X * s.X + s.Y * s.Y + s.Z * s.Z);
						}

						if(ds.DoUseBaseMatrix)
						{
							Vector3D s;
							ds.BaseMatrix.GetScale(s);
						
							radius = radius * sqrt(s.X * s.X + s.Y * s.Y + s.Z * s.Z);
						}

						ds.CullingObjectPointer->ChangeIntoSphere(radius);
					}

					m_cullingWorld->AddObject(ds.CullingObjectPointer);
				}
				(*it).second.IsParameterChanged = false;
			}

			m_renderingDrawSets.push_back( (*it).second );
			m_renderingDrawSetMaps[(*it).first] = (*it).second;
			++it;
		}

		if(m_cullingWorld != NULL)
		{
			for(size_t i = 0; i < m_renderingDrawSets.size(); i++)
			{
				m_renderingDrawSets[i].CullingObjectPointer->SetUserData(&(m_renderingDrawSets[i]));
			}
		}
	}

	m_culledObjects.clear();
	m_culledObjectSets.clear();
	m_culled = false;

	if( !m_autoFlip )
	{
		m_renderingMutex.unlock();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::Update( float deltaFrame )
{
	BeginUpdate();

	// start to measure time
	int64_t beginTime = ::Effekseer::GetTime();

	for (auto& drawSet : m_DrawSets)
	{
		UpdateHandle(drawSet.second, deltaFrame);
	}

	// end to measure time
	m_updateTime = (int)(Effekseer::GetTime() - beginTime);

	EndUpdate();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::BeginUpdate()
{
	m_renderingMutex.lock();
	m_isLockedWithRenderingMutex = true;

	if( m_autoFlip )
	{
		Flip();
	}

	m_sequenceNumber++;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::EndUpdate()
{
	m_renderingMutex.unlock();
	m_isLockedWithRenderingMutex = false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::UpdateHandle( Handle handle, float deltaFrame )
{
	std::map<Handle,DrawSet>::iterator it = m_DrawSets.find( handle );
	if( it != m_DrawSets.end() )
	{
		DrawSet& drawSet = it->second;

		UpdateHandle( drawSet, deltaFrame );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::UpdateHandle( DrawSet& drawSet, float deltaFrame )
{
	// calculate dynamic parameters
	auto e = static_cast<EffectImplemented*>(drawSet.ParameterPointer);
	assert(e != nullptr);
	assert(drawSet.GlobalPointer->dynamicEqResults.size() >= e->dynamicEquation.size());

	std::array<float, 1> globals;
	globals[0] = drawSet.GlobalPointer->GetUpdatedFrame() / 60.0f;

	for (size_t i = 0; i < e->dynamicEquation.size(); i++)
	{
		if (e->dynamicEquation[i].GetRunningPhase() != InternalScript::RunningPhaseType::Global)
			continue;

		drawSet.GlobalPointer->dynamicEqResults[i] =
			e->dynamicEquation[i].Execute(
				drawSet.GlobalPointer->dynamicInputParameters,
				globals,
				std::array<float, 5>(),
				InstanceGlobal::Rand, 
				InstanceGlobal::RandSeed, 
				drawSet.GlobalPointer);
	}

	if (!drawSet.IsPreupdated)
	{
		Preupdate(drawSet);
	}

	float df = drawSet.IsPaused ? 0 : deltaFrame * drawSet.Speed;

	drawSet.InstanceContainerPointer->Update( true, df, drawSet.IsShown );

	if( drawSet.DoUseBaseMatrix )
	{
		drawSet.InstanceContainerPointer->SetBaseMatrix( true, drawSet.BaseMatrix );
	}

	drawSet.GlobalPointer->AddUpdatedFrame( df );
}


void ManagerImplemented::Preupdate(DrawSet& drawSet)
{
	if (drawSet.IsPreupdated) return;

	// Create an instance through a container
	InstanceContainer* pContainer = CreateInstanceContainer(drawSet.ParameterPointer->GetRoot(), drawSet.GlobalPointer, true, drawSet.GlobalMatrix, NULL);

	drawSet.InstanceContainerPointer = pContainer;

	for (int32_t frame = 0; frame < drawSet.StartFrame; frame++)
	{
		UpdateHandle(drawSet, 1);
	}

	drawSet.IsPreupdated = true;
}

bool ManagerImplemented::IsClippedWithDepth(DrawSet& drawSet, InstanceContainer* container, const Manager::DrawParameter& drawParameter) {
	
	// don't use this parameter
	if (container->m_pEffectNode->DepthValues.DepthParameter.DepthClipping > FLT_MAX / 10)
		return false;

	Vector3D pos;
	drawSet.GlobalMatrix.GetTranslation(pos);
	auto distance = Vector3D::Dot(drawParameter.CameraPosition - pos, drawParameter.CameraDirection);
	if (container->m_pEffectNode->DepthValues.DepthParameter.DepthClipping < distance)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ManagerImplemented::Draw(const Manager::DrawParameter& drawParameter)
{
	std::lock_guard<std::mutex> lock(m_renderingMutex);

	// 開始時間を記録
	int64_t beginTime = ::Effekseer::GetTime();

	if(m_culled)
	{
		for( size_t i = 0; i < m_culledObjects.size(); i++ )
		{
			DrawSet& drawSet = *m_culledObjects[i];

			if( drawSet.IsShown && drawSet.IsAutoDrawing && ((drawParameter.CameraCullingMask & (1 << drawSet.Layer)) != 0))
			{
				if (drawSet.GlobalPointer->RenderedInstanceContainers.size() > 0)
				{
					for (auto& c : drawSet.GlobalPointer->RenderedInstanceContainers)
					{
						if (IsClippedWithDepth(drawSet, c, drawParameter))
							continue;

						c->Draw(false);
					}
				}
				else
				{
					drawSet.InstanceContainerPointer->Draw( true );
				}
			}
		}
	}
	else
	{
		for( size_t i = 0; i < m_renderingDrawSets.size(); i++ )
		{
			DrawSet& drawSet = m_renderingDrawSets[i];

			if( drawSet.IsShown && drawSet.IsAutoDrawing && ((drawParameter.CameraCullingMask & (1 << drawSet.Layer)) != 0))
			{
				if (drawSet.GlobalPointer->RenderedInstanceContainers.size() > 0)
				{
					for (auto& c : drawSet.GlobalPointer->RenderedInstanceContainers)
					{
						if (IsClippedWithDepth(drawSet, c, drawParameter))
							continue;

						c->Draw(false);
					}
				}
				else
				{
					drawSet.InstanceContainerPointer->Draw(true);
				}
			}
		}
	}

	// 経過時間を計算
	m_drawTime = (int)(Effekseer::GetTime() - beginTime);
}

void ManagerImplemented::DrawBack(const Manager::DrawParameter& drawParameter)
{
	std::lock_guard<std::mutex> lock(m_renderingMutex);
	
	// 開始時間を記録
	int64_t beginTime = ::Effekseer::GetTime();

	if (m_culled)
	{
		for (size_t i = 0; i < m_culledObjects.size(); i++)
		{
			DrawSet& drawSet = *m_culledObjects[i];

			if (drawSet.IsShown && drawSet.IsAutoDrawing && ((drawParameter.CameraCullingMask & (1 << drawSet.Layer)) != 0))
			{
				auto e = (EffectImplemented*)drawSet.ParameterPointer;
				for (int32_t j = 0; j < e->renderingNodesThreshold; j++)
				{
					if (IsClippedWithDepth(drawSet, drawSet.GlobalPointer->RenderedInstanceContainers[j], drawParameter))
						continue;

					drawSet.GlobalPointer->RenderedInstanceContainers[j]->Draw(false);
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i < m_renderingDrawSets.size(); i++)
		{
			DrawSet& drawSet = m_renderingDrawSets[i];

			if (drawSet.IsShown && drawSet.IsAutoDrawing && ((drawParameter.CameraCullingMask & (1 << drawSet.Layer)) != 0))
			{
				auto e = (EffectImplemented*)drawSet.ParameterPointer;
				for (int32_t j = 0; j < e->renderingNodesThreshold; j++)
				{
					if (IsClippedWithDepth(drawSet, drawSet.GlobalPointer->RenderedInstanceContainers[j], drawParameter))
						continue;

					drawSet.GlobalPointer->RenderedInstanceContainers[j]->Draw(false);
				}
			}
		}
	}

	// 経過時間を計算
	m_drawTime = (int)(Effekseer::GetTime() - beginTime);
}

void ManagerImplemented::DrawFront(const Manager::DrawParameter& drawParameter)
{
	std::lock_guard<std::mutex> lock(m_renderingMutex);

	// 開始時間を記録
	int64_t beginTime = ::Effekseer::GetTime();

	if (m_culled)
	{
		for (size_t i = 0; i < m_culledObjects.size(); i++)
		{
			DrawSet& drawSet = *m_culledObjects[i];

			if (drawSet.IsShown && drawSet.IsAutoDrawing && ((drawParameter.CameraCullingMask & (1 << drawSet.Layer)) != 0))
			{
				if (drawSet.GlobalPointer->RenderedInstanceContainers.size() > 0)
				{
					auto e = (EffectImplemented*)drawSet.ParameterPointer;
					for (size_t j = e->renderingNodesThreshold; j < drawSet.GlobalPointer->RenderedInstanceContainers.size(); j++)
					{
						if (IsClippedWithDepth(drawSet, drawSet.GlobalPointer->RenderedInstanceContainers[j], drawParameter))
							continue;

						drawSet.GlobalPointer->RenderedInstanceContainers[j]->Draw(false);
					}
				}
				else
				{
					drawSet.InstanceContainerPointer->Draw(true);
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i < m_renderingDrawSets.size(); i++)
		{
			DrawSet& drawSet = m_renderingDrawSets[i];

			if (drawSet.IsShown && drawSet.IsAutoDrawing && ((drawParameter.CameraCullingMask & (1 << drawSet.Layer)) != 0))
			{
				if (drawSet.GlobalPointer->RenderedInstanceContainers.size() > 0)
				{
					auto e = (EffectImplemented*)drawSet.ParameterPointer;
					for (size_t j = e->renderingNodesThreshold; j < drawSet.GlobalPointer->RenderedInstanceContainers.size(); j++)
					{
						if (IsClippedWithDepth(drawSet, drawSet.GlobalPointer->RenderedInstanceContainers[j], drawParameter))
							continue;

						drawSet.GlobalPointer->RenderedInstanceContainers[j]->Draw(false);
					}
				}
				else
				{
					drawSet.InstanceContainerPointer->Draw(true);
				}
			}
		}
	}

	// 経過時間を計算
	m_drawTime = (int)(Effekseer::GetTime() - beginTime);
}

Handle ManagerImplemented::Play( Effect* effect, float x, float y, float z )
{
	return Play(effect, Vector3D(x, y, z), 0);
}

Handle ManagerImplemented::Play(Effect* effect, const Vector3D& position, int32_t startFrame)
{
	if (effect == nullptr) return -1;

	auto e = (EffectImplemented*)effect;

	// Create root
	InstanceGlobal* pGlobal = new InstanceGlobal();
	
	if (e->m_defaultRandomSeed >= 0)
	{
		pGlobal->SetSeed(e->m_defaultRandomSeed);
	}
	else
	{
		pGlobal->SetSeed(GetRandFunc()());
	}

	pGlobal->dynamicInputParameters = e->defaultDynamicInputs;

	pGlobal->RenderedInstanceContainers.resize(e->renderingNodesCount);
	for (size_t i = 0; i < pGlobal->RenderedInstanceContainers.size(); i++)
	{
		pGlobal->RenderedInstanceContainers[i] = nullptr;
	}

	// create a dateSet without an instance
	// an instance is created in Preupdate because effects need to show instances without update(0 frame)
	Handle handle = AddDrawSet(effect, nullptr, pGlobal);

	auto& drawSet = m_DrawSets[handle];

	drawSet.GlobalMatrix.Indentity();
	drawSet.GlobalMatrix.Value[3][0] = position.X;
	drawSet.GlobalMatrix.Value[3][1] = position.Y;
	drawSet.GlobalMatrix.Value[3][2] = position.Z;

	drawSet.IsParameterChanged = true;
	drawSet.StartFrame = startFrame;

	return handle;
}

int ManagerImplemented::GetCameraCullingMaskToShowAllEffects()
{
	int mask = 0;

	for (auto& ds : m_DrawSets)
	{
		auto layer = 1 << ds.second.Layer;
		mask |= layer;
	}

	return mask;
}

void ManagerImplemented::DrawHandle(Handle handle, const Manager::DrawParameter& drawParameter)
{
	std::lock_guard<std::mutex> lock(m_renderingMutex);

	std::map<Handle,DrawSet>::iterator it = m_renderingDrawSetMaps.find( handle );
	if( it != m_renderingDrawSetMaps.end() )
	{
		DrawSet& drawSet = it->second;

		if(m_culled)
		{
			if(m_culledObjectSets.find(drawSet.Self) !=m_culledObjectSets.end())
			{
				if( drawSet.IsShown )
				{
					if (drawSet.GlobalPointer->RenderedInstanceContainers.size() > 0)
					{
						for (auto& c : drawSet.GlobalPointer->RenderedInstanceContainers)
						{
							if (IsClippedWithDepth(drawSet, c, drawParameter))
								continue;

							c->Draw(false);
						}
					}
					else
					{
						drawSet.InstanceContainerPointer->Draw(true);
					}
				}
			}
		}
		else
		{
			if( drawSet.IsShown )
			{
				if (drawSet.GlobalPointer->RenderedInstanceContainers.size() > 0)
				{
					for (auto& c : drawSet.GlobalPointer->RenderedInstanceContainers)
					{
						if (IsClippedWithDepth(drawSet, c, drawParameter))
							continue;

						c->Draw(false);
					}
				}
				else
				{
					drawSet.InstanceContainerPointer->Draw(true);
				}
			}
		}
	}
}

void ManagerImplemented::DrawHandleBack(Handle handle, const Manager::DrawParameter& drawParameter)
{
	std::lock_guard<std::mutex> lock(m_renderingMutex);

	std::map<Handle, DrawSet>::iterator it = m_renderingDrawSetMaps.find(handle);
	if (it != m_renderingDrawSetMaps.end())
	{
		DrawSet& drawSet = it->second;

		if (m_culled)
		{
			if (m_culledObjectSets.find(drawSet.Self) != m_culledObjectSets.end())
			{
				if (drawSet.IsShown)
				{
					auto e = (EffectImplemented*)drawSet.ParameterPointer;
					for (int32_t i = 0; i < e->renderingNodesThreshold; i++)
					{
						if (IsClippedWithDepth(drawSet, drawSet.GlobalPointer->RenderedInstanceContainers[i], drawParameter))
							continue;

						drawSet.GlobalPointer->RenderedInstanceContainers[i]->Draw(false);
					}
				}
			}
		}
		else
		{
			if (drawSet.IsShown)
			{
				auto e = (EffectImplemented*)drawSet.ParameterPointer;
				for (int32_t i = 0; i < e->renderingNodesThreshold; i++)
				{
					if (IsClippedWithDepth(drawSet, drawSet.GlobalPointer->RenderedInstanceContainers[i], drawParameter))
						continue;

					drawSet.GlobalPointer->RenderedInstanceContainers[i]->Draw(false);
				}
			}
		}
	}
}

void ManagerImplemented::DrawHandleFront(Handle handle, const Manager::DrawParameter& drawParameter)
{
	std::lock_guard<std::mutex> lock(m_renderingMutex);

	std::map<Handle, DrawSet>::iterator it = m_renderingDrawSetMaps.find(handle);
	if (it != m_renderingDrawSetMaps.end())
	{
		DrawSet& drawSet = it->second;

		if (m_culled)
		{
			if (m_culledObjectSets.find(drawSet.Self) != m_culledObjectSets.end())
			{
				if (drawSet.IsShown)
				{
					if (drawSet.GlobalPointer->RenderedInstanceContainers.size() > 0)
					{
						auto e = (EffectImplemented*)drawSet.ParameterPointer;
						for (size_t i = e->renderingNodesThreshold; i < drawSet.GlobalPointer->RenderedInstanceContainers.size(); i++)
						{
							if (IsClippedWithDepth(drawSet, drawSet.GlobalPointer->RenderedInstanceContainers[i], drawParameter))
								continue;

							drawSet.GlobalPointer->RenderedInstanceContainers[i]->Draw(false);
						}
					}
					else
					{
						drawSet.InstanceContainerPointer->Draw(true);
					}
				}
			}
		}
		else
		{
			if (drawSet.IsShown)
			{
				if (drawSet.GlobalPointer->RenderedInstanceContainers.size() > 0)
				{
					auto e = (EffectImplemented*)drawSet.ParameterPointer;
					for (size_t i = e->renderingNodesThreshold; i < drawSet.GlobalPointer->RenderedInstanceContainers.size(); i++)
					{
						if (IsClippedWithDepth(drawSet, drawSet.GlobalPointer->RenderedInstanceContainers[i], drawParameter))
							continue;

						drawSet.GlobalPointer->RenderedInstanceContainers[i]->Draw(false);
					}
				}
				else
				{
					drawSet.InstanceContainerPointer->Draw(true);
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::BeginReloadEffect( Effect* effect, bool doLockThread)
{
	if (doLockThread)
	{
		m_renderingMutex.lock();
		m_isLockedWithRenderingMutex = true;
	}

	std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
	std::map<Handle,DrawSet>::iterator it_end = m_DrawSets.end();

	for(; it != it_end; ++it )
	{
		if( (*it).second.ParameterPointer != effect ) continue;
	
		/* インスタンス削除 */
		(*it).second.InstanceContainerPointer->RemoveForcibly( true );
		(*it).second.InstanceContainerPointer->~InstanceContainer();
		InstanceContainer::operator delete( (*it).second.InstanceContainerPointer, this );
		(*it).second.InstanceContainerPointer = NULL;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::EndReloadEffect( Effect* effect, bool doLockThread)
{
	std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
	std::map<Handle,DrawSet>::iterator it_end = m_DrawSets.end();

	for(; it != it_end; ++it )
	{
		if( (*it).second.ParameterPointer != effect ) continue;

		auto e = (EffectImplemented*)effect;
		auto pGlobal = (*it).second.GlobalPointer;

		// reallocate
		if (e->m_defaultRandomSeed >= 0)
		{
			pGlobal->SetSeed(e->m_defaultRandomSeed);
		}
		else
		{
			pGlobal->SetSeed(GetRandFunc()());
		}

		pGlobal->RenderedInstanceContainers.resize(e->renderingNodesCount);
		for (size_t i = 0; i < pGlobal->RenderedInstanceContainers.size(); i++)
		{
			pGlobal->RenderedInstanceContainers[i] = nullptr;
		}

		// Create an instance through a container
		(*it).second.InstanceContainerPointer = CreateInstanceContainer( ((EffectImplemented*)effect)->GetRoot(), (*it).second.GlobalPointer, true, (*it).second.GlobalMatrix, NULL );
		
		// skip
		for( float f = 0; f < (*it).second.GlobalPointer->GetUpdatedFrame() - 1; f+= 1.0f )
		{
			(*it).second.InstanceContainerPointer->Update( true, 1.0f, false );
		}

		(*it).second.InstanceContainerPointer->Update( true, 1.0f, (*it).second.IsShown );
	}

	if (doLockThread)
	{
		m_renderingMutex.unlock();
		m_isLockedWithRenderingMutex = false;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::CreateCullingWorld( float xsize, float ysize, float zsize, int32_t layerCount)
{
	cullingNext.SizeX = xsize;
	cullingNext.SizeY = ysize;
	cullingNext.SizeZ = zsize;
	cullingNext.LayerCount = layerCount;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::CalcCulling( const Matrix44& cameraProjMat, bool isOpenGL)
{
	if(m_cullingWorld == NULL) return;

	m_culledObjects.clear();
	m_culledObjectSets.clear();
	
	Matrix44 mat = cameraProjMat;
	mat.Transpose();

	Culling3D::Matrix44* mat_ = (Culling3D::Matrix44*)(&mat);

	m_cullingWorld->Culling(*mat_, isOpenGL);

	for(int32_t i = 0; i < m_cullingWorld->GetObjectCount(); i++)
	{
		Culling3D::Object* o = m_cullingWorld->GetObject(i);
		DrawSet* ds = (DrawSet*)o->GetUserData();

		m_culledObjects.push_back(ds);
		m_culledObjectSets.insert(ds->Self);
	}

	m_culled = true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::RessignCulling()
{
	if (m_cullingWorld == NULL) return;

	m_culledObjects.clear();
	m_culledObjectSets.clear();

	m_cullingWorld->Reassign();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
