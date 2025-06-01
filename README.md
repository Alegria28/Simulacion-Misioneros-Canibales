# Simulación del Problema de Misioneros y Caníbales

Este proyecto es una simulación gráfica del clásico problema de lógica de los Misioneros y Caníbales, implementado en C++ utilizando la biblioteca Allegro 5. El programa permite al usuario definir el número inicial de misioneros y caníbales, y luego visualiza la solución paso a paso, si existe.

## Características

*   **Resolución del Problema:** Implementa un algoritmo de búsqueda (DFS) para encontrar una solución al problema de Misioneros y Caníbales.
*   **Visualización Gráfica:** Utiliza Allegro 5 para dibujar el estado del problema, incluyendo las orillas del río, el bote, y las figuras de los misioneros y caníbales.
*   **Animación de la Solución:** Muestra la secuencia de movimientos de la solución encontrada de forma animada.
*   **Entrada de Usuario:** Permite al usuario especificar el número inicial de misioneros y caníbales.
*   **Control de Velocidad:** El usuario puede ajustar la velocidad de la animación de la solución.
*   **Detección de Estados Inválidos:** Verifica la validez de los estados para asegurar que los misioneros no sean superados en número por los caníbales en ninguna orilla.
*   **Interfaz Interactiva:** Guía al usuario a través de diferentes fases: entrada de datos, resolución/animación, y estado final (solución encontrada o sin solución).

## Requisitos

*   Un compilador de C++ (como g++)
*   Allegro 5 

Asegúrate de tener Allegro 5 instalado en tu sistema.

**Para Ubuntu 18.04+ (o derivados):**

Primero, añade el PPA de Allegro. Esto te dará versiones actualizadas de Allegro:
```bash
sudo add-apt-repository ppa:allegro/5.2
sudo apt-get update
```
Luego, instala Allegro y los addons necesarios:
```bash
sudo apt-get install liballegro*5.2 liballegro*5-dev
```

**Para Windows:**

Existen varias formas de obtener los binarios de Allegro para Windows:
*   **Nuget:** El paquete de Allegro en Nuget contiene los binarios oficiales en un formato fácil de usar con MSVC.
*   **Allegro 5.2 binary packages:** Las releases de Allegro en GitHub también vienen con varias distribuciones binarias de Allegro para Windows.
*   **Allegro 5 dependencies:** El proyecto `allegro_winpkg` en GitHub tiene releases que vienen con las dependencias de Allegro precompiladas para Windows.

Consulta el [wiki tutorial de Allegro](https://liballeg.org/download.html#windows) para instrucciones detalladas de instalación en Windows.

Para otras distribuciones o sistemas operativos, consulta la [documentación oficial de Allegro](https://liballeg.org/download.html).

## Compilación y Ejecución

El proyecto incluye tareas de VS Code para facilitar la compilación y ejecución. Si no usas VS Code, puedes compilar el archivo `main.cpp` manualmente desde la terminal.

**Usando las tareas de VS Code:**

1.  Abre el proyecto en VS Code.
2.  Abre `main.cpp`.
3.  Presiona `Ctrl+Shift+B` (o `Cmd+Shift+B` en macOS).
4.  Selecciona la tarea:
    *   `Compile Active Allegro File`: Solo compila el archivo.
    *   `Compile and Run Active Allegro File`: Compila y luego ejecuta la simulación.

**Manualmente desde la terminal (Linux):**

Navega al directorio del proyecto y ejecuta el siguiente comando para compilar:
```bash
g++ main.cpp -o misioneros_canibales -std=c++11 `pkg-config --cflags allegro-5 allegro_font-5 allegro_primitives-5` `pkg-config --libs allegro-5 allegro_font-5 allegro_primitives-5`
```
Luego, para ejecutar la simulación:
```bash
./misioneros_canibales
```

## Cómo Jugar

1.  **Establecer Números Iniciales:**
    *   Al iniciar, estarás en la pantalla de configuración.
    *   Usa las teclas de **FLECHA ARRIBA** y **FLECHA ABAJO** para aumentar o disminuir el número de misioneros o caníbales seleccionados.
    *   Usa las teclas de **FLECHA IZQUIERDA** y **FLECHA DERECHA** para cambiar entre la selección de misioneros y caníbales.
    *   Presiona **ENTER** para iniciar la simulación con los números establecidos.

2.  **Visualización de la Solución:**
    *   Si se encuentra una solución, la animación comenzará a mostrar los pasos.
    *   Usa la **FLECHA ARRIBA** para aumentar la velocidad de la simulación.
    *   Usa la **FLECHA ABAJO** para disminuir la velocidad de la simulación (mínimo 0.5x).

3.  **Fin de la Simulación:**
    *   Si se encuentra una solución, se mostrará el mensaje "SOLUCION ENCONTRADA!" y "Simulacion Completa".
    *   Si no se encuentra una solución para los valores dados, se mostrará "NO SE ENCONTRO SOLUCION para estos valores."
    *   Presiona **ESC** en cualquier momento para salir del programa.

## Estructura del Código

*   `main.cpp`: Contiene toda la lógica del juego, incluyendo:
    *   Definición de la estructura `Estado`.
    *   Funciones para la validación de estados (`es_valido`, `es_solucion`).
    *   Función recursiva `resolver_mc_recursivo` para encontrar la solución.
    *   Funciones de dibujo (`dibujar_persona`, `dibujar_estado`) utilizando Allegro.
    *   La función `main` que maneja el bucle principal del juego, eventos y la lógica de las fases.

## Lógica del Problema

El problema consiste en mover a todos los misioneros y caníbales de una orilla de un río a la otra, utilizando un bote con una capacidad limitada (en esta simulación, 2 personas). La restricción principal es que en ninguna orilla, ni en el bote, el número de caníbales puede superar al número de misioneros, ya que de lo contrario los misioneros serían devorados.
El bote no puede cruzar el río vacío.
