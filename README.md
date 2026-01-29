# Workshop: Architecture de Projet

Un workshop pratique sur les architectures de projet modernes, avec focus sur le code et les bonnes pratiques.

## 📋 Contenu

### 🌐 Partie 1: Architecture Web Moderne

#### 📚 Guides Complets
- **[Clean Architecture](web/WEB_CLEAN_ARCHITECTURE.md)** - Guide détaillé avec diagrammes, patterns, comparaisons
  - Principes SOLID et Dependency Rule
  - Architecture en couches vs Hexagonale
  - Ports & Adapters
  - [Exercices pratiques](web/WEB_CLEAN_ARCHITECTURE_EXERCISES.md) (6 exercices progressifs)
  
- **[Domain-Driven Design](web/WEB_DDD.md)** - Modélisation centrée sur le métier
  - Ubiquitous Language, Bounded Contexts
  - Entities, Value Objects, Aggregates
  - Domain Events et Services
  - Strategic vs Tactical DDD
  
- **[CQRS & Event Sourcing](web/WEB_CQRS.md)** - Patterns avancés
  - Command Query Responsibility Segregation
  - Event Store et Replay
  - Read Models et Projections
  - Quand utiliser (et ne pas utiliser)

#### 💻 Exemples de Code
- **Repository Pattern** - Abstraction de la persistance
- **Dependency Injection** - Inversion de contrôle

### 🔧 Partie 2: Architecture C & Embedded Systems
- **NASA Coding Standards** - Les 10 règles critiques pour systèmes spatiaux
  - Documentation détaillée: `c/nasa-rules/docs/RULE01.md` à `RULE10.md`
  - Exercices pratiques: `c/nasa-rules/exercises/ex01-ex10`
- **Memory Safety** - Gestion sûre de la mémoire **SANS malloc!** ⭐
  - Guide complet: `c/memory-safety/MEMORY_RULES.md`
  - Code démonstration: `c/memory-safety/memory_safety.c`
  - **Focus**: Allocation statique, Object Pools, Arena Allocators
  - **Philosophie**: 90% des cas n'ont pas besoin de malloc()
- **Layered Architecture** - Architecture modulaire pour embedded
- **Error Handling** - Gestion robuste des erreurs

## 🚀 Structure du Workshop

```
web/                                    # Architecture web
├── WEB_CLEAN_ARCHITECTURE.md          # 📖 Guide complet Clean Architecture
├── WEB_CLEAN_ARCHITECTURE_EXERCISES.md # 🎯 6 exercices progressifs
├── WEB_DDD.md                          # 📖 Guide complet DDD
├── WEB_CQRS.md                         # 📖 Guide CQRS + Event Sourcing
├── clean-architecture/                 # Exemples code Clean Arch
│   ├── example.ts                      # Exemple complet commenté
│   └── README.md
├── ddd-example/                        # Exemples DDD
│   ├── order-domain.ts                 # Aggregate Order avec Value Objects
│   └── README.md
├── cqrs-pattern/                       # Exemple CQRS + Event Sourcing
│   ├── bank-account.ts                 # BankAccount avec events
│   └── README.md
└── repository-pattern/                 # Pattern Repository

c/                                      # Architecture C & Embedded
├── nasa-rules/         # 10 règles NASA appliquées
│   ├── docs/           # RULE01.md à RULE10.md (explications détaillées)
│   └── exercises/      # ex01 à ex10 (exercices pratiques)
├── memory-safety/      # 🌟 Gestion mémoire SANS malloc
│   ├── MEMORY_RULES.md # Guide complet (à lire en premier!)
│   └── memory_safety.c # Patterns: static, pools, arenas
└── layered-arch/       # Architecture modulaire HAL/Driver/Service
```

## 🎯 Objectifs

- Comprendre les principes SOLID appliqués
- Maîtriser la séparation des responsabilités
- Écrire du code maintenable et testable
- Appliquer des standards de sécurité critiques
- Architecturer des systèmes robustes

## 💻 Prérequis

## 📖 Comment utiliser ce workshop

### 📚 Ordre de Lecture Recommandé

**Pour la partie Web (Architecture Moderne):**

#### Débutant → Intermédiaire
1. **Commencer par:** `web/WEB_CLEAN_ARCHITECTURE.md`
   - Comprendre la séparation des couches
   - Domain vs Application vs Infrastructure
   - Dependency Inversion Principle
   
2. **Pratiquer:** `web/WEB_CLEAN_ARCHITECTURE_EXERCISES.md`
   - Exercice 1: Task Manager (débutant)
   - Exercice 2: Blog System (intermédiaire)
   - Exercice 3: E-commerce Cart (intermédiaire)

3. **Approfondir:** `web/WEB_DDD.md`
   - Entities vs Value Objects
   - Aggregates et Bounded Contexts
   - Ubiquitous Language
   - Domain Events

4. **Avancé:** `web/WEB_CQRS.md`
   - Séparation Command/Query
   - Event Sourcing
   - Projections et Read Models

#### Points Clés Web
- ✨ **Domain au centre** - La logique métier ne dépend de rien
- 🎯 **Testabilité** - Use cases testables avec mocks
- 🔄 **Flexibility** - Changement de DB/framework facile
- 📊 **Scalabilité** - Architecture qui supporte la croissance

---

**Pour la partie C (Memory Safety):**
**C:**
- GCC ou Clang
- Make
### 📦 Chaque Section Contient

**Guides Web:**
- 📖 **Explications détaillées** - Concepts avec diagrammes
- 💻 **Exemples complets** - Code commenté et fonctionnel
- 🎯 **Exercices progressifs** - Du débutant à l'avancé
- ⚠️ **Anti-patterns** - Erreurs courantes à éviter
- ✅ **Best practices** - Patterns recommandés
- 📊 **Comparaisons** - Quand utiliser quoi

**Exemples C:**
- 📝 **Théorie minimale** - Contexte essentiel
- 💻 **Code complet** - Exemples fonctionnels
- ⚠️ **Anti-patterns** - Ce qu'il faut éviter
- ✅ **Best practices** - Solutions recommandées
**Pour la partie C (Memory Safety):**

1. **Commencer par:** `c/memory-safety/MEMORY_RULES.md`
   - Comprendre pourquoi malloc() n'est pas nécessaire
   - Voir la hiérarchie des solutions (static > pools > arena > malloc)
   - Apprendre les 7 règles fondamentales

2. **Ensuite:** `c/memory-safety/memory_safety.c`
   - Exemples concrets de chaque pattern
   - Code compilable et exécutable
   - Démonstrations avec commentaires

3. **Puis:** Les règles NASA individuelles
   - `c/nasa-rules/docs/RULE03.md` (No dynamic memory)
   - Complète la compréhension memory safety

4. **Enfin:** Exercices pratiques
   - `c/nasa-rules/exercises/ex03_static_memory.c`
   - Mettre en pratique

**Points Clés à Retenir:**
- ✨ **malloc() n'est PAS obligatoire** - 90% des cas utilisent static
- 🎯 **Taille max connue = tableau fixe** - Simple et sûr
- 🔒 **Pas de malloc après init** - Systèmes critiques
- 📊 **Déterminisme > Flexibilité** - Performance prévisible

### 📦 Chaque Section Contient

- 📝 **Théorie minimale** - Contexte essentiel
- 💻 **Code complet** - Exemples fonctionnels
- ⚠️ **Anti-patterns** - Ce qu'il faut éviter
- ✅ **Best practices** - Solutions recommandées
- 🎓 **Exercices** - Mise en pratique

---

**Let's build better software! 🚀**
