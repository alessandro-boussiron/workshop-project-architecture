// ============================================
// EVENTS - Source of truth
// ============================================

export interface Event {
  eventId: string;
  aggregateId: string;
  eventType: string;
  timestamp: Date;
  version: number;
}

export interface AccountCreatedEvent extends Event {
  eventType: "AccountCreated";
  ownerId: string;
  initialBalance: number;
}

export interface MoneyDepositedEvent extends Event {
  eventType: "MoneyDeposited";
  amount: number;
  balance: number;
}

export interface MoneyWithdrawnEvent extends Event {
  eventType: "MoneyWithdrawn";
  amount: number;
  balance: number;
}

export interface AccountClosedEvent extends Event {
  eventType: "AccountClosed";
  reason: string;
}

// ============================================
// COMMANDS - Write operations
// ============================================

export interface Command {
  commandId: string;
  timestamp: Date;
}

export interface CreateAccountCommand extends Command {
  accountId: string;
  ownerId: string;
  initialBalance: number;
}

export interface DepositMoneyCommand extends Command {
  accountId: string;
  amount: number;
}

export interface WithdrawMoneyCommand extends Command {
  accountId: string;
  amount: number;
}

export interface CloseAccountCommand extends Command {
  accountId: string;
  reason: string;
}

// ============================================
// WRITE MODEL - Aggregate
// ============================================

export class BankAccount {
  private uncommittedEvents: Event[] = [];

  constructor(
    public readonly id: string,
    private ownerId: string,
    private balance: number,
    private isActive: boolean,
    private version: number = 0
  ) {}

  // Factory method
  static create(accountId: string, ownerId: string, initialBalance: number): BankAccount {
    if (initialBalance < 0) {
      throw new Error("Initial balance cannot be negative");
    }

    const account = new BankAccount(accountId, ownerId, 0, true, 0);

    // Apply event
    const event: AccountCreatedEvent = {
      eventId: generateId(),
      aggregateId: accountId,
      eventType: "AccountCreated",
      timestamp: new Date(),
      version: 1,
      ownerId,
      initialBalance,
    };

    account.applyEvent(event);
    account.uncommittedEvents.push(event);

    return account;
  }

  // Commands
  deposit(amount: number): void {
    if (!this.isActive) {
      throw new Error("Cannot deposit to closed account");
    }
    if (amount <= 0) {
      throw new Error("Deposit amount must be positive");
    }

    const newBalance = this.balance + amount;
    const event: MoneyDepositedEvent = {
      eventId: generateId(),
      aggregateId: this.id,
      eventType: "MoneyDeposited",
      timestamp: new Date(),
      version: this.version + 1,
      amount,
      balance: newBalance,
    };

    this.applyEvent(event);
    this.uncommittedEvents.push(event);
  }

  withdraw(amount: number): void {
    if (!this.isActive) {
      throw new Error("Cannot withdraw from closed account");
    }
    if (amount <= 0) {
      throw new Error("Withdrawal amount must be positive");
    }
    if (this.balance < amount) {
      throw new Error("Insufficient funds");
    }

    const newBalance = this.balance - amount;
    const event: MoneyWithdrawnEvent = {
      eventId: generateId(),
      aggregateId: this.id,
      eventType: "MoneyWithdrawn",
      timestamp: new Date(),
      version: this.version + 1,
      amount,
      balance: newBalance,
    };

    this.applyEvent(event);
    this.uncommittedEvents.push(event);
  }

  close(reason: string): void {
    if (!this.isActive) {
      throw new Error("Account is already closed");
    }
    if (this.balance > 0) {
      throw new Error("Cannot close account with positive balance");
    }

    const event: AccountClosedEvent = {
      eventId: generateId(),
      aggregateId: this.id,
      eventType: "AccountClosed",
      timestamp: new Date(),
      version: this.version + 1,
      reason,
    };

    this.applyEvent(event);
    this.uncommittedEvents.push(event);
  }

  // Event application (state changes)
  private applyEvent(event: Event): void {
    switch (event.eventType) {
      case "AccountCreated":
        const created = event as AccountCreatedEvent;
        this.balance = created.initialBalance;
        this.isActive = true;
        break;

      case "MoneyDeposited":
        const deposited = event as MoneyDepositedEvent;
        this.balance = deposited.balance;
        break;

      case "MoneyWithdrawn":
        const withdrawn = event as MoneyWithdrawnEvent;
        this.balance = withdrawn.balance;
        break;

      case "AccountClosed":
        this.isActive = false;
        break;
    }

    this.version = event.version;
  }

