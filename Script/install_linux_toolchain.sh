#!/bin/sh
set -eu

# Update these pins deliberately, then compare both clean launcher builds.
# Do not silently fall back to another compiler if a package is unavailable.
gcc_version=8.5.0-30.el8_10
cmake_version=3.31.6
dnf install -y "gcc-$gcc_version" "gcc-c++-$gcc_version" make python3.12 python3.12-pip
python3.12 -m pip install "cmake==$cmake_version"
test "$(rpm -q --qf '%{VERSION}-%{RELEASE}' gcc)" = "$gcc_version"
test "$(rpm -q --qf '%{VERSION}-%{RELEASE}' gcc-c++)" = "$gcc_version"
test "$(cmake --version | head -n 1)" = "cmake version $cmake_version"
