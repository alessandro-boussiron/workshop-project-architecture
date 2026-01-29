# NASA Power of 10 Rules - Extended Examples

Les 10 règles de codage pour missions critiques de la NASA/JPL, avec exemples détaillés séparés par règle.

## 📚 Structure des fichiers

### Exemples par règle
- `rule01_control_flow.c` - Règle 1: Contrôle de flux simple
- `rule02_loop_bounds.c` - Règle 2: Boucles bornées
- `rule03_no_dynamic_memory.c` - Règle 3: Pas d'allocation dynamique
- `nasa_rules.c` - Exemple complet avec toutes les règles

### Documentation
- `README.md` - Ce fichier
- `EXERCISES.md` - Exercices pratiques
- `Makefile` - Compilation et tests

## 🚀 Quick Start

### Compiler tous les exemples
```bash
make all
```

### Exécuter tous les exemples
```bash
make run
```

### Exécuter un exemple spécifique
```bash
make run-rule1    # Règle 1 seulement
make run-rule2    # Règle 2 seulement
make run-rule3    # Règle 3 seulement
```

### Analyse statique
```bash
make analyze      # Clang static analyzer
make check        # cppcheck (si installé)
```

## 📋 Les 10 Règles

### Rule 1: Restrict Control Flow ✅
**Fichier**: `rule01_control_flow.c`

Pas de `goto`, `setjmp/longjmp`, ou récursion indirecte.

**Exemples**:
- ❌ Spaghetti code avec goto
- ❌ Récursion directe/indirecte
- ✅ Structure de contrôle claire
- ✅ State machines avec switch
- ✅ Boucles itératives

**Pourquoi**: Flux d'exécution prévisible, analyse statique facilitée.

### Rule 2: Fixed Loop Bounds ✅
**Fichier**: `rule02_loop_bounds.c`

Toutes les boucles doivent avoir une borne supérieure fixe.

**Exemples**:
- ❌ `while(1)` sans limite claire
- ❌ Boucles dépendant d'état externe
- ✅ Boucles `for` avec constantes
- ✅ Recherche avec max iterations
- ✅ Ring buffers avec taille fixe

**Pourquoi**: Garantie de terminaison, temps d'exécution borné.

### Rule 3: No Dynamic Memory After Init ✅
**Fichier**: `rule03_no_dynamic_memory.c`

Pas d'allocation dynamique (`malloc/free`) après l'initialisation.

**Exemples**:
- ❌ `malloc` pendant l'exécution
- ❌ Structures de données qui grandissent
- ✅ Object pools statiques
- ✅ Ring buffers pré-alloués
- ✅ Hash tables fixes

**Pourquoi**: Pas de fragmentation, pas d'échec d'allocation, mémoire prévisible.

### Rule 4: Limit Function Size
Fonctions < 60 lignes (une page imprimée).

**Pourquoi**: Lisibilité, testabilité, compréhension rapide.

### Rule 5: Check Return Values
Toujours vérifier les valeurs de retour des fonctions non-void.

**Pourquoi**: Détection d'erreurs, robustesse.

### Rule 6: Limit Scope
Déclarer variables au scope le plus restreint possible.

**Pourquoi**: Réduction des effets de bord, clarté.

### Rule 7: Check Assertions
Utiliser assertions pour documenter et vérifier les invariants.

**Pourquoi**: Vérification des assumptions, debug facilité.

### Rule 8: Limit Pointer Indirection
Maximum 2 niveaux d'indirection (`**ptr` OK, `***ptr` NON).

**Pourquoi**: Complexité réduite, moins d'erreurs.

### Rule 9: Zero Compiler Warnings
Compiler avec maximum de warnings, zero toléré.

**Compilation**:
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c11 file.c
```

**Pourquoi**: Détection précoce de bugs potentiels.

### Rule 10: Use Static Analysis
Utiliser outils d'analyse statique (Coverity, PC-Lint, Clang Analyzer).

**Pourquoi**: Détection automatique de bugs, vérification formelle.

## 🎯 Exemples Pratiques

### Example 1: Système de télémétrie
- Buffer circulaire fixe (Rule 2, 3)
- Fonctions courtes (Rule 4)
- Vérifications de retour (Rule 5)
- Assertions (Rule 7)

### Example 2: Object Pool
- Allocation statique (Rule 3)
- Bounded iteration (Rule 2)
- Clear control flow (Rule 1)

### Example 3: Parser de protocole
- State machine sans goto (Rule 1)
- Timeout avec max iterations (Rule 2)
- Buffer fixe (Rule 3)

## 📊 Métriques de Code

| Métrique | Limite | Vérification |
|----------|--------|--------------|
| Lignes par fonction | < 60 | Manuel/script |
| Niveaux indirection | ≤ 2 | Analyse statique |
| Warnings | 0 | Compilateur |
| Bornes de boucles | Toutes fixes | Code review |
| malloc/free runtime | 0 | Code review |

## 🔧 Outils Recommandés

### Compilateur
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c11
clang -Weverything
```

### Analyse Statique
```bash
clang --analyze file.c
cppcheck --enable=all file.c
splint file.c
```

### Runtime Checks
```bash
gcc -fsanitize=address -fsanitize=undefined file.c
valgrind --leak-check=full ./program
```

## 📚 Resources Additionnelles

- NASA JPL Coding Standard (PDF)
- MISRA-C Guidelines
- CERT C Coding Standard
- "The Power of 10" by Gerard J. Holzmann

## 🎓 Exercices

Voir `EXERCISES.md` pour exercices pratiques graduels.

## ✅ Checklist pour Code Review

- [ ] Pas de goto, setjmp, recursion
- [ ] Toutes boucles avec borne fixe
- [ ] Pas de malloc/free en runtime
- [ ] Fonctions < 60 lignes
- [ ] Tous returns vérifiés
- [ ] Variables au scope minimal
- [ ] Assertions pour invariants
- [ ] Max 2 niveaux pointeurs
- [ ] Compile sans warnings
- [ ] Passe analyse statique

## 💡 Best Practices

1. **Commencer par le domaine** - Modéliser le problème
2. **Pré-allouer** - Estimer besoins mémoire
3. **Définir constantes** - `MAX_*` pour toutes limites
4. **Documenter** - Pourquoi, pas quoi
5. **Tester** - Unit tests pour chaque fonction
6. **Analyser** - Static analysis régulièrement

## 🏆 Applications Réelles

Ces règles sont utilisées dans:
- 🚀 Mars rovers (Curiosity, Perseverance)
- 🛰️ Satellites et sondes spatiales
- ✈️ Systèmes avioniques critiques
- 🏥 Dispositifs médicaux
- ⚡ Centrales nucléaires
- 🚗 Véhicules autonomes (ASIL-D)

---

**Maintenu par**: Workshop Project Architecture  
**Dernière mise à jour**: 2026-01-29
