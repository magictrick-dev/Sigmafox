#include <compiler/parser/validators/evaluator.hpp>
#include <compiler/exceptions.hpp>

ExpressionEvaluator::
ExpressionEvaluator(Environment *environment): 
    environment(environment), 
    evaluated_type(Datatype::DATA_TYPE_UNKNOWN),
    structure_type(Structuretype::STRUCTURE_TYPE_UNKNOWN),
    structure_length(1)
{

}

ExpressionEvaluator::
ExpressionEvaluator(Environment *environment, Datatype initial_type): 
    environment(environment), 
    evaluated_type(initial_type),
    structure_type(Structuretype::STRUCTURE_TYPE_UNKNOWN),
    structure_length(1)
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

Structuretype ExpressionEvaluator::
get_structure_type() const
{

    return this->structure_type;

}

i32 ExpressionEvaluator::
get_structure_length() const
{

    return this->structure_length;

}

Datatype ExpressionEvaluator::
get_data_type() const
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
    Structuretype left_structure_type = this->structure_type;
    i32 left_structure_length = this->structure_length;

    node->right->accept(this);
    Structuretype right_structure_type = this->structure_type;
    i32 right_structure_length = this->structure_length;

    if (left_structure_length != right_structure_length)
    {
        throw CompilerEvaluatorError(__LINE__,
                "Vector length mismatch, left is %i, right is %i.",
                left_structure_length,
                right_structure_length);
    }

    if (left_structure_type != right_structure_type)
    {

        throw CompilerEvaluatorError(__LINE__, "Structure type mismatch.");

    }

}

void ExpressionEvaluator::
visit(SyntaxNodeComparison* node)
{

    node->left->accept(this);
    Structuretype left_structure_type = this->structure_type;
    i32 left_structure_length = this->structure_length;

    node->right->accept(this);
    Structuretype right_structure_type = this->structure_type;
    i32 right_structure_length = this->structure_length;

    if (left_structure_length != right_structure_length)
    {
        throw CompilerEvaluatorError(__LINE__,
                "Vector length mismatch, left is %i, right is %i.",
                left_structure_length,
                right_structure_length);
    }

    if (left_structure_type != right_structure_type)
    {

        throw CompilerEvaluatorError(__LINE__, "Structure type mismatch.");

    }

}

void ExpressionEvaluator::
visit(SyntaxNodeConcatenation* node)
{

    node->left->accept(this);
    Datatype left_data_type = this->evaluated_type;
    Structuretype left_structure_type = this->structure_type;
    i32 left_structure_length = this->structure_length;

    node->right->accept(this);
    Datatype right_data_type = this->evaluated_type;
    Structuretype right_structure_type = this->structure_type;
    i32 right_structure_length = this->structure_length;

    // Left concatenate is a string cast.
    if (left_structure_type == Structuretype::STRUCTURE_TYPE_STRING)
    {
        
        this->structure_length = 1;
        this->structure_type = Structuretype::STRUCTURE_TYPE_STRING;

    }

    // Right concatenate string must have a left string.
    else if (right_structure_type == Structuretype::STRUCTURE_TYPE_STRING &&
            left_structure_type != right_structure_type)
    {

        throw CompilerEvaluatorError(__LINE__, "Structure type mismatch, unable to concatenate.");

    }

    else if (left_data_type == Datatype::DATA_TYPE_COMPLEX || right_data_type == Datatype::DATA_TYPE_COMPLEX)
    {

        throw CompilerEvaluatorError(__LINE__, "Complex vectors are not within specification...");

    }

    // Otherwise we're ensuring that the concatenate works (integers and floats).
    else
    {

        this->structure_length = left_structure_length + right_structure_length;
        this->structure_type = Structuretype::STRUCTURE_TYPE_VECTOR;

    }

}

void ExpressionEvaluator::
visit(SyntaxNodeTerm* node)
{

    node->left->accept(this);
    Structuretype left_structure_type = this->structure_type;
    i32 left_structure_length = this->structure_length;

    node->right->accept(this);
    Structuretype right_structure_type = this->structure_type;
    i32 right_structure_length = this->structure_length;

    if (left_structure_length != right_structure_length)
    {
        throw CompilerEvaluatorError(__LINE__,
                "Vector length mismatch, left is %i, right is %i.",
                left_structure_length,
                right_structure_length);
    }

    if (left_structure_type != right_structure_type)
    {

        throw CompilerEvaluatorError(__LINE__, "Structure type mismatch.");

    }

}

