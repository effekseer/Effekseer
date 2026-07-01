#include "EfkRes.EfkCurveSaver.h"
#include "EfkRes.EfkModelSaver.h"
#include "EfkRes.FBXLoader.h"
#include "EfkRes.GEOLoader.h"
#include "EfkRes.GLTFLoader.h"
#include "EfkRes.MQOLoader.h"
#include "EfkRes.Math.h"
#include <algorithm>
#include <charconv>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

namespace
{

std::string GetLowerExtension(const std::string& path)
{
	size_t index = path.find_last_of('.');
	if (index == path.npos)
	{
		return "";
	}

	std::string ext = path.substr(index);
	std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(tolower(c)); });
	return ext;
}

} // namespace

struct MainArgs
{
public:
	std::string inputFile;
	std::string inputFileExt;
	std::string outputFile;
	std::string outputFileExt;
	double modelScale = 1.0f;

public:
	bool Parse(int argc, char* argv[])
	{
		std::string_view option = "";
		for (int i = 1; i < argc; i++)
		{
			std::string_view arg(argv[i]);

			if (arg[0] == '-')
			{
				option = arg;
			}
			else
			{
				if (option == "-o")
				{
					outputFile = arg;
				}
				else if (option == "-s")
				{
					modelScale = strtod(arg.data(), nullptr);
				}
				else if (option == "")
				{
					inputFile = arg;
				}
				option = "";
			}
		}

		if (inputFile.empty())
		{
			return false;
		}
		inputFileExt = GetLowerExtension(inputFile);
		if (outputFile.empty())
		{
			outputFile = inputFile;
			size_t index = outputFile.find_last_of('.');
			if (index != outputFile.npos)
			{
				outputFile.erase(index);
			}
			outputFile += ".efkmodel";
		}
		outputFileExt = GetLowerExtension(outputFile);

		return true;
	}

	void PrintUsage()
	{
		printf("Effekseer Resource Conveter\n");
		printf("Usage: InputFile {-o OutputFile} {Options}\n");
		printf("\n");
		printf("InputFile:\n");
		printf("    Supports: *.fbx, *.gltf, *.glb, *.obj, *.mqo, *.geo, *.bgeo\n");
		printf("OutputFile:\n");
		printf("    Supports: *.efkmodel, *.efkcurve\n");
		printf("Options:");
		printf("    -s: Model Scale (0.0 ~ 10000.0)\n");
	}
};

int main(int argc, char* argv[])
{
	using namespace efkres;

	MainArgs args;
	if (!args.Parse(argc, argv))
	{
		args.PrintUsage();
		return -1;
	}

	if (args.outputFileExt == ".efkmodel")
	{
		std::optional<std::vector<Model>> modelFrames;

		if (args.inputFileExt == ".fbx" || args.inputFileExt == ".obj")
		{
			FBXLoader fbxLoader;
			modelFrames = fbxLoader.LoadModelSequence(args.inputFile);
		}
		else if (args.inputFileExt == ".gltf" || args.inputFileExt == ".glb")
		{
			GLTFLoader gltfLoader;
			auto model = gltfLoader.LoadModel(args.inputFile);
			if (model.has_value())
			{
				modelFrames.emplace();
				modelFrames->emplace_back(std::move(model.value()));
			}
		}
		else if (args.inputFileExt == ".mqo")
		{
			MQOLoader mqoLoader;
			auto model = mqoLoader.LoadModel(args.inputFile);
			if (model.has_value())
			{
				modelFrames.emplace();
				modelFrames->emplace_back(std::move(model.value()));
			}
		}
		else if (args.inputFileExt == ".geo" || args.inputFileExt == ".bgeo")
		{
			GEOLoader geoLoader;
			auto model = geoLoader.LoadModel(args.inputFile);
			if (model.has_value())
			{
				modelFrames.emplace();
				modelFrames->emplace_back(std::move(model.value()));
			}
		}

		if (!modelFrames.has_value())
		{
			fprintf(stderr, "Failed to load: %s\n", args.inputFile.c_str());
			return -1;
		}
		else
		{
			EfkModelSaver efkModelSaver;
			efkModelSaver.SetModelScale(args.modelScale);
			if (!efkModelSaver.Save(args.outputFile, modelFrames.value()))
			{
				fprintf(stderr, "Failed to save: %s\n", args.outputFile.c_str());
				return -1;
			}
		}
	}
	else if (args.outputFileExt == ".efkcurve")
	{
		std::optional<Curve> curve;

		if (args.inputFileExt == ".fbx")
		{
			FBXLoader fbxLoader;
			curve = fbxLoader.LoadCurve(args.inputFile);
		}

		if (!curve.has_value())
		{
			fprintf(stderr, "Failed to load: %s\n", args.inputFile.c_str());
			return -1;
		}
		else
		{
			EfkCurveSaver efkCurveSaver;
			if (!efkCurveSaver.Save(args.outputFile, curve.value()))
			{
				fprintf(stderr, "Failed to save: %s\n", args.outputFile.c_str());
				return -1;
			}
		}
	}

	return 0;
}
