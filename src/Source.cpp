#include "Source.hpp"

#include "FpCameraManipulator.hpp"
#include "Utils.hpp"

#include "Ray.hpp"
#include "Sphere.hpp"
#include "TextManager.hpp"
#include "SkyBox.hpp"
#include "Time.hpp"
#include "Input.hpp"

class ShapeController : public osgGA::GUIEventHandler {
public:
    ShapeController(osgViewer::Viewer* const viewer, Ref<osg::MatrixTransform> transform, std::shared_ptr<Sphere> shape)
        : m_Viewer{ viewer }, m_Transform { transform }, m_Shape{shape} 
    {
        static auto gen = std::mt19937{};
        static auto dist = std::uniform_real_distribution<double>{ 0.5, 1.5 };

        fallSpeed = dist(gen);
        rotSpeed = dist(gen);
        rotLengthX = dist(gen);
        rotLengthY = dist(gen);
        rotOffset = dist(gen);
    }

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override {

        if (ea.getEventType() != osgGA::GUIEventAdapter::FRAME) return false;
        
        auto deltaTime = Time::GetDeltaTime();
        auto time = Time::GetTime();

        auto fallVec = osg::Vec3d{ 
            rotLengthX*std::sin(time * rotSpeed + rotOffset), 
            rotLengthY*std::cos(time * rotSpeed + rotOffset), 
            -fallSpeed} * deltaTime;
        m_Transform->setMatrix(
            osg::Matrixd::rotate(m_Transform->getMatrix().getRotate()) *
            osg::Matrixd::scale(m_Transform->getMatrix().getScale()) * 
            osg::Matrixd::translate(m_Transform->getMatrix().getTrans() + fallVec)
        );

        m_Shape->m_Position = m_Transform->getMatrix().getTrans();

        return false;
    }
private:
    double fallSpeed{};
    double rotSpeed{};
    double rotLengthX{};
    double rotLengthY{};
    double rotOffset{};

    osgViewer::Viewer* const m_Viewer;
    Ref<osg::MatrixTransform> m_Transform{};
    std::shared_ptr<Sphere> m_Shape{};
};

struct ShapeNode {
    std::shared_ptr<Sphere> sphere;
    Ref<osg::Node> node;
    Ref<ShapeController> shapeController;
    bool shouldRemove = false;
};

class DrawRayController : public osgGA::GUIEventHandler {
public:
    DrawRayController(Ref<osg::Geode> rayGeode, osgViewer::Viewer* viewer)
        : m_RayGeode{ rayGeode }, m_Viewer{ viewer } {}

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override {
        if (invalid) return false;
        if (ea.getEventType() != osgGA::GUIEventAdapter::FRAME) return false;

        auto deltaTime = Time::GetDeltaTime();

        auto drawable = dynamic_cast<osg::Geometry*>(m_RayGeode->getDrawable(0));
        auto colors = dynamic_cast<osg::Vec4Array*>(drawable->getColorArray());

        bool remove = false;

        osg::Vec4* data = (osg::Vec4*)colors->getDataPointer();
        for (int i = 0; i < colors->size(); ++i) {
            data[i][3] -= 0.5 * deltaTime;
            if (data[i][3] < 0) remove = true;
        }

        if (remove) {
            for(const auto& parent: m_RayGeode->getParents()) 
                parent->removeChild(m_RayGeode);
            invalid = true; 
            return false;
        }

        drawable->dirtyGLObjects(); // Refresh object colors
        return false;
    }
private:
    Ref<osg::Geode> m_RayGeode{};
    osgViewer::Viewer* m_Viewer;
    bool invalid = false;
};

class PlayerController : public osgGA::GUIEventHandler {
public:
    PlayerController(osgViewer::Viewer* const viewer, Ref<osg::Group> shapesRoot )
        : m_Viewer{ viewer }, 
          m_ShapesRoot{shapesRoot},
          m_Camera{ dynamic_cast<FpCameraManipulator*>(viewer->getCameraManipulator()) }
    {
        InitShapes();
    }

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

    void InitShapes();
    void ShootRay(const Ray& ray);
    void RayTest(const Ray& ray, uint32_t depth, std::vector<osg::Vec3d>& hitPoints);
    void DrawRay(const std::vector<osg::Vec3d>& hitPoints);
    void SpawnSphere();
    void AddShape(const std::shared_ptr<Sphere>& sphere);
private:
    FpCameraManipulator* const m_Camera{};
    osgViewer::Viewer* const m_Viewer;

    Ref<osg::Group> m_ShapesRoot{};
    std::vector<ShapeNode> m_Shapes{};

    double moveForward = 0.0;
    double moveRight = 0.0;
    double moveBackwards = 0.0;
    double moveLeft = 0.0;

