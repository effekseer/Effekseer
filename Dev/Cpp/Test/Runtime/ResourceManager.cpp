#include <Effekseer.h>
#include <Effekseer/Effekseer.ResourceManager.h>
#include <Effekseer/Effekseer.TextureLoader.h>
#include <Effekseer/Model/ProceduralModelGenerator.h>
#include <Effekseer/Model/ProceduralModelParameter.h>

#include "../TestHelper.h"

class MockTextureLoader : public Effekseer::TextureLoader
{
public:
	Effekseer::TextureRef Load(const char16_t* path, Effekseer::TextureType textureType) override
	{
		return Effekseer::MakeRefPtr<Effekseer::Texture>();
	}

	Effekseer::TextureRef Load(const void* data, int32_t size, Effekseer::TextureType textureType, bool isMipMapEnabled) override
	{
		return nullptr;
	}
};

void ResourceManager_Basic()
{
	auto resourceManager = Effekseer::MakeRefPtr<Effekseer::ResourceManager>();
	resourceManager->SetTextureLoader(Effekseer::MakeRefPtr<MockTextureLoader>());
	resourceManager->SetProceduralMeshGenerator(Effekseer::MakeRefPtr<Effekseer::ProceduralModelGenerator>());

	{
		auto texture1 = resourceManager->LoadTexture(u"Test", Effekseer::TextureType::Color);
		if (texture1 == nullptr)
		{
			throw std::string("Failed.");
		}
		auto texture2 = resourceManager->LoadTexture(u"Test", Effekseer::TextureType::Color);
		if (texture1 != texture2)
		{
			throw std::string("Failed.");
		}

		resourceManager->UnloadTexture(texture1);
		resourceManager->UnloadTexture(texture2);

		if (texture1->GetRef() != 2)
		{
			throw std::string("Failed.");
		}
		texture2.Reset();
		if (texture1->GetRef() != 1)
		{
			throw std::string("Failed.");
		}
		texture1.Reset();
	}

	{
		Effekseer::ProceduralModelParameter param1;
		param1.Mesh.AngleBegin = 0.0f;
		param1.Mesh.AngleEnd = 360.0f;
		param1.Mesh.Divisions = {10, 10};
		param1.Mesh.Rotate = 0.0f;
		param1.Sphere.Radius = 1.0f;
		param1.Sphere.DepthMax = 1.0f;
		param1.Sphere.DepthMin = -1.0f;

		Effekseer::ProceduralModelParameter param2 = param1;

		auto model1 = resourceManager->GenerateProceduralModel(param1);
		auto model2 = resourceManager->GenerateProceduralModel(param2);

		if (model1 != model2)
		{
			throw std::string("Failed.");
		}

		resourceManager->UngenerateProceduralModel(model1);
		resourceManager->UngenerateProceduralModel(model2);

		if (model1->GetRef() != 2)
		{
			throw std::string("Failed.");
		}
		model2.Reset();
		if (model1->GetRef() != 1)
		{
			throw std::string("Failed.");
		}
		model1.Reset();
	}

	{
		Effekseer::ProceduralModelParameter param1;
		param1.Mesh.AngleBegin = 0.0f;
		param1.Mesh.AngleEnd = 360.0f;
		param1.Mesh.Divisions = {10, 10};
		param1.Mesh.Rotate = 0.0f;
		param1.Sphere.Radius = 1.0f;
		param1.Sphere.DepthMax = 1.0f;
		param1.Sphere.DepthMin = -1.0f;

		std::array<Effekseer::ProceduralModelParameter, 53> params;
		params.fill(param1);

		params[0].TiltNoiseFrequency[0]++;
		params[1].TiltNoiseOffset[0]++;
		params[2].TiltNoisePower[0]++;

		params[3].WaveNoiseFrequency[0]++;
		params[4].WaveNoiseOffset[0]++;
		params[5].WaveNoisePower[0]++;

		params[6].CurlNoiseFrequency[0]++;
		params[7].CurlNoiseOffset[0]++;
		params[8].CurlNoisePower[0]++;

		params[9].ColorCenterPosition[0]++;
		params[10].ColorCenterArea[0]++;

		params[11].ColorUpperLeft.R = 128;
		params[12].ColorUpperCenter.R = 128;
		params[13].ColorUpperRight.R = 128;
		params[14].ColorMiddleLeft.R = 128;
		params[15].ColorMiddleCenter.R = 128;
		params[16].ColorMiddleRight.R = 128;
		params[17].ColorLowerLeft.R = 128;
		params[18].ColorLowerCenter.R = 128;
		params[19].ColorLowerRight.R = 128;

		params[20].VertexColorNoiseFrequency[0]++;
		params[21].VertexColorNoiseOffset[0]++;
		params[22].VertexColorNoisePower[0]++;

		params[23].UVPosition[0]++;
		params[24].UVSize[0]++;

		const size_t meshOffset = 25;
		params[meshOffset + 0].Mesh.Divisions[0] = 11;
		params[meshOffset + 1].Mesh.Divisions[1] = 11;
		params[meshOffset + 2].Mesh.Divisions[0] = 9;
		params[meshOffset + 3].Mesh.Divisions[1] = 9;
		params[meshOffset + 4].Mesh.AngleBegin--;
		params[meshOffset + 5].Mesh.AngleEnd--;
		params[meshOffset + 6].Mesh.Rotate--;

		const size_t ribbonOffset = meshOffset + 7;
		params[ribbonOffset].Type = Effekseer::ProceduralModelType::Ribbon;
		params[ribbonOffset].Ribbon.CrossSection = Effekseer::ProceduralModelCrossSectionType::Cross;
		params[ribbonOffset].Ribbon.RibbonAngles = {1, 1};
		params[ribbonOffset].Ribbon.RibbonNoises = {1, 1};
		params[ribbonOffset].Ribbon.RibbonSizes = {1, 1};
		params[ribbonOffset + 1] = params[ribbonOffset];
		params[ribbonOffset + 1].Ribbon.CrossSection = Effekseer::ProceduralModelCrossSectionType::Plane;
		params[ribbonOffset + 2] = params[ribbonOffset];
		params[ribbonOffset + 2].Ribbon.RibbonAngles[0]++;
		params[ribbonOffset + 3] = params[ribbonOffset];
		params[ribbonOffset + 3].Ribbon.RibbonNoises[0]++;
		params[ribbonOffset + 4] = params[ribbonOffset];
		params[ribbonOffset + 4].Ribbon.RibbonSizes[0]++;

		const size_t sphereOffset = ribbonOffset + 5;
		params[sphereOffset].PrimitiveType = Effekseer::ProceduralModelPrimitiveType::Sphere;
		params[sphereOffset].Sphere.Radius = 2;
		params[sphereOffset].Sphere.DepthMax = 2;
		params[sphereOffset].Sphere.DepthMin = -2;

		params[sphereOffset + 1] = params[sphereOffset];
		params[sphereOffset + 1].Sphere.Radius++;

		params[sphereOffset + 2] = params[sphereOffset];
		params[sphereOffset + 2].Sphere.DepthMax++;

		params[sphereOffset + 3] = params[sphereOffset];
		params[sphereOffset + 3].Sphere.DepthMin--;

		const size_t coneOffset = sphereOffset + 4;
		params[coneOffset].PrimitiveType = Effekseer::ProceduralModelPrimitiveType::Cone;
		params[coneOffset].Cone.Depth = 1.0f;
		params[coneOffset].Cone.Radius = 1.0f;

		params[coneOffset + 1] = params[coneOffset];
		params[coneOffset].Cone.Depth++;

		params[coneOffset + 2] = params[coneOffset];
		params[coneOffset + 2].Cone.Radius++;

		const size_t cylinderOffset = coneOffset + 3;
		params[cylinderOffset].PrimitiveType = Effekseer::ProceduralModelPrimitiveType::Cylinder;
		params[cylinderOffset].Cylinder.Depth = 1.0f;
		params[cylinderOffset].Cylinder.Radius1 = 1.0f;
		params[cylinderOffset].Cylinder.Radius2 = 1.0f;

		params[cylinderOffset + 1] = params[cylinderOffset];
		params[cylinderOffset + 1].Cylinder.Depth++;
		params[cylinderOffset + 2] = params[cylinderOffset];
		params[cylinderOffset + 2].Cylinder.Radius1++;
		params[cylinderOffset + 3] = params[cylinderOffset];
		params[cylinderOffset + 3].Cylinder.Radius2++;

		const size_t spline4Offset = cylinderOffset + 4;
		params[spline4Offset].PrimitiveType = Effekseer::ProceduralModelPrimitiveType::Spline4;
		params[spline4Offset].Spline4.Point1 = {1, 1};
		params[spline4Offset].Spline4.Point2 = {1, 1};
		params[spline4Offset].Spline4.Point3 = {1, 1};
		params[spline4Offset].Spline4.Point4 = {1, 1};

		params[spline4Offset + 1] = params[spline4Offset + 0];
		params[spline4Offset + 1].Spline4.Point1[0]++;
		params[spline4Offset + 2] = params[spline4Offset + 0];
		params[spline4Offset + 2].Spline4.Point2[0]++;
		params[spline4Offset + 3] = params[spline4Offset + 0];
		params[spline4Offset + 3].Spline4.Point3[0]++;
		params[spline4Offset + 4] = params[spline4Offset + 0];
		params[spline4Offset + 4].Spline4.Point4[0]++;

		Effekseer::CustomAlignedMap<Effekseer::ProceduralModelParameter, int> m;

		for (const auto& param : params)
		{
			m[param] = 0;
		}

		if (params.size() != m.size())
		{
			throw std::string("Failed.");
		}
	}
}

TestRegister ResourceManager_Basic_Test("ResourceManager.Basic", []() -> void { ResourceManager_Basic(); });
