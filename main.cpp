// Librerías de allegro, ver documentación https://liballeg.org/a5docs/5.2.10.1/ cspell: disable
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

using namespace std;

// Dimensiones de la pantalla
const int anchoPantalla = 800;
const int altoPantalla = 600;

// Estados del juego
enum FaseActual
{
    ENTRADA_TECLADO,
    RESOLVIENDO,
    RESUELTO,
    SIN_SOLUCION
};

// Estructura para representar el estado del problema
struct Estado
{
    // Misioneros y caníbales en izquierda
    int m_izquierda, c_izquierda;
    // Misioneros y caníbales en la derecha
    int m_derecha, c_derecha;
    // Declaramos la posición del bote con 0 para la izquierda y 1 para la derecha
    int bote_pos;

    /*
    Dado que vamos a utilizar esta estructura en un set, necesitamos decirle como es que los debe de ordenar y
    si la estructura esta repetida
    */

    // Sobrecargamos el operador ==, comparando con la otra estructura (estado2) y sus miembros, se utiliza
    // el segundo const para asegurarnos de que en ningún momento se modifique algún valor (ya que solo estamos comparando),
    // ver https://shorturl.at/73FC6
    bool operator==(const Estado &estado2) const
    {
        // Si en algún momento un miembro es distinto, se devolverá false y obviamente esto significara que
        // las 2 estructuras no son iguales
        return m_izquierda == estado2.m_izquierda && c_izquierda == estado2.c_izquierda &&
               m_derecha == estado2.m_derecha && c_derecha == estado2.c_derecha &&
               bote_pos == estado2.bote_pos;
    }

    // Volvemos a hacer la sobrecarga pero ahora para el operador <, para que set sepa cual estructura va antes que la otra
    bool operator<(const Estado &estado2) const
    {
        // La comparación se hace con cada uno de los miembros de ambas estructuras
        if (m_izquierda != estado2.m_izquierda)
            return m_izquierda < estado2.m_izquierda;
        if (c_izquierda != estado2.c_izquierda)
            return c_izquierda < estado2.c_izquierda;
        if (m_derecha != estado2.m_derecha)
            return m_derecha < estado2.m_derecha;
        if (c_derecha != estado2.c_derecha)
            return c_derecha < estado2.c_derecha;
        return bote_pos < estado2.bote_pos;
    }
};

// La capacidad del bote para el problema es de 2
const int CAPACIDAD_BOTE = 2;

// Esta es la verificación mas importante, ya que a medida que vamos resolviendo el problema se utilizara para ver si
// vamos por buen camino o si se ha cometido algún error
bool es_valido(const Estado &s, int total_m, int total_c)
{
    // Comprueba números negativos
    if (s.m_izquierda < 0 || s.c_izquierda < 0 || s.m_derecha < 0 || s.c_derecha < 0)
        return false;

    // Comprueba si los misioneros son superados en número en la orilla izquierda
    if (s.m_izquierda > 0 && s.m_izquierda < s.c_izquierda)
        return false;
    // Comprueba si los misioneros son superados en número en la orilla derecha
    if (s.m_derecha > 0 && s.m_derecha < s.c_derecha)
        return false;

    // Si ninguna de estas condiciones es verdadera, significa que el estado actual
    // del problema es valido
    return true;
}

// Función simple para comprobar si se ha resuelto el problema
bool es_solucion(const Estado &s, int total_m, int total_c)
{
    // Donde los misioneros, caníbales y el bote estén a la izquierda
    return s.m_izquierda == 0 && s.c_izquierda == 0 && s.m_derecha == total_m && s.c_derecha == total_c && s.bote_pos == 1;
}

