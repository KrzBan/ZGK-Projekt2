#pragma once

#include "Core.hpp"

osg::ref_ptr<osg::Camera> SetupCamera();

osg::ref_ptr<osg::Node> LoadModel(const std::string& pathname);

osg::Vec4d RandomColor();

osgText::Text* createText(const osg::Vec3& pos,
	const std::string& content,
	float size);

osg::Camera* createHUDCamera(double left, double right,
	double bottom, double top);

osg::Geode* CreateCrosshair();

class HUDCameraManager : public osgGA::GUIEventHandler {
public:
	HUDCameraManager(osg::Camera* camera)
		: m_Camera{ camera } {}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
		if (ea.getEventType() != osgGA::GUIEventAdapter::RESIZE) return false;

		const auto width = ea.getWindowWidth();
		const auto height = ea.getWindowHeight();
		m_Camera->setProjectionMatrix(
			osg::Matrix::ortho2D(0, width, 0, height));

		return false;
	}
private:
	osg::Camera* m_Camera{};
};

class CrossManager : public osgGA::GUIEventHandler {
public:
	CrossManager(osgViewer::Viewer* const viewer, Ref<osg::MatrixTransform> transform)
		: m_Viewer{ viewer }, m_Transform{ transform } {}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
		if (ea.getEventType() != osgGA::GUIEventAdapter::RESIZE) return false;

		constexpr double size = 10.0;
		m_Transform->setMatrix(osg::Matrix::scale(size, size, size) *
			osg::Matrix::translate(ea.getWindowWidth()/2 - size/2, ea.getWindowHeight()/2 - size/2, 0.0));
		spdlog::info("Width: {}. Height: {}", ea.getWindowWidth(), ea.getWindowHeight());

		return false;
	}
private:
	osgViewer::Viewer* const m_Viewer;
	Ref<osg::MatrixTransform> m_Transform;
};