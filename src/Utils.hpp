#pragma once

#include "Core.hpp"

osg::ref_ptr<osg::Camera> SetupCamera();

osg::ref_ptr<osg::Node> LoadModel(const std::string& pathname);