#pragma once
#include "Prerequisites.h"

class Window;

/**
 * @class CShape
 * @brief Encapsula una figura geométrica de SFML.
 *
 * Esta clase actúa como un contenedor para objetos derivados
 * de sf::Shape, permitiendo crear y gestionar diferentes tipos
 * de figuras geométricas mediante el enum ShapeType.
 *
 * La memoria de la figura se administra automáticamente mediante
 * std::unique_ptr.
 */
class CShape {
public:

  /**
   * @brief Constructor por defecto.
   */
  CShape() = default;

  /**
   * @brief Construye una figura del tipo indicado.
   *
   * Internamente crea una instancia de la figura correspondiente
   * utilizando el método estático createShape().
   *
   * @param shapeType Tipo de figura a crear.
   */
  explicit CShape(ShapeType shapeType);

  /**
   * @brief Destructor por defecto.
   *
   * La liberación de memoria es gestionada automáticamente
   * por std::unique_ptr.
   */
  ~CShape() = default;

  /**
   * @brief Dibuja la figura en una ventana.
   *
   * Envía la figura almacenada al sistema de renderizado
   * de la ventana indicada.
   *
   * @param window Ventana donde se dibujará la figura.
   */
  void draw(Window& window);

  /**
   * @brief Obtiene acceso a la figura interna.
   *
   * Permite modificar propiedades específicas de SFML
   * como posición, color, rotación o escala.
   *
   * @return Puntero a la figura almacenada.
   */
  sf::Shape* getShape();

private:

  /**
   * @brief Crea una figura de acuerdo con el tipo indicado.
   *
   * Este método es responsable de instanciar la implementación
   * concreta de sf::Shape asociada al valor de ShapeType.
   *
   * @param shapeType Tipo de figura a crear.
   *
   * @return Puntero inteligente a la figura creada.
   */
  static std::unique_ptr<sf::Shape>
    createShape(ShapeType shapeType);

private:

  /**
   * @brief Figura geométrica administrada por el componente.
   */
  std::unique_ptr<sf::Shape> m_shape = nullptr;

  /**
   * @brief Tipo de figura almacenada.
   */
  ShapeType m_shapeType;
};