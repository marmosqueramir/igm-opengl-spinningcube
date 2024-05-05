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
#include <osgViewer/Viewer>

// Función callback que aplica la rotación al cubo PAT
class CubeSpinningUpdateCallback : public osg::NodeCallback
{
public:
    CubeSpinningUpdateCallback(const osg::Vec3& initialPosition, double startTime) 
        : _initialPosition(initialPosition), _startTime(startTime) {}

    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
        {
            double currentTime = nv->getFrameStamp()->getReferenceTime() - _startTime;

            osg::PositionAttitudeTransform* transform = dynamic_cast<osg::PositionAttitudeTransform*>(node);
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
osg::ref_ptr<osg::PositionAttitudeTransform> CreateSpinningCubeNode(const osg::Vec3& position, const osg::Vec4& color, double startTime)
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

int main()
{

    // Creamos el grupo de la escena
    osg::ref_ptr<osg::Group> group = new osg::Group();

    // Creamos un visor y cogemos la referencia temporal
    osgViewer::Viewer viewer;
    double startTime = viewer.getFrameStamp()->getReferenceTime();

    // Creamos el primer cubo
    osg::ref_ptr<osg::PositionAttitudeTransform> transform = CreateSpinningCubeNode(
        osg::Vec3 (-2.0f, 10.0f, 0.0f), // Posicionado más a la izquierda
        osg::Vec4 (1.0f, 0.2f, 0.4f, 1.0f), // Color ~ rojo-magenta
        startTime);

    // Creamos el segundo cubo
    osg::ref_ptr<osg::PositionAttitudeTransform> transform2 = CreateSpinningCubeNode(
        osg::Vec3 (2.0f, 15.0f, 0.0f), // Posicionado más a la derecha
        osg::Vec4 (0.4f, 1.0f, 1.0f, 1.0f), // Color ~ azul-marino 
        startTime + 1); // Valor extra para simular transformaciones desacompasada respecto al primer cubo
    
    // Añadimos los cubo al grupo
    group->addChild(transform);
    group->addChild(transform2);

    // Añadimos el grupo a la escena
    viewer.setSceneData(group);

    return viewer.run();

}