    double lastBallSpawn = 0.0;
    uint32_t shotsFired = 0;
};

void PlayerController::AddShape(const std::shared_ptr<Sphere>& sphere) {
    static auto sphereModel = LoadModel("assets/sphere.obj");

    Ref<osg::Material> mat = new osg::Material{};
    mat->setDiffuse(osg::Material::FRONT_AND_BACK, RandomColor());
    sphereModel->getOrCreateStateSet()->setAttributeAndModes(mat, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

    Ref<osg::MatrixTransform> trans = new osg::MatrixTransform();
    trans->setMatrix(osg::Matrix::scale(sphere->m_Radius, sphere->m_Radius, sphere->m_Radius) *
        osg::Matrix::translate(sphere->m_Position));
    trans->addChild(sphereModel);
    m_ShapesRoot->addChild(trans);

    Ref<ShapeController> sc = new ShapeController{ m_Viewer, trans, sphere };
    m_Viewer->addEventHandler(sc);
    m_Shapes.push_back({ sphere, trans, sc });
}

void PlayerController::SpawnSphere() {
    static auto gen = std::mt19937{};

    static auto radiusDist = std::uniform_real_distribution<double>{0.3, 1.0};
    static auto posDist = std::uniform_real_distribution<double>{ -5.0, 5.0 };

    const auto radius = radiusDist(gen);
    const auto pos = osg::Vec3d{ posDist(gen), posDist(gen), 20.0 };

    auto sphere = std::make_shared<Sphere>(pos, radius);

    AddShape(sphere);
}

void PlayerController::ShootRay(const Ray& ray) {
  
    std::vector<osg::Vec3d> hitLocations{ ray.Origin() + m_Camera->m_RightVec * 0.0 - m_Camera->m_ForwardVec * 0.05 - m_Camera->m_UpVec * 0.05 };
    RayTest(ray, 0, hitLocations);

    DrawRay(hitLocations);
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

        auto verCol = id % 2 ? osg::Vec4(0.1, 0.9, 0.1, 1.0) : osg::Vec4(0.8, 0.1, 0.05, 1.0);
        c->push_back(verCol);
    }

    g->setVertexArray(v);
    g->addPrimitiveSet(e);
    g->setColorArray(c);
    g->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    g->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    n->addDrawable(g);
    
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(2.0f);
    n->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);

    m_ShapesRoot->addChild(n);

    Ref<DrawRayController> drc = new DrawRayController{ n, m_Viewer };
    m_Viewer->addEventHandler(drc);
}

void PlayerController::RayTest(const Ray& ray, uint32_t depth, std::vector<osg::Vec3d>& hitPoints) {
    if (depth == 30) {
        spdlog::trace("[RayTest] Depth limit reached.");
        return;
    }

    // Find Closest Hit
    HitInfo hitInfo{};
    ShapeNode* recentNode = nullptr;
    float bestHit = std::numeric_limits<float>::infinity();

    for (auto& sphereNode : m_Shapes) {
        HitInfo tempInfo{};
        if (sphereNode.sphere->Hit(ray, 0.001, bestHit, tempInfo)) {
            bestHit = tempInfo.t;
            hitInfo = tempInfo;
            recentNode = &sphereNode;
        }
    }

    if (recentNode != nullptr) {
        TextManager::AddPoints(10 * (3*depth + 1));

        spdlog::trace("[RayTest] Hit!");
        hitPoints.push_back(hitInfo.position);

        Ray scattered{ hitInfo.position, hitInfo.normal };
        RayTest(scattered, depth + 1, hitPoints);
        recentNode->shouldRemove = true;
        return;
    }
   
    spdlog::trace("[RayTest] Hit nothing.");
    hitPoints.push_back(ray.Origin() + ray.Direction() * 1000);
}

void PlayerController::InitShapes() {
 
    std::vector<std::shared_ptr<Sphere>> spheres = {
        std::make_shared<Sphere>(osg::Vec3d{1.0, 2.0, 5.0}, 1.0),
        std::make_shared<Sphere>(osg::Vec3d{-1.0, 1.7, 10.0}, 0.5),
        std::make_shared<Sphere>(osg::Vec3d{0.0, 1.5, 15.0}, 0.1)
    };

    for (const auto& sphere : spheres) {
        AddShape(sphere);
    }
}

