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

std::string GetDirectoryName(const std::string &path) {
  const std::string::size_type pos =
      std::max<int32_t>(path.find_last_of('/'), path.find_last_of('\\'));
  return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}

std::string GetExecutingDirectory() {
  char buf[260];

#ifdef _WIN32
  int len = GetModuleFileNameA(nullptr, buf, 260);
  if (len <= 0)
    return "";
#elif defined(__APPLE__)
  uint32_t size = 260;
  if (_NSGetExecutablePath(buf, &size) != 0) {
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

void SetCurrentDir(const char *path) {
#ifdef _WIN32
  _chdir(path);
#else
  chdir(path);
#endif
}

int main(int argc, char *argv[]) {

  SetCurrentDir(GetExecutingDirectory().c_str());

  const auto cmd = "../Resources/Effekseer";
  auto fp = popen(cmd, "r");
  if (fp == nullptr) {
    std::cout << "Failed to call " << cmd << std::endl;
    return 1;
  }

  std::array<char, 256> buffer;

  while (!feof(fp)) {
    fgets(buffer.data(), buffer.size(), fp);
    std::cout << buffer.data();
  }

  pclose(fp);

  std::cout << "Finished " << cmd << std::endl;

  return 0;
}