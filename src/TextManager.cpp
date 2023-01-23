#include "TextManager.hpp"

struct TextManagerData {
	osgText::Text* text = nullptr;
	osgText::Text* time = nullptr;
	osgText::Text* effic = nullptr;
	uint32_t score = 0;
};

static TextManagerData textManagerData{};

void TextManager::SetTextNode(osgText::Text* text) {
	textManagerData.text = text;
}
void TextManager::SetTimeNode(osgText::Text* text) {
	textManagerData.time = text;
}
void TextManager::SetEfficNode(osgText::Text* text) {
	textManagerData.effic = text;
}

void TextManager::AddPoints(uint32_t points) {
	if (textManagerData.text == nullptr) return;

	textManagerData.score += points;
	textManagerData.text->setText(fmt::format("Score: {}", textManagerData.score));
}

void TextManager::SetTime(double time) {
	textManagerData.time->setText(fmt::format("Time: {:4.2f}", time));
}

void TextManager::UpdateEffic(uint32_t shots) {
	textManagerData.effic->setText(fmt::format("Efficiency: {:4.2f}", static_cast<double>(textManagerData.score) / shots));
}