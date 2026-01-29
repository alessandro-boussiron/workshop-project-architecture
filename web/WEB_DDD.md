# Domain-Driven Design (DDD) - Guide Complet

## 📚 Table des Matières

1. [Introduction au DDD](#introduction-au-ddd)
2. [Concepts Fondamentaux](#concepts-fondamentaux)
3. [Building Blocks Tactiques](#building-blocks-tactiques)
4. [Building Blocks Stratégiques](#building-blocks-stratégiques)
5. [Bounded Contexts](#bounded-contexts)
6. [Aggregates](#aggregates)
7. [Domain Events](#domain-events)
8. [Ubiquitous Language](#ubiquitous-language)
9. [Cas Pratiques](#cas-pratiques)
10. [Anti-Patterns](#anti-patterns)

---

## Introduction au DDD

### Qu'est-ce que le DDD?

**Domain-Driven Design** est une approche de conception logicielle qui se concentre sur:

- 🎯 **Le domaine métier** comme élément central
- 🗣️ **Le langage ubiquitaire** - vocabulaire partagé entre devs et experts métier
- 🏗️ **La modélisation** basée sur la complexité métier
- 🔄 **L'itération continue** avec les experts du domaine

### Pourquoi DDD?

**Problèmes traditionnels:**

```typescript
// ❌ Code technique qui ne reflète pas le métier
class OrderManager {
  async processOrder(orderId: string) {
    const order = await this.db.query('SELECT * FROM orders WHERE id = ?', [orderId]);
    const items = await this.db.query('SELECT * FROM order_items WHERE order_id = ?', [orderId]);
    
    let total = 0;
    for (const item of items) {
      total += item.price * item.quantity;
    }
    
    if (total > 100) {
      total = total * 0.9; // 10% discount
    }
    
    await this.db.query('UPDATE orders SET status = ?, total = ? WHERE id = ?', 
      ['processed', total, orderId]);
  }
}
```

**Problèmes:**
- 🔴 Pas de vocabulaire métier (où est la "règle de remise"?)
- 🔴 Logique métier noyée dans le technique
- 🔴 Difficile à comprendre pour les experts métier
- 🔴 Impossible de discuter avec un product owner

**Solution DDD:**

```typescript
// ✅ Modèle riche qui exprime le métier
class Order {
  private constructor(
    public readonly id: OrderId,
    private items: OrderLine[],
    private status: OrderStatus,
    private discount: Discount | null
  ) {}

  static create(customerId: CustomerId): Order {
    return new Order(
      OrderId.generate(),
      [],
      OrderStatus.DRAFT,
      null
    );
  }

  addItem(product: Product, quantity: Quantity): Order {
    // Business rule: max 10 items per product
    if (quantity.value > 10) {
      throw new DomainError('Cannot order more than 10 units');
    }

    const line = OrderLine.create(product, quantity);
    return new Order(this.id, [...this.items, line], this.status, this.discount);
  }

  applyVolumeDiscount(): Order {
    // Business rule: 10% discount for orders > 100€
    const total = this.calculateTotal();
    
    if (total.isGreaterThan(Money.euros(100))) {
      const discount = VolumeDiscount.tenPercent();
      return new Order(this.id, this.items, this.status, discount);
    }
    
    return this;
  }

  submit(): Order {
    // Business rule: cannot submit empty order
    if (this.items.length === 0) {
      throw new DomainError('Cannot submit empty order');
    }

    return new Order(this.id, this.items, OrderStatus.SUBMITTED, this.discount);
  }

  private calculateTotal(): Money {
    const subtotal = this.items.reduce(
      (sum, item) => sum.add(item.getTotalPrice()),
      Money.zero('EUR')
    );

    return this.discount 
      ? this.discount.applyTo(subtotal)
      : subtotal;
  }
}
```

**Avantages:**
- ✅ Vocabulaire métier clair (Order, OrderLine, VolumeDiscount)
- ✅ Règles métier explicites et testables
- ✅ Code compréhensible par les experts métier
- ✅ Base pour discussions avec le product owner

---

## Concepts Fondamentaux

### 1. Le Domaine

**Le domaine** = Le problème métier que vous résolvez.

**Exemples:**
- 🏦 **Banque:** Comptes, transactions, virements, prêts
- 🛒 **E-commerce:** Produits, commandes, panier, paiements
- 📚 **Bibliothèque:** Livres, emprunts, réservations, amendes
- 🏥 **Santé:** Patients, consultations, prescriptions, rendez-vous

### 2. Core Domain vs Supporting Domain vs Generic Domain

```
┌──────────────────────────────────────────────────┐
│              CORE DOMAIN                         │
│  Votre avantage compétitif unique                │
│  Ex: Algorithme de recommandation Netflix        │
│  → Investissement maximal                        │
└──────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────┐
│           SUPPORTING DOMAIN                      │
│  Nécessaire mais pas différenciant               │
│  Ex: Gestion des utilisateurs                    │
│  → Investissement modéré                         │
└──────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────┐
│            GENERIC DOMAIN                        │
│  Problème résolu (email, paiement)               │
│  Ex: Envoi d'emails, authentification            │
│  → Utiliser des librairies/SaaS                  │
└──────────────────────────────────────────────────┘
```

**Exemple E-commerce:**

- **Core Domain:** Moteur de pricing dynamique, système de fidélité
- **Supporting Domain:** Gestion du catalogue, suivi des commandes
- **Generic Domain:** Paiement (Stripe), Email (Sendgrid), Auth (Auth0)

### 3. Ubiquitous Language

**Langage partagé** entre développeurs et experts métier.

**❌ Sans Ubiquitous Language:**

```typescript
// Dev dit: "cart", "checkout", "process payment"
// Métier dit: "panier", "finaliser", "encaisser"

class ShoppingCart {
  async finalize() { // ← Quel terme utiliser?
    // ...
  }
}
```

**✅ Avec Ubiquitous Language:**

```typescript
// Tout le monde dit: "Order", "Submit", "Charge"

class Order {
  submit(): Order {
    // Terme utilisé par les experts métier
  }
}

// Dans les discussions:
// "Quand un Order est submitted, on charge le customer"
// ← Même vocabulaire dans le code et les réunions!
```

**Comment créer l'Ubiquitous Language:**

1. **Workshop avec experts métier** - Identifier les concepts clés
2. **Glossaire partagé** - Document vivant
3. **Utiliser dans le code** - Classes, méthodes, variables
4. **Utiliser dans les discussions** - Réunions, tickets, PRs
5. **Refactorer si le language évolue** - Le modèle suit le métier

---

## Building Blocks Tactiques

### 1. Entities (Entités)

**Définies par leur identité**, pas leurs attributs.

```typescript
// Une entité a une identité unique
class User {
  constructor(
    public readonly id: UserId, // ← Identité
    public readonly email: Email,
    public readonly name: string
  ) {}

  changeName(newName: string): User {
    return new User(
      this.id, // ← Même identité
      this.email,
      newName
    );
  }
}

// Deux users avec même email mais différent id = différents users
const user1 = new User(UserId.fromString('user_1'), Email.create('john@example.com'), 'John');
const user2 = new User(UserId.fromString('user_2'), Email.create('john@example.com'), 'John');

user1.id.equals(user2.id); // false ← Différentes entités!
```

### 2. Value Objects

**Définis par leurs valeurs**, sans identité.

```typescript
// Value Object: Money
class Money {
  private constructor(
    public readonly amount: number,
    public readonly currency: string
  ) {}

  static euros(amount: number): Money {
    return new Money(amount, 'EUR');
  }

  add(other: Money): Money {
    if (this.currency !== other.currency) {
      throw new Error('Cannot add different currencies');
    }
    return new Money(this.amount + other.amount, this.currency);
  }

  equals(other: Money): boolean {
    return this.amount === other.amount && this.currency === other.currency;
  }
}

// Deux Money avec mêmes valeurs = identiques
const price1 = Money.euros(10);
const price2 = Money.euros(10);

price1.equals(price2); // true ← Même valeur = identique!
```

**Caractéristiques des Value Objects:**

- ✅ **Immutables** - Toute opération retourne une nouvelle instance
- ✅ **Validation dans le constructeur** - Toujours valides
- ✅ **Pas d'identité** - Définis par leurs valeurs
- ✅ **Comparaison par valeur** - equals()

**Exemples courants:**

```typescript
class Email {
  private constructor(public readonly value: string) {}

  static create(email: string): Email {
    if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email)) {
      throw new DomainError('Invalid email format');
    }
    return new Email(email.toLowerCase());
  }
}

class DateRange {
  private constructor(
    public readonly start: Date,
    public readonly end: Date
  ) {}

  static create(start: Date, end: Date): DateRange {
    if (end <= start) {
      throw new DomainError('End must be after start');
    }
    return new DateRange(start, end);
  }

  getDurationInDays(): number {
    const diff = this.end.getTime() - this.start.getTime();
    return Math.ceil(diff / (1000 * 60 * 60 * 24));
  }

  contains(date: Date): boolean {
    return date >= this.start && date <= this.end;
  }
}

class Address {
  private constructor(
    public readonly street: string,
    public readonly city: string,
    public readonly zipCode: string,
    public readonly country: string
  ) {}

  static create(street: string, city: string, zipCode: string, country: string): Address {
    // Validations
    if (!street || !city || !zipCode || !country) {
      throw new DomainError('All address fields are required');
    }
    return new Address(street, city, zipCode, country);
  }

  toString(): string {
    return `${this.street}, ${this.zipCode} ${this.city}, ${this.country}`;
  }
}
```

### 3. Aggregates (Agrégats)

**Cluster d'objets** traités comme une unité pour les modifications.

**Règles:**
- 🔑 Un **Aggregate Root** (racine)
- 🚪 Accès externe **uniquement via la root**
- 🔒 **Cohérence transactionnelle** à l'intérieur
- 🆔 **Références entre aggregates** par ID uniquement

```typescript
// Aggregate: Order (root) + OrderLine (children)

class Order {  // ← AGGREGATE ROOT
  private constructor(
    public readonly id: OrderId,
    private items: OrderLine[], // ← Enfants
    private status: OrderStatus
  ) {}

  // ✅ Modification via la root
  addItem(product: Product, quantity: number): Order {
    const line = OrderLine.create(product, quantity);
    return new Order(
      this.id,
      [...this.items, line],
      this.status
    );
  }

  // ✅ La root garantit la cohérence
  removeItem(productId: ProductId): Order {
    const updatedItems = this.items.filter(
      item => !item.productId.equals(productId)
    );

    // Business rule: cannot remove all items from submitted order
    if (updatedItems.length === 0 && this.status === OrderStatus.SUBMITTED) {
      throw new DomainError('Cannot remove all items from submitted order');
    }

    return new Order(this.id, updatedItems, this.status);
  }

  // ✅ Encapsulation: items privés
  getItems(): ReadonlyArray<OrderLine> {
    return this.items;
  }

  getTotalPrice(): Money {
    return this.items.reduce(
      (sum, item) => sum.add(item.getTotalPrice()),
      Money.zero('EUR')
    );
  }
}

class OrderLine {  // ← Enfant (ne peut exister sans Order)
  constructor(
    public readonly productId: ProductId,
    public readonly productName: string,
    public readonly unitPrice: Money,
    public readonly quantity: number
  ) {}

  static create(product: Product, quantity: number): OrderLine {
    if (quantity <= 0) {
      throw new DomainError('Quantity must be positive');
    }
    return new OrderLine(
      product.id,
      product.name,
      product.price,
      quantity
    );
  }

  getTotalPrice(): Money {
    return this.unitPrice.multiply(this.quantity);
  }
}

// ❌ INTERDIT - Modifier un OrderLine directement
// const line = order.getItems()[0];
// line.quantity = 5; // ← NON!

// ✅ CORRECT - Passer par la root
const updatedOrder = order.updateItemQuantity(productId, 5);
```

**Références entre Aggregates:**

```typescript
// ✅ Référence par ID
class Order {
  constructor(
    public readonly id: OrderId,
    public readonly customerId: CustomerId, // ← ID, pas Customer entity!
    private items: OrderLine[]
  ) {}
}

// ❌ INTERDIT - Référence directe
class Order {
  constructor(
    public readonly id: OrderId,
    public readonly customer: Customer, // ← NON! Deux aggregates enchevêtrés
    private items: OrderLine[]
  ) {}
}

// Pour récupérer le Customer:
const order = await orderRepo.findById(orderId);
const customer = await customerRepo.findById(order.customerId); // ← Deux requêtes séparées
```

### 4. Services de Domaine

**Opérations qui n'appartiennent pas naturellement à une entité.**

```typescript
// ❌ Mettre dans une entité ne fait pas sens
class Order {
  calculateShippingCost(): Money {
    // Dépend de l'adresse, du poids, du transporteur...
    // Trop de responsabilités pour Order!
  }
}

// ✅ Service de domaine
class ShippingCostCalculator {
  constructor(
    private shippingRates: ShippingRateRepository
  ) {}

  calculate(order: Order, destination: Address): Money {
    const weight = this.calculateTotalWeight(order);
    const zone = this.getShippingZone(destination);
    const rate = this.shippingRates.getRateFor(zone, weight);
    
    return rate.calculateCost(weight);
  }

  private calculateTotalWeight(order: Order): Weight {
    // ...
  }

  private getShippingZone(address: Address): ShippingZone {
    // ...
  }
}

// Utilisation dans un use case:
class SubmitOrderUseCase {
  constructor(
    private orderRepo: IOrderRepository,
    private shippingCalculator: ShippingCostCalculator
  ) {}

  async execute(orderId: string, shippingAddress: Address): Promise<void> {
    const order = await this.orderRepo.findById(orderId);
    
    // Service domain calcule le coût
    const shippingCost = this.shippingCalculator.calculate(order, shippingAddress);
    
    const submittedOrder = order.submit(shippingCost);
    await this.orderRepo.save(submittedOrder);
  }
}
```

**Quand utiliser un Service de Domaine:**

- ✅ Opération impliquant **plusieurs aggregates**
- ✅ Calcul complexe nécessitant **données externes**
- ✅ Logique métier qui ne "colle" à **aucune entité**

### 5. Repositories

**Abstraction pour l'accès aux aggregates.**

```typescript
// Interface dans le domain layer
interface IOrderRepository {
  save(order: Order): Promise<void>;
  findById(id: OrderId): Promise<Order | null>;
  findByCustomer(customerId: CustomerId): Promise<Order[]>;
  findPendingOrders(): Promise<Order[]>;
}

// Implémentation dans infrastructure layer
class MongoOrderRepository implements IOrderRepository {
  constructor(private db: Db) {}

  async save(order: Order): Promise<void> {
    await this.db.collection('orders').updateOne(
      { _id: order.id.value },
      { $set: this.toDocument(order) },
      { upsert: true }
    );
  }

  async findById(id: OrderId): Promise<Order | null> {
    const doc = await this.db.collection('orders').findOne({ _id: id.value });
    return doc ? this.toDomain(doc) : null;
  }

  private toDocument(order: Order): any {
    // Convert domain model to DB model
  }

  private toDomain(doc: any): Order {
    // Reconstruct domain model from DB
  }
}
```

**Caractéristiques:**
- 🔍 **Collection-like interface** - findById, findAll, save
- 🏗️ **Encapsule la persistence** - Domain ne connaît pas la DB
- 🔄 **Conversion domain <-> DB** - Responsabilité du repo
- 📦 **Retourne des aggregates complets** - Pas de lazy loading

### 6. Factories

**Créer des objets complexes ou reconstruire depuis persistence.**

```typescript
// Factory pour création complexe
class OrderFactory {
  static createDraft(customerId: CustomerId): Order {
    return Order.create(customerId);
  }

  static createFromCart(cart: ShoppingCart, customerId: CustomerId): Order {
    const order = Order.create(customerId);
    
    // Conversion des items du cart en order lines
    for (const cartItem of cart.getItems()) {
      order = order.addItem(cartItem.product, cartItem.quantity);
    }

    return order;
  }

  // Reconstruction depuis DB
  static reconstitute(data: OrderData): Order {
    const items = data.items.map(itemData => 
      OrderLine.reconstitute(itemData)
    );

    return new Order(
      OrderId.fromString(data.id),
      CustomerId.fromString(data.customerId),
      items,
      OrderStatus.fromString(data.status),
      data.submittedAt ? new Date(data.submittedAt) : null
    );
  }
}
```

---

## Building Blocks Stratégiques

### 1. Bounded Context

**Zone avec un modèle cohérent et un langage uniforme.**

```
┌─────────────────────────────────────────────────────────────┐
│                      E-COMMERCE SYSTEM                      │
│                                                             │
│  ┌──────────────────┐  ┌──────────────────┐               │
│  │  SALES CONTEXT   │  │ INVENTORY CONTEXT│               │
│  │                  │  │                  │               │
│  │  • Order         │  │  • Product       │               │
│  │  • Customer      │  │  • Stock         │               │
│  │  • Payment       │  │  • Warehouse     │               │
│  │                  │  │                  │               │
│  │  "Order" =       │  │  "Product" =     │               │
│  │  Purchase from   │  │  Physical item   │               │
│  │  customer        │  │  in warehouse    │               │
│  └──────────────────┘  └──────────────────┘               │
│           │                      │                          │
│           └──────────┬───────────┘                          │
│                      ↓                                      │
│           ┌──────────────────┐                             │
│           │ SHIPPING CONTEXT │                             │
│           │                  │                             │
│           │  • Shipment      │                             │
│           │  • Package       │                             │
│           │  • Tracking      │                             │
│           │                  │                             │
│           │  "Order" =       │                             │
│           │  Items to ship   │                             │
│           └──────────────────┘                             │
└─────────────────────────────────────────────────────────────┘
```

**Même terme, significations différentes selon le context:**

```typescript
// SALES CONTEXT
class Order {
  constructor(
    public readonly id: OrderId,
    public readonly customerId: CustomerId,
    private items: OrderLine[],
    private paymentStatus: PaymentStatus // ← Focus: paiement
  ) {}

  calculateTotal(): Money {
    // Calcul du montant à payer
  }
}

// SHIPPING CONTEXT
class Order {
  constructor(
    public readonly id: OrderId,
    private packages: Package[], // ← Focus: expédition
    private shippingAddress: Address,
    private trackingNumber: TrackingNumber
  ) {}

  canBeShipped(): boolean {
    // Vérifier si prêt à expédier
  }
}

// INVENTORY CONTEXT
class Order {
  constructor(
    public readonly id: OrderId,
    private reservations: StockReservation[] // ← Focus: stock
  ) {}

  reserveStock(): void {
    // Réserver les produits
  }
}
```

**Communication entre Contexts:**

```typescript
// Sales Context publie un événement
class OrderPlacedEvent {
  constructor(
    public readonly orderId: string,
    public readonly customerId: string,
    public readonly items: { productId: string; quantity: number }[]
  ) {}
}

// Shipping Context écoute l'événement
class CreateShipmentWhenOrderPlaced {
  constructor(
    private shipmentService: ShipmentService
  ) {}

  async handle(event: OrderPlacedEvent): Promise<void> {
    // Créer un Shipment dans le Shipping Context
    await this.shipmentService.createShipmentFromOrder(event.orderId);
  }
}
```

### 2. Context Mapping

**Relations entre Bounded Contexts.**

#### Partnership
Deux équipes collaborent étroitement.

```
┌──────────────┐     ┌──────────────┐
│  Context A   │◄───►│  Context B   │
│              │     │              │
└──────────────┘     └──────────────┘
     Collaboration mutuelle
```

#### Customer-Supplier
Une équipe fournit, l'autre consomme.

```
┌──────────────┐     ┌──────────────┐
│  Supplier    │────►│  Customer    │
│  Context     │     │  Context     │
└──────────────┘     └──────────────┘
     API définie par Supplier
```

#### Conformist
Le downstream accepte le modèle de l'upstream.

```
┌──────────────┐     ┌──────────────┐
│  Upstream    │────►│ Downstream   │
│              │     │ (Conformist) │
└──────────────┘     └──────────────┘
     Downstream utilise modèle upstream tel quel
```

#### Anti-Corruption Layer (ACL)
Traduction entre deux modèles.

```
┌──────────────┐     ┌─────┐     ┌──────────────┐
│  External    │────►│ ACL │────►│ Your Context │
│  System      │     └─────┘     │              │
└──────────────┘    Translation  └──────────────┘
```

**Exemple ACL:**

```typescript
// External API model (legacy)
interface LegacyOrderDTO {
  order_id: string;
  cust_id: string;
  total_amt: number;
  status_cd: string;
}

// Your domain model
class Order {
  constructor(
    public readonly id: OrderId,
    public readonly customerId: CustomerId,
    private total: Money,
    private status: OrderStatus
  ) {}
}

// Anti-Corruption Layer
class OrderACL {
  toDomain(dto: LegacyOrderDTO): Order {
    return new Order(
      OrderId.fromString(dto.order_id),
      CustomerId.fromString(dto.cust_id),
      Money.euros(dto.total_amt),
      this.mapStatus(dto.status_cd)
    );
  }

  toExternal(order: Order): LegacyOrderDTO {
    return {
      order_id: order.id.value,
      cust_id: order.customerId.value,
      total_amt: order.getTotal().amount,
      status_cd: this.mapStatusToCode(order.status)
    };
  }

  private mapStatus(code: string): OrderStatus {
    const mapping: Record<string, OrderStatus> = {
      'P': OrderStatus.PENDING,
      'C': OrderStatus.CONFIRMED,
      'X': OrderStatus.CANCELLED
    };
    return mapping[code] || OrderStatus.PENDING;
  }
}
```

---

## Domain Events

**Événements significatifs dans le domaine.**

### Caractéristiques

- 📅 **Passé** - "OrderPlaced", pas "PlaceOrder"
- 🔒 **Immutables** - Ne peuvent pas être modifiés
- 📊 **Contiennent les données nécessaires** - Pas juste un ID
- ⏰ **Horodatés** - Savoir quand c'est arrivé

### Exemple

```typescript
// Domain Event
interface DomainEvent {
  eventId: string;
  occurredOn: Date;
  eventType: string;
}

class OrderPlacedEvent implements DomainEvent {
  public readonly eventId: string;
  public readonly occurredOn: Date;
  public readonly eventType = 'OrderPlaced';

  constructor(
    public readonly orderId: OrderId,
    public readonly customerId: CustomerId,
    public readonly totalAmount: Money,
    public readonly items: { productId: string; quantity: number }[]
  ) {
    this.eventId = `event_${Date.now()}_${Math.random()}`;
    this.occurredOn = new Date();
  }
}

class OrderCancelledEvent implements DomainEvent {
  public readonly eventId: string;
  public readonly occurredOn: Date;
  public readonly eventType = 'OrderCancelled';

  constructor(
    public readonly orderId: OrderId,
    public readonly reason: string,
    public readonly refundAmount: Money
  ) {
    this.eventId = `event_${Date.now()}_${Math.random()}`;
    this.occurredOn = new Date();
  }
}

// Aggregate produit des événements
class Order {
  private uncommittedEvents: DomainEvent[] = [];

  place(): Order {
    // Business logic
    const placedOrder = new Order(
      this.id,
      this.customerId,
      this.items,
      OrderStatus.PLACED
    );

    // Enregistrer l'événement
    placedOrder.uncommittedEvents.push(
      new OrderPlacedEvent(
        this.id,
        this.customerId,
        this.calculateTotal(),
        this.items.map(item => ({
          productId: item.productId.value,
          quantity: item.quantity
        }))
      )
    );

    return placedOrder;
  }

  getUncommittedEvents(): DomainEvent[] {
    return [...this.uncommittedEvents];
  }

  clearEvents(): void {
    this.uncommittedEvents = [];
  }
}

// Repository publie les événements
class OrderRepository implements IOrderRepository {
  constructor(
    private db: Db,
    private eventBus: IEventBus
  ) {}

  async save(order: Order): Promise<void> {
    // 1. Sauvegarder l'aggregate
    await this.db.collection('orders').updateOne(
      { _id: order.id.value },
      { $set: this.toDocument(order) },
      { upsert: true }
    );

    // 2. Publier les événements
    const events = order.getUncommittedEvents();
    for (const event of events) {
      await this.eventBus.publish(event);
    }

    // 3. Nettoyer
    order.clearEvents();
  }
}

// Event Handlers
class SendOrderConfirmationEmail {
  constructor(private emailService: IEmailService) {}

  async handle(event: OrderPlacedEvent): Promise<void> {
    await this.emailService.send({
      to: event.customerId.value,
      subject: 'Order Confirmation',
      body: `Your order ${event.orderId.value} has been placed!`
    });
  }
}

class ReserveInventory {
  constructor(private inventoryService: IInventoryService) {}

  async handle(event: OrderPlacedEvent): Promise<void> {
    for (const item of event.items) {
      await this.inventoryService.reserve(item.productId, item.quantity);
    }
  }
}
```

### Avantages des Domain Events

- ✅ **Découplage** - Contexts communiquent sans dépendances directes
- ✅ **Auditabilité** - Historique des changements
- ✅ **Intégration** - Communication entre systèmes
- ✅ **Eventual Consistency** - Propagation asynchrone
- ✅ **Event Sourcing** - Stocker les événements comme source de vérité

---

## Cas Pratiques

### Cas 1: E-commerce - Core Domain

**Bounded Contexts identifiés:**

1. **Catalog Context** (Supporting)
   - Product, Category, SearchIndex

2. **Order Context** (Core) ⭐
   - Order, OrderLine, PricingRules, Discount

3. **Inventory Context** (Supporting)
   - Stock, Warehouse, Reservation

4. **Shipping Context** (Supporting)
   - Shipment, Package, Tracking

5. **Payment Context** (Generic - use Stripe)

**Modèle du Core Domain (Order Context):**

```typescript
// Aggregate Root
class Order {
  private constructor(
    public readonly id: OrderId,
    public readonly customerId: CustomerId,
    private items: OrderLine[],
    private pricing: OrderPricing,
    private status: OrderStatus,
    private placedAt: Date | null
  ) {}

  static create(customerId: CustomerId): Order {
    return new Order(
      OrderId.generate(),
      customerId,
      [],
      OrderPricing.standard(),
      OrderStatus.DRAFT,
      null
    );
  }

  addItem(product: Product, quantity: Quantity): Order {
    // Business rule: max 10 different products
    if (this.items.length >= 10) {
      throw new DomainError('Cannot add more than 10 different products');
    }

    // Business rule: max 99 units per product
    if (quantity.value > 99) {
      throw new DomainError('Cannot order more than 99 units');
    }

    const existingItem = this.items.find(item => 
      item.productId.equals(product.id)
    );

    if (existingItem) {
      // Update quantity
      const updatedItems = this.items.map(item =>
        item.productId.equals(product.id)
          ? item.changeQuantity(quantity)
          : item
      );
      return new Order(this.id, this.customerId, updatedItems, this.pricing, this.status, this.placedAt);
    }

    // Add new item
    const line = OrderLine.create(product, quantity);
    return new Order(
      this.id,
      this.customerId,
      [...this.items, line],
      this.pricing,
      this.status,
      this.placedAt
    );
  }

  applyVolumeDiscount(): Order {
    const subtotal = this.calculateSubtotal();
    
    // Business rule: 10% discount for orders > 100€
    if (subtotal.isGreaterThan(Money.euros(100))) {
      const discountedPricing = this.pricing.withVolumeDiscount(Percentage.of(10));
      return new Order(this.id, this.customerId, this.items, discountedPricing, this.status, this.placedAt);
    }

    return this;
  }

  place(): Order {
    // Business rule: cannot place empty order
    if (this.items.length === 0) {
      throw new DomainError('Cannot place empty order');
    }

    // Business rule: can only place draft orders
    if (this.status !== OrderStatus.DRAFT) {
      throw new DomainError('Only draft orders can be placed');
    }

    return new Order(
      this.id,
      this.customerId,
      this.items,
      this.pricing,
      OrderStatus.PLACED,
      new Date()
    );
  }

  private calculateSubtotal(): Money {
    return this.items.reduce(
      (sum, item) => sum.add(item.getTotalPrice()),
      Money.zero('EUR')
    );
  }

  getTotal(): Money {
    const subtotal = this.calculateSubtotal();
    return this.pricing.calculateTotal(subtotal);
  }
}

// Value Objects
class OrderPricing {
  private constructor(
    private readonly discount: Discount | null,
    private readonly taxRate: TaxRate
  ) {}

  static standard(): OrderPricing {
    return new OrderPricing(null, TaxRate.standard());
  }

  withVolumeDiscount(percentage: Percentage): OrderPricing {
    return new OrderPricing(
      VolumeDiscount.create(percentage),
      this.taxRate
    );
  }

  calculateTotal(subtotal: Money): Money {
    const afterDiscount = this.discount
      ? this.discount.applyTo(subtotal)
      : subtotal;
    
    return this.taxRate.applyTo(afterDiscount);
  }
}

class Quantity {
  private constructor(public readonly value: number) {}

  static of(value: number): Quantity {
    if (value <= 0) throw new DomainError('Quantity must be positive');
    if (!Number.isInteger(value)) throw new DomainError('Quantity must be integer');
    return new Quantity(value);
  }
}
```

---

## Anti-Patterns

### 1. ❌ Anemic Domain Model

```typescript
// ❌ MAUVAIS - Entité sans comportement
class Order {
  id: string;
  customerId: string;
  items: OrderItem[];
  status: string;
  total: number;
}

class OrderService {
  placeOrder(order: Order) {
    // Toute la logique dans le service!
    if (order.items.length === 0) throw new Error('Empty order');
    
    order.total = order.items.reduce((sum, item) => sum + item.price * item.quantity, 0);
    order.status = 'PLACED';
    
    return order;
  }
}

// ✅ BON - Entité riche
class Order {
  private constructor(
    public readonly id: OrderId,
    private items: OrderLine[],
    private status: OrderStatus
  ) {}

  place(): Order {
    if (this.items.length === 0) {
      throw new DomainError('Cannot place empty order');
    }
    
    return new Order(this.id, this.items, OrderStatus.PLACED);
  }

  getTotal(): Money {
    return this.items.reduce(
      (sum, item) => sum.add(item.getTotalPrice()),
      Money.zero('EUR')
    );
  }
}
```

### 2. ❌ Aggregates Trop Gros

```typescript
// ❌ MAUVAIS - Aggregate énorme
class Customer {
  id: CustomerId;
  email: Email;
  orders: Order[]; // ← Tous les orders!
  addresses: Address[]; // ← Toutes les adresses!
  paymentMethods: PaymentMethod[]; // ← Tous les moyens de paiement!
  wishlist: Product[]; // ← Toute la wishlist!
  reviews: Review[]; // ← Tous les reviews!
}

// ✅ BON - Aggregates séparés
class Customer {
  id: CustomerId;
  email: Email;
  // Référence les autres aggregates par ID
}

class CustomerOrders {
  customerId: CustomerId;
  orders: OrderId[]; // ← Juste les IDs
}

class CustomerWishlist {
  customerId: CustomerId;
  items: ProductId[];
}
```

### 3. ❌ Violation du Bounded Context

```typescript
// ❌ MAUVAIS - Mélange de contexts
class Order {
  // Sales Context
  customerId: CustomerId;
  total: Money;
  
  // Shipping Context mélangé!
  trackingNumber: TrackingNumber;
  shippingCarrier: string;
  
  // Inventory Context mélangé!
  warehouseId: WarehouseId;
  stockReservations: Reservation[];
}

// ✅ BON - Contexts séparés
// Sales Context
class Order {
  customerId: CustomerId;
  total: Money;
  status: OrderStatus;
}

// Shipping Context
class Shipment {
  orderId: OrderId; // ← Référence
  trackingNumber: TrackingNumber;
  carrier: Carrier;
}

// Inventory Context
class StockReservation {
  orderId: OrderId; // ← Référence
  warehouseId: WarehouseId;
  items: ReservedItem[];
}
```

### 4. ❌ Technical Language au lieu d'Ubiquitous Language

```typescript
// ❌ MAUVAIS - Jargon technique
class DataManager {
  processData(input: any) {
    // What is "process"? What is "data"?
  }
}

// ✅ BON - Langage métier
class Order {
  place() {
    // Clear: "place an order"
  }

  cancel(reason: CancellationReason) {
    // Clear: "cancel an order"
  }
}
```

---

## 📚 Ressources

- **Domain-Driven Design** by Eric Evans (Blue Book)
- **Implementing Domain-Driven Design** by Vaughn Vernon (Red Book)
- **Domain-Driven Design Distilled** by Vaughn Vernon (Quick intro)

---

**Prochaine étape:** Voir les exercices pratiques dans `WEB_DDD_EXERCISES.md`
