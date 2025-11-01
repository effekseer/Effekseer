#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <charconv>
#include "EfkRes.Math.h"
#include "EfkRes.FBXLoader.h"
#include "EfkRes.GLTFLoader.h"
#include "EfkRes.MQOLoader.h"
#include "EfkRes.GEOLoader.h"
#include "EfkRes.EfkModelSaver.h"
#include "EfkRes.EfkCurveSaver.h"

struct MainArgs
{
public:
    std::string inputFile;
    std::string_view inputFileExt;
    std::string outputFile;
    std::string_view outputFileExt;
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
        if (size_t index = inputFile.find_last_of('.'); index != inputFile.npos)
        {
            inputFileExt = std::string_view(inputFile.data() + index, inputFile.size() - index);
        }
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
        if (size_t index = outputFile.find_last_of('.'); index != outputFile.npos)
        {
            outputFileExt = std::string_view(outputFile.data() + index, outputFile.size() - index);
        }

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
        std::optional<Model> model;

        if (args.inputFileExt == ".fbx" || args.inputFileExt == ".obj")
        {
            FBXLoader fbxLoader;
            model = fbxLoader.LoadModel(args.inputFile);
        }
        else if (args.inputFileExt == ".gltf" || args.inputFileExt == ".glb")
        {
            GLTFLoader gltfLoader;
            model = gltfLoader.LoadModel(args.inputFile);
        }
        else if (args.inputFileExt == ".mqo")
        {
            MQOLoader mqoLoader;
            model = mqoLoader.LoadModel(args.inputFile);
        }
        else if (args.inputFileExt == ".geo" || args.inputFileExt == ".bgeo")
        {
            GEOLoader geoLoader;
            model = geoLoader.LoadModel(args.inputFile);
        }

        if (!model.has_value())
        {
            fprintf(stderr, "Failed to load: %s\n", args.inputFile.c_str());
            return -1;
        }
        else
        {
            EfkModelSaver efkModelSaver;
            efkModelSaver.SetModelScale(args.modelScale);
            if (!efkModelSaver.Save(args.outputFile, model.value()))
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
