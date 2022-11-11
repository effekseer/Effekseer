#include <cassert>
#include <chrono>
#include "Noise/CurlNoise.h"

#define ASSERT(e) assert(e)
//#define ASSERT(e) if (!(e)) printf("assert failed.\n");

using namespace Effekseer;

void Test_Turbulence()
{
	using namespace std::chrono;

	const uint32_t IterationCount = 10;

	printf("Turbulence Test\n");

	CurlNoise noise(0);
	noise.Octave = 1;
	noise.Scale = 1;

	Vec3f position{0.0f, 0.0f, 0.0f};
	for (uint32_t iteration = 0; iteration < IterationCount; iteration++) {
		position = noise.Get(position);
		printf("- (%f, %f, %f)\n", position.GetX(), position.GetY(), position.GetZ());
	}
}

void MeasurePerformance_Turbulence()
{
	using namespace std::chrono;

	const uint32_t IterationCount = 10;
	const uint32_t CallCount = 100;
	const uint32_t InstanceCount = 1000;

	std::vector<std::unique_ptr<CurlNoise>> noise;
	for (uint32_t instanceIndex = 0; instanceIndex < InstanceCount; instanceIndex++) {
		noise.emplace_back(new CurlNoise(instanceIndex));
		noise[instanceIndex]->Octave = 1.0f;
		noise[instanceIndex]->Scale = 1.0f;
	}

	printf("Turbulence Performance - Calls=%u, Instances:%u\n", CallCount, InstanceCount);

	for (uint32_t iteration = 0; iteration < IterationCount; iteration++) {
		auto t0 = high_resolution_clock::now();
	
		for (size_t calling = 0; calling < CallCount; calling++) {
			for (size_t instanceIndex = 0; instanceIndex < InstanceCount; instanceIndex++) {
				noise[instanceIndex]->Get(Vec3f(0.0f, 0.0f, 0.0f));
			}
		}

		auto t1 = high_resolution_clock::now();

		printf("- elapsed: %lld[usec]\n", duration_cast<microseconds>(t1 - t0).count());
	}
}

int main(int argc, char *argv[])
{
	Test_Turbulence();
	MeasurePerformance_Turbulence();

	return 0;
}
