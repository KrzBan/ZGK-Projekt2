#pragma once

#include "Core.hpp"

struct Input {
	static bool GetKey(int keyCode);
	static bool GetKeyDown(int keyCode);
	static bool GetKeyUp(int keyCode);

	static double GetMousePosX();
	static double GetMousePosY();

	static double GetMouseDeltaX();
	static double GetMouseDeltaY();

	static void ClearKeys();
};

class InputManager : public osgGA::GUIEventHandler {
public:
	InputManager(osgViewer::Viewer* const viewer)
		: m_Viewer{ viewer } {}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
private:
	osgViewer::Viewer* const m_Viewer;
};