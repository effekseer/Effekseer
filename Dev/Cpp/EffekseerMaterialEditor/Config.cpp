#include "Config.h"
#include "ThirdParty/picojson.h"
#include <fstream>
#include <iostream>

namespace EffekseerMaterial
{

Config::Config() { Language = Effekseer::GetSystemLanguage(); }

Config::~Config() {}

bool Config::Save(const char* path)
{

	picojson::object root;

	root.insert(std::make_pair("Project", picojson::value("EffekseerMaterialConfig")));
	root.insert(std::make_pair("WindowWidth", picojson::value((double)WindowWidth)));
	root.insert(std::make_pair("WindowHeight", picojson::value((double)WindowHeight)));
	root.insert(std::make_pair("WindowPosX", picojson::value((double)WindowPosX)));
	root.insert(std::make_pair("WindowPosY", picojson::value((double)WindowPosY)));
	root.insert(std::make_pair("WindowIsMaximumMode", picojson::value((bool)WindowIsMaximumMode)));
	root.insert(std::make_pair("Language", picojson::value((double)Language)));

	auto serialized = picojson::value(root).serialize();

	std::ofstream outputfile(path);
	if (outputfile.bad())
	{
		return false;
	}

	outputfile << serialized;
	outputfile.close();
	return true;
}

bool Config::Load(const char* path)
{
	std::ifstream ifs(path);
	if (ifs.fail())
	{
		return false;
	}
	std::string serialized((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	picojson::value root;
	auto err = picojson::parse(root, serialized);
	if (!err.empty())
	{
		std::cerr << err << std::endl;
		return false;
	}

	picojson::value projectNameObj = root.get("Project");
	auto projectName = projectNameObj.get<std::string>();
	if (projectName != "EffekseerMaterialConfig")
		return false;

	WindowWidth = root.get("WindowWidth").get<double>();
	WindowHeight = root.get("WindowHeight").get<double>();
	WindowPosX = root.get("WindowPosX").get<double>();
	WindowPosY = root.get("WindowPosY").get<double>();

	if (root.contains("WindowIsMaximumMode"))
	{
		WindowIsMaximumMode = root.get("WindowIsMaximumMode").get<bool>();	
	}

	Language = static_cast<Effekseer::SystemLanguage>((int)root.get("Language").get<double>());

	return true;
}

} // namespace EffekseerMaterial