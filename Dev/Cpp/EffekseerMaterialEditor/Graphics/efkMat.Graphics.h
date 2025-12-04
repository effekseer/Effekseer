
#pragma once

#include <Effekseer.h>
#include <EffekseerRendererGL.h>
#include <cmath>
#include <cstring>
#include <efkMat.TextExporter.h>

#include <Effekseer/Material/Effekseer.MaterialFile.h>

namespace EffekseerMaterial
{

class Texture;
class TextureWithSampler;

enum class PreviewModelType
{
	Screen,
	Sphere,
};

struct Vector2
{
	float X;
	float Y;

	Vector2()
		: X(0.0f)
		, Y(0.0f)
	{
	}

	Vector2(float x, float y)
		: X(x)
		, Y(y)
	{
	}
};

struct Vector3
{
	float X;
	float Y;
	float Z;

	Vector3()
		: X(0.0f)
		, Y(0.0f)
		, Z(0.0f)
	{
	}

	Vector3(float x, float y, float z)
		: X(x)
		, Y(y)
		, Z(z)
	{
	}

	bool operator==(const Vector3& o) const
	{
		return X == o.X && Y == o.Y && Z == o.Z;
	}

	Vector3 operator+(const Vector3& o) const
	{
		return Vector3(X + o.X, Y + o.Y, Z + o.Z);
	}

	Vector3 operator-(const Vector3& o) const
	{
		return Vector3(X - o.X, Y - o.Y, Z - o.Z);
	}

	Vector3 operator/(const float& o) const
	{
		return Vector3(X / o, Y / o, Z / o);
	}

	float GetLength() const
	{
		return sqrt(GetSquaredLength());
	}

	float GetSquaredLength() const
	{
		return X * X + Y * Y + Z * Z;
	}

	Vector3 GetNormal()
	{
		float length = GetLength();
		return Vector3(X / length, Y / length, Z / length);
	}

	void Normalize()
	{
		float length = GetLength();
		X /= length;
		Y /= length;
		Z /= length;
	}

	static float Dot(const Vector3& v1, const Vector3& v2)
	{
		return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
	}

	static Vector3 Cross(const Vector3& v1, const Vector3& v2)
	{
		Vector3 o;

		float x = v1.Y * v2.Z - v1.Z * v2.Y;
		float y = v1.Z * v2.X - v1.X * v2.Z;
		float z = v1.X * v2.Y - v1.Y * v2.X;
		o.X = x;
		o.Y = y;
		o.Z = z;
		return o;
	}

	static Vector3 Subtract(Vector3 v1, Vector3 v2)
	{
		Vector3 o = Vector3();
		o.X = v1.X - v2.X;
		o.Y = v1.Y - v2.Y;
		o.Z = v1.Z - v2.Z;
		return o;
	}
};

struct CompileResult
{
	std::shared_ptr<Effekseer::MaterialFile> materialFile;
	std::vector<std::shared_ptr<TextureWithSampler>> textures;
	std::vector<std::shared_ptr<TextExporterUniform>> uniforms;
	std::vector<std::shared_ptr<TextExporterGradient>> gradients;
	std::vector<std::shared_ptr<TextExporterGradient>> fixedGradients;
	int customData1Count;
	int customData2Count;
};

struct Vertex
{
	Vector3 Pos;
	Vector3 Normal;
	Vector3 Binormal;
	Vector3 Tangent;
	Vector2 UV1;
	std::array<uint8_t, 4> Color;
};

static void CalcTangentSpace(const Vertex& v1, const Vertex& v2, const Vertex& v3, Vector3& binormal, Vector3& tangent)
{
	binormal = Vector3();
	tangent = Vector3();

	Vector3 cp0[3];
	cp0[0] = Vector3(v1.Pos.X, v1.UV1.X, v1.UV1.Y);
	cp0[1] = Vector3(v1.Pos.Y, v1.UV1.X, v1.UV1.Y);
	cp0[2] = Vector3(v1.Pos.Z, v1.UV1.X, v1.UV1.Y);

	Vector3 cp1[3];
	cp1[0] = Vector3(v2.Pos.X, v2.UV1.X, v2.UV1.Y);
	cp1[1] = Vector3(v2.Pos.Y, v2.UV1.X, v2.UV1.Y);
	cp1[2] = Vector3(v2.Pos.Z, v2.UV1.X, v2.UV1.Y);

	Vector3 cp2[3];
	cp2[0] = Vector3(v3.Pos.X, v3.UV1.X, v3.UV1.Y);
	cp2[1] = Vector3(v3.Pos.Y, v3.UV1.X, v3.UV1.Y);
	cp2[2] = Vector3(v3.Pos.Z, v3.UV1.X, v3.UV1.Y);

	float u[3];
	float v[3];

	for (int32_t i = 0; i < 3; i++)
	{
		auto abc = Vector3::Cross(cp1[i] - cp0[i], cp2[i] - cp1[i]);

		if (abc.X == 0.0f)
		{
			return;
		}
		else
		{
			u[i] = -abc.Y / abc.X;
			v[i] = -abc.Z / abc.X;
		}
	}

	tangent = Vector3(u[0], u[1], u[2]);
	tangent.Normalize();

	binormal = Vector3(v[0], v[1], v[2]);
	binormal.Normalize();
}

class Graphics
{
private:
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice_;

public:
	bool Initialize(int32_t width, int32_t height);

