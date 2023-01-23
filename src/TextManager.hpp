#pragma once

#include <Core.hpp>

struct TextManager {
	static void SetTextNode(osgText::Text* text);
	static void SetTimeNode(osgText::Text* text);
	static void AddPoints(uint32_t points);
	static void SetTime(double time);
};

