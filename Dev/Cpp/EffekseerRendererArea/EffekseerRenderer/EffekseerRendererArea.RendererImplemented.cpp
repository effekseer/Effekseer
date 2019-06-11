#include "EffekseerRendererArea.RendererImplemented.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h"
#include "EffekseerRendererArea.IndexBuffer.h"
#include "EffekseerRendererArea.RenderState.h"
#include "EffekseerRendererArea.Shader.h"
#include "EffekseerRendererArea.VertexBuffer.h"
#include <array>

namespace EffekseerRendererArea
{
Renderer* Renderer::Create()
{
	auto renderer = RendererImplemented::Create();
	if (!renderer->Initialize(16000))
	{
		ES_SAFE_RELEASE(renderer);
	}

	return renderer;
}

ModelRenderer::ModelRenderer(RendererImplemented* renderer) : m_renderer(renderer) {}

ModelRenderer::~ModelRenderer() {}

ModelRenderer* ModelRenderer::Create(RendererImplemented* renderer)
{
	assert(renderer != NULL);

	return new ModelRenderer(renderer);
}

void ModelRenderer::BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData)
{
	BeginRendering_(m_renderer, parameter, count, userData);
}

void ModelRenderer::Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData)
{
	Rendering_<RendererImplemented>(m_renderer, parameter, instanceParameter, userData);
}

void ModelRenderer::EndRendering(const efkModelNodeParam& parameter, void* userData)
{
	if (m_matrixes.size() == 0)
		return;
	if (parameter.ModelIndex < 0)
		return;

	if (parameter.Distortion)
		return;

	auto model = parameter.EffectPointer->GetModel(parameter.ModelIndex);
	if (model == nullptr)
		return;

	::EffekseerRenderer::RenderStateBase::State& state = m_renderer->GetRenderState()->Push();
	state.DepthTest = parameter.ZTest;
	state.DepthWrite = parameter.ZWrite;
	state.AlphaBlend = parameter.AlphaBlend;
	state.CullingType = parameter.Culling;

	m_renderer->GetRenderState()->Update(false);

	Effekseer::TextureData* textures[1];

	if (parameter.ColorTextureIndex >= 0)
	{
		textures[0] = parameter.EffectPointer->GetColorImage(parameter.ColorTextureIndex);
	}
	else
	{
		textures[0] = nullptr;
	}

	m_renderer->SetTextures(nullptr, textures, 1);

	m_renderer->DrawModel(model, m_matrixes, m_uv, m_colors);

	m_renderer->GetRenderState()->Pop();
}

RendererImplemented* RendererImplemented::Create() { return new RendererImplemented(); }

RendererImplemented::RendererImplemented()
{
	m_textures[0] = nullptr;
	m_textures[1] = nullptr;
	m_textures[2] = nullptr;
	m_textures[3] = nullptr;
}

RendererImplemented::~RendererImplemented()
{
	ES_SAFE_DELETE(m_renderState);
	ES_SAFE_DELETE(m_stanShader);
	ES_SAFE_DELETE(m_distShader);
	ES_SAFE_DELETE(m_standardRenderer);
	ES_SAFE_DELETE(m_vertexBuffer);
}

bool RendererImplemented::Initialize(int32_t squareMaxCount)
{
	m_squareMaxCount = squareMaxCount;
	m_renderState = new RenderState();
	m_vertexBuffer = new VertexBuffer(sizeof(Vertex) * m_squareMaxCount * 4, true);
	m_stanShader = new Shader();
	m_distShader = new Shader();

	m_standardRenderer = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>(
		this, m_stanShader, m_stanShader, m_stanShader, m_stanShader);

	return true;
}

void RendererImplemented::Destroy() { Release(); }

void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
{
	// TODO
}

