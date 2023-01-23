#include "Utils.hpp"

osg::Vec4d RandomColor() {
    static auto gen = std::mt19937{};
    static auto dist = std::uniform_real_distribution<double>{ 0.0, 1.0 };

    const auto color = osg::Vec4d{ dist(gen), dist(gen), dist(gen), 1.0 };
    return color;
}

osg::ref_ptr<osg::Camera> SetupCamera() {
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = 50;
    traits->y = 50;
    traits->width = 1280;
    traits->height = 720;
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

osgText::Text* createText(const osg::Vec3& pos,
    const std::string& content,
    float size) {
    osg::ref_ptr<osgText::Text> text = new osgText::Text;
    //text->setFont(g_font.get());
    text->setCharacterSize(size);
    text->setAxisAlignment(osgText::TextBase::XY_PLANE);
    text->setPosition(pos);
    text->setText(content);
    return text.release();
}

osg::Camera* createHUDCamera(double left, double right,
    double bottom, double top) {
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    camera->setRenderOrder(osg::Camera::POST_RENDER);
    camera->setAllowEventFocus(false);
    camera->setProjectionMatrix(
        osg::Matrix::ortho2D(left, right, bottom, top));
    return camera.release();
}

osg::Geode* CreateCrosshair() {
    auto v = new osg::Vec3Array();
    v->push_back(osg::Vec3(0.0, 1.0, 0.0));
    v->push_back(osg::Vec3(0.0, 0.0, 0.0));
    v->push_back(osg::Vec3(1.0, 0.0, 0.0));
    v->push_back(osg::Vec3(1.0, 1.0, 0.0));

    auto c = new osg::Vec4Array();
    c->push_back(osg::Vec4(1.0, 1.0, 1.0, 1.0));

    auto e = new osg::DrawElementsUByte(osg::PrimitiveSet::QUADS);
    e->push_back(3);
    e->push_back(2);
    e->push_back(1);
    e->push_back(0);

    osg::ref_ptr<osg::Vec2Array> texcoord = new osg::Vec2Array;
    texcoord->push_back(osg::Vec2{ 1.0, 0.0 });
    texcoord->push_back(osg::Vec2{ 0.0, 0.0 });
    texcoord->push_back(osg::Vec2{ 0.0, 1.0 });
    texcoord->push_back(osg::Vec2{ 1.0, 1.0 });

    osg::ref_ptr<osg::Image> image = osgDB::readImageFile("assets/crosshair.png");
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
    g->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    auto n = new osg::Geode();
    n->addDrawable(g);

    return n;
}