#include "VectorFieldLoader.h"
#include "../Effekseer.DefaultFile.h"

namespace Effekseer
{

// https://qiita.com/YuichiSato/items/bc160896b5883f1a125b

namespace
{

enum class VectorFieldType
{
	Scalar,
	Vector,
	Uknown,
};

#pragma pack(push, 1)
struct VectorFieldHeader
{
	char FourCC[4];
	std::array<uint16_t, 3> Size;

	VectorFieldType GetType() const
	{
		const char* VF_F = "VF_F";
		const char* VF_V = "VF_V";

		bool is_f = true;
		for (int i = 0; i < 4; i++)
		{
			if (FourCC[i] != VF_F[i])
			{
				is_f = false;
			}
		}

		if (is_f)
		{
			return VectorFieldType::Scalar;
		}

		bool is_v = true;
		for (int i = 0; i < 4; i++)
		{
			if (FourCC[i] != VF_V[i])
			{
				is_v = false;
			}
		}

		if (is_v)
		{
			return VectorFieldType::Vector;
		}

		return VectorFieldType::Uknown;
	}
};
#pragma pack(pop)

} // namespace

VectorFieldLoader::VectorFieldLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	if (fileInterface != nullptr)
	{
		fileInterface_ = fileInterface;
	}
	else
	{
		fileInterface_ = MakeRefPtr<DefaultFileInterface>();
	}
}

VectorFieldRef VectorFieldLoader::Load(const char16_t* path)
{
	auto reader = fileInterface_->OpenRead(path);
	if (reader == nullptr)
	{
		return nullptr;
	}

	size_t size = reader->GetLength();
	std::vector<uint8_t> data;
	data.resize(size);

	reader->Read(data.data(), size);

	return Load(data.data(), static_cast<int32_t>(size));
}

VectorFieldRef VectorFieldLoader::Load(const void* data, int32_t size)
{
	if (size < sizeof(VectorFieldHeader))
	{
		return nullptr;
	}

	auto header = reinterpret_cast<const VectorFieldHeader*>(data);
	auto buffer = reinterpret_cast<const uint8_t*>(data) + sizeof(VectorFieldHeader);

	int stride = 1;
	if (header->GetType() == VectorFieldType::Scalar)
	{
		stride = 1;
	}
	else if (header->GetType() == VectorFieldType::Vector)
	{
		stride = 3;
	}

	CustomAlignedVector<float> values;
	values.assign(buffer, buffer + (header->Size[0] * header->Size[1] * header->Size[2] * stride));

	VectorFieldRef ret = MakeRefPtr<VectorField>();

	ret->Init(
		std::array<int32_t, 3>{header->Size[0], header->Size[1], header->Size[2]}, stride);

	for (int z = 0; z < header->Size[2]; z++)
	{
		for (int y = 0; y < header->Size[1]; y++)
		{
			for (int x = 0; x < header->Size[0]; x++)
			{
				const auto ind = x + y * header->Size[0] + z * header->Size[0] * header->Size[1];

				if (stride == 1)
				{
					float v = values[ind];
					ret->SetData({v, 0, 0}, {x, y, z});
				}
				else if (stride == 3)
				{
					ret->SetData({values[ind * 3 + 0], values[ind * 3 + 1], values[ind * 3 + 2]}, {x, y, z});
				}
			}
		}
	}

	return ret;
}

void VectorFieldLoader::Unload(VectorFieldRef data)
{
}

} // namespace Effekseer