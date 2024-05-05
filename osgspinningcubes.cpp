/**
 * @file osgspinningcube.cpp
 *
 * @brief Open Scene Graph sipnning cubes.
 *
 *
 * @author Jaime Cabero Creus
 * @author Marcos Mosquera Miranda
 * @author Marcos Rial Troncoso
 *
 */

#include <osg/Group>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>
#include <osg/Node>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>

// Función para cargar una textura desde un archivo de imagen
osg::ref_ptr<osg::Texture2D> LoadTexture(const std::string& filename)
{
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(filename);
    if (!image)
    {
        std::cerr << "Error cargando la imagen: " << filename << std::endl;
        return nullptr;
    }

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage(image);

    return texture;
}

// Función callback que aplica la rotación al cubo PAT
class CubeSpinningUpdateCallback : public osg::NodeCallback
{
public:
    CubeSpinningUpdateCallback(const osg::Vec3 &initialPosition, double startTime)
        : _initialPosition(initialPosition), _startTime(startTime) {}

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
    {
        if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
        {
            double currentTime = nv->getFrameStamp()->getReferenceTime() - _startTime;

            osg::PositionAttitudeTransform *transform = dynamic_cast<osg::PositionAttitudeTransform *>(node);
            if (transform)
            {
                // Calculamos el ángulo de rotación actual
                float angleY = 40.0f * currentTime;
                float angleX = 81.0f * currentTime;

                // Calculamos la rotación total
                osg::Quat rotationY = osg::Quat(osg::DegreesToRadians(angleY), osg::Vec3(0.0f, 1.0f, 0.0f));
                osg::Quat rotationX = osg::Quat(osg::DegreesToRadians(angleX), osg::Vec3(1.0f, 0.0f, 0.0f));
                osg::Quat totalRotation = rotationY * rotationX;

                // Calculamos las coordenadas de traslación basadas en funciones trigonométricas
                float x = sinf(2.1f * currentTime) * 0.5f;
                float y = cosf(1.7f * currentTime) * 0.5f;
                float z = sinf(1.3f * currentTime) * cosf(1.5f * currentTime) * 2.0f;

                // Aplicamos la traslación y rotación al PAT
                transform->setAttitude(totalRotation);
                transform->setPosition(_initialPosition + osg::Vec3(x, y, z));
            }
        }

        // Continuar con el recorrido del nodo
        traverse(node, nv);
    }

private:
    double _startTime;
    osg::Vec3 _initialPosition;
};

// Función para generar los cubos que rotan y que se añadiran al grupo de la escena
osg::ref_ptr<osg::PositionAttitudeTransform> CreateSpinningCubeNode(const osg::Vec3 &position, const osg::Vec4 &color, const std::string& textureFile, double startTime)
{
    // Creamos una geometría de cubo
    osg::ref_ptr<osg::Box> box = new osg::Box(osg::Vec3(0, 0, 0), 1.0f);

    // Creamos un drawable con la geometría del cubo
    osg::ref_ptr<osg::ShapeDrawable> shapeDrawable = new osg::ShapeDrawable(box);

    // Creamos un nodo geode y añadimos el drawable
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode->addDrawable(shapeDrawable);

    // Creamos el array de colores para darle color al cubo
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(color);

    // Establecemos los colores en el shape drawable y lo aplicamos sobre todo el cubo
    shapeDrawable->setColorArray(colors);
    shapeDrawable->setColorBinding(osg::Geometry::BIND_OVERALL);

    // Cargar la textura
    osg::ref_ptr<osg::Texture2D> texture = LoadTexture(textureFile);
    if (!texture)
    {
        std::cerr << "Error cargando la textura desde el archivo: " << textureFile << std::endl;
        return nullptr;
    }

    // Aplicar la textura al cubo
    osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();
    stateSet->setTextureAttributeAndModes(0, texture);

    // Creamos un PAT para aplicar las transformaciones
    osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform;

    // Definimos y aplicamos la traslación inicial
    transform->setPosition(position);

    // Añadimos el geode al PAT
    transform->addChild(geode);

    // Añadimos el callback al cubo
    transform->setUpdateCallback(new CubeSpinningUpdateCallback(position, startTime));

    return transform;
}

// Función para generar el punto de luz adicional
osg::ref_ptr<osg::PositionAttitudeTransform> CreateLightSource()
{
    // Creamos una esfera pequeña
    osg::ref_ptr<osg::Sphere> lightSphere = new osg::Sphere(osg::Vec3(0, 0, 0), 0.1f);
    osg::ref_ptr<osg::ShapeDrawable> lightShapeDrawable = new osg::ShapeDrawable(lightSphere);

    // Establecer el color amarillo a la esfera
    osg::ref_ptr<osg::Vec4Array> lightColors = new osg::Vec4Array;
    lightColors->push_back(osg::Vec4(1.0f, 0.9f, 0.0f, 1.0f));
    lightShapeDrawable->setColorArray(lightColors);
    lightShapeDrawable->setColorBinding(osg::Geometry::BIND_OVERALL);

    // Creamos un nodo geode y añadimos el drawable
    osg::ref_ptr<osg::Geode> lightGeode = new osg::Geode();
    lightGeode->addDrawable(lightShapeDrawable);

    // Creamos una fuente de luz y se agrega a la esfera
    osg::ref_ptr<osg::LightSource> lightSource(new osg::LightSource());
    lightSource->getLight()->setLightNum(1);
    lightSource->getLight()->setPosition(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    lightSource->getLight()->setDiffuse(osg::Vec4(1.0, 1.0, 0.0, 1.0));
    lightGeode->addChild(lightSource);

    // Creamos un PAT a la que se le añade el nodo geode
    osg::ref_ptr<osg::PositionAttitudeTransform> lightPAT(new osg::PositionAttitudeTransform());
    lightPAT->addChild(lightGeode);

    return lightPAT;
}

int main()
{

    // Creamos el grupo de la escena
    osg::ref_ptr<osg::Group> group = new osg::Group();

    // Creamos un visor y cogemos la referencia temporal
    osgViewer::Viewer viewer;
    double startTime = viewer.getFrameStamp()->getReferenceTime();

    // Creamos el primer cubo
    osg::ref_ptr<osg::PositionAttitudeTransform> transform = CreateSpinningCubeNode(
        osg::Vec3(-2.0f, 10.0f, 0.0f),     // Posicionado más a la izquierda
        osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f), // Color ~ rojo-magenta
        "stone_texture.png",               // Textura - Piedra
        startTime);

    // Creamos el segundo cubo
    osg::ref_ptr<osg::PositionAttitudeTransform> transform2 = CreateSpinningCubeNode(
        osg::Vec3(2.0f, 15.0f, 0.0f),      // Posicionado más a la derecha
        osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f), // Color ~ azul-marino
        "wood_texture.png",                // Textura - Madera
        startTime + 1);                    // Valor extra para simular transformaciones desacompasada respecto al primer cubo

    // Añadimos los cubo al grupo
    group->addChild(transform);
    group->addChild(transform2);

    // Creamos la nueva fuente de luz y la añadimos al grupo
    osg::ref_ptr<osg::PositionAttitudeTransform> lightPAT = CreateLightSource();
    lightPAT->setPosition(osg::Vec3(3.0, 12.0, 3.0));
    group->addChild(lightPAT);

    // Activamos la nueva fuente de luz
    osg::ref_ptr<osg::StateSet> ss = group->getOrCreateStateSet();
    ss->setMode(GL_LIGHT1, osg::StateAttribute::ON);

    // Añadimos el grupo a la escena
    viewer.setSceneData(group);

    return viewer.run();
}
