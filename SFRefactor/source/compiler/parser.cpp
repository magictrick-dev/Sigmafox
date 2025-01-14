#include <compiler/parser.hpp>
#include <compiler/errorhandler.hpp>

// --- Parser Constructor/Destructors ------------------------------------------
// 
// The usual setup code needed to initialize the parser. No real magic here.
//

SyntaxParser::
SyntaxParser(Filepath filepath, DependencyGraph *graph) : tokenizer(filepath)
{

    this->path = filepath;
    this->graph = graph;
    this->error_count = 0;

}

SyntaxParser::
~SyntaxParser()
{

}

Filepath SyntaxParser::
get_source_path() const
{

    return this->path;

}

// --- Parser Helpers ----------------------------------------------------------
//
// Anything that doesn't directly pertain to constructing the AST is written here.
// Most of the functions are just helpers to make parsing easier.
//

void SyntaxParser::
synchronize_to(TokenType type)
{

    while (this->tokenizer.get_current_token().type != type)
    {

        // If the token is EOF, then we will break.
        if (this->tokenizer.get_current_token().type == TokenType::TOKEN_EOF) break;

        // Otherwise, shift until we reach our desired synchronization point.
        this->tokenizer.shift();

    }

    // Shift the token we are synchronizing to.
    this->tokenizer.shift();

}

void SyntaxParser::
synchronize_up_to(TokenType type)
{

    // Gets you right up to the next token for cases where you want to process it
    // when you break from loops.
    while (this->tokenizer.get_current_token().type != type)
    {

        // If the token is EOF, then we will break.
        if (this->tokenizer.get_current_token().type == TokenType::TOKEN_EOF) break;

        // Otherwise, shift until we reach our desired synchronization point.
        this->tokenizer.shift();

    }

}

void SyntaxParser::
process_error(i32 where, SyntaxException &error, bool was_just_handled)
{

    this->error_count++;
    if (error.handled == false)
    {
        std::cout << "[" << where << "]:";
        std::cout << error.what() << std::endl;
    }

    if (was_just_handled) error.handled = true;

}

void SyntaxParser::
process_warning(i32 where, SyntaxException &warning, bool was_just_handled)
{

    if (warning.handled == false)
    {
        std::cout << "[" << where << "]:";
        std::cout << warning.what() << std::endl;
    }

    if (was_just_handled) warning.handled = true;

}

template <TokenType expect> void SyntaxParser::
validate_grammar_token()
{

    // Expect the token.
    if (this->expect_current_token_as(expect))
    {
        this->tokenizer.shift();
        return;
    }

    // Throw a syntax error.
    Token error_token = this->tokenizer.get_current_token();
    throw SyntaxError(this->path, error_token, "expected %s, encountered '%s'.",
            Token::type_to_string(expect).c_str(), error_token.reference.c_str());

}

bool SyntaxParser::
construct_as_root()
{

    SF_ASSERT(this->base_node == nullptr &&
            "The 'construct_as_root()' function should only ever be called once.");
    auto root_node = this->match_root();
    if (root_node == nullptr) return false;
    if (this->error_count > 0) return false;
    this->base_node = root_node;
    return true;

}

bool SyntaxParser::
construct_as_module()
{

    if (this->base_node == nullptr)
    {

        auto module_node = this->match_module();
        if (module_node == nullptr) return false;
        if (this->error_count > 0) return false;
        this->base_node = module_node;

    }

    return true;

}

bool SyntaxParser::
expect_previous_token_as(TokenType type) const
{

    bool result = this->tokenizer.get_previous_token().type == type;
    return result;

}

bool SyntaxParser::
expect_current_token_as(TokenType type) const
{

    bool result = this->tokenizer.get_current_token().type == type;
    return result;

}

bool SyntaxParser::
expect_next_token_as(TokenType type) const
{

    bool result = this->tokenizer.get_next_token().type == type;
    return result;

}

void SyntaxParser::
visit_base_node(ISyntaxNodeVisitor *visitor)
{
    SF_ENSURE_PTR(visitor);
    SF_ENSURE_PTR(this->base_node);
    this->base_node->accept(visitor);
}

template <class T, typename... Params> std::shared_ptr<T> SyntaxParser::
generate_node(Params... args)
{

    // Thanks C++, very cool.
    std::shared_ptr<T> result = std::make_shared<T>(args...);
    std::shared_ptr<ISyntaxNode> cast_back = dynamic_pointer_cast<ISyntaxNode>(result);
    this->nodes.push_back(cast_back);
    return result;

}

// --- Parser Implementations --------------------------------------------------
//
// Here is the grammar implementations of the parser. There's a lot going on here
// that is probably difficult to trace just by looking at it. Take a reference file
// with minimal functionality and step through the code to follow what's going.
//
// The actual descent code matches the grammar layout in the header file for this.
// That's also a really good place to get your footing.
//

