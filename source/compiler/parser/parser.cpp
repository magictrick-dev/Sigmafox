#include <platform/filesystem.hpp>
#include <compiler/parser/parser.hpp>
#include <compiler/parser/subnodes.hpp>
#include <compiler/exceptions.hpp>
#include <compiler/parser/validators/evaluator.hpp>
#include <compiler/parser/validators/blockvalidator.hpp>

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
    
    //for (auto node : this->nodes) delete node;

}

bool ParseTree::
parse(string source_file)
{

    if (!file_exists(source_file.c_str()))
        return false;

    this->path = source_file;

    this->tokenizer = make_shared<Tokenizer>(source_file);
    SyntaxNode *root = this->match_root();
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

SyntaxNode* ParseTree::
get_root()
{

    return this->root;

}

vector<shared_ptr<SyntaxNode>>& ParseTree::
get_nodes()
{

    return this->nodes;

}

// --- Helper Methods ----------------------------------------------------------

void ParseTree::
synchronize_to(Tokentype type)
{

    while (!this->tokenizer->current_token_is(type))
    {

        if (this->tokenizer->current_token_is(Tokentype::TOKEN_EOF))
            return;

        this->tokenizer->shift();

    }

    this->tokenizer->shift();

}

void ParseTree::
synchronize_up_to(Tokentype type)
{

    while (!this->tokenizer->current_token_is(type))
    {

        if (this->tokenizer->current_token_is(Tokentype::TOKEN_EOF))
            return;

        this->tokenizer->shift();

    }

}

bool ParseTree::
expect_current_token_as(Tokentype type) const
{

    return this->tokenizer->current_token_is(type);

}

bool ParseTree::
expect_next_token_as(Tokentype type) const
{

    return this->tokenizer->next_token_is(type);

}

void ParseTree::
consume_current_token_as(Tokentype type, u64 sloc)
{

    if (this->expect_current_token_as(type))
    {
        this->tokenizer->shift();
    }
    else
    {
        throw CompilerSyntaxError(sloc,
            this->tokenizer->get_current_token().row,
            this->tokenizer->get_current_token().column,
            this->path.c_str(),
            "Unexpected token encountered '%s', expected '%s'.",
            this->tokenizer->get_current_token().reference.c_str(),
            Token::type_to_string(type).c_str());
    }

}

template <class T, typename ...Params> T* ParseTree::
generate_node(Params... args)
{

    T* raw_node = new T(args...);
    this->nodes.push_back(shared_ptr<SyntaxNode>(raw_node));
    return raw_node;

}

// --- Descent Methods ---------------------------------------------------------

SyntaxNode* ParseTree::
match_root()
{

    std::vector<SyntaxNode*> children;
    
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {
        
        try
        {

            SyntaxNode *current_node = this->match_global_statement();
            SF_ENSURE_PTR(current_node);
            children.push_back(current_node);

        }
        catch (CompilerException &e)
        {
            this->environment->handle_compiler_exception(e);
            this->synchronize_to(Tokentype::TOKEN_SEMICOLON);
        }
        
        
    }
    
    auto root_node = this->generate_node<SyntaxNodeRoot>();
    root_node->children = children;

    return root_node;

}

SyntaxNode* ParseTree::
match_global_statement()
{
    
    Token current_token = this->tokenizer->get_current_token();
    switch (this->tokenizer->get_current_token().type)
    {
        
        case Tokentype::TOKEN_KEYWORD_INCLUDE:
        {

            try
            {
                
                return this->match_include_statement();
                
            }
            catch (CompilerException &e)
            {
                
                throw;
                
            }

        }
        
        case Tokentype::TOKEN_KEYWORD_FUNCTION:
        {
            
            try
            {
                
                return this->match_function_statement(true);
                
            }
            catch (CompilerException &e)
            {
                
                this->synchronize_to(Tokentype::TOKEN_KEYWORD_ENDFUNCTION);
                throw;
                
            }
            
        }

        case Tokentype::TOKEN_KEYWORD_PROCEDURE:
        {
            
            try
            {
                
                return this->match_procedure_statement(true);
                
            }
            catch (CompilerException &e)
            {
                
                this->synchronize_to(Tokentype::TOKEN_KEYWORD_ENDPROCEDURE);
                throw;
                
            }
            
        }

        case Tokentype::TOKEN_KEYWORD_BEGIN:
        {
            
            try
            {
                
                return this->match_begin_statement();
                
            }
            catch (CompilerException &e)
            {
                
                this->synchronize_to(Tokentype::TOKEN_KEYWORD_END);
                throw;
                
            }
            
        }
        
        default:
        {
            
            throw CompilerSyntaxError(__LINE__,
                current_token.row,
                current_token.column,
                this->path.c_str(),
                "Unexpected token encountered. Expected: 'include', 'function', 'procedure', or 'begin'.");
            
        }
        
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

SyntaxNode* ParseTree::
match_include_statement()
{

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

SyntaxNode* ParseTree::
match_function_statement(bool is_global)
{

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_FUNCTION, __LINE__);

    Token identifier_token = this->tokenizer->get_current_token();
    this->consume_current_token_as(Tokentype::TOKEN_IDENTIFIER, __LINE__);

    string identifier = identifier_token.reference;
    if (this->environment->symbol_exists_locally(identifier))
    {

        throw CompilerSyntaxError(__LINE__,
            identifier_token.row,
            identifier_token.column,
            this->path.c_str(),
            "Identifier %s is already defined in the current scope.",
            identifier.c_str());

    }

    vector<SyntaxNodeVariableStatement*> parameters;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {

        if (this->expect_current_token_as(Tokentype::TOKEN_SEMICOLON)) break;

        Token current_parameter = this->tokenizer->get_current_token();
        this->consume_current_token_as(Tokentype::TOKEN_IDENTIFIER, __LINE__);

        string parameter_identifier = current_parameter.reference;
        if (parameter_identifier == identifier)
        {

            throw CompilerSyntaxError(__LINE__,
                current_parameter.row,
                current_parameter.column,
                this->path.c_str(),
                "Parameter %s conflicts with the name of the function.",
                parameter_identifier.c_str());

        }

        auto parameter_storage = this->generate_node<SyntaxNodePrimary>();
        parameter_storage->primarytype = Primarytype::PRIMARY_TYPE_INTEGER;
        parameter_storage->primitive = "4";

        auto parameter_node = this->generate_node<SyntaxNodeVariableStatement>();
        parameter_node->identifier      = parameter_identifier; 
        parameter_node->data_type       = Datatype::DATA_TYPE_UNKNOWN;
        parameter_node->structure_type  = Structuretype::STRUCTURE_TYPE_SCALAR;
        parameter_node->storage         = parameter_storage;
        parameter_node->expression      = nullptr;

        parameters.push_back(parameter_node);

    }

    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);

    // Generate the storage node.
    auto function_return_storage = this->generate_node<SyntaxNodePrimary>();
    function_return_storage->primarytype    = Primarytype::PRIMARY_TYPE_INTEGER;
    function_return_storage->primitive      = "4";

    // Generate the return variable node.
    auto function_return_variable = this->generate_node<SyntaxNodeVariableStatement>();
    function_return_variable->identifier        = identifier;
    function_return_variable->data_type         = Datatype::DATA_TYPE_UNKNOWN;
    function_return_variable->structure_type    = Structuretype::STRUCTURE_TYPE_SCALAR;
    function_return_variable->storage           = function_return_storage;
    function_return_variable->expression        = nullptr;

    // Process the body.
    this->environment->push_table();

    // Insert the name of the function into the body scope of the function.
    this->environment->set_symbol_locally(identifier, Symbol(identifier,
                Symboltype::SYMBOL_TYPE_DECLARED, function_return_variable));

    // Now the parameters. This ensures if there are name conflicts, they are caught here.
    for (auto parameter : parameters)
    {

        //SyntaxNodeVariableStatement *parameter_node = (SyntaxNodeVariableStatement*)parameter;
        SyntaxNodeVariableStatement *parameter_node = dynamic_cast<SyntaxNodeVariableStatement*>(parameter);
        SF_ENSURE_PTR(parameter_node);
        this->environment->set_symbol_locally(parameter_node->identifier, Symbol(parameter_node->identifier,
            Symboltype::SYMBOL_TYPE_VARIABLE, parameter_node));

    }

    // Collect the body statements.
    vector<SyntaxNode*> children;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {

        if (this->expect_current_token_as(Tokentype::TOKEN_KEYWORD_ENDFUNCTION)) break;

        try
        {

            auto current_node = this->match_local_statement();   
            SF_ENSURE_PTR(current_node);
            children.push_back(current_node);

        }

        catch(CompilerException &e)
        {
            this->environment->handle_compiler_exception(e);
            this->synchronize_to(Tokentype::TOKEN_SEMICOLON);
        }

    }

    // Ensures that the user properly set the return type.
    Symbol *return_symbol = this->environment->get_symbol(identifier);
    SF_ENSURE_PTR(return_symbol);
    if (return_symbol->get_type() != Symboltype::SYMBOL_TYPE_VARIABLE)
    {

        throw CompilerSyntaxError(__LINE__,
            identifier_token.row,
            identifier_token.column,
            this->path.c_str(),
            "Return value %s is not set in function definition.",
            identifier.c_str());

    }

    this->environment->pop_table();

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_ENDFUNCTION, __LINE__);
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);

    auto function_node = this->generate_node<SyntaxNodeFunctionStatement>();
    function_node->is_global            = is_global; // Changes generation?
    function_node->variable_node        = function_return_variable;
    function_node->parameters           = parameters;
    function_node->children             = children;

    // Insert the symbol into the symbol table.
    this->environment->set_symbol_locally(identifier, Symbol(identifier,
        Symboltype::SYMBOL_TYPE_FUNCTION, function_node, parameters.size()));

    return function_node;

}

