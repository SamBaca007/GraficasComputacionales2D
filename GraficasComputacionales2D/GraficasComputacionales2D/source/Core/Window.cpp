#include "Core/Window.h"

/**
 * @file Window.cpp
 * @brief Implementación de la clase Window.
 */

/**
 * @brief Construye una ventana de renderizado.
 *
 * Crea internamente una instancia de sf::RenderWindow utilizando
 * los parámetros especificados y configura un límite de 60 FPS.
 *
 * @param width Ancho de la ventana en píxeles.
 * @param height Alto de la ventana en píxeles.
 * @param title Título mostrado en la barra de la ventana.
 */
Window::Window(int width, int height, const std::string& title) {

  // Creación de la ventana principal.
  m_window = std::make_unique<sf::RenderWindow>(
    sf::VideoMode({
      static_cast<unsigned int>(width),
      static_cast<unsigned int>(height)
      }),
    title,
    sf::Style::Default
  );

  if (m_window) {
    m_window->setFramerateLimit(60);

    MESSAGE(
      "Window",
      "Window",
      "Window created successfully"
    );
  }
  else {
    ERROR(
      "Window",
      "Window",
      "Failed to create window"
    );
  }
}

/**
 * @brief Destructor por defecto.
 *
 * La liberación de recursos es gestionada automáticamente
 * por std::unique_ptr.
 */
Window::~Window() {
}

/**
 * @brief Comprueba si la ventana permanece abierta.
 *
 * Verifica que la instancia interna exista y consulta
 * el estado de la ventana de SFML.
 *
 * @return true si la ventana está abierta.
 * @return false si está cerrada.
 */
bool Window::isOpen() const {

  if (m_window) {
    return m_window->isOpen();
  }
  else {
    ERROR("Window", "isOpen", "Window is null");
    return false;
  }
}

/**
 * @brief Limpia el contenido actual de la ventana.
 *
 * El color indicado será utilizado como fondo para el
 * siguiente frame.
 *
 * @param color Color de limpieza.
 */
void Window::clear(const sf::Color& color) {

  if (m_window) {
    m_window->clear(color);
  }
  else {
    ERROR("Window", "clear", "Window is null");
  }
}

/**
 * @brief Dibuja un objeto en la ventana.
 *
 * @param drawable Objeto derivado de sf::Drawable.
 * @param states Estados de renderizado utilizados.
 */
void Window::draw(
  const sf::Drawable& drawable,
  const sf::RenderStates& states
) {

  if (m_window) {
    m_window->draw(drawable, states);
  }
  else {
    ERROR("Window", "draw", "Window is null");
  }
}

/**
 * @brief Muestra en pantalla el contenido renderizado.
 *
 * Intercambia los buffers internos de SFML para presentar
 * el frame actual.
 */
void Window::display() {

  if (m_window) {
    m_window->display();
  }
  else {
    ERROR("Window", "display", "Window is null");
  }
}

/**
 * @brief Solicita el cierre de la ventana.
 */
void Window::close() {

  if (m_window) {
    m_window->close();
  }
  else {
    ERROR("Window", "close", "Window is null");
  }
}

/**
 * @brief Actualiza el tiempo entre frames.
 *
 * Reinicia el reloj interno y almacena el tiempo
 * transcurrido desde la última actualización.
 */
void Window::update() {

  deltaTime = clock.restart();
}

/**
 * @brief Ejecuta el proceso de renderizado.
 *
 * Actualmente este método se encuentra vacío y queda
 * disponible para futuras ampliaciones del motor.
 */
void Window::render() {

}

/**
 * @brief Libera explícitamente la ventana.
 *
 * Reinicia el puntero inteligente y destruye la instancia
 * de sf::RenderWindow asociada.
 */
void Window::destroy() {

  m_window.reset();
}