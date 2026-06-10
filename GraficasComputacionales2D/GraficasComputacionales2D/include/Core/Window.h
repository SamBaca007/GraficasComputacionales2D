#pragma once
#include "Prerequisites.h"

/**
 * @class Window
 * @brief Encapsula una ventana de renderizado basada en SFML.
 *
 * Esta clase actúa como una abstracción de sf::RenderWindow,
 * proporcionando una interfaz simplificada para la gestión
 * de ventanas, renderizado y actualización del motor.
 *
 * También mantiene información relacionada con la vista activa
 * y el control temporal utilizado durante la ejecución.
 */
class
  Window {
public:

  /**
   * @brief Constructor por defecto.
   */
  Window() = default;

  /**
   * @brief Construye una ventana con las características indicadas.
   *
   * @param width Ancho de la ventana en píxeles.
   * @param height Alto de la ventana en píxeles.
   * @param title Título mostrado en la barra de la ventana.
   */
  Window(int width, int height, const std::string& title);

  /**
   * @brief Destructor de la ventana.
   */
  ~Window();

  /*
   * @brief Procesa los eventos de entrada.
   *
   * Método reservado para la integración con la interfaz gráfica
   * del motor.
   */
   // void handleEvents(EngineGUI& engineGUI);

   /**
    * @brief Comprueba si la ventana permanece abierta.
    *
    * @return true si la ventana está activa.
    * @return false si ha sido cerrada.
    */
  bool
    isOpen() const;

  /**
   * @brief Limpia el contenido actual de la ventana.
   *
   * Normalmente se invoca al inicio de cada frame antes de
   * realizar cualquier operación de dibujo.
   *
   * @param color Color utilizado para limpiar la pantalla.
   */
  void
    clear(
    const sf::Color& color = sf::Color(0, 0, 0, 255)
  );

  /**
   * @brief Dibuja un objeto renderizable.
   *
   * Recibe cualquier objeto derivado de sf::Drawable y lo
   * envía al pipeline de renderizado de SFML.
   *
   * @param drawable Objeto que será dibujado.
   * @param states Estados de renderizado utilizados.
   */
  void
    draw(
    const sf::Drawable& drawable,
    const sf::RenderStates& states =
    sf::RenderStates::Default
  );

  /**
   * @brief Presenta el frame renderizado en pantalla.
   *
   * Intercambia los buffers internos de la ventana para
   * mostrar los elementos dibujados durante el frame actual.
   */
  void
    display();

  /**
   * @brief Solicita el cierre de la ventana.
   */
  void
    close();

  /**
   * @brief Actualiza el estado interno de la ventana.
   *
   * Puede incluir el procesamiento de eventos, actualización
   * temporal y cualquier lógica relacionada con la ventana.
   */
  void
    update();

  /**
   * @brief Ejecuta el proceso de renderizado.
   *
   * Método encargado de coordinar el dibujado de los elementos
   * visibles durante el frame actual.
   */
  void
    render();

  /**
   * @brief Libera los recursos asociados a la ventana.
   *
   * Debe utilizarse cuando se desea destruir explícitamente
   * la instancia de renderizado.
   */
  void
    destroy();

  /**
   * @brief Ventana de renderizado principal.
   *
   * Se almacena mediante un puntero inteligente para facilitar
   * la gestión automática de memoria.
   */
  std::unique_ptr<sf::RenderWindow> m_window;

private:

  /**
   * @brief Vista activa utilizada para el renderizado.
   *
   * Define la región visible de la escena.
   */
  sf::View m_view;

  /**
   * @brief Tiempo transcurrido durante el último frame.
   */
  sf::Time deltaTime;

  /**
   * @brief Reloj utilizado para medir el tiempo entre frames.
   */
  sf::Clock clock;
};