SyntaxNode* ParseTree::
match_procedure_statement(bool is_global)
{

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_PROCEDURE, __LINE__);

    Token identifier_token = this->tokenizer->get_current_token();
    this->consume_current_token_as(Tokentype::TOKEN_IDENTIFIER, __LINE__);

    string identifier = identifier_token.reference;
    if (this->environment->symbol_exists_locally(identifier))
    {

        throw CompilerSyntaxError(__LINE__,
            identifier_token.row,
            identifier_token.column,
            this->path.c_str(),
            "Identifier %s is already defined in the current scope.",
            identifier.c_str());

    }

    vector<SyntaxNodeVariableStatement*> parameters;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {

        if (this->expect_current_token_as(Tokentype::TOKEN_SEMICOLON)) break;

        Token current_parameter = this->tokenizer->get_current_token();
        this->consume_current_token_as(Tokentype::TOKEN_IDENTIFIER, __LINE__);

        string parameter_identifier = current_parameter.reference;
        if (parameter_identifier == identifier)
        {

            throw CompilerSyntaxError(__LINE__,
                current_parameter.row,
                current_parameter.column,
                this->path.c_str(),
                "Parameter %s conflicts with the name of the function.",
                parameter_identifier.c_str());

        }

        auto parameter_storage = this->generate_node<SyntaxNodePrimary>();
        parameter_storage->primarytype = Primarytype::PRIMARY_TYPE_INTEGER;
        parameter_storage->primitive = "4";

        auto parameter_node = this->generate_node<SyntaxNodeVariableStatement>();
        parameter_node->identifier      = parameter_identifier; 
        parameter_node->data_type       = Datatype::DATA_TYPE_UNKNOWN;
        parameter_node->structure_type  = Structuretype::STRUCTURE_TYPE_SCALAR;
        parameter_node->storage         = parameter_storage;
        parameter_node->expression      = nullptr;

        parameters.push_back(parameter_node);

    }

    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);

    // Generate the storage node.
    auto procedure_return_storage = this->generate_node<SyntaxNodePrimary>();
    procedure_return_storage->primarytype    = Primarytype::PRIMARY_TYPE_INTEGER;
    procedure_return_storage->primitive      = "4";

    // Generate the return variable node.
    auto procedure_return_variable = this->generate_node<SyntaxNodeVariableStatement>();
    procedure_return_variable->identifier        = identifier;
    procedure_return_variable->data_type         = Datatype::DATA_TYPE_VOID;
    procedure_return_variable->structure_type    = Structuretype::STRUCTURE_TYPE_SCALAR;
    procedure_return_variable->storage           = procedure_return_storage;
    procedure_return_variable->expression        = nullptr;

    this->environment->push_table();

    // Now the parameters. This ensures if there are name conflicts, they are caught here.
    for (auto parameter : parameters)
    {

        //SyntaxNodeVariableStatement *parameter_node = (SyntaxNodeVariableStatement*)parameter;
        SyntaxNodeVariableStatement *parameter_node = dynamic_cast<SyntaxNodeVariableStatement*>(parameter);
        SF_ENSURE_PTR(parameter_node);
        this->environment->set_symbol_locally(parameter_node->identifier, Symbol(parameter_node->identifier,
            Symboltype::SYMBOL_TYPE_VARIABLE, parameter_node));

    }

    // Collect the body statements.
    vector<SyntaxNode*> children;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {

        if (this->expect_current_token_as(Tokentype::TOKEN_KEYWORD_ENDPROCEDURE)) break;

        try
        {

            auto current_node = this->match_local_statement();   
            SF_ENSURE_PTR(current_node);
            children.push_back(current_node);

        }

        catch(CompilerException &e)
        {
            this->environment->handle_compiler_exception(e);
            this->synchronize_to(Tokentype::TOKEN_SEMICOLON);
        }

    }

    this->environment->pop_table();

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_ENDPROCEDURE, __LINE__);
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);

    auto procedure_node = this->generate_node<SyntaxNodeProcedureStatement>();
    procedure_node->is_global            = is_global; // Changes generation?
    procedure_node->variable_node        = procedure_return_variable;
    procedure_node->parameters           = parameters;
    procedure_node->children             = children;

    // Insert the symbol into the symbol table.
    this->environment->set_symbol_locally(identifier, Symbol(identifier,
        Symboltype::SYMBOL_TYPE_PROCEDURE, procedure_node, parameters.size()));


    return procedure_node;

}

SyntaxNode* ParseTree::
match_begin_statement()
{

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_BEGIN, __LINE__);
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);
    
    this->environment->define_begin();
    this->environment->push_table();
    
    std::vector<SyntaxNode*> children;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {
        
        if (this->expect_current_token_as(Tokentype::TOKEN_KEYWORD_END)) break;
        
        try
        {
            
            SyntaxNode *current_node = this->match_local_statement();
            SF_ENSURE_PTR(current_node);
            children.push_back(current_node);
            
        }
        catch (CompilerException &e)
        {
            this->environment->handle_compiler_exception(e);
            this->synchronize_to(Tokentype::TOKEN_SEMICOLON);
        }
        
    }
    
    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_END, __LINE__);
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);
    
    this->environment->pop_table();
    

    auto main_node = this->generate_node<SyntaxNodeMain>();
    main_node->children = children;
    
    return main_node;

}

