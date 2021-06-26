package com.craftinginterpreters.lox;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;

class Resolver implements Expr.Visitor<Void>, Stmt.Visitor<Void> {
  private enum FunctionType {
    NONE,
    FUNCTION
  }

  static private class VarState {
    private enum VarType {
      LOCAL,
      PARAM
    }
    private Token token;
    private VarType type;
    private Boolean initialized = false;
    private Boolean read = false;

    public static VarState local(Token token) {
      return new VarState(token, VarType.LOCAL, false);
    }

    public static VarState param(Token token) {
      return new VarState(token, VarType.PARAM, true);
    }

    private VarState(Token token, VarType type, Boolean initialized) {
      this.token = token;
      this.type = type;
      this.initialized = initialized;
    }

    Boolean isUnused() {
      return !read && type == VarType.LOCAL;
    }

    Boolean isUninitialized() {
      return !initialized;
    }

    Token getToken() {
      return token;
    }

    void markInitialized() {
      this.initialized = true;
    }

    void markRead() {
      this.read = true;
    }
  }

  private final Interpreter interpreter;
  private final Stack<Map<String, VarState>> scopes = new Stack<>();
  private FunctionType currentFunction = FunctionType.NONE;

  Resolver(Interpreter interpreter) {
    this.interpreter = interpreter;
  }

  @Override
  public Void visitBlockStmt(Stmt.Block stmt) {
    beginScope();
    resolve(stmt.statements);
    endScope();
    return null;
  }

  @Override
  public Void visitExpressionStmt(Stmt.Expression stmt) {
    resolve(stmt.expression);
    return null;
  }

  @Override
  public Void visitIfStmt(Stmt.If stmt) {
    resolve(stmt.condition);
    resolve(stmt.thenBranch);
    if (stmt.elseBranch != null) resolve(stmt.elseBranch);
    return null;
  }

  @Override
  public Void visitPrintStmt(Stmt.Print stmt) {
    resolve(stmt.expression);
    return null;
  }

  @Override
  public Void visitReturnStmt(Stmt.Return stmt) {
    if (currentFunction == FunctionType.NONE) {
      Lox.error(stmt.keyword, "Can't return from top-level code.");
    }

    if (stmt.value != null) {
      resolve(stmt.value);
    }

    return null;
  }

  @Override
  public Void visitWhileStmt(Stmt.While stmt) {
    resolve(stmt.condition);
    resolve(stmt.body);
    return null;
  }

  @Override
  public Void visitBinaryExpr(Expr.Binary expr) {
    resolve(expr.left);
    resolve(expr.right);
    return null;
  }

  @Override
  public Void visitCallExpr(Expr.Call expr) {
    resolve(expr.callee);

    for (Expr argument : expr.arguments) {
      resolve(argument);
    }

    return null;
  }

  @Override
  public Void visitGroupingExpr(Expr.Grouping expr) {
    resolve(expr.expression);
    return null;
  }

  @Override
  public Void visitLiteralExpr(Expr.Literal expr) {
    return null;
  }

  @Override
  public Void visitLogicalExpr(Expr.Logical expr) {
    resolve(expr.left);
    resolve(expr.right);
    return null;
  }

  @Override
  public Void visitUnaryExpr(Expr.Unary expr) {
    resolve(expr.right);
    return null;
  }

  @Override
  public Void visitFunctionStmt(Stmt.Function stmt) {
    declare(stmt.name, VarState.param(stmt.name));

    resolveFunction(stmt, FunctionType.FUNCTION);
    return null;
  }

  @Override
  public Void visitVarStmt(Stmt.Var stmt) {
    VarState state = VarState.local(stmt.name);
    declare(stmt.name, state);
    if (stmt.initializer != null) {
      resolve(stmt.initializer);
    }
    state.markInitialized();
    return null;
  }

  @Override
  public Void visitAssignExpr(Expr.Assign expr) {
    resolve(expr.value);
    resolveLocal(expr, expr.name, false);
    return null;
  }

  @Override
  public Void visitVariableExpr(Expr.Variable expr) {
    if (!scopes.isEmpty() && isVarUninitialized(expr)) {
      Lox.error(expr.name, "Can't read local variable in its own initializer.");
    }

    resolveLocal(expr, expr.name, true);
    return null;
  }

  private Boolean isVarUninitialized(Expr.Variable expr) {
    if (scopes.isEmpty()) return false;

    VarState varstate = scopes.peek().get(expr.name.lexeme);
    return varstate != null && varstate.isUninitialized();
  }

  void resolve(List<Stmt> statements) {
    for (Stmt statement : statements) {
      resolve(statement);
    }
  }

  private void resolve(Stmt stmt) {
    stmt.accept(this);
  }

  private void resolve(Expr expr) {
    expr.accept(this);
  }

  private void resolveFunction(Stmt.Function function, FunctionType type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();
    for (Token param : function.params) {
      declare(param, VarState.param(param));
    }
    resolve(function.body);
    endScope();
    currentFunction = enclosingFunction;
  }

  private void beginScope() {
    scopes.push(new HashMap<String, VarState>());
  }

  private void endScope() {
    var scope = scopes.pop();

    for (VarState varstate : scope.values()) {
      if (varstate.isUnused()) {
        Lox.error(varstate.getToken(), "Unused variable.");
      }
    }
  }

  private void declare(Token name, VarState state) {
    if (scopes.isEmpty()) return;
    Map<String, VarState> scope = scopes.peek();
    if (scope.containsKey(name.lexeme)) {
      Lox.error(name, "Already variable with this name in this scope.");
    }

    scope.put(name.lexeme, state);
  }

  private void resolveLocal(Expr expr, Token name, Boolean read) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
      if (scopes.get(i).containsKey(name.lexeme)) {
        interpreter.resolve(expr, scopes.size() - 1 - i);
        if (read) scopes.get(i).get(name.lexeme).markRead();
        return;
      }
    }
  }
}