  // Reconstitute from events (Event Sourcing)
  static fromEvents(events: Event[]): BankAccount {
    if (events.length === 0) {
      throw new Error("No events to reconstitute");
    }

    const firstEvent = events[0] as AccountCreatedEvent;
    const account = new BankAccount(
      firstEvent.aggregateId,
      firstEvent.ownerId,
      0,
      false,
      0
    );

    events.forEach((event) => account.applyEvent(event));
    return account;
  }

  getUncommittedEvents(): Event[] {
    return [...this.uncommittedEvents];
  }

  clearUncommittedEvents(): void {
    this.uncommittedEvents = [];
  }

  // Getters for read
  getBalance(): number {
    return this.balance;
  }

  getIsActive(): boolean {
    return this.isActive;
  }

  getVersion(): number {
    return this.version;
  }
}

// ============================================
// EVENT STORE - Persistence
// ============================================

export interface IEventStore {
  saveEvents(aggregateId: string, events: Event[]): Promise<void>;
  getEvents(aggregateId: string): Promise<Event[]>;
}

export class InMemoryEventStore implements IEventStore {
  private events: Map<string, Event[]> = new Map();

  async saveEvents(aggregateId: string, events: Event[]): Promise<void> {
    const existing = this.events.get(aggregateId) || [];
    this.events.set(aggregateId, [...existing, ...events]);

    // Publish events to event bus
    events.forEach((event) => {
      console.log(`📢 Event published: ${event.eventType}`, {
        aggregateId: event.aggregateId,
        version: event.version,
      });
    });
  }

  async getEvents(aggregateId: string): Promise<Event[]> {
    return this.events.get(aggregateId) || [];
  }

  getAllEvents(): Event[] {
    const allEvents: Event[] = [];
    this.events.forEach((events) => allEvents.push(...events));
    return allEvents.sort((a, b) => a.timestamp.getTime() - b.timestamp.getTime());
  }
}

// ============================================
// COMMAND HANDLERS - Write side
// ============================================

export class CreateAccountHandler {
  constructor(private readonly eventStore: IEventStore) {}

  async handle(command: CreateAccountCommand): Promise<void> {
    // Check if account already exists
    const existingEvents = await this.eventStore.getEvents(command.accountId);
    if (existingEvents.length > 0) {
      throw new Error("Account already exists");
    }

    // Create aggregate
    const account = BankAccount.create(
      command.accountId,
      command.ownerId,
      command.initialBalance
    );

    // Save events
    await this.eventStore.saveEvents(
      command.accountId,
      account.getUncommittedEvents()
    );
  }
}

export class DepositMoneyHandler {
  constructor(private readonly eventStore: IEventStore) {}

  async handle(command: DepositMoneyCommand): Promise<void> {
    // Load aggregate from events
    const events = await this.eventStore.getEvents(command.accountId);
    if (events.length === 0) {
      throw new Error("Account not found");
    }

    const account = BankAccount.fromEvents(events);

    // Execute command
    account.deposit(command.amount);

    // Save new events
    await this.eventStore.saveEvents(
      command.accountId,
      account.getUncommittedEvents()
    );
  }
}

export class WithdrawMoneyHandler {
  constructor(private readonly eventStore: IEventStore) {}

  async handle(command: WithdrawMoneyCommand): Promise<void> {
    const events = await this.eventStore.getEvents(command.accountId);
    if (events.length === 0) {
      throw new Error("Account not found");
    }

    const account = BankAccount.fromEvents(events);
    account.withdraw(command.amount);

    await this.eventStore.saveEvents(
      command.accountId,
      account.getUncommittedEvents()
    );
  }
}

// ============================================
// READ MODEL - Projections
// ============================================

export interface AccountSummary {
  accountId: string;
  ownerId: string;
  currentBalance: number;
  isActive: boolean;
  createdAt: Date;
  lastActivity: Date;
  transactionCount: number;
}

export interface TransactionHistory {
  accountId: string;
  transactions: Transaction[];
}

export interface Transaction {
  eventId: string;
  type: "DEPOSIT" | "WITHDRAWAL" | "CREATED" | "CLOSED";
  amount: number;
  balance: number;
  timestamp: Date;
}

// ============================================
// PROJECTIONS - Event handlers for read model
// ============================================

export class AccountSummaryProjection {
  private summaries: Map<string, AccountSummary> = new Map();

  async handle(event: Event): Promise<void> {
    switch (event.eventType) {
      case "AccountCreated":
        this.handleAccountCreated(event as AccountCreatedEvent);
        break;
      case "MoneyDeposited":
        this.handleMoneyDeposited(event as MoneyDepositedEvent);
        break;
      case "MoneyWithdrawn":
        this.handleMoneyWithdrawn(event as MoneyWithdrawnEvent);
        break;
      case "AccountClosed":
        this.handleAccountClosed(event as AccountClosedEvent);
        break;
    }
  }

