#include "Source.hpp"

class ModelController : public osgGA::GUIEventHandler
{
public:
    ModelController(osg::MatrixTransform* node)
        : _model(node) {}

    virtual bool handle(const osgGA::GUIEventAdapter& ea,
        osgGA::GUIActionAdapter& aa) override;
protected:
    osg::ref_ptr<osg::MatrixTransform> _model;
};

bool ModelController::handle(const osgGA::GUIEventAdapter& ea,
    osgGA::GUIActionAdapter& aa)
{
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

osg::Node* createTable()
{
    using osg::Vec2;
    using osg::Vec3;
    using osg::Vec4;

    // tablica ze współrzędnymi wierzchołków bryły
    auto v = new osg::Vec3Array();
    v->push_back(Vec3(0.0, 1.0, 1.0));
    v->push_back(Vec3(0.0, 0.0, 1.0));
    v->push_back(Vec3(1.0, 0.0, 1.0));
    v->push_back(Vec3(1.0, 1.0, 1.0));
    v->push_back(Vec3(0.0, 1.0, 0.0));
    v->push_back(Vec3(0.0, 0.0, 0.0));
    v->push_back(Vec3(1.0, 0.0, 0.0));
    v->push_back(Vec3(1.0, 1.0, 0.0));

    // tablica z kolorami wierzchołków -- definiujemy tylko jeden, później
    // zaznaczymy przez BIND_OVERALL że wszystkie wierzchołki mają przypisany
    // jeden wspólny kolor
    auto c = new osg::Vec4Array();
    c->push_back(Vec4(1.0, 1.0, 1.0, 1.0));

    // tablica z definicjami rysowanych prymitywów
    auto e = new osg::DrawElementsUByte(osg::PrimitiveSet::QUADS);
    // pierwszy czworokąt:
    e->push_back(3);
    e->push_back(2);
    e->push_back(1);
    e->push_back(0);
    // drugi:
    e->push_back(7);
    e->push_back(6);
    e->push_back(5);
    e->push_back(4);
    // i następne:
    e->push_back(3);
    e->push_back(2);
    e->push_back(6);
    e->push_back(7);
    //
    e->push_back(4);
    e->push_back(0);
    e->push_back(3);
    e->push_back(7);
    //
    e->push_back(4);
    e->push_back(5);
    e->push_back(1);
    e->push_back(0);
    //
    e->push_back(1);
    e->push_back(5);
    e->push_back(6);
    e->push_back(2);

    osg::ref_ptr<osg::Vec2Array> texcoord = new osg::Vec2Array;
    texcoord->push_back(Vec2{ 1.0, 0.0 });
    texcoord->push_back(Vec2{ 0.0, 0.0 });
    texcoord->push_back(Vec2{ 0.0, 1.0 });
    texcoord->push_back(Vec2{ 1.0, 1.0 });

    texcoord->push_back(Vec2{ 1.0, 0.0 });
    texcoord->push_back(Vec2{ 0.0, 0.0 });
    texcoord->push_back(Vec2{ 0.0, 1.0 });
    texcoord->push_back(Vec2{ 1.0, 1.0 });

    osg::ref_ptr<osg::Image> image = osgDB::readImageFile("assets/wood.jpg");
    if (image == nullptr) {
        throw std::runtime_error("Couldn't read texture file!");
    }
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image.get());
    texture->setUnRefImageDataAfterApply(true);

    auto g = new osg::Geometry();
    g->setVertexArray(v);
    g->addPrimitiveSet(e);
    g->setTexCoordArray(0, texcoord.get());
    g->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());

    g->setColorArray(c);
    g->setColorBinding(osg::Geometry::BIND_OVERALL);
    g->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
    g->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);

    auto n = new osg::Geode();
    n->addDrawable(g);

    // białe krawędzie, rozpięte na punktach troszeczkę na zewnątrz sześcianu
    g = new osg::Geometry();
    v = new osg::Vec3Array();
    v->push_back(Vec3(0.0, 1.001, 1.001));
    v->push_back(Vec3(0.0, 0.0, 1.001));
    v->push_back(Vec3(1.001, 0.0, 1.001));
    v->push_back(Vec3(1.001, 1.001, 1.001));
    v->push_back(Vec3(0.0, 1.001, 0.0));
    v->push_back(Vec3(0.0, 0.0, 0.0));
    v->push_back(Vec3(1.001, 0.0, 0.0));
    v->push_back(Vec3(1.001, 1.001, 0.0));
    g->setVertexArray(v);
    c = new osg::Vec4Array();
    c->push_back(Vec4(1.0, 1.0, 1.0, 1.0));
    g->setColorArray(c);
    g->setColorBinding(osg::Geometry::BIND_OVERALL);
    e = new osg::DrawElementsUByte(osg::PrimitiveSet::LINE_STRIP);
    e->push_back(0);
    e->push_back(1);
    e->push_back(2);
    e->push_back(3);
    e->push_back(0);
    e->push_back(4);
    e->push_back(5);
    e->push_back(6);
    e->push_back(7);
    e->push_back(4);
    g->addPrimitiveSet(e);
    e = new osg::DrawElementsUByte(osg::PrimitiveSet::LINES);
    e->push_back(1);
    e->push_back(5);
    e->push_back(2);
    e->push_back(6);
    e->push_back(3);
    e->push_back(7);
    g->addPrimitiveSet(e);
    g->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    n->addDrawable(g);

    return n;
}

