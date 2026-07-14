/**
 * @file Window.h
 * @brief Declaración de la clase Window, encargada de crear y gestionar la ventana de renderizado principal.
 */

#pragma once
#include "Prerequisites.h"

 /**
	* @class Window
	* @brief Envoltorio (wrapper) para la ventana de SFML (sf::RenderWindow).
	*        Maneja la inicialización, limpieza, vistas de cámara, y el ciclo de dibujado.
	*/
class
	Window {
public:
	/**
	 * @brief Constructor por defecto.
	 */
	Window() = default;

	/**
	 * @brief Constructor parametrizado que inicializa y abre la ventana.
	 *
	 * @param width Ancho de la ventana en píxeles.
	 * @param height Alto de la ventana en píxeles.
	 * @param title Título que se mostrará en la barra de la ventana.
	 */
	Window(int width, int height, const std::string& title);

	/**
	 * @brief Destructor por defecto.
	 */
	~Window() = default;

	//void 
	//handleEvents(EngineGUI& engineGUI);

	/**
	 * @brief Comprueba si la ventana sigue abierta.
	 *
	 * @return true Si la ventana está abierta y activa.
	 * @return false Si la ventana ha sido cerrada.
	 */
	bool
		isOpen() const;

	/**
	 * @brief Limpia la ventana con un color de fondo específico.
	 *
	 * @param color Color de limpieza (por defecto es negro opaco: 0, 0, 0, 255).
	 */
	void
		clear(const sf::Color& color = sf::Color(0, 0, 0, 255));

	/**
	 * @brief Dibuja un objeto renderizable de SFML en la ventana.
	 *
	 * @param drawable Referencia al objeto que hereda de sf::Drawable (ej. sf::Shape, sf::Sprite).
	 * @param states Estados de renderizado a aplicar (transformaciones, shaders, blend modes).
	 */
	void
		draw(const sf::Drawable& drawable,
			const sf::RenderStates& states = sf::RenderStates::Default);

	/**
	 * @brief Muestra en pantalla lo que se ha dibujado en el frame actual (intercambio de buffers).
	 */
	void
		display();

	/**
	 * @brief Cierra la ventana actual de forma manual.
	 */
	void
		close();

	/**
	 * @brief Maneja el redimensionamiento de la ventana para mantener las proporciones o actualizar la vista.
	 *
	 * @param size Nuevo tamaño de la ventana en píxeles.
	 */
	void
		handleResize(const sf::Vector2u& size);

	/**
	 * @brief Aplica una vista de cámara personalizada a la ventana.
	 *
	 * @param center Punto central al que apuntará la cámara (coordenadas del mundo).
	 * @param zoom Nivel de acercamiento (zoom). Valores menores a 1 acercan, mayores a 1 alejan.
	 * @param rotationDeg Rotación de la vista en grados.
	 */
	void
		applyCameraView(const sf::Vector2f& center, float zoom, float rotationDeg);

	/**
	 * @brief Actualiza la lógica interna de la ventana, como el cálculo del deltaTime.
	 */
	void
		update();

	/**
	 * @brief Método principal de renderizado (se puede utilizar para invocar el dibujado interno).
	 */
	void
		render();

	/**
	 * @brief Libera los recursos de la ventana destruyendo explícitamente el puntero a sf::RenderWindow.
	 */
	void
		destroy();

public:
	/** @brief Puntero inteligente a la ventana de renderizado de SFML. */
	std::unique_ptr<sf::RenderWindow> m_window = nullptr;

private:
	/** @brief Vista (cámara) actual aplicada a la ventana. */
	sf::View m_view;

	/** @brief Tamaño base de la vista antes de aplicar cualquier zoom o transformación. */
	sf::Vector2f m_baseViewSize;

	/** @brief Tiempo transcurrido desde el último frame (Delta Time). */
	sf::Time deltaTime;

	/** @brief Reloj de SFML utilizado para calcular el tiempo entre frames. */
	sf::Clock clock;
};