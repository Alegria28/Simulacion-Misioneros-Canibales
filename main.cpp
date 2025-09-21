// Librerías de allegro, ver documentación https://liballeg.org/a5docs/5.2.10.1/
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
// Librerías estándar de c++
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <set>
#include <fstream>

using namespace std;

// Dimensiones de la pantalla
const int screen_width = 800;
const int screen_height = 600;

// Estados del juego
enum CurrentPhase
{
    KEYBOARD_INPUT,
    SOLVING,
    SOLVED,
    NO_SOLUTION
};

// Estructura para representar el estado del problema
struct State
{
    // Misioneros y caníbales en izquierda
    int m_left, c_left;
    // Misioneros y caníbales en la derecha
    int m_right, c_right;
    // Declaramos la posición del bote con 0 para la izquierda y 1 para la derecha
    int boat_pos;

    /*
    Dado que vamos a utilizar esta estructura en un set, necesitamos decirle como es que los debe de ordenar y
    si la estructura esta repetida
    */

    // Sobrecargamos el operador ==, comparando con la otra estructura (estado2) y sus miembros, se utiliza
    // el segundo const para asegurarnos de que en ningún momento se modifique algún valor (ya que solo estamos comparando),
    // ver https://shorturl.at/73FC6
    bool operator==(const State &state2) const
    {
        // Si en algún momento un miembro es distinto, se devolverá false y obviamente esto significara que
        // las 2 estructuras no son iguales
        return m_left == state2.m_left && c_left == state2.c_left &&
               m_right == state2.m_right && c_right == state2.c_right &&
               boat_pos == state2.boat_pos;
    }

    // Volvemos a hacer la sobrecarga pero ahora para el operador <, para que set sepa cual estructura va antes que la otra
    bool operator<(const State &state2) const
    {
        // La comparación se hace con cada uno de los miembros de ambas estructuras
        if (m_left != state2.m_left)
            return m_left < state2.m_left;
        if (c_left != state2.c_left)
            return c_left < state2.c_left;
        if (m_right != state2.m_right)
            return m_right < state2.m_right;
        if (c_right != state2.c_right)
            return c_right < state2.c_right;
        return boat_pos < state2.boat_pos;
    }
};

// La capacidad del bote para el problema es de 2
const int BOAT_CAPACITY = 2;

// Esta es la verificación mas importante, ya que a medida que vamos resolviendo el problema se utilizara para ver si
// vamos por buen camino o si se ha cometido algún error
bool is_valid(const State &s, int total_m, int total_c)
{
    // Comprueba números negativos
    if (s.m_left < 0 || s.c_left < 0 || s.m_right < 0 || s.c_right < 0)
        return false;

    // Comprueba si los misioneros son superados en número en la orilla izquierda
    if (s.m_left > 0 && s.m_left < s.c_left)
        return false;
    // Comprueba si los misioneros son superados en número en la orilla derecha
    if (s.m_right > 0 && s.m_right < s.c_right)
        return false;

    // Si ninguna de estas condiciones es verdadera, significa que el estado actual
    // del problema es valido
    return true;
}

// Función simple para comprobar si se ha resuelto el problema
bool is_solution(const State &s, int total_m, int total_c)
{
    // Donde los misioneros, caníbales y el bote estén a la izquierda
    return s.m_left == 0 && s.c_left == 0 && s.m_right == total_m && s.c_right == total_c && s.boat_pos == 1;
}