SyntaxNode* ParseTree::
match_local_statement()
{

    Token current_token = this->tokenizer->get_current_token();
    switch (current_token.type)
    {
        
        case Tokentype::TOKEN_KEYWORD_VARIABLE:
        {
            
            try
            {
                
                return this->match_variable_statement();
                
            }
            catch (CompilerException &e)
            {
                
                throw;
                
            }

        }
        
        case Tokentype::TOKEN_KEYWORD_SCOPE:
        {
            
            try
            {
                
                return this->match_scope_statement();
                
            }
            catch (CompilerException &e)
            {
                
                this->synchronize_to(Tokentype::TOKEN_KEYWORD_ENDSCOPE);
                throw;
                
            }
            
        }

        case Tokentype::TOKEN_KEYWORD_FUNCTION:
        {
            
            try
            {
                
                return this->match_function_statement(false);
                
            }
            catch (CompilerException &e)
            {
                
                this->synchronize_to(Tokentype::TOKEN_KEYWORD_ENDFUNCTION);
                throw;
                
            }
            
        }

        case Tokentype::TOKEN_KEYWORD_PROCEDURE:
        {
            
            try
            {
                
                return this->match_function_statement(false);
                
            }

            catch (CompilerException &e)
            {
                
                this->synchronize_to(Tokentype::TOKEN_KEYWORD_ENDPROCEDURE);
                throw;
                
            }
            
        }
        
        case Tokentype::TOKEN_KEYWORD_WHILE:
        {
            
            try
            {
                
                return this->match_while_statement();
                
            }
            catch (CompilerException &e)
            {
                
                this->synchronize_to(Tokentype::TOKEN_KEYWORD_ENDWHILE);
                throw;
                
            }
            
        }
        
        case Tokentype::TOKEN_KEYWORD_LOOP:
        {
            
            try
            {
                
                return this->match_loop_statement();
                
            }
            catch (CompilerException &e)
            {
                
                this->synchronize_to(Tokentype::TOKEN_KEYWORD_ENDLOOP);
                throw;
                
            }
            
        }

        case Tokentype::TOKEN_KEYWORD_IF:
        {
            
            try
            {
                
                return this->match_conditional_if_statement();
                
            }
            catch (CompilerException &e)
            {
                
                this->synchronize_to(Tokentype::TOKEN_KEYWORD_ENDIF);
                throw;
                
            }
            
        }
        
        case Tokentype::TOKEN_KEYWORD_READ:
        {
            
            try
            {
                
                return this->match_read_statement();
                
            }
            catch (CompilerException &e)
            {
                
                throw;
                
            }
            
        }

        case Tokentype::TOKEN_KEYWORD_WRITE:
        {
            
            try
            {
                
                return this->match_write_statement();
                
            }
            catch (CompilerException &e)
            {
                
                throw;
                
            }
            
        }

        default:
        {
            
            return this->match_expression_statement();

        }
        
    }

    SF_ASSERT(!"We should never directly return a nullptr.");
    return nullptr;

}

SyntaxNode* ParseTree::
match_expression_statement()
{

    // Base expressions.
    SyntaxNode *expression = this->match_expression();
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);
    
    auto *node = this->generate_node<SyntaxNodeExpressionStatement>();
    node->expression = expression;
    return node;

}

SyntaxNode* ParseTree::
match_variable_statement()
{

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_VARIABLE, __LINE__);
    this->consume_current_token_as(Tokentype::TOKEN_IDENTIFIER, __LINE__);
    
    auto identifier_token = this->tokenizer->get_previous_token();
    string identifier = identifier_token.reference;

    // Legacy feature of COSY which we need to honor for backwards compatibility.
    auto storage_expression = this->match_expression();
    
    // Dimensions.
    std::vector<SyntaxNode*> array_dimensions;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {
        
        if (this->expect_current_token_as(Tokentype::TOKEN_COLON_EQUALS)) break;
        if (this->expect_current_token_as(Tokentype::TOKEN_SEMICOLON)) break;
        
        auto dimension_expression = this->match_expression();
        array_dimensions.push_back(dimension_expression);
        
    }
    
    SyntaxNode *initializer_expression = nullptr;
    if (this->expect_current_token_as(Tokentype::TOKEN_COLON_EQUALS))
    {
        this->consume_current_token_as(Tokentype::TOKEN_COLON_EQUALS, __LINE__);
        initializer_expression = this->match_expression();
    }
    
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);

    auto *node = this->generate_node<SyntaxNodeVariableStatement>();
    node->dimensions        = array_dimensions;
    node->expression        = initializer_expression;
    node->identifier        = identifier;    
    node->storage           = storage_expression;
    
    // We can evaluate the initializer expression and determine the type.
    if (initializer_expression != nullptr)
    {
        
        ExpressionEvaluator evaluator(this->environment);
        initializer_expression->accept(&evaluator);

        node->data_type = evaluator();
        node->structure_type = Structuretype::STRUCTURE_TYPE_SCALAR;
        
    }

    // Construct the symbol and put it in the table.
    Symboltype symbol_type = Symboltype::SYMBOL_TYPE_DECLARED;
    if (initializer_expression != nullptr)
        symbol_type = Symboltype::SYMBOL_TYPE_VARIABLE;
    else if (array_dimensions.size() != 0)
        symbol_type = Symboltype::SYMBOL_TYPE_VARIABLE;
    
    Symbol variable_symbol(identifier, symbol_type, node, array_dimensions.size());
    this->environment->set_symbol_locally(identifier, variable_symbol);
    
    return node;

}

SyntaxNode* ParseTree::
match_scope_statement()
{
    
    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_SCOPE, __LINE__);
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);
    
    this->environment->push_table();
    vector<SyntaxNode*> children;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {
        
        if (this->expect_current_token_as(Tokentype::TOKEN_KEYWORD_ENDSCOPE)) break;
        
        try
        {
            
            SyntaxNode *current_node = this->match_local_statement();
            SF_ENSURE_PTR(current_node);
            children.push_back(current_node);
            
        }
        catch (CompilerException &e)
        {
            this->environment->handle_compiler_exception(e);
            this->synchronize_to(Tokentype::TOKEN_SEMICOLON);
        }
        
    }
    
    this->environment->pop_table();
    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_ENDSCOPE, __LINE__);
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);
    
    auto *node = this->generate_node<SyntaxNodeScopeStatement>();
    node->children = children;
    
    return node;

}

SyntaxNode* ParseTree::
match_while_statement()
{

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_WHILE, __LINE__);
    
    SyntaxNode *while_expression = this->match_expression();
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);
    

    this->environment->push_table();
    vector<SyntaxNode*> children;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {
        
        if (this->expect_current_token_as(Tokentype::TOKEN_KEYWORD_ENDWHILE)) break;
        
        try
        {

            SyntaxNode *current_node = this->match_local_statement();
            SF_ENSURE_PTR(current_node);
            children.push_back(current_node);

        }
        catch (CompilerException &e)
        {
            
            this->environment->handle_compiler_exception(e);
            this->synchronize_to(Tokentype::TOKEN_SEMICOLON);
            
        }

    }

    this->environment->pop_table();
    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_ENDWHILE, __LINE__);
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);

    auto while_node = this->generate_node<SyntaxNodeWhileStatement>();
    while_node->expression = while_expression;
    while_node->children = children;
    
    return while_node;

}