bool RendererImplemented::BeginRendering()
{
	m_vertexes.clear();
	m_indexes.clear();
	m_materials.clear();
	m_materialFaceCounts.clear();

	::Effekseer::Matrix44::Mul(m_cameraProj, m_camera, m_proj);

	m_standardRenderer->ResetAndRenderingIfRequired();

	return true;
}

bool RendererImplemented::EndRendering()
{
	m_standardRenderer->ResetAndRenderingIfRequired();

	return true;
}

const ::Effekseer::Vector3D& RendererImplemented::GetLightDirection() const { return m_lightDirection; }

void RendererImplemented::SetLightDirection(const ::Effekseer::Vector3D& direction) { m_lightDirection = direction; }

const ::Effekseer::Color& RendererImplemented::GetLightColor() const { return m_lightColor; }

void RendererImplemented::SetLightColor(const ::Effekseer::Color& color) { m_lightColor = color; }

const ::Effekseer::Color& RendererImplemented::GetLightAmbientColor() const { return m_lightAmbient; }

void RendererImplemented::SetLightAmbientColor(const ::Effekseer::Color& color) { m_lightAmbient = color; }

int32_t RendererImplemented::GetSquareMaxCount() const { return m_squareMaxCount; }

const ::Effekseer::Matrix44& RendererImplemented::GetProjectionMatrix() const { return m_proj; }

void RendererImplemented::SetProjectionMatrix(const ::Effekseer::Matrix44& mat) { m_proj = mat; }

const ::Effekseer::Matrix44& RendererImplemented::GetCameraMatrix() const { return m_camera; }

void RendererImplemented::SetCameraMatrix(const ::Effekseer::Matrix44& mat)
{
	m_cameraFrontDirection = ::Effekseer::Vector3D(mat.Values[0][2], mat.Values[1][2], mat.Values[2][2]);

	auto localPos = ::Effekseer::Vector3D(-mat.Values[3][0], -mat.Values[3][1], -mat.Values[3][2]);
	auto f = m_cameraFrontDirection;
	auto r = ::Effekseer::Vector3D(mat.Values[0][0], mat.Values[1][0], mat.Values[2][0]);
	auto u = ::Effekseer::Vector3D(mat.Values[0][1], mat.Values[1][1], mat.Values[2][1]);

	m_cameraPosition = r * localPos.X + u * localPos.Y + f * localPos.Z;

	m_camera = mat;
}

::Effekseer::Matrix44& RendererImplemented::GetCameraProjectionMatrix() { return m_cameraProj; }

::Effekseer::Vector3D RendererImplemented::GetCameraFrontDirection() const { return m_cameraFrontDirection; }

::Effekseer::Vector3D RendererImplemented::GetCameraPosition() const { return m_cameraPosition; }

void RendererImplemented::SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position)
{
	m_cameraFrontDirection = front;
	m_cameraPosition = position;
}

::Effekseer::SpriteRenderer* RendererImplemented::CreateSpriteRenderer()
{
	return new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
}

::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
{
	return new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
}

::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
{
	return new ::EffekseerRenderer::RingRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
}

::Effekseer::ModelRenderer* RendererImplemented::CreateModelRenderer() { return ModelRenderer::Create(this); }

::Effekseer::TrackRenderer* RendererImplemented::CreateTrackRenderer()
{
	return new ::EffekseerRenderer::TrackRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
}

::Effekseer::TextureLoader* RendererImplemented::CreateTextureLoader(::Effekseer::FileInterface* fileInterface) { return nullptr; }

::Effekseer::ModelLoader* RendererImplemented::CreateModelLoader(::Effekseer::FileInterface* fileInterface) { return nullptr; }

void RendererImplemented::ResetRenderState() {}

::EffekseerRenderer::DistortingCallback* RendererImplemented::GetDistortingCallback() { return nullptr; }

void RendererImplemented::SetDistortingCallback(::EffekseerRenderer::DistortingCallback* callback) {}

Effekseer::TextureData* RendererImplemented::GetBackground()
{
	// TODO
	return nullptr;
}

