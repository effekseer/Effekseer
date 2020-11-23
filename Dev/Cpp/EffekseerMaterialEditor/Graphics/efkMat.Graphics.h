
#pragma once

#include <AltseedRHI.h>
#include <cmath>
#include <cstring>
#include <efkMat.TextExporter.h>

namespace EffekseerMaterial
{

enum class PreviewModelType
{
	Screen,
	Sphere,
};

struct Vector2
{
	float X;
	float Y;

	Vector2() : X(0.0f), Y(0.0f) {}

	Vector2(float x, float y) : X(x), Y(y) {}
};

struct Vector3
{
	float X;
	float Y;
	float Z;

	Vector3() : X(0.0f), Y(0.0f), Z(0.0f) {}

	Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

	bool operator==(const Vector3& o) const { return X == o.X && Y == o.Y && Z == o.Z; }

	Vector3 operator+(const Vector3& o) const { return Vector3(X + o.X, Y + o.Y, Z + o.Z); }

	Vector3 operator-(const Vector3& o) const { return Vector3(X - o.X, Y - o.Y, Z - o.Z); }

	Vector3 operator/(const float& o) const { return Vector3(X / o, Y / o, Z / o); }

	float GetLength() const { return sqrt(GetSquaredLength()); }

	float GetSquaredLength() const { return X * X + Y * Y + Z * Z; }

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

	static float Dot(const Vector3& v1, const Vector3& v2) { return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z; }

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

struct Matrix44
{
private:
public:
	Matrix44() = default;

	float Values[4][4];

	Matrix44& OrthographicRH(float width, float height, float zn, float zf)
	{
		Values[0][0] = 2 / width;
		Values[0][1] = 0;
		Values[0][2] = 0;
		Values[0][3] = 0;

		Values[1][0] = 0;
		Values[1][1] = 2 / height;
		Values[1][2] = 0;
		Values[1][3] = 0;

		Values[2][0] = 0;
		Values[2][1] = 0;
		Values[2][2] = 1 / (zn - zf);
		Values[2][3] = 0;

		Values[3][0] = 0;
		Values[3][1] = 0;
		Values[3][2] = zn / (zn - zf);
		Values[3][3] = 1;
		return *this;
	}

	Matrix44& SetPerspectiveFovRH(float ovY, float aspect, float zn, float zf)
	{
		float yScale = 1 / tanf(ovY / 2);
		float xScale = yScale / aspect;

		Values[0][0] = xScale;
		Values[1][0] = 0;
		Values[2][0] = 0;
		Values[3][0] = 0;

		Values[0][1] = 0;
		Values[1][1] = yScale;
		Values[2][1] = 0;
		Values[3][1] = 0;

		Values[0][2] = 0;
		Values[1][2] = 0;
		Values[2][2] = zf / (zn - zf);
		Values[3][2] = -1;

		Values[0][3] = 0;
		Values[1][3] = 0;
		Values[2][3] = zn * zf / (zn - zf);
		Values[3][3] = 0;
		return *this;
	}

	Matrix44& SetPerspectiveFovRH_OpenGL(float ovY, float aspect, float zn, float zf)
	{
		float yScale = 1 / tanf(ovY / 2);
		float xScale = yScale / aspect;
		float dz = zf - zn;

		Values[0][0] = xScale;
		Values[1][0] = 0;
		Values[2][0] = 0;
		Values[3][0] = 0;

		Values[0][1] = 0;
		Values[1][1] = yScale;
		Values[2][1] = 0;
		Values[3][1] = 0;

		Values[0][2] = 0;
		Values[1][2] = 0;
		Values[2][2] = -(zf + zn) / dz;
		Values[3][2] = -1.0f;

		Values[0][3] = 0;
		Values[1][3] = 0;
		Values[2][3] = -2.0f * zn * zf / dz;
		Values[3][3] = 0.0f;

		return *this;
	}

	Matrix44& SetLookAtRH(const Vector3& eye, const Vector3& at, const Vector3& up)
	{
		std::memset(Values, 0, sizeof(float) * 16);

		Vector3 F = Vector3::Subtract(eye, at).GetNormal();
		Vector3 R = Vector3::Cross(up, F).GetNormal();
		Vector3 U = Vector3::Cross(F, R).GetNormal();

		Values[0][0] = R.X;
		Values[0][1] = R.Y;
		Values[0][2] = R.Z;
		Values[0][3] = 0.0f;

		Values[1][0] = U.X;
		Values[1][1] = U.Y;
		Values[1][2] = U.Z;
		Values[1][3] = 0.0f;

		Values[2][0] = F.X;
		Values[2][1] = F.Y;
		Values[2][2] = F.Z;
		Values[2][3] = 0.0f;

		Values[0][3] = -Vector3::Dot(R, eye);
		Values[1][3] = -Vector3::Dot(U, eye);
		Values[2][3] = -Vector3::Dot(F, eye);
		Values[3][3] = 1.0f;
		return *this;
	}
};

struct Vertex
{
	Vector3 Pos;
	Vector2 UV1;
	Vector2 UV2;
	std::array<uint8_t, 4> Normal;
	std::array<uint8_t, 4> Tangent;
	uint8_t Color[4];