SyntaxNode* ParseTree::
match_loop_statement()
{

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_LOOP, __LINE__);
    
    // Ensure that we have an identifier.
    Token identifier_token = this->tokenizer->get_current_token();
    this->consume_current_token_as(Tokentype::TOKEN_IDENTIFIER, __LINE__);
    string identifier = identifier_token.reference;

    // Our starting and ending values.
    SyntaxNode *initial_value = this->match_expression();
    SyntaxNode *ending_value = this->match_expression();
    SyntaxNode *step_value = nullptr;

    // If we aren't at the semicolon, user specified optional loop.
    if (!this->expect_current_token_as(Tokentype::TOKEN_SEMICOLON))
    {   
        step_value = this->match_expression();
    }

    // Otherwise, we need to generate the primary.
    else
    {

        auto *node = this->generate_node<SyntaxNodePrimary>();
        node->primarytype   = Primarytype::PRIMARY_TYPE_INTEGER; 
        node->primitive     = "1";
        step_value = node;
        
    }

    // Create a variable node for the iterator.
    auto iterator_size = this->generate_node<SyntaxNodePrimary>();
    iterator_size->primarytype      = Primarytype::PRIMARY_TYPE_INTEGER;
    iterator_size->primitive        = "4";

    auto iterator_variable = this->generate_node<SyntaxNodeVariableStatement>();
    iterator_variable->identifier       = identifier;
    iterator_variable->storage          = iterator_size; 
    iterator_variable->expression       = initial_value;
    iterator_variable->data_type        = Datatype::DATA_TYPE_INTEGER;
    iterator_variable->structure_type   = Structuretype::STRUCTURE_TYPE_SCALAR;

    ExpressionEvaluator value_type(this->environment);
    initial_value->accept(&value_type);
    ending_value->accept(&value_type);

    Datatype evaluated_type = value_type();
    iterator_variable->data_type = evaluated_type;

    // Consume the semicolon.
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);


    // Push the table and collect the internal statements.
    this->environment->push_table();
    this->environment->set_symbol_locally(identifier, Symbol(identifier, 
                Symboltype::SYMBOL_TYPE_VARIABLE, iterator_variable));

    vector<SyntaxNode*> children;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {

        if (this->expect_current_token_as(Tokentype::TOKEN_KEYWORD_ENDLOOP)) break;

        try
        {

            SyntaxNode *current_node = this->match_local_statement();
            SF_ENSURE_PTR(current_node);
            children.push_back(current_node);

        }
        catch (CompilerException &e)
        {

            this->environment->handle_compiler_exception(e);
            this->synchronize_to(Tokentype::TOKEN_SEMICOLON);

        }

    }

    this->environment->pop_table();

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_ENDLOOP, __LINE__);
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);

    auto loop_node = this->generate_node<SyntaxNodeLoopStatement>();
    loop_node->iterator    = identifier;   
    loop_node->variable    = iterator_variable;
    loop_node->start       = initial_value;
    loop_node->end         = ending_value;
    loop_node->step        = step_value;
    loop_node->children    = children;

    return loop_node;

}

SyntaxNode* ParseTree::
match_conditional_if_statement()
{

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_IF, __LINE__);

    auto condition_expression = this->match_expression();
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);

    this->environment->push_table();

    vector<SyntaxNode*> children;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {

        if (this->expect_current_token_as(Tokentype::TOKEN_KEYWORD_ELSEIF) ||
            this->expect_current_token_as(Tokentype::TOKEN_KEYWORD_ENDIF))
        {
            break;
        }

        try
        {

            SyntaxNode *current_node = this->match_local_statement();
            SF_ENSURE_PTR(current_node);
            children.push_back(current_node);

        }
        catch (CompilerException &e)
        {

            this->environment->handle_compiler_exception(e);
            this->synchronize_to(Tokentype::TOKEN_SEMICOLON);

        }

    }

    this->environment->pop_table();

    SyntaxNodeConditionalStatement *else_statement = nullptr;
    if (this->expect_current_token_as(Tokentype::TOKEN_KEYWORD_ELSEIF))
    {
        else_statement = (SyntaxNodeConditionalStatement*)this->match_conditional_elseif_statement();
    }

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_ENDIF, __LINE__);
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);

    auto conditional_node = this->generate_node<SyntaxNodeConditionalStatement>();
    conditional_node->expression    = condition_expression;
    conditional_node->next          = else_statement;
    conditional_node->children      = children;

    return conditional_node;

}

SyntaxNode* ParseTree::
match_conditional_elseif_statement()
{

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_ELSEIF, __LINE__);

    auto condition_expression = this->match_expression();
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);

    this->environment->push_table();

    vector<SyntaxNode*> children;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {

        if (this->expect_current_token_as(Tokentype::TOKEN_KEYWORD_ELSEIF) ||
            this->expect_current_token_as(Tokentype::TOKEN_KEYWORD_ENDIF))
        {
            break;
        }

        try
        {

            SyntaxNode *current_node = this->match_local_statement();
            SF_ENSURE_PTR(current_node);
            children.push_back(current_node);

        }
        catch (CompilerException &e)
        {

            this->environment->handle_compiler_exception(e);
            this->synchronize_to(Tokentype::TOKEN_SEMICOLON);

        }

    }

    this->environment->pop_table();

    SyntaxNodeConditionalStatement *else_statement = nullptr;
    if (this->expect_current_token_as(Tokentype::TOKEN_KEYWORD_ELSEIF))
    {
        else_statement = (SyntaxNodeConditionalStatement*)this->match_conditional_elseif_statement();
    }

    auto conditional_node = this->generate_node<SyntaxNodeConditionalStatement>();
    conditional_node->expression    = condition_expression;
    conditional_node->next          = else_statement;
    conditional_node->children      = children;

    return conditional_node;


}

SyntaxNode* ParseTree::
match_read_statement()
{

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_READ, __LINE__);
    
    auto unit_expression = this->match_expression();

    Token identifier_token = this->tokenizer->get_current_token();
    string identifier = identifier_token.reference;

    this->consume_current_token_as(Tokentype::TOKEN_IDENTIFIER, __LINE__);
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);

    if (!this->environment->symbol_exists(identifier))
    {

        throw CompilerSyntaxError(
            __LINE__,
            this->tokenizer->get_previous_token().row,
            this->tokenizer->get_previous_token().column,
            this->path.c_str(),
            "Undeclared identifier '%s' used in read expression.",
            this->tokenizer->get_previous_token().reference.c_str());

    }

    Symbol *read_symbol = this->environment->get_symbol(identifier);
    SF_ENSURE_PTR(read_symbol);
    if (read_symbol->get_type() == Symboltype::SYMBOL_TYPE_FUNCTION ||
        read_symbol->get_type() == Symboltype::SYMBOL_TYPE_PROCEDURE)
    {

        throw CompilerSyntaxError(
            __LINE__,
            identifier_token.row,
            identifier_token.column,
            this->path.c_str(),
            "Invalid identifier type '%s' used in read expression.",
            identifier_token.reference.c_str());

    }

    auto read_node = this->generate_node<SyntaxNodeReadStatement>();
    read_node->identifier   = identifier;
    read_node->location     = unit_expression;

    SyntaxNodeVariableStatement *read_variable = (SyntaxNodeVariableStatement*)
            read_symbol->get_node();
    read_variable->data_type = Datatype::DATA_TYPE_STRING;
    read_variable->structure_type = Structuretype::STRUCTURE_TYPE_STRING;

    return read_node;

}

