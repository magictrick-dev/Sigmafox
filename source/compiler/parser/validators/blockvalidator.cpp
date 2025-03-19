#include <compiler/parser/validators/blockvalidator.hpp>
#include <compiler/parser/validators/evaluator.hpp>

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
visit(SyntaxNodeFunctionStatement* node)
{

    // TODO(Chris): There is some additional nesting that we need to do here
    //              since procedures can be defined internally.
      
    this->environment->push_table();
    for (auto child : node->children)
    {
        child->accept(this);
    }
    this->environment->pop_table();
    
}

void BlockValidator::
visit(SyntaxNodeProcedureStatement* node)
{

    // TODO(Chris): There is some additional nesting that we need to do here
    //              since procedures can be defined internally.

    this->environment->push_table();
    for (auto child : node->children)
    {
        child->accept(this);
    }
    this->environment->pop_table();
    
}

void BlockValidator::
visit(SyntaxNodeExpressionStatement* node)
{
    
    node->expression->accept(this);
    
}

void BlockValidator::
visit(SyntaxNodeWhileStatement* node)
{

    this->environment->push_table();
    for (auto child : node->children)
    {
        child->accept(this);
    }
    this->environment->pop_table();
    
}

void BlockValidator::
visit(SyntaxNodeLoopStatement* node)
{
    
    // TODO(Chris): There is some additional nesting that we need to do here
    //              since procedures can be defined internally.

    this->environment->push_table();
    for (auto child : node->children)
    {
        child->accept(this);
    }
    this->environment->pop_table();
    
}

void BlockValidator::
visit(SyntaxNodeVariableStatement* node)
{

    // First, if the variable has a given expression type, we need to
    // descend it, then we need evaluate the type.
    if (node->expression != nullptr)
    {

        node->expression->accept(this);

        ExpressionEvaluator expression_evaluator(this->environment);
        node->expression->accept(&expression_evaluator);

        node->data_type = expression_evaluator();
        
    }

    Symbol variable_symbol(node->identifier, Symboltype::SYMBOL_TYPE_VARIABLE, 
            node, node->dimensions.size());

    this->environment->set_symbol_locally(node->identifier, variable_symbol);


     
}

void BlockValidator::
visit(SyntaxNodeScopeStatement* node)
{

    this->environment->push_table();
    for (auto child : node->children)
    {
        child->accept(this);
    }
    this->environment->pop_table();
    
}

void BlockValidator::
visit(SyntaxNodeConditionalStatement* node)
{

    this->environment->push_table();
    for (auto child : node->children)
    {
        child->accept(this);
    }
    this->environment->pop_table();

    auto next_conditional = node->next;
    while (next_conditional != nullptr)
    {

        this->environment->push_table();
        for (auto child : next_conditional->children)
        {
            child->accept(this);
        }
        this->environment->pop_table();

        next_conditional = next_conditional->next;

    }
    
}

void BlockValidator::
visit(SyntaxNodeReadStatement* node)
{

    return; // No evaluation required.
    
}

void BlockValidator::
visit(SyntaxNodeWriteStatement* node)
{

    return; // No evaluation required.
    
}

void BlockValidator::
visit(SyntaxNodeExpression* node)
{
    
    node->expression->accept(this);
    
}

void BlockValidator::
visit(SyntaxNodeProcedureCall* node)
{

    auto procedure_node = (SyntaxNodeProcedureStatement*)
        this->environment->get_symbol(node->identifier)->get_node();

    // Evaluate the parameters.
    for (u64 idx = 0; idx < node->arguments.size(); ++idx)
    {

        ExpressionEvaluator expression_evaluator(this->environment);
        procedure_node->parameters[idx]->accept(&expression_evaluator);
        node->arguments[idx]->accept(&expression_evaluator);

        procedure_node->parameters[idx]->data_type = expression_evaluator();
    
    }

    // We need to push the scope, and then evaluate the function call deeper.
    // Setup is the same in the parser.
    this->environment->push_table();

    // Insert the return variable.
    this->environment->set_symbol_locally(procedure_node->variable_node->identifier, 
            Symbol(procedure_node->variable_node->identifier, 
            Symboltype::SYMBOL_TYPE_VARIABLE, procedure_node));

    // Parameters.
    for (auto parameter : procedure_node->parameters)
    {

        this->environment->set_symbol_locally(parameter->identifier, Symbol(parameter->identifier,
            Symboltype::SYMBOL_TYPE_VARIABLE, parameter));

    }

    procedure_node->accept(this);

    this->environment->pop_table();
    
}

void BlockValidator::
visit(SyntaxNodeAssignment* node)
{

    node->left->accept(this);
    node->right->accept(this);
    
    ExpressionEvaluator type_evaluator(this->environment);
    node->left->accept(&type_evaluator);
    node->right->accept(&type_evaluator);

    string identifier;
    Nodetype left_node_type = node->left->get_nodetype();
    switch (left_node_type)
    {

        case Nodetype::NODE_TYPE_PRIMARY:
        {
            SyntaxNodePrimary *primary = (SyntaxNodePrimary*)node->left;
            identifier = primary->primitive;
        } break;

        case Nodetype::NODE_TYPE_ARRAY_INDEX:
        {
            SyntaxNodeArrayIndex *array_index = (SyntaxNodeArrayIndex*)node->left;
            identifier = array_index->identifier;
        } break;

        default:
        {
            SF_ASSERT(!"Unimplemented node type conditional.");
        } break;

    }

    Symbol *symbol = this->environment->get_symbol(identifier);
    SF_ENSURE_PTR(symbol);

    SyntaxNodeVariableStatement *variable_node = (SyntaxNodeVariableStatement*)symbol->get_node();
    variable_node->data_type = type_evaluator();
    
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

    // Evaluate the parameters.
    for (u64 idx = 0; idx < node->arguments.size(); ++idx)
    {

        ExpressionEvaluator expression_evaluator(this->environment);
        function_node->parameters[idx]->accept(&expression_evaluator);
        node->arguments[idx]->accept(&expression_evaluator);

        function_node->parameters[idx]->data_type = expression_evaluator();
    
    }

    // We need to push the scope, and then evaluate the function call deeper.
    // Setup is the same in the parser.
    this->environment->push_table();

    // Insert the return variable.
    this->environment->set_symbol_locally(function_node->variable_node->identifier, 
            Symbol(function_node->variable_node->identifier, Symboltype::SYMBOL_TYPE_VARIABLE, function_node));

    // Parameters.
    for (auto parameter : function_node->parameters)
    {

        this->environment->set_symbol_locally(parameter->identifier, Symbol(parameter->identifier,
            Symboltype::SYMBOL_TYPE_VARIABLE, parameter));

    }

    function_node->accept(this);

    this->environment->pop_table();

}

void BlockValidator::
visit(SyntaxNodeArrayIndex* node)
{
    return;
}

void BlockValidator::
visit(SyntaxNodePrimary* node)
{
    return;
}

void BlockValidator::
visit(SyntaxNodeGrouping* node)
{
    
    node->expression->accept(this);
    
}
