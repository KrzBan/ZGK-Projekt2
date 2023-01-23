#include "TextManager.hpp"

struct TextManagerData {
	osgText::Text* text = nullptr;
	osgText::Text* time = nullptr;
	uint32_t score = 0;
};

static TextManagerData textManagerData{};

void TextManager::SetTextNode(osgText::Text* text) {
	textManagerData.text = text;
}
void TextManager::AddPoints(uint32_t points) {
	if (textManagerData.text == nullptr) return;

	textManagerData.score += points;
	textManagerData.text->setText(fmt::format("Score: {}", textManagerData.score));
}

void TextManager::SetTimeNode(osgText::Text* text) {
	textManagerData.time = text;
}
void TextManager::SetTime(double time) {
	textManagerData.time->setText(fmt::format("Time: {:4.2f}", time));
}