SyntaxNode* ParseTree::
match_write_statement()
{

    this->consume_current_token_as(Tokentype::TOKEN_KEYWORD_WRITE, __LINE__);
    
    // The output location dictates the unit to write to.
    SyntaxNode *output_location = this->match_expression();
    
    // We need to guarantee at least one expression is matched to write.
    vector<SyntaxNode*> children;
    SyntaxNode *initial_write = this->match_expression();
    children.push_back(initial_write);
    
    // Additional expressions.
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {

        if (this->expect_current_token_as(Tokentype::TOKEN_SEMICOLON)) break;
        
        auto current_node = this->match_expression();
        children.push_back(current_node);

    }
    
    this->consume_current_token_as(Tokentype::TOKEN_SEMICOLON, __LINE__);
    
    auto write_node = this->generate_node<SyntaxNodeWriteStatement>();
    write_node->location = output_location;
    write_node->expressions = children;
    
    return write_node;

}

SyntaxNode* ParseTree::
match_expression()
{
    
    return this->match_procedure_call();

}

SyntaxNode* ParseTree::
match_procedure_call()
{

    auto left_hand_side = this->match_assignment();

    // If we aren't a primary node, it can't be a function call.
    Nodetype left_hand_type = left_hand_side->get_nodetype();
    if (left_hand_type != Nodetype::NODE_TYPE_PRIMARY)
    {
        return left_hand_side;
    }

    // Determine the type of primary node.
    SyntaxNodePrimary *primary_node = (SyntaxNodePrimary*)left_hand_side;
    if (primary_node->primarytype != Primarytype::PRIMARY_TYPE_IDENTIFIER)
    {
        return left_hand_side;
    }

    // Check if the identifier exists first.
    string identifier = primary_node->primitive;
    if (!this->environment->symbol_exists(identifier))
    {
        return left_hand_side;
    }

    // Is this a procedure?
    if (!this->environment->symbol_exists(identifier))
    {

        throw CompilerSyntaxError(__LINE__,
            this->tokenizer->get_previous_token().row,
            this->tokenizer->get_previous_token().column,
            this->path.c_str(),
            "Identifier '%s' is undeclared and undefined.", identifier.c_str());

    }

    Symbol *procedure_symbol = this->environment->get_symbol(identifier);
    if (procedure_symbol->get_type() != Symboltype::SYMBOL_TYPE_PROCEDURE)
    {
        return left_hand_side;
    }

    // Collecting parameters.
    vector<SyntaxNode*> parameters;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {

        if (this->expect_current_token_as(Tokentype::TOKEN_SEMICOLON)) break;

        auto parameter = this->match_expression();
        parameters.push_back(parameter);

    }
  
    // Check for arity match.
    if (parameters.size() != procedure_symbol->get_arity())
    {

        throw CompilerSyntaxError(__LINE__,
            this->tokenizer->get_current_token().row,
            this->tokenizer->get_current_token().column,
            this->path.c_str(),
            "Arity mismatch for procedure %s. Number of arguments is incorrect.", 
            identifier.c_str());

    }

    SyntaxNodeProcedureStatement *procedure_node = 
        (SyntaxNodeProcedureStatement*)procedure_symbol->get_node();

    // Okay, arity matches, now we need to discern our types.
    this->environment->push_table();

    // Arguments.
    for (i32 idx = 0; idx < parameters.size(); ++idx)
    {

        auto argument = procedure_node->parameters[idx];

        Symbol parameter_symbol = Symbol(argument->identifier,
            Symboltype::SYMBOL_TYPE_VARIABLE,
            argument);

        // Evaluate the parameter with the provided arguments.
        ExpressionEvaluator argument_evaluation(this->environment);
        parameters[idx]->accept(&argument_evaluation);
        argument->data_type = argument_evaluation();
        
        this->environment->set_symbol_locally(argument->identifier, Symbol(argument->identifier,
            Symboltype::SYMBOL_TYPE_VARIABLE, argument));
    
    }

    // Return symbol.
    this->environment->set_symbol_locally(identifier, Symbol(identifier, 
         Symboltype::SYMBOL_TYPE_VARIABLE, procedure_node->variable_node));

    BlockValidator block_validator(this->environment);
    procedure_node->accept(&block_validator);

    this->environment->pop_table();

    auto procedure_call_node = this->generate_node<SyntaxNodeProcedureCall>();   
    procedure_call_node->identifier     = identifier;
    procedure_call_node->arguments      = parameters;
    return procedure_call_node;


}

SyntaxNode* ParseTree::
match_assignment()
{

    auto left_hand_side = this->match_equality();

    // Ensure the left hand type is an assignment node.
    Nodetype left_type = left_hand_side->get_nodetype();
    if (left_type != Nodetype::NODE_TYPE_PRIMARY &&
        left_type != Nodetype::NODE_TYPE_ARRAY_INDEX) 
    {
        return left_hand_side;
    }

    // If the following token isn't equals, than we're aren't even assigning anything.
    if (!this->expect_current_token_as(Tokentype::TOKEN_COLON_EQUALS))
        return left_hand_side;
    
    // Fetching the identifier for the assignment expression.
    string identifier;
    switch (left_type)
    {

        case Nodetype::NODE_TYPE_PRIMARY:
        {

            // Determine if the primary node is an identifier type.
            SyntaxNodePrimary *primary_node = reinterpret_cast<SyntaxNodePrimary*>(left_hand_side);
            if (primary_node->primarytype != Primarytype::PRIMARY_TYPE_IDENTIFIER)
            {
                return left_hand_side;
            }

            identifier = primary_node->primitive;

        } break;
        
        case Nodetype::NODE_TYPE_ARRAY_INDEX:
        {

            SyntaxNodeArrayIndex *index_node = reinterpret_cast<SyntaxNodeArrayIndex*>(left_hand_side);
            identifier = index_node->identifier; 

        } break;
        
        default:
        {
            SF_NO_IMPL("Alternative cases not implemented.");
        }

    }

    this->consume_current_token_as(Tokentype::TOKEN_COLON_EQUALS, __LINE__);
    
    // Ensure the identifier exists in the symbol table.
    if (!this->environment->symbol_exists(identifier))
    {
        throw CompilerSyntaxError(
            __LINE__,
            this->tokenizer->get_previous_token().row,
            this->tokenizer->get_previous_token().column,
            this->path.c_str(),
            "Undeclared identifier '%s' used in assignment expression.",
            this->tokenizer->get_previous_token().reference.c_str());
    }
    
    // Get the right hand side expression.
    auto right_hand_side = this->match_equality();    
    
    // Fetch the type of the symbol.
    if (!this->environment->symbol_exists(identifier))
    {

        throw CompilerSyntaxError(__LINE__,
            this->tokenizer->get_previous_token().row,
            this->tokenizer->get_previous_token().column,
            this->path.c_str(),
            "Identifier '%s' is undeclared and undefined.", identifier.c_str());

    }

    Symbol *assignment_symbol = this->environment->get_symbol(identifier);
    auto variable = dynamic_cast<SyntaxNodeVariableStatement*>(assignment_symbol->get_node());
    
    // Type deduction.
    ExpressionEvaluator evaluator(this->environment, variable->data_type);
    right_hand_side->accept(&evaluator);

    Datatype type_deduction = evaluator();
    if (type_deduction == Datatype::DATA_TYPE_ERROR)
    {
        throw CompilerSyntaxError(
            __LINE__,
            this->tokenizer->get_previous_token().row,
            this->tokenizer->get_previous_token().column,
            this->path.c_str(),
            "Type deducation for '%s' in assignment expression is invalid.",
            identifier.c_str());
    }
    
    variable->data_type         = type_deduction;
    variable->structure_type    = Structuretype::STRUCTURE_TYPE_SCALAR;
    assignment_symbol->set_type(Symboltype::SYMBOL_TYPE_VARIABLE);
    
    auto assignment_node = this->generate_node<SyntaxNodeAssignment>();
    assignment_node->identifier     = identifier;
    assignment_node->left           = left_hand_side;
    assignment_node->right          = right_hand_side;
    
    return assignment_node;

}