// Función recursiva usando DFS
bool solve_mc_recursive(State current_s, int total_m, int total_c, vector<State> &path, set<State> &visited)
{
    // Lo primero que se hace es agregar al final de nuestro vector el estado actual
    path.push_back(current_s);
    // Agregamos el estado a nuestro set de estados visitados (recordar que no permite estructuras permitidas)
    visited.insert(current_s);

    // Verificamos si el estado actual es la solución al problema (caso base)
    if (is_solution(current_s, total_m, total_c))
    {
        // Nuestra función devuelve verdadero
        return true;
    }

    /* Exploramos los movimientos posibles*/

    // Iteramos sobre el numero de misioneros que podrían ir en el bote (de 0 a BOAT_CAPACITY, que en este caso es 2)
    for (int m_boat = 0; m_boat <= BOAT_CAPACITY; ++m_boat)
    {
        // Nuevamente iteramos sobre el numero de caníbales que podrían ir en el bote, haciendo una validación sobre el numero de
        // caníbales que hay en el (c_boat <= BOAT_CAPACITY - m_boat), esto para asegurarnos de que no nos pasemos de la capacidad
        // del bote
        for (int c_boat = 0; c_boat <= BOAT_CAPACITY - m_boat; ++c_boat)
        {

            /*
            Utilizamos los 2 for anidados para obtener todas las combinaciones posibles de misioneros y caníbales que pueden ir en el bote
            respetando su capacidad
            */

            // Si tenemos el caso de que el bote este vació, simplemente ignoramos este y seguimos con la siguiente iteración
            if (m_boat + c_boat == 0)
            {
                continue;
            }

            // Guardamos el estado actual del problema
            State next_s = current_s;

            // Si el bote se encuentra del lado izquierdo
            if (current_s.boat_pos == 0)
            {
                // Verificamos si el numero de misioneros y caníbales que vamos a probar (según los 2 for anidados) están disponibles
                // del lado izquierdo
                if (current_s.m_left >= m_boat && current_s.c_left >= c_boat)
                {
                    // De ser asi, entonces quitamos los misioneros y caníbales de la izquierda
                    next_s.m_left -= m_boat;
                    next_s.c_left -= c_boat;
                    // Y agregamos esta misma cantidad al lado derecho, simulando que paso el bote
                    next_s.m_right += m_boat;
                    next_s.c_right += c_boat;
                    // Cambiamos la posición del bote
                    next_s.boat_pos = 1;
                }
                // Si no hay suficientes misioneros y/o caníbales para este movimiento, entonces lo ignoramos y seguimos con
                // la siguiente iteración
                else
                {
                    continue;
                }
            }
            // Si el bote se encuentra del lado derecho
            else
            {
                // Realizamos la misma verificación para ver si del lado derecho hay suficientes misioneros y caníbales
                // disponibles para el movimiento
                if (current_s.m_right >= m_boat && current_s.c_right >= c_boat)
                {
                    // Quitamos los misioneros y caníbales del lado derecho
                    next_s.m_right -= m_boat;
                    next_s.c_right -= c_boat;
                    // Y los agregamos al otro lado
                    next_s.m_left += m_boat;
                    next_s.c_left += c_boat;
                    // Cambiamos la posición del bote
                    next_s.boat_pos = 0;
                }
                // Si no hay suficientes misioneros y/o caníbales para este movimiento, entonces lo ignoramos y seguimos con
                // la siguiente iteración
                else
                {
                    continue;
                }
            }

            // Tras haber hecho el movimiento (ya sea de izquierda a derecha o de derecha a izquierda), verificamos si el el estado actual del problema es valido,
            // ademas, buscamos si el estado actual ya ha sido visitado en el set y si lo encuentra, entonces devuelve un iterator apuntando a ese elemento, en caso
            // contrario devuelve un .end()
            if (is_valid(next_s, total_m, total_c) && visited.find(next_s) == visited.end())
            {
                // Si entramos al if es porque el estado actual del problema es valido y este no se encontró en el set, por lo que podemos seguir investigando las posibilidades
                // de este estado (DFS) esperando encontrar la solución
                if (solve_mc_recursive(next_s, total_m, total_c, path, visited))
                {
                    // Si esta función devuelve verdadero, entonces se ha encontrado la solución al problema, y retornando true propagamos como una señal de "solución encontrada"
                    // hacia arriba en la cadena de llamadas recursivas
                    return true;
                }
            }
        }
    }

    // Si llegamos a este punto, es porque durante la exploración de movimientos posibles, no se encontró ningún estado del problema valido, por lo que quitamos
    // este estado de nuestro vector
    path.pop_back();
    // Dado que no se resolvió y ni se encontró posibles caminos a soluciones, devolvemos false
    return false;
}

// Esta es la función principal la cual se va a encargar de solucionar el problema (si es que hay solución de este)
bool solve_mc(int initial_m, int initial_c, vector<State> &path)
{
    // Declaramos un set que trabaja con la estructura State
    set<State> visited;
    // Creamos una estructura inicial con el numero de misioneros y caníbales a trabajar (todos del lado izquierdo) y aparte
    // la posición del bote que esta del lado izquierdo
    State initial_state = {initial_m, initial_c, 0, 0, 0};

    // Antes de empezar a resolverlo, debemos de verificar si el problema se puede resolver con estos datos, para este momento,
    // simplemente se hace para ver si los misioneros pierden en numero contra los caníbales (lo cual no es un estado inicial
    // valido)
    if (!is_valid(initial_state, initial_m, initial_c))
        // De ser asi entonces ni siquiera podemos resolver el problema
        return false;

    // Si el problema es valido desde el inicio, entonces ya podemos resolver el problema
    return solve_mc_recursive(initial_state, initial_m, initial_c, path, visited);
}

// Función para dibujar una persona individualmente, ya sea misionero o caníbal, en una posición específica y con un color dado
void draw_person(float x, float y, ALLEGRO_COLOR color, bool is_missionary)
{
    // Dibuja un círculo relleno que representa el cuerpo de la persona
    al_draw_filled_circle(x, y, 10, color); // Cuerpo de la persona, un círculo de radio 10
    // Si la persona a dibujar es un misionero
    if (is_missionary)
    {                                                                                      // Para un misionero, se dibuja una especie de sombrero o una cruz simple
        al_draw_line(x - 3, y - 12, x + 3, y - 12, al_map_rgb(255, 255, 255), 2);          // Dibuja la línea horizontal del ala del sombrero
        al_draw_filled_rectangle(x - 2, y - 15, x + 2, y - 12, al_map_rgb(255, 255, 255)); // Dibuja la parte superior rectangular del sombrero
    }
    // Si no es misionero, entonces es un caníbal
    else
    {                                                                       // Para un caníbal, se dibujan dos líneas pequeñas como cuernos
        al_draw_line(x - 3, y - 12, x - 1, y - 15, al_map_rgb(0, 0, 0), 2); // Dibuja el cuerno izquierdo
        al_draw_line(x + 3, y - 12, x + 1, y - 15, al_map_rgb(0, 0, 0), 2); // Dibuja el cuerno derecho
    }
}

