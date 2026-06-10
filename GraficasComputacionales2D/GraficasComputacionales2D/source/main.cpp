#include <Prerequisites.h>
#include "Core/Window.h"
#include "Core/CShape.h"

/**
 * @file main.cpp
 * @brief Punto de entrada de la aplicación.
 */

/**
 * @brief Ventana principal de la aplicación.
 *
 * Se almacena mediante un puntero inteligente para garantizar
 * la liberación automática de recursos.
 */
std::unique_ptr<Window> g_window;

/**
 * @brief Figura utilizada durante la demostración.
 *
 * Se inicializa como un círculo y posteriormente se modifica
 * su color antes de comenzar el bucle principal.
 */
CShape Circle(ShapeType::CIRCLE);

/**
 * @brief Libera los recursos globales de la aplicación.
 *
 * Reinicia el puntero inteligente asociado a la ventana,
 * provocando la destrucción de la misma.
 */
void destroy() {
  g_window.reset();
}

/**
 * @brief Punto de entrada de la aplicación.
 *
 * Crea una ventana de renderizado, configura una figura
 * geométrica y ejecuta el bucle principal del programa.
 *
 * Durante cada iteración:
 * - Se procesan los eventos de la ventana.
 * - Se limpia la pantalla.
 * - Se renderizan los elementos de la escena.
 * - Se presenta el frame resultante.
 *
 * @return Código de finalización del programa.
 */
int main()
{
  // =========================================================
  // Inicialización
  // =========================================================

  /// Creación de la ventana principal.
  g_window =
    std::make_unique<Window>(
      800,
      600,
      "My window"
    );

  /// Configuración del color del círculo.
  Circle.getShape()->setFillColor(
    sf::Color(100, 250, 50)
  );

  // =========================================================
  // Bucle principal
  // =========================================================

  while (g_window->isOpen())
  {
    // -------------------------------------------------------
    // Procesamiento de eventos
    // -------------------------------------------------------

    while (
      const std::optional event =
      g_window->m_window->pollEvent()
      )
    {
      // Cierre solicitado por el usuario.
      if (event->is<sf::Event::Closed>())
        g_window->close();
    }

    // -------------------------------------------------------
    // Renderizado
    // -------------------------------------------------------

    /// Limpia la pantalla con color negro.
    g_window->clear(sf::Color::Black);

    /// Dibuja la figura de prueba.
    Circle.draw(*g_window);

    /// Presenta el frame en pantalla.
    g_window->display();
  }

  // =========================================================
  // Liberación de recursos
  // =========================================================

  destroy();

  return 0;
}