SyntaxNode* ParseTree::
match_equality()
{

    auto left_hand_side = this->match_comparison();
    
    while (this->expect_current_token_as(Tokentype::TOKEN_EQUALS) ||
           this->expect_current_token_as(Tokentype::TOKEN_HASH))
    {
        
        Token operator_token = this->tokenizer->get_current_token();
        this->tokenizer->shift();
        
        auto right_hand_side = this->match_comparison();
        
        Operationtype operation_type = Operationtype::OPERATION_TYPE_UNKNOWN;
        switch (operator_token.type)
        {
            
            case Tokentype::TOKEN_EQUALS:
            {
                operation_type = Operationtype::OPERATION_TYPE_EQUALS;
            } break;
                
            case Tokentype::TOKEN_HASH:
            {
                operation_type = Operationtype::OPERATION_TYPE_NOT_EQUALS;
            } break;
                
            default:
            {
                SF_ASSERT(!"We should never reach this point.");
            } break;
                
        }
        
        auto *node = this->generate_node<SyntaxNodeEquality>();
        node->left              = left_hand_side;
        node->right             = right_hand_side;
        node->operation         = operation_type;
        left_hand_side          = node;
        
    }
    
    return left_hand_side;

}

SyntaxNode* ParseTree::
match_comparison()
{

    auto left_hand_side = this->match_concatenation();
    
    while (this->expect_current_token_as(Tokentype::TOKEN_LESS_THAN) ||
           this->expect_current_token_as(Tokentype::TOKEN_GREATER_THAN) ||
           this->expect_current_token_as(Tokentype::TOKEN_LESS_THAN_EQUALS) ||
           this->expect_current_token_as(Tokentype::TOKEN_GREATER_THAN_EQUALS))
    {
        
        Token operator_token = this->tokenizer->get_current_token();
        this->tokenizer->shift();
        
        auto right_hand_side = this->match_concatenation();
        
        Operationtype operation_type = Operationtype::OPERATION_TYPE_UNKNOWN;
        switch (operator_token.type)
        {
            
            case Tokentype::TOKEN_LESS_THAN:
            {
                operation_type = Operationtype::OPERATION_TYPE_LESS_THAN;
            } break;
                
            case Tokentype::TOKEN_GREATER_THAN:
            {
                operation_type = Operationtype::OPERATION_TYPE_GREATER_THAN;
            } break;
                
            case Tokentype::TOKEN_LESS_THAN_EQUALS:
            {
                operation_type = Operationtype::OPERATION_TYPE_LESS_THAN_OR_EQUAL;
            } break;
                
            case Tokentype::TOKEN_GREATER_THAN_EQUALS:
            {
                operation_type = Operationtype::OPERATION_TYPE_GREATER_THAN_OR_EQUAL;
            } break;
                
            default:
            {
                SF_ASSERT(!"We should never reach this point.");
            } break;
                
        }
        
        auto *node = this->generate_node<SyntaxNodeComparison>();
        node->left              = left_hand_side;
        node->right             = right_hand_side;
        node->operation         = operation_type;
        left_hand_side          = node;
        
    }
    
    return left_hand_side;

}

SyntaxNode* ParseTree::
match_concatenation()
{

    auto left_hand_side = this->match_term();

    while (this->expect_current_token_as(Tokentype::TOKEN_AMPERSAND))
    {
        
        Token operator_token = this->tokenizer->get_current_token();
        this->tokenizer->shift();
        
        auto right_hand_side = this->match_term();
        
        Operationtype operation_type = Operationtype::OPERATION_TYPE_UNKNOWN;
        switch (operator_token.type)
        {
            
            case Tokentype::TOKEN_AMPERSAND:
            {
                operation_type = Operationtype::OPERATION_TYPE_CONCATENATE;
            } break;
                
            default:
            {
                SF_ASSERT(!"We should never reach this point.");
            } break;
                
        }
        
        auto *node = this->generate_node<SyntaxNodeTerm>();
        node->left              = left_hand_side;
        node->right             = right_hand_side;
        node->operation         = operation_type;
        left_hand_side          = node;
        
    }
    
    return left_hand_side;

}

SyntaxNode* ParseTree::
match_term()
{

    auto left_hand_side = this->match_factor();
    
    while (this->expect_current_token_as(Tokentype::TOKEN_PLUS) ||
           this->expect_current_token_as(Tokentype::TOKEN_MINUS))
    {
        
        Token operator_token = this->tokenizer->get_current_token();
        this->tokenizer->shift();
        
        auto right_hand_side = this->match_factor();
        
        Operationtype operation_type = Operationtype::OPERATION_TYPE_UNKNOWN;
        switch (operator_token.type)
        {
            
            case Tokentype::TOKEN_PLUS:
            {
                operation_type = Operationtype::OPERATION_TYPE_ADDITION;
            } break;
                
            case Tokentype::TOKEN_MINUS:
            {
                operation_type = Operationtype::OPERATION_TYPE_SUBTRACTION;
            } break;
                
            default:
            {
                SF_ASSERT(!"We should never reach this point.");
            } break;
                
        }
        
        auto *node = this->generate_node<SyntaxNodeTerm>();
        node->left              = left_hand_side;
        node->right             = right_hand_side;
        node->operation         = operation_type;
        left_hand_side          = node;
        
    }
    
    return left_hand_side;

}

SyntaxNode* ParseTree::
match_factor()
{

    auto left_hand_side = this->match_magnitude();
    
    while (this->expect_current_token_as(Tokentype::TOKEN_STAR) ||
           this->expect_current_token_as(Tokentype::TOKEN_FORWARD_SLASH))
    {
        
        Token operator_token = this->tokenizer->get_current_token();
        this->tokenizer->shift();
        
        auto right_hand_side = this->match_magnitude();
        
        Operationtype operation_type = Operationtype::OPERATION_TYPE_UNKNOWN;
        switch (operator_token.type)
        {
            
            case Tokentype::TOKEN_STAR:
            {
                operation_type = Operationtype::OPERATION_TYPE_MULTIPLICATION;
            } break;
                
            case Tokentype::TOKEN_FORWARD_SLASH:
            {
                operation_type = Operationtype::OPERATION_TYPE_DIVISION;
            } break;
                
            default:
            {
                SF_ASSERT(!"We should never reach this point.");
            } break;
                
        }
        
        auto *node = this->generate_node<SyntaxNodeFactor>();
        node->left              = left_hand_side;
        node->right             = right_hand_side;
        node->operation         = operation_type;
        left_hand_side          = node;
        
    }
    
    return left_hand_side;

}

