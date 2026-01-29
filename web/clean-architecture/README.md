# Clean Architecture - Example Complet

Architecture hexagonale avec séparation claire des couches.

## 🏗️ Structure

```
domain/     - Entités et logique métier pure
usecases/   - Cas d'usage (application business rules)
adapters/   - Implémentations concrètes (DB, API, etc.)
interfaces/ - Contrats (ports)
```

## 🎯 Principe

Les dépendances pointent vers l'intérieur. Le domaine ne dépend de rien.

```
[Frameworks/Drivers] -> [Adapters] -> [Use Cases] -> [Domain]
```
