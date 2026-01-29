// ============================================
// VALUE OBJECTS - Définis par leurs valeurs, immutables
// ============================================

/**
 * Value Object: Money
 * Représente une somme d'argent avec devise
 */
export class Money {
  private constructor(
    public readonly amount: number,
    public readonly currency: string
  ) {}

  static create(amount: number, currency: string): Money {
    if (amount < 0) {
      throw new Error("Amount cannot be negative");
    }
    if (!["EUR", "USD", "GBP"].includes(currency)) {
      throw new Error("Unsupported currency");
    }
    return new Money(amount, currency);
  }

  add(other: Money): Money {
    if (this.currency !== other.currency) {
      throw new Error("Cannot add money with different currencies");
    }
    return new Money(this.amount + other.amount, this.currency);
  }

  multiply(factor: number): Money {
    return new Money(this.amount * factor, this.currency);
  }

  equals(other: Money): boolean {
    return this.amount === other.amount && this.currency === other.currency;
  }

  toString(): string {
    return `${this.amount.toFixed(2)} ${this.currency}`;
  }
}

/**
 * Value Object: ProductId
 */
export class ProductId {
  private constructor(public readonly value: string) {}

  static create(value: string): ProductId {
    if (!value || value.trim().length === 0) {
      throw new Error("ProductId cannot be empty");
    }
    return new ProductId(value);
  }

  equals(other: ProductId): boolean {
    return this.value === other.value;
  }
}

/**
 * Value Object: CustomerId
 */
export class CustomerId {
  private constructor(public readonly value: string) {}

  static create(value: string): CustomerId {
    if (!value || value.trim().length === 0) {
      throw new Error("CustomerId cannot be empty");
    }
    return new CustomerId(value);
  }

  equals(other: CustomerId): boolean {
    return this.value === other.value;
  }
}

/**
 * Value Object: OrderId
 */
export class OrderId {
  private constructor(public readonly value: string) {}

  static create(): OrderId {
    const id = `ORD-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`;
    return new OrderId(id);
  }

  static fromString(value: string): OrderId {
    if (!value || !value.startsWith("ORD-")) {
      throw new Error("Invalid OrderId format");
    }
    return new OrderId(value);
  }

  equals(other: OrderId): boolean {
    return this.value === other.value;
  }
}

/**
 * Value Object: Address
 */
export class Address {
  private constructor(
    public readonly street: string,
    public readonly city: string,
    public readonly zipCode: string,
    public readonly country: string
  ) {}

  static create(
    street: string,
    city: string,
    zipCode: string,
    country: string
  ): Address {
    if (!street || !city || !zipCode || !country) {
      throw new Error("All address fields are required");
    }
    return new Address(street, city, zipCode, country);
  }

  toString(): string {
    return `${this.street}, ${this.zipCode} ${this.city}, ${this.country}`;
  }
}

// ============================================
// ENTITIES - Objets avec identité
// ============================================

/**
 * Entity: OrderLine
 * Ligne de commande dans l'aggregate Order
 */
export class OrderLine {
  constructor(
    public readonly productId: ProductId,
    public readonly productName: string,
    public readonly unitPrice: Money,
    public readonly quantity: number
  ) {
    if (quantity <= 0) {
      throw new Error("Quantity must be positive");
    }
  }

  getTotalPrice(): Money {
    return this.unitPrice.multiply(this.quantity);
  }

  changeQuantity(newQuantity: number): OrderLine {
    return new OrderLine(
      this.productId,
      this.productName,
      this.unitPrice,
      newQuantity
    );
  }
}

// ============================================
// DOMAIN EVENTS - Événements métier
// ============================================

export interface DomainEvent {
  occurredOn: Date;
  eventType: string;
}

export class OrderPlacedEvent implements DomainEvent {
  public readonly occurredOn: Date;
  public readonly eventType = "OrderPlaced";

  constructor(
    public readonly orderId: OrderId,
    public readonly customerId: CustomerId,
    public readonly totalAmount: Money
  ) {
    this.occurredOn = new Date();
  }
}

