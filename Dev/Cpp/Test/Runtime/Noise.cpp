#include <chrono>
#include "Effekseer/Noise/CurlNoise.h"
#include "Effekseer/Effekseer.Random.h"
#include "../TestHelper.h"

using namespace std::chrono;
using namespace Effekseer;
using namespace Effekseer::SIMD;

void TestNoise()
{
	const size_t ITERATIONS = 20;
	const size_t COUNT = 100000;

	{
		LightCurlNoise noise(1, 1.0f, 4);
		RandObject rand;

		{
			auto v = noise.Get(Vec3f(1.0f, 2.0f, 3.0f));
			//printf("%f, %f, %f\n", v.GetX(), v.GetY(), v.GetZ());
			if (!Vec3f::Equal(v, Vec3f(-0.247059f, -0.035294f, 0.835294f)))
				throw "Failed";
		}

		{
			auto v = noise.Get(Vec3f(-0.1, -0.2, -0.3));
			//printf("%f, %f, %f\n", v.GetX(), v.GetY(), v.GetZ());
			if (!Vec3f::Equal(v, Vec3f(-0.105067f, 0.126212f, -0.292988f)))
				throw "Failed";
		}

		auto perf = TestPerformance(ITERATIONS, [&]()
		{
			for (size_t count = 0; count < COUNT; count++)
			{
				float f = rand.GetRand();
				noise.Get(Vec3f(f, 1.0f - f, 1.0f + f));
			}
		});
		perf.Print("LightCurlNoise");
	}

	{
		CurlNoise noise(1, 1.0f, 4);
		RandObject rand;

		/*{
			auto v = noise.Get(Vec3f(1.0f, 2.0f, 3.0f));
			//printf("%f, %f, %f\n", v.GetX(), v.GetY(), v.GetZ());
			if (!Vec3f::Equal(v, Vec3f(-0.247059f, -0.035294f, 0.835294f)))
				throw "Failed";
		}

		{
			auto v = noise.Get(Vec3f(-0.1, -0.2, -0.3));
			//printf("%f, %f, %f\n", v.GetX(), v.GetY(), v.GetZ());
			if (!Vec3f::Equal(v, Vec3f(-0.105067f, 0.126212f, -0.292988f)))
				throw "Failed";
		}*/

		auto perf = TestPerformance(ITERATIONS, [&]()
		{
			for (size_t count = 0; count < COUNT; count++)
			{
				float f = rand.GetRand();
				noise.Get(Vec3f(f, 1.0f - f, 1.0f + f));
			}
		});
		perf.Print("CurlNoise");
	}
}

TestRegister Runtime_Noise("Runtime.Noise", []() -> void { TestNoise(); });