// Función recursiva usando DFS
bool resolver_mc_recursivo(Estado actual_s, int total_m, int total_c, vector<Estado> &camino, set<Estado> &visitados)
{
    // Lo primero que se hace es agregar al final de nuestro vector el estado actual
    camino.push_back(actual_s);
    // Agregamos el estado a nuestro set de estados visitados (recordar que no permite estructuras permitidas)
    visitados.insert(actual_s);

    // Verificamos si el estado actual es la solución al problema (caso base)
    if (es_solucion(actual_s, total_m, total_c))
    {
        // Nuestra función devuelve verdadero
        return true;
    }

    /* Exploramos los movimientos posibles*/

    // Iteramos sobre el numero de misioneros que podrían ir en el bote (de 0 a CAPACIDAD_BOTE, que en este caso es 2)
    for (int m_bote = 0; m_bote <= CAPACIDAD_BOTE; ++m_bote)
    {
        // Nuevamente iteramos sobre el numero de caníbales que podrían ir en el bote, haciendo una validación sobre el numero de
        // caníbales que hay en el (c_bote <= CAPACIDAD_BOTE - m_bote), esto para asegurarnos de que no nos pasemos de la capacidad
        // del bote
        for (int c_bote = 0; c_bote <= CAPACIDAD_BOTE - m_bote; ++c_bote)
        {

            /*
            Utilizamos los 2 for anidados para obtener todas las combinaciones posibles de misioneros y caníbales que pueden ir en el bote
            respetando su capacidad
            */

            // Si tenemos el caso de que el bote este vació, simplemente ignoramos este y seguimos con la siguiente iteración
            if (m_bote + c_bote == 0)
            {
                continue;
            }

            // Guardamos el estado actual del problema
            Estado siguiente_s = actual_s;

            // Si el bote se encuentra del lado izquierdo
            if (actual_s.bote_pos == 0)
            {
                // Verificamos si el numero de misioneros y caníbales que vamos a probar (según los 2 for anidados) están disponibles
                // del lado izquierdo
                if (actual_s.m_izquierda >= m_bote && actual_s.c_izquierda >= c_bote)
                {
                    // De ser asi, entonces quitamos los misioneros y caníbales de la izquierda
                    siguiente_s.m_izquierda -= m_bote;
                    siguiente_s.c_izquierda -= c_bote;
                    // Y agregamos esta misma cantidad al lado derecho, simulando que paso el bote
                    siguiente_s.m_derecha += m_bote;
                    siguiente_s.c_derecha += c_bote;
                    // Cambiamos la posición del bote
                    siguiente_s.bote_pos = 1;
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
                if (actual_s.m_derecha >= m_bote && actual_s.c_derecha >= c_bote)
                {
                    // Quitamos los misioneros y caníbales del lado derecho
                    siguiente_s.m_derecha -= m_bote;
                    siguiente_s.c_derecha -= c_bote;
                    // Y los agregamos al otro lado
                    siguiente_s.m_izquierda += m_bote;
                    siguiente_s.c_izquierda += c_bote;
                    // Cambiamos la posición del bote
                    siguiente_s.bote_pos = 0;
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
            if (es_valido(siguiente_s, total_m, total_c) && visitados.find(siguiente_s) == visitados.end())
            {
                // Si entramos al if es porque el estado actual del problema es valido y este no se encontró en el set, por lo que podemos seguir investigando las posibilidades
                // de este estado (DFS) esperando encontrar la solución
                if (resolver_mc_recursivo(siguiente_s, total_m, total_c, camino, visitados))
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
    camino.pop_back();
    // Dado que no se resolvió y ni se encontró posibles caminos a soluciones, devolvemos false
    return false;
}

// Esta es la función principal la cual se va a encargar de solucionar el problema (si es que hay solución de este)
bool resolver_mc(int m_inicial, int c_inicial, vector<Estado> &camino)
{
    // Declaramos un set que trabaja con la estructura Estado
    set<Estado> visitados;
    // Creamos una estructura inicial con el numero de misioneros y caníbales a trabajar (todos del lado izquierdo) y aparte
    // la posición del bote que esta del lado izquierdo
    Estado estado_inicial = {m_inicial, c_inicial, 0, 0, 0};

    // Antes de empezar a resolverlo, debemos de verificar si el problema se puede resolver con estos datos, para este momento,
    // simplemente se hace para ver si los misioneros pierden en numero contra los caníbales (lo cual no es un estado inicial
    // valido)
    if (!es_valido(estado_inicial, m_inicial, c_inicial))
        // De ser asi entonces ni siquiera podemos resolver el problema
        return false;

    // Si el problema es valido desde el inicio, entonces ya podemos resolver el problema
    return resolver_mc_recursivo(estado_inicial, m_inicial, c_inicial, camino, visitados);
}

// Función para dibujar una persona individualmente, ya sea misionero o caníbal, en una posición específica y con un color dado
void dibujar_persona(float x, float y, ALLEGRO_COLOR color, bool es_misionero)
{
    // Dibuja un círculo relleno que representa el cuerpo de la persona
    al_draw_filled_circle(x, y, 10, color); // Cuerpo de la persona, un círculo de radio 10
    // Si la persona a dibujar es un misionero
    if (es_misionero)
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
void dibujar_estado(const Estado &s_actual, const Estado *ptr_s_previo, int total_m_inicial, int total_c_inicial, ALLEGRO_FONT *fuente, float progreso_anim_bote)
{
    // Definimos los colores que se utilizarán para los distintos elementos de la simulación
    ALLEGRO_COLOR color_misionero = al_map_rgb(0, 0, 255); // Azul para los Misioneros
    ALLEGRO_COLOR color_canibal = al_map_rgb(255, 0, 0);   // Rojo para los Caníbales
    ALLEGRO_COLOR color_bote = al_map_rgb(139, 69, 19);    // Marrón para el Bote
    ALLEGRO_COLOR color_agua = al_map_rgb(0, 105, 148);    // Azul oscuro para el Agua del río
    ALLEGRO_COLOR color_orilla = al_map_rgb(34, 139, 34);  // Verde para las Orillas

    // Definimos las dimensiones y posiciones relativas de las orillas y el río en la pantalla
    float ancho_orilla = anchoPantalla * 0.25f; // Cada orilla ocupa un 25% del ancho de la pantalla
    float ancho_rio = anchoPantalla * 0.5f;     // El río ocupa el 50% central del ancho de la pantalla
    float inicio_y_orilla = altoPantalla * 0.4f; // Posición Y donde comienzan a dibujarse las orillas y el río
    float alto_orilla = altoPantalla * 0.5f;    // Altura de las orillas y el río

    // Inicializamos los contadores de misioneros y caníbales en cada orilla basándonos en el estado actual
    int m_en_orilla_izquierda = s_actual.m_izquierda; 
    int c_en_orilla_izquierda = s_actual.c_izquierda; 
    int m_en_orilla_derecha = s_actual.m_derecha;     
    int c_en_orilla_derecha = s_actual.c_derecha;   

    // Inicializamos los contadores de misioneros y caníbales en el bote a cero, se actualizarán si el bote está en tránsito
    int m_en_bote = 0;
    int c_en_bote = 0;

    // Definimos las dimensiones del bote
    float bote_ancho = 80; 
    float bote_alto = 40; 
    // Calculamos la posición Y del bote, centrado verticalmente en la zona media del río/orillas
    float bote_y = inicio_y_orilla + alto_orilla * 0.3f;
    // Calculamos la posición X del bote cuando está completamente en el muelle izquierdo
    float bote_muelle_izquierdo_x = ancho_orilla - bote_ancho / 2;
    // Calculamos la posición X del bote cuando está completamente en el muelle derecho
    float bote_muelle_derecho_x = ancho_orilla + ancho_rio - bote_ancho / 2;

    // Variable para almacenar la posición X actual del bote para el dibujo, se calculará según la animación
    float bote_dibujo_actual_x;

    // Determinamos la posición X visual del bote para la animación
    // Si la posición objetivo del bote en el estado actual (s_actual.bote_pos) es la derecha (1)
    if (s_actual.bote_pos == 1)
    {
        // El bote se mueve de izquierda a derecha, o ya está a la derecha
        // Interpolamos la posición X del bote desde el muelle izquierdo hacia el muelle derecho
        // basado en el progreso de la animación (progreso_anim_bote)
        bote_dibujo_actual_x = bote_muelle_izquierdo_x + (bote_muelle_derecho_x - bote_muelle_izquierdo_x) * progreso_anim_bote;
    }
    // Si la posición objetivo del bote es la izquierda (0)
    else 
    {
        // El bote se mueve de derecha a izquierda, o ya está a la izquierda
        // Interpolamos la posición X del bote desde el muelle derecho hacia el muelle izquierdo
        // basado en el progreso de la animación (progreso_anim_bote)
        bote_dibujo_actual_x = bote_muelle_derecho_x - (bote_muelle_derecho_x - bote_muelle_izquierdo_x) * progreso_anim_bote;
    }

    // Lógica para determinar quiénes están en el bote y ajustar los conteos en las orillas si el bote está en tránsito
    // Se activa si existe un estado previo (ptr_s_previo no es nulo) y la animación del bote está en curso (entre 0 y 1)
    if (ptr_s_previo && progreso_anim_bote > 0.0f && progreso_anim_bote < 1.0f)
    {
        // El bote está actualmente moviéndose entre orillas
        const Estado &s_previo = *ptr_s_previo; // Referencia al estado anterior para comparaciones

        // Si el bote en el estado previo estaba en la orilla izquierda (0)
        if (s_previo.bote_pos == 0)
        {
            // El bote se estaba moviendo de Izquierda a Derecha (s_actual.bote_pos debería ser 1)
            // Calculamos cuántos misioneros estaban en el bote restando los actuales en la orilla izq. de los que había antes
            m_en_bote = s_previo.m_izquierda - s_actual.m_izquierda;
            // Calculamos cuántos caníbales estaban en el bote de forma similar
            c_en_bote = s_previo.c_izquierda - s_actual.c_izquierda;

            // Los que quedan en la orilla izquierda son los de s_actual (ya se restaron los del bote)
            m_en_orilla_izquierda = s_actual.m_izquierda;
            c_en_orilla_izquierda = s_actual.c_izquierda;
            // Los que están en la orilla derecha son los que ya estaban allí en el estado previo (aún no han llegado los del bote)
            m_en_orilla_derecha = s_previo.m_derecha;
            c_en_orilla_derecha = s_previo.c_derecha;
        }
        // Si el bote en el estado previo estaba en la orilla derecha (1)
        else 
        {
            // El bote se estaba moviendo de Derecha a Izquierda (s_actual.bote_pos debería ser 0)
            // Calculamos cuántos misioneros estaban en el bote restando los actuales en la orilla der. de los que había antes
            m_en_bote = s_previo.m_derecha - s_actual.m_derecha;
            // Calculamos cuántos caníbales estaban en el bote de forma similar
            c_en_bote = s_previo.c_derecha - s_actual.c_derecha;

            // Los que quedan en la orilla derecha son los de s_actual
            m_en_orilla_derecha = s_actual.m_derecha;
            c_en_orilla_derecha = s_actual.c_derecha;
            // Los que están en la orilla izquierda son los que ya estaban allí en el estado previo
            m_en_orilla_izquierda = s_previo.m_izquierda;
            c_en_orilla_izquierda = s_previo.c_izquierda;
        }

        // Aseguramos que los conteos en el bote no sean negativos
        m_en_bote = max(0, m_en_bote);
        c_en_bote = max(0, c_en_bote);
    }

    // Si el bote no está en tránsito (está atracado o no hay estado previo para la animación),
    // los conteos en las orillas ya son los de s_actual y los del bote permanecen en 0 (como se inicializaron)

    // --- Comienza el dibujo de los elementos ---

    // Dibujar la orilla izquierda como un rectángulo relleno
    al_draw_filled_rectangle(0, inicio_y_orilla, ancho_orilla, inicio_y_orilla + alto_orilla, color_orilla);
    // Dibujar la orilla derecha como un rectángulo relleno
    al_draw_filled_rectangle(ancho_orilla + ancho_rio, inicio_y_orilla, anchoPantalla, inicio_y_orilla + alto_orilla, color_orilla);

    // Dibujar el río como un rectángulo relleno entre las dos orillas
    al_draw_filled_rectangle(ancho_orilla, inicio_y_orilla, ancho_orilla + ancho_rio, inicio_y_orilla + alto_orilla, color_agua);

    // Dibujar las personas en la orilla izquierda
    float persona_x_inicio_izquierda = ancho_orilla * 0.1f; // Posición X inicial para la primera persona en la orilla izquierda
    float persona_y_inicio = inicio_y_orilla + 30;         // Posición Y inicial para la fila de misioneros
    float espaciado_persona = 25;                          // Espacio horizontal entre cada persona

    // Bucle para dibujar cada misionero en la orilla izquierda
    for (int i = 0; i < m_en_orilla_izquierda; ++i)
    {
        dibujar_persona(persona_x_inicio_izquierda + i * espaciado_persona, persona_y_inicio, color_misionero, true);
    }
    // Bucle para dibujar cada caníbal en la orilla izquierda, en una fila debajo de los misioneros
    for (int i = 0; i < c_en_orilla_izquierda; ++i)
    {
        dibujar_persona(persona_x_inicio_izquierda + i * espaciado_persona, persona_y_inicio + 40, color_canibal, false);
    }

    // Dibujar las personas en la orilla derecha
    float persona_x_inicio_derecha = ancho_orilla + ancho_rio + ancho_orilla * 0.1f; // Posición X inicial para la primera persona en la orilla derecha
    // Bucle para dibujar cada misionero en la orilla derecha
    for (int i = 0; i < m_en_orilla_derecha; ++i)
    {
        dibujar_persona(persona_x_inicio_derecha + i * espaciado_persona, persona_y_inicio, color_misionero, true);
    }
    // Bucle para dibujar cada caníbal en la orilla derecha, en una fila debajo de los misioneros
    for (int i = 0; i < c_en_orilla_derecha; ++i)
    {
        dibujar_persona(persona_x_inicio_derecha + i * espaciado_persona, persona_y_inicio + 40, color_canibal, false);
    }

    // Dibujar el Bote en su posición X actual calculada (bote_dibujo_actual_x)
    al_draw_filled_rectangle(bote_dibujo_actual_x, bote_y, bote_dibujo_actual_x + bote_ancho, bote_y + bote_alto, color_bote);
    // Dibujar un contorno negro alrededor del bote para mejor visibilidad
    al_draw_rectangle(bote_dibujo_actual_x, bote_y, bote_dibujo_actual_x + bote_ancho, bote_y + bote_alto, al_map_rgb(0, 0, 0), 2);

    // Dibujar las personas que están actualmente en el bote
    if (m_en_bote > 0 || c_en_bote > 0)
    {
        // Calculamos el desplazamiento Y para centrar las personas verticalmente en el bote
        float bote_persona_y_offset = bote_alto / 2;
        // Calculamos el número total de personas en el bote para distribuir el espacio
        int total_en_bote = m_en_bote + c_en_bote;
        // Calculamos el espaciado horizontal entre personas dentro del bote
        float bote_persona_x_espaciado = bote_ancho / (total_en_bote + 1);
        int persona_idx_en_bote = 0; // Índice para posicionar secuencialmente a las personas en el bote
        // Bucle para dibujar a los misioneros en el bote
        for (int i = 0; i < m_en_bote; ++i)
        {
            dibujar_persona(bote_dibujo_actual_x + bote_persona_x_espaciado * (persona_idx_en_bote + 1), bote_y + bote_persona_y_offset, color_misionero, true);
            persona_idx_en_bote++; // Incrementar índice para la siguiente persona
        }
        // Bucle para dibujar a los caníbales en el bote
        for (int i = 0; i < c_en_bote; ++i)
        {
            dibujar_persona(bote_dibujo_actual_x + bote_persona_x_espaciado * (persona_idx_en_bote + 1), bote_y + bote_persona_y_offset, color_canibal, false);
            persona_idx_en_bote++; // Incrementar índice para la siguiente persona
        }
    }

    // Mostrar los conteos de misioneros y caníbales en cada orilla usando texto
    stringstream texto_orilla_izquierda, texto_orilla_derecha; // Stringstreams para formatear el texto de los conteos
    // Formatear texto para la orilla izquierda usando los valores de s_actual (reflejan el estado final del movimiento si está atracado)
    texto_orilla_izquierda << "Orilla Izq: M=" << s_actual.m_izquierda << ", C=" << s_actual.c_izquierda;
    // Formatear texto para la orilla derecha usando los valores de s_actual
    texto_orilla_derecha << "Orilla Der: M=" << s_actual.m_derecha << ", C=" << s_actual.c_derecha;
    // El contenido del bote se dibuja visualmente, el texto del bote podría ser redundante o complejo si se muestra siempre

    // Dibujar el texto del conteo para la orilla izquierda, centrado sobre la orilla
    al_draw_text(fuente, al_map_rgb(255, 255, 255), ancho_orilla / 2, inicio_y_orilla - 60, ALLEGRO_ALIGN_CENTRE, texto_orilla_izquierda.str().c_str());
    // Dibujar el texto del conteo para la orilla derecha, centrado sobre la orilla
    al_draw_text(fuente, al_map_rgb(255, 255, 255), ancho_orilla + ancho_rio + ancho_orilla / 2, inicio_y_orilla - 60, ALLEGRO_ALIGN_CENTRE, texto_orilla_derecha.str().c_str());

    // Mostrar los conteos totales iniciales de misioneros y caníbales como referencia en la esquina superior derecha
    stringstream texto_conteos_totales;
    texto_conteos_totales << "Total M: " << total_m_inicial << ", Total C: " << total_c_inicial;
    al_draw_text(fuente, al_map_rgb(200, 200, 200), anchoPantalla - 10, 10, ALLEGRO_ALIGN_RIGHT, texto_conteos_totales.str().c_str());
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
    // Iniciamos
    al_init_font_addon(); // Inicializar addon de fuentes

    // Creamos un temporizador que se va a activa cada (1.0 / 60.0) veces, de esta manera obteniendo una taza de refresco de 60hz
    ALLEGRO_TIMER *temporizador = al_create_timer(1.0 / 60.0);
    // Creamos una cola de eventos, esto se hace para manejar input dado por el usuario, ver https://shorturl.at/1pUjt
    ALLEGRO_EVENT_QUEUE *cola_eventos = al_create_event_queue();
    // Creamos nuestra pantalla pasándole el alto y ancho definido en las constantes
    ALLEGRO_DISPLAY *pantalla = al_create_display(anchoPantalla, altoPantalla);
    // Para simplicidad del proyecto, trabajamos con la fuente base de allegro: "Creates a monochrome bitmap font (8x8 pixels per character)." según
    // la documentación oficial
    ALLEGRO_FONT *fuente = al_create_builtin_font();

    // Registramos el "event source" en nuestra cola de eventos para los siguientes eventos:
    // - El teclado
    // - La pantalla
    // - El temporizador
    al_register_event_source(cola_eventos, al_get_keyboard_event_source());
    al_register_event_source(cola_eventos, al_get_display_event_source(pantalla));
    al_register_event_source(cola_eventos, al_get_timer_event_source(temporizador));

    // Creamos un evento, el cual se utilizara para "esperar" o "recibir" uno según los que hemos especificado
    // en nuestra cola de eventos
    ALLEGRO_EVENT evento;

    // Creamos un enum para mantener un control del ciclo de la simulación (dentro de las variables predefinidas dadas), dado que acabamos de iniciar, l
    // a fase actual es 0 (dado por ENTRADA_TECLADO)
    FaseActual fase_actual = ENTRADA_TECLADO;

    // Creamos nuestras variables para guardar el input ingresado por el usuario (iniciamos con 3 misioneros y 3 caníbales)
    int num_misioneros_input = 3;
    int num_canibales_input = 3;
    // Para saber en que campo estamos durante el menu de selección, marcamos 2 estados:
    // - 0 seleccionando el campo misioneros
    // - 1 seleccionando el campo caníbales
    int campo_input_seleccionado = 0;

    // Marcamos la velocidad inicial en 1
    float velocidad_simulacion = 1.0f;

    // Declaramos un vector que va a guardar estructuras de tipo Estado
    vector<Estado> camino_solucion;
    // Este va a ser nuestro indice para el vector
    int paso_solucion_actual = 0;
    // Simple variable para almacenar la duración del frame
    float tiempo_desde_ultimo_paso = 0.0f;

    // Esta variable nos ayuda a "animar" el bote durante la resolución, donde es 0.0 al inicio del movimiento
    // y 1.0 al finalizarlo
    float progreso_animacion_bote = 0.0f;

    // Antes de iniciar todo el ciclo de la simulación, iniciamos nuestro timer
    al_start_timer(temporizador);

    // Esta variable nos indica si debemos de hacer una "actualización" (un fotograma) de nuestra pantalla
    bool redibujar;

    // Ciclo infinito para nuestra simulación
    while (1)
    {
        // Aquí estaremos esperando unicamente por los event sources especificados anteriormente en la función
        // al_register_event_source(), ver https://shorturl.at/KBKRk entender como funciona esta función tan importante
        al_wait_for_event(cola_eventos, &evento);

        // Si el evento a procesar el el timer, el cual sera cierto si es que no hay ningún input por parte del usuario, (hay que recordar que el
        // timer da una señal cada 1.0 / 60.0s) entonces ingresamos a este if
        if (evento.type == ALLEGRO_EVENT_TIMER)
        {
            // Tras cada frame, se dibujara nuevamente
            redibujar = true;

            // Si ya estamos mostrando la solución, significa que el vector no esta vacio, ya que la función resolver_mc lo lleno
            // con toda la secuencia de estados para llegar a la solución
            if (fase_actual == RESOLVIENDO && !camino_solucion.empty())
            {
                // Guardamos el tiempo pasado en un frame, acumulándolo para controlar la duración del paso actual de la animación
                tiempo_desde_ultimo_paso += (1.0 / 60.0);
                // Calculamos el progreso de la animación del bote como una fracción de la duración total del paso,
                // esto depende del tiempo acumulado y la velocidad de simulación elegida por el usuario
                progreso_animacion_bote = tiempo_desde_ultimo_paso * velocidad_simulacion;
                // Aseguramos que el progreso no exceda 1, significando movimiento finalizado
                progreso_animacion_bote = min(1.0f, progreso_animacion_bote);

                // Si el tiempo acumulado es mayor o igual a la duración esperada para un paso de la animación,
                // significa que la animación de este paso ha concluido
                if (tiempo_desde_ultimo_paso >= (1.0 / velocidad_simulacion))
                {
                    // Avanzamos al siguiente paso de la solución, nuestro indice en el vector
                    paso_solucion_actual++;

                    // Si hemos superado el último paso de la solución
                    if (paso_solucion_actual >= camino_solucion.size())
                    {
                        // Cambiamos la fase a RESUELTO, indicando que la simulación ha terminado
                        fase_actual = RESUELTO;
                        // Mantenemos el índice en el último paso para mostrar el estado final
                        paso_solucion_actual = camino_solucion.size() - 1;
                        // Reiniciamos el progreso del bote para que se muestre estático en la orilla final
                        progreso_animacion_bote = 0.0f;
                    }
                    else
                    {
                        // Si aún hay pasos, reiniciamos el progreso del bote para la animación del nuevo paso
                        progreso_animacion_bote = 0.0f;
                    }
                    // Reseteamos el contador de tiempo para el nuevo paso de la animación
                    tiempo_desde_ultimo_paso = 0;
                }
            }
        }
        // Si el evento procesado es la "X" en nuestra ventana, entonces rompemos el ciclo y terminara el programa
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break;
        }
        // Si se pulsa un tecla (KEY_DOWN)
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            // Si la tecla presionada es el ESC, entonces rompemos el ciclo y terminara el programa
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                break;

            // Si al presionar una tecla seguimos estamos en la primera fase
            if (fase_actual == ENTRADA_TECLADO)
            {
                // Y la tecla presionada es flecha arriba
                if (evento.keyboard.keycode == ALLEGRO_KEY_UP)
                {
                    // Y el campo seleccionado es misioneros
                    if (campo_input_seleccionado == 0)
                        // Entonces aumentamos nuestro contador
                        num_misioneros_input++;
                    else
                        num_canibales_input++;
                }
                // Si la tecla presionada es flecha abajo
                else if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN)
                {
                    // Y el campo seleccionado es misioneros
                    if (campo_input_seleccionado == 0)
                        // Disminuimos nuestro contador, pero para que este nunca llegue a ser negativo, obtenemos el máximo entre 0 y
                        // el valor actual de la variable, para que en caso de que si la variable llega a ser negativa (el usuario disminuyo
                        // muchas veces) entonces max simplemente agarrara 0 y la variable valdrá eso
                        num_misioneros_input = max(0, num_misioneros_input - 1);
                    else
                        num_canibales_input = max(0, num_canibales_input - 1);
                }
                // Si la tecla seleccionada es la flecha derecha o izquierda
                else if (evento.keyboard.keycode == ALLEGRO_KEY_LEFT || evento.keyboard.keycode == ALLEGRO_KEY_RIGHT)
                {
                    // Cambiamos el input seleccionado (recordar que son valores entre 0 y 1)
                    campo_input_seleccionado = 1 - campo_input_seleccionado;
                }
                // Si la tecla seleccionada es enter
                else if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER)
                {
                    // Solo por buena practica, limpiamos nuestro vector antes de resolver el problema
                    camino_solucion.clear();
                    // Nuevamente por buena practica, limpiamos nuestras variables
                    paso_solucion_actual = 0;
                    tiempo_desde_ultimo_paso = 0;
                    progreso_animacion_bote = 0.0f;

                    // Llamamos a nuestra función para resolver el problema de misioneros y caníbales manando la información de este, asi como
                    // el vector que va a representar nuestra solución
                    if (resolver_mc(num_misioneros_input, num_canibales_input, camino_solucion))
                    {
                        // Dado que la situación inicial es valida, entonces cambiamos la fase
                        fase_actual = RESOLVIENDO;
                    }
                    // Si no hay una situación inicial valida, entonces cambiamos la fase
                    else
                    {
                        fase_actual = SIN_SOLUCION;
                    }
                }
            }
            // Si no estamos en la primera fase y se presiona una tecla
            else if (fase_actual == RESOLVIENDO || fase_actual == RESUELTO)
            {
                // Si esta es la tecla de flecha arriba
                if (evento.keyboard.keycode == ALLEGRO_KEY_UP)
                {
                    // Aumentamos la velocidad de simulación en un 0.5, utilizando min para tener un tope hasta 10
                    velocidad_simulacion = min(10.0f, velocidad_simulacion + 0.5f);
                }
                // Si la tecla es flecha abajo
                else if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN)
                {
                    // Disminuimos la velocidad en 0.5, utilizando max para tener un tope hasta 0.5
                    velocidad_simulacion = max(0.5f, velocidad_simulacion - 0.5f);
                }
            }
        }

        // Si la variable redibujar es verdadera y no hay ningún evento a procesar (la cola de eventos esta vacía)
        if (redibujar && al_is_event_queue_empty(cola_eventos))
        {
            // Limpiamos la pantalla con el color negro (nuestro fondo)
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Si al limpiar la pantalla estamos en la primera fase
            if (fase_actual == ENTRADA_TECLADO)
            {
                // Creamos variables stringstream, ver https://shorturl.at/V98Mm
                stringstream m_texto, c_texto;
                // Usando estas variables, guardamos el mensaje junto con su valor
                m_texto << "Misioneros: " << num_misioneros_input;
                c_texto << "Canibales: " << num_canibales_input;

                // Imprimimos el mensaje que va a estar arriba, esta función tiene los siguientes parámetros:
                // - Fuente a usar
                // - Color a usar
                // - Posición X
                // - Posición Y
                // - ALLEGRO_ALIGN_CENTRE esta definido en la librería <allegro5/allegro_font.h>, por lo que en realidad vale 1, y esto ayuda que el texto
                // este centrado en esas coordenadas
                // - Texto que se va a mostrar
                al_draw_text(fuente, al_map_rgb(255, 255, 255), anchoPantalla / 2, altoPantalla / 2 - 60, ALLEGRO_ALIGN_CENTRE, "Establecer Números Iniciales");

                // Declaramos variables de tipo color para las opciones
                ALLEGRO_COLOR m_color, c_color;
                // Si el campo seleccionado es el primero, entonces este va a ser de color amarillo
                if (campo_input_seleccionado == 0)
                {
                    m_color = al_map_rgb(255, 255, 0);
                    c_color = al_map_rgb(255, 255, 255);
                }
                else
                {
                    m_color = al_map_rgb(255, 255, 255);
                    c_color = al_map_rgb(255, 255, 0);
                }

                // Imprimimos las opciones en pantalla, obteniendo la cadena de m_texto usando .str() y su apuntador como constante, esto ya que la función
                // al_draw_text utiliza el texto como char const *text (checa definición de la función)
                al_draw_text(fuente, m_color, anchoPantalla / 2, altoPantalla / 2 - 20, ALLEGRO_ALIGN_CENTRE, m_texto.str().c_str());
                al_draw_text(fuente, c_color, anchoPantalla / 2, altoPantalla / 2 + 20, ALLEGRO_ALIGN_CENTRE, c_texto.str().c_str());

                // Imprimimos mensajes de información un poco mas abajo de las opciones
                al_draw_text(fuente, al_map_rgb(180, 180, 180), anchoPantalla / 2, altoPantalla / 2 + 60, ALLEGRO_ALIGN_CENTRE, "Usa ARRIBA/ABAJO para cambiar, IZQ/DER para seleccionar");
                al_draw_text(fuente, al_map_rgb(180, 180, 180), anchoPantalla / 2, altoPantalla / 2 + 80, ALLEGRO_ALIGN_CENTRE, "Presiona ENTER para Iniciar Simulación");
            }
            // Si estamos en alguna de las fases RESOLVIENDO o RESUELTO, es decir, mostrando la animación o el resultado final
            else if (fase_actual == RESOLVIENDO || fase_actual == RESUELTO)
            {
                // Primero, verificamos si hay una solución cargada y si el paso actual es válido dentro de esa solución
                if (!camino_solucion.empty() && paso_solucion_actual < camino_solucion.size())
                {
                    // Obtenemos el estado que se va a dibujar del vector de la solución
                    Estado s_a_dibujar = camino_solucion[paso_solucion_actual];
                    // Inicializamos un puntero al estado previo como nulo por defecto
                    const Estado *param_s_previo = nullptr;
                    // El progreso de la animación para dibujar_estado será el progreso actual del bote
                    float param_anim = progreso_animacion_bote;

                    // Si estamos activamente resolviendo (animando) y no es el primer paso de la solución
                    if (fase_actual == RESOLVIENDO && paso_solucion_actual > 0)
                    {
                        // Solo consideramos el estado previo si el bote está efectivamente en tránsito,
                        // es decir, el progreso de la animación es mayor a 0 y menor a 1
                        if (progreso_animacion_bote > 0.0f && progreso_animacion_bote < 1.0f)
                        {
                            // Asignamos la dirección del estado anterior para la interpolación del dibujo
                            param_s_previo = &camino_solucion[paso_solucion_actual - 1];
                        }
                    }
                    // Si la fase es RESUELTO, significa que la animación ha terminado
                    else if (fase_actual == RESUELTO)
                    {
                        // Forzamos el parámetro de animación a 0 para asegurar que el bote se dibuje parado
                        param_anim = 0.0f;
                    }

                    // Llamamos a la función principal de dibujo con el estado actual, el previo (si existe),
                    // los números iniciales, la fuente y el progreso de la animación del bote
                    dibujar_estado(s_a_dibujar, param_s_previo, num_misioneros_input, num_canibales_input, fuente, param_anim);
                }
                // Si la condición anterior no se cumple, pero estamos en RESUELTO y hay solución,
                // esto cubre el caso de dibujar el estado final estáticamente después de que la animación ha concluido,
                // o si por alguna razón paso_solucion_actual no fuera menor a camino_solucion.size(), pero sí es el último estado
                else if (fase_actual == RESUELTO && !camino_solucion.empty())
                {
                    // Dibujamos el último estado de la solución de forma estática, sin animación del bote
                    dibujar_estado(camino_solucion.back(), nullptr, num_misioneros_input, num_canibales_input, fuente, 0.0f);
                    // Mostramos el mensaje de "SOLUCION ENCONTRADA!"
                    al_draw_text(fuente, al_map_rgb(0, 255, 0), anchoPantalla / 2, altoPantalla / 2 - 20, ALLEGRO_ALIGN_CENTRE, "SOLUCION ENCONTRADA!");
                }

                // Preparamos y mostramos el mensaje de control de velocidad de la simulación
                stringstream msg_velocidad;
                msg_velocidad << "Velocidad: " << velocidad_simulacion << "x (ARRIBA/ABAJO para cambiar)";
                al_draw_text(fuente, al_map_rgb(200, 200, 200), 10, 10, ALLEGRO_ALIGN_LEFT, msg_velocidad.str().c_str());
                // Si la simulación ha sido completada y estamos en la fase RESUELTO
                if (fase_actual == RESUELTO)
                {
                    // Mostramos un mensaje indicando que la simulación está completa y cómo salir
                    al_draw_text(fuente, al_map_rgb(0, 255, 0), anchoPantalla / 2, altoPantalla - 40, ALLEGRO_ALIGN_CENTRE, "Simulacion Completa. Presiona ESC para salir");
                }
            }
            // Si estamos en esta fase ya que no se encontró solucion al problema
            else if (fase_actual == SIN_SOLUCION)
            {
                // Simplemente mostramos mensajes informativos
                al_draw_text(fuente, al_map_rgb(255, 0, 0), anchoPantalla / 2, altoPantalla / 2, ALLEGRO_ALIGN_CENTRE, "NO SE ENCONTRO SOLUCION para estos valores.");
                al_draw_text(fuente, al_map_rgb(180, 180, 180), anchoPantalla / 2, altoPantalla - 40, ALLEGRO_ALIGN_CENTRE, "Presiona ESC para salir.");
            }

            // Esta función actualiza la pantalla tras los cambios realizados
            al_flip_display();
            // Al haber hecho una actualización en pantalla, no volvemos dibujar
            redibujar = false;
        }
    }

    al_destroy_font(fuente);
    al_destroy_display(pantalla);
    al_destroy_timer(temporizador);
    al_destroy_event_queue(cola_eventos);
    al_shutdown_primitives_addon();
    al_shutdown_font_addon();

    return 0;
}