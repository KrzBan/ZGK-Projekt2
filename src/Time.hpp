#pragma once

#include "Core.hpp"

struct Time {
	static double GetDeltaTime();
	static double GetTime();

	static void UpdateTime(double time);
};

class TimeManager : public osgGA::GUIEventHandler {
public:
    TimeManager(osgViewer::Viewer* const viewer)
        : m_Viewer{ viewer } {}

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override {
        if (ea.getEventType() == osgGA::GUIEventAdapter::FRAME) {
            auto currentTime = m_Viewer->getFrameStamp()->getReferenceTime();
            Time::UpdateTime(currentTime);
        }
        return false;
    }
private:
    osgViewer::Viewer* const m_Viewer;
};