#include "Source.hpp"

#include "FpCameraManipulator.hpp"
#include "Utils.hpp"

class ModelController : public osgGA::GUIEventHandler {
public:
    ModelController(osg::MatrixTransform* node)
        : _model(node) {}

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

protected:
    Ref<osg::MatrixTransform> _model;
};

bool ModelController::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    if (!_model) return false;
    osg::Matrix matrix = _model->getMatrix();
    spdlog::info("Event: {}", ea.getEventType());
    switch (ea.getEventType()) {
    case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey()) {
        case 'a': case 'A':
            matrix *= osg::Matrix::rotate(-0.1f, osg::Z_AXIS);
            break;
        case 'd': case 'D':
            matrix *= osg::Matrix::rotate(0.1f, osg::Z_AXIS);
            break;
        case 'w': case 'W':
            matrix *= osg::Matrix::rotate(-0.1f, osg::X_AXIS);
            break;
        case 's': case 'S':
            matrix *= osg::Matrix::rotate(0.1f, osg::X_AXIS);
            break;
        default:
            break;
        }
        _model->setMatrix(matrix);
        break;
    default:
        break;
    }
    return false;
}

Ref<osg::Group> PrepareScene(osgViewer::Viewer* viewer)
{
    Ref<osg::Group> scn = new osg::Group();

    auto sphere = LoadModel("assets/sphere.obj");

    auto* t1 = new osg::MatrixTransform();
    t1->setMatrix(osg::Matrix::scale(1.0, 1.0, 1.0) *
        osg::Matrix::translate(0.0, 0.0, 0.0));
    t1->addChild(sphere);
    scn->addChild(t1);
    
    return scn;
}

void EntryPoint() {
    osgViewer::Viewer viewer;

    Ref<FpCameraManipulator> fpm = new FpCameraManipulator{};
    viewer.setCameraManipulator(fpm);

    auto camera = SetupCamera();
    viewer.setCamera(camera);

    auto scene = PrepareScene(&viewer);
    viewer.setSceneData(scene);

    // Instead of viewer.run();
    viewer.realize();

    osgViewer::Viewer::Windows windows;
    viewer.getWindows(windows);
    for (auto& window : windows) {
        window->useCursor(false);
        window->setCursor(osgViewer::GraphicsWindow::NoCursor);
    }

    while (!viewer.done()) {
        viewer.frame();
    }
}

int main() {
    spdlog::set_level(spdlog::level::trace);
    spdlog::trace("Program start");

    try {
        EntryPoint();
    }
    catch (std::exception& e) {
        spdlog::error(e.what());
    }
    
    spdlog::trace("Program exit");
    return EXIT_SUCCESS;
}
