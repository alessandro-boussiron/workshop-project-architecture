# Architecture Web Moderne

## 🎯 Vue d'ensemble

Cette section couvre les architectures modernes pour applications web, avec focus sur la maintenabilité, testabilité et scalabilité.

## 📚 Concepts Clés

### Clean Architecture
Indépendance vis-à-vis des frameworks, UI, base de données. Le domaine métier au centre.

### Domain-Driven Design (DDD)
Alignement du code avec la logique métier. Ubiquitous Language, Bounded Contexts, Entities, Value Objects.

### CQRS
Séparation lecture/écriture pour optimiser performances et scalabilité.

### Repository Pattern
Abstraction de la couche de persistance pour faciliter les tests et le changement de technologie.

## 🗂️ Modules

1. **clean-architecture/** - Architecture hexagonale complète
2. **ddd-example/** - Modélisation DDD d'un domaine e-commerce
3. **cqrs-pattern/** - Implémentation CQRS avec Event Sourcing
4. **repository-pattern/** - Patterns de persistance

Chaque module est autonome et exécutable.
