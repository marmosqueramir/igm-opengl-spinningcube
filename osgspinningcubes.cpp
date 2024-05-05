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
#include <osg/ShapeDrawable>
#include <osgViewer/Viewer>

int main()
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
    colors->push_back(osg::Vec4(1.0f, 0.4f, 0.4f, 1.0f));

    // Establecemos los colores en el shape drawable y lo aplicamos sobre todo el cubo
    shapeDrawable->setColorArray(colors);
    shapeDrawable->setColorBinding(osg::Geometry::BIND_OVERALL);
    
    // Creamos un PAT para aplicar las transformaciones
    osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform;
    
    // Definimos y aplicamos la traslación
    osg::Vec3 initialTranslation(0.0f, 0.0f, -20.0f);
    transform->setPosition(initialTranslation);

    // Añadimos el geode al PAT
    transform->addChild(geode);

    // Creamos el grupo de la escena y añadimos el cubo
    osg::ref_ptr<osg::Group> group = new osg::Group();
    group->addChild(transform);

    // Creamos el visor y añadimos el grupo a la escena
    osgViewer::Viewer viewer;
    viewer.setSceneData(group);

    // Tiempo inicial para la rotación continua
    double startTime = viewer.getFrameStamp()->getReferenceTime();

    // Ejecutamos el visor aplicando las rotaciones sobre el cubo
    while (!viewer.done())
    {
        // Tiempo actual
        double currentTime = viewer.getFrameStamp()->getReferenceTime() - startTime;

        // Calculamos el ángulo de rotación actual
        float angleY = 40.0f * currentTime;
        float angleX = 81.0f * currentTime;

        // Aplicamos la rotación al PositionAttitudeTransform
        osg::Quat rotationY = osg::Quat(osg::DegreesToRadians(angleY), osg::Vec3(0.0f, 1.0f, 0.0f));
        osg::Quat rotationX = osg::Quat(osg::DegreesToRadians(angleX), osg::Vec3(1.0f, 0.0f, 0.0f));
        osg::Quat totalRotation = rotationY * rotationX;

        // Calculamos las coordenadas de traslación basadas en funciones trigonométricas
        float x = sinf(2.1f * currentTime) * 0.5f;
        float y = cosf(1.7f * currentTime) * 0.5f;
        float z = sinf(1.3f * currentTime) * cosf(1.5f * currentTime) * 2.0f;
        
        // Establecemos la rotación continua
        transform->setAttitude(totalRotation);
        transform->setPosition(initialTranslation + osg::Vec3(x,y,z));

        // Renderizamos el fotograma
        viewer.frame();
    }

    return 0;

}
