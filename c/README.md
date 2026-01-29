# Architecture C & Standards Critiques

## 🎯 Vue d'ensemble

Code C robuste pour systèmes critiques: embedded, aérospatial, médical. Standards NASA et MISRA-C appliqués.

## 🚀 NASA Power of 10 Rules

10 règles pour code critique mission-critical:
1. **Restrict flow** - Pas de goto, setjmp, longjmp
2. **Fixed loop bounds** - Boucles avec limite connue
3. **No dynamic allocation** - Pas de malloc après init
4. **Limit function size** - <60 lignes par fonction
5. **Check return values** - Toujours vérifier
6. **Limit scope** - Variables au scope minimal
7. **Check params** - Valider assertions
8. **Limit pointers** - Maximum 2 niveaux d'indirection
9. **Compile warnings** - Zero warning policy
10. **Static analysis** - Utiliser outils automatiques

## 📊 MISRA-C

Standard de l'industrie automobile et embedded. Focus sécurité et fiabilité.

## 🗂️ Modules

1. **nasa-rules/** - Exemples des 10 règles NASA
2. **misra-examples/** - Code conforme MISRA-C
3. **memory-safety/** - Patterns sûrs de gestion mémoire
4. **layered-arch/** - Architecture modulaire en C

Chaque exemple compile avec `-Wall -Wextra -Werror`.
