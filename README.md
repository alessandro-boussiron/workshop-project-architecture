# Workshop: Architecture de Projet

Un workshop pratique sur les architectures de projet modernes, avec focus sur le code et les bonnes pratiques.

## 📋 Contenu

### 🌐 Partie 1: Architecture Web Moderne
- **Clean Architecture** - Séparation des préoccupations et indépendance des frameworks
- **Domain-Driven Design (DDD)** - Modélisation centrée sur le métier
- **CQRS** - Séparation commandes/requêtes
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
web/                    # Exemples architecture web
├── clean-architecture/ # Clean Architecture complète
├── ddd-example/        # Domain-Driven Design
├── cqrs-pattern/       # Command Query Responsibility Segregation
└── repository-pattern/ # Abstraction data access

c/                      # Exemples architecture C
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

**Web:**
- Node.js 18+
- TypeScript
- Connaissance basique des design patterns

**C:**
- GCC ou Clang
- Make
- Notions de programmation système

## 📖 Comment utiliser ce workshop

### 📚 Ordre de Lecture Recommandé

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
