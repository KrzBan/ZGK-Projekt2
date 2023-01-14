#include "TextManager.hpp"

struct TextManagerData {
	osgText::Text* text = nullptr;
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