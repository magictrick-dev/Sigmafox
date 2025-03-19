#include <compiler/parser/validators/blockvalidator.hpp>
#include <compiler/parser/validators/evaluator.hpp>
#include <compiler/parser/errorhandler.hpp>

BlockValidator::
BlockValidator(Environment *environment)
    : environment(environment)
{
    
}

BlockValidator::
~BlockValidator()
{
    
}

void BlockValidator::
visit(SyntaxNodeParameter* node)
{
    
}

void BlockValidator::
visit(SyntaxNodeFunctionStatement* node)
{
    
    // Check for recursion.
    if (std::find(this->call_stack.begin(), this->call_stack.end(), node->identifier) != this->call_stack.end())
    {
        
        std::cout << "-- Uncaught recursion error in block validator." << std::endl;
        return;

    }

    this->call_stack.push_back(node->identifier);
    
    for (auto child : node->children)
    {
        child->accept(this);
    }
    
}

void BlockValidator::
visit(SyntaxNodeProcedureStatement* node)
{
    
}

void BlockValidator::
visit(SyntaxNodeExpressionStatement* node)
{
    
    node->expression->accept(this);
    
}

void BlockValidator::
visit(SyntaxNodeProcedureCallStatement* node)
{
    
}

void BlockValidator::
visit(SyntaxNodeWhileStatement* node)
{
    
}

void BlockValidator::
visit(SyntaxNodeLoopStatement* node)
{
    
}

void BlockValidator::
visit(SyntaxNodeVariableStatement* node)
{
    
    // We need to visit this expression in order for any of the function calls to be validated.
    node->expression->accept(this);
    
    ExpressionEvaluator evaluator(this->environment);
    node->expression->accept(&evaluator);
    
    if (evaluator() == Datatype::DATA_TYPE_ERROR)
    {
        std::cout << "-- Uncaught error in block validator." << std::endl;
        return;
    }
      
    node->set_datatype(evaluator());
    
    Symbol variable_symbol = Symbol(node->identifier,
        Symboltype::SYMBOL_TYPE_VARIABLE,
        node);
    
    if (this->environment->symbol_exists_locally(node->identifier))
    {
        std::cout << "-- Uncaught error for variable pre-existing, this shouldn't happen." << std::endl;
    }

    this->environment->set_symbol_locally(node->identifier, variable_symbol);
    
}

void BlockValidator::
visit(SyntaxNodeScopeStatement* node)
{
    
}

void BlockValidator::
visit(SyntaxNodeConditionalStatement* node)
{
    
}

void BlockValidator::
visit(SyntaxNodeReadStatement* node)
{
    
}

void BlockValidator::
visit(SyntaxNodeWriteStatement* node)
{
    
}

void BlockValidator::
visit(SyntaxNodeExpression* node)
{
    
    node->expression->accept(this);
    
}

void BlockValidator::
visit(SyntaxNodeAssignment* node)
{
    
    ExpressionEvaluator left(this->environment);
    node->left->accept(&left);
    
    ExpressionEvaluator right(this->environment, left());
    node->right->accept(&right);
    
    if (right() == Datatype::DATA_TYPE_ERROR)
    {
        std::cout << "-- Uncaught error in block validator." << std::endl;
        return;
    }
    
    string identifier;
    if (node->left->get_nodetype() == Nodetype::NODE_TYPE_PRIMARY)
    {
        SyntaxNodePrimary *primary = (SyntaxNodePrimary*)node->left.get();
        identifier = primary->primitive;
    }
    
    else if (node->left->get_nodetype() == Nodetype::NODE_TYPE_ARRAY_INDEX)
    {
        SyntaxNodeArrayIndex *array_index = (SyntaxNodeArrayIndex*)node->left.get();
        identifier = array_index->identifier;
    }
    
    Symbol *symbol = this->environment->get_symbol(identifier);
    if (symbol == nullptr)
    {
        std::cout << "-- Uncaught error in block validator." << std::endl;
        return;
    }
    
    symbol->get_node()->set_datatype(right());
    
}

void BlockValidator::
visit(SyntaxNodeEquality* node)
{
    
    node->left->accept(this);
    node->right->accept(this);
    
}

void BlockValidator::
visit(SyntaxNodeComparison* node)
{
    
    node->left->accept(this);
    node->right->accept(this);
    
}

void BlockValidator::
visit(SyntaxNodeTerm* node)
{
    
    node->left->accept(this);
    node->right->accept(this);
    
}

void BlockValidator::
visit(SyntaxNodeFactor* node)
{
    
    node->left->accept(this);
    node->right->accept(this);
    
}

void BlockValidator::
visit(SyntaxNodeMagnitude* node)
{
    
    node->left->accept(this);
    node->right->accept(this);
    
}

void BlockValidator::
visit(SyntaxNodeExtraction* node)
{
    
    node->left->accept(this);
    node->right->accept(this);
    
}

void BlockValidator::
visit(SyntaxNodeDerivation* node)
{
    
    node->left->accept(this);
    node->right->accept(this);
    
}

void BlockValidator::
visit(SyntaxNodeUnary* node)
{
    
    node->expression->accept(this);
    
}

void BlockValidator::
visit(SyntaxNodeFunctionCall* node)
{
    
    auto function_node = (SyntaxNodeFunctionStatement*)
        this->environment->get_symbol(node->identifier)->get_node();
    
    vector<ExpressionEvaluator> argument_types;
    for (u64 i = 0; i < node->arguments.size(); i++)
    {
        
        Datatype current_type = function_node->parameters[i]->get_datatype();
        
        ExpressionEvaluator argument_type = { this->environment, current_type };
        node->arguments[i]->accept(&argument_type);
        argument_types.push_back(argument_type);
        
        if (argument_type() == Datatype::DATA_TYPE_ERROR)
        {
            std::cout << "-- Uncaught error in block validator." << std::endl;
            return;
        }
        
    }
    
    // Set the argument types.
    for (size_t i = 0; i < node->arguments.size(); i++)
        function_node->parameters[i]->set_datatype(argument_types[i]());

    // Now validate the block by pushing the environment and simulating the block.
    this->environment->push_table();

    for (auto argument : function_node->parameters)
    {
        
        SyntaxNodeParameter *parameter = (SyntaxNodeParameter*)argument.get();
        
        Symbol parameter_symbol = Symbol(parameter->identifier,
            Symboltype::SYMBOL_TYPE_VARIABLE,
            parameter);
        
        this->environment->set_symbol_locally(parameter->identifier, parameter_symbol);
        
    }

    Symbol return_symbol = Symbol(function_node->identifier,
        Symboltype::SYMBOL_TYPE_VARIABLE,
        function_node);

    this->environment->set_symbol_locally(function_node->identifier, return_symbol);

    BlockValidator validator(this->environment);
    function_node->accept(&validator);

    this->environment->pop_table();
    
}

void BlockValidator::
visit(SyntaxNodeArrayIndex* node)
{
    
}

void BlockValidator::
visit(SyntaxNodePrimary* node)
{
    
    
}

void BlockValidator::
visit(SyntaxNodeGrouping* node)
{
    
    node->expression->accept(this);
    
}