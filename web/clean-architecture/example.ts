// ============================================
// DOMAIN LAYER - Le coeur métier (aucune dépendance externe)
// ============================================

/**
 * Entity: User
 * Représente un utilisateur avec ses règles métier
 */
export class User {
  private constructor(
    public readonly id: string,
    public readonly email: string,
    public readonly name: string,
    private passwordHash: string,
    public readonly createdAt: Date,
    public readonly isActive: boolean
  ) {}

  static create(email: string, name: string, passwordHash: string): User {
    // Business rule: email must be valid
    if (!this.isValidEmail(email)) {
      throw new Error("Invalid email format");
    }

    // Business rule: name must not be empty
    if (!name || name.trim().length === 0) {
      throw new Error("Name cannot be empty");
    }

    return new User(
      this.generateId(),
      email.toLowerCase(),
      name.trim(),
      passwordHash,
      new Date(),
      true
    );
  }

  changePassword(newPasswordHash: string): User {
    if (!this.isActive) {
      throw new Error("Cannot change password for inactive user");
    }
    return new User(
      this.id,
      this.email,
      this.name,
      newPasswordHash,
      this.createdAt,
      this.isActive
    );
  }

  deactivate(): User {
    return new User(
      this.id,
      this.email,
      this.name,
      this.passwordHash,
      this.createdAt,
      false
    );
  }

  verifyPassword(hash: string): boolean {
    return this.passwordHash === hash;
  }

