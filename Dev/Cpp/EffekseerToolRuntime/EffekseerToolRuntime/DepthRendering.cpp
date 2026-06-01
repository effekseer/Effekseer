#include "DepthRendering.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace Effekseer
{
namespace ToolRuntime
{
namespace
{

float Clamp(float value, float minValue, float maxValue)
{
	return value < minValue ? minValue : (value > maxValue ? maxValue : value);
}

uint32_t PackColor(uint32_t r, uint32_t g, uint32_t b)
{
	return 0xFF000000 | (b << 16) | (g << 8) | r;
}

uint32_t MakeSkyColor(int32_t y, int32_t height)
{
	const auto t = height > 1 ? static_cast<float>(y) / static_cast<float>(height - 1) : 0.0f;
	const auto r = static_cast<uint32_t>(34.0f + 42.0f * t);
	const auto g = static_cast<uint32_t>(44.0f + 46.0f * t);
	const auto b = static_cast<uint32_t>(60.0f + 36.0f * t);
	return PackColor(r, g, b);
}

uint32_t MakeGroundColor(const Effekseer::Vector3D& position, const Effekseer::Vector3D& cameraPosition)
{
	const auto cellX = static_cast<int32_t>(std::floor(position.X));
	const auto cellZ = static_cast<int32_t>(std::floor(position.Z));
	const auto isBright = ((cellX + cellZ) & 1) == 0;
	const auto distance = Effekseer::Vector3D::Length(position - cameraPosition);
	const auto shade = Clamp(1.08f - distance / 42.0f, 0.58f, 1.0f);

	auto r = static_cast<uint32_t>((isBright ? 92.0f : 58.0f) * shade);
	auto g = static_cast<uint32_t>((isBright ? 112.0f : 78.0f) * shade);
	auto b = static_cast<uint32_t>((isBright ? 96.0f : 68.0f) * shade);

	const auto localX = std::fabs(position.X - std::floor(position.X + 0.5f));
	const auto localZ = std::fabs(position.Z - std::floor(position.Z + 0.5f));
	if (localX > 0.47f || localZ > 0.47f)
	{
		r = static_cast<uint32_t>(r * 0.65f);
		g = static_cast<uint32_t>(g * 0.65f);
		b = static_cast<uint32_t>(b * 0.65f);
	}

	return PackColor(r, g, b);
}

Effekseer::Vector3D Unproject(float x, float y, float z, const Effekseer::Matrix44& inverseViewProjection)
{
	Effekseer::Vector3D position{x, y, z};
	Effekseer::Vector3D::TransformWithW(position, position, inverseViewProjection);
	return position;
}

float ProjectDepth(const Effekseer::Vector3D& position, const Effekseer::Matrix44& viewProjection)
{
	auto projected = position;
	Effekseer::Vector3D::TransformWithW(projected, projected, viewProjection);
	return projected.Z;
}

std::array<float, 4> TransformToClip(const Effekseer::Vector3D& position, const Effekseer::Matrix44& matrix)
{
	std::array<float, 4> values{};
	for (int32_t i = 0; i < 4; i++)
	{
		values[i] += position.X * matrix.Values[0][i];
		values[i] += position.Y * matrix.Values[1][i];
		values[i] += position.Z * matrix.Values[2][i];
		values[i] += matrix.Values[3][i];
	}
	return values;
}

template <class T>
void FlipRows(std::vector<T>& pixels, int32_t width, int32_t height)
{
	for (int32_t y = 0; y < height / 2; y++)
	{
		for (int32_t x = 0; x < width; x++)
		{
			std::swap(pixels[x + y * width], pixels[x + (height - 1 - y) * width]);
		}
	}
}

Effekseer::Matrix44 CreateViewProjectionMatrix(const Effekseer::Matrix44& cameraMatrix, const Effekseer::Matrix44& projectionMatrix)
{
	Effekseer::Matrix44 viewProjection;
	Effekseer::Matrix44::Mul(viewProjection, cameraMatrix, projectionMatrix);
	return viewProjection;
}

} // namespace

GroundPlaneParameter CreateDefaultSoftParticleGround()
{
	return GroundPlaneParameter{};
}

std::array<uint16_t, 6> CreateGroundPlaneIndices16()
{
	return {0, 1, 2, 0, 2, 3};
}

std::array<int32_t, 6> CreateGroundPlaneIndices32()
{
	return {0, 1, 2, 0, 2, 3};
}

std::array<GroundPlaneWorldVertex, 4> CreateGroundPlaneWorldVertices(const GroundPlaneParameter& parameter)
{
	const auto extent = parameter.HalfExtent;
	return {
		GroundPlaneWorldVertex{{-extent, parameter.Height, -extent}, {0.0f, 0.0f}},
		GroundPlaneWorldVertex{{extent, parameter.Height, -extent}, {extent, 0.0f}},
		GroundPlaneWorldVertex{{extent, parameter.Height, extent}, {extent, extent}},
		GroundPlaneWorldVertex{{-extent, parameter.Height, extent}, {0.0f, extent}},
	};
}

std::array<GroundPlaneClipVertex, 4> CreateGroundPlaneClipVertices(
	const Effekseer::Matrix44& cameraMatrix,
	const Effekseer::Matrix44& projectionMatrix,
	const GroundPlaneParameter& parameter)
{
	const auto viewProjection = CreateViewProjectionMatrix(cameraMatrix, projectionMatrix);
	const auto worldVertices = CreateGroundPlaneWorldVertices(parameter);

	std::array<GroundPlaneClipVertex, 4> vertices{};
	for (int32_t i = 0; i < static_cast<int32_t>(vertices.size()); i++)
	{
		vertices[i].Pos = TransformToClip(worldVertices[i].Position, viewProjection);
		vertices[i].WorldXZ = {worldVertices[i].Position.X, worldVertices[i].Position.Z};
	}

	return vertices;
}

std::array<std::array<float, 4>, 4> CreateViewProjectionColumns(
	const Effekseer::Matrix44& cameraMatrix,
	const Effekseer::Matrix44& projectionMatrix)
{
	const auto viewProjection = CreateViewProjectionMatrix(cameraMatrix, projectionMatrix);

	std::array<std::array<float, 4>, 4> columns{};
	for (int32_t column = 0; column < 4; column++)
	{
		for (int32_t row = 0; row < 4; row++)
		{
			columns[column][row] = viewProjection.Values[row][column];
		}
	}

	return columns;
}

EffekseerRenderer::DepthReconstructionParameter CreateDepthReconstructionParameter(
	const Effekseer::Matrix44& projectionMatrix,
	float depthBufferScale,
	float depthBufferOffset)
{
	EffekseerRenderer::DepthReconstructionParameter reconstructionParam;
	reconstructionParam.DepthBufferScale = depthBufferScale;
	reconstructionParam.DepthBufferOffset = depthBufferOffset;
	reconstructionParam.ProjectionMatrix33 = projectionMatrix.Values[2][2];
	reconstructionParam.ProjectionMatrix43 = projectionMatrix.Values[2][3];
	reconstructionParam.ProjectionMatrix34 = projectionMatrix.Values[3][2];
	reconstructionParam.ProjectionMatrix44 = projectionMatrix.Values[3][3];
	return reconstructionParam;
}

GroundDepthRenderData CreateGroundDepthRenderData(
	int32_t width,
	int32_t height,
	const Effekseer::Matrix44& cameraMatrix,
	const Effekseer::Matrix44& projectionMatrix,
	const Effekseer::Vector3D& cameraPosition,
	const GroundPlaneParameter& ground,
	bool isOpenGLDepth,
	bool flipVertically)
{
	GroundDepthRenderData result;
	if (width <= 0 || height <= 0)
	{
		return result;
	}

	const auto farDepth = 1.0f;
	const auto viewProjection = CreateViewProjectionMatrix(cameraMatrix, projectionMatrix);

	Effekseer::Matrix44 inverseViewProjection;
	Effekseer::Matrix44::Inverse(inverseViewProjection, viewProjection);

	const auto nearClipDepth = isOpenGLDepth ? -1.0f : 0.0f;
	std::vector<float> depthValues(width * height, farDepth);
	result.BackgroundPixels.resize(width * height);

	for (int32_t y = 0; y < height; y++)
	{
		const auto ndcY = 1.0f - (2.0f * (static_cast<float>(y) + 0.5f)) / static_cast<float>(height);

		for (int32_t x = 0; x < width; x++)
		{
			const auto ndcX = (2.0f * (static_cast<float>(x) + 0.5f)) / static_cast<float>(width) - 1.0f;
			const auto nearPosition = Unproject(ndcX, ndcY, nearClipDepth, inverseViewProjection);
			const auto farPosition = Unproject(ndcX, ndcY, farDepth, inverseViewProjection);
			const auto direction = farPosition - nearPosition;

			auto color = MakeSkyColor(y, height);
			auto depth = farDepth;

			if (std::fabs(direction.Y) > 0.0001f)
			{
				const auto t = (ground.Height - nearPosition.Y) / direction.Y;
				if (t >= 0.0f)
				{
					const auto groundPosition = nearPosition + direction * t;
					if (std::fabs(groundPosition.X) <= ground.HalfExtent && std::fabs(groundPosition.Z) <= ground.HalfExtent)
					{
						color = MakeGroundColor(groundPosition, cameraPosition);
						depth = ProjectDepth(groundPosition, viewProjection);
					}
				}
			}

			result.BackgroundPixels[x + y * width] = color;
			depthValues[x + y * width] = depth;
		}
	}

	if (flipVertically)
	{
		FlipRows(result.BackgroundPixels, width, height);
	}

	result.DepthTextureInitialData.resize(sizeof(float) * 4 * width * height);
	for (int32_t y = 0; y < height; y++)
	{
		const auto srcY = flipVertically ? (height - 1 - y) : y;
		for (int32_t x = 0; x < width; x++)
		{
			std::array<float, 4> depthPixel;
			depthPixel.fill(1.0f);
			depthPixel[0] = depthValues[x + srcY * width];
			std::memcpy(
				result.DepthTextureInitialData.data() + sizeof(float) * 4 * (x + y * width),
				depthPixel.data(),
				sizeof(float) * 4);
		}
	}

	return result;
}

} // namespace ToolRuntime
} // namespace Effekseer