void ExpressionEvaluator::
visit(SyntaxNodeFactor* node)
{

    node->left->accept(this);
    Structuretype left_structure_type = this->structure_type;
    i32 left_structure_length = this->structure_length;

    node->right->accept(this);
    Structuretype right_structure_type = this->structure_type;
    i32 right_structure_length = this->structure_length;

}

void ExpressionEvaluator::
visit(SyntaxNodeMagnitude* node)
{

    node->left->accept(this);
    Structuretype left_structure_type = this->structure_type;
    i32 left_structure_length = this->structure_length;

    node->right->accept(this);
    Structuretype right_structure_type = this->structure_type;
    i32 right_structure_length = this->structure_length;

    if (left_structure_length != right_structure_length)
    {
        throw CompilerEvaluatorError(__LINE__,
                "Vector length mismatch, left is %i, right is %i.",
                left_structure_length,
                right_structure_length);
    }

    if (left_structure_type != right_structure_type)
    {

        throw CompilerEvaluatorError(__LINE__, "Structure type mismatch.");

    }

}

void ExpressionEvaluator::
visit(SyntaxNodeExtraction* node)
{

    node->left->accept(this);
    Structuretype left_structure_type = this->structure_type;
    i32 left_structure_length = this->structure_length;

    node->right->accept(this);
    Structuretype right_structure_type = this->structure_type;
    i32 right_structure_length = this->structure_length;

}

void ExpressionEvaluator::
visit(SyntaxNodeDerivation* node)
{

    node->left->accept(this);
    Structuretype left_structure_type = this->structure_type;
    i32 left_structure_length = this->structure_length;

    node->right->accept(this);
    Structuretype right_structure_type = this->structure_type;
    i32 right_structure_length = this->structure_length;

    if (left_structure_length != right_structure_length)
    {
        throw CompilerEvaluatorError(__LINE__,
                "Vector length mismatch, left is %i, right is %i.",
                left_structure_length,
                right_structure_length);
    }

    if (left_structure_type != right_structure_type)
    {

        throw CompilerEvaluatorError(__LINE__, "Structure type mismatch.");

    }

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
    this->structure_type = function_node->variable_node->structure_type;
    this->structure_length = function_node->variable_node->structure_length;

}

void ExpressionEvaluator::
visit(SyntaxNodeArrayIndex* node)
{

    Symbol *array_symbol = this->environment->get_symbol(node->identifier);
    SF_ENSURE_PTR(array_symbol);
    SF_ASSERT(array_symbol->is_array());
    
    SyntaxNodeVariableStatement *array_node = (SyntaxNodeVariableStatement*)array_symbol->get_node();
    this->evaluate(array_node->data_type);
    this->structure_type = array_node->structure_type;
    this->structure_length = array_node->structure_length;
    
}
    

void ExpressionEvaluator::
visit(SyntaxNodePrimary* node)
{

    Datatype current_type = Datatype::DATA_TYPE_UNKNOWN;

    switch (node->primarytype)
    {

        case Primarytype::PRIMARY_TYPE_REAL: 
        {

            current_type = Datatype::DATA_TYPE_REAL; 
            this->structure_length = 1;
            this->structure_type = Structuretype::STRUCTURE_TYPE_SCALAR;

        } break;

        case Primarytype::PRIMARY_TYPE_INTEGER: 
        {

            current_type = Datatype::DATA_TYPE_INTEGER; 
            this->structure_length = 1;
            this->structure_type = Structuretype::STRUCTURE_TYPE_SCALAR;

        } break;

        case Primarytype::PRIMARY_TYPE_COMPLEX:
        {

            current_type = Datatype::DATA_TYPE_COMPLEX; 
            this->structure_type = Structuretype::STRUCTURE_TYPE_SCALAR;
            this->structure_length = 1;

        } break;

        case Primarytype::PRIMARY_TYPE_STRING: 
        {

            current_type = Datatype::DATA_TYPE_STRING; 
            this->structure_type = Structuretype::STRUCTURE_TYPE_STRING;
            this->structure_length = 1;

        } break;

        case Primarytype::PRIMARY_TYPE_IDENTIFIER:
        {

            Symbol *symbol = this->environment->get_symbol(node->primitive);
            SF_ENSURE_PTR(symbol);
            SyntaxNodeVariableStatement *variable_node = (SyntaxNodeVariableStatement*)symbol->get_node();
            current_type = variable_node->data_type;

            this->structure_length  = variable_node->structure_length;
            this->structure_type    = variable_node->structure_type;
            
        } break;

        default:
        {

            SF_ASSERT(!"Unreachable condition.");
            
        } break;

    }

    this->evaluate(current_type);

}

void ExpressionEvaluator::
visit(SyntaxNodeGrouping* node)
{

    node->expression->accept(this);

}