osg::Node* createLightSource(
    unsigned int num,
    const osg::Vec3& trans,
    const osg::Vec4& color)
{
    osg::ref_ptr<osg::Light> light = new osg::Light{};
    light->setLightNum(num);
    light->setDiffuse(color);
    light->setPosition(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    light->setDirection(osg::Vec3(0.0, 0.0, -1.0));
    light->setSpotCutoff(45.0f);

    osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource{};
    lightSource->setLight(light);

    osg::ref_ptr<osg::MatrixTransform> sourceTrans = new osg::MatrixTransform{};
    sourceTrans->setMatrix(osg::Matrix::translate(trans));
    sourceTrans->addChild(lightSource.get());
    return sourceTrans.release();
}

osg::Node* stworz_scene(osgViewer::Viewer* viewer)
{
    auto scn = new osg::Group();

    {
        auto kostka = createTable();
        osg::MatrixTransform* t = new osg::MatrixTransform();
        t->setMatrix(osg::Matrix::scale(5, 2.0, 0.2) *
            osg::Matrix::translate(-2.0, 0.0, -1.0));
        t->addChild(kostka);
        scn->addChild(t);
    }

    {
        osg::Node* zuzia = osgDB::readNodeFile("assets/suzanne.obj");
        if (zuzia == nullptr) {
            std::fprintf(stderr, "Blad: nie udaoo sie wczytac modelu 3D.\n");
            std::exit(EXIT_FAILURE);
        }
        zuzia->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

        auto* t1 = new osg::MatrixTransform();
        t1->setMatrix(osg::Matrix::scale(1.0, 1.0, 1.0) *
            osg::Matrix::translate(0.0, 0.0, 0.0));
        t1->addChild(zuzia);
        scn->addChild(t1);

        auto* t2 = new osg::MatrixTransform();
        t2->setMatrix(osg::Matrix::scale(0.5, 0.5, 0.5) *
            osg::Matrix::translate(0.0, 0.0, 1.25));
        t2->addChild(zuzia);
        scn->addChild(t2);

        auto* t3 = new osg::MatrixTransform();
        t3->setMatrix(osg::Matrix::scale(0.25, 0.25, 0.25) *
            osg::Matrix::translate(0.0, 0.0, 1.75));
        t3->addChild(zuzia);
        scn->addChild(t3);

        osg::Node* light0 = createLightSource(
            0, osg::Vec3(0.0f, 0.0f, 3.0f), osg::Vec4(
                1.0f, 0.0f, 0.0f, 1.0f));
        scn->getOrCreateStateSet()->setMode(GL_LIGHT0,
            osg::StateAttribute::ON);
        scn->addChild(light0);

        osg::Node* light1 = createLightSource(
            1, osg::Vec3(-1.0f, 0.0f, 3.0f), osg::Vec4(
                0.0f, 1.0f, 0.0f, 1.0f));
        scn->getOrCreateStateSet()->setMode(GL_LIGHT1,
            osg::StateAttribute::ON);
        scn->addChild(light1);

        osg::Node* light2 = createLightSource(
            2, osg::Vec3(1.0f, 0.0f, 3.0f), osg::Vec4(
                0.0f, 0.0f, 1.0f, 1.0f));
        scn->getOrCreateStateSet()->setMode(GL_LIGHT2,
            osg::StateAttribute::ON);
        scn->addChild(light2);
    }

    return scn;
}

int main()
{
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = 50;
    traits->y = 50;
    traits->width = 800;
    traits->height = 600;
    traits->windowName = "OSG application";
    traits->windowDecoration = true;
    traits->doubleBuffer = true;
    traits->samples = 4;
    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setGraphicsContext(gc);
    camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    camera->setClearColor(osg::Vec4(0.2f, 0.2f, 0.4f, 1.0f));
    double aspect = static_cast<double>(traits->width) / static_cast<double>(traits->height);
    camera->setProjectionMatrixAsPerspective(30.0, aspect, 1.0, 1000.0);
    camera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    
    osg::ref_ptr<osgGA::FirstPersonManipulator> fpm = new osgGA::FirstPersonManipulator{};

    osgViewer::Viewer viewer;
    viewer.setCamera(camera.get());
    viewer.setCameraManipulator(nullptr);
    auto scene = stworz_scene(&viewer);
    viewer.setSceneData(scene);

    viewer.realize();
    while (!viewer.done()) {
        viewer.frame();
    }

    return EXIT_SUCCESS;
}
