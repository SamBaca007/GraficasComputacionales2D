/**
 * @file Window.cpp
 * @brief Implementación de la clase Window.
 *        Contiene la lógica interna para la inicialización, dibujado y gestión
 *        de la ventana de renderizado de SFML.
 */

#include "Core/Window.h"

 /**
	* @brief Constructor parametrizado.
	*        Crea la ventana de SFML con las dimensiones y título dados, y establece
	*        el límite de fotogramas por segundo a 60.
	*/
Window::Window(int width, int height, const std::string& title) {

	m_window = std::make_unique<sf::RenderWindow>(sf::VideoMode({ static_cast<unsigned int>
		(width), static_cast<unsigned int>(height) }), title, sf::Style::Default);
	if (m_window) {
		m_window->setFramerateLimit(60);
		MESSAGE("Window", "Window", "Window created successfully");
	}
	else {
		ERROR("Window", "Window", "Failed to create window");
	}
}

/**
 * @brief Verifica el estado de la ventana.
 */
bool
Window::isOpen() const {
	// Check that window is not null
	if (m_window) {
		return m_window && m_window->isOpen();
	}
	else {
		ERROR("Window", "isOpen", "Window is null");
		return false;
	}
}

/**
 * @brief Limpia la ventana con el color especificado.
 */
void
Window::clear(const sf::Color& color) {
	if (m_window) {
		m_window->clear(color);
	}
	else {
		ERROR("Window", "clear", "Window is null");
	}
}

/**
 * @brief Dibuja un objeto en la ventana actual.
 */
void
Window::draw(const sf::Drawable& drawable, const sf::RenderStates& states) {
	if (m_window) {
		m_window->draw(drawable, states);
	}
	else {
		ERROR("Window", "draw", "Window is null");
	}
}

/**
 * @brief Muestra el búfer dibujado en la pantalla.
 */
void
Window::display() {
	if (m_window) {
		m_window->display();
	}
	else {
		ERROR("Window", "display", "Window is null");
	}
}

/**
 * @brief Cierra el proceso de la ventana.
 */
void
Window::close()
{
	if (m_window) {
		m_window->close();
	}
	else {
		ERROR("Window", "close", "Window is null");
	}
}

/**
 * @brief Maneja el evento de redimensionamiento de la ventana.
 *
 * @details Configura una vista 1:1 con el tamaño de la ventana (sin estiramiento).
 *          El centro de la vista queda en (0,0), de modo que el origen del mundo
 *          se sitúa en el CENTRO de la pantalla.
 */
void Window::handleResize(const sf::Vector2u& size) {
	if (!m_window) {
		ERROR("Window", "handleResize", "Window is null");
		return;
	}
	// Vista 1:1 con el tamaño de la ventana -> sin estiramiento.
	// Centro de la vista queda en (0,0) -> el origen del mundo queda en
	// el CENTRO de la pantalla. Área visible: (-w/2, -h/2)..(w/2, h/2).
	const sf::Vector2f fSize(static_cast<float>(size.x), static_cast<float>(size.y));

	m_baseViewSize = fSize; // tamaño base (sin zoom para la cámara)
	m_view.setSize(fSize);
	m_view.setCenter({ 0.f, 0.f });
	m_window->setView(m_view);
}

/**
 * @brief Aplica una transformación de cámara a la vista actual.
 *
 * @details El tamaño visible se calcula dividiendo el tamaño base entre el zoom
 *          (un mayor zoom implica menos mundo visible). Se previene automáticamente
 *          la división por cero y las vistas invertidas.
 */
void
Window::applyCameraView(const sf::Vector2f& center, float zoom, float rotationDeg) {
	if (!m_window) {
		ERROR("Window", "applyCameraView", "Window is null");
		return;
	}
	if (zoom <= 0.f) zoom = 1.f; // evita división por cero / vista invertida

	// Tamaño visible = tamaño base / zoom (más zoom -> menos mundo visible).
	m_view.setSize(m_baseViewSize / zoom);
	m_view.setCenter(center);
	m_view.setRotation(sf::degrees(rotationDeg)); // rota toda la vista
	m_window->setView(m_view);
}

/**
 * @brief Actualiza la lógica de la ventana y los cálculos de tiempo (Delta Time).
 */
void
Window::update() {
	// Almacena el deltaTime una sola vez
	deltaTime = clock.restart();
}

/**
 * @brief Ejecuta el ciclo de renderizado.
 */
void
Window::render() {
}

/**
 * @brief Destruye explícitamente la instancia de la ventana.
 */
void
Window::destroy() {
	m_window.reset();
}