VertexBuffer* RendererImplemented::GetVertexBuffer()
{
	// Todo make flexible
	return m_vertexBuffer;
}

IndexBuffer* RendererImplemented::GetIndexBuffer()
{
	// TODO
	return nullptr;
}

EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>* RendererImplemented::GetStandardRenderer()
{
	return m_standardRenderer;
}

::EffekseerRenderer::RenderStateBase* RendererImplemented::GetRenderState() { return m_renderState; }

void RendererImplemented::SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size)
{
	// TODO
}

void RendererImplemented::SetIndexBuffer(IndexBuffer* indexBuffer)
{
	// TODO
}

void RendererImplemented::SetLayout(Shader* shader)
{
	// TODO
}

void RendererImplemented::DrawSprites(int32_t spriteCount, int32_t vertexOffset)
{
	impl->drawcallCount++;
	impl->drawvertexCount += spriteCount * 4;

	if (m_distorting)
		return;

	// chek whether one ring
	auto stanMat = ((Effekseer::Matrix44*)m_stanShader->GetVertexConstantBuffer())[0];
	auto cameraMat = m_camera;
	Effekseer::Matrix44 ringMat;

	bool isSingleRing = false;

	for (int32_t r = 0; r < 4; r++)
	{
		for (int32_t c = 0; c < 4; c++)
		{
			if (stanMat.Values[r][c] != cameraMat.Values[r][c])
			{
				isSingleRing = true;
				goto Exit;
			}
		}
	}
Exit:;

	if (isSingleRing)
	{
		Effekseer::Matrix44 inv;
		Effekseer::Matrix44::Mul(ringMat, stanMat, Effekseer::Matrix44::Inverse(inv, cameraMat));
	}

	// TODO make flexible
	Vertex* vs = (Vertex*)m_vertexBuffer->GetResource();

	int32_t currentIndOffset = m_vertexes.size();

	for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
	{
		auto& v = vs[vi];

		if (isSingleRing)
		{
			Effekseer::Matrix44 trans;
			trans.Translation(v.Pos.X, v.Pos.Y, v.Pos.Z);
			Effekseer::Matrix44::Mul(trans, trans, ringMat);
			v.Pos.X = trans.Values[3][0];
			v.Pos.Y = trans.Values[3][1];
			v.Pos.Z = trans.Values[3][2];
		}

		//{
		//	Effekseer::Matrix44 trans;
		//	trans.Translation(v.Pos.X, v.Pos.Y, v.Pos.Z);
		//	Effekseer::Matrix44::Mul(trans, trans, cameraMat);
		//	v.Pos.X = trans.Values[3][0];
		//	v.Pos.Y = trans.Values[3][1];
		//	v.Pos.Z = trans.Values[3][2];
		//}

		m_vertexes.push_back(v);
	}

	for (int32_t si = 0; si < spriteCount; si++)
	{
		std::array<int32_t, 3> f0;
		std::array<int32_t, 3> f1;

		f0[0] = currentIndOffset + si * 4 + 2;
		f0[1] = currentIndOffset + si * 4 + 1;
		f0[2] = currentIndOffset + si * 4 + 0;

		f1[0] = currentIndOffset + si * 4 + 3;
		f1[1] = currentIndOffset + si * 4 + 1;
		f1[2] = currentIndOffset + si * 4 + 2;

		m_indexes.push_back(f0);
		m_indexes.push_back(f1);

		assert(f0[0] < m_vertexes.size());
		assert(f0[1] < m_vertexes.size());
		assert(f0[2] < m_vertexes.size());
		assert(f1[0] < m_vertexes.size());
		assert(f1[1] < m_vertexes.size());
		assert(f1[2] < m_vertexes.size());
	}

	auto material = RenderedMaterial();

	material.AlphaBlend = m_renderState->GetActiveState().AlphaBlend;

	if (m_textures[0] != nullptr)
	{
		material.TexturePath = ((TextureData*)m_textures[0])->Path;
	}

	material.DepthTest = m_renderState->GetActiveState().DepthTest;

	m_materials.push_back(material);
	m_materialFaceCounts.push_back(spriteCount * 2);
}