export class OrderCancelledEvent implements DomainEvent {
  public readonly occurredOn: Date;
  public readonly eventType = "OrderCancelled";

  constructor(
    public readonly orderId: OrderId,
    public readonly reason: string
  ) {
    this.occurredOn = new Date();
  }
}

export class OrderShippedEvent implements DomainEvent {
  public readonly occurredOn: Date;
  public readonly eventType = "OrderShipped";

  constructor(
    public readonly orderId: OrderId,
    public readonly trackingNumber: string
  ) {
    this.occurredOn = new Date();
  }
}

// ============================================
// AGGREGATE ROOT - Order
// ============================================

export enum OrderStatus {
  PENDING = "PENDING",
  CONFIRMED = "CONFIRMED",
  SHIPPED = "SHIPPED",
  DELIVERED = "DELIVERED",
  CANCELLED = "CANCELLED",
}

/**
 * Aggregate Root: Order
 * Garantit la cohérence de toutes les données liées à une commande
 */
export class Order {
  private domainEvents: DomainEvent[] = [];

  private constructor(
    public readonly id: OrderId,
    public readonly customerId: CustomerId,
    public readonly shippingAddress: Address,
    private orderLines: OrderLine[],
    private status: OrderStatus,
    public readonly createdAt: Date
  ) {}

  /**
   * Factory method pour créer une nouvelle commande
   */
  static create(
    customerId: CustomerId,
    shippingAddress: Address,
    orderLines: OrderLine[]
  ): Order {
    if (orderLines.length === 0) {
      throw new Error("Order must have at least one line");
    }

    const order = new Order(
      OrderId.create(),
      customerId,
      shippingAddress,
      orderLines,
      OrderStatus.PENDING,
      new Date()
    );

    // Émettre événement de domaine
    order.domainEvents.push(
      new OrderPlacedEvent(order.id, customerId, order.getTotalAmount())
    );

    return order;
  }

  /**
   * Reconstituer depuis persistance
   */
  static reconstitute(
    id: OrderId,
    customerId: CustomerId,
    shippingAddress: Address,
    orderLines: OrderLine[],
    status: OrderStatus,
    createdAt: Date
  ): Order {
    return new Order(
      id,
      customerId,
      shippingAddress,
      orderLines,
      status,
      createdAt
    );
  }

  // ============================================
  // BUSINESS LOGIC - Règles métier
  // ============================================

  /**
   * Confirmer la commande
   */
  confirm(): void {
    if (this.status !== OrderStatus.PENDING) {
      throw new Error("Only pending orders can be confirmed");
    }
    this.status = OrderStatus.CONFIRMED;
  }

  /**
   * Expédier la commande
   */
  ship(trackingNumber: string): void {
    if (this.status !== OrderStatus.CONFIRMED) {
      throw new Error("Only confirmed orders can be shipped");
    }
    if (!trackingNumber) {
      throw new Error("Tracking number is required");
    }

    this.status = OrderStatus.SHIPPED;
    this.domainEvents.push(new OrderShippedEvent(this.id, trackingNumber));
  }

  /**
   * Annuler la commande
   */
  cancel(reason: string): void {
    if (this.status === OrderStatus.SHIPPED) {
      throw new Error("Cannot cancel shipped order");
    }
    if (this.status === OrderStatus.DELIVERED) {
      throw new Error("Cannot cancel delivered order");
    }
    if (this.status === OrderStatus.CANCELLED) {
      throw new Error("Order is already cancelled");
    }

    this.status = OrderStatus.CANCELLED;
    this.domainEvents.push(new OrderCancelledEvent(this.id, reason));
  }

  /**
   * Ajouter une ligne de commande (seulement si PENDING)
   */
  addLine(line: OrderLine): void {
    if (this.status !== OrderStatus.PENDING) {
      throw new Error("Cannot modify confirmed or shipped orders");
    }
    this.orderLines.push(line);
  }

