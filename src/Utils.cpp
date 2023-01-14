#include "Utils.hpp"

osg::ref_ptr<osg::Camera> SetupCamera() {
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = 50;
    traits->y = 50;
    traits->width = 800;
    traits->height = 600;
    traits->windowName = "OSG application";
    traits->windowDecoration = true;
    traits->doubleBuffer = true;
    traits->samples = 4;
    double aspect = static_cast<double>(traits->width) / static_cast<double>(traits->height);

    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setGraphicsContext(gc);
    camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    camera->setClearColor(osg::Vec4(0.2f, 0.2f, 0.4f, 1.0f));
    camera->setProjectionMatrixAsPerspective(30.0, aspect, 0.01, 1000.0);
    camera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    return camera;
}

osg::ref_ptr<osg::Node> LoadModel(const std::string& pathanme) {
    osg::ref_ptr<osg::Node> sphere = osgDB::readNodeFile(pathanme);
    if (sphere == nullptr) {
        auto error = fmt::format("Cannot load {}", pathanme);
        throw std::runtime_error(error);
    }
    sphere->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    return sphere;
}