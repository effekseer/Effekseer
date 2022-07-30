#pragma once

#include <stdio.h>
#include <stdint.h>
#include <memory>

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

}
