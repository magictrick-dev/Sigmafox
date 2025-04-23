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

        node->data_type = expression_evaluator.get_data_type();
        node->structure_type = expression_evaluator.get_structure_type();
        node->structure_length = expression_evaluator.get_structure_length();
        
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

        procedure_node->parameters[idx]->data_type = expression_evaluator.get_data_type();
        procedure_node->parameters[idx]->structure_type = expression_evaluator.get_structure_type();
        procedure_node->parameters[idx]->structure_length = expression_evaluator.get_structure_length();
    
    }

    // We need to push the scope, and then evaluate the function call deeper.
    // Setup is the same in the parser.
    this->environment->push_table();

    // NOTE(Chris): Here lies my last bit of sanity--gone, but not forgotten.
    //              You should always remember to put VARIABLE NODES into the
    //              symbol table, and *NOTHING* else. Or you get WEIRD heap corruption
    //              errors that brick the entire state of the program.
    // Insert the return variable.
    this->environment->set_symbol_locally(procedure_node->variable_node->identifier, 
            Symbol(procedure_node->variable_node->identifier, 
            Symboltype::SYMBOL_TYPE_VARIABLE, procedure_node->variable_node));

    // Parameters.
    for (auto parameter : procedure_node->parameters)
    {

        this->environment->set_symbol_locally(parameter->identifier, Symbol(parameter->identifier,
            Symboltype::SYMBOL_TYPE_VARIABLE, parameter));

    }

    for (auto child : procedure_node->children)
    {
        child->accept(this);
    }

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
            SyntaxNodePrimary *primary = dynamic_cast<SyntaxNodePrimary*>(node->left);
            SF_ENSURE_PTR(primary);
            identifier = primary->primitive;
        } break;

        case Nodetype::NODE_TYPE_ARRAY_INDEX:
        {
            SyntaxNodeArrayIndex *array_index = dynamic_cast<SyntaxNodeArrayIndex*>(node->left);
            SF_ENSURE_PTR(array_index);
            identifier = array_index->identifier;
        } break;

        default:
        {
            SF_ASSERT(!"Unimplemented node type conditional.");
        } break;

    }

    Symbol *symbol = this->environment->get_symbol(identifier);
    SF_ENSURE_PTR(symbol);

    SyntaxNodeVariableStatement *variable_node = dynamic_cast<SyntaxNodeVariableStatement*>(symbol->get_node());
    SF_ENSURE_PTR(variable_node);
    variable_node->data_type = type_evaluator.get_data_type();
    variable_node->structure_type = type_evaluator.get_structure_type();
    variable_node->structure_length = type_evaluator.get_structure_length();
    
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

    // NOTE(Chris): WE SHOULD ALWAYS BE USING DYNAMIC_CAST.
    //              PERIOD.
    auto function_node = dynamic_cast<SyntaxNodeFunctionStatement*>(this->environment->
            get_symbol(node->identifier)->get_node());
    SF_ENSURE_PTR(function_node);

    // Evaluate the parameters.
    for (u64 idx = 0; idx < node->arguments.size(); ++idx)
    {

        // TODO(Chris): The following COSY doesn't evaluate right:
        //
        // The add numbers a parameter isn't resolving.
        //
        /*
        function add_numbers a b;

            variable result 4 := a + b;
            add_numbers := result;

        endfunction;

        begin;

            variable foo 4 := 8 + 16 - 26 + 10^2;
            variable bar 4 := 16.0 / (2 + 2) * (2^(1 + 1));
            variable baz 4 := 32 - 16 + 4.0 * 26.0i;

            write 4 foo bar baz "\n";
            variable user_input 4;
            read 5 user_input;
            write 4 user_input;

            scope;

                variable foo 4 := 8 + 16 - 26 + 10^2;
                variable bar 4 := 16.0 / (2 + 2) * (2^(1 + 1));
                variable baz 4 := 32 - 16 + 4.0 * 26.0i;

                write 4 foo bar baz "\n";
                variable user_input 4;
                read 5 user_input;
                write 4 user_input;

                variable idx 4 := 0;
                while idx <= 1024;
                
                    idx := idx + 1 * add_numbers(idx * 1.0i, idx);
                    write 4 idx;
            
                endwhile;
            
        */

        ExpressionEvaluator expression_evaluator(this->environment);
        function_node->parameters[idx]->accept(&expression_evaluator);
        node->arguments[idx]->accept(&expression_evaluator);
        function_node->parameters[idx]->data_type = expression_evaluator.get_data_type();
        function_node->parameters[idx]->structure_type = expression_evaluator.get_structure_type();
        function_node->parameters[idx]->structure_length = expression_evaluator.get_structure_length();
    }

    // We need to push the scope, and then evaluate the function call deeper.
    // Setup is the same in the parser.
    this->environment->push_table();

    // Insert the return variable.
    // NOTE(Chris): Here lies my last bit of sanity--gone, but not forgotten.
    //              You should always remember to put VARIABLE NODES into the
    //              symbol table, and *NOTHING* else. Or you get WEIRD heap corruption
    //              errors that brick the entire state of the program.
    this->environment->set_symbol_locally(function_node->variable_node->identifier, 
            Symbol(function_node->variable_node->identifier, Symboltype::SYMBOL_TYPE_VARIABLE, 
                function_node->variable_node));

    // Parameters.
    for (auto parameter : function_node->parameters)
    {

        this->environment->set_symbol_locally(parameter->identifier, Symbol(parameter->identifier,
            Symboltype::SYMBOL_TYPE_VARIABLE, parameter));

    }

    for (auto child : function_node->children)
    {
        child->accept(this);
    }

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
