#include "Effekseer.CurveLoader.h"
#include "Utils/Effekseer.BinaryReader.h"

namespace Effekseer
{

CurveLoader::CurveLoader(::Effekseer::FileInterfaceRef fileInterface)
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

CurveRef CurveLoader::Load(const char16_t* path)
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

CurveRef CurveLoader::Load(const void* data, int32_t size)
{
	BinaryReader<false> reader((uint8_t*)(data), size);

	auto curve = Effekseer::MakeRefPtr<Effekseer::Curve>();

	// load converter version
	int converter_version = 0;
	reader.Read(converter_version);

	// load controll point count
	reader.Read(curve->controlPointCount_);

	// load controll points
	for (int i = 0; i < curve->controlPointCount_; i++)
	{
		dVector4 value;
		reader.Read(value);
		curve->controlPoints_.push_back(value);
	}

	// load knot count
	reader.Read(curve->knotCount_);

	// load knot values
	for (int i = 0; i < curve->knotCount_; i++)
	{
		double value;
		reader.Read(value);
		curve->knotValues_.push_back(value);
	}

	// load order
	reader.Read(curve->order_);

	// load step
	reader.Read(curve->step_);

	// load type
	reader.Read(curve->type_);

	// load dimension
	reader.Read(curve->dimension_);

	// calc curve length
	curve->length_ = 0;

	for (int i = 1; i < curve->controlPointCount_; i++)
	{
		dVector4 p0 = curve->controlPoints_[i - 1];
		dVector4 p1 = curve->controlPoints_[i];

		float len = Vector3D::Length(Vector3D((float)p1.X, (float)p1.Y, (float)p1.Z) - Vector3D((float)p0.X, (float)p0.Y, (float)p0.Z));
		curve->length_ += len;
	}

	return curve;
}

void CurveLoader::Unload(CurveRef data)
{
}

} // namespace Effekseer