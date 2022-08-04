#pragma once

#include <memory>
#include <stdint.h>
#include <stdio.h>

namespace Utils
{

class Input
{
public:
	static void UpdateKeyState(int key, bool down);

	static bool IsKeyPressed(int key);

	static bool IsKeyReleased(int key);

private:
	struct KeyState
	{
		bool down = false;
		bool lastDown = false;
	};
	static KeyState keyStateList[256];
};

} // namespace Utils