// Función principal para dibujar el programa
void draw_state(const State &current_s, const State *ptr_prev_s, int total_initial_m, int total_initial_c, ALLEGRO_FONT *font, float boat_anim_progress)
{
    // Definimos los colores que se utilizarán para los distintos elementos de la simulación
    ALLEGRO_COLOR missionary_color = al_map_rgb(0, 0, 255); // Azul para los Misioneros
    ALLEGRO_COLOR cannibal_color = al_map_rgb(255, 0, 0);   // Rojo para los Caníbales
    ALLEGRO_COLOR boat_color = al_map_rgb(139, 69, 19);    // Marrón para el Bote
    ALLEGRO_COLOR water_color = al_map_rgb(0, 105, 148);    // Azul oscuro para el Agua del río
    ALLEGRO_COLOR shore_color = al_map_rgb(34, 139, 34);  // Verde para las Orillas

    // Definimos las dimensiones y posiciones relativas de las orillas y el río en la pantalla
    float shore_width = screen_width * 0.25f;  // Cada orilla ocupa un 25% del ancho de la pantalla
    float river_width = screen_width * 0.5f;      // El río ocupa el 50% central del ancho de la pantalla
    float shore_y_start = screen_height * 0.4f; // Posición Y donde comienzan a dibujarse las orillas y el río
    float shore_height = screen_height * 0.5f;     // Altura de las orillas y el río

    // Inicializamos los contadores de misioneros y caníbales en cada orilla basándonos en el estado actual
    int m_on_left_shore = current_s.m_left;
    int c_on_left_shore = current_s.c_left;
    int m_on_right_shore = current_s.m_right;
    int c_on_right_shore = current_s.c_right;

    // Inicializamos los contadores de misioneros y caníbales en el bote a cero, se actualizarán si el bote está en tránsito
    int m_in_boat = 0;
    int c_in_boat = 0;

    // Definimos las dimensiones del bote
    float boat_width = 80;
    float boat_height = 40;
    // Calculamos la posición Y del bote, centrado verticalmente en la zona media del río/orillas
    float boat_y = shore_y_start + shore_height * 0.3f;
    // Calculamos la posición X del bote cuando está completamente en el muelle izquierdo
    float boat_left_dock_x = shore_width - boat_width / 2;
    // Calculamos la posición X del bote cuando está completamente en el muelle derecho
    float boat_right_dock_x = shore_width + river_width - boat_width / 2;

    // Variable para almacenar la posición X actual del bote para el dibujo, se calculará según la animación
    float current_boat_draw_x;

    // Determinamos la posición X visual del bote para la animación
    // Si la posición objetivo del bote en el estado actual (current_s.boat_pos) es la derecha (1)
    if (current_s.boat_pos == 1)
    {
        // El bote se mueve de izquierda a derecha, o ya está a la derecha
        // Interpolamos la posición X del bote desde el muelle izquierdo hacia el muelle derecho
        // basado en el progreso de la animación (boat_anim_progress)
        current_boat_draw_x = boat_left_dock_x + (boat_right_dock_x - boat_left_dock_x) * boat_anim_progress;
    }
    // Si la posición objetivo del bote es la izquierda (0)
    else
    {
        // El bote se mueve de derecha a izquierda, o ya está a la izquierda
        // Interpolamos la posición X del bote desde el muelle derecho hacia el muelle izquierdo
        // basado en el progreso de la animación (boat_anim_progress)
        current_boat_draw_x = boat_right_dock_x - (boat_right_dock_x - boat_left_dock_x) * boat_anim_progress;
    }

    // Lógica para determinar quiénes están en el bote y ajustar los conteos en las orillas si el bote está en tránsito
    // Se activa si existe un estado previo (ptr_prev_s no es nulo) y la animación del bote está en curso (entre 0 y 1)
    if (ptr_prev_s && boat_anim_progress > 0.0f && boat_anim_progress < 1.0f)
    {
        // El bote está actualmente moviéndose entre orillas
        const State &prev_s = *ptr_prev_s; // Referencia al estado anterior para comparaciones

        // Si el bote en el estado previo estaba en la orilla izquierda (0)
        if (prev_s.boat_pos == 0)
        {
            // El bote se estaba moviendo de Izquierda a Derecha (current_s.boat_pos debería ser 1)
            // Calculamos cuántos misioneros estaban en el bote restando los actuales en la orilla izq. de los que había antes
            m_in_boat = prev_s.m_left - current_s.m_left;
            // Calculamos cuántos caníbales estaban en el bote de forma similar
            c_in_boat = prev_s.c_left - current_s.c_left;

            // Los que quedan en la orilla izquierda son los de current_s (ya se restaron los del bote)
            m_on_left_shore = current_s.m_left;
            c_on_left_shore = current_s.c_left;
            // Los que están en la orilla derecha son los que ya estaban allí en el estado previo (aún no han llegado los del bote)
            m_on_right_shore = prev_s.m_right;
            c_on_right_shore = prev_s.c_right;
        }
        // Si el bote en el estado previo estaba en la orilla derecha (1)
        else
        {
            // El bote se estaba moviendo de Derecha a Izquierda (current_s.boat_pos debería ser 0)
            // Calculamos cuántos misioneros estaban en el bote restando los actuales en la orilla der. de los que había antes
            m_in_boat = prev_s.m_right - current_s.m_right;
            // Calculamos cuántos caníbales estaban en el bote de forma similar
            c_in_boat = prev_s.c_right - current_s.c_right;

            // Los que quedan en la orilla derecha son los de current_s
            m_on_right_shore = current_s.m_right;
            c_on_right_shore = current_s.c_right;
            // Los que están en la orilla izquierda son los que ya estaban allí en el estado previo
            m_on_left_shore = prev_s.m_left;
            c_on_left_shore = prev_s.c_left;
        }

        // Aseguramos que los conteos en el bote no sean negativos
        m_in_boat = max(0, m_in_boat);
        c_in_boat = max(0, c_in_boat);
    }

    // Si el bote no está en tránsito (está atracado o no hay estado previo para la animación),
    // los conteos en las orillas ya son los de current_s y los del bote permanecen en 0 (como se inicializaron)

    // --- Comienza el dibujo de los elementos ---

    // Dibujar la orilla izquierda como un rectángulo relleno
    al_draw_filled_rectangle(0, shore_y_start, shore_width, shore_y_start + shore_height, shore_color);
    // Dibujar la orilla derecha como un rectángulo relleno
    al_draw_filled_rectangle(shore_width + river_width, shore_y_start, screen_width, shore_y_start + shore_height, shore_color);

    // Dibujar el río como un rectángulo relleno entre las dos orillas
    al_draw_filled_rectangle(shore_width, shore_y_start, shore_width + river_width, shore_y_start + shore_height, water_color);

    // Dibujar las personas en la orilla izquierda
    float person_x_start_left = shore_width * 0.1f; // Posición X inicial para la primera persona en la orilla izquierda
    float person_y_start = shore_y_start + 30;          // Posición Y inicial para la fila de misioneros
    float person_spacing = 25;                           // Espacio horizontal entre cada persona

    // Bucle para dibujar cada misionero en la orilla izquierda
    for (int i = 0; i < m_on_left_shore; ++i)
    {
        draw_person(person_x_start_left + i * person_spacing, person_y_start, missionary_color, true);
    }
    // Bucle para dibujar cada caníbal en la orilla izquierda, en una fila debajo de los misioneros
    for (int i = 0; i < c_on_left_shore; ++i)
    {
        draw_person(person_x_start_left + i * person_spacing, person_y_start + 40, cannibal_color, false);
    }

    // Dibujar las personas en la orilla derecha
    float person_x_start_right = shore_width + river_width + shore_width * 0.1f; // Posición X inicial para la primera persona en la orilla derecha
    // Bucle para dibujar cada misionero en la orilla derecha
    for (int i = 0; i < m_on_right_shore; ++i)
    {
        draw_person(person_x_start_right + i * person_spacing, person_y_start, missionary_color, true);
    }
    // Bucle para dibujar cada caníbal en la orilla derecha, en una fila debajo de los misioneros
    for (int i = 0; i < c_on_right_shore; ++i)
    {
        draw_person(person_x_start_right + i * person_spacing, person_y_start + 40, cannibal_color, false);
    }

    // Dibujar el Bote en su posición X actual calculada (current_boat_draw_x)
    al_draw_filled_rectangle(current_boat_draw_x, boat_y, current_boat_draw_x + boat_width, boat_y + boat_height, boat_color);
    // Dibujar un contorno negro alrededor del bote para mejor visibilidad
    al_draw_rectangle(current_boat_draw_x, boat_y, current_boat_draw_x + boat_width, boat_y + boat_height, al_map_rgb(0, 0, 0), 2);

    // Dibujar las personas que están actualmente en el bote
    if (m_in_boat > 0 || c_in_boat > 0)
    {
        // Calculamos el desplazamiento Y para centrar las personas verticalmente en el bote
        float boat_person_y_offset = boat_height / 2;
        // Calculamos el número total de personas en el bote para distribuir el espacio
        int total_in_boat = m_in_boat + c_in_boat;
        // Calculamos el espaciado horizontal entre personas dentro del bote
        float boat_person_x_spacing = boat_width / (total_in_boat + 1);
        int person_idx_in_boat = 0; // Índice para posicionar secuencialmente a las personas en el bote
        // Bucle para dibujar a los misioneros en el bote
        for (int i = 0; i < m_in_boat; ++i)
        {
            draw_person(current_boat_draw_x + boat_person_x_spacing * (person_idx_in_boat + 1), boat_y + boat_person_y_offset, missionary_color, true);
            person_idx_in_boat++; // Incrementar índice para la siguiente persona
        }
        // Bucle para dibujar a los caníbales en el bote
        for (int i = 0; i < c_in_boat; ++i)
        {
            draw_person(current_boat_draw_x + boat_person_x_spacing * (person_idx_in_boat + 1), boat_y + boat_person_y_offset, cannibal_color, false);
            person_idx_in_boat++; // Incrementar índice para la siguiente persona
        }
    }

    // Mostrar los conteos de misioneros y caníbales en cada orilla usando texto
    stringstream left_shore_text, right_shore_text; // Stringstreams para formatear el texto de los conteos
    // Formatear texto para la orilla izquierda usando los valores de current_s (reflejan el estado final del movimiento si está atracado)
    left_shore_text << "Left Shore: M=" << current_s.m_left << ", C=" << current_s.c_left;
    // Formatear texto para la orilla derecha usando los valores de current_s
    right_shore_text << "Right Shore: M=" << current_s.m_right << ", C=" << current_s.c_right;
    // El contenido del bote se dibuja visualmente, el texto del bote podría ser redundante o complejo si se muestra siempre

    // Dibujar el texto del conteo para la orilla izquierda, centrado sobre la orilla
    al_draw_text(font, al_map_rgb(255, 255, 255), shore_width / 2, shore_y_start - 60, ALLEGRO_ALIGN_CENTRE, left_shore_text.str().c_str());
    // Dibujar el texto del conteo para la orilla derecha, centrado sobre la orilla
    al_draw_text(font, al_map_rgb(255, 255, 255), shore_width + river_width + shore_width / 2, shore_y_start - 60, ALLEGRO_ALIGN_CENTRE, right_shore_text.str().c_str());

    // Mostrar los conteos totales iniciales de misioneros y caníbales como referencia en la esquina superior derecha
    stringstream total_counts_text;
    total_counts_text << "Total M: " << total_initial_m << ", Total C: " << total_initial_c;
    al_draw_text(font, al_map_rgb(200, 200, 200), screen_width - 10, 10, ALLEGRO_ALIGN_RIGHT, total_counts_text.str().c_str());
}

