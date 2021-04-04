
#pragma once
#include "../GUI/efk.GUIManager.h"

class NodeFrameTimeline
{
public:
	static bool BeginNodeFrameTimeline();
	static void TimelineNode(const char* title);
	static void EndNodeFrameTimeline();
};