bool PlayerController::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
  
    if (ea.getEventType() != osgGA::GUIEventAdapter::FRAME) return false;

    TextManager::SetTime(Time::GetTime());

    auto deltaTime = Time::GetDeltaTime();
    lastBallSpawn += deltaTime;

    constexpr double shapeSpawnTime = 0.5;
    if (lastBallSpawn > shapeSpawnTime) {
        SpawnSphere();
        lastBallSpawn = 0.0;
    }

    if (Input::GetKeyDown('f')) {
        constexpr double spread = 0.05;
        Ray rayLeft{ m_Camera->m_Position - m_Camera->m_RightVec * spread - m_Camera->m_UpVec * spread, m_Camera->m_ForwardVec};
        Ray rayMid{ m_Camera->m_Position, m_Camera->m_ForwardVec };
        Ray rayRight{ m_Camera->m_Position + m_Camera->m_RightVec * spread - m_Camera->m_UpVec * spread, m_Camera->m_ForwardVec };
        ShootRay(rayLeft);
        ShootRay(rayMid);
        ShootRay(rayRight);

        TextManager::UpdateEffic(++shotsFired);

        // Clean-up
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

    // Move Player
    constexpr double moveSpeed = 5.0;
    constexpr double rotSpeed = 1.5;

    double moveHor = (Input::GetKey('d') ? 1 : 0) - (Input::GetKey('a') ? 1 : 0);
    double moveVer = (Input::GetKey('w') ? 1 : 0) - (Input::GetKey('s') ? 1 : 0);

    osg::Vec2d moveVec = { moveHor , moveVer };
    moveVec.normalize();
    m_Camera->MoveCamera(deltaTime * moveSpeed * moveVec.y(), deltaTime * moveSpeed * moveVec.x());

    // Rotate camera
    double rotHor = (Input::GetKey(osgGA::GUIEventAdapter::KeySymbol::KEY_Right) ? 1 : 0) - (Input::GetKey(osgGA::GUIEventAdapter::KeySymbol::KEY_Left) ? 1 : 0);
    double rotVer = (Input::GetKey(osgGA::GUIEventAdapter::KeySymbol::KEY_Up) ? 1 : 0) - (Input::GetKey(osgGA::GUIEventAdapter::KeySymbol::KEY_Down) ? 1 : 0);
    osg::Vec2d rotVec = { rotHor , rotVer };
    rotVec.normalize();
    m_Camera->RotateView(deltaTime * rotSpeed * rotVec.x(), deltaTime * rotSpeed * rotVec.y());

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

    // HUD Camera
    osg::ref_ptr<osg::Geode> textGeode = new osg::Geode;
    auto textScore = createText( osg::Vec3(10.0f, 30.0f, 0.0f), "Score: 0", 20.0f);
    auto textTime = createText(osg::Vec3(10.0f, 50.0f, 0.0f), "Time: 0", 20.0f);
    auto textEffic = createText(osg::Vec3(10.0f, 10.0f, 0.0f), "Efficiency: 0", 20.0f);

    textGeode->addDrawable(textScore);
    textGeode->addDrawable(textTime);
    textGeode->addDrawable(textEffic);

    TextManager::SetTextNode(textScore);
    TextManager::SetTimeNode(textTime);
    TextManager::SetEfficNode(textEffic);

    osg::Camera* camera = createHUDCamera(0, 1024, 0, 768);
    camera->getOrCreateStateSet()->setMode(
        GL_LIGHTING, osg::StateAttribute::OFF);
    Ref<HUDCameraManager> hcm = new HUDCameraManager(camera);
    viewer->addEventHandler(hcm);

    camera->addChild(textGeode);

    osg::MatrixTransform* t = new osg::MatrixTransform();
    auto crossGeode = CreateCrosshair();
    t->addChild(crossGeode);
    camera->addChild(t);
    Ref<CrossManager> cm = new CrossManager(viewer, t);
    viewer->addEventHandler(cm);

    scn->addChild(camera);

    // Sky-Box
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(new osg::ShapeDrawable(
        new osg::Sphere(osg::Vec3(), 100)));

    osg::ref_ptr<SkyBox> skybox = new SkyBox;
    skybox->getOrCreateStateSet()->setTextureAttributeAndModes(
        0, new osg::TexGen);
    skybox->setEnvironmentMap(0,
        osgDB::readImageFile("assets/negx.jpg"),
        osgDB::readImageFile("assets/posx.jpg"),
        osgDB::readImageFile("assets/negz.jpg"),
        osgDB::readImageFile("assets/posz.jpg"),
        osgDB::readImageFile("assets/posy.jpg"),
        osgDB::readImageFile("assets/negy.jpg"));
    skybox->addChild(geode.get());
    skybox->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");
    scn->addChild(skybox);

    return scn;
}

void EntryPoint() {
    osgViewer::Viewer viewer;

    Ref<TimeManager> tm = new TimeManager(&viewer);
    viewer.addEventHandler(tm);

    Ref<InputManager> im = new InputManager(&viewer);
    viewer.addEventHandler(im);

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
        Input::ClearKeys();
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