shared_ptr<ISyntaxNode> SyntaxParser::
match_root()
{

    try
    {

        // Match all global statements.
        std::vector<shared_ptr<ISyntaxNode>> global_nodes;
        shared_ptr<ISyntaxNode> current_node = nullptr;
        while (true)
        {

            try {
                current_node = this->match_global_statement();
                if (current_node == nullptr) break;
                global_nodes.push_back(current_node);
            }
            catch (SyntaxException& syntax_error)
            {
                this->process_error(__LINE__, syntax_error, true);
            }

        }

        // Match main.
        shared_ptr<ISyntaxNode> main_node = this->match_main();

        // Match end-of-file.
        this->validate_grammar_token<TokenType::TOKEN_EOF>();

        // Create the root node.
        auto root_node = this->generate_node<SyntaxNodeRoot>();
        root_node->globals = global_nodes;
        root_node->main = main_node;

        // Return the root node.
        return root_node;

    }
    catch (SyntaxException& syntax_error)
    {
        this->process_error(__LINE__, syntax_error, true);
        return nullptr;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_module()
{

    try
    {

        // Match all global statements.
        std::vector<shared_ptr<ISyntaxNode>> global_nodes;
        shared_ptr<ISyntaxNode> current_node = nullptr;
        while (true)
        {

            try
            {
                current_node = this->match_global_statement();
                if (current_node == nullptr) break;
                global_nodes.push_back(current_node);
            }
            catch (SyntaxException& syntax_error)
            {
                this->process_error(__LINE__, syntax_error, true);
            }

        }

        // Match end-of-file.
        this->validate_grammar_token<TokenType::TOKEN_EOF>();

        // Create the root node.
        auto module_node = this->generate_node<SyntaxNodeModule>();
        module_node->globals = global_nodes;
        return module_node;

    }
    catch (SyntaxException& syntax_error)
    {
        this->process_error(__LINE__, syntax_error, true);
        return nullptr;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_global_statement()
{

    Token current_token = this->tokenizer.get_current_token();
    switch (current_token.type)
    {

        case TokenType::TOKEN_KEYWORD_INCLUDE:      return this->match_include_statement();
        case TokenType::TOKEN_KEYWORD_PROCEDURE:    return this->match_procedure_statement();
        case TokenType::TOKEN_KEYWORD_FUNCTION:     return this->match_function_statement();

    }

    return nullptr;

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_include_statement()
{

    Token include_path_token;

    try
    {

        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_INCLUDE>();

        // Get the path string token.
        include_path_token = this->tokenizer.get_current_token();
        this->validate_grammar_token<TokenType::TOKEN_STRING>();
        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

    }
    catch (SyntaxException& syntax_error)
    {
        this->synchronize_to(TokenType::TOKEN_SEMICOLON);
        this->process_error(__LINE__, syntax_error, true);
        throw;
    }

    // Generate the filepath.
    Filepath include_path = this->path.root_directory();
    include_path += "./";
    std::string path = include_path_token.reference;
    include_path += path;
    include_path.canonicalize();

    // Before we generate the node, we need to add it to the dependency graph.
    if (!this->graph->insert_dependency(this->path, include_path))
    {

        throw SyntaxError(this->path, include_path_token,
                "Cyclical dependency encountered for %s.",
                include_path_token.reference.c_str());

    }

    // Attempt to parse the new include.
    shared_ptr<SyntaxParser> include_parser = this->graph->get_parser_for(include_path);
    SF_ENSURE_PTR(include_parser);

    if (!include_parser->construct_as_module())
    {
        throw SyntaxError(this->path, include_path_token,
                "Unable to parse %s.", include_path_token.reference.c_str());
    }

    if (!include_parser->symbol_stack.is_root_table())
    {
        throw SyntaxError(this->path, include_path_token,
                "Module %s did not successfully pop all symbol tables.",
                include_path_token.reference.c_str());
    }

    auto include_node = this->generate_node<SyntaxNodeInclude>();
    include_node->path = include_path.c_str();
    include_node->module = include_parser->get_base_node();
    return include_node;

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_main()
{

    this->validate_grammar_token<TokenType::TOKEN_KEYWORD_BEGIN>();
    this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

    // Push the scope.
    this->symbol_stack.push_table();

    // Match all body statements.
    std::vector<shared_ptr<ISyntaxNode>> body_statements;
    shared_ptr<ISyntaxNode> current_node = nullptr;
    while (this->expect_current_token_as(TokenType::TOKEN_EOF) == false)
    {

        if (this->expect_current_token_as(TokenType::TOKEN_KEYWORD_END)) break;

        try
        {
            current_node = this->match_body_statement();
            if (current_node == nullptr) break;
            body_statements.push_back(current_node);
        }
        catch (SyntaxException& syntax_error)
        {
            this->process_error(__LINE__, syntax_error, true);
        }

    }

    // Pop the scope.
    this->symbol_stack.pop_table();

    if (this->expect_current_token_as(TokenType::TOKEN_EOF))
    {
        throw SyntaxError(this->path, this->tokenizer.get_current_token(),
                "Unexpected end-of-file encountered.");
    }

    this->validate_grammar_token<TokenType::TOKEN_KEYWORD_END>();
    this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

    auto main_node = this->generate_node<SyntaxNodeMain>();
    main_node->children = body_statements;
    return main_node;

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_body_statement()
{

    // Body statements can match to several types, but the fall-through case
    // will just be expression statements.
    Token current_token = this->tokenizer.get_current_token();
    switch (current_token.type)
    {

        case TokenType::TOKEN_KEYWORD_VARIABLE:     return this->match_variable_statement();
        case TokenType::TOKEN_KEYWORD_SCOPE:        return this->match_scope_statement();
        case TokenType::TOKEN_KEYWORD_PROCEDURE:    return this->match_procedure_statement();
        case TokenType::TOKEN_KEYWORD_FUNCTION:     return this->match_function_statement();
        case TokenType::TOKEN_KEYWORD_WHILE:        return this->match_while_statement();
        case TokenType::TOKEN_KEYWORD_LOOP:         return this->match_loop_statement();
        case TokenType::TOKEN_KEYWORD_IF:           return this->match_if_statement();
        case TokenType::TOKEN_KEYWORD_READ:         return this->match_read_statement();
        case TokenType::TOKEN_KEYWORD_WRITE:        return this->match_write_statement();
        default: break;

    }

    // Expression statement.
    shared_ptr<ISyntaxNode> expression_statement = this->match_expression_statement();
    return expression_statement;

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_write_statement()
{

    return nullptr;
}

shared_ptr<ISyntaxNode> SyntaxParser::
match_read_statement()
{

    return nullptr;
}

shared_ptr<ISyntaxNode> SyntaxParser::
match_if_statement()
{

    shared_ptr<ISyntaxNode> condition_node = nullptr;
    shared_ptr<ISyntaxNode> else_node = nullptr;
    std::vector<shared_ptr<ISyntaxNode>> body_statements;
    try
    {

        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_IF>();

        // Match the condition.
        condition_node = this->match_expression();

        // Match the semicolon.
        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Push the scope.
        this->symbol_stack.push_table();

        // Match all body statements.
        shared_ptr<ISyntaxNode> current_node = nullptr;
        while (this->expect_current_token_as(TokenType::TOKEN_EOF) == false)
        {

            if (this->expect_current_token_as(TokenType::TOKEN_KEYWORD_ENDIF) ||
                this->expect_current_token_as(TokenType::TOKEN_KEYWORD_ELSEIF)) 
                    break;

            try
            {
                current_node = this->match_body_statement();
                if (current_node == nullptr) break;
                body_statements.push_back(current_node);
            }
            catch (SyntaxException& syntax_error)
            {
                this->process_error(__LINE__, syntax_error, true);
                throw;
            }

        }

        // Pop the scope.
        this->symbol_stack.pop_table();

        // Process the next else-if statement.
        shared_ptr<ISyntaxNode> elseif_node = this->match_conditional();
        else_node = elseif_node;

        // We should have recursed into the endif token by this point.
        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_ENDIF>();

    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_KEYWORD_ENDIF);
        this->process_error(__LINE__, error, true);
    }

    try
    {

        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Generate the if node.
        auto if_node = this->generate_node<SyntaxNodeIfStatement>();
        if_node->conditional = condition_node;
        if_node->conditional_else = else_node;
        if_node->children = body_statements;

        return if_node;


    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_SEMICOLON);
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_conditional()
{

    shared_ptr<ISyntaxNode> condition_node = nullptr;
    shared_ptr<ISyntaxNode> else_node = nullptr;
    std::vector<shared_ptr<ISyntaxNode>> body_statements;
    try
    {

        if (!this->expect_current_token_as(TokenType::TOKEN_KEYWORD_ELSEIF))
            return nullptr;

        // Match the elseif token.
        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_ELSEIF>();

        // Match the condition.
        condition_node = this->match_expression();

        // Match the semicolon.
        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Push the scope.
        this->symbol_stack.push_table();

        // Match all body statements.
        shared_ptr<ISyntaxNode> current_node = nullptr;
        while (this->expect_current_token_as(TokenType::TOKEN_EOF) == false)
        {

            if (this->expect_current_token_as(TokenType::TOKEN_KEYWORD_ENDIF) ||
                this->expect_current_token_as(TokenType::TOKEN_KEYWORD_ELSEIF)) 
                    break;

            try
            {
                current_node = this->match_body_statement();
                if (current_node == nullptr) break;
                body_statements.push_back(current_node);
            }
            catch (SyntaxException& syntax_error)
            {
                this->process_error(__LINE__, syntax_error, true);
            }

        }

        // Pop the scope.
        this->symbol_stack.pop_table();

        // Process the next else-if statement. Since this will continue to recurse
        // all additional elseif statements, we can just call this function again.
        // We don't process the endif.
        shared_ptr<ISyntaxNode> elseif_node = this->match_conditional();
        else_node = elseif_node;

    }
    catch (SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw; // Forward back up to the if statement.
    }

    try
    {

        // Generate the node.
        auto elseif_node = this->generate_node<SyntaxNodeConditional>();
        elseif_node->condition = condition_node;
        elseif_node->conditional_else = else_node;
        elseif_node->children = body_statements;
        return elseif_node;

    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_SEMICOLON);
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_loop_statement()
{

    std::string identifier;
    shared_ptr<ISyntaxNode> initial_node = nullptr;
    shared_ptr<ISyntaxNode> terminal_node = nullptr;
    shared_ptr<ISyntaxNode> step_node = nullptr;
    std::vector<shared_ptr<ISyntaxNode>> body_statements;

    try
    {

        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_LOOP>();

        // Match the identifier.
        Token identifier_token = this->tokenizer.get_current_token();
        this->validate_grammar_token<TokenType::TOKEN_IDENTIFIER>();
        identifier = identifier_token.reference;

        // Get the initial expression.
        initial_node = this->match_expression();

        // Get the terminal expression.
        terminal_node = this->match_expression();

        // If there is a step expression, get it.
        if (this->expect_current_token_as(TokenType::TOKEN_SEMICOLON) == false)
        {
            step_node = this->match_expression();
        }

        // Process the semicolon.
        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Push the scope.
        this->symbol_stack.push_table();

        // Add the identifier to the symbol table.
        this->symbol_stack.insert_symbol_locally(identifier, 
            Symbol(identifier, Symboltype::SYMBOL_TYPE_VARIABLE, 0));

        // Match all body statements.
        shared_ptr<ISyntaxNode> current_node = nullptr;
        while (this->expect_current_token_as(TokenType::TOKEN_EOF) == false)
        {

            if (this->expect_current_token_as(TokenType::TOKEN_KEYWORD_ENDLOOP)) break;

            try
            {
                current_node = this->match_body_statement();
                if (current_node == nullptr) break;
                body_statements.push_back(current_node);
            }
            catch (SyntaxException& syntax_error)
            {
                this->process_error(__LINE__, syntax_error, true);
            }

        }

        // Pop the scope.
        this->symbol_stack.pop_table();

        // Validate the end of the loop statement.
        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_ENDLOOP>();

    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_KEYWORD_ENDLOOP);
        this->process_error(__LINE__, error, true);
    }

    // Construct the node.
    try
    {

        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Generate the loop node.
        auto loop_node = this->generate_node<SyntaxNodeLoopStatement>();
        loop_node->identifier = identifier;
        loop_node->initial = initial_node;
        loop_node->terminal = terminal_node;
        loop_node->step = step_node;
        loop_node->children = body_statements;
        return loop_node;

    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_SEMICOLON);
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_while_statement()
{

    shared_ptr<ISyntaxNode> condition_node = nullptr;
    std::vector<shared_ptr<ISyntaxNode>> body_statements;

    try
    {

        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_WHILE>();

        // Match the condition.
        condition_node = this->match_expression();
        if (condition_node == nullptr)
        {
            throw SyntaxError(this->path, this->tokenizer.get_current_token(),
                    "Expected expression in while statement.");
        }

        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Push the scope.
        this->symbol_stack.push_table();

        // Match all body statements.
        shared_ptr<ISyntaxNode> current_node = nullptr;
        while (this->expect_current_token_as(TokenType::TOKEN_EOF) == false)
        {

            if (this->expect_current_token_as(TokenType::TOKEN_KEYWORD_ENDWHILE)) break;

            try
            {
                current_node = this->match_body_statement();
                if (current_node == nullptr) break;
                body_statements.push_back(current_node);
            }
            catch (SyntaxException& syntax_error)
            {
                this->process_error(__LINE__, syntax_error, true);
            }

        }

        // Pop the scope.
        this->symbol_stack.pop_table();

        // Validate the end of the while statement.
        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_ENDWHILE>();

    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_KEYWORD_ENDWHILE);
        this->process_error(__LINE__, error, true);

    }

    // Construct the node.
    try
    {

        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Generate the while node.
        auto while_node = this->generate_node<SyntaxNodeWhileStatement>();
        while_node->condition = condition_node;
        while_node->children = body_statements;
        return while_node;

    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_SEMICOLON);
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_procedure_statement()
{

    Token identifier_token = {};
    std::vector<std::string> parameters;
    std::vector<shared_ptr<ISyntaxNode>> body_statements;

    try 
    {

        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_PROCEDURE>();

        // Get the identifier.
        identifier_token = this->tokenizer.get_current_token();
        this->validate_grammar_token<TokenType::TOKEN_IDENTIFIER>();

        // If the symbol is already defined, it is an error. We disallow procedure
        // definitions shadowing other procedure definitions, and all procedure definitions
        // are lofted into the global scope.
        if (this->symbol_stack.identifier_exists_globally(identifier_token.reference))
        {
            throw SyntaxError(this->path, this->tokenizer.get_current_token(),
                    "Procedure declaration '%s' is already defined.",
                    identifier_token.reference.c_str());
        }

        // Collect all the parameters, they must all be identifiers.
        while (!this->expect_current_token_as(TokenType::TOKEN_SEMICOLON))
        {

            Token parameter_token = this->tokenizer.get_current_token();
            this->validate_grammar_token<TokenType::TOKEN_IDENTIFIER>();
            parameters.push_back(parameter_token.reference);

        }

        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Push the scope.
        this->symbol_stack.push_table();

        // Add all the parameters into the current scope.
        for (const std::string& parameter : parameters)
        {
            this->symbol_stack.insert_symbol_locally(parameter, 
                Symbol(parameter, Symboltype::SYMBOL_TYPE_PARAMETER, 0));
        }

        // Match all body statements.
        shared_ptr<ISyntaxNode> current_node = nullptr;
        while (this->expect_current_token_as(TokenType::TOKEN_EOF) == false)
        {

            if (this->expect_current_token_as(TokenType::TOKEN_KEYWORD_ENDPROCEDURE)) break;

            try
            {
                current_node = this->match_body_statement();
                if (current_node == nullptr) break;
                body_statements.push_back(current_node);
            }
            catch (SyntaxException& syntax_error)
            {
                this->process_error(__LINE__, syntax_error, true);
            }

        }

        // Pop the scope.
        this->symbol_stack.pop_table();

        // Validate the end of the procedure.
        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_ENDPROCEDURE>();

        // Insert the procedure into the symbol table, globally.
        this->symbol_stack.insert_symbol_globally(identifier_token.reference, Symbol(
            identifier_token.reference, Symboltype::SYMBOL_TYPE_PROCEDURE, parameters.size()));

    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_KEYWORD_ENDPROCEDURE);
        this->process_error(__LINE__, error, true);
    }

    // Construct the node.
    try
    {

        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Generate the procedure node.
        auto procedure_node = this->generate_node<SyntaxNodeProcedureStatement>();
        procedure_node->identifier_name = identifier_token.reference;
        procedure_node->parameters = parameters;
        procedure_node->body_statements = body_statements;
        return procedure_node;

    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_SEMICOLON);
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_function_statement()
{

    Token identifier_token = {};
    std::vector<std::string> parameters;
    std::vector<shared_ptr<ISyntaxNode>> body_statements;

    try 
    {

        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_FUNCTION>();

        // Get the identifier.
        identifier_token = this->tokenizer.get_current_token();
        this->validate_grammar_token<TokenType::TOKEN_IDENTIFIER>();

        // If the symbol is already defined, it is an error. We disallow procedure
        // definitions shadowing other procedure definitions, and all procedure definitions
        // are lofted into the global scope.
        if (this->symbol_stack.identifier_exists_globally(identifier_token.reference))
        {
            throw SyntaxError(this->path, this->tokenizer.get_current_token(),
                    "Function declaration '%s' is already defined.",
                    identifier_token.reference.c_str());
        }

        // Collect all the parameters, they must all be identifiers.
        while (!this->expect_current_token_as(TokenType::TOKEN_SEMICOLON))
        {

            Token parameter_token = this->tokenizer.get_current_token();
            this->validate_grammar_token<TokenType::TOKEN_IDENTIFIER>();
            parameters.push_back(parameter_token.reference);

        }

        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Push the scope.
        this->symbol_stack.push_table();

        // The function requires that the name of the function is scoped as a variable,
        // so we insert it into the symbol table as undefined.
        this->symbol_stack.insert_symbol_locally(identifier_token.reference, 
            Symbol(identifier_token.reference, Symboltype::SYMBOL_TYPE_UNDEFINED, 0));

        // Add all the parameters into the current scope.
        for (const std::string& parameter : parameters)
        {
            this->symbol_stack.insert_symbol_locally(parameter, 
                Symbol(parameter, Symboltype::SYMBOL_TYPE_PARAMETER, 0));
        }

        // Match all body statements.
        shared_ptr<ISyntaxNode> current_node = nullptr;
        while (this->expect_current_token_as(TokenType::TOKEN_EOF) == false)
        {

            if (this->expect_current_token_as(TokenType::TOKEN_KEYWORD_ENDFUNCTION)) break;

            try
            {
                current_node = this->match_body_statement();
                if (current_node == nullptr) break;
                body_statements.push_back(current_node);
            }
            catch (SyntaxException& syntax_error)
            {
                this->process_error(__LINE__, syntax_error, true);
            }

        }

        // Before we pop the scope, we ensure that the function identifier is not
        // undefined.
        Symbol *function_symbol = this->symbol_stack.get_symbol_locally(identifier_token.reference);
        if (function_symbol->type == Symboltype::SYMBOL_TYPE_UNDEFINED)
        {
            throw SyntaxError(this->path, identifier_token,
                    "The return value is not defined for %s.",
                    identifier_token.reference.c_str());
        }

        // Pop the scope.
        this->symbol_stack.pop_table();

        // Validate the end of the procedure.
        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_ENDFUNCTION>();

        // Insert the procedure into the symbol table, globally.
        this->symbol_stack.insert_symbol_globally(identifier_token.reference, Symbol(
            identifier_token.reference, Symboltype::SYMBOL_TYPE_FUNCTION, parameters.size()));

    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_KEYWORD_ENDFUNCTION);
        this->process_error(__LINE__, error, true);
    }

    // Construct the node.
    try
    {

        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Generate the procedure node.
        auto function_node = this->generate_node<SyntaxNodeFunctionStatement>();
        function_node->identifier_name = identifier_token.reference;
        function_node->parameters = parameters;
        function_node->body_statements = body_statements;
        return function_node;

    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_SEMICOLON);
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_variable_statement()
{

    try
    {

        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_VARIABLE>();

        // Get the identifier.
        Token identifier_token = this->tokenizer.get_current_token();
        this->validate_grammar_token<TokenType::TOKEN_IDENTIFIER>();

        // If the symbol is already defined, it is an error.
        if (this->symbol_stack.identifier_exists_locally(identifier_token.reference))
        {
            throw SyntaxError(this->path, this->tokenizer.get_current_token(),
                    "Variable declaration '%s' is already defined.",
                    identifier_token.reference.c_str());
        }

        // If the symbol exists in a parent scope, then issue a warning.
        if (this->symbol_stack.identifier_exists_above(identifier_token.reference))
        {

            SyntaxWarning warning(this->path, this->tokenizer.get_current_token(),
                    "Variable declaration '%s' shadows a parent scope.",
                    identifier_token.reference.c_str());
            if (ApplicationParameters::runtime_warnings_as_errors) 
            {
                throw warning;
            }
            else
            {
                this->process_warning(__LINE__, warning, true);
            }

        }

        // NOTE(Chris): There is potential here to make size optional in the grammar.
        //              This will actually cause some issues with the existing script
        //              files that I have, so I'm going to leave it as mandatory for now.

        if (this->expect_current_token_as(TokenType::TOKEN_COLON_EQUALS))
        {
            throw SyntaxError(this->path, this->tokenizer.get_current_token(),
                    "Unexpected ':=' encountered, did you mean to specify a size?");
        }

        // Get the size.
        shared_ptr<ISyntaxNode> size = this->match_expression();

        // Get the optional expressions.
        std::vector<shared_ptr<ISyntaxNode>> optional_dimensions;
        i32 arity = 0;
        while (!this->expect_current_token_as(TokenType::TOKEN_SEMICOLON) &&
               !this->expect_current_token_as(TokenType::TOKEN_COLON_EQUALS))
        {

            if (this->expect_current_token_as(TokenType::TOKEN_EOF))
            {
                throw SyntaxError(this->path, this->tokenizer.get_current_token(),
                        "Unexpected end-of-file encountered.");
            }

            shared_ptr<ISyntaxNode> optional_dimension = this->match_expression();
            optional_dimensions.push_back(optional_dimension);
            arity++;

        }

        // Get the optional definition statement.
        shared_ptr<ISyntaxNode> definition_statement = nullptr;
        if (this->expect_current_token_as(TokenType::TOKEN_COLON_EQUALS))
        {
            this->tokenizer.shift();
            definition_statement = this->match_expression();
        }

        // Validate the semicolon.
        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Insert the symbol into the symbol table.
        Symboltype insert_type = (arity > 0) ? Symboltype::SYMBOL_TYPE_ARRAY : Symboltype::SYMBOL_TYPE_VARIABLE;
        this->symbol_stack.insert_symbol_locally(identifier_token.reference, Symbol(
            identifier_token.reference, insert_type, arity));

        // Generate the variable node.
        auto variable_node = this->generate_node<SyntaxNodeVariableStatement>();
        variable_node->variable_name = identifier_token.reference;
        variable_node->right_hand_side = definition_statement;
        variable_node->size = size;
        variable_node->dimensions = optional_dimensions;
        return variable_node;

    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_SEMICOLON);
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_scope_statement()
{

    std::vector<shared_ptr<ISyntaxNode>> body_statements;

    // Since we are processing that needs to recover to the end of its block,
    // we need to wrap the entire block in a try-catch block and allow this
    // try-catch to synchronize to ENDSCOPE on error.
    try
    {

        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_SCOPE>();
        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Push the scope.
        this->symbol_stack.push_table();

        // Match all body statements.
        shared_ptr<ISyntaxNode> current_node = nullptr;
        while (this->expect_current_token_as(TokenType::TOKEN_EOF) == false)
        {

            if (this->expect_current_token_as(TokenType::TOKEN_KEYWORD_ENDSCOPE)) break;

            try
            {
                current_node = this->match_body_statement();
                if (current_node == nullptr) break;
                body_statements.push_back(current_node);
            }
            catch (SyntaxException& syntax_error)
            {
                this->process_error(__LINE__, syntax_error, true);
            }

        }

        // Pop the scope.
        this->symbol_stack.pop_table();

        // Validate the end of the scope.
        this->validate_grammar_token<TokenType::TOKEN_KEYWORD_ENDSCOPE>();

    }
    catch (SyntaxException& error)
    {

        // We recover here, so we let this fall through and validate the semicolon.
        this->synchronize_to(TokenType::TOKEN_KEYWORD_ENDSCOPE);
        this->process_error(__LINE__, error, true);

    }

    try
    {
        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        // Generate the scope node.
        auto scope_node = this->generate_node<SyntaxNodeScopeStatement>();
        scope_node->children = body_statements;
        return scope_node;

    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_SEMICOLON);
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_expression_statement()
{

    try
    {

        shared_ptr<ISyntaxNode> expression = this->match_expression();
        this->validate_grammar_token<TokenType::TOKEN_SEMICOLON>();

        auto expression_statement = this->generate_node<SyntaxNodeExpressionStatement>();
        expression_statement->expression = expression;
        return expression_statement;

    }
    catch (SyntaxException& error)
    {
        this->synchronize_to(TokenType::TOKEN_SEMICOLON);
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_expression()
{

    shared_ptr<ISyntaxNode> expression = this->match_procedure_call();
    return expression;

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_procedure_call()
{

    try 
    {

        // Check if the next token is an identifier.
        Token identifier_token = this->tokenizer.get_current_token();

        // Is the identifier a procedure?
        if (!this->symbol_stack.identifier_exists_globally(identifier_token.reference))
        {
            return this->match_assignment();
        }

        Symbol* symbol = this->symbol_stack.get_symbol_globally(identifier_token.reference);
        if (symbol->type != Symboltype::SYMBOL_TYPE_PROCEDURE)
        {
            return this->match_assignment();
        }

        // The symbol is a procedure, so we can match it.
        this->tokenizer.shift();

        // Match all the parameters. Procedure calls, by language specification don't
        // use parenthesis to match their parameters. Trust me, I wouldn't have done it
        // this way, but it's what I have to work with.
        std::vector<shared_ptr<ISyntaxNode>> parameters;
        while (!this->expect_current_token_as(TokenType::TOKEN_SEMICOLON))
        {

            if (this->expect_current_token_as(TokenType::TOKEN_EOF))
            {
                throw SyntaxError(this->path, this->tokenizer.get_current_token(),
                        "Unexpected end-of-file encountered.");
            }

            // Get the parameter.
            shared_ptr<ISyntaxNode> parameter = this->match_expression();
            parameters.push_back(parameter);

        }

        // Validate the arity.
        if (parameters.size() != symbol->arity)
        {
            throw SyntaxError(this->path, identifier_token,
                    "Procedure '%s' expects %d parameters, but %d were provided.",
                    identifier_token.reference.c_str(), symbol->arity, parameters.size());
        }

        // Generate the procedure call node.
        auto procedure_call_node = this->generate_node<SyntaxNodeProcedureCall>();
        procedure_call_node->procedure_name = identifier_token.reference;
        procedure_call_node->parameters = parameters;
        return procedure_call_node;

    }
    catch (SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_assignment()
{

    try
    {

        // Fetch the right hand side.
        shared_ptr<ISyntaxNode> left_hand_side = this->match_equality();

        // If it's not a primary node, then we can forward.
        if (left_hand_side->get_type() != SyntaxNodeType::NodeTypePrimary &&
            left_hand_side->get_type() != SyntaxNodeType::NodeTypeArrayIndex)
        {
            return left_hand_side;
        }

        std::string identifier;

        // If the left hand side is a primary type, then we can check if it's an identifier.
        if (left_hand_side->get_type() == SyntaxNodeType::NodeTypePrimary)
        {

            shared_ptr<SyntaxNodePrimary> primary_node = 
                dynamic_pointer_cast<SyntaxNodePrimary>(left_hand_side);

            if (primary_node->literal_type != TokenType::TOKEN_IDENTIFIER)
                return left_hand_side;

            identifier = primary_node->literal_reference;

        }

        // If the next token is not an assignment operator, then we can forward.
        if (!this->expect_current_token_as(TokenType::TOKEN_COLON_EQUALS))
            return left_hand_side;

        // Validate the colon equals.
        this->validate_grammar_token<TokenType::TOKEN_COLON_EQUALS>();

        // Validate that the symbol exists in the local symbol table, arrays are guaranteed
        // to exist in the symbol table intrinsically.
        if (left_hand_side->get_type() == SyntaxNodeType::NodeTypePrimary)
        {

            shared_ptr<SyntaxNodePrimary> primary_node = 
                dynamic_pointer_cast<SyntaxNodePrimary>(left_hand_side);

            if (!this->symbol_stack.identifier_exists(primary_node->literal_reference))
            {
                throw SyntaxError(this->path, this->tokenizer.get_current_token(),
                        "Undefined symbol '%s'.",
                        primary_node->literal_reference.c_str());
            }

        }

        // Now get the right hand side.
        shared_ptr<ISyntaxNode> right_hand_side = this->match_expression();

        // Get the symbol from the symbol table and then make it defined.
        // Arrays are automatically defined on initialization.
        if (left_hand_side->get_type() == SyntaxNodeType::NodeTypePrimary)
        {

            shared_ptr<SyntaxNodePrimary> primary_node = 
                dynamic_pointer_cast<SyntaxNodePrimary>(left_hand_side);

            Symbol* symbol = this->symbol_stack.get_symbol(primary_node->literal_reference);
            SF_ENSURE_PTR(symbol);
            if (symbol->type == Symboltype::SYMBOL_TYPE_UNDEFINED)
            {
                symbol->type = Symboltype::SYMBOL_TYPE_VARIABLE;
            }

        }

        // Generate the assignment node.
        auto assignment_node = this->generate_node<SyntaxNodeAssignment>();
        assignment_node->left = left_hand_side;
        assignment_node->right = right_hand_side;
        return dynamic_pointer_cast<ISyntaxNode>(assignment_node);

    }
    catch(SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_equality()
{

    try
    {

        shared_ptr<ISyntaxNode> left_hand_side = this->match_comparison();
        while (this->expect_current_token_as(TokenType::TOKEN_EQUALS) ||
               this->expect_current_token_as(TokenType::TOKEN_HASH))
        {

            Token operator_token = this->tokenizer.get_current_token();
            this->tokenizer.shift();

            shared_ptr<ISyntaxNode> right_hand_side = this->match_comparison();

            // Generate the equality node.
            auto equality_node = this->generate_node<SyntaxNodeEquality>();
            equality_node->left             = left_hand_side;
            equality_node->right            = right_hand_side;
            equality_node->operation_type   = operator_token.type;
            left_hand_side                  = dynamic_pointer_cast<ISyntaxNode>(equality_node);

        }

        return left_hand_side;

    }

    catch(SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_comparison()
{

    try
    {

        shared_ptr<ISyntaxNode> left_hand_side = this->match_term();
        while (this->expect_current_token_as(TokenType::TOKEN_LESS_THAN) ||
               this->expect_current_token_as(TokenType::TOKEN_LESS_THAN_EQUALS) ||
               this->expect_current_token_as(TokenType::TOKEN_GREATER_THAN) ||
               this->expect_current_token_as(TokenType::TOKEN_GREATER_THAN_EQUALS))
        {

            Token operator_token = this->tokenizer.get_current_token();
            this->tokenizer.shift();

            shared_ptr<ISyntaxNode> right_hand_side = this->match_term();

            // Generate the comparison node.
            auto comparison_node = this->generate_node<SyntaxNodeComparison>();
            comparison_node->left           = left_hand_side;
            comparison_node->right          = right_hand_side;
            comparison_node->operation_type = operator_token.type;
            left_hand_side                  = dynamic_pointer_cast<ISyntaxNode>(comparison_node);

        }

        return left_hand_side;

    }

    catch (SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_term()
{

    try
    {

        shared_ptr<ISyntaxNode> left_hand_side = this->match_factor();
        while (this->expect_current_token_as(TokenType::TOKEN_PLUS) ||
               this->expect_current_token_as(TokenType::TOKEN_MINUS))
        {

            Token operator_token = this->tokenizer.get_current_token();
            this->tokenizer.shift();

            shared_ptr<ISyntaxNode> right_hand_side = this->match_factor();

            // Generate the term node.
            auto term_node = this->generate_node<SyntaxNodeTerm>();
            term_node->left             = left_hand_side;
            term_node->right            = right_hand_side;
            term_node->operation_type   = operator_token.type;
            left_hand_side              = dynamic_pointer_cast<ISyntaxNode>(term_node);

        }

        return left_hand_side;

    }

    catch (SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_factor()
{

    try
    {

        shared_ptr<ISyntaxNode> left_hand_side = this->match_magnitude();
        while (this->expect_current_token_as(TokenType::TOKEN_STAR) ||
               this->expect_current_token_as(TokenType::TOKEN_FORWARD_SLASH))
        {

            Token operator_token = this->tokenizer.get_current_token();
            this->tokenizer.shift();

            shared_ptr<ISyntaxNode> right_hand_side = this->match_magnitude();

            // Generate the factor node.
            auto factor_node = this->generate_node<SyntaxNodeFactor>();
            factor_node->left               = left_hand_side;
            factor_node->right              = right_hand_side;
            factor_node->operation_type     = operator_token.type;
            left_hand_side                  = dynamic_pointer_cast<ISyntaxNode>(factor_node);

        }

        return left_hand_side;

    }

    catch (SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_magnitude()
{

    try
    {

        shared_ptr<ISyntaxNode> left_hand_side = this->match_extraction();
        while (this->expect_current_token_as(TokenType::TOKEN_CARROT))
        {

            Token operator_token = this->tokenizer.get_current_token();
            this->tokenizer.shift();

            shared_ptr<ISyntaxNode> right_hand_side = this->match_extraction();

            // Generate the magnitude node.
            auto magnitude_node = this->generate_node<SyntaxNodeMagnitude>();
            magnitude_node->left            = left_hand_side;
            magnitude_node->right           = right_hand_side;
            magnitude_node->operation_type  = operator_token.type;
            left_hand_side                  = dynamic_pointer_cast<ISyntaxNode>(magnitude_node);

        }

        return left_hand_side;

    }

    catch (SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_extraction()
{

    try
    {

        shared_ptr<ISyntaxNode> left_hand_side = this->match_derivation();
        while (this->expect_current_token_as(TokenType::TOKEN_PIPE))
        {

            Token operator_token = this->tokenizer.get_current_token();
            this->tokenizer.shift();

            shared_ptr<ISyntaxNode> right_hand_side = this->match_extraction();

            // Generate the extraction node.
            auto extraction_node = this->generate_node<SyntaxNodeExtraction>();
            extraction_node->left           = left_hand_side;
            extraction_node->right          = right_hand_side;
            extraction_node->operation_type = operator_token.type;
            left_hand_side                  = dynamic_pointer_cast<ISyntaxNode>(extraction_node);

        }

        return left_hand_side;

    }

    catch (SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_derivation()
{

    try
    {
        shared_ptr<ISyntaxNode> left_hand_side = this->match_unary();
        while (this->expect_current_token_as(TokenType::TOKEN_PERCENT))
        {

            Token operator_token = this->tokenizer.get_current_token();
            this->tokenizer.shift();

            shared_ptr<ISyntaxNode> right_hand_side = this->match_unary();

            // Generate the derivation node.
            auto derivation_node = this->generate_node<SyntaxNodeDerivation>();
            derivation_node->left           = left_hand_side;
            derivation_node->right          = right_hand_side;
            derivation_node->operation_type = operator_token.type;
            left_hand_side                  = dynamic_pointer_cast<ISyntaxNode>(derivation_node);

        }

        return left_hand_side;
    }
    catch (SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_unary()
{

    try
    {

        if (this->expect_current_token_as(TokenType::TOKEN_MINUS))
        {

            Token operator_token = this->tokenizer.get_current_token();
            this->tokenizer.shift();

            shared_ptr<ISyntaxNode> right_hand_side = this->match_unary();

            // Generate the unary node.
            auto unary_node = this->generate_node<SyntaxNodeUnary>();
            unary_node->right           = right_hand_side;
            unary_node->operation_type  = operator_token.type;
            return unary_node;

        }

        shared_ptr<ISyntaxNode> right = this->match_function_call();
        return right;
        
    }

    catch (SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_function_call()
{

    try
    {

        // If the symbol is an identifer, we need to check if it is a function,
        // then validate it as a function call.
        Token identifier_token = this->tokenizer.get_current_token();
        if (!this->symbol_stack.identifier_exists_globally(identifier_token.reference))
        {
            return this->match_array_index();
        }

        Symbol* symbol = this->symbol_stack.get_symbol_globally(identifier_token.reference);
        if (symbol->type != Symboltype::SYMBOL_TYPE_FUNCTION)
        {
            return this->match_array_index();
        }

        // The symbol is a function, so we can match it.
        this->tokenizer.shift();
        this->validate_grammar_token<TokenType::TOKEN_LEFT_PARENTHESIS>();

        // Match all the parameters. We check for parenthesis here because functions
        // are superior to procedures.
        std::vector<shared_ptr<ISyntaxNode>> parameters;
        while (!this->expect_current_token_as(TokenType::TOKEN_RIGHT_PARENTHESIS))
        {

            if (this->expect_current_token_as(TokenType::TOKEN_EOF))
            {
                throw SyntaxError(this->path, this->tokenizer.get_current_token(),
                        "Unexpected end-of-file encountered.");
            }

            // Get the parameter.
            shared_ptr<ISyntaxNode> parameter = this->match_expression();
            parameters.push_back(parameter);

            if (this->expect_current_token_as(TokenType::TOKEN_COMMA))
            {
                this->tokenizer.shift();
            }

        }

        // Check for right parenthesis.
        this->validate_grammar_token<TokenType::TOKEN_RIGHT_PARENTHESIS>();

        // Validate the arity.
        if (parameters.size() != symbol->arity)
        {
            throw SyntaxError(this->path, identifier_token,
                    "Function '%s' expects %d parameters, but %d were provided.",
                    identifier_token.reference.c_str(), symbol->arity, parameters.size());
        }

        // Generate the function call node.
        auto function_call_node = this->generate_node<SyntaxNodeFunctionCall>();
        function_call_node->function_name = identifier_token.reference;
        function_call_node->parameters = parameters;
        return function_call_node;

    }
    catch (SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw;
    }

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_array_index()
{

    try
    {

        if (this->expect_current_token_as(TokenType::TOKEN_IDENTIFIER) &&
            this->expect_next_token_as(TokenType::TOKEN_LEFT_PARENTHESIS))
        {

            Token identifier_token = this->tokenizer.get_current_token();
            this->tokenizer.shift();
            this->tokenizer.shift();

            // Get the parameters.
            std::vector<shared_ptr<ISyntaxNode>> parameters;
            shared_ptr<ISyntaxNode> current_parameter = nullptr;
            i32 arity = 0;
            while (!this->expect_current_token_as(TokenType::TOKEN_RIGHT_PARENTHESIS))
            {

                if (this->expect_current_token_as(TokenType::TOKEN_EOF))
                {
                    throw SyntaxError(this->path, this->tokenizer.get_current_token(),
                            "Unexpected end-of-file encountered.");
                }

                try
                {
                    current_parameter = this->match_expression();
                    if (current_parameter == nullptr) break;
                    arity++;
                    parameters.push_back(current_parameter);
                }
                catch (SyntaxException& syntax_error)
                {
                    this->synchronize_up_to(TokenType::TOKEN_RIGHT_PARENTHESIS);
                    this->process_error(__LINE__, syntax_error, true);
                }

                if (this->expect_current_token_as(TokenType::TOKEN_COMMA))
                {
                    this->tokenizer.shift();
                }

            }

            // Validate the right parenthesis.
            this->validate_grammar_token<TokenType::TOKEN_RIGHT_PARENTHESIS>();

            // Validate the arity.
            Symbol* symbol = this->symbol_stack.get_symbol_locally(identifier_token.reference);
            if (symbol == nullptr)
            {
                throw SyntaxError(this->path, identifier_token,
                        "Undefined symbol '%s'.", identifier_token.reference.c_str());
            }
            if (symbol->type != Symboltype::SYMBOL_TYPE_ARRAY)
            {
                throw SyntaxError(this->path, identifier_token,
                        "Symbol '%s' is not an array.", identifier_token.reference.c_str());
            }
            if (arity != symbol->arity)
            {
                throw SyntaxError(this->path, identifier_token,
                        "Symbol '%s' expects %d arguments, but %d were provided.",
                        identifier_token.reference.c_str(), symbol->arity, arity);
            }

            // Generate the array index node.
            auto array_index_node = this->generate_node<SyntaxNodeArrayIndex>();
            array_index_node->variable_name = identifier_token.reference;
            array_index_node->indices = parameters;
            return array_index_node;

        }

        else
        {
            return this->match_primary();
        }

    }
    catch (SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw;
    }

    return this->match_primary();

}

shared_ptr<ISyntaxNode> SyntaxParser::
match_primary()
{

    try
    {

        if (this->expect_current_token_as(TokenType::TOKEN_REAL) ||
            this->expect_current_token_as(TokenType::TOKEN_INTEGER) ||
            this->expect_current_token_as(TokenType::TOKEN_STRING))
        {

            Token literal_token = this->tokenizer.get_current_token();
            this->tokenizer.shift();

            // Generate the primary node.
            auto primary_node = this->generate_node<SyntaxNodePrimary>();
            primary_node->literal_reference     = literal_token.reference;
            primary_node->literal_type          = literal_token.type;
            return primary_node;

        }

        else if (this->expect_current_token_as(TokenType::TOKEN_IDENTIFIER))
        {

            Token literal_token = this->tokenizer.get_current_token();
            this->tokenizer.shift();

            // Check if the token is undefined (it is declared).
            if (!this->symbol_stack.identifier_exists(literal_token.reference))
            {
                throw SyntaxError(this->path, literal_token,
                        "Undeclared symbol '%s' used in expression.", literal_token.reference.c_str());
            }

            // Generate the primary node.
            auto primary_node = this->generate_node<SyntaxNodePrimary>();
            primary_node->literal_reference     = literal_token.reference;
            primary_node->literal_type          = literal_token.type;
            return primary_node;

        }
        
        else if (this->expect_current_token_as(TokenType::TOKEN_LEFT_PARENTHESIS))
        {

            this->tokenizer.shift();
            shared_ptr<ISyntaxNode> inside = this->match_expression();
            this->validate_grammar_token<TokenType::TOKEN_RIGHT_PARENTHESIS>();

            // Generate the grouping node.
            auto grouping_node = this->generate_node<SyntaxNodeGrouping>();
            grouping_node->grouping = inside;
            return grouping_node;

        }

        Token current_token = this->tokenizer.get_current_token();
        throw SyntaxError(this->path, current_token,
                "Unexpected token encountered: '%s'.", current_token.reference.c_str());

    }

    catch (SyntaxException& error)
    {
        this->process_error(__LINE__, error, true);
        throw;
    }

}