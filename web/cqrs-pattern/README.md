# CQRS Pattern - Command Query Responsibility Segregation

Séparation des opérations de lecture et d'écriture pour optimiser performances et scalabilité.

## 🎯 Principe

- **Commands**: Modifient l'état (write model)
- **Queries**: Lisent l'état (read model)
- Modèles séparés, optimisés différemment

## 📊 Architecture

```
Commands -> Write Model -> Event Store
                 ↓
              Events
                 ↓
         Event Handlers -> Read Model (projections)
                              ↓
Queries ←――――――――――――――――――――――┘
```

## 💡 Avantages

- Scalabilité indépendante (read vs write)
- Optimisation spécifique par modèle
- Audit trail naturel avec events
- Performance améliorée
