# Clean Architecture - Guide Complet

## 📚 Table des Matières

1. [Introduction](#introduction)
2. [Les Principes Fondamentaux](#principes-fondamentaux)
3. [Architecture en Couches](#architecture-en-couches)
4. [Règle de Dépendance](#règle-de-dépendance)
5. [Hexagonal Architecture](#hexagonal-architecture)
6. [Comparaison des Approches](#comparaison-des-approches)
7. [Patterns Associés](#patterns-associés)
8. [Cas d'Usage Pratiques](#cas-dusage-pratiques)
9. [Anti-Patterns à Éviter](#anti-patterns-à-éviter)
10. [Checklist](#checklist)

---

## Introduction

### Qu'est-ce que la Clean Architecture?

La Clean Architecture, popularisée par **Robert C. Martin (Uncle Bob)**, est une approche qui vise à créer des systèmes:

- ✅ **Indépendants des frameworks** - Pas de couplage fort avec Express, NestJS, etc.
- ✅ **Testables** - La logique métier peut être testée sans UI, DB, serveur web
- ✅ **Indépendants de l'UI** - L'UI peut changer sans affecter la logique métier
- ✅ **Indépendants de la DB** - Peut passer de MongoDB à PostgreSQL facilement
- ✅ **Indépendants de toute agency externe** - Les règles métier ne connaissent rien du monde extérieur

### Pourquoi Clean Architecture?

**Problème classique:**
```typescript
// ❌ MAUVAIS - Tout mélangé
class UserController {
  async register(req, res) {
    // Validation dans le controller
    if (!req.body.email) return res.status(400).json({ error: 'Email required' });
    
    // Logique métier dans le controller
    if (req.body.age < 18) return res.status(400).json({ error: 'Too young' });
    
    // Accès direct à la DB dans le controller
    const user = await mongoose.model('User').create({
      email: req.body.email,
      age: req.body.age,
      passwordHash: bcrypt.hashSync(req.body.password)
    });
    
    // Email dans le controller
    await sendgrid.send({ to: user.email, subject: 'Welcome!' });
    
    res.json(user);
  }
}
```

**Problèmes:**
- 🔴 Impossible de tester sans MongoDB
- 🔴 Impossible de tester sans Sendgrid
- 🔴 Couplé à Express (req, res)
- 🔴 Logique métier mélangée avec infrastructure
- 🔴 Impossible de réutiliser cette logique dans un CLI ou une queue

**Solution Clean Architecture:**
```typescript
// ✅ BON - Séparation claire

// 1. DOMAIN - Pure business logic
class User {
  static create(email: string, age: number): User {
    if (age < 18) throw new Error('Must be 18+');
    // ...
  }
}

// 2. USE CASE - Application logic
class RegisterUserUseCase {
  constructor(
    private userRepo: IUserRepository,
    private emailService: IEmailService,
    private passwordService: IPasswordService
  ) {}
  
  async execute(input: RegisterInput): Promise<User> {
    const hash = await this.passwordService.hash(input.password);
    const user = User.create(input.email, input.age);
    await this.userRepo.save(user);
    await this.emailService.sendWelcome(user.email);
    return user;
  }
}

// 3. ADAPTER - Express controller
class UserController {
  constructor(private registerUseCase: RegisterUserUseCase) {}
  
  async register(req, res) {
    try {
      const user = await this.registerUseCase.execute(req.body);
      res.json(user);
    } catch (error) {
      res.status(400).json({ error: error.message });
    }
  }
}
```

**Avantages:**
- ✅ Use case testable avec mocks (pas besoin de DB réelle)
- ✅ Peut être utilisé depuis Express, CLI, GraphQL, gRPC
- ✅ Changement de DB facile (juste changer l'adapter)
- ✅ Changement d'email service facile
- ✅ Logique métier claire et isolée

---

## Principes Fondamentaux

### 1. Separation of Concerns (SoC)

Chaque partie du code a **une seule responsabilité**.

```
┌─────────────────────────────────────────┐
│         PRESENTATION LAYER              │  HTTP, GraphQL, CLI
├─────────────────────────────────────────┤
│         APPLICATION LAYER               │  Use Cases
├─────────────────────────────────────────┤
│           DOMAIN LAYER                  │  Business Logic
├─────────────────────────────────────────┤
│        INFRASTRUCTURE LAYER             │  DB, Email, File System
└─────────────────────────────────────────┘
```

### 2. Dependency Rule

**Les dépendances pointent TOUJOURS vers l'intérieur.**

```
┌───────────────────────────────────────┐
│  Frameworks & Drivers                 │ ─┐
│  (Web, DB, UI)                        │  │
└───────────────────────────────────────┘  │
         ↓                                  │
┌───────────────────────────────────────┐  │
│  Interface Adapters                   │  │  Dépendances
│  (Controllers, Gateways, Presenters)  │  │     vers
└───────────────────────────────────────┘  │  l'intérieur
         ↓                                  │
┌───────────────────────────────────────┐  │
│  Application Business Rules           │  │
│  (Use Cases)                          │  │
└───────────────────────────────────────┘  │
         ↓                                  │
┌───────────────────────────────────────┐  │
│  Enterprise Business Rules            │ ─┘
│  (Entities)                           │
└───────────────────────────────────────┘
```

**Règles:**
- ❌ Le domaine NE DOIT PAS connaître les use cases
- ❌ Les use cases NE DOIVENT PAS connaître les controllers
- ❌ Rien ne doit connaître les détails d'implémentation (DB, framework)

### 3. Dependency Inversion Principle (DIP)

Les abstractions ne dépendent pas des détails. **Les détails dépendent des abstractions.**

```typescript
// ❌ MAUVAIS - Dépendance concrète
class RegisterUser {
  private repo = new MongoUserRepository(); // ❌ Couplage fort!
  
  async execute(data) {
    await this.repo.save(data);
  }
}

// ✅ BON - Dépendance sur abstraction
interface IUserRepository {
  save(user: User): Promise<void>;
}

class RegisterUser {
  constructor(private repo: IUserRepository) {} // ✅ Interface!
  
  async execute(data) {
    await this.repo.save(data);
  }
}

// Implémentations concrètes
class MongoUserRepository implements IUserRepository { /* ... */ }
class PostgresUserRepository implements IUserRepository { /* ... */ }
class InMemoryUserRepository implements IUserRepository { /* ... */ }
```

---

## Architecture en Couches

### Les 4 Couches

#### 1. Domain Layer (Entities)

**C'est le cœur du système** - Pure business logic, zéro dépendance externe.

```typescript
// domain/entities/User.ts
export class User {
  private constructor(
    public readonly id: UserId,
    public readonly email: Email,
    public readonly name: string,
    private passwordHash: string,
    public readonly role: UserRole,
    public readonly isActive: boolean
  ) {}

  // Factory method avec validation métier
  static create(email: string, name: string, password: string): User {
    // Business rules
    if (name.length < 2) throw new DomainError('Name too short');
    if (password.length < 8) throw new DomainError('Password too weak');
    
    return new User(
      UserId.generate(),
      Email.create(email),
      name,
      this.hashPassword(password),
      UserRole.STANDARD,
      true
    );
  }

  // Business logic methods
  changePassword(oldPassword: string, newPassword: string): User {
    if (!this.verifyPassword(oldPassword)) {
      throw new DomainError('Invalid old password');
    }
    
    return new User(
      this.id,
      this.email,
      this.name,
      User.hashPassword(newPassword),
      this.role,
      this.isActive
    );
  }

  promoteToAdmin(): User {
    if (!this.isActive) {
      throw new DomainError('Cannot promote inactive user');
    }
    
    return new User(
      this.id,
      this.email,
      this.name,
      this.passwordHash,
      UserRole.ADMIN,
      this.isActive
    );
  }

  deactivate(): User {
    return new User(
      this.id,
      this.email,
      this.name,
      this.passwordHash,
      this.role,
      false
    );
  }

  private verifyPassword(password: string): boolean {
    return this.passwordHash === User.hashPassword(password);
  }

  private static hashPassword(password: string): string {
    // Simple hash for example - use bcrypt in production
    return `hashed_${password}`;
  }
}

// Value Objects
export class UserId {
  private constructor(public readonly value: string) {}
  
  static generate(): UserId {
    return new UserId(`user_${Date.now()}_${Math.random()}`);
  }
  
  static fromString(id: string): UserId {
    if (!id.startsWith('user_')) throw new Error('Invalid UserId');
    return new UserId(id);
  }
}

export class Email {
  private constructor(public readonly value: string) {}
  
  static create(email: string): Email {
    if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email)) {
      throw new DomainError('Invalid email format');
    }
    return new Email(email.toLowerCase());
  }
}

export enum UserRole {
  STANDARD = 'standard',
  ADMIN = 'admin',
  MODERATOR = 'moderator'
}

export class DomainError extends Error {
  constructor(message: string) {
    super(message);
    this.name = 'DomainError';
  }
}
```

**Caractéristiques:**
- ✅ Zéro import externe (pas de Express, Mongoose, etc.)
- ✅ Pure TypeScript/JavaScript
- ✅ Testable avec des tests unitaires simples
- ✅ Contient TOUTE la logique métier

#### 2. Application Layer (Use Cases)

**Orchestration de la logique applicative** - Coordonne les entités et services.

```typescript
// application/use-cases/RegisterUser.ts
export class RegisterUserUseCase {
  constructor(
    private userRepository: IUserRepository,
    private emailService: IEmailService,
    private eventBus: IEventBus
  ) {}

  async execute(input: RegisterUserInput): Promise<RegisterUserOutput> {
    // 1. Vérifier que l'email n'existe pas
    const existingUser = await this.userRepository.findByEmail(input.email);
    if (existingUser) {
      throw new ApplicationError('Email already registered');
    }

    // 2. Créer l'entité (applique les règles métier)
    const user = User.create(input.email, input.name, input.password);

    // 3. Persister
    await this.userRepository.save(user);

    // 4. Envoyer email de bienvenue
    await this.emailService.sendWelcomeEmail(user.email.value, user.name);

    // 5. Publier événement
    await this.eventBus.publish({
      type: 'UserRegistered',
      userId: user.id.value,
      email: user.email.value,
      occurredAt: new Date()
    });

    return {
      userId: user.id.value,
      email: user.email.value,
      name: user.name
    };
  }
}

export interface RegisterUserInput {
  email: string;
  name: string;
  password: string;
}

export interface RegisterUserOutput {
  userId: string;
  email: string;
  name: string;
}

// application/use-cases/ChangePassword.ts
export class ChangePasswordUseCase {
  constructor(private userRepository: IUserRepository) {}

  async execute(input: ChangePasswordInput): Promise<void> {
    // 1. Récupérer l'utilisateur
    const user = await this.userRepository.findById(input.userId);
    if (!user) {
      throw new ApplicationError('User not found');
    }

    // 2. Appliquer la logique métier (dans l'entité)
    const updatedUser = user.changePassword(input.oldPassword, input.newPassword);

    // 3. Persister
    await this.userRepository.save(updatedUser);
  }
}

export interface ChangePasswordInput {
  userId: string;
  oldPassword: string;
  newPassword: string;
}
```

**Caractéristiques:**
- ✅ Coordonne les entités et services
- ✅ Ne contient PAS de logique métier (elle est dans les entités)
- ✅ Dépend des interfaces (ports), pas des implémentations
- ✅ Un use case = une action utilisateur

#### 3. Interface Adapters Layer

**Convertit les données** entre format externe (HTTP, DB) et format interne (domain).

```typescript
// adapters/http/UserController.ts
export class UserController {
  constructor(
    private registerUseCase: RegisterUserUseCase,
    private changePasswordUseCase: ChangePasswordUseCase,
    private getUserUseCase: GetUserUseCase
  ) {}

  async register(req: Request, res: Response): Promise<void> {
    try {
      const result = await this.registerUseCase.execute({
        email: req.body.email,
        name: req.body.name,
        password: req.body.password
      });
      
      res.status(201).json(result);
    } catch (error) {
      if (error instanceof ApplicationError) {
        res.status(400).json({ error: error.message });
      } else {
        res.status(500).json({ error: 'Internal server error' });
      }
    }
  }

  async changePassword(req: Request, res: Response): Promise<void> {
    try {
      await this.changePasswordUseCase.execute({
        userId: req.params.userId,
        oldPassword: req.body.oldPassword,
        newPassword: req.body.newPassword
      });
      
      res.status(204).send();
    } catch (error) {
      if (error instanceof ApplicationError) {
        res.status(400).json({ error: error.message });
      } else {
        res.status(500).json({ error: 'Internal server error' });
      }
    }
  }
}

// adapters/repositories/MongoUserRepository.ts
export class MongoUserRepository implements IUserRepository {
  constructor(private db: MongoDatabase) {}

  async save(user: User): Promise<void> {
    // Convert domain entity to DB model
    await this.db.collection('users').updateOne(
      { _id: user.id.value },
      {
        $set: {
          email: user.email.value,
          name: user.name,
          passwordHash: user['passwordHash'], // Private field access
          role: user.role,
          isActive: user.isActive,
          updatedAt: new Date()
        }
      },
      { upsert: true }
    );
  }

  async findById(id: string): Promise<User | null> {
    const doc = await this.db.collection('users').findOne({ _id: id });
    if (!doc) return null;
    
    // Convert DB model to domain entity
    return this.toDomain(doc);
  }

  async findByEmail(email: string): Promise<User | null> {
    const doc = await this.db.collection('users').findOne({ 
      email: email.toLowerCase() 
    });
    if (!doc) return null;
    
    return this.toDomain(doc);
  }

  private toDomain(doc: any): User {
    // Reconstruct domain entity from DB document
    return User.reconstruct({
      id: UserId.fromString(doc._id),
      email: Email.create(doc.email),
      name: doc.name,
      passwordHash: doc.passwordHash,
      role: doc.role,
      isActive: doc.isActive
    });
  }
}
```

#### 4. Frameworks & Drivers Layer

**Infrastructure technique** - Express, MongoDB, etc.

```typescript
// infrastructure/web/app.ts
import express from 'express';
import { UserController } from '../../adapters/http/UserController';
import { RegisterUserUseCase } from '../../application/use-cases/RegisterUser';
import { MongoUserRepository } from '../../adapters/repositories/MongoUserRepository';
import { SendgridEmailService } from '../../adapters/email/SendgridEmailService';

// Dependency Injection
const userRepository = new MongoUserRepository(mongoDb);
const emailService = new SendgridEmailService(sendgridClient);
const registerUseCase = new RegisterUserUseCase(userRepository, emailService);
const userController = new UserController(registerUseCase);

// Routes
const app = express();
app.post('/users/register', (req, res) => userController.register(req, res));
app.put('/users/:userId/password', (req, res) => userController.changePassword(req, res));
```

---

## Règle de Dépendance

### Visualisation

```
       Outer Layer                              Inner Layer
┌─────────────────┐                       ┌─────────────────┐
│   Express       │───────────────────────│  Use Cases      │
│   Controller    │  Depends on (→)       │                 │
└─────────────────┘                       └─────────────────┘
                                                   │
                                                   │ Depends on (→)
                                                   ↓
                                          ┌─────────────────┐
                                          │   Domain        │
                                          │   Entities      │
                                          └─────────────────┘
```

### Comment Inverser les Dépendances?

**Problème:** Le use case a besoin d'envoyer un email, mais ne doit pas dépendre de Sendgrid.

**Solution:** Dependency Inversion via interfaces.

```typescript
// ❌ MAUVAIS - Use case dépend de l'implémentation
import { SendgridClient } from 'sendgrid-library'; // ❌ Dépendance externe!

class RegisterUser {
  async execute(data) {
    // ...
    await SendgridClient.send({ to: data.email, subject: 'Welcome' }); // ❌
  }
}

// ✅ BON - Use case dépend d'une interface
// application/ports/IEmailService.ts (dans le layer application)
export interface IEmailService {
  sendWelcomeEmail(email: string, name: string): Promise<void>;
}

// application/use-cases/RegisterUser.ts
class RegisterUser {
  constructor(private emailService: IEmailService) {} // ✅ Interface!
  
  async execute(data) {
    // ...
    await this.emailService.sendWelcomeEmail(data.email, data.name); // ✅
  }
}

// adapters/email/SendgridEmailService.ts (dans le layer adapters)
import { SendgridClient } from 'sendgrid-library'; // OK ici!

export class SendgridEmailService implements IEmailService {
  constructor(private client: SendgridClient) {}
  
  async sendWelcomeEmail(email: string, name: string): Promise<void> {
    await this.client.send({
      to: email,
      subject: 'Welcome',
      html: `<h1>Hello ${name}!</h1>`
    });
  }
}
```

**Résultat:**
- ✅ Le use case ne connaît pas Sendgrid
- ✅ Peut facilement passer à Mailgun, AWS SES, etc.
- ✅ Testable avec un mock simple

---

## Hexagonal Architecture

### Concept

Aussi appelée **Ports & Adapters**, c'est une autre vision de Clean Architecture.

```
                  ┌─────────────────────────────────┐
                  │                                 │
     Adapter      │                                 │      Adapter
    (HTTP/REST)   │          APPLICATION            │    (Database)
  ┌──────────┐    │         ┌──────────┐            │   ┌──────────┐
  │Controller│────┼────────►│Use Cases │────────────┼──►│Repository│
  └──────────┘    │         └──────────┘            │   └──────────┘
                  │              │                  │
     Adapter      │              │                  │     Adapter
    (GraphQL)     │              ↓                  │    (Email)
  ┌──────────┐    │         ┌──────────┐            │   ┌──────────┐
  │ Resolver │────┼────────►│  Domain  │            │   │  SMTP    │
  └──────────┘    │         │ Entities │────────────┼──►│ Service  │
                  │         └──────────┘            │   └──────────┘
     Adapter      │                                 │
      (CLI)       │                                 │     Adapter
  ┌──────────┐    │                                 │   (File System)
  │ Command  │────┼─────────────────────────────────┼──►│   Disk   │
  └──────────┘    │                                 │   └──────────┘
                  │                                 │
                  └─────────────────────────────────┘
                         HEXAGONE (Application)
```

### Ports vs Adapters

**Port** = Interface (contrat)
**Adapter** = Implémentation concrète

```typescript
// Port (Primary - driving)
interface IUserService {
  registerUser(data: RegisterData): Promise<User>;
}

// Port (Secondary - driven)
interface IUserRepository {
  save(user: User): Promise<void>;
  findByEmail(email: string): Promise<User | null>;
}

// Adapter Primary (driving) - HTTP
class HttpUserController {
  constructor(private userService: IUserService) {}
  
  async register(req, res) {
    const user = await this.userService.registerUser(req.body);
    res.json(user);
  }
}

// Adapter Primary (driving) - CLI
class CliUserCommand {
  constructor(private userService: IUserService) {}
  
  async execute(args: string[]) {
    const user = await this.userService.registerUser({
      email: args[0],
      name: args[1],
      password: args[2]
    });
    console.log(`User created: ${user.id}`);
  }
}

// Adapter Secondary (driven) - MongoDB
class MongoUserRepository implements IUserRepository {
  async save(user: User): Promise<void> { /* ... */ }
  async findByEmail(email: string): Promise<User | null> { /* ... */ }
}

// Adapter Secondary (driven) - In-Memory (tests)
class InMemoryUserRepository implements IUserRepository {
  private users: User[] = [];
  
  async save(user: User): Promise<void> {
    this.users.push(user);
  }
  
  async findByEmail(email: string): Promise<User | null> {
    return this.users.find(u => u.email === email) || null;
  }
}
```

---

## Comparaison des Approches

### Clean Architecture vs Layered Architecture

| Aspect | Layered (Traditionnel) | Clean Architecture |
|--------|------------------------|-------------------|
| **Dépendances** | Chaque couche dépend de celle du dessous | Tout dépend du domaine |
| **DB** | Couche Data Access en bas | DB est un détail externe |
| **Testabilité** | Difficile (dépend de la DB) | Facile (domaine isolé) |
| **Flexibility** | Couplage fort | Découplage maximal |
| **Complexité** | Simple au début | Plus de boilerplate |

**Layered traditionnel:**
```
┌──────────────┐
│ Presentation │ (Controllers)
├──────────────┤
│   Business   │ (Services)
├──────────────┤
│ Data Access  │ (Repositories)
├──────────────┤
│   Database   │ (MongoDB/Postgres)
└──────────────┘
      ↓ Dépendances vers le bas
```

**Clean Architecture:**
```
        ┌──────────────┐
        │   Domain     │ (Centre, zéro dépendance)
        └──────────────┘
               ↑
        ┌──────────────┐
        │  Use Cases   │
        └──────────────┘
               ↑
        ┌──────────────┐
        │   Adapters   │
        └──────────────┘
               ↑
        ┌──────────────┐
        │ Frameworks   │
        └──────────────┘
      Dépendances vers le centre
```

### Quand utiliser Clean Architecture?

**✅ Utiliser quand:**
- Projet long terme (> 1 an)
- Équipe multiple (> 3 personnes)
- Logique métier complexe
- Besoins de testabilité élevés
- Plusieurs interfaces (API + CLI + GraphQL)
- Changements fréquents de technologie

**❌ Ne pas utiliser quand:**
- Prototype/MVP rapide
- CRUD simple sans logique métier
- Projet court terme (< 3 mois)
- Équipe solo ou très petite
- Contraintes de temps fortes

---

## Patterns Associés

### 1. Repository Pattern

**But:** Abstraire l'accès aux données.

```typescript
// Port
interface IUserRepository {
  save(user: User): Promise<void>;
  findById(id: string): Promise<User | null>;
  findAll(): Promise<User[]>;
  delete(id: string): Promise<void>;
}

// Adapter MongoDB
class MongoUserRepository implements IUserRepository {
  constructor(private db: Db) {}
  
  async save(user: User): Promise<void> {
    await this.db.collection('users').insertOne({
      _id: user.id,
      email: user.email,
      // ...
    });
  }
  
  async findById(id: string): Promise<User | null> {
    const doc = await this.db.collection('users').findOne({ _id: id });
    return doc ? this.toDomain(doc) : null;
  }
  
  // ...
}

// Adapter Postgres
class PostgresUserRepository implements IUserRepository {
  constructor(private pool: Pool) {}
  
  async save(user: User): Promise<void> {
    await this.pool.query(
      'INSERT INTO users (id, email, ...) VALUES ($1, $2, ...)',
      [user.id, user.email, ...]
    );
  }
  
  // ...
}
```

### 2. Dependency Injection

**But:** Fournir les dépendances de l'extérieur.

```typescript
// Manual DI
const userRepository = new MongoUserRepository(db);
const emailService = new SendgridEmailService(sendgridClient);
const registerUseCase = new RegisterUserUseCase(userRepository, emailService);
const userController = new UserController(registerUseCase);

// Avec container DI (ex: tsyringe, inversify)
import { container } from 'tsyringe';

container.register('IUserRepository', { useClass: MongoUserRepository });
container.register('IEmailService', { useClass: SendgridEmailService });
container.register(RegisterUserUseCase, { useClass: RegisterUserUseCase });
container.register(UserController, { useClass: UserController });

const userController = container.resolve(UserController);
```

### 3. Factory Pattern

**But:** Créer des entités complexes.

```typescript
// Factory pour reconstruire depuis DB
class UserFactory {
  static reconstitute(data: UserData): User {
    return new User(
      UserId.fromString(data.id),
      Email.create(data.email),
      data.name,
      data.passwordHash,
      data.role,
      data.isActive
    );
  }
  
  static create(email: string, name: string, password: string): User {
    return User.create(email, name, password);
  }
}
```

### 4. DTO (Data Transfer Object)

**But:** Transférer les données entre couches.

```typescript
// Input DTO (vient du controller)
export interface RegisterUserDTO {
  email: string;
  name: string;
  password: string;
}

// Output DTO (va au controller)
export interface UserDTO {
  id: string;
  email: string;
  name: string;
  role: string;
}

// Mapper
class UserMapper {
  static toDTO(user: User): UserDTO {
    return {
      id: user.id.value,
      email: user.email.value,
      name: user.name,
      role: user.role
    };
  }
}
```

---

## Cas d'Usage Pratiques

### Exemple 1: E-commerce - Passer une commande

```typescript
// DOMAIN
class Order {
  private constructor(
    public readonly id: OrderId,
    public readonly customerId: CustomerId,
    private items: OrderItem[],
    private status: OrderStatus
  ) {}

  static create(customerId: CustomerId, items: OrderItem[]): Order {
    if (items.length === 0) {
      throw new DomainError('Order must have at least one item');
    }
    
    return new Order(
      OrderId.generate(),
      customerId,
      items,
      OrderStatus.PENDING
    );
  }

  getTotalAmount(): Money {
    return this.items.reduce(
      (total, item) => total.add(item.getTotalPrice()),
      Money.zero('EUR')
    );
  }

  confirm(): Order {
    if (this.status !== OrderStatus.PENDING) {
      throw new DomainError('Only pending orders can be confirmed');
    }
    
    return new Order(this.id, this.customerId, this.items, OrderStatus.CONFIRMED);
  }
}

class OrderItem {
  constructor(
    public readonly productId: ProductId,
    public readonly quantity: number,
    public readonly unitPrice: Money
  ) {
    if (quantity <= 0) throw new DomainError('Quantity must be positive');
  }

  getTotalPrice(): Money {
    return this.unitPrice.multiply(this.quantity);
  }
}

// USE CASE
class PlaceOrderUseCase {
  constructor(
    private orderRepository: IOrderRepository,
    private productRepository: IProductRepository,
    private paymentService: IPaymentService,
    private inventoryService: IInventoryService
  ) {}

  async execute(input: PlaceOrderInput): Promise<PlaceOrderOutput> {
    // 1. Vérifier stock
    for (const item of input.items) {
      const hasStock = await this.inventoryService.checkStock(
        item.productId,
        item.quantity
      );
      if (!hasStock) {
        throw new ApplicationError(`Insufficient stock for product ${item.productId}`);
      }
    }

    // 2. Récupérer les produits pour obtenir les prix
    const orderItems: OrderItem[] = [];
    for (const item of input.items) {
      const product = await this.productRepository.findById(item.productId);
      if (!product) {
        throw new ApplicationError(`Product not found: ${item.productId}`);
      }
      
      orderItems.push(new OrderItem(
        ProductId.fromString(item.productId),
        item.quantity,
        product.price
      ));
    }

    // 3. Créer la commande
    const order = Order.create(
      CustomerId.fromString(input.customerId),
      orderItems
    );

    // 4. Processus de paiement
    const paymentResult = await this.paymentService.charge(
      input.customerId,
      order.getTotalAmount()
    );

    if (!paymentResult.success) {
      throw new ApplicationError('Payment failed');
    }

    // 5. Confirmer la commande
    const confirmedOrder = order.confirm();

    // 6. Réserver le stock
    for (const item of orderItems) {
      await this.inventoryService.reserveStock(
        item.productId.value,
        item.quantity
      );
    }

    // 7. Persister
    await this.orderRepository.save(confirmedOrder);

    return {
      orderId: confirmedOrder.id.value,
      totalAmount: confirmedOrder.getTotalAmount().amount,
      status: 'confirmed'
    };
  }
}
```

### Exemple 2: Blog - Publier un article

```typescript
// DOMAIN
class Article {
  private constructor(
    public readonly id: ArticleId,
    public readonly authorId: UserId,
    public readonly title: string,
    public readonly content: string,
    public readonly status: ArticleStatus,
    public readonly publishedAt: Date | null,
    public readonly tags: Tag[]
  ) {}

  static create(authorId: UserId, title: string, content: string): Article {
    if (title.length < 5) throw new DomainError('Title too short');
    if (content.length < 100) throw new DomainError('Content too short');
    
    return new Article(
      ArticleId.generate(),
      authorId,
      title,
      content,
      ArticleStatus.DRAFT,
      null,
      []
    );
  }

  publish(): Article {
    if (this.status === ArticleStatus.PUBLISHED) {
      throw new DomainError('Article already published');
    }
    
    return new Article(
      this.id,
      this.authorId,
      this.title,
      this.content,
      ArticleStatus.PUBLISHED,
      new Date(),
      this.tags
    );
  }

  addTag(tag: Tag): Article {
    if (this.tags.some(t => t.equals(tag))) {
      throw new DomainError('Tag already exists');
    }
    
    return new Article(
      this.id,
      this.authorId,
      this.title,
      this.content,
      this.status,
      this.publishedAt,
      [...this.tags, tag]
    );
  }
}

// USE CASE
class PublishArticleUseCase {
  constructor(
    private articleRepository: IArticleRepository,
    private eventBus: IEventBus,
    private searchIndexer: ISearchIndexer
  ) {}

  async execute(input: PublishArticleInput): Promise<void> {
    // 1. Récupérer l'article
    const article = await this.articleRepository.findById(input.articleId);
    if (!article) throw new ApplicationError('Article not found');

    // 2. Vérifier ownership
    if (article.authorId.value !== input.userId) {
      throw new ApplicationError('Unauthorized');
    }

    // 3. Publier (logique métier)
    const publishedArticle = article.publish();

    // 4. Persister
    await this.articleRepository.save(publishedArticle);

    // 5. Indexer pour recherche
    await this.searchIndexer.index({
      id: publishedArticle.id.value,
      title: publishedArticle.title,
      content: publishedArticle.content,
      tags: publishedArticle.tags.map(t => t.value)
    });

    // 6. Publier événement
    await this.eventBus.publish({
      type: 'ArticlePublished',
      articleId: publishedArticle.id.value,
      authorId: publishedArticle.authorId.value,
      occurredAt: new Date()
    });
  }
}
```

---

## Anti-Patterns à Éviter

### 1. ❌ Anemic Domain Model

**Problème:** Entités sans comportement (juste des getters/setters).

```typescript
// ❌ MAUVAIS - Anemic
class User {
  public id: string;
  public email: string;
  public password: string;
  
  // Pas de logique métier!
}

class UserService {
  register(email: string, password: string) {
    // Toute la logique est dans le service
    const user = new User();
    user.id = generateId();
    user.email = email.toLowerCase();
    
    if (password.length < 8) throw new Error('Weak password');
    user.password = hash(password);
    
    return user;
  }
}

// ✅ BON - Rich Domain Model
class User {
  private constructor(
    public readonly id: string,
    public readonly email: Email,
    private passwordHash: string
  ) {}

  static create(email: string, password: string): User {
    if (password.length < 8) throw new DomainError('Weak password');
    
    return new User(
      generateId(),
      Email.create(email),
      hash(password)
    );
  }

  changePassword(oldPassword: string, newPassword: string): User {
    // Logique dans l'entité!
  }
}
```

### 2. ❌ God Class/Use Case

**Problème:** Un use case fait trop de choses.

```typescript
// ❌ MAUVAIS
class UserService {
  register() { /* ... */ }
  login() { /* ... */ }
  changePassword() { /* ... */ }
  updateProfile() { /* ... */ }
  deactivate() { /* ... */ }
  sendResetEmail() { /* ... */ }
  // ... 20 autres méthodes
}

// ✅ BON - Un use case = une action
class RegisterUserUseCase { /* ... */ }
class LoginUserUseCase { /* ... */ }
class ChangePasswordUseCase { /* ... */ }
class UpdateProfileUseCase { /* ... */ }
class DeactivateUserUseCase { /* ... */ }
```

### 3. ❌ Violation de la Dependency Rule

```typescript
// ❌ MAUVAIS - Domaine dépend d'Express
import { Request } from 'express'; // ❌

class User {
  static fromRequest(req: Request): User {
    return new User(req.body.email, req.body.name);
  }
}

// ✅ BON - Domaine indépendant
class User {
  static create(email: string, name: string): User {
    return new User(email, name);
  }
}

// Controller fait la conversion
class UserController {
  register(req: Request, res: Response) {
    const user = User.create(req.body.email, req.body.name);
    // ...
  }
}
```

### 4. ❌ Repository qui retourne des DTOs

```typescript
// ❌ MAUVAIS
interface IUserRepository {
  findById(id: string): Promise<UserDTO>; // ❌ DTO!
}

// ✅ BON
interface IUserRepository {
  findById(id: string): Promise<User>; // ✅ Entity!
}
```

---

## Checklist

### ✅ Structure de Projet

```
src/
├── domain/              # Aucune dépendance externe
│   ├── entities/
│   │   ├── User.ts
│   │   └── Order.ts
│   ├── value-objects/
│   │   ├── Email.ts
│   │   ├── Money.ts
│   │   └── UserId.ts
│   └── errors/
│       └── DomainError.ts
│
├── application/         # Dépend uniquement du domain
│   ├── use-cases/
│   │   ├── RegisterUser.ts
│   │   ├── PlaceOrder.ts
│   │   └── PublishArticle.ts
│   ├── ports/          # Interfaces
│   │   ├── IUserRepository.ts
│   │   ├── IEmailService.ts
│   │   └── IPaymentService.ts
│   └── errors/
│       └── ApplicationError.ts
│
├── adapters/           # Implémentations
│   ├── http/
│   │   └── UserController.ts
│   ├── repositories/
│   │   ├── MongoUserRepository.ts
│   │   └── PostgresUserRepository.ts
│   ├── email/
│   │   └── SendgridEmailService.ts
│   └── payment/
│       └── StripePaymentService.ts
│
├── infrastructure/     # Configuration
│   ├── web/
│   │   ├── app.ts
│   │   └── routes.ts
│   ├── database/
│   │   └── mongo.ts
│   └── di/
│       └── container.ts
│
└── tests/
    ├── unit/          # Domain + Use Cases
    ├── integration/   # Avec DB réelle
    └── e2e/           # End-to-end
```

### ✅ Vérifications

**Domain Layer:**
- [ ] Aucun import externe (pas de Express, DB, etc.)
- [ ] Toutes les règles métier sont dans les entités
- [ ] Entités immutables (retournent de nouvelles instances)
- [ ] Value Objects utilisés pour concepts métier
- [ ] Factory methods pour création complexe

**Application Layer:**
- [ ] Use cases dépendent d'interfaces (ports)
- [ ] Un use case = une action utilisateur
- [ ] Orchestration pure (pas de logique métier)
- [ ] Gestion des erreurs applicatives

**Adapters Layer:**
- [ ] Implémentent les interfaces du domain
- [ ] Conversion entre domain et infrastructure
- [ ] Peuvent dépendre de librairies externes

**Infrastructure Layer:**
- [ ] Configuration et wiring
- [ ] Dependency injection
- [ ] Pas de logique métier

### ✅ Tests

```typescript
// Domain - Tests unitaires simples
describe('User', () => {
  it('should create valid user', () => {
    const user = User.create('test@example.com', 'John', 'password123');
    expect(user.email.value).toBe('test@example.com');
  });

  it('should reject weak password', () => {
    expect(() => User.create('test@example.com', 'John', '123'))
      .toThrow('Password too weak');
  });
});

// Use Case - Tests avec mocks
describe('RegisterUserUseCase', () => {
  it('should register new user', async () => {
    const mockRepo = {
      save: jest.fn(),
      findByEmail: jest.fn().mockResolvedValue(null)
    };
    const mockEmail = { sendWelcomeEmail: jest.fn() };
    
    const useCase = new RegisterUserUseCase(mockRepo, mockEmail);
    
    await useCase.execute({
      email: 'test@example.com',
      name: 'John',
      password: 'password123'
    });
    
    expect(mockRepo.save).toHaveBeenCalled();
    expect(mockEmail.sendWelcomeEmail).toHaveBeenCalled();
  });
});

// Integration - Avec DB réelle
describe('MongoUserRepository', () => {
  let db: Db;
  
  beforeAll(async () => {
    db = await connectToTestDatabase();
  });
  
  it('should save and retrieve user', async () => {
    const repo = new MongoUserRepository(db);
    const user = User.create('test@example.com', 'John', 'password123');
    
    await repo.save(user);
    const found = await repo.findById(user.id.value);
    
    expect(found?.email.value).toBe('test@example.com');
  });
});
```

---

## 📚 Ressources

- **Clean Architecture** by Robert C. Martin
- **Domain-Driven Design** by Eric Evans
- **Hexagonal Architecture** by Alistair Cockburn
- **Screaming Architecture** blog post by Uncle Bob

---

**Prochaine étape:** Voir les exercices pratiques dans `WEB_CLEAN_ARCHITECTURE_EXERCISES.md`