  /**
   * Supprimer une ligne de commande
   */
  removeLine(productId: ProductId): void {
    if (this.status !== OrderStatus.PENDING) {
      throw new Error("Cannot modify confirmed or shipped orders");
    }

    const index = this.orderLines.findIndex((line) =>
      line.productId.equals(productId)
    );

    if (index === -1) {
      throw new Error("Product not found in order");
    }

    this.orderLines.splice(index, 1);

    if (this.orderLines.length === 0) {
      throw new Error("Order must have at least one line");
    }
  }

  // ============================================
  // QUERIES - Interrogations
  // ============================================

  getTotalAmount(): Money {
    return this.orderLines.reduce(
      (total, line) => total.add(line.getTotalPrice()),
      Money.create(0, this.orderLines[0].unitPrice.currency)
    );
  }

  getStatus(): OrderStatus {
    return this.status;
  }

  getOrderLines(): readonly OrderLine[] {
    return [...this.orderLines]; // Return copy to preserve encapsulation
  }

  /**
   * Récupérer et vider les événements de domaine
   */
  pullDomainEvents(): DomainEvent[] {
    const events = [...this.domainEvents];
    this.domainEvents = [];
    return events;
  }

  /**
   * Vérifier si la commande peut être modifiée
   */
  canBeModified(): boolean {
    return this.status === OrderStatus.PENDING;
  }
}

// ============================================
// DOMAIN SERVICE - Logique métier complexe
// ============================================

/**
 * Domain Service: Order Pricing
 * Calcul de prix complexe qui ne rentre pas dans Order
 */
export class OrderPricingService {
  constructor(private readonly taxRate: number = 0.2) {}

  calculateTotalWithTax(order: Order): Money {
    const subtotal = order.getTotalAmount();
    const tax = subtotal.multiply(this.taxRate);
    return subtotal.add(tax);
  }

  calculateShippingCost(order: Order): Money {
    const total = order.getTotalAmount();
    const currency = total.currency;

    // Règle métier: livraison gratuite au-dessus de 100
    if (total.amount >= 100) {
      return Money.create(0, currency);
    }

    // Sinon 10 EUR de frais
    return Money.create(10, currency);
  }

  calculateFinalTotal(order: Order): Money {
    const totalWithTax = this.calculateTotalWithTax(order);
    const shipping = this.calculateShippingCost(order);
    return totalWithTax.add(shipping);
  }
}

// ============================================
// REPOSITORY INTERFACE
// ============================================

export interface IOrderRepository {
  save(order: Order): Promise<void>;
  findById(id: OrderId): Promise<Order | null>;
  findByCustomerId(customerId: CustomerId): Promise<Order[]>;
}

/**
 * In-Memory Implementation (pour tests/démo)
 */
export class InMemoryOrderRepository implements IOrderRepository {
  private orders: Map<string, Order> = new Map();

  async save(order: Order): Promise<void> {
    this.orders.set(order.id.value, order);

    // En production: persister les événements de domaine
    const events = order.pullDomainEvents();
    events.forEach((event) => {
      console.log(`📢 Domain Event: ${event.eventType}`, event);
    });
  }

  async findById(id: OrderId): Promise<Order | null> {
    return this.orders.get(id.value) || null;
  }

  async findByCustomerId(customerId: CustomerId): Promise<Order[]> {
    const result: Order[] = [];
    for (const order of this.orders.values()) {
      if (order.customerId.equals(customerId)) {
        result.push(order);
      }
    }
    return result;
  }
}

// ============================================
// EXEMPLE D'UTILISATION
// ============================================

