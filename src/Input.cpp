#include "Input.hpp"

struct KeyState {
	bool isPressedDown = false;	//one frame down
	bool isPressed = false;		//just down
	bool released = false;		//one frame released
};


struct InputData {
	std::array<KeyState, std::numeric_limits<uint16_t>::max()> keyStates{};
	std::vector<uint16_t> releasedKeys;

	double MousePosX = 0.0;
	double MousePosY = 0.0;
	double MouseDeltaX = 0.0, MouseDeltaY = 0.0;
};

static InputData inputData;

bool Input::GetKey(int keyCode) { return inputData.keyStates[keyCode].isPressed; }
bool Input::GetKeyDown(int keyCode) { return inputData.keyStates[keyCode].isPressedDown; }
bool Input::GetKeyUp(int keyCode) { return inputData.keyStates[keyCode].released; }

double Input::GetMousePosX() { return inputData.MousePosX; }
double Input::GetMousePosY() { return inputData.MousePosY; }

double Input::GetMouseDeltaX() { return  inputData.MouseDeltaX; }
double Input::GetMouseDeltaY() { return  inputData.MouseDeltaY; }

void Input::ClearKeys() {
	inputData.MouseDeltaX = 0;
	inputData.MouseDeltaY = 0;
	for (int key : inputData.releasedKeys) {
		inputData.keyStates[key].released = false;
		inputData.keyStates[key].isPressedDown = false;
	}
	inputData.releasedKeys.clear();
}

bool InputManager::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {

	auto key = ea.getKey();
	switch (ea.getEventType()) {
	case osgGA::GUIEventAdapter::KEYDOWN:
		if(inputData.keyStates[key].isPressed == false)
			inputData.keyStates[key].isPressedDown = true;
		inputData.keyStates[key].isPressed = true;
		inputData.releasedKeys.push_back(key);
		break;
	case osgGA::GUIEventAdapter::KEYUP:
		inputData.keyStates[key].isPressedDown = false;;
		inputData.keyStates[key].isPressed = false;
		inputData.keyStates[key].released = true;
		inputData.releasedKeys.push_back(key);
		break;
	case osgGA::GUIEventAdapter::MOVE:
		inputData.MouseDeltaX = ea.getX() - inputData.MousePosX;
		inputData.MouseDeltaY = ea.getY() - inputData.MousePosY;
		inputData.MousePosX = ea.getX();
		inputData.MousePosY = ea.getY();
	}

	return false;
}