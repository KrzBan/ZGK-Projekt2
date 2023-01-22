#pragma once

#include "Core.hpp"

osg::ref_ptr<osg::Camera> SetupCamera();

osg::ref_ptr<osg::Node> LoadModel(const std::string& pathname);

osg::Vec4d RandomColor();

inline osg::ref_ptr<osgText::Font> g_font = osgText::readFontFile("fonts/arial.ttf");

inline osgText::Text* createText(const osg::Vec3& pos,
	const std::string& content,
	float size)
{
	osg::ref_ptr<osgText::Text> text = new osgText::Text;
	text->setFont(g_font.get());
	text->setCharacterSize(size);
	text->setAxisAlignment(osgText::TextBase::XY_PLANE);
	text->setPosition(pos);
	text->setText(content);
	return text.release();
}

inline osg::Camera* createHUDCamera(double left, double right,
	double bottom, double top)
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	camera->setAllowEventFocus(false);
	camera->setProjectionMatrix(
		osg::Matrix::ortho2D(left, right, bottom, top));
	return camera.release();
}