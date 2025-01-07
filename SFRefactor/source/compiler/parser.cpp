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
        default: break;

    }

    // Expression statement.
    shared_ptr<ISyntaxNode> expression_statement = this->match_expression_statement();
    return expression_statement;

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



















// --- C-Style Parser Implementation -------------------------------------------
//
// The code that turns C++ programmers to Rust programmers.
//

/*
syntax_node*
source_parser_match_primary(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Literals.
    if (source_parser_match_token(parser, 3, TOKEN_REAL, TOKEN_INTEGER, TOKEN_STRING))
    {

        source_token literal = source_parser_consume_token(parser);

        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser, &literal, &object);

        syntax_node *primary_node = source_parser_push_node(parser);
        primary_node->type = PRIMARY_EXPRESSION_NODE;
        primary_node->primary.literal = object;
        primary_node->primary.type = type;
        return primary_node;

    }

    // Identifiers.
    else if (source_parser_match_token(parser, 1, TOKEN_IDENTIFIER))
    {
        
        source_token identifier = source_parser_consume_token(parser);

        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser, &identifier, &object);

        // Determine if the identifier is declared for use in expressions.
        if (!source_parser_identifier_is_declared(parser, object.identifier))
        {
            parser->error_count++; 
            
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->previous_token, 
                    PARSER_ERROR_UNDECLARED_IDENTIFIER, ": '%s'", object.identifier);
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        syntax_node *primary_node = source_parser_push_node(parser);
        primary_node->type = PRIMARY_EXPRESSION_NODE;
        primary_node->primary.literal = object;
        primary_node->primary.type = type;

        return primary_node;

    }

    // Groupings.
    else if (source_parser_match_token(parser, 1, TOKEN_LEFT_PARENTHESIS))
    {

        source_parser_consume_token(parser);

        syntax_node *inside = source_parser_match_expression(parser);
        if (source_parser_should_propagate_error(inside, parser, mem_state)) return NULL;

        if (!source_parser_expect_token(parser, TOKEN_RIGHT_PARENTHESIS))
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    PARSER_ERROR_EXPECTED_SYMBOL, ": expected ')'.", "");
            return NULL;
        }

        else
        {
            source_parser_consume_token(parser);
        }

        syntax_node *grouping_node = source_parser_push_node(parser);
        grouping_node->type = GROUPING_EXPRESSION_NODE;
        grouping_node->grouping.grouping = inside;

        return grouping_node;


    }
    
    // Error tokens.
    else if (source_parser_match_token(parser, 1, TOKEN_UNDEFINED_EOF))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_UNEXPECTED_EOF, ".", "");
        return NULL;
    }

    else if (source_parser_match_token(parser, 1, TOKEN_UNDEFINED_EOL))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_UNEXPECTED_EOL, ".", "");
        return NULL;
    }

    char hold_character;
    cc64 string = source_token_string_nullify(parser->tokenizer->current_token, &hold_character);
    parser->error_count++; 
    display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
            PARSER_ERROR_UNEXPECTED_SYMBOL, ": '%s'.", string);
    source_token_string_unnullify(parser->tokenizer->current_token, hold_character);

    return NULL;

}

syntax_node*
source_parser_match_array_index(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    if (source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {

        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser,
                parser->tokenizer->current_token, &object);
        cc64 identifier = object.identifier;

        // If it isn't defined, then its probably not an array.
        if (!source_parser_identifier_is_defined(parser, identifier))
        {
            syntax_node *forward = source_parser_match_primary(parser);
            return forward;
        }

        // Check if it is an indexable array.
        symbol *array_symbol = source_parser_locate_symbol(parser, identifier);
        if (array_symbol->type != SYMBOL_TYPE_ARRAY)
        {
            syntax_node *forward = source_parser_match_primary(parser);
            return forward;
        }

        // Consume the identifier.
        source_parser_consume_token(parser);

        // We have the array, now we need to ensure we match syntax.
        if (!source_parser_expect_token(parser, TOKEN_LEFT_PARENTHESIS))
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path,
                    parser->tokenizer->current_token,
                    PARSER_ERROR_EXPECTED_SYMBOL,
                    ": expected '(' for array indexing.", "");
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        // Consume the left parenthesis.
        source_parser_consume_token(parser);

        syntax_node *head_index_node = NULL;
        syntax_node *last_index_node = NULL;
        i32 arity_count = 0;
        while (!source_parser_match_token(parser, 2, TOKEN_RIGHT_PARENTHESIS, TOKEN_SEMICOLON))
        {

            if (source_parser_should_break_on_eof(parser)) break;

            // The first entry has arity zero and we don't need to check if it is
            // a comma. Otherwise, the entry must be a comma followed by an expression.
            if (arity_count != 0)
            {

                if (!source_parser_expect_token(parser, TOKEN_COMMA))
                {

                    parser->error_count++; 
                    display_error_message(parser->tokenizer->tokenizer.file_path,
                            parser->tokenizer->current_token,
                            PARSER_ERROR_EXPECTED_SYMBOL,
                            ": expected ',' for array indexing.", "");
                    source_parser_should_propagate_error(NULL, parser, mem_state);
                    return NULL;
                    
                }

                // Consume the comma.
                source_parser_consume_token(parser);

            }
 
            // Get the index expression.
            syntax_node *index = source_parser_match_expression(parser);
            if (source_parser_should_propagate_error(index, parser, mem_state))
            {
                return NULL;
            }

            // Set the parameter.
            if (head_index_node == NULL)
            {
                head_index_node = index;
                last_index_node = index;
            }

            else
            {
                last_index_node->next_node = index;
                last_index_node = index;
            }

            arity_count++;

        }

        // Validate the arity of the array access.
        if (array_symbol->arity != arity_count)
        {

            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path,
                    parser->tokenizer->current_token,
                    PARSER_ERROR_ARITY_MISMATCH,
                    ": array access members mismatch from definition of '%s'.",
                    identifier);
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;

        }

        // Check for right bracket.
        if (!source_parser_expect_token(parser, TOKEN_RIGHT_PARENTHESIS))
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path,
                    parser->tokenizer->current_token,
                    PARSER_ERROR_EXPECTED_SYMBOL,
                    ": expected ')' for array indexing.", "");
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        // Consume the right bracket.
        source_parser_consume_token(parser);

        syntax_node *array_index_node = source_parser_push_node(parser);
        array_index_node->type = ARRAY_INDEX_EXPRESSION_NODE;
        array_index_node->array_index.name = identifier;
        array_index_node->array_index.accessors = head_index_node;
        return array_index_node;

    }

    syntax_node *forward = source_parser_match_primary(parser);
    return forward;

}

syntax_node*
source_parser_match_function_call(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    if (source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {

        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser,
                parser->tokenizer->current_token, &object);
        cc64 identifier = object.identifier;

        // If it isn't defined, then its probably not a procedure.
        if (!source_parser_identifier_is_defined(parser, identifier))
        {
            syntax_node *forward = source_parser_match_array_index(parser);
            return forward;
        }

        // Check if it is procedure call.
        symbol *procedure_call = source_parser_locate_symbol(parser, identifier);
        if (procedure_call->type != SYMBOL_TYPE_FUNCTION)
        {
            syntax_node *forward = source_parser_match_array_index(parser);
            return forward;
        }

        // Consume the identifier token.
        source_parser_consume_token(parser);

        // Check for left bracket.
        if (!source_parser_expect_token(parser, TOKEN_LEFT_PARENTHESIS))
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path,
                    parser->tokenizer->current_token,
                    PARSER_ERROR_EXPECTED_SYMBOL,
                    ": expected '(' for procedure call.", "");
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        // Consume left parenthesis.
        source_parser_consume_token(parser);

        // Now we process expressions until semicolon. Expressions are self-validating
        // and therefore we only need to collect them.
        syntax_node *head_parameter_node = NULL;
        syntax_node *last_parameter_node = NULL;
        i32 arity_count = 0;
        while (!source_parser_match_token(parser, 2, TOKEN_RIGHT_PARENTHESIS, TOKEN_SEMICOLON))
        {

            if (source_parser_should_break_on_eof(parser)) break;
            if (arity_count != 0)
            {

                if (!source_parser_expect_token(parser, TOKEN_COMMA))
                {

                    parser->error_count++; 
                    display_error_message(parser->tokenizer->tokenizer.file_path,
                            parser->tokenizer->current_token,
                            PARSER_ERROR_EXPECTED_SYMBOL,
                            ": expected ',' for function parameters.", "");
                    source_parser_should_propagate_error(NULL, parser, mem_state);
                    return NULL;
                    
                }

                // Consume the comma.
                source_parser_consume_token(parser);

            }
 
            syntax_node *parameter = source_parser_match_expression(parser);
            if (source_parser_should_propagate_error(parameter, parser, mem_state))
            {
                return NULL;
            }

            // Set the parameter.
            if (head_parameter_node == NULL)
            {
                head_parameter_node = parameter;
                last_parameter_node = parameter;
            }

            else
            {
                last_parameter_node->next_node = parameter;
                last_parameter_node = parameter;
            }

            arity_count++;

        }

        // Check for right bracket.
        if (!source_parser_expect_token(parser, TOKEN_RIGHT_PARENTHESIS))
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path,
                    parser->tokenizer->current_token,
                    PARSER_ERROR_EXPECTED_SYMBOL,
                    ": expected ')' for procedure call.", "");
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        // Consume right parenthesis.
        source_parser_consume_token(parser);

        syntax_node *call_node = source_parser_push_node(parser);
        call_node->type = FUNCTION_CALL_EXPRESSION_NODE;
        call_node->func_call.identifier = identifier;
        call_node->func_call.parameters = head_parameter_node;
        
        if (arity_count != procedure_call->arity)
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    PARSER_ERROR_ARITY_MISMATCH, ": see definition of '%s'.", identifier);
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        // We don't check for semicolons here.
        
        return call_node;

    }

    syntax_node *forward = source_parser_match_array_index(parser);
    return forward;

}

syntax_node*
source_parser_match_unary(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    if (source_parser_match_token(parser, 1, TOKEN_MINUS))
    {
        
        syntax_operation_type operation = OPERATION_NEGATIVE_ASSOCIATE;
        source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_unary(parser);
        if (source_parser_should_propagate_error(right, parser, mem_state)) return NULL;

        syntax_node *unary_node = source_parser_push_node(parser);
        unary_node->type = UNARY_EXPRESSION_NODE;
        unary_node->unary.right = right;
        unary_node->unary.type = operation;

        return unary_node;

    }

    syntax_node *right = source_parser_match_function_call(parser);

    return right;

}

syntax_node*
source_parser_match_factor(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    syntax_node *left = source_parser_match_unary(parser);
    if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

    while (source_parser_match_token(parser, 2, TOKEN_STAR, TOKEN_FORWARD_SLASH))
    {

        source_token operation = source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_unary(parser);
        if (source_parser_should_propagate_error(right, parser, mem_state)) return NULL;

        syntax_node *binary_node = source_parser_push_node(parser);
        binary_node->type = BINARY_EXPRESSION_NODE;
        binary_node->binary.left = left;
        binary_node->binary.right = right;
        binary_node->binary.type = source_parser_token_to_operation(&operation);
        
        left = binary_node;

    }

    return left;

}

syntax_node*
source_parser_match_term(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    syntax_node *left = source_parser_match_factor(parser);
    if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

    while (source_parser_match_token(parser, 2, TOKEN_PLUS, TOKEN_MINUS))
    {

        source_token operation = source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_factor(parser);
        if (source_parser_should_propagate_error(right, parser, mem_state)) return NULL;

        syntax_node *binary_node = source_parser_push_node(parser);
        binary_node->type = BINARY_EXPRESSION_NODE;
        binary_node->binary.left = left;
        binary_node->binary.right = right;
        binary_node->binary.type = source_parser_token_to_operation(&operation);
        
        left = binary_node;

    }

    return left;

}

syntax_node*
source_parser_match_comparison(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    syntax_node *left = source_parser_match_term(parser);
    if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

    while (source_parser_match_token(parser, 4, TOKEN_LESS_THAN, TOKEN_LESS_THAN_EQUALS,
            TOKEN_GREATER_THAN, TOKEN_GREATER_THAN_EQUALS))
    {

        source_token operation = source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_term(parser);
        if (source_parser_should_propagate_error(right, parser, mem_state)) return NULL;

        syntax_node *binary_node = source_parser_push_node(parser);
        binary_node->type = BINARY_EXPRESSION_NODE;
        binary_node->binary.left = left;
        binary_node->binary.right = right;
        binary_node->binary.type = source_parser_token_to_operation(&operation);
        
        left = binary_node;

    }

    return left;

}

syntax_node*
source_parser_match_equality(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    syntax_node *left = source_parser_match_comparison(parser);
    if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

    while (source_parser_match_token(parser, 2, TOKEN_EQUALS, TOKEN_HASH))
    {

        source_token operation = source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_comparison(parser);
        if (source_parser_should_propagate_error(right, parser, mem_state)) return NULL;

        syntax_node *binary_node = source_parser_push_node(parser);
        binary_node->type = BINARY_EXPRESSION_NODE;
        binary_node->binary.left = left;
        binary_node->binary.right = right;
        binary_node->binary.type = source_parser_token_to_operation(&operation);

        left = binary_node;

    }

    return left;

}

syntax_node*
source_parser_match_expression(source_parser *parser)
{

    syntax_node *expression = source_parser_match_equality(parser);
    return expression;

}

syntax_node* 
source_parser_match_procedure_call(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    if (source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {

        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser,
                parser->tokenizer->current_token, &object);
        cc64 identifier = object.identifier;

        // If it isn't defined, then its probably not a procedure.
        if (!source_parser_identifier_is_defined(parser, identifier))
        {
            syntax_node *forward = source_parser_match_expression(parser);
            return forward;
        }

        // Check if it is procedure call.
        symbol *procedure_call = source_parser_locate_symbol(parser, identifier);
        if (procedure_call->type != SYMBOL_TYPE_PROCEDURE)
        {
            syntax_node *forward = source_parser_match_expression(parser);
            return forward;
        }

        // Consume the identifier token.
        source_parser_consume_token(parser);

        // Now we process expressions until semicolon. Expressions are self-validating
        // and therefore we only need to collect them.
        syntax_node *head_parameter_node = NULL;
        syntax_node *last_parameter_node = NULL;
        i32 arity_count = 0;
        while (!source_parser_match_token(parser, 1, TOKEN_SEMICOLON))
        {

            if (source_parser_should_break_on_eof(parser)) break;
 
            syntax_node *parameter = source_parser_match_expression(parser);
            if (source_parser_should_propagate_error(parameter, parser, mem_state))
            {
                source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
                return NULL;
            }

            // Set the parameter.
            if (head_parameter_node == NULL)
            {
                head_parameter_node = parameter;
                last_parameter_node = parameter;
            }

            else
            {
                last_parameter_node->next_node = parameter;
                last_parameter_node = parameter;
            }

            arity_count++;

        }

        syntax_node *call_node = source_parser_push_node(parser);
        call_node->type = PROCEDURE_CALL_EXPRESSION_NODE;
        call_node->proc_call.identifier = identifier;
        call_node->proc_call.parameters = head_parameter_node;
        
        if (arity_count != procedure_call->arity)
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    PARSER_ERROR_ARITY_MISMATCH, ": see definition of '%s'.", identifier);
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        // We don't check for semicolons here.
        
        return call_node;

    }

    syntax_node *forward = source_parser_match_expression(parser);
    return forward;

}

syntax_node*
source_parser_match_assignment(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

#if 1
    // 1. We match LHS.
    // 2. LHS can either be: primary identifier OR array index.
    // 3. If we have either, we check for assignment.
    // 4. If we have an assignment, we validate the assignment.
    // 5. Otherwise, we pass the node on.

    // Match the left hand side.
    syntax_node *left_hand_side = source_parser_match_procedure_call(parser);
    if (source_parser_should_propagate_error(left_hand_side, parser, mem_state))
    {
        return NULL;
    }

    // If we aren't primary or an index expression node, return it since we aren't
    // matching these for assignment expressions.
    if (!(left_hand_side->type == PRIMARY_EXPRESSION_NODE ||
        left_hand_side->type == ARRAY_INDEX_EXPRESSION_NODE))
    {
        return left_hand_side;
    }

    // If the next thing we're looking at isn't the assignment op, we return
    // the expression as-is.
    if (!source_parser_expect_token(parser, TOKEN_COLON_EQUALS))
    {
        return left_hand_side;
    }

    // Consume the colon equals.
    source_parser_consume_token(parser);

    // Now, assuming that we have the assignment operator, then we need to check if
    // the thing we're checking is a validatable.
    if (left_hand_side->type == PRIMARY_EXPRESSION_NODE)
    {

        if (left_hand_side->primary.type != OBJECT_IDENTIFIER)
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path,
                    parser->tokenizer->current_token,
                    PARSER_ERROR_UNEXPECTED_SYMBOL,
                    ": non-identifying primary symbol encountered: '%s'.",
                    left_hand_side->primary.literal.identifier);
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        // We need to check if the identifier is declared. We know it's an identifier
        // by this point.
        ccptr identifier = left_hand_side->primary.literal.identifier;
        if (!source_parser_identifier_is_declared(parser, identifier))
        {

            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    PARSER_ERROR_UNDECLARED_IDENTIFIER, ": '%s'.", identifier);
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;

        }

    }

    // Now, we have a valid LHS, we need a valid RHS.
    syntax_node *right_hand_side = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(right_hand_side, parser, mem_state))
        return NULL;

    // Now, we need to update the symbol table entry.
    if (left_hand_side->type == PRIMARY_EXPRESSION_NODE)
    {

        ccptr identifier = left_hand_side->primary.literal.identifier;
        symbol *variable_symbol = source_parser_locate_symbol(parser, identifier);
        if (source_parser_should_propagate_error(variable_symbol, parser, mem_state))
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    SYSTEM_ERROR_SYMBOL_SHOULD_BE_LOCATABLE, ": this is a runtime error.", "");
            return NULL;
        }

        variable_symbol->type = SYMBOL_TYPE_VARIABLE;

    }

    // We have the LHS and RHS, we can construct the node.
    syntax_node *assignment_node = source_parser_push_node(parser);
    assignment_node->type = ASSIGNMENT_EXPRESSION_NODE;
    assignment_node->assignment.left = left_hand_side;
    assignment_node->assignment.right = right_hand_side;
    return assignment_node;

#else 
    b32 current_is_identifier = source_parser_expect_token(parser, TOKEN_IDENTIFIER);
    b32 next_is_assignment = source_parser_next_token_is(parser, TOKEN_COLON_EQUALS);
    if (current_is_identifier && next_is_assignment)
    {

        // Get the object literal.
        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser,
                parser->tokenizer->current_token, &object);
        cc64 identifier = object.identifier;

        // We need to check if the identifier is declared.
        if (!source_parser_identifier_is_declared(parser, identifier))
        {

            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    PARSER_ERROR_UNDECLARED_IDENTIFIER, ": '%s'.", identifier);
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;

        }

        // Consume the identifier and assignment operator tokens.
        source_parser_consume_token(parser); // Identifier.
        source_parser_consume_token(parser); // Assignment.

        // Now we can generate the remaining part of the expression.          
        syntax_node *assignment_expression = source_parser_match_expression(parser);
        if (source_parser_should_propagate_error(assignment_expression, parser, mem_state))
            return NULL;

        // At this point, the assignment expression should be valid and there
        // is an identifier to set.
        symbol *variable_symbol = source_parser_locate_symbol(parser, identifier);
        if (source_parser_should_propagate_error(variable_symbol, parser, mem_state))
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    SYSTEM_ERROR_SYMBOL_SHOULD_BE_LOCATABLE, ": this is a runtime error.", "");
            return NULL;
        }

        variable_symbol->type = SYMBOL_TYPE_VARIABLE;

        // Now we can return back the syntax node.
        syntax_node *assignment_node = source_parser_push_node(parser);
        assignment_node->type = ASSIGNMENT_EXPRESSION_NODE;
        assignment_node->assignment.identifier = identifier;
        assignment_node->assignment.right = assignment_expression;

        return assignment_node;
        
    }

    syntax_node *forward = source_parser_match_procedure_call(parser);
    return forward;
#endif

}

syntax_node*
source_parser_match_expression_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);
    
    syntax_node *expression = source_parser_match_assignment(parser);
    if (source_parser_should_propagate_error(expression, parser, mem_state))
    {
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    syntax_node *expression_node = source_parser_push_node(parser);
    expression_node->type = EXPRESSION_STATEMENT_NODE;
    expression_node->expression.expression = expression;

    // Expect a semi-colon at EOS.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    return expression_node;

}

syntax_node*
source_parser_match_variable_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);
    
    // Consume the VARIABLE token.
    source_parser_consume_token(parser);

    // Generate the variable node.
    syntax_node *variable_node = source_parser_push_node(parser);
    variable_node->type = VARIABLE_STATEMENT_NODE;

    // Check for identifier.
    if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_IDENTIFIER, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    else
    {

        source_token identifier = source_parser_consume_token(parser);

        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser, &identifier, &object);
        assert(type == OBJECT_IDENTIFIER); // This should always be true.

        if (source_parser_identifier_is_declared_in_scope(parser, object.identifier))
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path,
                    parser->tokenizer->previous_token, 
                    PARSER_ERROR_VARIABLE_REDECLARATION, ": '%s'.", object.identifier);
            source_parser_should_propagate_error(NULL, parser, mem_state);
            source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
            return NULL;
        }

        else if (source_parser_identifier_is_declared_above_scope(parser, object.identifier))
        {
            display_warning_message(parser->tokenizer->tokenizer.file_path,
                    parser->tokenizer->previous_token,
                    PARSER_WARNING_VARIABLE_SCOPE_SHADOW,
                    ", see previous declaration of '%s'", object.identifier);
        }

        variable_node->variable.name = object.identifier;

    }

    // The first expression is required.
    syntax_node *size_expression = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(size_expression, parser, mem_state)) 
    {
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    variable_node->variable.size = size_expression;

    // Process all additional expression statement afterwards as array dimensions.
    // Stopping points are :=, ;, or EOF.
    syntax_node *head_dimension_expression = NULL;
    syntax_node *last_dimension_expression = NULL;
    u32 array_arity = 0;
    while (!source_parser_match_token(parser, 2, TOKEN_SEMICOLON, TOKEN_COLON_EQUALS))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        syntax_node *expression = source_parser_match_expression(parser);

        // The expression could be NULL, indicating there was an error.
        // Synchronize to the next semi-colon and return an invalid statement.
        if (source_parser_should_propagate_error(expression, parser, mem_state))
        {
            source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
            return NULL;
        }

        array_arity++;

        // First expression.
        if (head_dimension_expression == NULL)
        {
            head_dimension_expression = expression;
            last_dimension_expression = expression;
        }

        // All other expressions.
        else
        {
            last_dimension_expression->next_node = expression;   
            last_dimension_expression = expression;
        }
        
    }

    // It is valid for this to evaluate as NULL, optional specification.
    variable_node->variable.dimensions = head_dimension_expression;

    if (source_parser_expect_token(parser, TOKEN_COLON_EQUALS))
    {

        source_parser_consume_token(parser);
        syntax_node *expression = source_parser_match_expression(parser);
        if (source_parser_should_propagate_error(expression, parser, mem_state))
        {
            source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
            return NULL;
        }

        variable_node->variable.assignment = expression;

    }

    // Check for semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        return NULL;
    }
    else
    {
        source_parser_consume_token(parser);
    }

    // Variable is considered valid, we can insert into symbol table.
    symbol *identifier = source_parser_insert_into_symbol_table(parser,
            variable_node->variable.name);
    if (variable_node->variable.assignment != NULL)
    {
        identifier->type = SYMBOL_TYPE_VARIABLE;
    }
    else if (variable_node->variable.dimensions != NULL)
    {
        identifier->type = SYMBOL_TYPE_ARRAY;
        identifier->arity = array_arity;
    }
    else
    {
        identifier->type = SYMBOL_TYPE_UNDEFINED;
    }

    return variable_node;

}

syntax_node*
source_parser_match_read_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the read token.
    source_parser_consume_token(parser);

    // The following token is an expression which matches the write location.
    syntax_node *location = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(location, parser, mem_state))
    {
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    // The following should be a valid, declared identifier.
    if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->previous_token,
                PARSER_ERROR_EXPECTED_IDENTIFIER, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    source_token identifier = source_parser_consume_token(parser);
    object_literal object = {0};
    object_type type = source_parser_token_to_literal(parser, &identifier, &object);
    assert(type == OBJECT_IDENTIFIER); // This should always be true.
    
    if (!source_parser_identifier_is_declared(parser, object.identifier))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->previous_token,
                PARSER_ERROR_UNDECLARED_IDENTIFIER, ": '%s'.", object.identifier);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // If no semicolon, synchronize.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume semiclon.
    source_parser_consume_token(parser);

    syntax_node *read_node = source_parser_push_node(parser);
    read_node->type = READ_STATEMENT_NODE;
    read_node->read.identifier = object.identifier;
    read_node->read.location = location;

    symbol* read_var = source_parser_locate_symbol(parser, object.identifier);
    read_var->type = SYMBOL_TYPE_VARIABLE;

    return read_node;

}

syntax_node*
source_parser_match_write_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the write token.
    source_parser_consume_token(parser);

    // The following token is an expression which matches the write location.
    syntax_node *location = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(location, parser, mem_state))
    {
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    // We have the location, now we need the list of statements that follow.
    syntax_node *head_parameter_node = NULL;
    syntax_node *last_parameter_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_SEMICOLON))
    {

        if (source_parser_should_break_on_eof(parser)) break;

        syntax_node *parameter = source_parser_match_expression(parser);
        if (source_parser_should_propagate_error(parameter, parser, mem_state))
        {
            source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
            return NULL;
        }

        if (head_parameter_node == NULL)
        {
            head_parameter_node = parameter;
            last_parameter_node = parameter;
        }

        else
        {
            last_parameter_node->next_node = parameter;
            last_parameter_node = parameter;
        }

    }

    // If no semicolon, synchronize.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume semiclon.
    source_parser_consume_token(parser);


    // Create the node.
    syntax_node *write_node = source_parser_push_node(parser);
    write_node->type = WRITE_STATEMENT_NODE;
    write_node->write.location = location;
    write_node->write.body_expressions = head_parameter_node;

    return write_node;

}

syntax_node* 
source_parser_match_scope_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the scope token.
    source_parser_consume_token(parser);

    // If no semicolon, synchronize to the end scope.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);

        // Synchronize to endscope, then check for semicolon. If it is, consume it
        // so that we align to the next area of parsable code. If not, there is yet
        // another error that needs to be processed.
        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDSCOPE))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume semicolon.
    source_parser_consume_token(parser);

    // Create the scope node.
    syntax_node *scope_node = source_parser_push_node(parser);
    scope_node->type = SCOPE_STATEMENT_NODE;

    // Push a new symbol table.
    source_parser_push_symbol_table(parser);

    // Process all statements inside the scope block.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_KEYWORD_ENDSCOPE))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        syntax_node *statement = source_parser_match_statement(parser);

        // The statement could be NULL, which we ignore and move on. Synchronization
        // happens inside statements.
        if (statement == NULL)
        {
            continue;
        }

        // First statement.
        if (head_statement_node == NULL)
        {
            head_statement_node = statement;
            last_statement_node = statement;
        }

        // All other statements.
        else
        {
            last_statement_node->next_node = statement;   
            last_statement_node = statement;
        }

    }

    // Pop the symbol table.
    source_parser_pop_symbol_table(parser);
    scope_node->scope.body_statements = head_statement_node;

    // We assume now that the ending token is the end scope token.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_ENDSCOPE))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SYMBOL, ": expected 'endscope' keyword.", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDSCOPE);
        return NULL;
    }

    // Consume endscope.
    source_parser_consume_token(parser);

    // If no semicolon, synchronize.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume semiclon.
    source_parser_consume_token(parser);

    return scope_node;

}

syntax_node*
source_parser_match_loop_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the LOOP token.
    source_parser_consume_token(parser);

    // The first parameter of a loop is an identifier.
    if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SYMBOL, ": expected identifier in loop expression.", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDLOOP))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;
    }

    // Consume the identifier and then convert it to an object.
    source_token identifier = source_parser_consume_token(parser);
    object_literal object = {0};
    object_type type = source_parser_token_to_literal(parser, &identifier, &object);
    assert(type == OBJECT_IDENTIFIER); // This should always be true.

    // The second part of a loop is the initial bounds.
    syntax_node *initial_bounds = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(initial_bounds, parser, mem_state))
    {

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDLOOP))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // The third part of the loop is the exit bounds.
    syntax_node *exit_bounds = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(exit_bounds, parser, mem_state))
    {

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDLOOP))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // The fourth part is optional, so only check if the following token
    // isn't the semicolon.
    syntax_node *increment = NULL;
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {
        increment = source_parser_match_expression(parser);
        if (source_parser_should_propagate_error(increment, parser, mem_state))
        {

            if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDLOOP))
            {
                if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                    source_parser_consume_token(parser);
            }

            return NULL;

        }
    }

    // Finally, we require the last token to be a semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDLOOP))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    // Create the while node.
    syntax_node *loop_node = source_parser_push_node(parser);
    loop_node->type = LOOP_STATEMENT_NODE;
    loop_node->for_loop.iterator_identifier = object.identifier;
    loop_node->for_loop.initial_value_expression = initial_bounds;
    loop_node->for_loop.terminate_value_expression = exit_bounds;
    loop_node->for_loop.step_value_expression = increment;

    // Push the iterator symbol into the current table. This iterator is a known
    // variable with a known initial value. This value corresponds to the initial
    // value expression, and each iterator gets reset to the next iterator step value
    // which is by default to be one.
    //
    // NOTE(Chris): We may want to slightly adjust the procedure here to take a
    //              variable node instead, setting it to have an assignment to the
    //              initial value expression. Either way works, but the actual transpilation
    //              will need to take the current format into account that the actual
    //              value of this iterator value is the initial expression + iterator.
    //              Ugly, for sure.
    source_parser_push_symbol_table(parser);
    symbol *iterator_symbol = source_parser_insert_into_symbol_table(parser, object.identifier);
    iterator_symbol->type = SYMBOL_TYPE_VARIABLE;

    // Process all statements inside the scope block.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_KEYWORD_ENDLOOP))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        syntax_node *statement = source_parser_match_statement(parser);

        // The statement could be NULL, which we ignore and move on. Synchronization
        // happens inside statements.
        if (statement == NULL)
        {
            continue;
        }

        // First statement.
        if (head_statement_node == NULL)
        {
            head_statement_node = statement;
            last_statement_node = statement;
        }

        // All other statements.
        else
        {
            last_statement_node->next_node = statement;   
            last_statement_node = statement;
        }

    }

    // Pop the symbol table.
    source_parser_pop_symbol_table(parser);
    loop_node->for_loop.body_statements = head_statement_node;

    // We are assuming the following the token is ENDLOOP token.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_ENDLOOP))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SYMBOL, ": expected 'endloop' keyword.", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDLOOP);
        return NULL;
    }

    // Consume ENDLOOP.
    source_parser_consume_token(parser);

    // If no semicolon, synchronize to the next semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume SEMICOLON.
    source_parser_consume_token(parser);

    return loop_node;

}

syntax_node*
source_parser_match_if_statement(source_parser *parser)
{
    
    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the IF token.
    source_parser_consume_token(parser);

    // The first part of the if-statment is a eval-expression, if there's an error,
    // we will probably match until ENDIF to make things easier on the error checker.
    syntax_node *if_expression = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(if_expression, parser, mem_state))
    {

        // Synchronize to the ENDWHILE token.
        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDIF))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // We have the expression, now we need to terminate the semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDIF))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    // Generate the syntax node.
    syntax_node *if_node = source_parser_push_node(parser);
    if_node->type = IF_STATEMENT_NODE;
    if_node->if_conditional.evaluation_expression = if_expression;

    // Push a new symbol table.
    source_parser_push_symbol_table(parser);

    // This is the tricky bit here, we need to match to either an ENDIF or an ELSEIF.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 2, TOKEN_KEYWORD_ELSEIF, TOKEN_KEYWORD_ENDIF))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        syntax_node *statement = source_parser_match_statement(parser);

        // The statement could be NULL, which we ignore and move on. Synchronization
        // happens inside statements.
        if (statement == NULL)
        {
            continue;
        }

        // First statement.
        if (head_statement_node == NULL)
        {
            head_statement_node = statement;
            last_statement_node = statement;
        }

        // All other statements.
        else
        {
            last_statement_node->next_node = statement;   
            last_statement_node = statement;
        }

    }

    // Pop the symbol table.
    source_parser_pop_symbol_table(parser);
    if_node->if_conditional.body_statements = head_statement_node;

    // Now, we process *all* elseif blocks. It's sort of like we're recursively
    // descending, but this behavior tends to be more self-contained than what is
    // normally done.
    syntax_node *else_head = NULL;
    syntax_node *else_last = NULL;
    while (source_parser_match_token(parser, 1, TOKEN_KEYWORD_ELSEIF))
    {

        // Consume the ELSEIF token or break on EOF.
        source_parser_consume_token(parser);
        if (source_parser_should_break_on_eof(parser)) break; 

        // The following is the elseif eval.
        syntax_node *elseif_expression = source_parser_match_expression(parser);
        if (source_parser_should_propagate_error(elseif_expression, parser, mem_state))
        {

            // Synchronize to the ENDWHILE token.
            if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDIF))
            {
                if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                    source_parser_consume_token(parser);
            }

            return NULL;

        }
        
        // Now we have a semicolon to take care of.
        if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
        {

            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
            source_parser_should_propagate_error(NULL, parser, mem_state);

            if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDIF))
            {
                if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                    source_parser_consume_token(parser);
            }

            return NULL;

        }

        // Consume the semicolon.
        source_parser_consume_token(parser);

        // Consume all statements until we reach ELSEIF/ENDIF as-per-usual.
        source_parser_push_symbol_table(parser);

        // This is the tricky bit here, we need to match to either an ENDIF or an ELSEIF.
        syntax_node *head_statement_node = NULL;
        syntax_node *last_statement_node = NULL;
        while (!source_parser_match_token(parser, 2, TOKEN_KEYWORD_ELSEIF, TOKEN_KEYWORD_ENDIF))
        {

            if (source_parser_should_break_on_eof(parser)) break;
            syntax_node *statement = source_parser_match_statement(parser);

            // The statement could be NULL, which we ignore and move on. Synchronization
            // happens inside statements.
            if (statement == NULL)
            {
                continue;
            }

            // First statement.
            if (head_statement_node == NULL)
            {
                head_statement_node = statement;
                last_statement_node = statement;
            }

            // All other statements.
            else
            {
                last_statement_node->next_node = statement;   
                last_statement_node = statement;
            }

        }

        // Pop the symbol table.
        source_parser_pop_symbol_table(parser);

        // So we have the eval-expression and the body statements, now we need to
        // connect them all together.
        syntax_node *elseif_node = source_parser_push_node(parser);
        elseif_node->type = ELSEIF_STATEMENT_NODE;
        elseif_node->elseif_conditional.body_statements = head_statement_node;
        elseif_node->elseif_conditional.evaluation_expression = elseif_expression;

        if (else_head == NULL)
        {

            // Prepare the chain.
            else_head = elseif_node;
            else_last = elseif_node;

        }
        
        else
        {

            // Chain elseif node.
            else_last->elseif_conditional.else_statement = elseif_node;
            else_last = elseif_node;

        }

    }

    // We need to set the elseif root node.
    if_node->if_conditional.else_statement = else_head;

    // With all that done, we should *expect* the following token to be a ENDIF
    // token or an error, which ever it may be.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_ENDIF))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SYMBOL, ": expected 'endif' keyword.", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDIF);
        return NULL;
    }

    // Consume the ENDIF token.
    source_parser_consume_token(parser);

    // Now, at the end of all this jank, we expect the semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDIF))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    return if_node;

}

syntax_node* 
source_parser_match_while_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the while token.
    source_parser_consume_token(parser);

    // The following expression is the check expression. If we can't get the expression,
    // we synchronize to the ENDWHILE token which ensures interior statements aren't
    // leaked into further parsing.
    syntax_node *check_expression = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(check_expression, parser, mem_state))
    {

        // Synchronize to the ENDWHILE token.
        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDWHILE))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // If no semicolon, synchronize to the ENDWHILE token.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDWHILE))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    // Create the while node.
    syntax_node *while_node = source_parser_push_node(parser);
    while_node->type = WHILE_STATEMENT_NODE;
    while_node->while_loop.evaluation_expression = check_expression;

    // Push a new symbol table.
    source_parser_push_symbol_table(parser);

    // Process all statements inside the scope block.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_KEYWORD_ENDWHILE))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        syntax_node *statement = source_parser_match_statement(parser);

        // The statement could be NULL, which we ignore and move on. Synchronization
        // happens inside statements.
        if (statement == NULL)
        {
            continue;
        }

        // First statement.
        if (head_statement_node == NULL)
        {
            head_statement_node = statement;
            last_statement_node = statement;
        }

        // All other statements.
        else
        {
            last_statement_node->next_node = statement;   
            last_statement_node = statement;
        }

    }

    // Pop the symbol table.
    source_parser_pop_symbol_table(parser);
    while_node->while_loop.body_statements = head_statement_node;

    // We are assuming the following the token is ENDWHILE token.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_ENDWHILE))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SYMBOL, ": expected 'endwhile' keyword.", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDWHILE);
        return NULL;
    }

    // Consume ENDWHILE.
    source_parser_consume_token(parser);

    // If no semicolon, synchronize to the next semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume SEMICOLON.
    source_parser_consume_token(parser);

    return while_node;


}

syntax_node* 
source_parser_match_procedure_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the PROCEDURE token.
    source_parser_consume_token(parser);

    // The first identifier is the name of procedure.
    if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_IDENTIFIER, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDPROCEDURE))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;
    }

    // Get the identifier.
    source_token identifier = source_parser_consume_token(parser);
    object_literal object = {0};
    object_type type = source_parser_token_to_literal(parser, &identifier, &object);
    assert(type == OBJECT_IDENTIFIER); // This should always be true.

    // We need to check if the identifier for the procedure is already declared.
    if (source_parser_identifier_is_declared(parser, object.identifier))
    {
        
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_VARIABLE_REDECLARATION, ", procedure is already defined.", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDPROCEDURE))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // The procedure wasn't declared, so we define it here. This goes in the parent
    // scope.
    //
    // NOTE(Chris): Do we want to limit the scope depth of function/procedure decs?
    //              We can probably do this grammatically rather than coding for an
    //              edge case.
    symbol *param_symbol = source_parser_insert_into_symbol_table(parser, object.identifier);
    param_symbol->type = SYMBOL_TYPE_PROCEDURE;

    // Push the node.
    syntax_node *procedure_node = source_parser_push_node(parser);
    procedure_node->type = PROCEDURE_STATEMENT_NODE;
    procedure_node->procedure.name = object.identifier;

    // We can push the scope here.
    source_parser_push_symbol_table(parser);

    // Process all identifiers until semicolon, these are the parameters. These
    // parameters are also shoved into the scope for validation.
    syntax_node *head_parameter_node = NULL;
    syntax_node *last_parameter_node = NULL;
    i32 arity_count = 0;
    while (!source_parser_match_token(parser, 1, TOKEN_SEMICOLON))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        
        // Each parameter must be an identifier.
        if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    PARSER_ERROR_EXPECTED_IDENTIFIER, ".", "");
            source_parser_should_propagate_error(NULL, parser, mem_state);

            if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDPROCEDURE))
            {
                if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                    source_parser_consume_token(parser);
            }

            return NULL;
        }

        // Get the identifier.
        source_token identifier = source_parser_consume_token(parser);
        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser, &identifier, &object);
        assert(type == OBJECT_IDENTIFIER); // This should always be true.

        // Create the parameter node.
        syntax_node *param_node = source_parser_push_node(parser);
        param_node->type = PARAMETER_STATEMENT_NODE;
        param_node->parameter.name = object.identifier;
        param_node->parameter.next_parameter = NULL;

        symbol *param_symbol = source_parser_insert_into_symbol_table(parser, object.identifier);
        param_symbol->type = SYMBOL_TYPE_VARIABLE;

        if (head_parameter_node == NULL)
        {
            head_parameter_node = param_node;
            last_parameter_node = param_node;
        }

        else
        {
            last_parameter_node->parameter.next_parameter = param_node;
            last_parameter_node = param_node;
        }

        arity_count++;

    }

    // We should be at the semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDPROCEDURE))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    // Set the parameter node in the procedure. This may be NULL, which is fine.
    procedure_node->procedure.parameters = head_parameter_node;
    param_symbol->arity = arity_count;

    // Finally, we process all body statements.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_KEYWORD_ENDPROCEDURE))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        syntax_node *statement = source_parser_match_statement(parser);

        // The statement could be NULL, which we ignore and move on. Synchronization
        // happens inside statements.
        if (statement == NULL)
        {
            continue;
        }

        // First statement.
        if (head_statement_node == NULL)
        {
            head_statement_node = statement;
            last_statement_node = statement;
        }

        // All other statements.
        else
        {
            last_statement_node->next_node = statement;   
            last_statement_node = statement;
        }

    }


    // Expecting the ENDPROCEDURE.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_ENDPROCEDURE))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SYMBOL, ", expected 'endprocedure' keyword.", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDPROCEDURE);
        return NULL;
    }

    // Consume the ENDIF token.
    source_parser_consume_token(parser);

    // Pop the scope once we're done.
    source_parser_pop_symbol_table(parser);
    procedure_node->procedure.body_statements = head_statement_node;

    // Now, at the end of all this jank, we expect the semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    return procedure_node;
}

syntax_node* 
source_parser_match_function_statement(source_parser *parser)
{
 
    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the FUNCTION token.
    source_parser_consume_token(parser);

    // The first identifier is the name of procedure.
    if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_IDENTIFIER, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDFUNCTION))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;
    }

    // Get the identifier.
    source_token identifier = source_parser_consume_token(parser);
    object_literal object = {0};
    object_type type = source_parser_token_to_literal(parser, &identifier, &object);
    assert(type == OBJECT_IDENTIFIER); // This should always be true.

    // We need to check if the identifier for the procedure is already declared.
    if (source_parser_identifier_is_declared(parser, object.identifier))
    {
        
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_VARIABLE_REDECLARATION, ", function is already defined.", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDFUNCTION))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // The procedure wasn't declared, so we define it here. This goes in the parent
    // scope.
    //
    // NOTE(Chris): Do we want to limit the scope depth of function/procedure decs?
    //              We can probably do this grammatically rather than coding for an
    //              edge case.
    symbol *param_symbol = source_parser_insert_into_symbol_table(parser, object.identifier);
    param_symbol->type = SYMBOL_TYPE_FUNCTION;

    // Push the node.
    syntax_node *function_node = source_parser_push_node(parser);
    function_node->type = FUNCTION_STATEMENT_NODE;
    function_node->function.name = object.identifier;

    // We can push the scope here.
    source_parser_push_symbol_table(parser);

    // Push the return symbol onto the stack. When we do this, the variable must evaluate
    // to be defined by the end of the function call, otherwise it is an error.
    symbol *return_symbol = source_parser_insert_into_symbol_table(parser, object.identifier);
    return_symbol->type = SYMBOL_TYPE_UNDEFINED;

    // Process all identifiers until semicolon, these are the parameters. These
    // parameters are also shoved into the scope for validation.
    syntax_node *head_parameter_node = NULL;
    syntax_node *last_parameter_node = NULL;
    i32 arity_count = 0;
    while (!source_parser_match_token(parser, 1, TOKEN_SEMICOLON))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        
        // Each parameter must be an identifier.
        if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    PARSER_ERROR_EXPECTED_IDENTIFIER, ".", "");
            source_parser_should_propagate_error(NULL, parser, mem_state);

            if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDFUNCTION))
            {
                if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                    source_parser_consume_token(parser);
            }

            return NULL;
        }

        // Get the identifier.
        source_token identifier = source_parser_consume_token(parser);
        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser, &identifier, &object);
        assert(type == OBJECT_IDENTIFIER); // This should always be true.

        // Create the parameter node.
        syntax_node *param_node = source_parser_push_node(parser);
        param_node->type = PARAMETER_STATEMENT_NODE;
        param_node->parameter.name = object.identifier;
        param_node->parameter.next_parameter = NULL;

        symbol *param_symbol = source_parser_insert_into_symbol_table(parser, object.identifier);
        param_symbol->type = SYMBOL_TYPE_VARIABLE;

        if (head_parameter_node == NULL)
        {
            head_parameter_node = param_node;
            last_parameter_node = param_node;
        }

        else
        {
            last_parameter_node->parameter.next_parameter = param_node;
            last_parameter_node = param_node;
        }

        arity_count++;

    }

    // We should be at the semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDFUNCTION))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    // Set the parameter node in the procedure. This may be NULL, which is fine.
    function_node->function.parameters = head_parameter_node;
    param_symbol->arity = arity_count;

    // Finally, we process all body statements.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_KEYWORD_ENDFUNCTION))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        syntax_node *statement = source_parser_match_statement(parser);

        // The statement could be NULL, which we ignore and move on. Synchronization
        // happens inside statements.
        if (statement == NULL)
        {
            continue;
        }

        // First statement.
        if (head_statement_node == NULL)
        {
            head_statement_node = statement;
            last_statement_node = statement;
        }

        // All other statements.
        else
        {
            last_statement_node->next_node = statement;   
            last_statement_node = statement;
        }

    }

    // Expecting the ENDFUNCTION.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_ENDFUNCTION))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SYMBOL, ", expected 'endfunction' keyword.", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDFUNCTION);
        return NULL;
    }


    // Consume the ENDFUNCTION.
    source_parser_consume_token(parser);

    // Here's the kicker, we need to now make sure the function's name is considered
    // defined. We can easily check that.
    if (return_symbol->type != SYMBOL_TYPE_VARIABLE)
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_NO_RETURN, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    // Pop the scope once we're done.
    source_parser_pop_symbol_table(parser);
    function_node->function.body_statements = head_statement_node;

    // Now, at the end of all this jank, we expect the semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    return function_node;

}

syntax_node*
source_parser_match_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);
    syntax_node *result = NULL;

    // Variable statements.
    if (source_parser_expect_token(parser, TOKEN_KEYWORD_VARIABLE))
    {
        result = source_parser_match_variable_statement(parser);
    }

    // Scope statements.
    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_SCOPE))
    {
        result = source_parser_match_scope_statement(parser);
    }

    // While statements.
    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_WHILE))
    {
        result = source_parser_match_while_statement(parser);
    }

    // Loop statmeents.
    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_LOOP))
    {
        result = source_parser_match_loop_statement(parser);
    }

    // If statements.
    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_IF))
    {
        result = source_parser_match_if_statement(parser);
    }

    // Procedure statement.
    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_PROCEDURE))
    {
        result = source_parser_match_procedure_statement(parser);
    }
    
    // Function statement.
    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_FUNCTION))
    {
        result = source_parser_match_function_statement(parser);
    }

    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_WRITE))
    {
        result = source_parser_match_write_statement(parser);
    }

    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_READ))
    {
        result = source_parser_match_read_statement(parser);
    }

    // All other expression statements.
    else
    {
        result = source_parser_match_expression_statement(parser);
    }

    return result;

}

syntax_node*
source_parser_match_global_statement(source_parser *parser)
{

    syntax_node *result = NULL;

    // Procedure statement.
    if (source_parser_expect_token(parser, TOKEN_KEYWORD_PROCEDURE))
    {
        result = source_parser_match_procedure_statement(parser);
    }
    
    // Function statement.
    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_FUNCTION))
    {
        result = source_parser_match_function_statement(parser);
    }

    else
    {
        
        char hold_character;
        cc64 string = source_token_string_nullify(parser->tokenizer->current_token, &hold_character);
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_UNEXPECTED_GLOBAL_STATEMENT, ", '%s' encountered.", string);
        source_token_string_unnullify(parser->tokenizer->current_token, hold_character);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        parser->error_count++;
        return NULL;

    }

    return result;

}

syntax_node*
source_parser_match_include_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    syntax_node *head_include_node = NULL;
    syntax_node *last_include_node = NULL;
    while (source_parser_match_token(parser, 1, TOKEN_KEYWORD_INCLUDE))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        
        // Consume the include keyword.
        source_parser_consume_token(parser);

        // The following token should be a string.
        if (!source_parser_expect_token(parser, TOKEN_STRING))
        {
            parser->error_count++;
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    PARSER_ERROR_EXPECTED_SYMBOL, ", expected string in include statement.", "");
            continue;
        }

        // Get the path.
        source_token path = source_parser_consume_token(parser);
        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser, &path, &object);
        assert(type == OBJECT_STRING); // This should always be true.

        // After processing, we can now expect a semicolon.
        if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
        {

            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
            source_parser_should_propagate_error(NULL, parser, mem_state);
            source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
            return NULL;

        }

        source_parser_consume_token(parser);

        // We need to now process this.
        syntax_node *include_node = source_parser_push_node(parser);
        include_node->type = INCLUDE_STATEMENT_NODE;
        include_node->include.file_path = object.string;
        
        // First statement.
        if (head_include_node == NULL)
        {
            head_include_node = include_node;
            last_include_node = include_node;
        }

        // All other statements.
        else
        {
            last_include_node->next_node = include_node;   
            last_include_node = include_node;
        }



    }

    return head_include_node;

}

void 
source_parser_push_tokenizer(source_parser *parser, ccptr file_path, cptr source_buffer)
{

    // Push, set parent, and then set before we perform any initialization.
    parser_tokenizer *current_tokenizer = memory_arena_push_type_top(
            &parser->transient_arena, parser_tokenizer);
    current_tokenizer->parent_tokenizer = parser->tokenizer;
    parser->tokenizer = current_tokenizer;

    current_tokenizer->previous_token   = &current_tokenizer->tokens[0];
    current_tokenizer->current_token    = &current_tokenizer->tokens[1];
    current_tokenizer->next_token       = &current_tokenizer->tokens[2];
    source_tokenizer_initialize(&current_tokenizer->tokenizer, source_buffer, file_path);
    source_parser_consume_token(parser); // Prime the parser tokens.
    source_parser_consume_token(parser);

}

void 
source_parser_pop_tokenizer(source_parser *parser)
{

    parser->tokenizer = parser->tokenizer->parent_tokenizer;
    return;

}

syntax_node*
source_parser_match_module(ccptr file_name, source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);
    u64 top_state = memory_arena_save_top(&parser->transient_arena);

    // We need to load the file into memory.
    if (!fileio_file_exists(file_name)) 
    {
        parser->error_count++;
        printf("Resource error: Unable to open file: %s\n", file_name);
        return NULL;
    }

    u64 source_size = fileio_file_size(file_name);
    char *source_buffer = (char*)memory_arena_push_top(&parser->transient_arena, source_size + 1);
    fileio_file_read(file_name, source_buffer, source_size, source_size + 1);
    source_buffer[source_size] = '\0'; // Null-terminate.

    // Now we need to create the tokenizer for it.
    source_parser_push_tokenizer(parser, file_name, source_buffer);

    // Generate program syntax node.
    syntax_node *module_node = source_parser_push_node(parser);
    module_node->type = MODULE_ROOT_NODE;

    // Match all global statements.
    syntax_node *head_global_node = NULL;
    syntax_node *last_global_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_EOF))
    {

        syntax_node *global_statement = source_parser_match_global_statement(parser);

        // The statement could be NULL, which we ignore and move on. Synchronization
        // happens inside statements.
        if (global_statement == NULL)
        {
            continue;
        }

        // First statement.
        if (head_global_node == NULL)
        {
            head_global_node = global_statement;
            last_global_node = global_statement;
        }

        // All other statements.
        else
        {
            last_global_node->next_node = global_statement;   
            last_global_node = global_statement;
        }
        
        if (source_parser_should_break_on_eof(parser)) break;

    }

    module_node->module.global_statements = head_global_node;

    source_parser_pop_tokenizer(parser);
    memory_arena_restore_top(&parser->transient_arena, top_state);
    return module_node;

}

syntax_node*
source_parser_match_program(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Generate program syntax node.
    syntax_node *program_node = source_parser_push_node(parser);
    program_node->type = PROGRAM_ROOT_NODE;

    // Match all global statements.
    syntax_node *head_global_node = NULL;
    syntax_node *last_global_node = NULL;

    // Get all the first-layer depencies.
    syntax_node *dependencies = source_parser_match_include_statement(parser);
    if (dependencies != NULL)
    {
        syntax_node *current_dependency = dependencies;
        while (current_dependency != NULL)
        {
            printf("-- Dependency requested: %s\n", current_dependency->include.file_path);
            
            syntax_node *module_node = source_parser_match_module(
                    current_dependency->include.file_path, parser);
            if (module_node != NULL) 
            {

                // First statement.
                if (head_global_node == NULL)
                {
                    head_global_node = module_node;
                    last_global_node = module_node;
                }

                // All other statements.
                else
                {
                    last_global_node->next_node = module_node;   
                    last_global_node = module_node;
                }
            }

            current_dependency = current_dependency->next_node;
        }
    }

    // Now, we should expect the begin keyword here.
    while (!source_parser_match_token(parser, 1, TOKEN_KEYWORD_BEGIN))
    {

        syntax_node *global_statement = source_parser_match_global_statement(parser);
        if (source_parser_should_break_on_eof(parser)) break;

        // The statement could be NULL, which we ignore and move on. Synchronization
        // happens inside statements.
        if (global_statement == NULL)
        {
            continue;
        }

        // First statement.
        if (head_global_node == NULL)
        {
            head_global_node = global_statement;
            last_global_node = global_statement;
        }

        // All other statements.
        else
        {
            last_global_node->next_node = global_statement;   
            last_global_node = global_statement;
        }

    }

    program_node->program.global_statements = head_global_node;

    // Match the begin keyword with trailing semicolon.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_BEGIN))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SYMBOL, ", expected 'begin' keyword.", "");
        return NULL;
    }

    else
    {
        source_parser_consume_token(parser);

        if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
            return NULL;
        }

        source_parser_consume_token(parser);

    }

    // Push the scope.
    source_parser_push_symbol_table(parser);

    // Match all expression statements and create a statement chain.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_KEYWORD_END))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        syntax_node *statement = source_parser_match_statement(parser);

        // The statement could be NULL, which we ignore and move on. Synchronization
        // happens inside statements.
        if (statement == NULL)
        {
            continue;
        }

        // First statement.
        if (head_statement_node == NULL)
        {
            head_statement_node = statement;
            last_statement_node = statement;
        }

        // All other statements.
        else
        {
            last_statement_node->next_node = statement;   
            last_statement_node = statement;
        }

    }

    // Set the program node's next node as the head statement.
    program_node->program.body_statements = head_statement_node;
    
    // Pop the scope.
    source_parser_pop_symbol_table(parser);

    // Match the end keyword with trailing semicolon.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_END))
    {
        parser->error_count++; 
        display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                PARSER_ERROR_EXPECTED_SYMBOL, ", expected 'end' keyword.", "");
        return NULL;
    }
    else
    {

        source_parser_consume_token(parser);

        if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
        {
            parser->error_count++; 
            display_error_message(parser->tokenizer->tokenizer.file_path, parser->tokenizer->current_token,
                    PARSER_ERROR_EXPECTED_SEMICOLON, ".", "");
            return NULL;
        }

        source_parser_consume_token(parser);

    }

    return program_node;

}

syntax_node*    
source_parser_create_ast(source_parser *parser, cc64 path, memory_arena *arena)
{

    assert(parser != NULL);

    // Cache our memory arena state in the event we need to unwind on error.
    u64 mem_cache = memory_arena_save(arena);

    // Initialize.
    parser->entry           = NULL;
    parser->nodes           = NULL;
    parser->arena           = arena;
    parser->error_count     = 0;
    parser->tokenizer       = NULL;

    // Program Memory Layout:
    // [      ][                               ]
    // ^        ^-- String Pool               ^
    // |          & Symbol Tables             |
    // |                                      |
    // |                 Files & Tokenizer -- |
    // | -- AST nodes
    //
    // Take special note of what is where:
    //
    // 1.   The AST Nodes are placed at the head where they can be freely gen'd.
    //
    // 2.   The string pool and symbol tables are offsetted, placed where they
    //      grow "up" from their location. These frequently update and therefore
    //      will cause page faulting interrupts. OS expects faults to occur "upward"
    //      and therefore more efficient than putting them at the end.
    //
    // 3.   Files and tokenizer are placed at the end and grow down. They don't change
    //      as frequently as the others and therefore can take a hit in growing down.
    //      Eventually, we will do away with this and use a resource manager instead.

    // Set the two arenas, the syntax tree arena is a fixed size of 64 megabytes,
    // which equates to (if each syntax node is 48 bytes) to 1.4 million nodes.
    // The transient arena takes the remaining area of the arena and is used for
    // everything else.
    memory_arena_partition(arena, &parser->syntax_tree_arena, SF_MEGABYTES(64));
    u64 primary_arena_remainder_size = memory_arena_free_size(arena);
    memory_arena_partition(arena, &parser->transient_arena, primary_arena_remainder_size);

    // Pull the file source file into memory.
    u64 source_size = fileio_file_size(path);
    char *source_buffer = (char*)memory_arena_push_top(&parser->transient_arena, source_size + 1);
    fileio_file_read(path, source_buffer, source_size, source_size + 1);
    source_buffer[source_size] = '\0'; // Null-terminate.
                                    
    // We look back at my absurdity and praise that it panned out as well 
    // as it has up to this point.
    source_parser_push_tokenizer(parser, path, source_buffer);

    // Reserve the string pool and the bottom of the transient arena.
    string_pool_initialize(&parser->spool, &parser->transient_arena, STRING_POOL_DEFAULT_SIZE);

    // Allocate the global symbol table at the bottom of symbol table.
    parser->symbol_table = memory_arena_push_type(&parser->transient_arena, symbol_table);
    symbol_table_initialize(parser->symbol_table, &parser->transient_arena, 1024);

    // Generate AST from program node.
    syntax_node *program = source_parser_match_program(parser);
    parser->entry = program;
    parser->nodes = program;

    // If there we errors, return NULL and indicate failure to parse.
    if (parser->error_count > 0)
    {
        printf("\n");
        printf("-- Failed to parse, no output has been generated.\n");
        memory_arena_restore(arena, mem_cache);
        return NULL;
    }

    // Pop the transient arena, since we don't need it anymore.
    memory_arena_pop(arena, parser->transient_arena.size);

    return program;

}

// --- Helpers -----------------------------------------------------------------
//
// The following functions are helper functions for the parser.
//

syntax_node*
source_parser_push_node(source_parser *parser)
{
    
    syntax_node *allocation = memory_arena_push_type(&parser->syntax_tree_arena, syntax_node);
    allocation->type        = NULL_EXPRESSION_NODE;
    allocation->next_node   = NULL;

    return allocation;

}

source_token
source_parser_get_previous_token(source_parser *parser)
{
    source_token *result = parser->tokenizer->previous_token;
    return *result;
}

source_token
source_parser_get_current_token(source_parser *parser)
{
    source_token *result = parser->tokenizer->current_token;
    return *result;
}

source_token
source_parser_get_next_token(source_parser *parser)
{
    source_token *result = parser->tokenizer->next_token;
    return *result;
}

source_token
source_parser_consume_token(source_parser *parser)
{
    source_token *temporary = parser->tokenizer->previous_token;
    parser->tokenizer->previous_token = parser->tokenizer->current_token;
    parser->tokenizer->current_token = parser->tokenizer->next_token;
    parser->tokenizer->next_token = temporary;
    source_tokenizer_get_next_token(&parser->tokenizer->tokenizer, parser->tokenizer->next_token);

    // NOTE(Chris): Due to the fact that expression and statement grammar does
    //              not take into account that new lines and comments can coexist
    //              in them, we ignore them here for now.

    while (parser->tokenizer->next_token->type == TOKEN_NEW_LINE ||
           parser->tokenizer->next_token->type == TOKEN_COMMENT_BLOCK)
    {
        source_tokenizer_get_next_token(&parser->tokenizer->tokenizer, parser->tokenizer->next_token);
    }
    
    return *parser->tokenizer->previous_token;
}

b32 
source_parser_should_break_on_eof(source_parser *parser)
{
    if (source_parser_match_token(parser, 4,
            TOKEN_EOF, 
            TOKEN_UNDEFINED,
            TOKEN_UNDEFINED_EOF,
            TOKEN_UNDEFINED_EOL))
    {
        return true;
    }

    return false;
}

b32 
source_parser_expect_token(source_parser *parser, source_token_type type)
{
    
    b32 is_type = (parser->tokenizer->current_token->type == type);
    return is_type;

}

b32 
source_parser_next_token_is(source_parser *parser, source_token_type type)
{

    b32 is_type = (parser->tokenizer->next_token->type == type);
    return is_type;

}

b32 
source_parser_match_token(source_parser *parser, u32 count, ...)
{

    va_list args;
    va_start(args, count);
    b32 matched = false;

    source_token_type current = parser->tokenizer->current_token->type;
    for (u32 idx = 0; idx < count; ++idx)
    {
        source_token_type type = va_arg(args, source_token_type);
        if (type == current)
        {
            matched = true;
            break;
        }
    }

    va_end(args);

    return matched;

}

object_type
source_parser_token_to_literal(source_parser *parser, source_token *token, object_literal *object)
{
    
    source_token_type type = token->type;

    switch(type)
    {

        case TOKEN_REAL:
        {

            char hold;
            cc64 token_string = source_token_string_nullify(token, &hold);
            cc64 pool_string = source_parser_insert_into_string_pool(parser, token_string);
            object->identifier = pool_string;

            double result = atof(token_string);
            object->real = result;

            source_token_string_unnullify(token, hold);

            return OBJECT_REAL;

        } break;

        case TOKEN_INTEGER:
        {

            char hold;
            cc64 token_string = source_token_string_nullify(token, &hold);
            cc64 pool_string = source_parser_insert_into_string_pool(parser, token_string);
            object->identifier = pool_string;

            i64 result = atoll(token_string);
            object->signed_integer = result;

            source_token_string_unnullify(token, hold);
            return OBJECT_SIGNED_INTEGER;
            
        } break;

        case TOKEN_IDENTIFIER:
        {

            char hold;
            cc64 token_string = source_token_string_nullify(token, &hold);

            cc64 pool_string = source_parser_insert_into_string_pool(parser, token_string);
            object->string = pool_string;

            source_token_string_unnullify(token, hold);
            return OBJECT_IDENTIFIER;

        } break;

        case TOKEN_STRING:
        {
            
            char hold;
            cc64 token_string = source_token_string_nullify(token, &hold);

            cc64 pool_string = source_parser_insert_into_string_pool(parser, token_string);
            object->string = pool_string;

            source_token_string_unnullify(token, hold);
            return OBJECT_STRING;

        } break;

    };

    assert(!"Unreachable condition, not all types handled.");
    return 0; // So the compiler doesn't complain.

}

cc64 
source_parser_insert_into_string_pool(source_parser *parser, cc64 string)
{
 
    sh64 pool_entry = string_pool_insert(&parser->spool, string);
    cc64 result = string_pool_string_from_handle(pool_entry);
    return result;

}

syntax_operation_type 
source_parser_token_to_operation(source_token *token)
{

    source_token_type type = token->type;

    switch (type)
    {
        case TOKEN_PLUS:                return OPERATION_ADDITION;
        case TOKEN_MINUS:               return OPERATION_SUBTRACTION;
        case TOKEN_STAR:                return OPERATION_MULTIPLICATION;
        case TOKEN_FORWARD_SLASH:       return OPERATION_DIVISION;
        case TOKEN_EQUALS:              return OPERATION_EQUALS;
        case TOKEN_HASH:                return OPERATION_NOT_EQUALS;
        case TOKEN_LESS_THAN:           return OPERATION_LESS_THAN;
        case TOKEN_LESS_THAN_EQUALS:    return OPERATION_LESS_THAN_EQUALS;
        case TOKEN_GREATER_THAN:        return OPERATION_GREATER_THAN;
        case TOKEN_GREATER_THAN_EQUALS: return OPERATION_GREATER_THAN_EQUALS;
    };

    assert(!"Unreachable condition, not all types handled.");
    return 0; // So the compiler doesn't complain.

}

b32 
source_parser_should_propagate_error(void *check, source_parser *parser, u64 state)
{

    if (check == NULL)
    {
        memory_arena_restore(&parser->syntax_tree_arena, state);
        return true;
    }

    return false;

}

b32
source_parser_synchronize_to(source_parser *parser, source_token_type type)
{

    while (!source_parser_match_token(parser, 2, type, TOKEN_EOF))
    {
        source_parser_consume_token(parser);
    }

    if (source_parser_expect_token(parser, type))
    {
        source_parser_consume_token(parser);
        return true;
    }

    else
    {
        return false;
    }

}

void 
source_parser_push_symbol_table(source_parser *parser)
{

    assert(parser != NULL);
    assert(parser->symbol_table != NULL);

    symbol_table *parent = parser->symbol_table;
    symbol_table *table = memory_arena_push_type(&parser->transient_arena, symbol_table);
    symbol_table_initialize(table, &parser->transient_arena, 1024);

    table->parent = parent;
    parser->symbol_table = table;

}

void 
source_parser_pop_symbol_table(source_parser *parser)
{

    assert(parser != NULL);
    assert(parser->symbol_table != NULL);
    assert(parser->symbol_table->parent != NULL);

    symbol_table *parent_table = parser->symbol_table->parent;
    symbol_table_collapse_arena(parser->symbol_table);
    memory_arena_pop_type(&parser->transient_arena, symbol_table);

    parser->symbol_table = parent_table;

}

symbol*
source_parser_insert_into_symbol_table(source_parser *parser, cc64 identifier)
{

    assert(parser != NULL);
    assert(parser->symbol_table != NULL);
    symbol *result = symbol_table_insert(parser->symbol_table, identifier, SYMBOL_TYPE_UNDEFINED);

    r64 current_load_factor = symbol_table_load_factor(parser->symbol_table);
    if (current_load_factor >= 0.66)
    {
        printf("-- Current symbol table reached load factor limit of 0.66, resizing.\n");
        symbol_table_resize(parser->symbol_table);
    }

    return result;


}

b32 
source_parser_identifier_is_declared(source_parser *parser, cc64 identifier)
{

    symbol *result = symbol_table_search_from_any_table(parser->symbol_table, identifier);

    if (result == NULL)
    {
        return false;
    }

    else
    {
        return true;
    }

}

b32 
source_parser_identifier_is_declared_in_scope(source_parser *parser, cc64 identifier)
{

    symbol *result = symbol_table_search_from_current_table(parser->symbol_table, identifier);

    if (result == NULL)
    {
        return false;
    }

    else
    {
        return true;
    }

}

b32 
source_parser_identifier_is_declared_above_scope(source_parser *parser, cc64 identifier)
{

    if (parser->symbol_table->parent == NULL) return false;
    symbol *result = symbol_table_search_from_any_table(parser->symbol_table->parent, identifier);

    if (result == NULL)
    {
        return false;
    }

    else
    {
        return true;
    }

}


b32 
source_parser_identifier_is_defined(source_parser *parser, cc64 identifier)
{

    symbol *result = symbol_table_search_from_any_table(parser->symbol_table, identifier);

    if (result != NULL && result->type != SYMBOL_TYPE_UNDEFINED)
    {
        return true;
    }

    else
    {
        return false;
    }

}

symbol* 
source_parser_locate_symbol(source_parser *parser, cc64 identifier)
{

    symbol *result = symbol_table_search_from_any_table(parser->symbol_table, identifier);
    return result;

}

// --- Print Traversal ---------------------------------------------------------
//
// The following print traversal is designed for viewing the raw output of the
// tree's interpretation. It's mainly used for debugging and not meant to be
// used as production code.
//

void
parser_print_tree(syntax_node *root_node)
{

    switch(root_node->type)
    {

        case PROGRAM_ROOT_NODE:
        {
            
            printf("begin main;\n");
            syntax_node *current_node = root_node->program.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("end main;\n");

        } break;

        case WRITE_STATEMENT_NODE:
        {

            printf("print ");
            parser_print_tree(root_node->write.location);
            printf(" ");

            syntax_node *current_node = root_node->write.body_expressions;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                current_node = current_node->next_node;
                if (current_node != NULL)
                    printf(" ");
            }


        } break;

        case READ_STATEMENT_NODE:
        {

            printf("read ");
            parser_print_tree(root_node->read.location);
            printf(" %s", root_node->read.identifier);

        } break;

        case SCOPE_STATEMENT_NODE:
        {
            
            printf("{\n");
            syntax_node *current_node = root_node->scope.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");

        } break;

        case PROCEDURE_CALL_EXPRESSION_NODE:
        {

            printf("%s(", root_node->proc_call.identifier);
            syntax_node *params = root_node->proc_call.parameters;
            while(params != NULL)
            {
                parser_print_tree(params);
                if (params->next_node != NULL) printf(", ");
                params = params->next_node;
            }
            printf(")");

        } break;

        case FUNCTION_CALL_EXPRESSION_NODE:
        {

            printf("f:%s(", root_node->proc_call.identifier);
            syntax_node *params = root_node->proc_call.parameters;
            while(params != NULL)
            {
                parser_print_tree(params);
                if (params->next_node != NULL) printf(", ");
                params = params->next_node;
            }
            printf(")");

        } break;

        case PARAMETER_STATEMENT_NODE:
        {
            printf("%s", root_node->parameter.name);
        } break;

        case FUNCTION_STATEMENT_NODE:
        {

            printf("function %s ( ", root_node->function.name);
            syntax_node *params = root_node->function.parameters;
            while (params != NULL)
            {
                parser_print_tree(params);
                if (params->parameter.next_parameter != NULL) printf(", ");
                params = params->parameter.next_parameter;
            }

            printf(" )\n");
            printf("{\n");
            syntax_node *current_node = root_node->procedure.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");

        } break;

        case PROCEDURE_STATEMENT_NODE:
        {

            printf("procedure %s ( ", root_node->procedure.name);
            syntax_node *params = root_node->procedure.parameters;
            while (params != NULL)
            {
                parser_print_tree(params);
                if (params->parameter.next_parameter != NULL) printf(", ");
                params = params->parameter.next_parameter;
            }

            printf(" )\n");
            printf("{\n");
            syntax_node *current_node = root_node->procedure.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");

        } break;

        case ELSEIF_STATEMENT_NODE:
        {
            printf("elseif: ");
            parser_print_tree(root_node->elseif_conditional.evaluation_expression);
            printf("\n");
            printf("{\n");
            syntax_node *current_node = root_node->elseif_conditional.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");
        } break;

        case IF_STATEMENT_NODE:
        {

            printf("if: ");
            parser_print_tree(root_node->if_conditional.evaluation_expression);
            printf("\n");
            printf("{\n");
            syntax_node *current_node = root_node->if_conditional.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");

            syntax_node *else_node = root_node->if_conditional.else_statement;
            while (else_node != NULL)
            {
                parser_print_tree(else_node);
                else_node = else_node->elseif_conditional.else_statement;
            }

        } break;

        case WHILE_STATEMENT_NODE:
        {
            
            printf("while: ");
            parser_print_tree(root_node->while_loop.evaluation_expression);
            printf("\n");
            printf("{\n");
            syntax_node *current_node = root_node->while_loop.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");

        } break;

        case LOOP_STATEMENT_NODE:
        {
            printf("for %s = ", root_node->for_loop.iterator_identifier);
            parser_print_tree(root_node->for_loop.initial_value_expression);
            printf("; %s < ", root_node->for_loop.iterator_identifier);
            parser_print_tree(root_node->for_loop.terminate_value_expression);
            printf("; %s += ", root_node->for_loop.iterator_identifier);
            if (root_node->for_loop.step_value_expression == NULL)
                printf("1\n");
            else
            {
                parser_print_tree(root_node->for_loop.step_value_expression);
                printf("\n");
            }

            printf("{\n");
            syntax_node *current_node = root_node->for_loop.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");

        } break;

        case VARIABLE_STATEMENT_NODE:
        {
            
            printf("var %s ", root_node->variable.name);
            parser_print_tree(root_node->variable.size);

            syntax_node *current_dim = root_node->variable.dimensions;
            while (current_dim != NULL)
            {
                if (current_dim != NULL) printf(" ");
                printf("[ ");
                parser_print_tree(current_dim);
                printf(" ]");
                current_dim = current_dim->next_node;
            }
            
            if (root_node->variable.assignment != NULL)
            {
                printf(" = ");
                parser_print_tree(root_node->variable.assignment);
            }


        } break;

        case ASSIGNMENT_EXPRESSION_NODE:
        {
            
            parser_print_tree(root_node->assignment.left);
            printf(" = ");
            parser_print_tree(root_node->assignment.right);

        } break;

        case BINARY_EXPRESSION_NODE:
        {

            parser_print_tree(root_node->binary.left);

            switch (root_node->binary.type)
            {

                case OPERATION_ADDITION: printf(" + "); break;
                case OPERATION_SUBTRACTION: printf(" - "); break;
                case OPERATION_MULTIPLICATION: printf(" * "); break;
                case OPERATION_DIVISION: printf(" / "); break;
                case OPERATION_EQUALS: printf(" == "); break;
                case OPERATION_NOT_EQUALS: printf(" != "); break;
                case OPERATION_LESS_THAN: printf(" < "); break;
                case OPERATION_LESS_THAN_EQUALS: printf(" <= "); break;
                case OPERATION_GREATER_THAN: printf(" > "); break;
                case OPERATION_GREATER_THAN_EQUALS: printf(" >= "); break;
                case OPERATION_ASSIGNMENT: printf(" = "); break;
                
                default:
                {

                    assert(!"Unimplemented operation for binary expression printing.");
                    return;

                } break;
            }

            parser_print_tree(root_node->binary.right);

        } break;

        case UNARY_EXPRESSION_NODE:
        {

            switch (root_node->unary.type)
            {

                case OPERATION_NEGATIVE_ASSOCIATE: printf("-"); break;

                default:
                {

                    assert(!"Unimplemented operation for unary expression printing.");
                    return;

                } break;

            };

            parser_print_tree(root_node->unary.right);

        } break;

        case GROUPING_EXPRESSION_NODE:
        {

            printf("( ");
            parser_print_tree(root_node->grouping.grouping);
            printf(" )");

        } break;

        case PRIMARY_EXPRESSION_NODE:
        {

            switch (root_node->primary.type)
            {

                case OBJECT_UNSIGNED_INTEGER: 
                {
                    printf("%llu", root_node->primary.literal.unsigned_integer);
                } break;

                case OBJECT_SIGNED_INTEGER: 
                {
                    printf("%lld", root_node->primary.literal.signed_integer);
                } break;

                case OBJECT_REAL: 
                {
                    printf("%f", root_node->primary.literal.real);
                } break;

                case OBJECT_BOOLEAN: 
                {
                    printf("%lld", root_node->primary.literal.boolean);
                } break;

                case OBJECT_STRING: 
                {
                    printf("\"%s\"", root_node->primary.literal.string);
                } break;

                case OBJECT_IDENTIFIER: 
                {
                    printf("%s", root_node->primary.literal.identifier);
                } break;
                
                default:
                {

                    assert(!"Unimplemented case for primary expression printing.");
                    return;

                } break;

            }

        } break;

        default:
        {

            assert(!"Unimplemented parser print case.");
            return;

        };

    };

}
*/
