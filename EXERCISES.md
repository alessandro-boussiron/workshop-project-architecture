# Workshop Exercises

## 🌐 Web Exercises

### Exercise 1: Clean Architecture - Add New Use Case
**Difficulté**: ⭐⭐

Ajouter un use case "ChangeUserEmail" dans `web/clean-architecture/example.ts`:
- Vérifier que le nouvel email est valide
- Vérifier qu'il n'existe pas déjà
- Mettre à jour l'utilisateur
- Envoyer un email de confirmation

**Objectif**: Comprendre comment ajouter de la fonctionnalité sans toucher aux couches basses.

### Exercise 2: DDD - Nouveau Value Object
**Difficulté**: ⭐⭐

Dans `web/ddd-example/order-domain.ts`, créer un Value Object `Discount`:
- Représente un pourcentage (0-100)
- Immutable
- Méthode `apply(money: Money): Money`
- Intégrer dans Order

**Objectif**: Maîtriser les Value Objects et leur immutabilité.

### Exercise 3: CQRS - Nouvelle Projection
**Difficulté**: ⭐⭐⭐

Dans `web/cqrs-pattern/bank-account.ts`, créer une projection "AccountAuditLog":
- Liste toutes les transactions avec timestamp
- Calcul du solde à chaque étape
- Filtrage par date
- Export en CSV

**Objectif**: Comprendre comment créer différentes vues des mêmes événements.

## 🔧 C Exercises

### Exercise 4: NASA Rules - Refactoring
**Difficulté**: ⭐⭐

Dans `c/nasa-rules/nasa_rules.c`, trouver et corriger les violations:
```c
// Code à corriger (violations de règles NASA)
int process_data(int *data) {
    int i = 0;
    while (data[i] != -1) {  // Unbounded loop
        int result = compute(data[i]);  // Return not checked
        i++;
    }
    return i;
}
```

**Objectif**: Appliquer les 10 règles NASA.

### Exercise 5: Memory Safety - Arena Allocator
**Difficulté**: ⭐⭐⭐

Étendre `c/memory-safety/memory_safety.c`:
- Ajouter alignement personnalisé à l'arena
- Implémenter arena_realloc()
- Statistiques d'utilisation (peak memory, fragmentation)
- Thread-safety avec mutex

**Objectif**: Maîtriser les patterns d'allocation avancés.

### Exercise 6: Layered Arch - Nouveau Driver
**Difficulté**: ⭐⭐⭐

Dans `c/layered-arch/layered_arch.c`, ajouter un driver LCD:
- Interface HAL I2C
- Driver LCD 16x2
- Service DisplayService pour afficher température
- Intégrer dans Application

**Objectif**: Comprendre l'architecture en couches et son extensibilité.

## 🏆 Advanced Challenges

### Challenge 1: Full Stack System
**Difficulté**: ⭐⭐⭐⭐

Créer un système complet:
- Backend TypeScript avec Clean Architecture + CQRS
- Simulation embedded C avec layered architecture
- Communication via API REST
- Event-driven avec WebSockets

### Challenge 2: Migrate Legacy Code
**Difficulté**: ⭐⭐⭐⭐⭐

Prendre un code legacy (fourni) et le refactorer:
- Identifier les anti-patterns
- Appliquer Clean Architecture
- Séparer en couches
- Ajouter tests unitaires
- Documenter les décisions

## 📝 Solutions

Les solutions sont dans le dossier `solutions/` (à créer par vous-même en suivant les patterns du workshop).

## 🎓 Grading Criteria

- ✅ Respect des principes architecturaux
- ✅ Code compilable et exécutable
- ✅ Tests unitaires
- ✅ Documentation
- ✅ Pas de warnings compiler
- ✅ Respect des conventions

## 💡 Tips

1. Commencez par les exercices ⭐ et ⭐⭐
2. Lisez bien les exemples avant
3. Compilez souvent
4. Utilisez les outils (sanitizers, static analyzers)
5. Demandez de l'aide si bloqué
