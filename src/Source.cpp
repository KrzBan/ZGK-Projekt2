#include "Source.hpp"

#include "FpCameraManipulator.hpp"
#include "Utils.hpp"

class PlayerController : public osgGA::GUIEventHandler {
public:
    PlayerController(osgViewer::Viewer* const viewer) 
        : m_Viewer{ viewer }, m_Camera{ dynamic_cast<FpCameraManipulator*>(viewer->getCameraManipulator()) }, lastTime{ m_Viewer->getFrameStamp()->getReferenceTime() } {}

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

private:
    FpCameraManipulator* const m_Camera{};
    osgViewer::Viewer* const m_Viewer;

    double moveForward = 0.0;
    double moveRight = 0.0;
    double moveBackwards = 0.0;
    double moveLeft = 0.0;

    double lastTime = 0.0;
};

bool PlayerController::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {

    constexpr double moveSpeed = 0.1;
    auto currentTime = m_Viewer->getFrameStamp()->getReferenceTime();
    auto deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    if (deltaTime == 0) return false;

    spdlog::info("Reference time: {}, FPS: {}", deltaTime, 1.0 / deltaTime);

    switch (ea.getEventType()) {
    case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey()) {
        case 'a': case 'A':
            moveLeft = 1.0;
            break;
        case 'd': case 'D':
            moveRight = 1.0;
            break;
        case 'w': case 'W':
            moveForward = 1.0;
            break;
        case 's': case 'S':
            moveBackwards = 1.0;
            break;
        default:
            break;
        }
        break;
    case osgGA::GUIEventAdapter::KEYUP:
        switch (ea.getKey()) {
        case 'a': case 'A':
            moveLeft = 0.0;
            break;
        case 'd': case 'D':
            moveRight = 0.0;
            break;
        case 'w': case 'W':
            moveForward = 0.0;
            break;
        case 's': case 'S':
            moveBackwards = 0.0;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    osg::Vec2d moveVec = { moveRight - moveLeft , moveForward - moveBackwards };
    moveVec.normalize();
    m_Camera->MoveCamera(moveSpeed * moveVec.y(), moveSpeed * moveVec.x());

    return false;
}

Ref<osg::Group> PrepareScene(osgViewer::Viewer* viewer)
{
    Ref<osg::Group> scn = new osg::Group();

    Ref<PlayerController> pc = new PlayerController{ viewer };
    viewer->addEventHandler(pc);

    auto sphere = LoadModel("assets/sphere.obj");

    auto* trans = new osg::MatrixTransform();
    trans->setMatrix(osg::Matrix::scale(1.0, 1.0, 1.0) *
        osg::Matrix::translate(0.0, 0.0, 0.0));
    trans->addChild(sphere);
    scn->addChild(trans);
    
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