void RendererImplemented::DrawModel(void* model,
									std::vector<Effekseer::Matrix44>& matrixes,
									std::vector<Effekseer::RectF>& uvs,
									std::vector<Effekseer::Color>& colors)
{
	int32_t modelCount = matrixes.size();

	auto material = RenderedMaterial();

	material.AlphaBlend = m_renderState->GetActiveState().AlphaBlend;

	if (m_textures[0] != nullptr)
	{
		material.TexturePath = ((TextureData*)m_textures[0])->Path;
	}

	material.DepthTest = m_renderState->GetActiveState().DepthTest;

	auto m = (Effekseer::Model*)model;

	auto vc = m->GetVertexCount();
	auto fc = m->GetFaceCount();

	impl->drawcallCount++;
	impl->drawvertexCount += vc;

	m_materials.push_back(material);
	m_materialFaceCounts.push_back(fc * modelCount);

	int32_t vertexOffset = m_vertexes.size();

	for (auto mind = 0; mind < modelCount; mind++)
	{
		for (auto vind = 0; vind < vc; vind++)
		{
			auto v_ = m->GetVertexes()[vind];
			Vertex v;
			v.Pos = v_.Position;
			v.Col = Effekseer::Color::Mul(v_.VColor, colors[mind]);

			v.UV[0] = v_.UV.X * uvs[mind].Width + uvs[mind].X;
			v.UV[1] = v_.UV.Y * uvs[mind].Height + uvs[mind].Y;

			{
				Effekseer::Matrix44 trans;
				trans.Translation(v.Pos.X, v.Pos.Y, v.Pos.Z);
				Effekseer::Matrix44::Mul(trans, trans, matrixes[mind]);
				v.Pos.X = trans.Values[3][0];
				v.Pos.Y = trans.Values[3][1];
				v.Pos.Z = trans.Values[3][2];
			}

			m_vertexes.push_back(v);
		}

		for (auto find = 0; find < fc; find++)
		{
			auto find0 = m->GetFaces()[find].Indexes[0] + vertexOffset;
			auto find1 = m->GetFaces()[find].Indexes[1] + vertexOffset;
			auto find2 = m->GetFaces()[find].Indexes[2] + vertexOffset;

			std::array<int32_t, 3> f;
			f[0] = find0;
			f[1] = find1;
			f[2] = find2;

			m_indexes.push_back(f);
		}

		vertexOffset += vc;
	}
}

Shader* RendererImplemented::GetShader(bool useTexture, bool useDistortion) const
{

	if (useDistortion)
	{
		return m_distShader;
	}
	else
	{
		return m_stanShader;
	}
}

void RendererImplemented::BeginShader(Shader* shader)
{
	m_distorting = shader == m_distShader;
	currentShader = shader;
}

void RendererImplemented::EndShader(Shader* shader) { currentShader = nullptr; }

void RendererImplemented::SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset) { 
	assert(currentShader != nullptr);
	auto p = static_cast<uint8_t*>(currentShader->GetVertexConstantBuffer());
	p += dstOffset;
	memcpy(p, data, size);
}

void RendererImplemented::SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset) { 
	assert(currentShader != nullptr); 
	auto p = static_cast<uint8_t*>(currentShader->GetPixelConstantBuffer());
	p += dstOffset;
	memcpy(p, data, size);
}

void RendererImplemented::SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count)
{
	if (count > 0)
	{
		if (textures[0] != nullptr)
		{
			m_textures[0] = textures[0]->UserPtr;
		}
		else
		{
			m_textures[0] = nullptr;
		}
	}
	else
	{
		m_textures[0] = nullptr;
	}
}
} // namespace EffekseerRendererArea
