#include "Source.hpp"

#include "FpCameraManipulator.hpp"
#include "Utils.hpp"

#include "Ray.hpp"
#include "Sphere.hpp"

class PlayerController : public osgGA::GUIEventHandler {
public:
    PlayerController(osgViewer::Viewer* const viewer, Ref<osg::Group> shapesRoot )
        : m_Viewer{ viewer }, 
          m_ShapesRoot{shapesRoot},
          m_Camera{ dynamic_cast<FpCameraManipulator*>(viewer->getCameraManipulator()) }, 
          lastTime{ m_Viewer->getFrameStamp()->getReferenceTime() } 
    {
        InitShapes();
    }

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

    void InitShapes();

private:
    FpCameraManipulator* const m_Camera{};
    osgViewer::Viewer* const m_Viewer;

    Ref<osg::Group> m_ShapesRoot{};
    std::vector<SphereNode> m_Shapes{};

    double moveForward = 0.0;
    double moveRight = 0.0;
    double moveBackwards = 0.0;
    double moveLeft = 0.0;

    double lastTime = 0.0;
};

void PlayerController::InitShapes() {
    static auto sphereModel = LoadModel("assets/sphere.obj");

    std::vector<Sphere> spheres = {
        Sphere{{1.0, 2.0, 1.0}, 1.0},
        Sphere{{-1.0, 1.7, 0.5}, 0.5},
        Sphere{{0.0, 1.5, 0.1}, 0.1}
    };

    for (const auto& sphere : spheres) {
        auto* trans = new osg::MatrixTransform();
        trans->setMatrix(osg::Matrix::scale(sphere.m_Radius, sphere.m_Radius, sphere.m_Radius) *
                         osg::Matrix::translate(sphere.m_Position));
        trans->addChild(sphereModel);
        m_ShapesRoot->addChild(trans);

        m_Shapes.push_back({ sphere, trans });
    }
}

bool PlayerController::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {

    auto currentTime = m_Viewer->getFrameStamp()->getReferenceTime();
    auto deltaTime = currentTime - lastTime;
    lastTime = currentTime;

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

    constexpr double moveSpeed = 1.0;

    osg::Vec2d moveVec = { moveRight - moveLeft , moveForward - moveBackwards };
    moveVec.normalize();
    m_Camera->MoveCamera(deltaTime * moveSpeed * moveVec.y(), deltaTime * moveSpeed * moveVec.x());

    return false;
}

Ref<osg::Group> PrepareScene(osgViewer::Viewer* viewer)
{
    Ref<osg::Group> scn = new osg::Group();

    Ref<osg::Group> shapesRoot = new osg::Group{};
    scn->addChild(shapesRoot);

    Ref<PlayerController> pc = new PlayerController{ viewer, shapesRoot };
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
