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
#include <osg/LightSource>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/GUIEventHandler>
#include <osgGA/FirstPersonManipulator>

// 3D Model Plugins
USE_OSGPLUGIN(obj)
// 2D Texture Plugins
USE_OSGPLUGIN(jpeg)
// OSG Settings
USE_GRAPHICSWINDOW()