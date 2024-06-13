#include <compiler/parser.h>

std::vector<sharedptr<Statement>> Parser::
parse()
{

#if 0
    try
    {
        sharedptr<Expression> expr = this->expression();
        return expr;
    }
    catch(SyntaxErrorException error)
    {
        std::cout << error.to_string() << std::endl;
        return { NULL };
    }

#endif

    std::vector<sharedptr<Statement>> statements;
    while (!this->is_eof())
    {
        try
        {
            sharedptr<Statement> current_statement = statement();
            statements.push_back(current_statement);
        }
        catch (SyntaxErrorException error)
        {
            std::cout << error.to_string() << std::endl;
            this->synchronize();
        }
    }

    return statements;

}

Token Parser::
advance()
{
    if (this->current < tokens.size())
        this->current++;
    return this->tokens[this->current];
}

Token Parser::
previous()
{
    return this->tokens[this->current - 1];
}

Token Parser::
peek()
{
    return this->tokens[this->current];
}

bool Parser::
is_eof()
{
    return this->peek().type == TokenType::ENDOFFILE;
}

bool Parser::
check(TokenType type)
{

    if (this->is_eof())
        return false;
    else
        return this->peek().type == type;

}

bool Parser::
match(std::vector<TokenType> types)
{

    for (TokenType type : types)
    {
        if (check(type))
        {
            this->advance();
            return true;
        }
    }

    return false;

}

sharedptr<Expression> Parser::
equality()
{

    sharedptr<Expression> expr = this->comparison();

    while (this->match({TokenType::EQUALS, TokenType::NOTEQUALS}))
    {
        Token operation = this->previous();
        sharedptr<Expression> right = this->comparison();
        expr = std::make_shared<BinaryExpression>(expr, operation, right);
    }

    return expr;

}

sharedptr<Expression> Parser::
comparison()
{

    sharedptr<Expression> expr = this->term();

    while (this->match({TokenType::LESSTHAN, TokenType::GREATERTHAN}))
    {
        Token operation = this->previous();
        sharedptr<Expression> right = this->term();
        expr = std::make_shared<BinaryExpression>(expr, operation, right);
    }

    return expr;

}

sharedptr<Expression> Parser::
term()
{

    sharedptr<Expression> expr = this->factor();

    while (this->match({TokenType::PLUS, TokenType::MINUS}))
    {
        Token operation = this->previous();
        sharedptr<Expression> right = this->factor();
        expr = std::make_shared<BinaryExpression>(expr, operation, right);
    }

    return expr;

}

sharedptr<Expression> Parser::
factor()
{

    sharedptr<Expression> expr = this->unary();

    while (this->match({TokenType::MULTIPLY, TokenType::DIVISION}))
    {
        Token operation = this->previous();
        sharedptr<Expression> right = this->unary();
        expr = std::make_shared<BinaryExpression>(expr, operation, right);
    }

    return expr;

}

sharedptr<Expression> Parser::
unary()
{

    if (match({TokenType::MINUS}))
    {
        Token operation = previous();
        sharedptr<Expression> right = this->unary();
        return std::make_shared<UnaryExpression>(operation, right);
    }

    return this->primary();

}

sharedptr<Expression> Parser::
primary()
{

    if (match({TokenType::STRING, TokenType::NUMBER}))
    {
        Token literal_token = previous();

        LiteralType literal_type;
        if (literal_token.type == TokenType::STRING)
            literal_type = LiteralType::STRING;
        else
            literal_type = LiteralType::NUMBER;

        return std::make_shared<LiteralExpression>(literal_token.lexeme, literal_type);
    }

    else if (match({TokenType::LEFT_PARENTHESIS}))
    {
        sharedptr<Expression> expr = expression();
        this->consume(TokenType::RIGHT_PARENTHESIS, "Expected ')' after expression");
        return std::make_shared<GroupingExpression>(expr);
    }

    // We need to do more with this to ensure we have a proper error
    // output here.
    throw SyntaxErrorException(this->peek(), "Unexpected/undefined expression");

    return NULL;

}

sharedptr<Expression> Parser::
expression()
{
    return this->equality();
}

Token Parser::
consume(TokenType type, std::string message)
{
    if (this->check(type)) return advance();

    throw SyntaxErrorException(this->peek(), message);
}

void Parser::
synchronize()
{

    while (!this->is_eof())
    {
        if (previous().type == TokenType::SEMICOLON)
            return;

        switch (peek().type)
        {
            case TokenType::BEGIN:
            case TokenType::PROCEDURE:
            case TokenType::FUNCTION:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::LOOP:
            case TokenType::PLOOP:
            case TokenType::FIT:
            case TokenType::PRINT:
            return;
        };

        this->advance();
    }


}

sharedptr<Statement> Parser::
statement()
{

    if (match({ TokenType::PRINT }))
        return print_statement();

    return this->expression_statement();

}

sharedptr<Statement> Parser::
print_statement()
{

    sharedptr<Expression> value = this->expression();
    consume(TokenType::SEMICOLON, "Expected ';' befor value");
    return std::make_shared<PrintStatement>(value);

}

sharedptr<Statement> Parser::
expression_statement()
{

    sharedptr<Expression> expr = this->expression();
    consume(TokenType::SEMICOLON, "Expected ';' before expression");
    return std::make_shared<ExpressionStatement>(expr);

}
