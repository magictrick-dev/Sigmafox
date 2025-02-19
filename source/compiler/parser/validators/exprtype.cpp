#include <compiler/parser/validators/exprtype.hpp>
#include <compiler/parser/errorhandler.hpp>

ExpressionTypeVisitor::
ExpressionTypeVisitor(Environment *environment, string path)
{

    this->evaluated_type    = Datatype::DATA_TYPE_UNKNOWN;
    this->environment       = environment;
    this->path              = path;

}

ExpressionTypeVisitor::
~ExpressionTypeVisitor()
{

}

Datatype ExpressionTypeVisitor::
get_evaluated_type() const
{

    return this->evaluated_type;

}

void ExpressionTypeVisitor::
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

void ExpressionTypeVisitor::
visit(SyntaxNodeEquality* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionTypeVisitor::
visit(SyntaxNodeComparison* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionTypeVisitor::
visit(SyntaxNodeTerm* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionTypeVisitor::
visit(SyntaxNodeFactor* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionTypeVisitor::
visit(SyntaxNodeMagnitude* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionTypeVisitor::
visit(SyntaxNodeExtraction* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionTypeVisitor::
visit(SyntaxNodeDerivation* node)
{

    node->left->accept(this);
    node->right->accept(this);

}

void ExpressionTypeVisitor::
visit(SyntaxNodeUnary* node)
{

    node->expression->accept(this);

}

void ExpressionTypeVisitor::
visit(SyntaxNodeFunctionCall* node)
{

    // TODO(Chris): Functions require return type processing as well.
    //              For now, we won't process this case and will just return.

    SF_ASSERT(!"Functions require return type processing as well.");
    return;

}

void ExpressionTypeVisitor::
visit(SyntaxNodeArrayIndex* node)
{

    Symbol *array_symbol = this->environment->get_symbol(node->identifier);
    SF_ENSURE_PTR(array_symbol);
    SF_ASSERT(array_symbol->is_array());
    this->evaluate(array_symbol->get_node()->get_datatype());

}

void ExpressionTypeVisitor::
visit(SyntaxNodePrimary* node)
{

    Datatype current_type = Datatype::DATA_TYPE_UNKNOWN;

    switch (node->primary)
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
            current_type = symbol->get_node()->get_datatype();
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

void ExpressionTypeVisitor::
visit(SyntaxNodeGrouping* node)
{

    node->expression->accept(this);

}