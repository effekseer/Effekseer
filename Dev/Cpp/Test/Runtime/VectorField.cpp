#include "../TestHelper.h"
#include <Effekseer.h>

void VectorFieldTest()
{
	{
		Effekseer::VectorField vectorField;
		vectorField.Init({2, 2, 2}, 1);

		for (int z = 0; z < 2; z++)
		{
			for (int y = 0; y < 2; y++)
			{
				for (int x = 0; x < 2; x++)
				{
					vectorField.SetData(
						{static_cast<float>(x) + static_cast<float>(y) * 2 + static_cast<float>(z) * 4, 0, 0},
						{x, y, z});
				}
			}
		}

		{
			auto v = vectorField.GetData(std::array<float, 3>{0.0f, 0.0f, 0.0f}, Effekseer::VectorFieldSamplingMode::Clamp);
			EXPECT_TRUE(v[0] == 0.0f);
		}

		{
			auto v = vectorField.GetData(std::array<float, 3>{0.5f, 0.0f, 0.0f}, Effekseer::VectorFieldSamplingMode::Clamp);
			EXPECT_TRUE(v[0] == 0.5f);
		}

		{
			auto v = vectorField.GetData(std::array<float, 3>{0.25f, 0.0f, 0.0f}, Effekseer::VectorFieldSamplingMode::Clamp);
			EXPECT_TRUE(v[0] == 0.25f);
		}

		{
			auto v = vectorField.GetData(std::array<float, 3>{0.0f, 0.5f, 0.0f}, Effekseer::VectorFieldSamplingMode::Clamp);
			EXPECT_TRUE(v[0] == 1.0f);
		}

		{
			auto v = vectorField.GetData(std::array<float, 3>{0.0f, 0.0f, 0.25f}, Effekseer::VectorFieldSamplingMode::Clamp);
			EXPECT_TRUE(v[0] == 1.0f);
		}

		{
			auto v = vectorField.GetData(std::array<float, 3>{0.0f, 0.0f, 0.5f}, Effekseer::VectorFieldSamplingMode::Clamp);
			EXPECT_TRUE(v[0] == 2.0f);
		}
	}

	{
		Effekseer::VectorField vectorField;
		vectorField.Init({2, 2, 2}, 3);

		for (int z = 0; z < 2; z++)
		{
			for (int y = 0; y < 2; y++)
			{
				for (int x = 0; x < 2; x++)
				{
					auto v = static_cast<float>(x) + static_cast<float>(y) * 2 + static_cast<float>(z) * 4;
					vectorField.SetData(
						{v, v * 2, v * 3},
						{x, y, z});
				}
			}
		}

		for (int i = 0; i < 3; i++)
		{
			auto eps = 0.0001f;

			{
				auto v = vectorField.GetData(std::array<float, 3>{0.0f, 0.0f, 0.0f}, Effekseer::VectorFieldSamplingMode::Clamp);
				EXPECT_EQUAL_NEAR(v[i], 0.0f * (i + 1), eps);
			}

			{
				auto v = vectorField.GetData(std::array<float, 3>{0.0f, 0.0f, 0.0f}, Effekseer::VectorFieldSamplingMode::Clamp);
				EXPECT_EQUAL_NEAR(v[i], 0.0f * (i + 1), eps);
			}

			{
				auto v = vectorField.GetData(std::array<float, 3>{0.5f, 0.0f, 0.0f}, Effekseer::VectorFieldSamplingMode::Clamp);
				EXPECT_EQUAL_NEAR(v[i], 0.5f * (i + 1), eps);
			}

			{
				auto v = vectorField.GetData(std::array<float, 3>{0.25f, 0.0f, 0.0f}, Effekseer::VectorFieldSamplingMode::Clamp);
				EXPECT_EQUAL_NEAR(v[i], 0.25f * (i + 1), eps);
			}

			{
				auto v = vectorField.GetData(std::array<float, 3>{0.0f, 0.5f, 0.0f}, Effekseer::VectorFieldSamplingMode::Clamp);
				EXPECT_EQUAL_NEAR(v[i], 1.0f * (i + 1), eps);
			}

			{
				auto v = vectorField.GetData(std::array<float, 3>{0.0f, 0.0f, 0.25f}, Effekseer::VectorFieldSamplingMode::Clamp);
				EXPECT_EQUAL_NEAR(v[i], 1.0f * (i + 1), eps);
			}

			{
				auto v = vectorField.GetData(std::array<float, 3>{0.0f, 0.0f, 0.5f}, Effekseer::VectorFieldSamplingMode::Clamp);
				EXPECT_EQUAL_NEAR(v[i], 2.0f * (i + 1), eps);
			}
		}
	}

	{
		auto vf_path = GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/VectorFields/vf_simple.vf";

		Effekseer::VectorFieldLoader loader;
		auto vectorField = loader.Load(vf_path.c_str());

		{
			auto v = vectorField->GetData(std::array<float, 3>{0.0f, 0.0f, 0.0f}, Effekseer::VectorFieldSamplingMode::Clamp);
			EXPECT_TRUE(v[0] == 0.0f);
		}
	}
}

TestRegister Runtime_VectorFieldTest("Runtime.VectorField", []() -> void
									 { VectorFieldTest(); });
