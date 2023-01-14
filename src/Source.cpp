#include "Source.hpp"

#include "FpCameraManipulator.hpp"
#include "Utils.hpp"

#include "Ray.hpp"
#include "Sphere.hpp"

class ShapeController : public osgGA::GUIEventHandler {
public:
    ShapeController(osgViewer::Viewer* const viewer, Ref<osg::MatrixTransform> transform, std::shared_ptr<Sphere> shape)
        : m_Viewer{ viewer }, m_Transform { transform }, m_Shape{shape}, lastTime{ viewer->getFrameStamp()->getReferenceTime() } {}

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override {

        if (ea.getEventType() != osgGA::GUIEventAdapter::FRAME) return false;
        
        auto currentTime = m_Viewer->getFrameStamp()->getReferenceTime();
        auto deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        constexpr double fallSpeed = 1.0;

        auto fallVec = osg::Vec3d{ 0.0, 0.0, -fallSpeed * deltaTime };
        m_Transform->setMatrix(
            osg::Matrixd::rotate(m_Transform->getMatrix().getRotate()) *
            osg::Matrixd::scale(m_Transform->getMatrix().getScale()) * 
            osg::Matrixd::translate(m_Transform->getMatrix().getTrans() + fallVec)
        );

        m_Shape->m_Position = m_Transform->getMatrix().getTrans();

        spdlog::info("{}", m_Transform->getMatrix().getTrans().z());
        return false;
    }
private:
    osgViewer::Viewer* const m_Viewer;
    Ref<osg::MatrixTransform> m_Transform{};
    std::shared_ptr<Sphere> m_Shape{};
    double lastTime = 0.0;
};

struct ShapeNode {
    std::shared_ptr<Sphere> sphere;
    Ref<osg::Node> node;
    Ref<ShapeController> shapeController;
    bool shouldRemove = false;
};

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
    void ShootRay();
    void RayTest(const Ray& ray, uint32_t depth, std::vector<osg::Vec3d>& hitPoints);
    void DrawRay(const std::vector<osg::Vec3d>& hitPoints);
private:
    FpCameraManipulator* const m_Camera{};
    osgViewer::Viewer* const m_Viewer;

    Ref<osg::Group> m_ShapesRoot{};
    std::vector<ShapeNode> m_Shapes{};

    double moveForward = 0.0;
    double moveRight = 0.0;
    double moveBackwards = 0.0;
    double moveLeft = 0.0;

    double lastTime = 0.0;
};

class DrawRayController : public osgGA::GUIEventHandler {
public:
    DrawRayController(Ref<osg::Geode> rayGeode, Ref<osg::Group> shapesRoot)
        : m_RayGeode{ rayGeode }, m_ShapesRoot { shapesRoot } {}

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override {
        

        return false;
    }
private:
    Ref<osg::Geode> m_RayGeode{};
    Ref<osg::Group> m_ShapesRoot{};
};

void PlayerController::ShootRay() {
    Ray ray{ m_Camera->m_Position, m_Camera->m_ForwardVec };

    std::vector<osg::Vec3d> hitLocations{ m_Camera->m_Position + osg::Vec3d{0.0, 0.05, -0.05} };
    RayTest(ray, 10, hitLocations);

    DrawRay(hitLocations);

    std::vector<ShapeNode> newList{};
    for (auto& sphereNode : m_Shapes) {
        if (sphereNode.shouldRemove == true) {
            m_ShapesRoot->removeChild(sphereNode.node);
            m_Viewer->removeEventHandler(sphereNode.shapeController);
        }
        else {
            newList.push_back(std::move(sphereNode));
        }
    }

    m_Shapes = std::move(newList);
}

void PlayerController::DrawRay(const std::vector<osg::Vec3d>& hitPoints) {
    Ref<osg::Geode> n = new osg::Geode();
    auto g = new osg::Geometry();

    auto v = new osg::Vec3Array();
    auto e = new osg::DrawElementsUByte(osg::PrimitiveSet::LINE_STRIP);
    auto c = new osg::Vec4Array();

    uint32_t id = 0;
    for (const auto& point : hitPoints) {
        v->push_back(point);
        e->push_back(id++);

        auto verCol = id % 2 ? osg::Vec4(1.0, 0.1, 0.1, 0.5) : osg::Vec4(0.7, 0.8, 0.1, 0.5);
        c->push_back(verCol);
    }

    g->setVertexArray(v);
    g->addPrimitiveSet(e);
    g->setColorArray(c);
    g->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    g->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    n->addDrawable(g);
    
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(5.0f);
    n->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);

    m_ShapesRoot->addChild(n);
}

void PlayerController::RayTest(const Ray& ray, uint32_t depth, std::vector<osg::Vec3d>& hitPoints) {
    if (depth == 0) {
        spdlog::trace("[RayTest] Depth limit reached.");
        return;
    }

    // Find Closest Hit
    HitInfo hitInfo{};
    bool hitAnything = false;
    ShapeNode* recentNode = nullptr;
    float bestHit = std::numeric_limits<float>::infinity();

    for (auto& sphereNode : m_Shapes) {
        HitInfo tempInfo{};
        if (sphereNode.sphere->Hit(ray, 0.001, bestHit, tempInfo)) {
            hitAnything = true;
            bestHit = tempInfo.t;
            hitInfo = tempInfo;
            recentNode = &sphereNode;
        }
    }

    if (hitAnything) {
        spdlog::trace("[RayTest] Hit!");
        hitPoints.push_back(hitInfo.position);
        Ray scattered{ hitInfo.position, hitInfo.normal };
        RayTest(scattered, depth - 1, hitPoints);
        recentNode->shouldRemove = true;
        return;
    }
   
    spdlog::trace("[RayTest] Hit nothing.");
    hitPoints.push_back(ray.Origin() + ray.Direction() * 1000);
}

void PlayerController::InitShapes() {
    static auto sphereModel = LoadModel("assets/sphere.obj");

    std::vector<std::shared_ptr<Sphere>> spheres = {
        std::make_shared<Sphere>(osg::Vec3d{1.0, 2.0, 0.0}, 1.0),
        std::make_shared<Sphere>(osg::Vec3d{-1.0, 1.7, 0.0}, 0.5),
        std::make_shared<Sphere>(osg::Vec3d{0.0, 1.5, 0.0}, 0.1)
    };

    for (const auto& sphere : spheres) {
        Ref<osg::MatrixTransform> trans = new osg::MatrixTransform();
        trans->setMatrix(osg::Matrix::scale(sphere->m_Radius, sphere->m_Radius, sphere->m_Radius) *
                         osg::Matrix::translate(sphere->m_Position));
        trans->addChild(sphereModel);
        m_ShapesRoot->addChild(trans);

        Ref<ShapeController> sc = new ShapeController{ m_Viewer, trans, sphere };
        m_Viewer->addEventHandler(sc);
        m_Shapes.push_back({ sphere, trans, sc });
    }
}

bool PlayerController::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {

    auto currentTime = m_Viewer->getFrameStamp()->getReferenceTime();
    auto deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // spdlog::info("Reference time: {}, FPS: {}", deltaTime, 1.0 / deltaTime);

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
        case 'f': case 'F':
            ShootRay();
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

    osg::BlendFunc *blend = new osg::BlendFunc;
    blend->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
    scn->getOrCreateStateSet()->setAttributeAndModes(blend, osg::StateAttribute::ON);

    Ref<osg::Group> shapesRoot = new osg::Group{};
    scn->addChild(shapesRoot);

    Ref<PlayerController> pc = new PlayerController{ viewer, shapesRoot };
    viewer->addEventHandler(pc);

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