  private static isValidEmail(email: string): boolean {
    return /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email);
  }

  private static generateId(): string {
    return `user_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;
  }
}

/**
 * Value Object: Email
 * Immutable, défini par ses valeurs
 */
export class Email {
  private constructor(private readonly value: string) {}

  static create(email: string): Email {
    const normalized = email.toLowerCase().trim();
    if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(normalized)) {
      throw new Error("Invalid email format");
    }
    return new Email(normalized);
  }

  getValue(): string {
    return this.value;
  }

  equals(other: Email): boolean {
    return this.value === other.value;
  }
}

// ============================================
// INTERFACES LAYER - Contrats (Ports)
// ============================================

/**
 * Repository Interface (Port)
 * Le domaine définit ce dont il a besoin, pas comment c'est implémenté
 */
export interface IUserRepository {
  save(user: User): Promise<void>;
  findById(id: string): Promise<User | null>;
  findByEmail(email: string): Promise<User | null>;
  delete(id: string): Promise<void>;
}

/**
 * Password Service Interface
 */
export interface IPasswordService {
  hash(password: string): Promise<string>;
  verify(password: string, hash: string): Promise<boolean>;
}

/**
 * Email Service Interface
 */
export interface IEmailService {
  sendWelcomeEmail(email: string, name: string): Promise<void>;
}

// ============================================
// USE CASES LAYER - Application Business Rules
// ============================================

/**
 * Use Case: Register User
 * Orchestration de la logique applicative
 */
export class RegisterUserUseCase {
  constructor(
    private readonly userRepository: IUserRepository,
    private readonly passwordService: IPasswordService,
    private readonly emailService: IEmailService
  ) {}

  async execute(input: RegisterUserInput): Promise<RegisterUserOutput> {
    // 1. Vérifier que l'email n'existe pas déjà
    const existingUser = await this.userRepository.findByEmail(input.email);
    if (existingUser) {
      throw new Error("Email already registered");
    }

    // 2. Hasher le mot de passe
    const passwordHash = await this.passwordService.hash(input.password);

    // 3. Créer l'entité User (applique les règles métier)
    const user = User.create(input.email, input.name, passwordHash);

    // 4. Persister
    await this.userRepository.save(user);

    // 5. Envoyer email de bienvenue
    await this.emailService.sendWelcomeEmail(user.email, user.name);

    return {
      userId: user.id,
      email: user.email,
      name: user.name,
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

/**
 * Use Case: Authenticate User
 */
export class AuthenticateUserUseCase {
  constructor(
    private readonly userRepository: IUserRepository,
    private readonly passwordService: IPasswordService
  ) {}

  async execute(input: AuthenticateUserInput): Promise<AuthenticateUserOutput> {
    const user = await this.userRepository.findByEmail(input.email);
    if (!user) {
      throw new Error("Invalid credentials");
    }

    if (!user.isActive) {
      throw new Error("User account is deactivated");
    }

    const isValid = await this.passwordService.verify(
      input.password,
      user["passwordHash"]
    );

    if (!isValid) {
      throw new Error("Invalid credentials");
    }

    return {
      userId: user.id,
      email: user.email,
      name: user.name,
    };
  }
}

export interface AuthenticateUserInput {
  email: string;
  password: string;
}

export interface AuthenticateUserOutput {
  userId: string;
  email: string;
  name: string;
}

// ============================================
// ADAPTERS LAYER - Implémentations concrètes
// ============================================

/**
 * Adapter: InMemory User Repository
 * Implémentation concrète pour tests ou démo
 */
export class InMemoryUserRepository implements IUserRepository {
  private users: Map<string, User> = new Map();

  async save(user: User): Promise<void> {
    this.users.set(user.id, user);
  }

  async findById(id: string): Promise<User | null> {
    return this.users.get(id) || null;
  }

  async findByEmail(email: string): Promise<User | null> {
    for (const user of this.users.values()) {
      if (user.email === email.toLowerCase()) {
        return user;
      }
    }
    return null;
  }

  async delete(id: string): Promise<void> {
    this.users.delete(id);
  }
}

/**
 * Adapter: Bcrypt Password Service
 * Note: En vrai, utiliser bcrypt library
 */
export class BcryptPasswordService implements IPasswordService {
  async hash(password: string): Promise<string> {
    // Simulation - utiliser bcrypt.hash() en production
    return `hashed_${password}`;
  }

  async verify(password: string, hash: string): Promise<boolean> {
    // Simulation - utiliser bcrypt.compare() en production
    return hash === `hashed_${password}`;
  }
}

/**
 * Adapter: Console Email Service
 * Pour démo/tests
 */
export class ConsoleEmailService implements IEmailService {
  async sendWelcomeEmail(email: string, name: string): Promise<void> {
    console.log(`📧 Sending welcome email to ${email} (${name})`);
  }
}

/**
 * Adapter: SMTP Email Service
 * Production-ready
 */
export class SmtpEmailService implements IEmailService {
  constructor(
    private readonly host: string,
    private readonly port: number,
    private readonly user: string,
    private readonly password: string
  ) {}

  async sendWelcomeEmail(email: string, name: string): Promise<void> {
    // En production: utiliser nodemailer ou service externe
    console.log(`📧 SMTP: Sending welcome email via ${this.host}:${this.port}`);
    console.log(`   To: ${email}, Name: ${name}`);
  }
}

// ============================================
// COMPOSITION ROOT - Dependency Injection
// ============================================

/**
 * Factory pour créer l'application avec ses dépendances
 */
export class ApplicationFactory {
  static createRegisterUserUseCase(): RegisterUserUseCase {
    const userRepository = new InMemoryUserRepository();
    const passwordService = new BcryptPasswordService();
    const emailService = new ConsoleEmailService();

    return new RegisterUserUseCase(
      userRepository,
      passwordService,
      emailService
    );
  }

  static createAuthenticateUserUseCase(
    userRepository: IUserRepository
  ): AuthenticateUserUseCase {
    const passwordService = new BcryptPasswordService();
    return new AuthenticateUserUseCase(userRepository, passwordService);
  }
}

// ============================================
// EXEMPLE D'UTILISATION
// ============================================

async function main() {
  console.log("🏗️  Clean Architecture Demo\n");

  // Setup - créer les use cases
  const repository = new InMemoryUserRepository();
  const registerUseCase = ApplicationFactory.createRegisterUserUseCase();
  const authenticateUseCase =
    ApplicationFactory.createAuthenticateUserUseCase(repository);

  try {
    // Scénario 1: Enregistrer un utilisateur
    console.log("📝 Registering user...");
    const result = await registerUseCase.execute({
      email: "john.doe@example.com",
      name: "John Doe",
      password: "SecurePass123!",
    });
    console.log("✅ User registered:", result);
    console.log();

    // Scénario 2: Authentifier
    console.log("🔐 Authenticating user...");
    const authResult = await authenticateUseCase.execute({
      email: "john.doe@example.com",
      password: "SecurePass123!",
    });
    console.log("✅ User authenticated:", authResult);
    console.log();

    // Scénario 3: Email invalide (règle métier)
    console.log("❌ Trying invalid email...");
    try {
      await registerUseCase.execute({
        email: "invalid-email",
        name: "Test User",
        password: "pass",
      });
    } catch (error) {
      console.log("✅ Caught expected error:", (error as Error).message);
    }
    console.log();

    // Scénario 4: Duplicate email
    console.log("❌ Trying duplicate email...");
    try {
      await registerUseCase.execute({
        email: "john.doe@example.com",
        name: "Another John",
        password: "pass",
      });
    } catch (error) {
      console.log("✅ Caught expected error:", (error as Error).message);
    }
  } catch (error) {
    console.error("❌ Error:", error);
  }

  console.log("\n🎉 Demo complete!");
}

// Run if this is the main module
if (require.main === module) {
  main();
}

// ============================================
// AVANTAGES DE CETTE ARCHITECTURE
// ============================================

/*
✅ TESTABILITÉ
- Chaque couche testable indépendamment
- Mocking facile grâce aux interfaces
- Tests unitaires sans base de données

✅ MAINTENABILITÉ
- Séparation claire des responsabilités
- Changements localisés
- Code auto-documenté

✅ FLEXIBILITÉ
- Changer de DB sans toucher au domaine
- Remplacer email service facilement
- Ajouter nouveaux adapters sans impact

✅ INDÉPENDANCE
- Domaine indépendant de tout framework
- Peut migrer de Express à Fastify sans problème
- Tests métier stables même si infra change

❌ ANTI-PATTERNS À ÉVITER
- Dépendances du domaine vers l'infra
- Logique métier dans les controllers
- Entities couplées à l'ORM
- Use cases qui connaissent HTTP/DB
*/