	Effekseer::Backend::GraphicsDeviceRef GetGraphicsDevice()
	{
		return graphicsDevice_;
	}
};

class Texture
{
private:
	std::string path_;
	std::shared_ptr<Graphics> graphics_;
	Effekseer::Backend::TextureRef texture_ = nullptr;

public:
	bool Validate();
	void Invalidate();

	uint64_t GetInternal();

	const std::string GetPath() const
	{
		return path_;
	}

	Effekseer::Backend::TextureRef GetTexture()
	{
		return texture_;
	}

	static std::shared_ptr<Texture> Load(std::shared_ptr<Graphics> graphics, const char* path);

	static std::shared_ptr<Texture> Load(std::shared_ptr<Graphics> graphics, Vector2 size, const void* initialData);
};

class TextureWithSampler
{
private:
public:
	std::string Name;
	std::shared_ptr<Texture> TexturePtr;
	EffekseerMaterial::TextureSamplerType SamplerType;
};

class TextureCache
{
private:
	static std::map<std::string, std::shared_ptr<Texture>> textures_;

public:
	static std::shared_ptr<Texture> Load(std::shared_ptr<Graphics> graphics, const char* path);

	static void NotifyFileChanged(const char* path);
};

class Mesh
{
private:
	Effekseer::Backend::VertexBufferRef vertexBuffer_ = nullptr;
	Effekseer::Backend::IndexBufferRef indexBuffer_ = nullptr;
	int32_t indexCount_ = 0;

public:
	Effekseer::Backend::VertexBufferRef GetVertexBuffer()
	{
		return vertexBuffer_;
	}

	Effekseer::Backend::IndexBufferRef GetIndexBuffer()
	{
		return indexBuffer_;
	}

	int32_t GetIndexCount()
	{
		return indexCount_;
	}

	static std::shared_ptr<Mesh> Load(std::shared_ptr<Graphics> graphics, const char* path);
};

class Preview
{
private:
	std::shared_ptr<Graphics> graphics_;

	Effekseer::Backend::TextureRef renderTarget_ = nullptr;
	Effekseer::Backend::TextureRef depthTarget_ = nullptr;
	Effekseer::Backend::ShaderRef shader_ = nullptr;
	Effekseer::Backend::VertexBufferRef screenVB_ = nullptr;
	Effekseer::Backend::IndexBufferRef screenIB_ = nullptr;
	Effekseer::Backend::RenderPassRef renderPass_ = nullptr;
	Effekseer::Backend::PipelineStateRef pipelineState_ = nullptr;
	Effekseer::Backend::UniformBufferRef vertexUniformBuffer_ = nullptr;
	Effekseer::Backend::UniformBufferRef pixelUniformBuffer_ = nullptr;
	Effekseer::Backend::UniformLayoutRef uniformLayout_ = nullptr;

	std::vector<std::shared_ptr<TextureWithSampler>> textures_;
	std::shared_ptr<Mesh> mesh_;
	std::shared_ptr<Texture> black_;
	std::shared_ptr<Texture> white_;

public:
	bool Initialize(std::shared_ptr<Graphics> graphics);
	bool UpdateShader(const CompileResult& compileResult);
	bool UpdateUniforms(std::vector<std::shared_ptr<TextureWithSampler>> textures,
						std::vector<std::shared_ptr<TextExporterUniform>>& uniforms,
						std::vector<std::shared_ptr<TextExporterGradient>>& gradients);
	bool UpdateConstantValues(float time, std::array<float, 4> customData1, std::array<float, 4> customData2);
	void Render();

	PreviewModelType ModelType = PreviewModelType::Screen;

	uint64_t GetInternal();

	std::string VS;
	std::string PS;

	static const int32_t TextureSize = 128;
};

} // namespace EffekseerMaterial

namespace std
{
template <>
struct hash<EffekseerMaterial::Vector3>
{
	size_t operator()(const EffekseerMaterial::Vector3& _Keyval) const noexcept
	{
		return std::hash<float>()(_Keyval.X) + std::hash<float>()(_Keyval.Y) + std::hash<float>()(_Keyval.Z);
	}
};

template <>
struct hash<std::tuple<EffekseerMaterial::Vector3, EffekseerMaterial::Vector3>>
{
	size_t operator()(const std::tuple<EffekseerMaterial::Vector3, EffekseerMaterial::Vector3>& _Keyval) const noexcept
	{
		return std::hash<EffekseerMaterial::Vector3>()(std::get<0>(_Keyval)) +
			   std::hash<EffekseerMaterial::Vector3>()(std::get<1>(_Keyval));
	}
};

} // namespace std
