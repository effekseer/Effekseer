#include "Model.h"
#include "../Backend/GraphicsDevice.h"

namespace Effekseer
{

Model::Model(const CustomVector<Vertex>& vertecies, const CustomVector<Face>& faces)
{
	models_.resize(1);
	models_[0].vertexes = vertecies;
	models_[0].faces = faces;
}

Model::Model(const void* data, int32_t size)
{
	if (data == nullptr)
	{
		models_.resize(1);
		return;
	}

	const uint8_t* p = (const uint8_t*)data;

	memcpy(&version_, p, sizeof(int32_t));
	p += sizeof(int32_t);

	// load scale except version 3(for compatibility)
	if (version_ == 2 || version_ >= 5)
	{
		// Scale
		p += sizeof(int32_t);
	}

	if (version_ > LatestVersion)
	{
		models_.resize(1);
		return;
	}

	// For compatibility
	int32_t modelCount = 0;
	memcpy(&modelCount, p, sizeof(int32_t));
	p += sizeof(int32_t);

	int32_t frameCount = 1;

	if (version_ >= 5)
	{
		memcpy(&frameCount, p, sizeof(int32_t));
		p += sizeof(int32_t);
	}

	models_.resize(frameCount);

	for (int32_t f = 0; f < frameCount; f++)
	{
		int32_t vertexCount = 0;
		memcpy(&vertexCount, p, sizeof(int32_t));
		p += sizeof(int32_t);

		models_[f].vertexes.resize(vertexCount);

		if (version_ >= 6)
		{
			memcpy(models_[f].vertexes.data(), p, sizeof(Vertex) * vertexCount);
			p += sizeof(Vertex) * vertexCount;
		}
		else
		{
			Vertex* vertexes = models_[f].vertexes.data();

			const Color defaultColor(255, 255, 255, 255);
			for (int32_t i = 0; i < vertexCount; i++)
			{
				memcpy(&vertexes[i].Position, p, sizeof(Vector3D));
				p += sizeof(Vector3D);
				Vector3D normal, binormal, tangent;
				memcpy(&normal, p, sizeof(Vector3D));
				p += sizeof(Vector3D);
				memcpy(&binormal, p, sizeof(Vector3D));
				p += sizeof(Vector3D);
				memcpy(&tangent, p, sizeof(Vector3D));
				p += sizeof(Vector3D);

				Vector3D cross;
				Vector3D::Cross(cross, normal, tangent);
				float sign = copysign(1.0f, Vector3D::Dot(binormal, cross));
				vertexes[i].OctNormal = PackUnorm2(OctNormalEncode(normal));
				vertexes[i].OctTangent = PackUnorm2(OctTangentEncode(tangent, sign));

				memcpy(&vertexes[i].UV1, p, sizeof(Vector2D));
				p += sizeof(Vector2D);
				vertexes[i].UV2 = vertexes[i].UV1;

				if (version_ >= 1)
				{
					memcpy(&vertexes[i].VColor, p, sizeof(Color));
					p += sizeof(Color);
				}
				else
				{
					vertexes[i].VColor = defaultColor;
				}
			}
		}

		int32_t faceCount = 0;
		memcpy(&faceCount, p, sizeof(int32_t));
		p += sizeof(int32_t);

		models_[f].faces.resize(faceCount);
		memcpy(models_[f].faces.data(), p, sizeof(Face) * faceCount);
		p += sizeof(Face) * faceCount;
	}
}

Model ::~Model()
{
}

const RefPtr<Backend::VertexBuffer>& Model::GetVertexBuffer(int32_t index) const
{
	return models_[index].vertexBuffer;
}

const RefPtr<Backend::IndexBuffer>& Model::GetIndexBuffer(int32_t index) const
{
	return models_[index].indexBuffer;
}

const RefPtr<Backend::IndexBuffer>& Model::GetWireIndexBuffer(int32_t index) const
{
	return models_[index].wireIndexBuffer;
}

const Model::Vertex* Model::GetVertexes(int32_t index) const
{
	return models_[index].vertexes.data();
}

int32_t Model::GetVertexCount(int32_t index) const
{
	return static_cast<int32_t>(models_[index].vertexes.size());
}

const Model::Face* Model::GetFaces(int32_t index) const
{
	return models_[index].faces.data();
}

int32_t Model::GetFaceCount(int32_t index) const
{
	return static_cast<int32_t>(models_[index].faces.size());
}

int32_t Model::GetFrameCount() const
{
	return static_cast<int32_t>(models_.size());
}

Model::Emitter Model::GetEmitter(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification)
{
	time = time % GetFrameCount();

	const auto faceCount = GetFaceCount(time);
	if (faceCount == 0)
	{
		return GetEmitterFromVertex(g, time, coordinate, magnification);
	}

	int32_t faceInd = static_cast<int32_t>(GetFaceCount(time) * g->GetRand());
	faceInd = Clamp(faceInd, GetFaceCount(time) - 1, 0);
	const Face& face = GetFaces(time)[faceInd];
	const Vertex& v0 = GetVertexes(time)[face.Indexes[0]];
	const Vertex& v1 = GetVertexes(time)[face.Indexes[1]];
	const Vertex& v2 = GetVertexes(time)[face.Indexes[2]];

	float p1 = g->GetRand();
	float p2 = g->GetRand();

	// Fit within plane
	if (p1 + p2 > 1.0f)
	{
		p1 = 1.0f - p1;
		p2 = 1.0f - p2;
	}

	float p0 = 1.0f - p1 - p2;
	auto tbn0 = GetVertexTBN(v0);
	auto tbn1 = GetVertexTBN(v1);
	auto tbn2 = GetVertexTBN(v2);

	Emitter emitter;
	emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
	emitter.Normal = tbn0[2] * p0 + tbn1[2] * p1 + tbn2[2] * p2;
	emitter.Binormal = tbn0[1] * p0 + tbn1[1] * p1 + tbn2[1] * p2;
	emitter.Tangent = tbn0[0] * p0 + tbn1[0] * p1 + tbn2[0] * p2;

	if (coordinate == CoordinateSystem::LH)
	{
		emitter.Position.Z = -emitter.Position.Z;
		emitter.Normal.Z = -emitter.Normal.Z;
		emitter.Binormal.Z = -emitter.Binormal.Z;
		emitter.Tangent.Z = -emitter.Tangent.Z;
	}

	return emitter;
}

Model::Emitter Model::GetEmitterFromVertex(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification)
{
	time = time % GetFrameCount();

	const auto vertexCount = GetVertexCount(time);
	if (vertexCount == 0)
	{
		return Model::Emitter{};
	}

	int32_t vertexInd = static_cast<int32_t>(GetVertexCount(time) * g->GetRand());
	vertexInd = Clamp(vertexInd, GetVertexCount(time) - 1, 0);
	const Vertex& v = GetVertexes(time)[vertexInd];
	auto tbn = GetVertexTBN(v);

	Emitter emitter;
	emitter.Position = v.Position * magnification;
	emitter.Normal = tbn[2];
	emitter.Binormal = tbn[1];
	emitter.Tangent = tbn[0];

	if (coordinate == CoordinateSystem::LH)
	{
		emitter.Position.Z = -emitter.Position.Z;
		emitter.Normal.Z = -emitter.Normal.Z;
		emitter.Binormal.Z = -emitter.Binormal.Z;
		emitter.Tangent.Z = -emitter.Tangent.Z;
	}

	return emitter;
}

Model::Emitter Model::GetEmitterFromVertex(int32_t index, int32_t time, CoordinateSystem coordinate, float magnification)
{
	time = time % GetFrameCount();

	const auto vertexCount = GetVertexCount(time);
	if (vertexCount == 0)
	{
		return Model::Emitter{};
	}

	int32_t vertexInd = index % GetVertexCount(time);
	const Vertex& v = GetVertexes(time)[vertexInd];
	auto tbn = GetVertexTBN(v);

	Emitter emitter;
	emitter.Position = v.Position * magnification;
	emitter.Normal = tbn[2];
	emitter.Binormal = tbn[1];
	emitter.Tangent = tbn[0];

	if (coordinate == CoordinateSystem::LH)
	{
		emitter.Position.Z = -emitter.Position.Z;
		emitter.Normal.Z = -emitter.Normal.Z;
		emitter.Binormal.Z = -emitter.Binormal.Z;
		emitter.Tangent.Z = -emitter.Tangent.Z;
	}

	return emitter;
}

Model::Emitter Model::GetEmitterFromFace(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification)
{
	time = time % GetFrameCount();

	const auto faceCount = GetFaceCount(time);
	if (faceCount == 0)
	{
		return Model::Emitter{};
	}

	int32_t faceInd = static_cast<int32_t>(GetFaceCount(time) * g->GetRand());
	faceInd = Clamp(faceInd, GetFaceCount(time) - 1, 0);
	const Face& face = GetFaces(time)[faceInd];
	const Vertex& v0 = GetVertexes(time)[face.Indexes[0]];
	const Vertex& v1 = GetVertexes(time)[face.Indexes[1]];
	const Vertex& v2 = GetVertexes(time)[face.Indexes[2]];

	float p0 = 1.0f / 3.0f;
	float p1 = 1.0f / 3.0f;
	float p2 = 1.0f / 3.0f;
	auto tbn0 = GetVertexTBN(v0);
	auto tbn1 = GetVertexTBN(v1);
	auto tbn2 = GetVertexTBN(v2);

	Emitter emitter;
	emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
	emitter.Normal = tbn0[2] * p0 + tbn1[2] * p1 + tbn2[2] * p2;
	emitter.Binormal = tbn0[1] * p0 + tbn1[1] * p1 + tbn2[1] * p2;
	emitter.Tangent = tbn0[0] * p0 + tbn1[0] * p1 + tbn2[0] * p2;

	if (coordinate == CoordinateSystem::LH)
	{
		emitter.Position.Z = -emitter.Position.Z;
		emitter.Normal.Z = -emitter.Normal.Z;
		emitter.Binormal.Z = -emitter.Binormal.Z;
		emitter.Tangent.Z = -emitter.Tangent.Z;
	}

	return emitter;
}

Model::Emitter Model::GetEmitterFromFace(int32_t index, int32_t time, CoordinateSystem coordinate, float magnification)
{
	time = time % GetFrameCount();

	const auto faceCount = GetFaceCount(time);
	if (faceCount == 0)
	{
		return Model::Emitter{};
	}

	int32_t faceInd = index % (GetFaceCount(time) - 1);
	const Face& face = GetFaces(time)[faceInd];
	const Vertex& v0 = GetVertexes(time)[face.Indexes[0]];
	const Vertex& v1 = GetVertexes(time)[face.Indexes[1]];
	const Vertex& v2 = GetVertexes(time)[face.Indexes[2]];

	float p0 = 1.0f / 3.0f;
	float p1 = 1.0f / 3.0f;
	float p2 = 1.0f / 3.0f;
	auto tbn0 = GetVertexTBN(v0);
	auto tbn1 = GetVertexTBN(v1);
	auto tbn2 = GetVertexTBN(v2);

	Emitter emitter;
	emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
	emitter.Normal = tbn0[2] * p0 + tbn1[2] * p1 + tbn2[2] * p2;
	emitter.Binormal = tbn0[1] * p0 + tbn1[1] * p1 + tbn2[1] * p2;
	emitter.Tangent = tbn0[0] * p0 + tbn1[0] * p1 + tbn2[0] * p2;

	if (coordinate == CoordinateSystem::LH)
	{
		emitter.Position.Z = -emitter.Position.Z;
		emitter.Normal.Z = -emitter.Normal.Z;
		emitter.Binormal.Z = -emitter.Binormal.Z;
		emitter.Tangent.Z = -emitter.Tangent.Z;
	}

	return emitter;
}

bool Model::StoreBufferToGPU(Backend::GraphicsDevice* graphicsDevice, bool flipVertexColor)
{
	if (isBufferStoredOnGPU_)
	{
		return false;
	}

	if (graphicsDevice == nullptr)
	{
		return false;
	}

	for (int32_t f = 0; f < GetFrameCount(); f++)
	{
		if (flipVertexColor)
		{
			auto vdata = models_[f].vertexes;

			for (auto& v : vdata)
			{
				std::swap(v.VColor.R, v.VColor.B);
			}

			models_[f].vertexBuffer = graphicsDevice->CreateVertexBuffer(sizeof(Effekseer::Model::Vertex) * GetVertexCount(f), vdata.data(), false);
			if (models_[f].vertexBuffer == nullptr)
			{
				return false;
			}
		}
		else
		{
			models_[f].vertexBuffer = graphicsDevice->CreateVertexBuffer(sizeof(Effekseer::Model::Vertex) * GetVertexCount(f), models_[f].vertexes.data(), false);
			if (models_[f].vertexBuffer == nullptr)
			{
				return false;
			}
		}

		{
			models_[f].indexBuffer = graphicsDevice->CreateIndexBuffer(3 * GetFaceCount(f), models_[f].faces.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);
			if (models_[f].indexBuffer == nullptr)
			{
				return false;
			}
		}
	}

	isBufferStoredOnGPU_ = true;
	return true;
}

bool Model::GetIsBufferStoredOnGPU() const
{
	return isBufferStoredOnGPU_;
}

bool Model::GenerateWireIndexBuffer(Backend::GraphicsDevice* graphicsDevice)
{
	if (isWireIndexBufferGenerated_)
	{
		return true;
	}

	if (graphicsDevice == nullptr)
	{
		return false;
	}

	for (int32_t f = 0; f < GetFrameCount(); f++)
	{
		CustomVector<int32_t> indexes;
		indexes.reserve(GetFaceCount(f) * 6);

		auto fp = GetFaces(f);

		for (int32_t i = 0; i < GetFaceCount(f); i++)
		{
			indexes.emplace_back(fp->Indexes[0]);
			indexes.emplace_back(fp->Indexes[1]);
			indexes.emplace_back(fp->Indexes[1]);
			indexes.emplace_back(fp->Indexes[2]);
			indexes.emplace_back(fp->Indexes[2]);
			indexes.emplace_back(fp->Indexes[0]);
			fp++;
		}

		{
			models_[f].wireIndexBuffer = graphicsDevice->CreateIndexBuffer(static_cast<int32_t>(indexes.size()), indexes.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);
			if (models_[f].wireIndexBuffer == nullptr)
			{
				return false;
			}
		}
	}

	isWireIndexBufferGenerated_ = true;

	return isWireIndexBufferGenerated_;
}

bool Model::GetIsWireIndexBufferGenerated() const
{
	return isWireIndexBufferGenerated_;
}

std::array<Vector3D, 3> Model::GetVertexTBN(const Vertex& vertex)
{
	auto normal = OctNormalDecode(UnpackUnorm2(vertex.OctNormal));
	auto [tangent, sign] = OctTangentDecode(UnpackUnorm2(vertex.OctTangent));
	Vector3D binormal;
	Vector3D::Cross(binormal, normal, tangent);
	binormal *= sign;
	return { tangent, binormal, normal };
}

Vector2D Model::OctNormalEncode(Vector3D n)
{
	Vector3D::Normal(n, n);
	n /= abs(n.X) + abs(n.Y) + abs(n.Z);

	Vector2D res;
	if (n.Z >= 0.0f)
	{
		res.X = n.X;
		res.Y = n.Y;
	}
	else
	{
		res.X = (1.0f - abs(n.Y)) * (n.X >= 0.0f ? 1.0f : -1.0f);
		res.Y = (1.0f - abs(n.X)) * (n.Y >= 0.0f ? 1.0f : -1.0f);
	}
	res.X = res.X * 0.5f + 0.5f;
	res.Y = res.Y * 0.5f + 0.5f;

	return res;
}

Vector2D Model::OctTangentEncode(Vector3D t, float sign)
{
	const float bias = 1.0f / 32767.0f;
	Vector2D res = OctNormalEncode(t);
	res.Y = Max(res.Y, bias);
	res.Y = res.Y * 0.5f + 0.5f;
	res.Y = sign >= 0.0f ? res.Y : 1.0f - res.Y;
	return res;
}

Vector3D Model::OctNormalDecode(Vector2D oct) {
	Vector2D f(oct.X * 2.0f - 1.0f, oct.Y * 2.0f - 1.0f);
	Vector3D n(f.X, f.Y, 1.0f - abs(f.X) - abs(f.Y));
	float t = Clamp(-n.Z, 0.0f, 1.0f);
	n.X += n.X >= 0.0f ? -t : t;
	n.Y += n.Y >= 0.0f ? -t : t;
	Vector3D::Normal(n, n);
	return n;
}

std::tuple<Vector3D, float> Model::OctTangentDecode(Vector2D oct) {
	oct.Y = oct.Y * 2.0f - 1.0f;
	float sign = oct.Y >= 0.0f ? 1.0f : -1.0f;
	oct.Y = abs(oct.Y);
	Vector3D res = OctNormalDecode(oct);
	return std::make_tuple(res, sign);
}

uint32_t Model::PackUnorm2(Vector2D vec)
{
	using limits = std::numeric_limits<uint16_t>;

	const float minValue = static_cast<float>(limits::min());
	const float maxValue = static_cast<float>(limits::max());
	const float x = vec.X * limits::max();
	const float y = vec.Y * limits::max();

	uint32_t value = 0;
	value |= static_cast<uint16_t>(Clamp(x, maxValue, minValue));
	value |= static_cast<uint16_t>(Clamp(y, maxValue, minValue)) << 16;
	return value;
}

Vector2D Model::UnpackUnorm2(uint32_t value)
{
	using limits = std::numeric_limits<uint16_t>;

	const float maxValue = static_cast<float>(limits::max());

	Vector2D vec;
	vec.X = (value & 0xFFFF) / maxValue;
	vec.Y = ((value >> 16) & 0xFFFF) / maxValue;
	return vec;
}

} // namespace Effekseer
