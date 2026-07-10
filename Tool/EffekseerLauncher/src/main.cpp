#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#else
#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

extern char** environ;
#endif

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

std::string GetDirectoryName(const std::string& path)
{
	const std::string::size_type pos = path.find_last_of("/\\");
	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}

std::string GetExecutingDirectory()
{
#ifdef _WIN32
	std::vector<char> buffer(1024);
	for (;;)
	{
		const auto len = GetModuleFileNameA(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
		if (len == 0)
			return {};
		if (len < buffer.size() - 1)
			return GetDirectoryName(buffer.data());
		buffer.resize(buffer.size() * 2);
	}
#elif defined(__APPLE__)
	std::vector<char> buffer(1024);
	uint32_t size = static_cast<uint32_t>(buffer.size());
	if (_NSGetExecutablePath(buffer.data(), &size) != 0)
	{
		buffer.resize(size);
		if (_NSGetExecutablePath(buffer.data(), &size) != 0)
			return {};
	}
	return GetDirectoryName(buffer.data());
#else
	char temp[32];
	sprintf(temp, "/proc/%d/exe", getpid());
	std::vector<char> buffer(1024);
	for (;;)
	{
		const auto bytes = readlink(temp, buffer.data(), buffer.size() - 1);
		if (bytes < 0)
			return {};
		if (static_cast<size_t>(bytes) < buffer.size() - 1)
		{
			buffer[bytes] = '\0';
			return GetDirectoryName(buffer.data());
		}
		buffer.resize(buffer.size() * 2);
	}
#endif
}

bool SetCurrentDir(const char* path)
{
#ifdef _WIN32
	return _chdir(path) == 0;
#else
	return chdir(path) == 0;
#endif
}

#ifdef _WIN32
struct Platform
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	Platform()
	{
		ZeroMemory(&pi, sizeof(pi));
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
	}

	~Platform()
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	bool Execute(const char* cmd)
	{
		return CreateProcess(nullptr, (LPSTR)cmd, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi) == TRUE;
	}

	void Wait()
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
	}
};
#else
struct Platform
{
	pid_t pid = -1;
	Platform() = default;
	~Platform()
	{
		Wait();
	}

	bool Execute(const std::string& executable, const std::vector<std::string>& arguments)
	{
		std::vector<char*> argv;
		argv.reserve(arguments.size() + 2);
		argv.push_back(const_cast<char*>(executable.c_str()));
		for (const auto& argument : arguments)
		{
			argv.push_back(const_cast<char*>(argument.c_str()));
		}
		argv.push_back(nullptr);

		const auto result = posix_spawn(&pid, executable.c_str(), nullptr, nullptr, argv.data(), environ);
		if (result != 0)
		{
			pid = -1;
			std::cerr << "posix_spawn failed: " << std::strerror(result) << std::endl;
			return false;
		}

		return true;
	}

	void Wait()
	{
		if (pid < 0)
			return;

		int status = 0;
		while (waitpid(pid, &status, 0) < 0 && errno == EINTR)
		{
		}
		pid = -1;
	}
};
#endif

int mainLoop(int argc, char* argv[])
{
	const auto executingDirectory = GetExecutingDirectory();
	if (executingDirectory.empty() || !SetCurrentDir(executingDirectory.c_str()))
	{
		std::cerr << "Failed to set the executing directory." << std::endl;
		return 1;
	}

	std::string cmd;

#ifdef _WIN32
	cmd = ".\\bin\\Effekseer.exe";
#elif defined(__APPLE__)
	cmd = "../Resources/Effekseer";
#else
	cmd = "./bin/Effekseer";
#endif

#ifdef _WIN32
	for (int i = 1; i < argc; i++)
	{
		cmd = cmd + " \"" + argv[i] + "\"";
	}
#else
	std::vector<std::string> arguments;
	arguments.reserve(argc > 1 ? static_cast<size_t>(argc - 1) : 0);
	for (int i = 1; i < argc; i++)
	{
		arguments.emplace_back(argv[i]);
	}
#endif

	Platform platform;
#ifdef _WIN32
	if (!platform.Execute(cmd.c_str()))
#else
	if (!platform.Execute(cmd, arguments))
#endif
	{
		std::cout << "Failed to call " << cmd << std::endl;
		return 1;
	}

#ifndef _WIN32
	platform.Wait();
#endif
	std::cout << "Finished " << cmd << std::endl;
	return 0;
}

#if defined(NDEBUG) && defined(_WIN32)
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
