/**
 * @file Render.h
 * @brief Componente Render de DATOS PUROS para el Entity Component System (ECS).
 *
 * @details Guarda la forma dibujable (sf::Shape) de una entidad.
 *          Se almacena en un ComponentPool<Render>. Usa shared_ptr (movible y copiable)
 *          para ser compatible con el pool, a diferencia de CShape, que tiene el move suprimido.
 *
 *          La POSICIÓN/ROTACIÓN/ESCALA NO viven aquí: vienen del componente Transform
 *          y las aplica el RenderSystem cada frame.
 */

#pragma once
#include "Prerequisites.h"

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @struct Render
   * @brief Componente que define el aspecto visual de una entidad mediante una forma de SFML.
   */
  struct Render {
    /** @brief La forma a dibujar. */
    std::shared_ptr<sf::Shape> shape;

    /** @brief Sprite OPCIONAL (nullptr = sin sprite). */
    std::shared_ptr<sf::Texture> texture;

    /** @brief Color de relleno (por defecto blanco). */
    sf::Color fillColor{ sf::Color::White };

    /** @brief Permite ocultar la entidad sin necesidad de quitar el componente. */
    bool visible{ true };

    /**
     * @brief Constructor por defecto.
     */
    Render() = default;

    /**
     * @brief Constructor parametrizado.
     *
     * @param s Puntero compartido a la forma (sf::Shape) a utilizar.
     * @param color Color de relleno inicial (por defecto sf::Color::White).
     */
    explicit Render(std::shared_ptr<sf::Shape> s,
      sf::Color color = sf::Color::White) noexcept
      : shape(std::move(s)), fillColor(color) {
    }

    // Sprite opcional

    /**
     * @brief Carga una textura desde disco y la aplica a la figura.
     *
     * @details La textura se conserva en 'texture' (la figura solo guarda un puntero
     *          crudo en ella). No toca el estado si falla la carga.
     *
     * @param path Ruta del archivo de la textura a cargar.
     * @param resetRect Si es true, ajusta el rectángulo de textura al tamaño real de la imagen.
     * @return true Si la textura se cargó y aplicó correctamente.
     * @return false Si la figura es nula o el archivo no se pudo cargar.
     */
    bool SetTexture(const std::string& path, bool resetRect = true) {
      if (!shape) return false;
      auto tex = std::make_shared<sf::Texture>();
      if (!tex->loadFromFile(path)) return false; // No toca el estado si falla
      texture = std::move(tex);
      shape->setTexture(texture.get(), resetRect); // resetRect ajusta el rect al tamaño
      return true;
    }

    /**
     * @brief Aplica una textura ya cargada.
     *
     * @details Compartir texturas ya instanciadas entre entidades es mucho más eficiente.
     *
     * @param tex Puntero compartido a la textura (sf::Texture) ya existente.
     * @param resetRect Si es true, ajusta el rectángulo de textura.
     */
    void SetTexture(std::shared_ptr<sf::Texture> tex, bool resetRect = true) {
      if (!shape) return;
      texture = std::move(tex);
      shape->setTexture(texture ? texture.get() : nullptr, resetRect);
    }

    /**
     * @brief Quita el sprite actual y vuelve a mostrar la figura con su color sólido.
     */
    void ClearTexture() {
      if (shape)shape->setTexture(nullptr);
      texture.reset();
    }

    /**
     * @brief Factory de conveniencia que crea una forma predefinida centrada en su origen.
     *
     * @details Crea la forma centrada en su origen, para que el componente Transform
     *          la posicione por su CENTRO (no por la esquina superior izquierda).
     *          Reutiliza el enum ShapeType.
     *
     * @param type Tipo de figura a crear (proveniente del enum ShapeType).
     * @param color Color de relleno (por defecto sf::Color::White).
     * @param texturePath Ruta opcional a una textura. Si se pasa una ruta, intenta cargar
     *                    el sprite (falla de forma silenciosa si no lo encuentra).
     * @return Render Una nueva instancia del componente Render inicializada.
     */
    [[nodiscard]] static Render
      Make(ShapeType type, sf::Color color = sf::Color::White,
        const std::string& texturePath = "") {
      std::shared_ptr<sf::Shape> s;
      switch (type) {
      case CIRCLE: {
        auto c = std::make_shared<sf::CircleShape>(50.f);
        c->setOrigin({ 50.f,50.f }); // centro = radio
        s = c;
        break;
      }
      case RECTANGLE: {
        sf::Vector2f size{ 100.f,100.f };
        auto r = std::make_shared<sf::RectangleShape>(size);
        r->setOrigin(size / 2.f);
        s = r;
        break;
      }
      case TRIANGLE: {
        auto t = std::make_shared<sf::ConvexShape>(3);
        t->setPoint(0, { 0.f, 0.f });
        t->setPoint(1, { 100.f, 0.f });
        t->setPoint(2, { 50.f, 100.f });
        t->setOrigin({ 50.f, 50.f });
        s = t;
        break;
      }
      default:
        break;
      }
      if (s) s->setFillColor(color);
      Render render{ s, color };
      // Si se pasó una ruta, intenta cargar el sprite (silencioso si falla)
      if (!texturePath.empty())
        render.SetTexture(texturePath);
      return render;
    }
  };
} // Namespace ECS