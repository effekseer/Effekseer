#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>


std::u16string ToU16Str(const char* text);

std::string GetDirectoryPath(const char* path);

std::u16string GetDirectoryPathAsU16(const char* path);

std::string GetFileNameWithoutExtention(const std::string& path);

std::string GetParentDirectory(const std::string& path);