  private handleAccountCreated(event: AccountCreatedEvent): void {
    this.summaries.set(event.aggregateId, {
      accountId: event.aggregateId,
      ownerId: event.ownerId,
      currentBalance: event.initialBalance,
      isActive: true,
      createdAt: event.timestamp,
      lastActivity: event.timestamp,
      transactionCount: 1,
    });
  }

  private handleMoneyDeposited(event: MoneyDepositedEvent): void {
    const summary = this.summaries.get(event.aggregateId);
    if (summary) {
      summary.currentBalance = event.balance;
      summary.lastActivity = event.timestamp;
      summary.transactionCount++;
    }
  }

  private handleMoneyWithdrawn(event: MoneyWithdrawnEvent): void {
    const summary = this.summaries.get(event.aggregateId);
    if (summary) {
      summary.currentBalance = event.balance;
      summary.lastActivity = event.timestamp;
      summary.transactionCount++;
    }
  }

  private handleAccountClosed(event: AccountClosedEvent): void {
    const summary = this.summaries.get(event.aggregateId);
    if (summary) {
      summary.isActive = false;
      summary.lastActivity = event.timestamp;
    }
  }

  // Queries
  getAccountSummary(accountId: string): AccountSummary | null {
    return this.summaries.get(accountId) || null;
  }

  getAllAccounts(): AccountSummary[] {
    return Array.from(this.summaries.values());
  }

  getActiveAccounts(): AccountSummary[] {
    return Array.from(this.summaries.values()).filter((s) => s.isActive);
  }
}

export class TransactionHistoryProjection {
  private histories: Map<string, TransactionHistory> = new Map();

  async handle(event: Event): Promise<void> {
    const history = this.histories.get(event.aggregateId) || {
      accountId: event.aggregateId,
      transactions: [],
    };

    switch (event.eventType) {
      case "AccountCreated":
        const created = event as AccountCreatedEvent;
        history.transactions.push({
          eventId: event.eventId,
          type: "CREATED",
          amount: created.initialBalance,
          balance: created.initialBalance,
          timestamp: event.timestamp,
        });
        break;

      case "MoneyDeposited":
        const deposited = event as MoneyDepositedEvent;
        history.transactions.push({
          eventId: event.eventId,
          type: "DEPOSIT",
          amount: deposited.amount,
          balance: deposited.balance,
          timestamp: event.timestamp,
        });
        break;

      case "MoneyWithdrawn":
        const withdrawn = event as MoneyWithdrawnEvent;
        history.transactions.push({
          eventId: event.eventId,
          type: "WITHDRAWAL",
          amount: withdrawn.amount,
          balance: withdrawn.balance,
          timestamp: event.timestamp,
        });
        break;
    }

    this.histories.set(event.aggregateId, history);
  }

  getTransactionHistory(accountId: string): TransactionHistory | null {
    return this.histories.get(accountId) || null;
  }
}

// ============================================
// QUERIES - Read side
// ============================================

export class AccountQueries {
  constructor(private readonly projection: AccountSummaryProjection) {}

  getAccountSummary(accountId: string): AccountSummary | null {
    return this.projection.getAccountSummary(accountId);
  }

  getAllAccounts(): AccountSummary[] {
    return this.projection.getAllAccounts();
  }

  getActiveAccounts(): AccountSummary[] {
    return this.projection.getActiveAccounts();
  }

  getTotalBalance(): number {
    return this.projection
      .getAllAccounts()
      .reduce((total, account) => total + account.currentBalance, 0);
  }
}

export class TransactionQueries {
  constructor(private readonly projection: TransactionHistoryProjection) {}

  getTransactionHistory(accountId: string): TransactionHistory | null {
    return this.projection.getTransactionHistory(accountId);
  }

  getRecentTransactions(accountId: string, limit: number): Transaction[] {
    const history = this.projection.getTransactionHistory(accountId);
    if (!history) return [];

    return history.transactions
      .sort((a, b) => b.timestamp.getTime() - a.timestamp.getTime())
      .slice(0, limit);
  }
}

// ============================================
// UTILITIES
// ============================================

function generateId(): string {
  return `${Date.now()}-${Math.random().toString(36).substr(2, 9)}`;
}

// ============================================
// EXAMPLE USAGE
// ============================================

