# CQRS & Event Sourcing - Guide Complet

## 📚 Table des Matières

1. [Introduction](#introduction)
2. [CQRS: Command Query Responsibility Segregation](#cqrs)
3. [Event Sourcing](#event-sourcing)
4. [Combinaison CQRS + Event Sourcing](#combinaison)
5. [Patterns et Techniques](#patterns)
6. [Cas Pratiques](#cas-pratiques)
7. [Avantages et Inconvénients](#avantages-inconvénients)

---

## Introduction

### CQRS (Command Query Responsibility Segregation)

**Principe:** Séparer les opérations de lecture et d'écriture en modèles différents.

```
     ┌─────────────────────────────────────────┐
     │          TRADITIONAL MODEL              │
     │                                         │
     │  ┌──────────────────────────────────┐  │
     │  │    User Model                    │  │
     │  │  - id, email, name               │  │
     │  │  - create()                      │  │
     │  │  - update()                      │  │
     │  │  - findById()                    │  │
     │  │  - findAll()                     │  │
     │  └──────────────────────────────────┘  │
     │       ↓                         ↑       │
     │    Writes                    Reads      │
     │      Same Model                         │
     └─────────────────────────────────────────┘

              VS

     ┌─────────────────────────────────────────┐
     │             CQRS MODEL                  │
     │                                         │
     │  ┌──────────────────┐  ┌─────────────┐  │
     │  │  WRITE MODEL     │  │ READ MODEL  │  │
     │  │                  │  │             │  │
     │  │  User (Entity)   │  │  UserDTO    │  │
     │  │  - create()      │  │  - getById()│  │
     │  │  - update()      │  │  - search() │  │
     │  │                  │  │  - filter() │  │
     │  └──────────────────┘  └─────────────┘  │
     │          ↓                    ↑         │
     │       Commands             Queries      │
     │     (Writes)               (Reads)      │
     └─────────────────────────────────────────┘
```

### Event Sourcing

**Principe:** Stocker l'historique des changements (events) au lieu de l'état actuel.

```
TRADITIONAL:
┌─────────────────────────┐
│   Current State (DB)    │
│                         │
│  User { id: 1,          │
│    email: "j@ex.com",   │
│    balance: 150 }       │
└─────────────────────────┘

EVENT SOURCING:
┌─────────────────────────────────────┐
│         Event Store                 │
│                                     │
│  [AccountCreated    { bal: 0 }]     │
│  [MoneyDeposited    { amt: 100 }]   │
│  [MoneyDeposited    { amt: 50 }]    │
│  [MoneyWithdrawn    { amt: 50 }]    │
│  [EmailChanged      {...}]          │
│                                     │
│  → Replay = balance: 100            │
└─────────────────────────────────────┘
```

---

## CQRS

### Concept de Base

**Command:** Demande de changement d'état
**Query:** Demande de lecture

```typescript
// ============================================
// COMMANDS (Write Side)
// ============================================

interface Command {
  commandId: string;
  timestamp: Date;
}

interface CreateUserCommand extends Command {
  userId: string;
  email: string;
  name: string;
}

interface ChangeEmailCommand extends Command {
  userId: string;
  newEmail: string;
}

// Command Handler
class CreateUserHandler {
  constructor(private userRepository: IUserRepository) {}

  async handle(command: CreateUserCommand): Promise<void> {
    // Validation
    if (!command.email.includes('@')) {
      throw new Error('Invalid email');
    }

    // Create aggregate
    const user = User.create(
      command.userId,
      command.email,
      command.name
    );

    // Save
    await this.userRepository.save(user);
  }
}

// ============================================
// QUERIES (Read Side)
// ============================================

interface Query {
  queryId: string;
}

interface GetUserByIdQuery extends Query {
  userId: string;
}

interface SearchUsersQuery extends Query {
  searchTerm: string;
  page: number;
  pageSize: number;
}

// Query Result (DTO)
interface UserDTO {
  id: string;
  email: string;
  name: string;
  createdAt: string;
  lastLogin: string | null;
}

interface UserListDTO {
  users: UserDTO[];
  total: number;
  page: number;
}

// Query Handler
class GetUserByIdHandler {
  constructor(private readModel: IUserReadModel) {}

  async handle(query: GetUserByIdQuery): Promise<UserDTO | null> {
    // Direct read from optimized read model
    return await this.readModel.findById(query.userId);
  }
}

class SearchUsersHandler {
  constructor(private readModel: IUserReadModel) {}

  async handle(query: SearchUsersQuery): Promise<UserListDTO> {
    // Use read model with full-text search, etc.
    return await this.readModel.search(
      query.searchTerm,
      query.page,
      query.pageSize
    );
  }
}
```

### Write Model vs Read Model

**Write Model (Domain Model):**
- Focus: Business rules et cohérence
- Normalisé, agrégats
- Optimisé pour écriture

**Read Model (Query Model):**
- Focus: Performance de lecture
- Dénormalisé, aplati
- Optimisé pour requêtes spécifiques

```typescript
// WRITE MODEL - Aggregate
class Order {
  private constructor(
    public readonly id: OrderId,
    private items: OrderLine[],
    private status: OrderStatus
  ) {}

  addItem(product: Product, quantity: number): Order {
    // Business rules
    if (this.status !== OrderStatus.DRAFT) {
      throw new Error('Cannot modify submitted order');
    }
    // ...
  }
}

// READ MODEL - Flat DTO optimized for queries
interface OrderListItemDTO {
  orderId: string;
  customerName: string;        // Dénormalisé
  customerEmail: string;        // Dénormalisé
  totalAmount: number;
  itemCount: number;            // Pré-calculé
  status: string;
  createdAt: string;
  canBeCancelled: boolean;      // Pré-calculé
}

// Different read models for different needs
interface OrderDetailsDTO {
  orderId: string;
  customer: {
    id: string;
    name: string;
    email: string;
    shippingAddress: AddressDTO;
  };
  items: Array<{
    productId: string;
    productName: string;
    productImage: string;       // Dénormalisé
    unitPrice: number;
    quantity: number;
    totalPrice: number;         // Pré-calculé
  }>;
  subtotal: number;
  discount: number;
  taxes: number;
  total: number;
  status: string;
  timeline: Array<{             // Historique
    status: string;
    timestamp: string;
  }>;
}
```

### Synchronisation Write → Read

```typescript
// Option 1: Synchrone (simple mais couplé)
class CreateOrderHandler {
  constructor(
    private orderRepository: IOrderRepository,
    private orderReadModel: IOrderReadModel
  ) {}

  async handle(command: CreateOrderCommand): Promise<void> {
    const order = Order.create(/* ... */);
    
    // 1. Write
    await this.orderRepository.save(order);
    
    // 2. Update read model
    await this.orderReadModel.insert({
      orderId: order.id.value,
      customerName: command.customerName,
      totalAmount: order.getTotal().amount,
      // ...
    });
  }
}

// Option 2: Asynchrone via events (découplé)
class CreateOrderHandler {
  constructor(
    private orderRepository: IOrderRepository,
    private eventBus: IEventBus
  ) {}

  async handle(command: CreateOrderCommand): Promise<void> {
    const order = Order.create(/* ... */);
    await this.orderRepository.save(order);
    
    // Publish event
    await this.eventBus.publish(new OrderCreatedEvent(/* ... */));
  }
}

// Event handler updates read model
class UpdateOrderReadModelWhenOrderCreated {
  constructor(private orderReadModel: IOrderReadModel) {}

  async handle(event: OrderCreatedEvent): Promise<void> {
    await this.orderReadModel.insert({
      orderId: event.orderId,
      customerName: event.customerName,
      totalAmount: event.totalAmount,
      // ...
    });
  }
}
```

---

## Event Sourcing

### Principe

Au lieu de stocker l'état actuel, on stocke **tous les événements** qui ont conduit à cet état.

```typescript
// Traditional: Store current state
UPDATE accounts SET balance = 150 WHERE id = 1;

// Event Sourcing: Store events
INSERT INTO events (aggregate_id, event_type, data) VALUES
  (1, 'AccountCreated', '{"initialBalance": 0}'),
  (1, 'MoneyDeposited', '{"amount": 100}'),
  (1, 'MoneyDeposited', '{"amount": 50}'),
  (1, 'MoneyWithdrawn', '{"amount": 50}');

// Reconstruct state by replaying events
const account = new Account();
for (const event of events) {
  account.apply(event);
}
// → account.balance === 100
```

### Event Store

```typescript
interface StoredEvent {
  eventId: string;
  aggregateId: string;
  aggregateType: string;
  eventType: string;
  eventData: any;
  metadata: {
    timestamp: Date;
    userId: string;
    version: number;
  };
}

class EventStore {
  async append(
    aggregateId: string,
    aggregateType: string,
    events: DomainEvent[]
  ): Promise<void> {
    for (const event of events) {
      await this.db.collection('events').insertOne({
        eventId: generateId(),
        aggregateId,
        aggregateType,
        eventType: event.eventType,
        eventData: this.serializeEvent(event),
        metadata: {
          timestamp: event.occurredOn,
          version: event.version
        }
      });
    }
  }

  async loadEvents(
    aggregateId: string,
    fromVersion: number = 0
  ): Promise<DomainEvent[]> {
    const storedEvents = await this.db.collection('events')
      .find({ 
        aggregateId,
        'metadata.version': { $gt: fromVersion }
      })
      .sort({ 'metadata.version': 1 })
      .toArray();

    return storedEvents.map(stored => this.deserializeEvent(stored));
  }
}
```

### Aggregate avec Event Sourcing

```typescript
// Events
interface AccountCreatedEvent {
  eventType: 'AccountCreated';
  accountId: string;
  ownerId: string;
  initialBalance: number;
  version: number;
}

interface MoneyDepositedEvent {
  eventType: 'MoneyDeposited';
  accountId: string;
  amount: number;
  balance: number; // État après dépôt
  version: number;
}

interface MoneyWithdrawnEvent {
  eventType: 'MoneyWithdrawn';
  accountId: string;
  amount: number;
  balance: number; // État après retrait
  version: number;
}

// Aggregate
class BankAccount {
  private uncommittedEvents: DomainEvent[] = [];

  constructor(
    public readonly id: string,
    private ownerId: string,
    private balance: number,
    private isActive: boolean,
    private version: number = 0
  ) {}

  // ============================================
  // Command methods (produce events)
  // ============================================

  static create(accountId: string, ownerId: string, initialBalance: number): BankAccount {
    const account = new BankAccount(accountId, ownerId, 0, true, 0);

    const event: AccountCreatedEvent = {
      eventType: 'AccountCreated',
      accountId,
      ownerId,
      initialBalance,
      version: 1
    };

    account.apply(event);
    account.uncommittedEvents.push(event);

    return account;
  }

  deposit(amount: number): void {
    if (!this.isActive) {
      throw new Error('Cannot deposit to closed account');
    }
    if (amount <= 0) {
      throw new Error('Amount must be positive');
    }

    const newBalance = this.balance + amount;
    const event: MoneyDepositedEvent = {
      eventType: 'MoneyDeposited',
      accountId: this.id,
      amount,
      balance: newBalance,
      version: this.version + 1
    };

    this.apply(event);
    this.uncommittedEvents.push(event);
  }

  withdraw(amount: number): void {
    if (!this.isActive) {
      throw new Error('Cannot withdraw from closed account');
    }
    if (amount <= 0) {
      throw new Error('Amount must be positive');
    }
    if (this.balance < amount) {
      throw new Error('Insufficient funds');
    }

    const newBalance = this.balance - amount;
    const event: MoneyWithdrawnEvent = {
      eventType: 'MoneyWithdrawn',
      accountId: this.id,
      amount,
      balance: newBalance,
      version: this.version + 1
    };

    this.apply(event);
    this.uncommittedEvents.push(event);
  }

  // ============================================
  // Event application (changes state)
  // ============================================

  private apply(event: DomainEvent): void {
    switch (event.eventType) {
      case 'AccountCreated':
        const created = event as AccountCreatedEvent;
        this.balance = created.initialBalance;
        this.version = created.version;
        break;

      case 'MoneyDeposited':
        const deposited = event as MoneyDepositedEvent;
        this.balance = deposited.balance;
        this.version = deposited.version;
        break;

      case 'MoneyWithdrawn':
        const withdrawn = event as MoneyWithdrawnEvent;
        this.balance = withdrawn.balance;
        this.version = withdrawn.version;
        break;
    }
  }

  // ============================================
  // Reconstruction from events
  // ============================================

  static fromEvents(events: DomainEvent[]): BankAccount {
    if (events.length === 0) {
      throw new Error('Cannot reconstruct from empty event stream');
    }

    const firstEvent = events[0] as AccountCreatedEvent;
    const account = new BankAccount(
      firstEvent.accountId,
      firstEvent.ownerId,
      0,
      true,
      0
    );

    for (const event of events) {
      account.apply(event);
    }

    return account;
  }

  getUncommittedEvents(): DomainEvent[] {
    return [...this.uncommittedEvents];
  }

  markEventsAsCommitted(): void {
    this.uncommittedEvents = [];
  }
}
```

### Repository avec Event Sourcing

```typescript
class EventSourcedAccountRepository {
  constructor(private eventStore: EventStore) {}

  async save(account: BankAccount): Promise<void> {
    const events = account.getUncommittedEvents();
    
    await this.eventStore.append(
      account.id,
      'BankAccount',
      events
    );

    account.markEventsAsCommitted();
  }

  async findById(accountId: string): Promise<BankAccount | null> {
    const events = await this.eventStore.loadEvents(accountId);
    
    if (events.length === 0) {
      return null;
    }

    return BankAccount.fromEvents(events);
  }
}
```

### Snapshots (Optimisation)

Pour éviter de rejouer tous les événements à chaque fois:

```typescript
interface Snapshot {
  aggregateId: string;
  aggregateType: string;
  version: number;
  state: any;
  timestamp: Date;
}

class EventStore {
  async saveSnapshot(
    aggregateId: string,
    aggregateType: string,
    version: number,
    state: any
  ): Promise<void> {
    await this.db.collection('snapshots').insertOne({
      aggregateId,
      aggregateType,
      version,
      state,
      timestamp: new Date()
    });
  }

  async loadSnapshot(aggregateId: string): Promise<Snapshot | null> {
    return await this.db.collection('snapshots')
      .findOne({ aggregateId })
      .sort({ version: -1 })
      .limit(1);
  }

  async loadEventsFromSnapshot(aggregateId: string): Promise<{
    snapshot: Snapshot | null;
    events: DomainEvent[];
  }> {
    const snapshot = await this.loadSnapshot(aggregateId);
    
    const fromVersion = snapshot ? snapshot.version : 0;
    const events = await this.loadEvents(aggregateId, fromVersion);

    return { snapshot, events };
  }
}

// Reconstruction optimisée
class EventSourcedAccountRepository {
  async findById(accountId: string): Promise<BankAccount | null> {
    const { snapshot, events } = await this.eventStore.loadEventsFromSnapshot(accountId);

    if (!snapshot && events.length === 0) {
      return null;
    }

    let account: BankAccount;

    if (snapshot) {
      // Reconstruct from snapshot
      account = BankAccount.fromSnapshot(snapshot.state);
    } else {
      // Start from beginning
      account = BankAccount.fromEvents([events[0]]);
      events.shift();
    }

    // Apply remaining events
    for (const event of events) {
      account.apply(event);
    }

    return account;
  }
}
```

---

## Combinaison CQRS + Event Sourcing

```
┌─────────────────────────────────────────────────────────┐
│                    USER COMMAND                         │
│              (CreateAccount, Deposit, etc.)             │
└─────────────────────────┬───────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────┐
│                  COMMAND HANDLER                        │
│  - Load aggregate from event store                      │
│  - Execute command                                      │
│  - Save new events                                      │
└─────────────────────────┬───────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────┐
│                    EVENT STORE                          │
│  [AccountCreated, MoneyDeposited, MoneyWithdrawn, ...]  │
└─────────────────────────┬───────────────────────────────┘
                          │
                          ↓ (events published)
                          │
         ┌────────────────┼────────────────┐
         ↓                ↓                ↓
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│ Read Model 1 │  │ Read Model 2 │  │ Read Model 3 │
│  (Account    │  │  (Audit      │  │  (Analytics) │
│   Balance)   │  │   Log)       │  │              │
└──────────────┘  └──────────────┘  └──────────────┘
         │                │                │
         └────────────────┼────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────┐
│                    QUERY HANDLER                        │
│  - Read from optimized read models                      │
│  - Fast, denormalized data                              │
└─────────────────────────┬───────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────┐
│                      QUERY RESULT                       │
└─────────────────────────────────────────────────────────┘
```

### Exemple Complet

```typescript
// ============================================
// COMMANDS & EVENTS
// ============================================

interface CreateAccountCommand {
  accountId: string;
  ownerId: string;
  initialBalance: number;
}

interface DepositMoneyCommand {
  accountId: string;
  amount: number;
}

interface AccountCreatedEvent {
  accountId: string;
  ownerId: string;
  initialBalance: number;
  timestamp: Date;
}

interface MoneyDepositedEvent {
  accountId: string;
  amount: number;
  balance: number;
  timestamp: Date;
}

// ============================================
// WRITE SIDE (Event Sourcing)
// ============================================

class CreateAccountHandler {
  constructor(private repository: EventSourcedAccountRepository) {}

  async handle(command: CreateAccountCommand): Promise<void> {
    const account = BankAccount.create(
      command.accountId,
      command.ownerId,
      command.initialBalance
    );

    await this.repository.save(account);
  }
}

class DepositMoneyHandler {
  constructor(private repository: EventSourcedAccountRepository) {}

  async handle(command: DepositMoneyCommand): Promise<void> {
    const account = await this.repository.findById(command.accountId);
    if (!account) {
      throw new Error('Account not found');
    }

    account.deposit(command.amount);

    await this.repository.save(account);
  }
}

// ============================================
// READ SIDE (CQRS)
// ============================================

// Read Model 1: Account Balance (pour affichage)
interface AccountBalanceDTO {
  accountId: string;
  ownerName: string;
  balance: number;
  lastTransaction: Date;
}

class AccountBalanceReadModel {
  constructor(private db: Db) {}

  async insert(data: AccountBalanceDTO): Promise<void> {
    await this.db.collection('account_balances').insertOne(data);
  }

  async update(accountId: string, balance: number, lastTransaction: Date): Promise<void> {
    await this.db.collection('account_balances').updateOne(
      { accountId },
      { $set: { balance, lastTransaction } }
    );
  }

  async findById(accountId: string): Promise<AccountBalanceDTO | null> {
    return await this.db.collection('account_balances').findOne({ accountId });
  }
}

// Read Model 2: Transaction History (pour audit)
interface TransactionDTO {
  transactionId: string;
  accountId: string;
  type: 'DEPOSIT' | 'WITHDRAWAL';
  amount: number;
  balance: number;
  timestamp: Date;
}

class TransactionHistoryReadModel {
  constructor(private db: Db) {}

  async append(transaction: TransactionDTO): Promise<void> {
    await this.db.collection('transactions').insertOne(transaction);
  }

  async getHistory(accountId: string, limit: number = 10): Promise<TransactionDTO[]> {
    return await this.db.collection('transactions')
      .find({ accountId })
      .sort({ timestamp: -1 })
      .limit(limit)
      .toArray();
  }
}

// Event Handlers (Update Read Models)
class UpdateAccountBalanceWhenMoneyDeposited {
  constructor(private readModel: AccountBalanceReadModel) {}

  async handle(event: MoneyDepositedEvent): Promise<void> {
    await this.readModel.update(
      event.accountId,
      event.balance,
      event.timestamp
    );
  }
}

class RecordTransactionWhenMoneyDeposited {
  constructor(private readModel: TransactionHistoryReadModel) {}

  async handle(event: MoneyDepositedEvent): Promise<void> {
    await this.readModel.append({
      transactionId: generateId(),
      accountId: event.accountId,
      type: 'DEPOSIT',
      amount: event.amount,
      balance: event.balance,
      timestamp: event.timestamp
    });
  }
}

// Query Handlers
class GetAccountBalanceHandler {
  constructor(private readModel: AccountBalanceReadModel) {}

  async handle(accountId: string): Promise<AccountBalanceDTO | null> {
    return await this.readModel.findById(accountId);
  }
}

class GetTransactionHistoryHandler {
  constructor(private readModel: TransactionHistoryReadModel) {}

  async handle(accountId: string, limit: number): Promise<TransactionDTO[]> {
    return await this.readModel.getHistory(accountId, limit);
  }
}
```

---

## Avantages et Inconvénients

### CQRS

**✅ Avantages:**
- Performance optimisée séparément pour lecture et écriture
- Modèles adaptés aux besoins spécifiques
- Scalabilité indépendante (scale reads != scale writes)
- Requêtes complexes simplifiées

**❌ Inconvénients:**
- Complexité accrue
- Eventual consistency
- Plus de code à maintenir
- Synchronisation write ↔ read

### Event Sourcing

**✅ Avantages:**
- Audit trail complet et immuable
- Time travel (état à n'importe quel moment)
- Debugging facilité (replay events)
- Event-driven architecture naturelle
- Aucune perte de données

**❌ Inconvénients:**
- Complexité
- Performance (rejouer beaucoup d'events)
- Snapshots nécessaires
- Migrations difficiles
- Stockage croissant

### Quand Utiliser?

**✅ Utiliser CQRS quand:**
- Besoins lecture/écriture très différents
- Performance critique sur les reads
- Requêtes complexes nombreuses
- Besoin de scale différemment

**✅ Utiliser Event Sourcing quand:**
- Audit trail obligatoire (finance, santé)
- Besoin d'historique complet
- Règles métier temporelles
- Event-driven architecture

**❌ Ne PAS utiliser quand:**
- CRUD simple
- Pas de complexité métier
- Équipe inexpérimentée
- Projet court terme

---

**Ressources:**
- "Implementing Domain-Driven Design" by Vaughn Vernon
- "Versioning in an Event Sourced System" by Greg Young
- Martin Fowler's blog: CQRS, Event Sourcing

**Voir aussi:**
- `WEB_CQRS_EXERCISES.md` pour exercices pratiques
- `web/cqrs-pattern/bank-account.ts` pour exemple complet
