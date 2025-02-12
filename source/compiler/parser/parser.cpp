#include <platform/filesystem.hpp>
#include <compiler/parser/parser.hpp>
#include <compiler/parser/statements.hpp>
#include <compiler/parser/expressions.hpp>
#include <compiler/parser/errorhandler.hpp>

ParseTree::
ParseTree(DependencyGraph* graph, Environment* environment)
{

    this->graph         = graph;
    this->environment   = environment;
    this->root          = nullptr;
    this->tokenizer     = nullptr;

}

ParseTree::
~ParseTree()
{

}

bool ParseTree::
parse(string source_file)
{

    if (!file_exists(source_file.c_str()))
        return false;

    this->path = source_file;

    this->tokenizer = make_shared<Tokenizer>(source_file);
    shared_ptr<SyntaxNode> root = this->match_root();
    if (root == nullptr)
        return false;

    this->root = root;
    return true;

}

bool ParseTree::
valid() const
{

    return this->root != nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
get_root() const
{

    return this->root;

}

vector<shared_ptr<SyntaxNode>> ParseTree::
get_nodes() const
{

    return this->nodes;

}

// --- Helper Methods ----------------------------------------------------------

void ParseTree::
synchronize_to(TokenType type)
{

    while (!this->tokenizer->current_token_is(type))
    {

        if (this->tokenizer->current_token_is(TokenType::TOKEN_EOF))
            return;

        this->tokenizer->shift();

    }

    this->tokenizer->shift();

}

void ParseTree::
synchronize_up_to(TokenType type)
{

    while (!this->tokenizer->current_token_is(type))
    {

        if (this->tokenizer->current_token_is(TokenType::TOKEN_EOF))
            return;

        this->tokenizer->shift();

    }

}

bool ParseTree::
expect_current_token_as(TokenType type) const
{

    return this->tokenizer->current_token_is(type);

}

bool ParseTree::
expect_next_token_as(TokenType type) const
{

    return this->tokenizer->next_token_is(type);

}

void ParseTree::
consume_current_token_as(TokenType type, u64 sloc)
{

    if (this->expect_current_token_as(type))
    {
        this->tokenizer->shift();
    }
    else
    {
        throw new SyntaxError(sloc, this->path, this->tokenizer->get_current_token(),
            "Unexpected token encountered. Expected: '%s'.",
            Token::type_to_string(type).c_str());
    }

}

template <class T, typename ...Params> shared_ptr<T> ParseTree::
generate_node(Params... args)
{

    shared_ptr<T> node = make_shared<T>(args...);
    this->nodes.push_back(node);
    return node;

}

// --- Descent Methods ---------------------------------------------------------

shared_ptr<SyntaxNode> ParseTree::
match_root()
{

    vector<shared_ptr<SyntaxNode>> children;
    while (!this->tokenizer->current_token_is(TokenType::TOKEN_EOF))
    {
    
        try
        {

            auto global_node = this->match_global_statement();
            children.push_back(global_node);

        }
        catch (SyntaxException& e)
        {

            if (e.handled == false)
            {
                std::cout << e.what() << std::endl;
                e.handled = true;
                this->synchronize_to(TokenType::TOKEN_SEMICOLON);
            }

        }

    }

    auto root_node = this->generate_node<SyntaxNodeRoot>();
    root_node->children = children;
    return root_node;

}

shared_ptr<SyntaxNode> ParseTree::
match_global_statement()
{

    Token current_token = this->tokenizer->get_current_token();
    switch (current_token.type)
    {

        case TokenType::TOKEN_KEYWORD_INCLUDE:
        {
            this->match_include_statement();
        } break;

        case TokenType::TOKEN_KEYWORD_PROCEDURE:
        {
            return this->match_procedure_statement(true);
        } break;

        case TokenType::TOKEN_KEYWORD_FUNCTION:
        {
            return this->match_function_statement(true);
        } break;

        case TokenType::TOKEN_KEYWORD_BEGIN:
        {
            return this->match_begin_statement();
        } break;

    }

    throw new SyntaxError(__LINE__, this->path, current_token,
        "Unexpected symbol encountered '%s'.", current_token.reference.c_str());

}

shared_ptr<SyntaxNode> ParseTree::
match_include_statement()
{

    try
    {

        this->consume_current_token_as(TokenType::TOKEN_KEYWORD_INCLUDE, __LINE__);

        Token string_token = this->tokenizer->get_current_token();
        this->consume_current_token_as(TokenType::TOKEN_STRING, __LINE__);
        this->consume_current_token_as(TokenType::TOKEN_SEMICOLON, __LINE__);

        string file_include_path;
        file_include_path = string_token.reference;

        // Generate the filepath.
        Filepath include_path = this->path.root_directory();
        include_path += "./";
        include_path += file_include_path;
        include_path.canonicalize();

        // Check if the file exists.
        if (!file_exists(include_path.c_str()))
        {
            throw new SyntaxError(__LINE__, this->path, string_token,
                "Include file '%s' does not exist.",
                include_path.c_str());
        }

        // Check if the dependency is already added.
        bool is_already_included = this->graph->has_dependency(this->path.c_str(), include_path.c_str());

        // Before we generate the node, we need to add it to the dependency graph.
        DependencyResult result = this->graph->add_dependency(this->path.c_str(), include_path.c_str());
        if (result == DependencyResult::DEPENDENCY_IS_CIRCULAR)
        {
            throw new SyntaxError(__LINE__, this->path, string_token,
                "Cyclical dependency encountered for %s.",
                include_path.c_str());
        }
        else if (result == DependencyResult::DEPENDENCY_ALREADY_INCLUDED)
        {
            throw new SyntaxWarning(__LINE__, this->path, string_token,
                "File %s has already been included.",
                include_path.c_str());
        }
        else if (result == DependencyResult::DEPENDENCY_PARENT_NOT_FOUND)
        {
            throw new SyntaxError(__LINE__, this->path, string_token,
                "Parent file %s not found.",
                include_path.c_str());
        }
        else if (result == DependencyResult::DEPENDENCY_SELF_INCLUDED)
        {
            throw new SyntaxError(__LINE__, this->path, string_token,
                "File %s cannot include itself.",
                include_path.c_str());
        }

        // NOTE(Chris): If the file is already included, we don't need to parse it again.
        //              While what we see here isn't a lot of code, a lot does happen here.
        //              After the include file is validated, and we check if it is unique,
        //              (first time it is included), we need to parse that file as its own
        //              parse tree. The list of nodes that are returned must be copied into
        //              the parent's vector of nodes.
        //
        //              When we traverse the parse tree, we can expect to find the module
        //              node to be null for all nodes except the first include statement.
        //              This means that there is no way for the module node to be traversed
        //              more than once.

        shared_ptr<SyntaxNode> module_node = nullptr;
        if (!is_already_included)
        {

            ParseTree include_parser(this->graph, this->environment);
            if (!include_parser.parse(include_path.c_str()))
            {
                throw new SyntaxError(__LINE__, this->path, string_token,
                    "Failed to parse include file %s.",
                    include_path.c_str());
            }

            // Get the module node and set it to the include.
            module_node = include_parser.get_root();

            // Append the nodes to the parent tree.
            vector<shared_ptr<SyntaxNode>> nodes = include_parser.get_nodes();
            for (auto node : nodes)
                this->nodes.push_back(node);

        }

        // Generate the include node.
        auto include_node = this->generate_node<SyntaxNodeIncludeStatement>();
        include_node->absolute_path = include_path.c_str();
        include_node->module = module_node;
        return include_node;

    }
    catch (SyntaxException& e)
    {
        throw e;
    }

}

shared_ptr<SyntaxNode> ParseTree::
match_function_statement(bool is_global)
{

    try
    {

        auto function_node = this->generate_node<SyntaxNodeFunctionStatement>();

        this->consume_current_token_as(TokenType::TOKEN_KEYWORD_FUNCTION, __LINE__);

        Token identifier_token = this->tokenizer->get_current_token();
        this->consume_current_token_as(TokenType::TOKEN_IDENTIFIER, __LINE__);

        // Check if the symbol is already defined.
        if (this->environment->symbol_exists_locally(identifier_token.reference))
        {
            throw new SyntaxError(__LINE__, this->path, identifier_token,
                "Function declaration '%s' is already defined.",
                identifier_token.reference.c_str());
        }

        // Collect the parameters.
        vector<shared_ptr<SyntaxNode>> parameters;
        while (!this->expect_current_token_as(TokenType::TOKEN_SEMICOLON))
        {

            Token parameter_token = this->tokenizer->get_current_token();
            this->consume_current_token_as(TokenType::TOKEN_IDENTIFIER, __LINE__);

            auto parameter_node = this->generate_node<SyntaxNodeParameter>();
            parameter_node->identifier = parameter_token.reference;
            parameters.push_back(parameter_node);

        }

        // Consume the semicolon.
        this->consume_current_token_as(TokenType::TOKEN_SEMICOLON, __LINE__);

        // Push the scope.
        this->environment->push_table();

        // Insert the name of the function into the symbol table.
        Symbol function_symbol(identifier_token.reference,
            Symboltype::SYMBOL_TYPE_DECLARED,
            Datatype::DATA_TYPE_UNKNOWN,
            function_node);

        this->environment->set_symbol_locally(identifier_token.reference, function_symbol);

        // Add the parameters to the symbol table.
        for (auto parameter : parameters)
        {

            auto parameter_node = dynamic_pointer_cast<SyntaxNodeParameter>(parameter);
            SF_ENSURE_PTR(parameter_node);

            Symbol parameter_symbol(parameter_node->identifier, 
                Symboltype::SYMBOL_TYPE_VARIABLE, 
                Datatype::DATA_TYPE_UNKNOWN,
                parameter_node);

            this->environment->set_symbol_locally(parameter_node->identifier, parameter_symbol);

        }

        // Match all the body statements.
        vector<shared_ptr<SyntaxNode>> children;
        while (!this->expect_current_token_as(TokenType::TOKEN_EOF))
        {

            if (this->expect_current_token_as(TokenType::TOKEN_KEYWORD_ENDFUNCTION)) break;

            try
            {

                auto local_node = this->match_local_statement();
                children.push_back(local_node);

            }
            catch (SyntaxException& e)
            {

                // The match_local_statement function will catch and recover most errors.
                // If the error is not handled, throw it again and let the caller resynchronize.
                if (e.handled == false) throw e;

            }

        }

        Symbol *function_return_symbol = this->environment->get_symbol_locally(identifier_token.reference);
        if (function_return_symbol->get_type() == Symboltype::SYMBOL_TYPE_DECLARED)
        {
            throw new SyntaxError(__LINE__, this->path, identifier_token,
                "Function '%s' does not have a return value.",
                identifier_token.reference.c_str());
        }

        // Pop the scope.
        this->environment->pop_table();

        // Check for the end function keyword.
        this->consume_current_token_as(TokenType::TOKEN_KEYWORD_ENDFUNCTION, __LINE__);

        // Semicolon.
        this->consume_current_token_as(TokenType::TOKEN_SEMICOLON, __LINE__);

        // Insert the function into the symbol table.
        Symbol function_declaration_symbol = Symbol(identifier_token.reference,
            Symboltype::SYMBOL_TYPE_FUNCTION,
            Datatype::DATA_TYPE_UNKNOWN,
            function_node, parameters.size());

        this->environment->set_symbol_locally(identifier_token.reference, function_declaration_symbol);

        // Fill out the function node.
        function_node->children     = children;
        function_node->parameters   = parameters;
        function_node->identifier   = identifier_token.reference;
        return function_node;

    }
    catch (SyntaxException& e)
    {
        throw e;
    }

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_procedure_statement(bool is_global)
{

    try
    {

        auto procedure_node = this->generate_node<SyntaxNodeProcedureStatement>();
        this->consume_current_token_as(TokenType::TOKEN_KEYWORD_PROCEDURE, __LINE__);

        Token identifier_token = this->tokenizer->get_current_token();
        this->consume_current_token_as(TokenType::TOKEN_IDENTIFIER, __LINE__);

        // Check if the symbol is already defined.
        if (this->environment->symbol_exists_locally(identifier_token.reference))
        {
            throw new SyntaxError(__LINE__, this->path, identifier_token,
                "Procedure declaration '%s' is already defined.",
                identifier_token.reference.c_str());
        }

        // Collect the parameters.
        vector<shared_ptr<SyntaxNode>> parameters;
        while (!this->expect_current_token_as(TokenType::TOKEN_SEMICOLON))
        {

            Token parameter_token = this->tokenizer->get_current_token();
            this->consume_current_token_as(TokenType::TOKEN_IDENTIFIER, __LINE__);

            auto parameter_node = this->generate_node<SyntaxNodeParameter>();
            parameter_node->identifier = parameter_token.reference;
            parameters.push_back(parameter_node);

        }

        // Consume the semicolon.
        this->consume_current_token_as(TokenType::TOKEN_SEMICOLON, __LINE__);

        // Push the scope.
        this->environment->push_table();

        // Add the parameters to the symbol table.
        for (auto parameter : parameters)
        {

            auto parameter_node = dynamic_pointer_cast<SyntaxNodeParameter>(parameter);
            SF_ENSURE_PTR(parameter_node);

            Symbol parameter_symbol(parameter_node->identifier, 
                Symboltype::SYMBOL_TYPE_VARIABLE, 
                Datatype::DATA_TYPE_UNKNOWN,
                parameter_node);

            this->environment->set_symbol_locally(parameter_node->identifier, parameter_symbol);

        }

        // Match all the body statements.
        vector<shared_ptr<SyntaxNode>> children;
        while (!this->expect_current_token_as(TokenType::TOKEN_EOF))
        {

            if (this->expect_current_token_as(TokenType::TOKEN_KEYWORD_ENDPROCEDURE)) break;

            try
            {

                auto local_node = this->match_local_statement();
                children.push_back(local_node);

            }
            catch (SyntaxException& e)
            {

                // The match_local_statement function will catch and recover most errors.
                // If the error is not handled, throw it again and let the caller resynchronize.
                if (e.handled == false) throw e;

            }

        }

        // Pop the scope.
        this->environment->pop_table();

        // Check for end procedure keyword.
        this->consume_current_token_as(TokenType::TOKEN_KEYWORD_ENDPROCEDURE, __LINE__);

        // Semicolon.
        this->consume_current_token_as(TokenType::TOKEN_SEMICOLON, __LINE__);

        // NOTE(Chris): We assume that the datatype for this is void because procedures
        //              don't return anything.
        Symbol procedure_declaration_symbol = Symbol(identifier_token.reference,
            Symboltype::SYMBOL_TYPE_PROCEDURE,
            Datatype::DATA_TYPE_VOID,
            procedure_node, parameters.size());

        // Set the symbol.
        this->environment->set_symbol_locally(identifier_token.reference, procedure_declaration_symbol);

        // Fill out the procedure node.
        procedure_node->children    = children;
        procedure_node->parameters  = parameters;
        procedure_node->identifier  = identifier_token.reference;
        return procedure_node;

    }
    catch (SyntaxException& e)
    {
        throw e;
    }

    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_begin_statement()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_local_statement()
{

    Token current_token = this->tokenizer->get_current_token();
    switch (current_token.type)
    {

        case TokenType::TOKEN_KEYWORD_FUNCTION:
        {
            return this->match_function_statement(false);
        } break;

        case TokenType::TOKEN_KEYWORD_PROCEDURE:
        {
            return this->match_procedure_statement(false);
        } break;

        case TokenType::TOKEN_KEYWORD_VARIABLE:
        {
            return this->match_variable_statement();
        } break;

        case TokenType::TOKEN_KEYWORD_SCOPE:
        {
            return this->match_scope_statement();
        } break;

        case TokenType::TOKEN_KEYWORD_WHILE:
        {
            return this->match_while_statement();
        } break;

        case TokenType::TOKEN_KEYWORD_LOOP:
        {
            return this->match_loop_statement();
        } break;

        case TokenType::TOKEN_KEYWORD_IF:
        {
            return this->match_conditional_if_statement();
        } break;

        case TokenType::TOKEN_KEYWORD_READ:
        {
            return this->match_read_statement();
        } break;

        case TokenType::TOKEN_KEYWORD_WRITE:
        {
            return this->match_write_statement();
        } break;

        default:
        {
            return this->match_expression_statement();
        } break;

    }

    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_expression_statement()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_variable_statement()
{

    try
    {

        this->consume_current_token_as(TokenType::TOKEN_KEYWORD_VARIABLE, __LINE__);

        Token identifier_token = this->tokenizer->get_current_token();
        this->consume_current_token_as(TokenType::TOKEN_IDENTIFIER, __LINE__);

        // Check if the symbol is already defined.
        if (this->environment->symbol_exists_locally(identifier_token.reference))
        {
            throw new SyntaxError(__LINE__, this->path, identifier_token,
                "Variable declaration '%s' is already defined.",
                identifier_token.reference.c_str());
        }

        // Get the size.
        shared_ptr<SyntaxNode> size_node = this->match_expression();

    }
    catch (SyntaxException &e)
    {
        throw e;
    }

    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_scope_statement()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_while_statement()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_loop_statement()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_conditional_if_statement()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_conditional_elseif_statement()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_read_statement()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_write_statement()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_procedure_call_statement()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_expression()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_assignment()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_equality()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_comparison()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_term()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_factor()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_magnitude()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_extraction()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_derivation()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_unary()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_function_call()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_array_index()
{

    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_primary()
{

    return nullptr;
}

