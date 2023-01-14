#pragma once

#include <iostream>
#include <spdlog/spdlog.h>

// OpenSceneGraph
#include <osgViewer/Viewer>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/AnimationPath>
#include <osg/Vec3>
#include <osg/LightSource>
#include <osg/LineWidth>
#include <osg/BlendFunc>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgGA/GUIEventHandler>
#include <osgGA/StandardManipulator>

// 3D Model Plugins
USE_OSGPLUGIN(obj)
// 2D Texture Plugins
USE_OSGPLUGIN(jpeg)
// OSG Settings
USE_GRAPHICSWINDOW()

template<class T>
using Ref = osg::ref_ptr<T>;