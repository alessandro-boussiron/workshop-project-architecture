# Clean Architecture - Exercices Pratiques

## 📋 Table des Matières

1. [Exercice 1: Task Manager (Débutant)](#exercice-1-task-manager)
2. [Exercice 2: Blog System (Intermédiaire)](#exercice-2-blog-system)
3. [Exercice 3: E-commerce Cart (Intermédiaire)](#exercice-3-e-commerce-cart)
4. [Exercice 4: Booking System (Avancé)](#exercice-4-booking-system)
5. [Exercice 5: Social Network (Avancé)](#exercice-5-social-network)
6. [Exercice 6: Refactoring Legacy Code (Expert)](#exercice-6-refactoring-legacy-code)

---

## Exercice 1: Task Manager

### 🎯 Objectif
Créer un système de gestion de tâches avec Clean Architecture.

### 📝 Fonctionnalités
- Créer une tâche
- Marquer comme complétée
- Assigner à un utilisateur
- Filtrer par statut

### 🏗️ Structure Attendue

```
src/
├── domain/
│   ├── entities/
│   │   └── Task.ts
│   └── value-objects/
│       ├── TaskId.ts
│       ├── TaskStatus.ts
│       └── UserId.ts
├── application/
│   ├── use-cases/
│   │   ├── CreateTask.ts
│   │   ├── CompleteTask.ts
│   │   └── AssignTask.ts
│   └── ports/
│       └── ITaskRepository.ts
├── adapters/
│   ├── http/
│   │   └── TaskController.ts
│   └── repositories/
│       └── InMemoryTaskRepository.ts
└── infrastructure/
    └── web/
        └── app.ts
```

### 📚 Étape par Étape

#### Étape 1: Domain Layer

```typescript
// domain/value-objects/TaskStatus.ts
export enum TaskStatus {
  TODO = 'TODO',
  IN_PROGRESS = 'IN_PROGRESS',
  DONE = 'DONE'
}

// domain/value-objects/TaskId.ts
export class TaskId {
  private constructor(public readonly value: string) {}
  
  static generate(): TaskId {
    return new TaskId(`task_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`);
  }
  
  static fromString(id: string): TaskId {
    if (!id.startsWith('task_')) {
      throw new Error('Invalid TaskId format');
    }
    return new TaskId(id);
  }
  
  equals(other: TaskId): boolean {
    return this.value === other.value;
  }
}

// domain/entities/Task.ts
export class Task {
  private constructor(
    public readonly id: TaskId,
    public readonly title: string,
    public readonly description: string,
    public readonly status: TaskStatus,
    public readonly assignedTo: UserId | null,
    public readonly createdAt: Date,
    public readonly completedAt: Date | null
  ) {}

  static create(title: string, description: string): Task {
    // Business rules
    if (title.trim().length === 0) {
      throw new Error('Title cannot be empty');
    }
    if (title.length > 100) {
      throw new Error('Title too long (max 100 characters)');
    }
    
    return new Task(
      TaskId.generate(),
      title.trim(),
      description.trim(),
      TaskStatus.TODO,
      null,
      new Date(),
      null
    );
  }

  complete(): Task {
    if (this.status === TaskStatus.DONE) {
      throw new Error('Task is already completed');
    }
    
    return new Task(
      this.id,
      this.title,
      this.description,
      TaskStatus.DONE,
      this.assignedTo,
      this.createdAt,
      new Date()
    );
  }

  assignTo(userId: UserId): Task {
    if (this.status === TaskStatus.DONE) {
      throw new Error('Cannot assign completed task');
    }
    
    return new Task(
      this.id,
      this.title,
      this.description,
      TaskStatus.IN_PROGRESS,
      userId,
      this.createdAt,
      this.completedAt
    );
  }

  unassign(): Task {
    return new Task(
      this.id,
      this.title,
      this.description,
      TaskStatus.TODO,
      null,
      this.createdAt,
      this.completedAt
    );
  }
}
```

#### Étape 2: Application Layer

```typescript
// application/ports/ITaskRepository.ts
export interface ITaskRepository {
  save(task: Task): Promise<void>;
  findById(id: string): Promise<Task | null>;
  findAll(): Promise<Task[]>;
  findByStatus(status: TaskStatus): Promise<Task[]>;
  delete(id: string): Promise<void>;
}

// application/use-cases/CreateTask.ts
export class CreateTaskUseCase {
  constructor(private taskRepository: ITaskRepository) {}

  async execute(input: CreateTaskInput): Promise<CreateTaskOutput> {
    // 1. Créer l'entité (applique les règles métier)
    const task = Task.create(input.title, input.description);

    // 2. Persister
    await this.taskRepository.save(task);

    return {
      taskId: task.id.value,
      title: task.title,
      status: task.status
    };
  }
}

export interface CreateTaskInput {
  title: string;
  description: string;
}

export interface CreateTaskOutput {
  taskId: string;
  title: string;
  status: TaskStatus;
}

// application/use-cases/CompleteTask.ts
export class CompleteTaskUseCase {
  constructor(private taskRepository: ITaskRepository) {}

  async execute(input: CompleteTaskInput): Promise<void> {
    const task = await this.taskRepository.findById(input.taskId);
    if (!task) {
      throw new Error('Task not found');
    }

    const completedTask = task.complete();
    await this.taskRepository.save(completedTask);
  }
}

export interface CompleteTaskInput {
  taskId: string;
}

// application/use-cases/AssignTask.ts
export class AssignTaskUseCase {
  constructor(private taskRepository: ITaskRepository) {}

  async execute(input: AssignTaskInput): Promise<void> {
    const task = await this.taskRepository.findById(input.taskId);
    if (!task) {
      throw new Error('Task not found');
    }

    const userId = UserId.fromString(input.userId);
    const assignedTask = task.assignTo(userId);
    
    await this.taskRepository.save(assignedTask);
  }
}

export interface AssignTaskInput {
  taskId: string;
  userId: string;
}
```

#### Étape 3: Adapters Layer

```typescript
// adapters/repositories/InMemoryTaskRepository.ts
export class InMemoryTaskRepository implements ITaskRepository {
  private tasks: Map<string, Task> = new Map();

  async save(task: Task): Promise<void> {
    this.tasks.set(task.id.value, task);
  }

  async findById(id: string): Promise<Task | null> {
    return this.tasks.get(id) || null;
  }

  async findAll(): Promise<Task[]> {
    return Array.from(this.tasks.values());
  }

  async findByStatus(status: TaskStatus): Promise<Task[]> {
    return Array.from(this.tasks.values()).filter(t => t.status === status);
  }

  async delete(id: string): Promise<void> {
    this.tasks.delete(id);
  }
}

// adapters/http/TaskController.ts
import { Request, Response } from 'express';

export class TaskController {
  constructor(
    private createTaskUseCase: CreateTaskUseCase,
    private completeTaskUseCase: CompleteTaskUseCase,
    private assignTaskUseCase: AssignTaskUseCase,
    private taskRepository: ITaskRepository
  ) {}

  async createTask(req: Request, res: Response): Promise<void> {
    try {
      const result = await this.createTaskUseCase.execute({
        title: req.body.title,
        description: req.body.description
      });
      
      res.status(201).json(result);
    } catch (error) {
      res.status(400).json({ error: error.message });
    }
  }

  async completeTask(req: Request, res: Response): Promise<void> {
    try {
      await this.completeTaskUseCase.execute({
        taskId: req.params.id
      });
      
      res.status(204).send();
    } catch (error) {
      res.status(400).json({ error: error.message });
    }
  }

  async assignTask(req: Request, res: Response): Promise<void> {
    try {
      await this.assignTaskUseCase.execute({
        taskId: req.params.id,
        userId: req.body.userId
      });
      
      res.status(204).send();
    } catch (error) {
      res.status(400).json({ error: error.message });
    }
  }

  async listTasks(req: Request, res: Response): Promise<void> {
    try {
      const status = req.query.status as TaskStatus | undefined;
      
      const tasks = status
        ? await this.taskRepository.findByStatus(status)
        : await this.taskRepository.findAll();
      
      res.json(tasks);
    } catch (error) {
      res.status(500).json({ error: error.message });
    }
  }
}
```

#### Étape 4: Infrastructure Layer

```typescript
// infrastructure/web/app.ts
import express from 'express';
import { TaskController } from '../../adapters/http/TaskController';
import { CreateTaskUseCase } from '../../application/use-cases/CreateTask';
import { CompleteTaskUseCase } from '../../application/use-cases/CompleteTask';
import { AssignTaskUseCase } from '../../application/use-cases/AssignTask';
import { InMemoryTaskRepository } from '../../adapters/repositories/InMemoryTaskRepository';

// Dependency Injection
const taskRepository = new InMemoryTaskRepository();
const createTaskUseCase = new CreateTaskUseCase(taskRepository);
const completeTaskUseCase = new CompleteTaskUseCase(taskRepository);
const assignTaskUseCase = new AssignTaskUseCase(taskRepository);

const taskController = new TaskController(
  createTaskUseCase,
  completeTaskUseCase,
  assignTaskUseCase,
  taskRepository
);

// Express app
const app = express();
app.use(express.json());

// Routes
app.post('/tasks', (req, res) => taskController.createTask(req, res));
app.put('/tasks/:id/complete', (req, res) => taskController.completeTask(req, res));
app.put('/tasks/:id/assign', (req, res) => taskController.assignTask(req, res));
app.get('/tasks', (req, res) => taskController.listTasks(req, res));

app.listen(3000, () => {
  console.log('Task Manager API running on port 3000');
});
```

### ✅ Tests

```typescript
// tests/domain/Task.test.ts
describe('Task Entity', () => {
  it('should create a valid task', () => {
    const task = Task.create('Buy groceries', 'Milk, bread, eggs');
    
    expect(task.title).toBe('Buy groceries');
    expect(task.status).toBe(TaskStatus.TODO);
    expect(task.assignedTo).toBeNull();
  });

  it('should reject empty title', () => {
    expect(() => Task.create('', 'Description'))
      .toThrow('Title cannot be empty');
  });

  it('should complete a task', () => {
    const task = Task.create('Test', 'Description');
    const completed = task.complete();
    
    expect(completed.status).toBe(TaskStatus.DONE);
    expect(completed.completedAt).not.toBeNull();
  });

  it('should not complete an already completed task', () => {
    const task = Task.create('Test', 'Description');
    const completed = task.complete();
    
    expect(() => completed.complete())
      .toThrow('Task is already completed');
  });

  it('should assign task to user', () => {
    const task = Task.create('Test', 'Description');
    const userId = UserId.fromString('user_123');
    const assigned = task.assignTo(userId);
    
    expect(assigned.assignedTo?.value).toBe('user_123');
    expect(assigned.status).toBe(TaskStatus.IN_PROGRESS);
  });
});

// tests/application/CreateTask.test.ts
describe('CreateTaskUseCase', () => {
  it('should create and save task', async () => {
    const mockRepo = {
      save: jest.fn(),
      findById: jest.fn(),
      findAll: jest.fn(),
      findByStatus: jest.fn(),
      delete: jest.fn()
    };
    
    const useCase = new CreateTaskUseCase(mockRepo);
    
    const result = await useCase.execute({
      title: 'Test Task',
      description: 'Test Description'
    });
    
    expect(result.title).toBe('Test Task');
    expect(mockRepo.save).toHaveBeenCalled();
  });
});
```

### 🎁 Solution Complète
Voir `web/clean-architecture/exercises/ex01-task-manager/`

---

## Exercice 2: Blog System

### 🎯 Objectif
Système de blog avec articles, commentaires, et catégories.

### 📝 Fonctionnalités
- Créer un article (brouillon)
- Publier un article
- Ajouter des commentaires
- Catégoriser les articles
- Modération des commentaires

### 🏗️ Entités

**Article:**
- id, authorId, title, content, slug, status
- tags, publishedAt, createdAt, updatedAt

**Comment:**
- id, articleId, authorId, content, isApproved
- createdAt

**Tag:**
- Value Object (nom unique)

### 🔑 Business Rules

1. **Article:**
   - Title min 5 chars, max 200 chars
   - Content min 100 chars
   - Slug généré automatiquement (kebab-case)
   - Peut publier seulement si status = DRAFT
   - Ne peut pas modifier après publication

2. **Comment:**
   - Content min 10 chars, max 1000 chars
   - Nécessite approbation avant affichage
   - Ne peut commenter que sur article publié

3. **Tag:**
   - Nom unique, lowercase
   - Max 5 tags par article

### 💡 À Implémenter

```typescript
// Domain
class Article {
  static create(authorId, title, content): Article
  publish(): Article
  addTag(tag: Tag): Article
  removeTag(tag: Tag): Article
}

class Comment {
  static create(articleId, authorId, content): Comment
  approve(): Comment
  reject(): Comment
}

// Use Cases
class CreateArticleUseCase
class PublishArticleUseCase
class AddCommentUseCase
class ApproveCommentUseCase
class ListPublishedArticlesUseCase

// Repositories
interface IArticleRepository
interface ICommentRepository
```

### ✅ Critères de Réussite

- [ ] Entités immuables avec factory methods
- [ ] Règles métier dans le domain
- [ ] Use cases pour chaque action
- [ ] Interfaces pour repositories
- [ ] Au moins 2 implémentations (InMemory + MongoDB/Postgres)
- [ ] Tests unitaires pour domain
- [ ] Tests d'intégration pour use cases

### 🎁 Solution
Voir `web/clean-architecture/exercises/ex02-blog-system/`

---

## Exercice 3: E-commerce Cart

### 🎯 Objectif
Panier d'achat avec gestion des produits, quantités, et promotions.

### 📝 Fonctionnalités
- Ajouter un produit au panier
- Modifier la quantité
- Retirer un produit
- Appliquer un code promo
- Calculer le total avec taxes

### 🏗️ Entités

**Cart:**
- id, customerId, items[], promoCode, status
- createdAt, updatedAt

**CartItem:**
- productId, productName, unitPrice, quantity

**Product:**
- id, name, price, stock

**PromoCode:**
- code, discountType (PERCENTAGE | FIXED), discountValue
- validFrom, validUntil, minAmount

### 🔑 Business Rules

1. **Cart:**
   - Max 50 items différents
   - Quantity max 99 par item
   - Panier expire après 7 jours d'inactivité
   - Ne peut pas checkout si item out of stock

2. **PromoCode:**
   - Un seul code promo par panier
   - Vérifie date de validité
   - Vérifie montant minimum
   - PERCENTAGE: 0-100%
   - FIXED: montant fixe en euros

3. **Product:**
   - Stock vérifié avant ajout au panier
   - Prix toujours positif

### 💡 À Implémenter

```typescript
// Domain
class Cart {
  static create(customerId): Cart
  addItem(product: Product, quantity: number): Cart
  removeItem(productId: string): Cart
  updateQuantity(productId: string, quantity: number): Cart
  applyPromoCode(promoCode: PromoCode): Cart
  getSubtotal(): Money
  getDiscount(): Money
  getTaxes(): Money
  getTotal(): Money
  isEmpty(): boolean
  canCheckout(): boolean
}

class CartItem {
  constructor(product: Product, quantity: number)
  changeQuantity(quantity: number): CartItem
  getTotalPrice(): Money
}

class PromoCode {
  static create(code, type, value, validFrom, validUntil, minAmount): PromoCode
  isValid(now: Date): boolean
  canApplyTo(amount: Money): boolean
  calculateDiscount(amount: Money): Money
}

// Use Cases
class AddToCartUseCase
class RemoveFromCartUseCase
class UpdateCartItemQuantityUseCase
class ApplyPromoCodeUseCase
class CheckoutCartUseCase

// Repositories
interface ICartRepository
interface IProductRepository
interface IPromoCodeRepository
```

### ✅ Critères de Réussite

- [ ] Money value object pour les montants
- [ ] Calculs de prix corrects avec promotions
- [ ] Vérification du stock avant checkout
- [ ] Gestion de l'expiration du panier
- [ ] Tests avec différents scénarios de promotion
- [ ] Tests de calcul de taxes

### 🎁 Solution
Voir `web/clean-architecture/exercises/ex03-ecommerce-cart/`

---

## Exercice 4: Booking System

### 🎯 Objectif
Système de réservation (hôtel, salle, etc.) avec disponibilité et conflicts.

### 📝 Fonctionnalités
- Vérifier disponibilité
- Créer une réservation
- Annuler une réservation
- Modifier une réservation
- Gérer les conflits

### 🏗️ Entités

**Booking:**
- id, resourceId, userId, startDate, endDate
- status (PENDING, CONFIRMED, CANCELLED)
- totalPrice, createdAt

**Resource:**
- id, name, type, pricePerDay, maxCapacity

**DateRange:**
- Value Object (startDate, endDate)

### 🔑 Business Rules

1. **Booking:**
   - StartDate doit être dans le futur
   - EndDate > StartDate
   - Min 1 jour, max 30 jours
   - Ne peut pas réserver resource déjà bookée
   - Annulation gratuite jusqu'à 48h avant
   - Annulation après 48h: pénalité 50%

2. **Resource:**
   - Capacity doit être respectée
   - Prix peut varier selon la saison

3. **Conflict Detection:**
   - Vérifier overlapping bookings
   - Buffer time entre bookings (ex: 2h)

### 💡 À Implémenter

```typescript
// Domain
class Booking {
  static create(resourceId, userId, dateRange, price): Booking
  confirm(): Booking
  cancel(cancelledAt: Date): Booking
  getCancellationFee(): Money
  canBeCancelled(now: Date): boolean
}

class DateRange {
  static create(start: Date, end: Date): DateRange
  getDurationInDays(): number
  overlapsWith(other: DateRange): boolean
  isInFuture(now: Date): boolean
}

class Resource {
  calculatePrice(dateRange: DateRange): Money
  isAvailableFor(dateRange: DateRange, existingBookings: Booking[]): boolean
}

// Use Cases
class CheckAvailabilityUseCase
class CreateBookingUseCase
class CancelBookingUseCase
class ListBookingsUseCase

// Services
interface IAvailabilityChecker {
  checkAvailability(resourceId, dateRange): Promise<boolean>
}
```

### ✅ Critères de Réussite

- [ ] DateRange value object avec validation
- [ ] Détection de conflits d'horaires
- [ ] Calcul des pénalités d'annulation
- [ ] Vérification de disponibilité robuste
- [ ] Tests de edge cases (même jour, overlap partiel, etc.)

### 🎁 Solution
Voir `web/clean-architecture/exercises/ex04-booking-system/`

---

## Exercice 5: Social Network

### 🎯 Objectif
Mini réseau social avec posts, likes, follows, et feed.

### 📝 Fonctionnalités
- Créer un post
- Liker/Unliker un post
- Suivre/Ne plus suivre un utilisateur
- Générer le feed d'un utilisateur
- Commenter un post

### 🏗️ Entités

**Post:**
- id, authorId, content, media[], likes[], commentCount
- createdAt, updatedAt

**User:**
- id, username, bio, followers[], following[]

**Follow:**
- followerId, followeeId, createdAt

**Like:**
- userId, postId, createdAt

### 🔑 Business Rules

1. **Post:**
   - Content max 280 characters
   - Max 4 images/video
   - Peut éditer dans les 5 minutes
   - Auteur peut supprimer n'importe quand

2. **Follow:**
   - Ne peut pas se suivre soi-même
   - Relation unidirectionnelle
   - Max 5000 following

3. **Like:**
   - Un like par user par post
   - Peut unliker

4. **Feed:**
   - Posts des utilisateurs suivis
   - Triés par date décroissante
   - Pagination (20 posts par page)

### 💡 À Implémenter

```typescript
// Domain
class Post {
  static create(authorId, content, media): Post
  edit(newContent: string, editedAt: Date): Post
  canBeEditedAt(now: Date): boolean
  addLike(userId: UserId): Post
  removeLike(userId: UserId): Post
  hasLikedBy(userId: UserId): boolean
}

class User {
  follow(otherUserId: UserId): User
  unfollow(otherUserId: UserId): User
  isFollowing(userId: UserId): boolean
  canFollowMore(): boolean
}

// Use Cases
class CreatePostUseCase
class EditPostUseCase
class LikePostUseCase
class FollowUserUseCase
class GenerateFeedUseCase

// Query (Read Model)
interface IFeedQuery {
  getFeedForUser(userId, page, pageSize): Promise<PostDTO[]>
}
```

### ✅ Critères de Réussite

- [ ] Séparation Command/Query (CQRS léger)
- [ ] Feed optimisé (pas de N+1 queries)
- [ ] Gestion des relations bidirectionnelles
- [ ] Tests de performance pour feed
- [ ] Events (PostCreated, PostLiked, UserFollowed)

### 🎁 Solution
Voir `web/clean-architecture/exercises/ex05-social-network/`

---

## Exercice 6: Refactoring Legacy Code

### 🎯 Objectif
Refactorer du code legacy vers Clean Architecture.

### 📝 Code Legacy Fourni

```typescript
// ❌ MAUVAIS CODE - Tout mélangé
class UserService {
  async register(req: any, res: any) {
    const { email, password, name } = req.body;
    
    // Validation dans le service
    if (!email || !email.includes('@')) {
      return res.status(400).json({ error: 'Invalid email' });
    }
    
    if (password.length < 6) {
      return res.status(400).json({ error: 'Password too short' });
    }
    
    // Direct DB access
    const existingUser = await mongoose.model('User').findOne({ email });
    if (existingUser) {
      return res.status(400).json({ error: 'Email exists' });
    }
    
    // Business logic
    const passwordHash = crypto.createHash('md5').update(password).digest('hex');
    
    // Insert
    const user = await mongoose.model('User').create({
      email: email.toLowerCase(),
      name: name,
      passwordHash: passwordHash,
      credits: 100, // Welcome bonus
      createdAt: new Date()
    });
    
    // Send email
    await sendgrid.send({
      to: user.email,
      subject: 'Welcome!',
      html: `<h1>Hello ${user.name}!</h1>`
    });
    
    // Analytics
    await analytics.track({
      userId: user._id,
      event: 'User Registered',
      properties: { email: user.email }
    });
    
    res.json({
      id: user._id,
      email: user.email,
      name: user.name
    });
  }
  
  async login(req: any, res: any) {
    const { email, password } = req.body;
    
    const user = await mongoose.model('User').findOne({ email: email.toLowerCase() });
    if (!user) {
      return res.status(401).json({ error: 'Invalid credentials' });
    }
    
    const passwordHash = crypto.createHash('md5').update(password).digest('hex');
    if (user.passwordHash !== passwordHash) {
      return res.status(401).json({ error: 'Invalid credentials' });
    }
    
    const token = jwt.sign({ userId: user._id }, 'secret123');
    
    res.json({
      token: token,
      user: {
        id: user._id,
        email: user.email,
        name: user.name
      }
    });
  }
}
```

### 📝 Tâches

1. **Identifier les problèmes** dans le code legacy
2. **Extraire le domain** (User entity, règles métier)
3. **Créer les use cases** (RegisterUser, LoginUser)
4. **Définir les ports** (IUserRepository, IEmailService, IAnalyticsService)
5. **Implémenter les adapters**
6. **Créer les controllers** (propres)
7. **Écrire les tests**

### 🎯 Résultat Attendu

```
src/
├── domain/
│   ├── entities/
│   │   └── User.ts
│   └── value-objects/
│       └── Email.ts
├── application/
│   ├── use-cases/
│   │   ├── RegisterUser.ts
│   │   └── LoginUser.ts
│   └── ports/
│       ├── IUserRepository.ts
│       ├── IEmailService.ts
│       ├── IPasswordService.ts
│       └── IAnalyticsService.ts
├── adapters/
│   ├── http/
│   │   └── UserController.ts
│   ├── repositories/
│   │   └── MongoUserRepository.ts
│   ├── email/
│   │   └── SendgridEmailService.ts
│   └── analytics/
│       └── SegmentAnalyticsService.ts
└── infrastructure/
    └── web/
        └── app.ts
```

### ✅ Critères de Réussite

- [ ] Aucune dépendance externe dans le domain
- [ ] Use cases testables avec mocks
- [ ] Règles métier (welcome bonus) dans l'entité
- [ ] Email value object avec validation
- [ ] Password service avec bcrypt (pas MD5!)
- [ ] Controller ne fait que de l'orchestration
- [ ] Tests unitaires pour domain
- [ ] Tests d'intégration pour use cases

### 🎁 Solution
Voir `web/clean-architecture/exercises/ex06-refactoring-legacy/`

---

## 📊 Grille d'Évaluation

### Domain Layer (30 points)
- [ ] Entités riches avec comportement (10 pts)
- [ ] Value Objects utilisés (5 pts)
- [ ] Règles métier dans le domain (10 pts)
- [ ] Aucune dépendance externe (5 pts)

### Application Layer (25 points)
- [ ] Use cases clairs et simples (10 pts)
- [ ] Dépendances via interfaces (10 pts)
- [ ] Un use case = une action (5 pts)

### Adapters Layer (20 points)
- [ ] Implémentations correctes des interfaces (10 pts)
- [ ] Conversion domain <-> infrastructure (5 pts)
- [ ] Au moins 2 implémentations par interface (5 pts)

### Infrastructure Layer (10 points)
- [ ] Dependency injection propre (5 pts)
- [ ] Configuration séparée (5 pts)

### Tests (15 points)
- [ ] Tests unitaires domain (5 pts)
- [ ] Tests use cases avec mocks (5 pts)
- [ ] Tests d'intégration (5 pts)

---

## 🎓 Conseils

### Pour Démarrer
1. **Toujours commencer par le domain** - C'est le cœur
2. **Penser "business rules first"** - Pas technique first
3. **Entités immuables** - Retourner de nouvelles instances
4. **Value Objects** pour concepts métier importants

### Pendant le Développement
1. **Red-Green-Refactor** - TDD quand possible
2. **Une classe = une responsabilité**
3. **Ne pas optimiser prématurément**
4. **Nommer clairement** - Code auto-documenté

### Pour Tester
1. **Domain: tests unitaires purs** - Pas de mocks
2. **Use Cases: tests avec mocks** - Isoler les dépendances
3. **Adapters: tests d'intégration** - Avec DB/API réelles
4. **Controllers: tests E2E** - Scénarios complets

---

## 📚 Ressources Complémentaires

- **Guide Clean Architecture:** `WEB_CLEAN_ARCHITECTURE.md`
- **Exemples complets:** `web/clean-architecture/example.ts`
- **Patterns associés:** `WEB_PATTERNS_COMPARISON.md`

---

**Bon courage! 🚀**
