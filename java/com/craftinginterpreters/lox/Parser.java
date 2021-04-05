package com.craftinginterpreters.lox;

import java.util.LinkedList;
import java.util.List;

import static com.craftinginterpreters.lox.TokenType.*;

class Parser {
  private static class ParseError extends RuntimeException {}

  private final List<Token> tokens;
  private int current = 0;

  Parser(List<Token> tokens) {
    this.tokens = tokens;
  }

  Expr parse() {
    try {
      return expression();
    } catch (ParseError error) {
      return null;
    }
  }

  private Expr expression() {
    Expr expr = equality();
    if (check(COMMA)) {
      return cexpressions(expr);
    }

    return expr;
  }

  private Expr cexpressions(Expr first) {
    List<Expr> exprs = new LinkedList<>();
    exprs.add(first);

    while (match(COMMA)) {
     exprs.add(equality());
    }

    return new Expr.Multi(exprs);
  }

  private Expr equality() {
    return binary(
      () ->  { return comparison(); },
      BANG_EQUAL, EQUAL_EQUAL
    );
  }

  @FunctionalInterface
  private interface ExprLambda {
    public abstract Expr run();
  }

  private Expr binary(ExprLambda fn, TokenType... types) {
    Expr expr = fn.run();

    while (match(types)) {
      Token operator = previous();
      Expr right = fn.run();
      expr = new Expr.Binary(expr, operator, right);
    }

    return expr;
  }

  private Expr comparison() {
    return binary(
      () ->  { return term(); },
      GREATER, GREATER_EQUAL, LESS, LESS_EQUAL
    );
  }

  private Expr term() {
    return binary(
      () ->  { return factor(); },
      MINUS, PLUS
    );
  }

  private Expr factor() {
    return binary(
      () ->  { return unary(); },
      SLASH, STAR
    );
  }

  private Expr unary() {
    if (match(BANG, MINUS)) {
      Token operator = previous();
      Expr right = unary();
      return new Expr.Unary(operator, right);
    }

    return primary();
  }

  private Expr primary() {
    if (match(FALSE)) return new Expr.Literal(false);
    if (match(TRUE)) return new Expr.Literal(true);
    if (match(NIL)) return new Expr.Literal(null);

    if (match(NUMBER, STRING)) {
      return new Expr.Literal(previous().literal);
    }

    if (match(LEFT_PAREN)) {
      Expr expr = equality();
      consume(RIGHT_PAREN, "Expect ')' after expression.");
      return new Expr.Grouping(expr);
    }

    throw error(peek(), "Expect expression.");
  }

  private boolean match(TokenType... types) {
    for (TokenType type : types) {
      if (check(type)) {
        advance();
        return true;
      }
    }

    return false;
  }

  private Token consume(TokenType type, String message) {
    if (check(type)) return advance();

    throw error(peek(), message);
  }

  private boolean check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
  }

  private Token advance() {
    if (!isAtEnd()) current++;
    return previous();
  }

  private boolean isAtEnd() {
    return peek().type == EOF;
  }

  private Token peek() {
    return tokens.get(current);
  }

  private Token previous() {
    return tokens.get(current - 1);
  }

  private ParseError error(Token token, String message) {
    Lox.error(token, message);
    return new ParseError();
  }
  
  private void synchronize() {
    advance();

    while (!isAtEnd()) {
      if (previous().type == SEMICOLON) return;

      switch (peek().type) {
        case CLASS:
        case FUN:
        case VAR:
        case FOR:
        case IF:
        case WHILE:
        case PRINT:
        case RETURN:
          return;
        default:
          advance();
      }
    }
  }
}