async function main() {
  console.log("🏦 CQRS Pattern - Bank Account Example\n");

  // Setup
  const eventStore = new InMemoryEventStore();

  // Command handlers (write side)
  const createAccountHandler = new CreateAccountHandler(eventStore);
  const depositHandler = new DepositMoneyHandler(eventStore);
  const withdrawHandler = new WithdrawMoneyHandler(eventStore);

  // Projections (read side)
  const summaryProjection = new AccountSummaryProjection();
  const historyProjection = new TransactionHistoryProjection();

  // Queries
  const accountQueries = new AccountQueries(summaryProjection);
  const transactionQueries = new TransactionQueries(historyProjection);

  try {
    // 1. Create account (COMMAND)
    console.log("📝 Creating account...");
    await createAccountHandler.handle({
      commandId: generateId(),
      timestamp: new Date(),
      accountId: "ACC-001",
      ownerId: "USER-001",
      initialBalance: 1000,
    });

    // Update projections
    let events = await eventStore.getEvents("ACC-001");
    for (const event of events) {
      await summaryProjection.handle(event);
      await historyProjection.handle(event);
    }

    console.log("✅ Account created\n");

    // 2. Query account (QUERY)
    console.log("🔍 Querying account summary...");
    let summary = accountQueries.getAccountSummary("ACC-001");
    console.log("Account Summary:", summary);
    console.log();

    // 3. Deposit money (COMMAND)
    console.log("💰 Depositing 500...");
    await depositHandler.handle({
      commandId: generateId(),
      timestamp: new Date(),
      accountId: "ACC-001",
      amount: 500,
    });

    // Update projections
    events = await eventStore.getEvents("ACC-001");
    for (const event of events) {
      await summaryProjection.handle(event);
      await historyProjection.handle(event);
    }

    console.log("✅ Money deposited\n");

    // 4. Withdraw money (COMMAND)
    console.log("💳 Withdrawing 200...");
    await withdrawHandler.handle({
      commandId: generateId(),
      timestamp: new Date(),
      accountId: "ACC-001",
      amount: 200,
    });

    // Update projections
    events = await eventStore.getEvents("ACC-001");
    for (const event of events) {
      await summaryProjection.handle(event);
      await historyProjection.handle(event);
    }

    console.log("✅ Money withdrawn\n");

    // 5. Query final state (QUERY)
    console.log("🔍 Final account summary:");
    summary = accountQueries.getAccountSummary("ACC-001");
    console.log(summary);
    console.log();

    console.log("📋 Transaction history:");
    const history = transactionQueries.getTransactionHistory("ACC-001");
    console.log(history);
    console.log();

    // 6. Event sourcing: reconstruct from events
    console.log("♻️  Reconstructing account from events...");
    const allEvents = await eventStore.getEvents("ACC-001");
    const reconstructed = BankAccount.fromEvents(allEvents);
    console.log(`Balance from events: ${reconstructed.getBalance()}`);
    console.log(`Version: ${reconstructed.getVersion()}`);
    console.log();

    // 7. Show all events
    console.log("📜 All events in store:");
    const allStoredEvents = eventStore.getAllEvents();
    allStoredEvents.forEach((e) => {
      console.log(`  ${e.eventType} v${e.version} @ ${e.timestamp.toISOString()}`);
    });
  } catch (error) {
    console.error("❌ Error:", error);
  }

  console.log("\n🎉 CQRS Demo complete!");
}

// Run if this is the main module
if (require.main === module) {
  main();
}

// ============================================
// AVANTAGES CQRS + EVENT SOURCING
// ============================================

/*
✅ PERFORMANCE
- Read model optimisé pour queries
- Write model optimisé pour commands
- Scalabilité indépendante

✅ AUDIT TRAIL
- Historique complet des changements
- Reconstruct state à n'importe quel moment
- Debugging facilité

✅ FLEXIBILITY
- Multiples projections du même événement
- Ajouter nouvelles vues sans changer write model
- Time travel / replay possible

✅ EVENTUAL CONSISTENCY
- Write immediate, read eventually consistent
- Scalabilité massive
- Résilience aux pannes

✅ BUSINESS INSIGHTS
- Events = log des actions métier
- Analytics sur événements
- Machine learning sur patterns

❌ COMPLEXITÉ
- Plus complexe que CRUD simple
- Eventual consistency à gérer
- Projections à maintenir
- Event versioning nécessaire

⚠️  QUAND UTILISER CQRS
- Domain complexe avec beaucoup de lectures
- Besoin d'audit complet
- Scalabilité critique
- Multiple representations des données

❌ QUAND NE PAS UTILISER
- CRUD simple
- Petite application
- Consistency forte requise partout
- Équipe non familière avec pattern
*/