int main()
{

    /* ------ Iniciar addons ------ */

    // Usamos al_init para iniciar los addons que se van a utilizar durante el programa,
    // ver https://shorturl.at/2Iknn para mas información sobre que son los addons

    // Iniciamos allegro
    al_init();
    // Iniciamos el teclado
    al_install_keyboard();
    // Iniciamos el addon para poder dibujar figuras
    al_init_primitives_addon();
    // Inicializar addon de fuente
    al_init_font_addon();

    // Creamos un temporizador que se va a activa cada (1.0 / 60.0) veces, de esta manera obteniendo una taza de refresco de 60hz
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    // Creamos una cola de eventos, esto se hace para manejar input dado por el usuario, ver https://shorturl.at/1pUjt
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    // Creamos nuestra pantalla pasándole el alto y ancho definido en las constantes
    ALLEGRO_DISPLAY *display = al_create_display(screen_width, screen_height);
    // Para simplicidad del proyecto, trabajamos con la fuente base de allegro: "Creates a monochrome bitmap font (8x8 pixels per character)." según
    // la documentación oficial
    ALLEGRO_FONT *font = al_create_builtin_font();

    // Registramos el "event source" en nuestra cola de eventos para los siguientes eventos:
    // - El teclado
    // - La pantalla
    // - El temporizador
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    // Creamos un evento, el cual se utilizara para "esperar" o "recibir" uno según los que hemos especificado
    // en nuestra cola de eventos
    ALLEGRO_EVENT event;

    // Creamos un enum para mantener un control del ciclo de la simulación (dentro de las variables predefinidas dadas), dado que acabamos de iniciar, l
    // a fase actual es 0 (dado por KEYBOARD_INPUT)
    CurrentPhase current_phase = KEYBOARD_INPUT;

    // Creamos nuestras variables para guardar el input ingresado por el usuario (iniciamos con 3 misioneros y 3 caníbales)
    int num_missionaries_input = 3;
    int num_cannibals_input = 3;
    // Para saber en que campo estamos durante el menu de selección, marcamos 2 estados:
    // - 0 seleccionando el campo misioneros
    // - 1 seleccionando el campo caníbales
    int selected_input_field = 0;

    // Marcamos la velocidad inicial en 1
    float simulation_speed = 1.0f;

    // Declaramos un vector que va a guardar estructuras de tipo State
    vector<State> solution_path;
    // Este va a ser nuestro indice para el vector
    int current_solution_step = 0;
    // Simple variable para almacenar la duración del frame
    float time_since_last_step = 0.0f;

    // Esta variable nos ayuda a "animar" el bote durante la resolución, donde es 0.0 al inicio del movimiento
    // y 1.0 al finalizarlo
    float boat_animation_progress = 0.0f;

    // Antes de iniciar todo el ciclo de la simulación, iniciamos nuestro timer
    al_start_timer(timer);

    // Esta variable nos indica si debemos de hacer una "actualización" (un fotograma) de nuestra pantalla
    bool redraw;

    // Ciclo infinito para nuestra simulación
    while (1)
    {
        // Aquí estaremos esperando unicamente por los event sources especificados anteriormente en la función
        // al_register_event_source(), ver https://shorturl.at/KBKRk entender como funciona esta función tan importante
        al_wait_for_event(event_queue, &event);

        // Si el evento a procesar el el timer, el cual sera cierto si es que no hay ningún input por parte del usuario, (hay que recordar que el
        // timer da una señal cada 1.0 / 60.0s) entonces ingresamos a este if
        if (event.type == ALLEGRO_EVENT_TIMER)
        {
            // Tras cada frame, se dibujara nuevamente
            redraw = true;

            // Si ya estamos mostrando la solución, significa que el vector no esta vacio, ya que la función solve_mc lo lleno
            // con toda la secuencia de estados para llegar a la solución
            if (current_phase == SOLVING && !solution_path.empty())
            {
                // Guardamos el tiempo pasado en un frame, acumulándolo para controlar la duración del paso actual de la animación
                time_since_last_step += (1.0 / 60.0);
                // Calculamos el progreso de la animación del bote como una fracción de la duración total del paso,
                // esto depende del tiempo acumulado y la velocidad de simulación elegida por el usuario
                boat_animation_progress = time_since_last_step * simulation_speed;
                // Aseguramos que el progreso no exceda 1, significando movimiento finalizado
                boat_animation_progress = min(1.0f, boat_animation_progress);

                // Si el tiempo acumulado es mayor o igual a la duración esperada para un paso de la animación,
                // significa que la animación de este paso ha concluido
                if (time_since_last_step >= (1.0 / simulation_speed))
                {
                    // Avanzamos al siguiente paso de la solución, nuestro indice en el vector
                    current_solution_step++;

                    // Si hemos superado el último paso de la solución
                    if (current_solution_step >= solution_path.size())
                    {
                        // Cambiamos la fase a SOLVED, indicando que la simulación ha terminado
                        current_phase = SOLVED;
                        // Mantenemos el índice en el último paso para mostrar el estado final
                        current_solution_step = solution_path.size() - 1;
                        // Reiniciamos el progreso del bote para que se muestre estático en la orilla final
                        boat_animation_progress = 0.0f;

                        // Abrimos el archivo resultados, y si este no existe, entonces lo creamos
                        ofstream results_file("results.txt");

                        // Verificamos si este se abrio correctamente
                        if (results_file.is_open())
                        {
                            // Imprimimos arriba de este archivo la información sobre el problema que se resolvió
                            results_file << "Solution for Missionaries: " << num_missionaries_input << ", Cannibals: " << num_cannibals_input << endl;
                            results_file << "Boat capacity: " << BOAT_CAPACITY << endl;
                            results_file << "------------------------------------------" << endl;
                            // Recorremos nuestro vector
                            for (size_t i = 0; i < solution_path.size(); ++i)
                            {
                                // Guardamos una instancia temporal para el estado del vector
                                State &s = solution_path[i];
                                // Imprimimos la información de este estado en el archivo
                                results_file << "Step " << i << ": "
                                                   << "Left(M:" << s.m_left << ", C:" << s.c_left << ") "
                                                   << "Right(M:" << s.m_right << ", C:" << s.c_right << ") "
                                                   << "Boat:" << (s.boat_pos == 0 ? "Left" : "Right") << endl;
                            }
                            // Tras finalizar cerramos el archivo
                            results_file.close();
                        }
                        else
                        // Si no se pudo abrir o crear
                        {
                            // cerr se utiliza como un cout pero este esta especializado para mandar mensajes de errores en stderr, ademas
                            // de que este no se guarda en buffer y se muestra inmediatamente
                            cerr << "Error: Could not open results.txt to save the results." << endl;
                        }
                    }
                    else
                    {
                        // Si aún hay pasos, reiniciamos el progreso del bote para la animación del nuevo paso
                        boat_animation_progress = 0.0f;
                    }
                    // Reseteamos el contador de tiempo para el nuevo paso de la animación
                    time_since_last_step = 0;
                }
            }
        }
        // Si el evento procesado es la "X" en nuestra ventana, entonces rompemos el ciclo y terminara el programa
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break;
        }
        // Si se pulsa un tecla (KEY_DOWN)
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            // Si la tecla presionada es el ESC, entonces rompemos el ciclo y terminara el programa
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                break;

            // Si al presionar una tecla seguimos estamos en la primera fase
            if (current_phase == KEYBOARD_INPUT)
            {
                // Y la tecla presionada es flecha arriba
                if (event.keyboard.keycode == ALLEGRO_KEY_UP)
                {
                    // Y el campo seleccionado es misioneros
                    if (selected_input_field == 0)
                        // Entonces aumentamos nuestro contador
                        num_missionaries_input++;
                    else
                        num_cannibals_input++;
                }
                // Si la tecla presionada es flecha abajo
                else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)
                {
                    // Y el campo seleccionado es misioneros
                    if (selected_input_field == 0)
                        // Disminuimos nuestro contador, pero para que este nunca llegue a ser negativo, obtenemos el máximo entre 0 y
                        // el valor actual de la variable, para que en caso de que si la variable llega a ser negativa (el usuario disminuyo
                        // muchas veces) entonces max simplemente agarrara 0 y la variable valdrá eso
                        num_missionaries_input = max(0, num_missionaries_input - 1);
                    else
                        num_cannibals_input = max(0, num_cannibals_input - 1);
                }
                // Si la tecla seleccionada es la flecha derecha o izquierda
                else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT || event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
                {
                    // Cambiamos el input seleccionado (recordar que son valores entre 0 y 1)
                    selected_input_field = 1 - selected_input_field;
                }
                // Si la tecla seleccionada es enter
                else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
                {
                    // Solo por buena practica, limpiamos nuestro vector antes de resolver el problema
                    solution_path.clear();
                    // Nuevamente por buena practica, limpiamos nuestras variables
                    current_solution_step = 0;
                    time_since_last_step = 0;
                    boat_animation_progress = 0.0f;

                    // Llamamos a nuestra función para resolver el problema de misioneros y caníbales manando la información de este, asi como
                    // el vector que va a representar nuestra solución
                    if (solve_mc(num_missionaries_input, num_cannibals_input, solution_path))
                    {
                        // Dado que la situación inicial es valida, entonces cambiamos la fase
                        current_phase = SOLVING;
                    }
                    // Si no hay una situación inicial valida, entonces cambiamos la fase
                    else
                    {
                        current_phase = NO_SOLUTION;
                    }
                }
            }
            // Si no estamos en la primera fase y se presiona una tecla
            else if (current_phase == SOLVING || current_phase == SOLVED)
            {
                // Si esta es la tecla de flecha arriba
                if (event.keyboard.keycode == ALLEGRO_KEY_UP)
                {
                    // Aumentamos la velocidad de simulación en un 0.5, utilizando min para tener un tope hasta 10
                    simulation_speed = min(10.0f, simulation_speed + 0.5f);
                }
                // Si la tecla es flecha abajo
                else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)
                {
                    // Disminuimos la velocidad en 0.5, utilizando max para tener un tope hasta 0.5
                    simulation_speed = max(0.5f, simulation_speed - 0.5f);
                }
                // Si se presiona R, regresar al menú principal
                else if (event.keyboard.keycode == ALLEGRO_KEY_R)
                {
                    // Cambiamos la fase de vuelta a entrada de teclado
                    current_phase = KEYBOARD_INPUT;
                    // Limpiamos la solución actual
                    solution_path.clear();
                    // Reiniciamos variables de control de animación
                    current_solution_step = 0;
                    time_since_last_step = 0;
                    boat_animation_progress = 0.0f;
                    // Reiniciamos la velocidad de simulación
                    simulation_speed = 1.0f;
                }
                // Si se presiona ESPACIO, reiniciar la simulación con los mismos valores
                else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE)
                {
                    // Limpiamos la solución actual
                    solution_path.clear();
                    // Reiniciamos variables de control
                    current_solution_step = 0;
                    time_since_last_step = 0;
                    boat_animation_progress = 0.0f;
                    // Reiniciamos la velocidad de simulación
                    simulation_speed = 1.0f;

                    // Volvemos a resolver el problema con los mismos valores
                    solve_mc(num_missionaries_input, num_cannibals_input, solution_path);

                    // Cambiamos la fase
                    current_phase = SOLVING;
                }
            }
            // Si estamos en la fase NO_SOLUTION y se presiona una tecla
            else if (current_phase == NO_SOLUTION)
            {
                // Si se presiona R, regresar al menú principal
                if (event.keyboard.keycode == ALLEGRO_KEY_R)
                {
                    // Cambiamos la fase de vuelta a entrada de teclado
                    current_phase = KEYBOARD_INPUT;
                    // Limpiamos variables
                    solution_path.clear();
                    current_solution_step = 0;
                    time_since_last_step = 0;
                    boat_animation_progress = 0.0f;
                    simulation_speed = 1.0f;
                }
            }
        }

        // Si la variable redibujar es verdadera y no hay ningún evento a procesar (la cola de eventos esta vacía)
        if (redraw && al_is_event_queue_empty(event_queue))
        {
            // Limpiamos la pantalla con el color negro (nuestro fondo)
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Si al limpiar la pantalla estamos en la primera fase
            if (current_phase == KEYBOARD_INPUT)
            {
                // Creamos variables stringstream, ver https://shorturl.at/V98Mm
                stringstream m_text, c_text;
                // Usando estas variables, guardamos el mensaje junto con su valor
                m_text << "Missionaries: " << num_missionaries_input;
                c_text << "Cannibals: " << num_cannibals_input;

                // Imprimimos el mensaje que va a estar arriba, esta función tiene los siguientes parámetros:
                // - Fuente a usar
                // - Color a usar
                // - Posición X
                // - Posición Y
                // - ALLEGRO_ALIGN_CENTRE esta definido en la librería <allegro5/allegro_font.h>, por lo que en realidad vale 1, y esto ayuda que el texto
                // este centrado en esas coordenadas
                // - Texto que se va a mostrar
                al_draw_text(font, al_map_rgb(255, 255, 255), screen_width / 2, screen_height / 2 - 60, ALLEGRO_ALIGN_CENTRE, "Set Initial Numbers");

                // Declaramos variables de tipo color para las opciones
                ALLEGRO_COLOR m_color, c_color;
                // Si el campo seleccionado es el primero, entonces este va a ser de color amarillo
                if (selected_input_field == 0)
                {
                    m_color = al_map_rgb(255, 255, 0);
                    c_color = al_map_rgb(255, 255, 255);
                }
                else
                {
                    m_color = al_map_rgb(255, 255, 255);
                    c_color = al_map_rgb(255, 255, 0);
                }

                // Imprimimos las opciones en pantalla, obteniendo la cadena de m_text usando .str() y su apuntador como constante, esto ya que la función
                // al_draw_text utiliza el texto como char const *text (checa definición de la función)
                al_draw_text(font, m_color, screen_width / 2, screen_height / 2 - 20, ALLEGRO_ALIGN_CENTRE, m_text.str().c_str());
                al_draw_text(font, c_color, screen_width / 2, screen_height / 2 + 20, ALLEGRO_ALIGN_CENTRE, c_text.str().c_str());

                // Imprimimos mensajes de información un poco mas abajo de las opciones
                al_draw_text(font, al_map_rgb(180, 180, 180), screen_width / 2, screen_height / 2 + 60, ALLEGRO_ALIGN_CENTRE, "Use UP/DOWN to change, LEFT/RIGHT to select");
                al_draw_text(font, al_map_rgb(180, 180, 180), screen_width / 2, screen_height / 2 + 80, ALLEGRO_ALIGN_CENTRE, "Press ENTER to Start Simulation");
            }
            // Si estamos en alguna de las fases SOLVING o SOLVED, es decir, mostrando la animación o el resultado final
            else if (current_phase == SOLVING || current_phase == SOLVED)
            {
                // Primero, verificamos si hay una solución cargada y si el paso actual es válido dentro de esa solución
                if (!solution_path.empty() && current_solution_step < solution_path.size())
                {
                    // Obtenemos el estado que se va a dibujar del vector de la solución
                    State s_to_draw = solution_path[current_solution_step];
                    // Inicializamos un puntero al estado previo como nulo por defecto
                    const State *param_prev_s = nullptr;
                    // El progreso de la animación para draw_state será el progreso actual del bote
                    float param_anim = boat_animation_progress;

                    // Si estamos activamente resolviendo (animando) y no es el primer paso de la solución
                    if (current_phase == SOLVING && current_solution_step > 0)
                    {
                        // Solo consideramos el estado previo si el bote está efectivamente en tránsito,
                        // es decir, el progreso de la animación es mayor a 0 y menor a 1
                        if (boat_animation_progress > 0.0f && boat_animation_progress < 1.0f)
                        {
                            // Asignamos la dirección del estado anterior para la interpolación del dibujo
                            param_prev_s = &solution_path[current_solution_step - 1];
                        }
                    }
                    // Si la fase es SOLVED, significa que la animación ha terminado
                    else if (current_phase == SOLVED)
                    {
                        // Forzamos el parámetro de animación a 0 para asegurar que el bote se dibuje parado
                        param_anim = 0.0f;
                    }

                    // Llamamos a la función principal de dibujo con el estado actual, el previo (si existe),
                    // los números iniciales, la fuente y el progreso de la animación del bote
                    draw_state(s_to_draw, param_prev_s, num_missionaries_input, num_cannibals_input, font, param_anim);
                }
                // Si la condición anterior no se cumple, pero estamos en SOLVED y hay solución,
                // esto cubre el caso de dibujar el estado final estáticamente después de que la animación ha concluido,
                // o si por alguna razón current_solution_step no fuera menor a solution_path.size(), pero sí es el último estado
                else if (current_phase == SOLVED && !solution_path.empty())
                {
                    // Dibujamos el último estado de la solución de forma estática, sin animación del bote
                    draw_state(solution_path.back(), nullptr, num_missionaries_input, num_cannibals_input, font, 0.0f);
                    // Mostramos el mensaje de "SOLUTION FOUND!"
                    al_draw_text(font, al_map_rgb(0, 255, 0), screen_width / 2, screen_height / 2 - 20, ALLEGRO_ALIGN_CENTRE, "SOLUTION FOUND!");
                }

                // Preparamos y mostramos el mensaje de control de velocidad de la simulación
                stringstream speed_msg;
                speed_msg << "Speed: " << simulation_speed << "x (UP/DOWN to change)";
                al_draw_text(font, al_map_rgb(200, 200, 200), 10, 10, ALLEGRO_ALIGN_LEFT, speed_msg.str().c_str());

                // Mostramos los controles disponibles
                al_draw_text(font, al_map_rgb(180, 180, 180), 10, 30, ALLEGRO_ALIGN_LEFT, "R - Back to menu | SPACE - Restart simulation");

                // Si la simulación ha sido completada y estamos en la fase SOLVED
                if (current_phase == SOLVED)
                {
                    // Mostramos un mensaje indicando que la simulación está completa y cómo salir
                    al_draw_text(font, al_map_rgb(0, 255, 0), screen_width / 2, screen_height - 60, ALLEGRO_ALIGN_CENTRE, "Simulation Complete. Press ESC to exit");
                    al_draw_text(font, al_map_rgb(180, 180, 180), screen_width / 2, screen_height - 40, ALLEGRO_ALIGN_CENTRE, "Results saved in results.txt");
                    al_draw_text(font, al_map_rgb(255, 255, 0), screen_width / 2, screen_height - 20, ALLEGRO_ALIGN_CENTRE, "R - Menu | SPACE - Restart | ESC - Exit");
                }
            }
            // Si estamos en esta fase ya que no se encontró solucion al problema
            else if (current_phase == NO_SOLUTION)
            {
                // Simplemente mostramos mensajes informativos
                al_draw_text(font, al_map_rgb(255, 0, 0), screen_width / 2, screen_height / 2, ALLEGRO_ALIGN_CENTRE, "NO SOLUTION FOUND for these values.");
                al_draw_text(font, al_map_rgb(180, 180, 180), screen_width / 2, screen_height - 40, ALLEGRO_ALIGN_CENTRE, "Press ESC to exit | R - Menu ");
            }

            // Esta función actualiza la pantalla tras los cambios realizados
            al_flip_display();
            // Al haber hecho una actualización en pantalla, no volvemos dibujar
            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_shutdown_primitives_addon();
    al_shutdown_font_addon();

    return 0;
}
