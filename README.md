### Notas para la ejecución

Necesario la instalación de Open Scene Graph:
- Descarga de código fuente en GitHub
- Build y enlazado de librerías para ejecutar correctamente los comandos de compilacion
- Para la última parte es necesario tener instalada la librería `libpng-dev` para cargar los recursos `.png`

Las texturas necesarias para la última parte:
- *wood_texture.png*
- *stone_texture.png*

Compilación mediante el comando:

```
g++ -o osgspinningcubes osgspinningcubes.cpp -lGL -lGLU -losg -losgViewer -losgDB -losgGA
```

### Notas sobre la prácita

- El fichero con el código es `osgspinningcubes.cpp`
- Los tags de las diferentes partes son: `osg-pt1`, `osg-pt2`, `osg-pt3` y `osg-pt4`
- Se pueden ver en la carpeta `/osg_resources` tanto los grafos, como las evidencias de la escena de cada apartado en formato video corto
- Se emplearon varias formas que incluye *osg* como `osg::Box` y `osg::Sphere`
- Se podría haber reutilizado la textura para los cubos, pero se decidió que cada uno tuviese una propia