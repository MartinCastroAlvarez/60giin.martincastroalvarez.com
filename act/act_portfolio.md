# Actividad Portfolio

Peso - 40% del total de la asignatura

- Primera Convocatoria 01/06/2026 - 08:00
- Segunda Convocatoria 06/07/2026 - 08:00

Para cada apartado de esta actividad desarrollar un proyecto nuevo a partir de la estructura del motor desarrollado en clase. En dicho proyecto, habrá un fichero `main.cpp` con el código necesario para resolver el problema, junto a ese fichero estarán aquellos *shaders* que sean necesarios para la resolución. El alumno es libre de realizar todas las modificaciones/ampliaciones que considere necesarias en el motor desarrollado en clase. Si se necesitara algún *asset* extra para alguna actividad se pueden dejar los ficheros necesarios en la carpeta `assets` a tal efecto en la estructura del proyecto.

Para la entrega se recomienda crear un archivo comprimido que contenga todos los ficheros de código y assets necesarios para la compilación y ejecución de los proyectos. En dicho archivo no se deben incluir los binarios.

- 01 - Hacer el modelo de iluminación Phong-Blinn en espacio de view, en lugar de en espacio de mundo.
- 02 - En la cámara, implementar la función LookAt, y no usar la de GLM.
- 03 - Implementar un nuevo tipo de luz, FlashLight o Linterna, que funcione como una SpotLight (conos suavizados), pero que se mueva con la cámara y siempre ilumine hacia adelante. Como si el personaje de un juego llevara una linterna siempre apuntando hacia adelante.
- 04 - Importar un modelo 3D complejo a elección (de algún repositorio de modelos 3D gratis). El modelo tiene que tener lo necesario para pintarlo correctamente. Para pintar hay que usar `normal mapping` con el modelo de iluminación `Blinn`, mapas difusos y `speculares`, y usar al menos una luz direccional, y una point light.
- 05 - Usando una escena 3D cualquiera, pintarla de manera normal desde el punto de vista de una cámara. Pero poner un panel en una esquina de la escena mostrando la misma escena desde el punto de vista de otra cámara fija, usando un `framebuffer` para ello.