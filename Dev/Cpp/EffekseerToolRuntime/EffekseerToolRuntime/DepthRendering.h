#pragma once

#include <Effekseer.h>
#include <EffekseerRendererCommon/EffekseerRenderer.Renderer.h>

#include <array>
#include <cstdint>
#include <vector>

namespace Effekseer
{
namespace ToolRuntime
{

struct GroundPlaneParameter
{
	float Height = -1.0f;
	float HalfExtent = 18.0f;
};

struct GroundPlaneClipVertex
{
	std::array<float, 4> Pos;
	std::array<float, 2> WorldXZ;
};

struct GroundPlaneWorldVertex
{
	Effekseer::Vector3D Position;
	std::array<float, 2> UV;
};

struct GroundDepthRenderData
{
	std::vector<uint32_t> BackgroundPixels;
	std::vector<uint8_t> DepthTextureInitialData;
};

GroundPlaneParameter CreateDefaultSoftParticleGround();

std::array<uint16_t, 6> CreateGroundPlaneIndices16();

std::array<int32_t, 6> CreateGroundPlaneIndices32();

std::array<GroundPlaneWorldVertex, 4> CreateGroundPlaneWorldVertices(const GroundPlaneParameter& parameter);

std::array<GroundPlaneClipVertex, 4> CreateGroundPlaneClipVertices(
	const Effekseer::Matrix44& cameraMatrix,
	const Effekseer::Matrix44& projectionMatrix,
	const GroundPlaneParameter& parameter);

std::array<std::array<float, 4>, 4> CreateViewProjectionColumns(
	const Effekseer::Matrix44& cameraMatrix,
	const Effekseer::Matrix44& projectionMatrix);

EffekseerRenderer::DepthReconstructionParameter CreateDepthReconstructionParameter(
	const Effekseer::Matrix44& projectionMatrix,
	float depthBufferScale = 1.0f,
	float depthBufferOffset = 0.0f);

GroundDepthRenderData CreateGroundDepthRenderData(
	int32_t width,
	int32_t height,
	const Effekseer::Matrix44& cameraMatrix,
	const Effekseer::Matrix44& projectionMatrix,
	const Effekseer::Vector3D& cameraPosition,
	const GroundPlaneParameter& ground,
	bool isOpenGLDepth,
	bool flipVertically);

} // namespace ToolRuntime
} // namespace Effekseer
