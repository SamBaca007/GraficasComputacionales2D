#pragma once

namespace ECS {
  struct DRS {
    bool isActive = false;
    float speedBoost = 40.f;          // +40px/s de velocidad punta extra
    float accelBoost = 120.f;         // Aceleración extra para alcanzar rápido
    float detectionRange = 250.f;     // Rango de visión frontal
    float safeBrakingZone = 180.f;    // Distancia mínima al siguiente punto para apagarse

    // Variables para guardar los valores originales y poder restaurarlos
    float originalMaxSpeed = 0.f;
    float originalAccel = 0.f;
    bool initialized = false;
  };
}