	static std::array<uint8_t, 4> CreatePacked(const Vector3& v)
	{
		std::array<uint8_t, 4> ret;
		ret[0] = static_cast<uint8_t>((v.X + 1.0f) / 2.0f * 255);
		ret[1] = static_cast<uint8_t>((v.Y + 1.0f) / 2.0f * 255);
		ret[2] = static_cast<uint8_t>((v.Z + 1.0f) / 2.0f * 255);
		return ret;
	}

	static Vector3 CreateUnpacked(const std::array<uint8_t, 4>& v)
	{
		Vector3 ret;
		ret.X = v[0] / 255.0f * 2.0f - 1.0f;
		ret.Y = v[1] / 255.0f * 2.0f - 1.0f;
		ret.Z = v[2] / 255.0f * 2.0f - 1.0f;
		return ret;
	}
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
		auto v1 = cp1[i] - cp0[i];
		auto v2 = cp2[i] - cp1[i];
		auto abc = Vector3::Cross(v1, v2);

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
	ar::Manager* manager = nullptr;
	ar::Compiler* compiler = nullptr;

public:
	bool Initialize(int32_t width, int32_t height);
	Graphics();
	virtual ~Graphics();

	ar::Manager* GetManager() { return manager; }
	ar::Compiler* GetCompiler() { return compiler; }
};

class Texture
{
private:
	std::string path_;
	std::shared_ptr<Graphics> graphics_;
	ar::Texture2D* texture_ = nullptr;

public:
	Texture();
	virtual ~Texture();

	bool Validate();
	void Invalidate();

	uint64_t GetInternal();

	const std::string GetPath() const { return path_; }

	ar::Texture2D* GetTexture() { return texture_; }
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
	ar::VertexBuffer* vb = nullptr;
	ar::IndexBuffer* ib = nullptr;
	int32_t indexCount = 0;

public:
	Mesh();
	virtual ~Mesh();

	ar::VertexBuffer* GetVertexBuffer() { return vb; }

	ar::IndexBuffer* GetIndexBuffer() { return ib; }

	int32_t GetIndexCount() { return indexCount; }

	static std::shared_ptr<Mesh> Load(std::shared_ptr<Graphics> graphics, const char* path);
};

class Preview
{
private:
	std::shared_ptr<Graphics> graphics_;
	ar::RenderTexture2D* renderTarget_ = nullptr;
	ar::DepthTexture* depthTarget_ = nullptr;
	ar::Shader* shader = nullptr;
	ar::VertexBuffer* vb = nullptr;
	ar::IndexBuffer* ib = nullptr;
	ar::ConstantBuffer* constantBuffer = nullptr;
	ar::Context* context = nullptr;
	std::vector<std::shared_ptr<TextureWithSampler>> textures_;
	std::shared_ptr<Mesh> mesh_;
	std::shared_ptr<Texture> black_;
	std::shared_ptr<Texture> white_;

public:
	Preview();
	virtual ~Preview();
	bool Initialize(std::shared_ptr<Graphics> graphics);
	bool CompileShader(std::string& vs,
					   std::string& ps,
					   std::vector<std::shared_ptr<TextureWithSampler>> textures,
					   std::vector<std::shared_ptr<TextExporterUniform>>& uniforms);
	bool UpdateUniforms(std::vector<std::shared_ptr<TextureWithSampler>> textures,
						std::vector<std::shared_ptr<TextExporterUniform>>& uniforms);
	bool UpdateConstantValues(float time, std::array<float, 4> customData1, std::array<float, 4> customData2);
	void Render();

	std::string VS;
	std::string PS;

	PreviewModelType ModelType = PreviewModelType::Screen;

	uint64_t GetInternal();

	static const int32_t TextureSize = 128;
};

} // namespace EffekseerMaterial

namespace std
{
template <> struct hash<EffekseerMaterial::Vector3>
{
	size_t operator()(const EffekseerMaterial::Vector3& _Keyval) const noexcept
	{
		return std::hash<float>()(_Keyval.X) + std::hash<float>()(_Keyval.Y) + std::hash<float>()(_Keyval.Z);
	}
};

template <> struct hash < std::tuple<EffekseerMaterial::Vector3, EffekseerMaterial::Vector3>>
{
	size_t operator()(const std::tuple<EffekseerMaterial::Vector3, EffekseerMaterial::Vector3>& _Keyval) const noexcept
	{
		return std::hash<EffekseerMaterial::Vector3>()(std::get<0>(_Keyval)) +
			   std::hash<EffekseerMaterial::Vector3>()(std::get<1>(_Keyval));
	}
};

} // namespace std