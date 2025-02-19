#include <platform/filesystem.hpp>
#include <compiler/parser/parser.hpp>
#include <compiler/parser/statements.hpp>
#include <compiler/parser/expressions.hpp>
#include <compiler/parser/errorhandler.hpp>
#include <compiler/parser/validators/exprtype.hpp>

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
        throw SyntaxError(sloc, this->path, this->tokenizer->get_current_token(),
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
            try
            {
                this->match_include_statement();
            }
            catch (SyntaxException& e)
            {
                throw;
            }
        } break;

        case TokenType::TOKEN_KEYWORD_PROCEDURE:
        {
            try
            {
                return this->match_procedure_statement(true);
            }
            catch (SyntaxException& e)
            {
                this->synchronize_to(TokenType::TOKEN_KEYWORD_ENDPROCEDURE);
                throw;
            }
        } break;

        case TokenType::TOKEN_KEYWORD_FUNCTION:
        {
            try
            {
                return this->match_function_statement(true);
            }
            catch (SyntaxException& e)
            {
                this->synchronize_to(TokenType::TOKEN_KEYWORD_ENDFUNCTION);
                throw;
            }
        } break;

        case TokenType::TOKEN_KEYWORD_BEGIN:
        {
            try
            {
                return this->match_begin_statement();
            }
            catch (SyntaxException& e)
            {
                this->synchronize_to(TokenType::TOKEN_KEYWORD_END);
                throw;
            }
        } break;

    }

    throw SyntaxError(__LINE__, this->path, current_token,
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
            throw SyntaxError(__LINE__, this->path, string_token,
                "Include file '%s' does not exist.",
                include_path.c_str());
        }

        // Check if the dependency is already added.
        bool is_already_included = this->graph->has_dependency(this->path.c_str(), include_path.c_str());

        // Before we generate the node, we need to add it to the dependency graph.
        DependencyResult result = this->graph->add_dependency(this->path.c_str(), include_path.c_str());
        if (result == DependencyResult::DEPENDENCY_IS_CIRCULAR)
        {
            throw SyntaxError(__LINE__, this->path, string_token,
                "Cyclical dependency encountered for %s.",
                include_path.c_str());
        }
        else if (result == DependencyResult::DEPENDENCY_ALREADY_INCLUDED)
        {
            throw SyntaxWarning(__LINE__, this->path, string_token,
                "File %s has already been included.",
                include_path.c_str());
        }
        else if (result == DependencyResult::DEPENDENCY_PARENT_NOT_FOUND)
        {
            throw SyntaxError(__LINE__, this->path, string_token,
                "Parent file %s not found.",
                include_path.c_str());
        }
        else if (result == DependencyResult::DEPENDENCY_SELF_INCLUDED)
        {
            throw SyntaxError(__LINE__, this->path, string_token,
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
                throw SyntaxError(__LINE__, this->path, string_token,
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
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

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
            throw SyntaxError(__LINE__, this->path, identifier_token,
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
            function_node);

        this->environment->set_symbol_locally(identifier_token.reference, function_symbol);

        // Add the parameters to the symbol table.
        for (auto parameter : parameters)
        {

            auto parameter_node = dynamic_pointer_cast<SyntaxNodeParameter>(parameter);
            SF_ENSURE_PTR(parameter_node);

            Symbol parameter_symbol(parameter_node->identifier, 
                Symboltype::SYMBOL_TYPE_VARIABLE, 
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

                if (e.handled == false)
                {
                    std::cout << e.what() << std::endl;
                    e.handled = true;
                }

                this->synchronize_to(TokenType::TOKEN_SEMICOLON);

            }

        }

        Symbol *function_return_symbol = this->environment->get_symbol_locally(identifier_token.reference);
        if (function_return_symbol->get_type() == Symboltype::SYMBOL_TYPE_DECLARED)
        {
            throw SyntaxError(__LINE__, this->path, identifier_token,
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
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
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
            throw SyntaxError(__LINE__, this->path, identifier_token,
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

                if (e.handled == false)
                {
                    std::cout << e.what() << std::endl;
                    e.handled = true;
                }

                this->synchronize_to(TokenType::TOKEN_SEMICOLON);

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
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_begin_statement()
{

    try
    {

        this->consume_current_token_as(TokenType::TOKEN_KEYWORD_BEGIN, __LINE__);
        this->consume_current_token_as(TokenType::TOKEN_SEMICOLON, __LINE__);
        
        // Check if begin is already defined.
        if (this->environment->is_begin_defined())
        {
            throw SyntaxError(__LINE__, this->path, this->tokenizer->get_current_token(),
                "Begin block is already defined.");
        }

        // Define the begin block.
        this->environment->define_begin(); 

        // Push the scope.
        this->environment->push_table();

        // Match all the body statements.
        vector<shared_ptr<SyntaxNode>> children;
        while (!this->expect_current_token_as(TokenType::TOKEN_EOF))
        {

            if (this->expect_current_token_as(TokenType::TOKEN_KEYWORD_END)) break;

            try
            {

                auto local_node = this->match_local_statement();
                children.push_back(local_node);

            }
            catch (SyntaxException& e)
            {

                if (e.handled == false)
                {
                    std::cout << e.what() << std::endl;
                    e.handled = true;
                }

                this->synchronize_to(TokenType::TOKEN_SEMICOLON);

            }

        }

        // Pop the scope.
        this->environment->pop_table();

        // Check for the end keyword.
        this->consume_current_token_as(TokenType::TOKEN_KEYWORD_END, __LINE__);

        // Semicolon.
        this->consume_current_token_as(TokenType::TOKEN_SEMICOLON, __LINE__);

        auto begin_node = this->generate_node<SyntaxNodeMain>();
        begin_node->children = children;

        // TODO(Chris): This would be a good place to issue warnings for all statements
        //              proceeding that they are unreachable in the code. For now,
        //              let the magic happen.

        return begin_node;

    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
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
            try 
            {
                return this->match_function_statement(false);
            }
            catch (SyntaxException& e)
            {
                synchronize_to(TokenType::TOKEN_KEYWORD_ENDFUNCTION);
                throw;
            }
        } break;

        case TokenType::TOKEN_KEYWORD_PROCEDURE:
        {
            try
            {
                return this->match_procedure_statement(false);
            }
            catch (SyntaxException& e)
            {
                synchronize_to(TokenType::TOKEN_KEYWORD_ENDPROCEDURE);
                throw;
            }
        } break;

        case TokenType::TOKEN_KEYWORD_VARIABLE:
        {

            try 
            {
                return this->match_variable_statement();
            }
            catch (SyntaxException& e)
            {
                throw;
            }

        } break;

        case TokenType::TOKEN_KEYWORD_SCOPE:
        {
            try
            {
                return this->match_scope_statement();
            }
            catch (SyntaxException& e)
            {
                this->synchronize_to(TokenType::TOKEN_KEYWORD_ENDSCOPE);
                throw;
            }
        } break;

        case TokenType::TOKEN_KEYWORD_WHILE:
        {
            try
            {
                return this->match_while_statement();
            }
            catch (SyntaxException& e)
            {
                this->synchronize_to(TokenType::TOKEN_KEYWORD_ENDWHILE);
                throw;
            }
        } break;

        case TokenType::TOKEN_KEYWORD_LOOP:
        {
            try
            {
                return this->match_loop_statement();
            }
            catch (SyntaxException& e)
            {
                this->synchronize_to(TokenType::TOKEN_KEYWORD_ENDLOOP);
                throw;
            }
        } break;

        case TokenType::TOKEN_KEYWORD_IF:
        {
            try
            {
                return this->match_conditional_if_statement();
            }
            catch (SyntaxException& e)
            {
                this->synchronize_to(TokenType::TOKEN_KEYWORD_ENDIF);
                throw;
            }
        } break;

        case TokenType::TOKEN_KEYWORD_READ:
        {

            try
            {
                this->match_read_statement();
            }
            catch (SyntaxException& e)
            {
                throw;
            }

        } break;

        case TokenType::TOKEN_KEYWORD_WRITE:
        {
            try
            {
                return this->match_write_statement();
            }
            catch (SyntaxException& e)
            {
                throw;
            }
        } break;

        default:
        {
            try
            {
                return this->match_expression_statement();
            }
            catch (SyntaxException& e)
            {
                throw;
            }
        } break;

    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_expression_statement()
{

    try
    {

        shared_ptr<SyntaxNode> expression = this->match_expression();
        this->consume_current_token_as(TokenType::TOKEN_SEMICOLON, __LINE__);

        auto expression_node = this->generate_node<SyntaxNodeExpressionStatement>();
        expression_node->expression = expression;

        return expression_node;

    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_variable_statement()
{

    try
    {

        // Generate the variable node.
        auto variable_node = this->generate_node<SyntaxNodeVariableStatement>();

        this->consume_current_token_as(TokenType::TOKEN_KEYWORD_VARIABLE, __LINE__);

        Token identifier_token = this->tokenizer->get_current_token();
        this->consume_current_token_as(TokenType::TOKEN_IDENTIFIER, __LINE__);

        // Check if the symbol is already defined.
        if (this->environment->symbol_exists_locally(identifier_token.reference))
        {
            throw SyntaxError(__LINE__, this->path, identifier_token,
                "Variable declaration '%s' is already defined.",
                identifier_token.reference.c_str());
        }

        // Get the size.
        shared_ptr<SyntaxNode> size_node = this->match_expression();

        // Get the optional dimensions.
        vector<shared_ptr<SyntaxNode>> dimensions;
        while (!this->expect_current_token_as(TokenType::TOKEN_EOF))
        {

            if (this->expect_current_token_as(TokenType::TOKEN_SEMICOLON)) break;
            if (this->expect_current_token_as(TokenType::TOKEN_COLON_EQUALS)) break;

            shared_ptr<SyntaxNode> dimension_node = this->match_expression();
            dimensions.push_back(dimension_node);

        }

        // Check for the assignment operator.
        shared_ptr<SyntaxNode> assignment_node = nullptr;
        if (this->expect_current_token_as(TokenType::TOKEN_COLON_EQUALS))
        {
            this->tokenizer->shift();
            assignment_node = this->match_expression();
        }

        // Validate the semicolon.
        this->consume_current_token_as(TokenType::TOKEN_SEMICOLON, __LINE__);

        // Insert the variable into the symbol table.
        Symbol variable_symbol(identifier_token.reference,
            Symboltype::SYMBOL_TYPE_VARIABLE,
            variable_node);

        this->environment->set_symbol_locally(identifier_token.reference, variable_symbol);

        // Fill out the variable node.
        variable_node->identifier   = identifier_token.reference;
        variable_node->storage      = size_node;
        variable_node->dimensions   = dimensions;
        variable_node->expression   = assignment_node;

        if (assignment_node != nullptr)
        {

            ExpressionTypeVisitor right_type(this->environment, this->path.c_str());
            assignment_node->accept(&right_type);

            if (right_type.get_evaluated_type() == Datatype::DATA_TYPE_UNKNOWN)
            {
                throw SyntaxError(__LINE__, this->path, this->tokenizer->get_previous_token(),
                    "Unknown datatype in assignment expression.");
            }

            else if (right_type.get_evaluated_type() == Datatype::DATA_TYPE_ERROR)
            {
                throw SyntaxError(__LINE__, this->path, this->tokenizer->get_previous_token(),
                    "Error in assignment expression.");
            }

            variable_node->set_datatype(right_type.get_evaluated_type());

        }

        return variable_node;

    }
    catch (SyntaxException &e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_scope_statement()
{

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_while_statement()
{

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_loop_statement()
{

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_conditional_if_statement()
{

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_conditional_elseif_statement()
{

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_read_statement()
{

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_write_statement()
{

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_procedure_call_statement()
{

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_expression()
{

    shared_ptr<SyntaxNode> expression = this->match_assignment();
    return expression;

}

shared_ptr<SyntaxNode> ParseTree::
match_assignment()
{

    try
    {

        shared_ptr<SyntaxNode> left_hand_side = this->match_equality();

        if (left_hand_side->get_nodetype() != Nodetype::NODE_TYPE_PRIMARY &&
            left_hand_side->get_nodetype() != Nodetype::NODE_TYPE_ARRAY_INDEX)
        {
            return left_hand_side;
        }

        string identifier;

        if (left_hand_side->get_nodetype() == Nodetype::NODE_TYPE_PRIMARY)
        {

            shared_ptr<SyntaxNodePrimary> primary_node = 
                dynamic_pointer_cast<SyntaxNodePrimary>(left_hand_side);

            if (primary_node->primary != Primarytype::PRIMARY_TYPE_IDENTIFIER)
                return left_hand_side;

            identifier = primary_node->primitive;

        }

        if (!this->expect_current_token_as(TokenType::TOKEN_COLON_EQUALS))
            return left_hand_side;

        this->consume_current_token_as(TokenType::TOKEN_COLON_EQUALS, __LINE__);

        // Validate it exists in the local symbol table.
        if (left_hand_side->get_nodetype() == Nodetype::NODE_TYPE_PRIMARY)
        {

            shared_ptr<SyntaxNodePrimary> primary_node = 
                dynamic_pointer_cast<SyntaxNodePrimary>(left_hand_side);

            if (!this->environment->symbol_exists_locally(primary_node->primitive))
            {

                throw SyntaxError(__LINE__, this->path, this->tokenizer->get_previous_token(),
                    "Undefined '%s' symbol in assignment expression.",
                    primary_node->primitive.c_str());

            }

        }

        // Match right hand side.
        shared_ptr<SyntaxNode> right_hand_side = this->match_expression();

        // NOTE(Chris): At this point, we would then traverse down this expression
        //              and evaluate what the highest type this expression can be
        //              and update the symbol table with the type of the variable.
        //
        // TODO(Chris): For now, we will let this run throw as a generic and once
        //              we finish the refactor, we will come back and write the
        //              evaluator for this.

        // Update the primary symbol as defined. Arrays are assumed to be defined.
        if (left_hand_side->get_nodetype() == Nodetype::NODE_TYPE_PRIMARY)
        {

            shared_ptr<SyntaxNodePrimary> primary_node = 
                dynamic_pointer_cast<SyntaxNodePrimary>(left_hand_side);

            if (primary_node->primary == Primarytype::PRIMARY_TYPE_IDENTIFIER)
            {

                Symbol *symbol = this->environment->get_symbol(primary_node->primitive);
                if (symbol->get_type() == Symboltype::SYMBOL_TYPE_DECLARED)
                {
                    symbol->set_type(Symboltype::SYMBOL_TYPE_VARIABLE);
                }

            }

        }

        // Generate the assignment node.
        auto assignment_node = this->generate_node<SyntaxNodeAssignment>();
        assignment_node->left = left_hand_side;
        assignment_node->right = right_hand_side;

        ExpressionTypeVisitor left_type(this->environment, this->path.c_str());
        ExpressionTypeVisitor right_type(this->environment, this->path.c_str());
        left_hand_side->accept(&left_type);
        right_type.evaluate(left_type.get_evaluated_type());
        right_hand_side->accept(&right_type);

        if (right_type.get_evaluated_type() == Datatype::DATA_TYPE_UNKNOWN)
        {
            throw SyntaxError(__LINE__, this->path, this->tokenizer->get_previous_token(),
                "Unknown datatype in assignment expression.");
        }

        else if (right_type.get_evaluated_type() == Datatype::DATA_TYPE_ERROR)
        {
            throw SyntaxError(__LINE__, this->path, this->tokenizer->get_previous_token(),
                "Error in assignment expression.");
        }

        // Determine what was returned. This value will tell us how to promote the
        // left-hand-side part of the expression.
        Symbol *symbol = this->environment->get_symbol(identifier);
        SF_ENSURE_PTR(symbol);
        symbol->get_node()->set_datatype(right_type.get_evaluated_type());
        
        // TODO(Chris): A mechanism for nodes to be identifiable for their ability
        //              to have a datatype associated with them would be nice. Don't
        //              really need to translate the nodes just to set this, so we should
        //              make this a thing in the validator OR we should make a visitor
        //              to do this.

        return assignment_node;

    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_equality()
{

    try
    {

        // Fetch the left hand side.
        shared_ptr<SyntaxNode> left_hand_side = this->match_comparison();

        // Process the equality expression.
        while (this->expect_current_token_as(TokenType::TOKEN_EQUALS) ||
               this->expect_current_token_as(TokenType::TOKEN_HASH))
        {

            // Get the token to validate the operation type.
            Token operator_token = this->tokenizer->get_current_token();
            this->tokenizer->shift();

            // Collect the right hand side.
            shared_ptr<SyntaxNode> right_hand_side = this->match_comparison();

            // Select the operation type.
            Operationtype operation = Operationtype::OPERATION_TYPE_UNKNOWN;
            switch (operator_token.type)
            {
                case TokenType::TOKEN_EQUALS:   operation = Operationtype::OPERATION_TYPE_EQUALS; break;
                case TokenType::TOKEN_HASH:     operation = Operationtype::OPERATION_TYPE_NOT_EQUALS; break;
                default:
                {
                    throw SyntaxError(__LINE__, this->path, operator_token,
                        "Unknown operation type '%s' in equality expression.",
                        operator_token.reference.c_str());
                }
            }

            // Generate the equality node.
            auto equality_node = this->generate_node<SyntaxNodeEquality>();
            equality_node->left             = left_hand_side;
            equality_node->right            = right_hand_side;
            equality_node->operation        = operation;
            left_hand_side                  = dynamic_pointer_cast<SyntaxNode>(equality_node);

        }

        return left_hand_side;

    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_comparison()
{

    try
    {

        shared_ptr<SyntaxNode> left_hand_side = this->match_term();
        while (this->expect_current_token_as(TokenType::TOKEN_LESS_THAN) ||
               this->expect_current_token_as(TokenType::TOKEN_LESS_THAN_EQUALS) ||
               this->expect_current_token_as(TokenType::TOKEN_GREATER_THAN) ||
               this->expect_current_token_as(TokenType::TOKEN_GREATER_THAN_EQUALS))
        {

            Token operator_token = this->tokenizer->get_current_token();
            this->tokenizer->shift();

            shared_ptr<SyntaxNode> right_hand_side = this->match_term();

            Operationtype operation = Operationtype::OPERATION_TYPE_UNKNOWN;
            switch (operator_token.type)
            {
                case TokenType::TOKEN_LESS_THAN:            operation = Operationtype::OPERATION_TYPE_LESS_THAN; break;
                case TokenType::TOKEN_LESS_THAN_EQUALS:     operation = Operationtype::OPERATION_TYPE_LESS_THAN_OR_EQUAL; break;
                case TokenType::TOKEN_GREATER_THAN:         operation = Operationtype::OPERATION_TYPE_GREATER_THAN; break;
                case TokenType::TOKEN_GREATER_THAN_EQUALS:  operation = Operationtype::OPERATION_TYPE_GREATER_THAN_OR_EQUAL; break;
                default:
                {
                    throw SyntaxError(__LINE__, this->path, operator_token,
                        "Unknown operation type '%s' in comparison expression.",
                        operator_token.reference.c_str());
                }
            }

            // Generate the comparison node.
            auto comparison_node = this->generate_node<SyntaxNodeComparison>();
            comparison_node->left           = left_hand_side;
            comparison_node->right          = right_hand_side;
            comparison_node->operation      = operation;
            left_hand_side                  = dynamic_pointer_cast<SyntaxNode>(comparison_node);

        }

        return left_hand_side;

    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_term()
{

    try
    {

        shared_ptr<SyntaxNode> left_hand_side = this->match_factor();
        while (this->expect_current_token_as(TokenType::TOKEN_PLUS) ||
               this->expect_current_token_as(TokenType::TOKEN_MINUS))
        {

            Token operator_token = this->tokenizer->get_current_token();
            this->tokenizer->shift();

            shared_ptr<SyntaxNode> right_hand_side = this->match_factor();

            Operationtype operation = Operationtype::OPERATION_TYPE_UNKNOWN;
            switch (operator_token.type)
            {
                case TokenType::TOKEN_PLUS:     operation = Operationtype::OPERATION_TYPE_ADDITION; break;
                case TokenType::TOKEN_MINUS:    operation = Operationtype::OPERATION_TYPE_SUBTRACTION; break;
                default:
                {
                    throw SyntaxError(__LINE__, this->path, operator_token,
                        "Unknown operation type '%s' in term expression.",
                        operator_token.reference.c_str());
                }
            }

            // Generate the term node.
            auto term_node = this->generate_node<SyntaxNodeTerm>();
            term_node->left             = left_hand_side;
            term_node->right            = right_hand_side;
            term_node->operation        = operation;
            left_hand_side              = dynamic_pointer_cast<SyntaxNode>(term_node);

        }

        return left_hand_side;

    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_factor()
{

    try
    {

        shared_ptr<SyntaxNode> left_hand_side = this->match_magnitude();
        while (this->expect_current_token_as(TokenType::TOKEN_STAR) ||
               this->expect_current_token_as(TokenType::TOKEN_FORWARD_SLASH))
        {

            Token operator_token = this->tokenizer->get_current_token();
            this->tokenizer->shift();

            shared_ptr<SyntaxNode> right_hand_side = this->match_magnitude();

            Operationtype operation = Operationtype::OPERATION_TYPE_UNKNOWN;
            switch (operator_token.type)
            {
                case TokenType::TOKEN_STAR:             operation = Operationtype::OPERATION_TYPE_MULTIPLICATION; break;
                case TokenType::TOKEN_FORWARD_SLASH:    operation = Operationtype::OPERATION_TYPE_DIVISION; break;
                default:
                {
                    throw SyntaxError(__LINE__, this->path, operator_token,
                        "Unknown operation type '%s' in factor expression.",
                        operator_token.reference.c_str());
                }
            }

            // Generate the factor node.
            auto factor_node = this->generate_node<SyntaxNodeFactor>();
            factor_node->left           = left_hand_side;
            factor_node->right          = right_hand_side;
            factor_node->operation      = operation;
            left_hand_side              = dynamic_pointer_cast<SyntaxNode>(factor_node);

        }

        return left_hand_side;

    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_magnitude()
{

    try
    {

        shared_ptr<SyntaxNode> left_hand_side = this->match_extraction();
        while (this->expect_current_token_as(TokenType::TOKEN_CARROT))
        {

            Token operator_token = this->tokenizer->get_current_token();
            this->tokenizer->shift();

            shared_ptr<SyntaxNode> right_hand_side = this->match_extraction();

            Operationtype operation = Operationtype::OPERATION_TYPE_UNKNOWN;
            switch (operator_token.type)
            {
                case TokenType::TOKEN_CARROT:   operation = Operationtype::OPERATION_TYPE_POWER; break;
                default:
                {
                    throw SyntaxError(__LINE__, this->path, operator_token,
                        "Unknown operation type '%s' in magnitude expression.",
                        operator_token.reference.c_str());
                }
            }

            // Generate the magnitude node.
            auto magnitude_node = this->generate_node<SyntaxNodeMagnitude>();
            magnitude_node->left           = left_hand_side;
            magnitude_node->right          = right_hand_side;
            magnitude_node->operation      = operation;
            left_hand_side                  = dynamic_pointer_cast<SyntaxNode>(magnitude_node);

        }

        return left_hand_side;

    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_extraction()
{

    try
    {

        shared_ptr<SyntaxNode> left_hand_side = this->match_derivation();
        while (this->expect_current_token_as(TokenType::TOKEN_PIPE))
        {

            Token operator_token = this->tokenizer->get_current_token();
            this->tokenizer->shift();

            shared_ptr<SyntaxNode> right_hand_side = this->match_derivation();

            Operationtype operation = Operationtype::OPERATION_TYPE_UNKNOWN;
            switch (operator_token.type)
            {
                case TokenType::TOKEN_PIPE: operation = Operationtype::OPERATION_TYPE_EXTRACTION; break;
                default:
                {
                    throw SyntaxError(__LINE__, this->path, operator_token,
                        "Unknown operation type '%s' in extraction expression.",
                        operator_token.reference.c_str());
                }
            }

            // Generate the extraction node.
            auto extraction_node = this->generate_node<SyntaxNodeExtraction>();
            extraction_node->left           = left_hand_side;
            extraction_node->right          = right_hand_side;
            extraction_node->operation      = operation;

            left_hand_side = dynamic_pointer_cast<SyntaxNode>(extraction_node);

        }

        return left_hand_side;

    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;
}

shared_ptr<SyntaxNode> ParseTree::
match_derivation()
{

    try
    {

        shared_ptr<SyntaxNode> left_hand_side = this->match_unary();
        while (this->expect_current_token_as(TokenType::TOKEN_PERCENT))
        {

            Token operator_token = this->tokenizer->get_current_token();
            this->tokenizer->shift();

            shared_ptr<SyntaxNode> right_hand_side = this->match_unary();

            Operationtype operation = Operationtype::OPERATION_TYPE_UNKNOWN;
            switch (operator_token.type)
            {
                case TokenType::TOKEN_PERCENT: operation = Operationtype::OPERATION_TYPE_DERIVATION; break;
                default:
                {
                    throw SyntaxError(__LINE__, this->path, operator_token,
                        "Unknown operation type '%s' in derivation expression.",
                        operator_token.reference.c_str());
                }
            }

            // Generate the derivation node.
            auto derivation_node = this->generate_node<SyntaxNodeDerivation>();
            derivation_node->left           = left_hand_side;
            derivation_node->right          = right_hand_side;
            derivation_node->operation      = operation;

            left_hand_side = dynamic_pointer_cast<SyntaxNode>(derivation_node);

        }

        return left_hand_side;

    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_unary()
{

    try
    {

        if (this->expect_current_token_as(TokenType::TOKEN_MINUS))
        {

            Token operator_token = this->tokenizer->get_current_token();
            this->tokenizer->shift();

            shared_ptr<SyntaxNode> right_hand_side = this->match_primary();

            Operationtype operation = Operationtype::OPERATION_TYPE_UNKNOWN;
            switch (operator_token.type)
            {
                case TokenType::TOKEN_MINUS: operation = Operationtype::OPERATION_TYPE_NEGATION; break;
                default:
                {
                    throw SyntaxError(__LINE__, this->path, operator_token,
                        "Unknown operation type '%s' in unary expression.",
                        operator_token.reference.c_str());
                }
            }

            // Generate the unary node.
            auto unary_node = this->generate_node<SyntaxNodeUnary>();
            unary_node->expression      = right_hand_side;
            unary_node->operation       = operation;

        }

        shared_ptr<SyntaxNode> right_hand_side = this->match_function_call();
        return right_hand_side;

    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_function_call()
{

    try
    {

        // If the symbol is an identifier, we need to check if it is a function.
        Token identifier_token = this->tokenizer->get_current_token();
        if (!this->expect_current_token_as(TokenType::TOKEN_IDENTIFIER))
            return this->match_array_index();

        Symbol *symbol = this->environment->get_symbol(identifier_token.reference);
        if (symbol->get_type() != Symboltype::SYMBOL_TYPE_FUNCTION)
        {
            return this->match_array_index();
        }

        this->tokenizer->shift();
        this->consume_current_token_as(TokenType::TOKEN_LEFT_PARENTHESIS, __LINE__);

        // Collect the arguments.
        vector<shared_ptr<SyntaxNode>> arguments;
        while (!this->expect_current_token_as(TokenType::TOKEN_EOF))
        {

            if (this->expect_current_token_as(TokenType::TOKEN_RIGHT_PARENTHESIS))
                break;

            // Get the argument.
            shared_ptr<SyntaxNode> argument = this->match_expression();
            arguments.push_back(argument);

            if (this->expect_current_token_as(TokenType::TOKEN_COMMA))
            {
                this->consume_current_token_as(TokenType::TOKEN_COMMA, __LINE__);
            }

        }

        this->consume_current_token_as(TokenType::TOKEN_RIGHT_PARENTHESIS, __LINE__);

        // Validate arity.
        if (symbol->get_arity() != arguments.size())
        {
            throw SyntaxError(__LINE__, this->path, identifier_token,
                "Function '%s' expects %d arguments, but %d were provided.",
                identifier_token.reference.c_str(), symbol->get_arity(), arguments.size());
        }

        // NOTE(Chris): At this point, we will want to traverse the function node
        //              and validate the return type with respect to the set of
        //              argument expressions.
        //
        //              The main idea is that we will descend the argument expressions
        //              and determine their highest order types. With these types,
        //              we go into the symbol table, and then promote and validate
        //              them as needed. Once they're validated, we then traverse the
        //              function, checking the return value and promoting the function's
        //              return value once we get back.
        //
        //              Easy. Right?
        //
        // TODO(Chris): Perform this witch craft.

        // Generate the function call node.
        auto function_call_node = this->generate_node<SyntaxNodeFunctionCall>();
        function_call_node->identifier = identifier_token.reference;
        function_call_node->arguments = arguments;

        return function_call_node;

    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_array_index()
{

    try
    {

        if (this->expect_current_token_as(TokenType::TOKEN_IDENTIFIER) &&
            this->expect_next_token_as(TokenType::TOKEN_LEFT_PARENTHESIS))
        {

            Token identifier_token = this->tokenizer->get_current_token();
            this->tokenizer->shift();
            this->tokenizer->shift();

            // Get the dimensions.
            vector<shared_ptr<SyntaxNode>> dimensions;
            while (!this->expect_current_token_as(TokenType::TOKEN_EOF))
            {

                if (this->expect_current_token_as(TokenType::TOKEN_RIGHT_PARENTHESIS))
                    break;

                // Get the argument.
                shared_ptr<SyntaxNode> dimension = this->match_expression();
                dimensions.push_back(dimension);

                if (this->expect_current_token_as(TokenType::TOKEN_COMMA))
                {
                    this->consume_current_token_as(TokenType::TOKEN_COMMA, __LINE__);
                }

            }

            this->consume_current_token_as(TokenType::TOKEN_RIGHT_PARENTHESIS, __LINE__);

            Symbol *symbol = this->environment->get_symbol(identifier_token.reference);
            if (symbol->get_type() != Symboltype::SYMBOL_TYPE_VARIABLE)
            {
                throw SyntaxError(__LINE__, this->path, identifier_token,
                    "Symbol '%s' is not an array.",
                    identifier_token.reference.c_str());
            }

            if (symbol->is_array() == false)
            {
                throw SyntaxError(__LINE__, this->path, identifier_token,
                    "Symbol '%s' is not an array.",
                    identifier_token.reference.c_str());
            }

            if (symbol->get_arity() != dimensions.size())
            {
                throw SyntaxError(__LINE__, this->path, identifier_token,
                    "Array '%s' expects %d dimensions, but %d were provided.",
                    identifier_token.reference.c_str(), symbol->get_arity(), dimensions.size());
            }

            // Generate the array index node.
            auto array_index_node = this->generate_node<SyntaxNodeArrayIndex>();
            array_index_node->identifier = identifier_token.reference;
            array_index_node->indices = dimensions;

            return array_index_node;

        }

        return this->match_primary();
    
    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

shared_ptr<SyntaxNode> ParseTree::
match_primary()
{

    try
    {

        if (this->expect_current_token_as(TokenType::TOKEN_REAL) ||
            this->expect_current_token_as(TokenType::TOKEN_COMPLEX) ||
            this->expect_current_token_as(TokenType::TOKEN_INTEGER) ||
            this->expect_current_token_as(TokenType::TOKEN_STRING))
        {

            Token literal_token = this->tokenizer->get_current_token();
            this->tokenizer->shift();

            Primarytype primary_type = Primarytype::PRIMARY_TYPE_UNKNOWN;
            switch (literal_token.type)
            {
                case TokenType::TOKEN_COMPLEX: primary_type = Primarytype::PRIMARY_TYPE_COMPLEX; break;
                case TokenType::TOKEN_REAL:    primary_type = Primarytype::PRIMARY_TYPE_REAL; break;
                case TokenType::TOKEN_INTEGER: primary_type = Primarytype::PRIMARY_TYPE_INTEGER; break;
                case TokenType::TOKEN_STRING:  primary_type = Primarytype::PRIMARY_TYPE_STRING; break;
                default:
                {
                    throw SyntaxError(__LINE__, this->path, literal_token,
                        "Unknown literal type '%s' in primary expression.",
                        literal_token.reference.c_str());
                }
            }

            // Generate the primary node.
            auto primary_node = this->generate_node<SyntaxNodePrimary>();
            primary_node->primitive = literal_token.reference;
            primary_node->primary = primary_type;

            return primary_node;
            
        }

        else if (this->expect_current_token_as(TokenType::TOKEN_IDENTIFIER))
        {

            Token literal_token = this->tokenizer->get_current_token();
            this->tokenizer->shift();

            // Check if the token is declared.
            if (!this->environment->symbol_exists(literal_token.reference))
            {
                throw SyntaxError(__LINE__, this->path, literal_token,
                    "Undefined '%s' symbol in primary expression.",
                    literal_token.reference.c_str());
            }

            // Generate the primary node.
            auto primary_node = this->generate_node<SyntaxNodePrimary>();
            primary_node->primitive = literal_token.reference;
            primary_node->primary = Primarytype::PRIMARY_TYPE_IDENTIFIER;

            return primary_node;

        }

        else if (this->expect_current_token_as(TokenType::TOKEN_LEFT_PARENTHESIS))
        {

            this->consume_current_token_as(TokenType::TOKEN_LEFT_PARENTHESIS, __LINE__);
            shared_ptr<SyntaxNode> expression = this->match_expression();
            this->consume_current_token_as(TokenType::TOKEN_RIGHT_PARENTHESIS, __LINE__);

            // Create the grouping node.           
            auto grouping_node = this->generate_node<SyntaxNodeGrouping>();
            grouping_node->expression = expression;

            return grouping_node;

        }

        else
        {
            throw SyntaxError(__LINE__, this->path, this->tokenizer->get_current_token(),
                "Unexpected token '%s' in primary expression.",
                this->tokenizer->get_current_token().reference.c_str());
        }

    }
    catch (SyntaxException& e)
    {
        throw;
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