SyntaxNode* ParseTree::
match_magnitude()
{

    auto left_hand_side = this->match_extraction();

    while (this->expect_current_token_as(Tokentype::TOKEN_CARROT))
    {
        
        Token operator_token = this->tokenizer->get_current_token();
        this->tokenizer->shift();
        
        auto right_hand_side = this->match_extraction();
        
        Operationtype operation_type = Operationtype::OPERATION_TYPE_UNKNOWN;
        switch (operator_token.type)
        {
            
            case Tokentype::TOKEN_CARROT:
            {
                operation_type = Operationtype::OPERATION_TYPE_POWER;
            } break;
                
            default:
            {
                SF_ASSERT(!"We should never reach this point.");
            } break;
                
        }
        
        auto *node = this->generate_node<SyntaxNodeMagnitude>();
        node->left              = left_hand_side;
        node->right             = right_hand_side;
        node->operation         = operation_type;
        left_hand_side          = node;
        
    }
    
    return left_hand_side;

}

SyntaxNode* ParseTree::
match_extraction()
{

    auto left_hand_side = this->match_derivation();

    while (this->expect_current_token_as(Tokentype::TOKEN_PIPE))
    {
        
        Token operator_token = this->tokenizer->get_current_token();
        this->tokenizer->shift();
        
        auto right_hand_side = this->match_derivation();
        
        Operationtype operation_type = Operationtype::OPERATION_TYPE_UNKNOWN;
        switch (operator_token.type)
        {
            
            case Tokentype::TOKEN_PIPE:
            {
                operation_type = Operationtype::OPERATION_TYPE_EXTRACTION;
            } break;
                
            default:
            {
                SF_ASSERT(!"We should never reach this point.");
            } break;
                
        }
        
        auto *node = this->generate_node<SyntaxNodeExtraction>();
        node->left              = left_hand_side;
        node->right             = right_hand_side;
        node->operation         = operation_type;
        left_hand_side          = node;
        
    }
    
    return left_hand_side;

}

SyntaxNode* ParseTree::
match_derivation()
{

    auto left_hand_side = this->match_unary();

    while (this->expect_current_token_as(Tokentype::TOKEN_PERCENT))
    {
        
        Token operator_token = this->tokenizer->get_current_token();
        this->tokenizer->shift();
        
        auto right_hand_side = this->match_unary();
        
        Operationtype operation_type = Operationtype::OPERATION_TYPE_UNKNOWN;
        switch (operator_token.type)
        {
            
            case Tokentype::TOKEN_PERCENT:
            {
                operation_type = Operationtype::OPERATION_TYPE_DERIVATION;
            } break;
                
            default:
            {
                SF_ASSERT(!"We should never reach this point.");
            } break;
                
        }
        
        auto *node = this->generate_node<SyntaxNodeDerivation>();
        node->left              = left_hand_side;
        node->right             = right_hand_side;
        node->operation         = operation_type;
        left_hand_side          = node;
        
    }
    
    return left_hand_side;

}

SyntaxNode* ParseTree::
match_unary()
{

    if (this->expect_current_token_as(Tokentype::TOKEN_MINUS))
    {
        
        Token operator_token = this->tokenizer->get_current_token();
        this->tokenizer->shift();
        
        auto right_hand_side = this->match_unary();
        
        Operationtype operation_type = Operationtype::OPERATION_TYPE_UNKNOWN;
        switch (operator_token.type)
        {
            
            case Tokentype::TOKEN_MINUS:
            {
                operation_type = Operationtype::OPERATION_TYPE_NEGATION;
            } break;
                
            default:
            {
                SF_ASSERT(!"We should never reach this point.");
            } break;
                
        }
        
        auto *node = this->generate_node<SyntaxNodeUnary>();
        node->expression        = right_hand_side;
        node->operation         = operation_type;
        return node;
        
    }

    return this->match_function_call();

}

SyntaxNode* ParseTree::
match_function_call()
{

    auto left_hand_side = this->match_array_index();

    // If we aren't a primary node, it can't be a function call.
    Nodetype left_hand_type = left_hand_side->get_nodetype();
    if (left_hand_type != Nodetype::NODE_TYPE_PRIMARY)
    {
        return left_hand_side;
    }

    // Determine the type of primary node.
    SyntaxNodePrimary *primary_node = (SyntaxNodePrimary*)left_hand_side;
    if (primary_node->primarytype != Primarytype::PRIMARY_TYPE_IDENTIFIER)
    {
        return left_hand_side;
    }

    // Check if the identifier exists first.
    string identifier = primary_node->primitive;
    if (!this->environment->symbol_exists(identifier))
    {
        return left_hand_side;
    }

    // Is this a function?
    if (!this->environment->symbol_exists(identifier))
    {

        throw CompilerSyntaxError(__LINE__,
            this->tokenizer->get_previous_token().row,
            this->tokenizer->get_previous_token().column,
            this->path.c_str(),
            "Identifier '%s' is undeclared and undefined.", identifier.c_str());

    }

    Symbol *function_symbol = this->environment->get_symbol(identifier);
    if (function_symbol->get_type() != Symboltype::SYMBOL_TYPE_FUNCTION)
    {
        return left_hand_side;
    }

    // It's a function, now we need to gather arguments.
    this->consume_current_token_as(Tokentype::TOKEN_LEFT_PARENTHESIS, __LINE__);

    // Collecting parameters.
    vector<SyntaxNode*> parameters;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {

        if (this->expect_current_token_as(Tokentype::TOKEN_RIGHT_PARENTHESIS)) break;

        auto parameter = this->match_expression();
        parameters.push_back(parameter);

        if (this->expect_current_token_as(Tokentype::TOKEN_COMMA))
        {

            this->consume_current_token_as(Tokentype::TOKEN_COMMA, __LINE__);
            if (this->expect_current_token_as(Tokentype::TOKEN_RIGHT_PARENTHESIS))
            {

                throw CompilerSyntaxError(__LINE__,
                    this->tokenizer->get_current_token().row,
                    this->tokenizer->get_current_token().column,
                    this->path.c_str(),
                    "Expected expression in parameter list, encountered '%s'.", 
                    identifier.c_str());

            }

        }

    }

    this->consume_current_token_as(Tokentype::TOKEN_RIGHT_PARENTHESIS, __LINE__);
    
    // Check for arity match.
    if (parameters.size() != function_symbol->get_arity())
    {

        throw CompilerSyntaxError(__LINE__,
            this->tokenizer->get_current_token().row,
            this->tokenizer->get_current_token().column,
            this->path.c_str(),
            "Arity mismatch for function %s. Number of arguments is incorrect.", 
            identifier.c_str());

    }

    SyntaxNodeFunctionStatement *function_node = dynamic_cast<SyntaxNodeFunctionStatement*>(function_symbol->get_node());
    SF_ENSURE_PTR(function_node);

    // Okay, arity matches, now we need to discern our types.
    this->environment->push_table();

    // Arguments.
    for (i32 idx = 0; idx < parameters.size(); ++idx)
    {

        auto argument = function_node->parameters[idx];

        Symbol parameter_symbol = Symbol(argument->identifier, Symboltype::SYMBOL_TYPE_VARIABLE, argument);

        // Evaluate the parameter with the provided arguments.
        ExpressionEvaluator argument_evaluation(this->environment);
        parameters[idx]->accept(&argument_evaluation);
        argument->data_type = argument_evaluation();
        
        this->environment->set_symbol_locally(argument->identifier, Symbol(argument->identifier,
            Symboltype::SYMBOL_TYPE_VARIABLE, argument));
    
    }

    // Return symbol.
    this->environment->set_symbol_locally(identifier, Symbol(identifier, 
         Symboltype::SYMBOL_TYPE_VARIABLE, function_node->variable_node));

    BlockValidator block_validator(this->environment);
    function_node->accept(&block_validator);

    this->environment->pop_table();

    auto function_call_node = this->generate_node<SyntaxNodeFunctionCall>();   
    function_call_node->identifier      = identifier;
    function_call_node->arguments       = parameters;
    return function_call_node;

}

