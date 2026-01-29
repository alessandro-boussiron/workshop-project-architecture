# NASA Rules Exercises - README

Exercices pratiques pour maîtriser les 10 règles NASA Power of 10.

## Structure

```
exercises/
├── ex01_control_flow.c         # Règle 1: Pas de goto/recursion
├── ex02_loop_bounds.c          # Règle 2: Boucles bornées
├── ex03_static_memory.c        # Règle 3: Allocation statique
├── ex04_function_size.c        # Règle 4: Fonctions < 60 lignes
├── ex05_check_returns.c        # Règle 5: Vérifier les retours
├── ex06_limit_scope.c          # Règle 6: Portée minimale
├── ex07_assertions.c           # Règle 7: Utiliser les assertions
├── ex08_pointer_indirection.c  # Règle 8: Max 2 niveaux de pointeurs
├── ex09_warnings.c             # Règle 9: Zéro warning compilateur
├── ex10_static_analysis.c      # Règle 10: Analyse statique
└── SOLUTIONS.md                # Solutions complètes
```

## Compilation

### Exercice individuel

```bash
cd exercises/
gcc -Wall -Wextra -Werror -std=c11 ex01_control_flow.c -o ex01
./ex01
```

### Tous les exercices

```bash
cd ..  # Retour au dossier nasa-rules/
make exercises
```

### Exercice spécifique

```bash
make ex01  # Compile ex01_control_flow
./ex01     # Exécute
```

## Format des exercices

Chaque exercice contient:

1. **❌ BAD CODE** - Code avec problèmes à identifier
2. **✅ YOUR TASK** - Sections à compléter (TODO)
3. **TEST HARNESS** - Tests pour valider vos solutions
4. **GRADING CRITERIA** - Critères de réussite

## Progression recommandée

### Débutant
1. **Ex01** - Control Flow (facile)
2. **Ex02** - Loop Bounds (facile)
3. **Ex03** - Static Memory (moyen)

### Intermédiaire
4. **Ex04** - Function Size (moyen)
5. **Ex05** - Check Returns (moyen)
6. **Ex06** - Limit Scope (moyen)

### Avancé
7. **Ex07** - Assertions (difficile)
8. **Ex08** - Pointer Indirection (difficile)
9. **Ex09** - Zero Warnings (difficile)
10. **Ex10** - Static Analysis (difficile)

## Validation

### Compilation stricte

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c11 -g ex01_control_flow.c -o ex01
```

### Analyse statique

```bash
# Clang analyzer
clang --analyze ex01_control_flow.c

# Cppcheck
cppcheck --enable=all ex01_control_flow.c

# Valgrind (runtime)
valgrind --leak-check=full ./ex01
```

### Sanitizers

```bash
# Address sanitizer
gcc -fsanitize=address -g ex03_static_memory.c -o ex03
./ex03

# Undefined behavior sanitizer
gcc -fsanitize=undefined -g ex01_control_flow.c -o ex01
./ex01
```

## Conseils

### Ex01 - Control Flow
- Remplacer `goto` par `return` précoce
- Utiliser `switch` pour les états
- Transformer récursion en boucle

### Ex02 - Loop Bounds
- Définir `#define MAX_*` constants
- Utiliser `for (i = 0; i < MAX; i++)`
- Ajouter timeouts aux boucles while

### Ex03 - Static Memory
- Utiliser des tableaux fixes
- Créer des object pools
- Éviter malloc/free après init

### Ex04 - Function Size
- Une fonction = une responsabilité
- Extraire les calculs en helpers
- Maximum 60 lignes par fonction

### Ex05 - Check Returns
- Vérifier TOUS les retours non-void
- Utiliser `if (ptr == NULL)`
- Nettoyer sur tous les chemins d'erreur

### Ex06 - Limit Scope
- Déclarer dans le plus petit scope
- Utiliser `{ }` pour limiter
- Passer paramètres, pas globals

### Ex07 - Assertions
- `assert(ptr != NULL)` pour tous les pointeurs
- `assert(index < size)` pour les tableaux
- Documenter les invariants

### Ex08 - Pointer Indirection
- Maximum `**ptr`, jamais `***ptr`
- Utiliser indices au lieu de pointeurs
- Structures plates avec calcul d'index

### Ex09 - Zero Warnings
- Initialiser toutes les variables
- `(void)unused` pour paramètres
- Utiliser bons format specifiers

### Ex10 - Static Analysis
- Vérifier pointeurs avant usage
- Pas de fuites mémoire
- Pas de buffer overflow

## Tests

Chaque exercice inclut des tests. Décommentez les tests d'assertions pour les voir échouer:

```c
// Uncomment to test assertions:
// good_queue_enqueue(NULL, 10);  // Should assert
```

## Solutions

Voir `SOLUTIONS.md` pour les solutions complètes commentées.

⚠️ **Important**: Essayez d'abord par vous-même avant de regarder les solutions!

## Ressources

- `../rule01_control_flow.c` - Exemples détaillés Rule 1
- `../rule02_loop_bounds.c` - Exemples détaillés Rule 2
- `../rule03_no_dynamic_memory.c` - Exemples détaillés Rule 3
- `../README.md` - Documentation complète NASA rules

## Grading System

Chaque exercice sur 100 points:
- Fonctionnel (40 pts)
- Compile sans warnings (20 pts)
- Suit la règle NASA (30 pts)
- Bonnes pratiques (10 pts)

**Bonus**: +10 pts pour implémentation exceptionnelle

## Support

Si bloqué:
1. Lire les hints à la fin de chaque exercice
2. Regarder les exemples dans `rule0X_*.c`
3. Consulter `SOLUTIONS.md`
4. Tester avec différents compilateurs

## Commandes utiles

```bash
# Compiler tous les exercices
make exercises

# Nettoyer
make clean

# Exercice spécifique avec debug
gcc -g -Wall -Wextra ex01_control_flow.c -o ex01
gdb ./ex01

# Vérifier les fuites mémoire
valgrind --leak-check=full ./ex03

# Analyse statique complète
clang --analyze ex10_static_analysis.c
cppcheck --enable=all ex10_static_analysis.c
```

Bon courage! 🚀
