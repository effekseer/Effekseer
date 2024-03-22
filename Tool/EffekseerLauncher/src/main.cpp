#include <array>
#include <iostream>
#include <stdio.h>
#include <string>

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

std::string GetDirectoryName(const std::string& path)
{
	const std::string::size_type pos =
		std::max<int32_t>(path.find_last_of('/'), path.find_last_of('\\'));
	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}

std::string GetExecutingDirectory()
{
	char buf[260];

#ifdef _WIN32
	int len = GetModuleFileNameA(nullptr, buf, 260);
	if (len <= 0)
		return "";
#elif defined(__APPLE__)
	uint32_t size = 260;
	if (_NSGetExecutablePath(buf, &size) != 0)
	{
		buf[0] = 0;
	}
#else

	char temp[32];
	sprintf(temp, "/proc/%d/exe", getpid());
	int bytes = std::min((int)readlink(temp, buf, 260), 260 - 1);
	if (bytes >= 0)
		buf[bytes] = '\0';
#endif

	return GetDirectoryName(buf);
}

void SetCurrentDir(const char* path)
{
#ifdef _WIN32
	_chdir(path);
#else
	chdir(path);
#endif
}

FILE* Popen(const char* path, const char* mode)
{
#ifdef _WIN32
	return _popen(path, mode);
#else
	return popen(path, mode);
#endif
}

void Pclose(FILE* file)
{
#ifdef _WIN32
	_pclose(file);
#else
	pclose(file);
#endif
}

int mainLoop(int argc, char* argv[])
{
	SetCurrentDir(GetExecutingDirectory().c_str());

	std::string cmd;

#ifdef _WIN32
	cmd = ".\\bin\\Effekseer.exe";
#else
	cmd = "../Resources/Effekseer";
#endif

	for (int i = 1; i < argc; i++)
	{
		cmd = cmd + " \"" + argv[i] + "\"";
	}

	auto fp = Popen(cmd.c_str(), "r");
	if (fp == nullptr)
	{
		std::cout << "Failed to call " << cmd << std::endl;
		return 1;
	}

	std::array<char, 256> buffer;

	while (!feof(fp))
	{
		fgets(buffer.data(), buffer.size(), fp);
		std::cout << buffer.data();
	}

	Pclose(fp);

	std::cout << "Finished " << cmd << std::endl;

	return 0;
}

#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nShowCmd)
{
	return mainLoop(__argc, __argv);
}
#else
int main(int argc, char* argv[])
{
	return mainLoop(argc, argv);
}
#endif