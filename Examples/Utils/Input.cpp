#include "Input.h"

namespace Utils
{

Input::KeyState Input::keyStateList[256];

void Input::UpdateKeyState(int key, bool down)
{
	keyStateList[key].lastDown = keyStateList[key].down;
	keyStateList[key].down = down;
}

bool Input::IsKeyPressed(int key)
{
	return keyStateList[key].down && !keyStateList[key].lastDown;
}

bool Input::IsKeyReleased(int key)
{
	return !keyStateList[key].down && keyStateList[key].lastDown;
}

} // namespace Utils
