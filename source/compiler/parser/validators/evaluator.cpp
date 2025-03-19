#include <compiler/parser/validators/evaluator.hpp>

ExpressionEvaluator::
ExpressionEvaluator(Environment *environment)
    : environment(environment), evaluated_type(Datatype::DATA_TYPE_UNKNOWN)
{

}

ExpressionEvaluator::
ExpressionEvaluator(Environment *environment, Datatype initial_type)
    : environment(environment), evaluated_type(initial_type)
{

}

ExpressionEvaluator::
~ExpressionEvaluator()
{

}

Datatype ExpressionEvaluator::
operator()() const
{

    return this->evaluated_type;

}

void ExpressionEvaluator::
evaluate(Datatype type)
{
    
    // Our error cases are handled first.
    if (type == Datatype::DATA_TYPE_ERROR)
    {
        this->evaluated_type = Datatype::DATA_TYPE_ERROR;
        return;
    }

    else if (this->evaluated_type == Datatype::DATA_TYPE_ERROR)
        return;

    // The type hasn't been set yet, set it to the current type.
    else if (this->evaluated_type == Datatype::DATA_TYPE_UNKNOWN)
    {
        this->evaluated_type = type;
        return;
    }

    // The type has been set, but it's different from the current type, so we
    // need to promote the type if it is promotable.
    else if (type > this->evaluated_type)
        this->evaluated_type = type;

    else if (this->evaluated_type > Datatype::DATA_TYPE_UNKNOWN && type == Datatype::DATA_TYPE_STRING)
        this->evaluated_type = Datatype::DATA_TYPE_ERROR;
    

}

// --- Visitor Routines --------------------------------------------------------

void ExpressionEvaluator::
visit(SyntaxNodeEquality* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionEvaluator::
visit(SyntaxNodeComparison* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionEvaluator::
visit(SyntaxNodeTerm* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionEvaluator::
visit(SyntaxNodeFactor* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionEvaluator::
visit(SyntaxNodeMagnitude* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionEvaluator::
visit(SyntaxNodeExtraction* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionEvaluator::
visit(SyntaxNodeDerivation* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionEvaluator::
visit(SyntaxNodeUnary* node)
{

    node->expression->accept(this);

}

void ExpressionEvaluator::
visit(SyntaxNodeFunctionCall* node)
{

    Symbol *function_symbol = this->environment->get_symbol(node->identifier);
    SF_ENSURE_PTR(function_symbol);

    SyntaxNodeFunctionStatement *function_node = (SyntaxNodeFunctionStatement*)function_symbol->get_node();
    this->evaluate(function_node->variable_node->data_type);

}

void ExpressionEvaluator::
visit(SyntaxNodeArrayIndex* node)
{

    Symbol *array_symbol = this->environment->get_symbol(node->identifier);
    SF_ENSURE_PTR(array_symbol);
    SF_ASSERT(array_symbol->is_array());
    
    SyntaxNodeVariableStatement *array_node = (SyntaxNodeVariableStatement*)array_symbol->get_node();
    this->evaluate(array_node->data_type);
    
}
    

void ExpressionEvaluator::
visit(SyntaxNodePrimary* node)
{

    Datatype current_type = Datatype::DATA_TYPE_UNKNOWN;

    switch (node->primarytype)
    {
        case Primarytype::PRIMARY_TYPE_REAL: 
            current_type = Datatype::DATA_TYPE_REAL; break;
        case Primarytype::PRIMARY_TYPE_INTEGER: 
            current_type = Datatype::DATA_TYPE_INTEGER; break;
        case Primarytype::PRIMARY_TYPE_STRING: 
            current_type = Datatype::DATA_TYPE_STRING; break;
        case Primarytype::PRIMARY_TYPE_COMPLEX:
            current_type = Datatype::DATA_TYPE_COMPLEX; break;
        case Primarytype::PRIMARY_TYPE_IDENTIFIER:
        {
            Symbol *symbol = this->environment->get_symbol(node->primitive);
            SF_ENSURE_PTR(symbol);
            SyntaxNodeVariableStatement *variable_node = (SyntaxNodeVariableStatement*)symbol->get_node();
            current_type = variable_node->data_type;
            break;
        }
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    this->evaluate(current_type);

}

void ExpressionEvaluator::
visit(SyntaxNodeGrouping* node)
{

    node->expression->accept(this);

}
