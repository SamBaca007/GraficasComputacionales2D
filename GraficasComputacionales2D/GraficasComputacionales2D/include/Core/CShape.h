/**
 * @file CShape.h
 * @brief Definición de la clase CShape, encargada de la creación y gestión de figuras geométricas.
 */

#pragma once
#include "Prerequisites.h"

 /**
	* @brief Declaración adelantada (forward declaration) de la clase Window.
	*/
class Window;

/**
 * @class CShape
 * @brief Clase contenedora que administra una figura de SFML (sf::Shape) según el tipo especificado.
 *        Permite abstraer la creación y el renderizado de la figura.
 */
class
	CShape {
public:
	/**
	 * @brief Constructor por defecto de CShape.
	 */
	CShape() = default;

	/**
	 * @brief Constructor parametrizado que inicializa la figura con un tipo específico.
	 *
	 * @param shapeType El tipo de figura geométrica a crear (proveniente del enum ShapeType).
	 */
	explicit CShape(ShapeType shapeType);

	/**
	 * @brief Destructor por defecto de CShape.
	 */
	~CShape() = default;

	/**
	 * @brief Dibuja la figura geométrica en la ventana proporcionada.
	 *
	 * @param window Referencia a la ventana (Window) donde se renderizará la figura.
	 */
	void
		draw(Window& window);

	/**
	 * @brief Obtiene un puntero a la figura subyacente de SFML.
	 *
	 * @return sf::Shape* Puntero a la figura base de SFML.
	 */
	sf::Shape*
		getShape();

private:
	/**
	 * @brief Método de fábrica interno para instanciar el objeto sf::Shape correspondiente.
	 *
	 * @param shapeType El tipo de figura que se desea instanciar.
	 * @return std::unique_ptr<sf::Shape> Puntero inteligente con la nueva instancia de la figura.
	 */
	static std::unique_ptr<sf::Shape>
		createShape(ShapeType shapeType);

private:
	/** @brief Puntero inteligente que administra el ciclo de vida de la figura de SFML. */
	std::unique_ptr<sf::Shape> m_shape = nullptr;

	/** @brief Identificador del tipo de figura actual. */
	ShapeType m_shapeType;
};