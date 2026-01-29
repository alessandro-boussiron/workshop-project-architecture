# Workshop Project Architecture - Cheat Sheet

## 🌐 Clean Architecture

### Structure
```
Domain → Use Cases → Adapters → Frameworks
```

### Règles
- Dépendances pointent vers l'intérieur
- Domaine sans dépendances externes
- Interfaces aux frontières

### Code Pattern
```typescript
// Domain
class User { /* pure business logic */ }

// Use Case
class RegisterUser {
  constructor(private repo: IUserRepository) {}
  async execute(input: Input): Promise<Output> { }
}

// Adapter
class PostgresUserRepository implements IUserRepository { }
```

## 📦 DDD (Domain-Driven Design)

### Concepts Clés
- **Entity**: Identité unique (User, Order)
- **Value Object**: Défini par valeurs (Money, Email)
- **Aggregate**: Cluster cohérent (Order + OrderLines)
- **Domain Event**: Événement métier (OrderPlaced)

### Patterns
```typescript
// Value Object
class Money {
  private constructor(readonly amount: number, readonly currency: string) {}
  static create(amount: number, currency: string): Money
  add(other: Money): Money
}

// Entity
class Order {
  private constructor(
    readonly id: OrderId,
    private lines: OrderLine[]
  ) {}
  
  addLine(line: OrderLine): void { /* business rule */ }
}
```

## ⚡ CQRS + Event Sourcing

### Séparation
- **Commands**: Modifient l'état
- **Queries**: Lisent l'état
- **Events**: Source of truth

### Flow
```
Command → Aggregate → Event → Event Store
                        ↓
                    Projections → Query
```

## 🚀 NASA Power of 10 Rules

1. **Simple control flow** - Pas de goto/recursion
2. **Fixed loop bounds** - Toutes boucles bornées
3. **No dynamic allocation** - Pas malloc après init
4. **Small functions** - < 60 lignes
5. **Check return values** - Toujours vérifier
6. **Limited scope** - Variables au plus petit scope
7. **Check assertions** - `assert()` partout
8. **Limited pointers** - Max 2 niveaux (`**ptr`)
9. **Zero warnings** - `-Wall -Wextra -Werror`
10. **Static analysis** - Coverity, PC-Lint

## 🛡️ Memory Safety Patterns

### Arena Allocator
```c
Arena *arena = arena_create(4096);
void *ptr = arena_alloc(arena, size);
// No individual free
arena_destroy(arena);  // Free all
```

### Object Pool
```c
PoolObject *obj = pool_acquire(&pool);
// Use object
pool_release(&pool, obj);
```

### Cleanup Pattern
```c
int function(void) {
    Resource *r1 = NULL, *r2 = NULL;
    int result = -1;
    
    r1 = acquire1();
    if (!r1) goto cleanup;
    
    r2 = acquire2();
    if (!r2) goto cleanup;
    
    result = 0;
    
cleanup:
    if (r2) free(r2);
    if (r1) free(r1);
    return result;
}
```

## 🏗️ Layered Architecture C

### Couches
```
Application  → High-level logic
Service      → Business coordination
Driver       → Device-specific
HAL          → Hardware abstraction
```

### Pattern
```c
// HAL Interface
typedef struct {
    bool (*init)(void);
    bool (*read)(uint8_t *data);
} SpiInterface;

// Driver uses HAL
typedef struct {
    const SpiInterface *spi;
} SensorDriver;

// Application uses Driver
void app_run(SensorDriver *sensor) {
    uint8_t data;
    sensor->spi->read(&data);
}
```

## 🔧 Compilation Commands

### TypeScript
```bash
npm install
npm start
tsc
```

### C with Safety
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c11 file.c
gcc -fsanitize=address -g file.c
valgrind --leak-check=full ./program
```

## 📊 SOLID Principles

- **S**ingle Responsibility
- **O**pen/Closed (extension, pas modification)
- **L**iskov Substitution
- **I**nterface Segregation
- **D**ependency Inversion

## ⚠️ Anti-Patterns to Avoid

### Web
- ❌ Business logic in controllers
- ❌ Direct DB access from use cases
- ❌ God objects
- ❌ Anemic domain model

### C
- ❌ Global variables
- ❌ Unbounded loops
- ❌ Unchecked return values
- ❌ Deep pointer indirection
- ❌ Magic numbers

## 🎯 Best Practices

### General
- ✅ Small, focused functions
- ✅ Clear naming
- ✅ Comments explain "why", not "what"
- ✅ Tests for critical paths
- ✅ Error handling everywhere

### C Specific
- ✅ `const` correctness
- ✅ Initialize all variables
- ✅ NULL checks before dereference
- ✅ Bounds checking
- ✅ Static allocation when possible

## 📚 Quick References

### TypeScript Types
```typescript
interface IRepository<T> {
  save(entity: T): Promise<void>;
  findById(id: string): Promise<T | null>;
}
```

### C Safe String
```c
char buf[256];
strncpy(buf, src, sizeof(buf) - 1);
buf[sizeof(buf) - 1] = '\0';
```

### C Assertions
```c
assert(ptr != NULL);
assert(index < array_size);
assert(denominator != 0);
```