SyntaxNode* ParseTree::
match_array_index()
{

    auto *left_hand_side = this->match_primary();

    // Check if primary (could be grouping).
    Nodetype node_type = left_hand_side->get_nodetype();
    if (node_type != Nodetype::NODE_TYPE_PRIMARY) 
        return left_hand_side;

    SyntaxNodePrimary *primary_node = dynamic_cast<SyntaxNodePrimary*>(left_hand_side);
    SF_ENSURE_PTR(primary_node);

    // Check if it is a primary identifier node.
    Primarytype primary_type = primary_node->primarytype;
    if (primary_type != Primarytype::PRIMARY_TYPE_IDENTIFIER)
        return left_hand_side;

    // Is it an array index?
    if (!this->expect_current_token_as(Tokentype::TOKEN_LEFT_PARENTHESIS))
        return left_hand_side;

    string identifier = primary_node->primitive;

    // Does the identifier exist?
    if (!this->environment->symbol_exists(identifier))
    {

        throw CompilerSyntaxError(__LINE__,
            this->tokenizer->get_previous_token().row,
            this->tokenizer->get_previous_token().column,
            this->path.c_str(),
            "Identifier '%s' is undeclared and undefined.", identifier.c_str());

    }

    Symbol *array_symbol = this->environment->get_symbol(identifier);
    SF_ENSURE_PTR(array_symbol);

    if (array_symbol->get_type() != Symboltype::SYMBOL_TYPE_VARIABLE)
        return left_hand_side;

    if (array_symbol->get_arity() == 0)
        return left_hand_side;

    this->consume_current_token_as(Tokentype::TOKEN_LEFT_PARENTHESIS, __LINE__);

    // Collect the indices.
    vector<SyntaxNode*> array_index_expressions;
    while (!this->expect_current_token_as(Tokentype::TOKEN_EOF))
    {

        if (this->expect_current_token_as(Tokentype::TOKEN_RIGHT_PARENTHESIS)) break;

        auto expression = this->match_expression();
        array_index_expressions.push_back(expression);

        if (this->expect_current_token_as(Tokentype::TOKEN_COMMA))
        {

            this->consume_current_token_as(Tokentype::TOKEN_COMMA, __LINE__);
            if (this->expect_current_token_as(Tokentype::TOKEN_RIGHT_PARENTHESIS))
            {

                throw CompilerSyntaxError(__LINE__,
                    this->tokenizer->get_current_token().row,
                    this->tokenizer->get_current_token().column,
                    this->path.c_str(),
                    "Expected expression in parameter list, encountered '%s'.", 
                    identifier.c_str());

            }

        }

    }

    this->consume_current_token_as(Tokentype::TOKEN_RIGHT_PARENTHESIS, __LINE__);

    // Variable symbol cast.
    SyntaxNodeVariableStatement *array_variable = 
        dynamic_cast<SyntaxNodeVariableStatement*>(array_symbol->get_node());
    SF_ENSURE_PTR(array_variable);

    if (array_symbol->get_arity() != array_index_expressions.size())
    {

        throw CompilerSyntaxError(__LINE__,
            this->tokenizer->get_previous_token().row,
            this->tokenizer->get_previous_token().column,
            this->path.c_str(),
            "Array '%s' dimensions are mismatched, expected %i, received %i.",
            identifier.c_str(), array_symbol->get_arity(), array_index_expressions.size());

    }

    auto array_index_node = this->generate_node<SyntaxNodeArrayIndex>();
    array_index_node->identifier    = identifier;
    array_index_node->indices       = array_index_expressions;
    return array_index_node;

}

SyntaxNode* ParseTree::
match_primary()
{

    if (this->expect_current_token_as(Tokentype::TOKEN_REAL) ||
        this->expect_current_token_as(Tokentype::TOKEN_INTEGER) ||
        this->expect_current_token_as(Tokentype::TOKEN_STRING) ||
        this->expect_current_token_as(Tokentype::TOKEN_COMPLEX))
    {
        
        Token literal_token = this->tokenizer->get_current_token();
        this->tokenizer->shift();
    
        Primarytype primary_type = Primarytype::PRIMARY_TYPE_UNKNOWN;    
        switch (literal_token.type)
        {
            case Tokentype::TOKEN_REAL:
            {
                primary_type = Primarytype::PRIMARY_TYPE_REAL;
            } break;

            case Tokentype::TOKEN_INTEGER:
            {
                primary_type = Primarytype::PRIMARY_TYPE_INTEGER;
            } break;

            case Tokentype::TOKEN_STRING:
            {
                primary_type = Primarytype::PRIMARY_TYPE_STRING;
            } break;
            
            case Tokentype::TOKEN_COMPLEX:
            {
                primary_type = Primarytype::PRIMARY_TYPE_COMPLEX;
            } break;

            default:
            {
                SF_ASSERT(!"We should never reach this point.");
            } break;
        }
        
        
        auto *node = this->generate_node<SyntaxNodePrimary>();
        node->primarytype   = primary_type; 
        node->primitive     = literal_token.reference;
        return node;
        
    }

    else if (this->expect_current_token_as(Tokentype::TOKEN_IDENTIFIER))
    {
        
        Token identifier_token = this->tokenizer->get_current_token();
        this->tokenizer->shift();
        
        string identifier = identifier_token.reference;
        
        // Does the identifier exist?
        if (!this->environment->symbol_exists(identifier))
        {

            throw CompilerSyntaxError(__LINE__,
                identifier_token.row,
                identifier_token.column,
                this->path.c_str(),
                "Identifier '%s' is undeclared and undefined.", identifier.c_str());

        }
        
        auto *node = this->generate_node<SyntaxNodePrimary>();
        node->primarytype   = Primarytype::PRIMARY_TYPE_IDENTIFIER;
        node->primitive     = identifier;
        return node;
        
    }
    
    else if (this->expect_current_token_as(Tokentype::TOKEN_LEFT_PARENTHESIS))
    {
        
        this->consume_current_token_as(Tokentype::TOKEN_LEFT_PARENTHESIS, __LINE__);
        auto *expression = this->match_expression();
        this->consume_current_token_as(Tokentype::TOKEN_RIGHT_PARENTHESIS, __LINE__);

        auto *node = this->generate_node<SyntaxNodeGrouping>();
        node->expression = expression;
        
        return node;

        
    }
    
    else
    {
        
        this->tokenizer->shift();
        throw CompilerSyntaxError(__LINE__,
            this->tokenizer->get_previous_token().row,
            this->tokenizer->get_previous_token().column,
            this->path.c_str(),
            "Unexpected token encountered '%s' encountered in expression.",
            this->tokenizer->get_previous_token().reference.c_str());
        
    }

}