async function main() {
  console.log("🛒 Domain-Driven Design - E-Commerce Example\n");

  const repository = new InMemoryOrderRepository();
  const pricingService = new OrderPricingService(0.2); // 20% tax

  try {
    // 1. Créer des value objects
    const customerId = CustomerId.create("CUST-001");
    const address = Address.create(
      "123 Main St",
      "Paris",
      "75001",
      "France"
    );

    // 2. Créer des lignes de commande
    const line1 = new OrderLine(
      ProductId.create("PROD-001"),
      "Laptop",
      Money.create(999.99, "EUR"),
      1
    );

    const line2 = new OrderLine(
      ProductId.create("PROD-002"),
      "Mouse",
      Money.create(29.99, "EUR"),
      2
    );

    // 3. Créer l'aggregate Order
    console.log("📝 Creating order...");
    const order = Order.create(customerId, address, [line1, line2]);
    console.log(`✅ Order created: ${order.id.value}`);
    console.log(`   Subtotal: ${order.getTotalAmount()}`);
    console.log(`   Status: ${order.getStatus()}`);
    console.log();

    // 4. Calculer le prix total avec service de domaine
    const finalTotal = pricingService.calculateFinalTotal(order);
    console.log(`💰 Pricing calculation:`);
    console.log(`   Total with tax: ${pricingService.calculateTotalWithTax(order)}`);
    console.log(`   Shipping: ${pricingService.calculateShippingCost(order)}`);
    console.log(`   Final total: ${finalTotal}`);
    console.log();

    // 5. Persister (émet les événements)
    await repository.save(order);
    console.log();

    // 6. Workflow: Confirmer -> Expédier
    console.log("📦 Processing order workflow...");
    order.confirm();
    console.log(`✅ Order confirmed`);

    order.ship("TRACK-123456");
    console.log(`✅ Order shipped with tracking: TRACK-123456`);

    await repository.save(order);
    console.log();

    // 7. Tentative d'annulation (devrait échouer)
    console.log("❌ Trying to cancel shipped order...");
    try {
      order.cancel("Changed mind");
    } catch (error) {
      console.log(`✅ Caught expected error: ${(error as Error).message}`);
    }
    console.log();

    // 8. Créer et annuler une autre commande
    console.log("📝 Creating another order to cancel...");
    const order2 = Order.create(customerId, address, [line1]);
    console.log(`✅ Order created: ${order2.id.value}`);

    order2.cancel("Out of stock");
    console.log(`✅ Order cancelled`);
    await repository.save(order2);
    console.log();

    // 9. Rechercher les commandes du client
    console.log("🔍 Finding all orders for customer...");
    const customerOrders = await repository.findByCustomerId(customerId);
    console.log(`✅ Found ${customerOrders.length} orders:`);
    customerOrders.forEach((o) => {
      console.log(`   - ${o.id.value}: ${o.getStatus()} (${o.getTotalAmount()})`);
    });
  } catch (error) {
    console.error("❌ Error:", error);
  }

  console.log("\n🎉 DDD Demo complete!");
}

// Run if this is the main module
if (require.main === module) {
  main();
}

// ============================================
// PRINCIPES DDD APPLIQUÉS
// ============================================

/*
✅ VALUE OBJECTS
- Immutables (Money, Address, ProductId)
- Égalité par valeur, pas par référence
- Auto-validants (validation dans constructor)

✅ ENTITIES
- Identité unique (Order, OrderLine)
- Peuvent changer d'état
- Logique métier encapsulée

✅ AGGREGATE
- Order est l'aggregate root
- OrderLine accessible uniquement via Order
- Cohérence transactionnelle garantie

✅ DOMAIN EVENTS
- Capture des événements métier significatifs
- Découplage entre aggregates
- Base pour Event Sourcing

✅ UBIQUITOUS LANGUAGE
- Termes métier partout (Order, Ship, Confirm)
- Code lisible par le métier
- Pas de leak technique dans le domaine

✅ INVARIANTS
- Règles métier toujours respectées
- "Order must have at least one line"
- "Cannot ship unconfirmed order"

❌ ANTI-PATTERNS
- Anemic Domain Model (getters/setters sans logique)
- Aggregate trop gros (> 3 niveaux)
- Logique métier dans services applicatifs
- Entities couplées à l'ORM
*/
