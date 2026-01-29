# Layered Architecture in C

Architecture modulaire en couches pour systèmes embedded.

## 🏗️ Structure

```
Application Layer    (Business logic)
      ↓
Service Layer        (Coordination)
      ↓
Driver Layer         (Hardware abstraction)
      ↓
Hardware Layer       (Registers, peripherals)
```

## 📐 Principes

1. **Separation of Concerns** - Chaque couche un rôle
2. **Dependency Inversion** - Interfaces, pas implémentations
3. **Encapsulation** - API publique minimale
4. **Testability** - Mock hardware via interfaces

## 🎯 Avantages

- Portabilité (change hardware sans toucher app)
- Testabilité (mock HAL)
- Maintenabilité (couches indépendantes)
- Réutilisabilité (drivers génériques)
