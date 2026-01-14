#pragma once

#include "EfkRes.Model.h"
#include <stdio.h>
#include <string>
#include <string_view>
#include <type_traits>

namespace efkres
{

class BinaryReader
{
	FILE* m_fp = nullptr;

public:
	BinaryReader() = default;

	~BinaryReader() { Close(); }

	bool Open(const char* filepath)
	{
#if defined(_WIN32)
		errno_t err = fopen_s(&m_fp, filepath, "rb");
		return err == 0;
#else
		m_fp = fopen(filepath, "rb");
		return m_fp != nullptr;
#endif
	}

	void Close()
	{
		if (m_fp)
		{
			fclose(m_fp);
		}
	}

	void Read(void* data, size_t size) { fread(data, size, 1, m_fp); }

	template <class T, std::enable_if_t<std::is_arithmetic_v<T>, std::nullptr_t> = nullptr> T Read()
	{
		T data{};
		fread(&data, sizeof(data), 1, m_fp);
		return data;
	}

	template <class T, size_t N> std::array<T, N> Read()
	{
		std::array<T, N> data{};
		fread(data.data(), sizeof(T) * N, 1, m_fp);
		return data;
	}

	void SetPosition(size_t position) { fseek(m_fp, static_cast<long>(position), SEEK_SET); }

	void MovePosition(int offset) { fseek(m_fp, offset, SEEK_CUR); }

	size_t GetPosition() const { return static_cast<size_t>(ftell(m_fp)); }
};

class BinaryWriter
{
	FILE* m_fp = nullptr;

public:
	BinaryWriter() = default;

	~BinaryWriter() { Close(); }

	bool Open(const char* filepath)
	{
#if defined(_WIN32)
		errno_t err = fopen_s(&m_fp, filepath, "wb");
		return err == 0;
#else
		m_fp = fopen(filepath, "wb");
		return m_fp != nullptr;
#endif
	}

	void Close()
	{
		if (m_fp)
		{
			fclose(m_fp);
		}
	}

	void Write(const void* data, size_t size) { fwrite(data, size, 1, m_fp); }

	template <class T, std::enable_if_t<std::is_arithmetic_v<T>, std::nullptr_t> = nullptr> void Write(T data)
	{
		fwrite(&data, sizeof(data), 1, m_fp);
	}

	template <class T, size_t N> void Write(const std::array<T, N>& data) { fwrite(data.data(), sizeof(T) * N, 1, m_fp); }

	void SetPosition(size_t position) { fseek(m_fp, static_cast<long>(position), SEEK_SET); }

	size_t GetPosition() const { return static_cast<size_t>(ftell(m_fp)); }
};

inline void CalcTangentSpace(MeshVertex& v1, MeshVertex& v2, MeshVertex& v3)
{
	Vec3 cp0[3];
	cp0[0] = Vec3(v1.position.x, v1.uv1.x, v1.uv1.y);
	cp0[1] = Vec3(v1.position.y, v1.uv1.x, v1.uv1.y);
	cp0[2] = Vec3(v1.position.z, v1.uv1.x, v1.uv1.y);

	Vec3 cp1[3];
	cp1[0] = Vec3(v2.position.x, v2.uv1.x, v2.uv1.y);
	cp1[1] = Vec3(v2.position.y, v2.uv1.x, v2.uv1.y);
	cp1[2] = Vec3(v2.position.z, v2.uv1.x, v2.uv1.y);

	Vec3 cp2[3];
	cp2[0] = Vec3(v3.position.x, v3.uv1.x, v3.uv1.y);
	cp2[1] = Vec3(v3.position.y, v3.uv1.x, v3.uv1.y);
	cp2[2] = Vec3(v3.position.z, v3.uv1.x, v3.uv1.y);

	double u[3];
	double v[3];

	for (int32_t i = 0; i < 3; i++)
	{
		auto v1 = cp1[i] - cp0[i];
		auto v2 = cp2[i] - cp1[i];
		auto abc = CrossProduct(v1, v2);

		if (abc.x == 0.0f)
		{
			return;
		}
		else
		{
			u[i] = -abc.y / abc.x;
			v[i] = -abc.z / abc.x;
		}
	}

	Vec3 tangent = Vec3(u[0], u[1], u[2]).Normalized();
	Vec3 binormal = Vec3(v[0], v[1], v[2]).Normalized();

	v1.tangent = tangent;
	v2.tangent = tangent;
	v3.tangent = tangent;

	v1.binormal = binormal;
	v2.binormal = binormal;
	v3.binormal = binormal;
}

} // namespace efkres
