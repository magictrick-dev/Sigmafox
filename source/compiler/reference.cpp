#include <iostream>
#include <compiler/reference.hpp>

ReferenceVisitor::
ReferenceVisitor()
{
}

ReferenceVisitor::
ReferenceVisitor(i32 tab_size)
{
    this->tab_size = tab_size;
}

ReferenceVisitor::
~ReferenceVisitor()
{
}

void ReferenceVisitor::
print_tabs()
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";

}

void ReferenceVisitor::
push_tabs()
{
    this->tabs += this->tab_size;
}

void ReferenceVisitor::
pop_tabs()
{
    this->tabs -= this->tab_size;
    if (this->tabs < 0) this->tabs = 0;
}

// --- Visitors ----------------------------------------------------------------

void ReferenceVisitor::
visit(SyntaxNodeRoot* node)
{

    std::cout << "ROOT" << std::endl;

    this->push_tabs();
    for (auto child : node->children) child->accept(this);
    this->pop_tabs();

    std::cout << "ENDROOT" << std::endl;

}

void ReferenceVisitor::
visit(SyntaxNodeModule* node)
{
}

void ReferenceVisitor::
visit(SyntaxNodeMain* node)
{

    this->print_tabs();
    std::cout << "BEGIN" << std::endl;

    this->push_tabs();
    for (auto child : node->children) child->accept(this);
    this->pop_tabs();

    this->print_tabs();
    std::cout << "END" << std::endl;

}

void ReferenceVisitor::
visit(SyntaxNodeIncludeStatement* node)
{

}

void ReferenceVisitor::
visit(SyntaxNodeParameter* node)
{

    string datatype_string = datatype_to_string(node->get_datatype());
    std::cout << "PARAMETER " << "[TYPE: " << datatype_string << "] " << node->identifier;

}

void ReferenceVisitor::
visit(SyntaxNodeFunctionStatement* node)
{

    string datatype_string = datatype_to_string(node->get_datatype());

    this->print_tabs();
    std::cout << "FUNCTION [TYPE: " << datatype_string << "] "
        << node->identifier << " ";
    for (int i = 0; i < node->parameters.size(); ++i)
    {
        node->parameters[i]->accept(this);
        if (i < node->parameters.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;

    this->push_tabs();
    for (auto child : node->children) child->accept(this);
    this->pop_tabs();

    this->print_tabs();
    std::cout << "ENDFUNCTION" << std::endl;

}

void ReferenceVisitor::
visit(SyntaxNodeProcedureStatement* node)
{

    string datatype_string = datatype_to_string(node->get_datatype());

    this->print_tabs();
    std::cout << "PROCEDURE [TYPE: " << datatype_string << "] "
        << node->identifier << " ";
    for (int i = 0; i < node->parameters.size(); ++i)
    {
        node->parameters[i]->accept(this);
        if (i < node->parameters.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;

    this->push_tabs();
    for (auto child : node->children) child->accept(this);
    this->pop_tabs();

    this->print_tabs();
    std::cout << "ENDPROCEDURE" << std::endl;

}

void ReferenceVisitor::
visit(SyntaxNodeExpressionStatement* node)
{

    this->print_tabs();
    node->expression->accept(this);
    std::cout << ";" << std::endl;

}

void ReferenceVisitor::
visit(SyntaxNodeProcedureCallStatement* node)
{
}

void ReferenceVisitor::
visit(SyntaxNodeWhileStatement* node)
{
}

void ReferenceVisitor::
visit(SyntaxNodeLoopStatement* node)
{
}

void ReferenceVisitor::
visit(SyntaxNodeVariableStatement* node)
{

    string datatype_string;
    switch (node->get_datatype())
    {
        case Datatype::DATA_TYPE_VOID:      datatype_string = "VOID"; break;
        case Datatype::DATA_TYPE_STRING:    datatype_string = "STRING"; break;
        case Datatype::DATA_TYPE_INTEGER:   datatype_string = "INTEGER"; break;
        case Datatype::DATA_TYPE_REAL:      datatype_string = "REAL"; break;
        case Datatype::DATA_TYPE_COMPLEX:   datatype_string = "COMPLEX"; break;
        case Datatype::DATA_TYPE_UNKNOWN:   datatype_string = "UNKNOWN"; break;
        case Datatype::DATA_TYPE_ERROR:     datatype_string = "ERROR"; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    this->print_tabs();
    std::cout << "VARIABLE " << "[TYPE: " << datatype_string << "] "
        << node->identifier << " ";
    node->storage->accept(this);
    std::cout << " ";

    for (auto dimension : node->dimensions)
    {
        dimension->accept(this);
    }

    if (node->expression != nullptr)
    {
        std::cout << " = ";
        node->expression->accept(this);
    }

    std::cout << ";" << std::endl;

}

void ReferenceVisitor::
visit(SyntaxNodeScopeStatement* node)
{
}

void ReferenceVisitor::
visit(SyntaxNodeConditionalStatement* node)
{
}

void ReferenceVisitor::
visit(SyntaxNodeReadStatement* node)
{
}

void ReferenceVisitor::
visit(SyntaxNodeWriteStatement* node)
{
}

void ReferenceVisitor::
visit(SyntaxNodeExpression* node)
{

    node->expression->accept(this);

}

void ReferenceVisitor::
visit(SyntaxNodeAssignment* node)
{

    node->left->accept(this);
    std::cout << " = ";
    node->right->accept(this);

}

void ReferenceVisitor::
visit(SyntaxNodeEquality* node)
{

    node->left->accept(this);

    switch (node->operation)
    {
        case Operationtype::OPERATION_TYPE_EQUALS: std::cout << " == "; break;
        case Operationtype::OPERATION_TYPE_NOT_EQUALS: std::cout << " != "; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

}

void ReferenceVisitor::
visit(SyntaxNodeComparison* node)
{

    node->left->accept(this);

    switch (node->operation)
    {
        case Operationtype::OPERATION_TYPE_LESS_THAN: std::cout << " < "; break;
        case Operationtype::OPERATION_TYPE_LESS_THAN_OR_EQUAL: std::cout << " <= "; break;
        case Operationtype::OPERATION_TYPE_GREATER_THAN: std::cout << " > "; break;
        case Operationtype::OPERATION_TYPE_GREATER_THAN_OR_EQUAL: std::cout << " >= "; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

}

void ReferenceVisitor::
visit(SyntaxNodeTerm* node)
{

    node->left->accept(this);

    switch (node->operation)
    {
        case Operationtype::OPERATION_TYPE_ADDITION: std::cout << " + "; break;
        case Operationtype::OPERATION_TYPE_SUBTRACTION: std::cout << " - "; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

}

void ReferenceVisitor::
visit(SyntaxNodeFactor* node)
{

    node->left->accept(this);

    switch (node->operation)
    {
        case Operationtype::OPERATION_TYPE_MULTIPLICATION: std::cout << " * "; break;
        case Operationtype::OPERATION_TYPE_DIVISION: std::cout << " / "; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

}

void ReferenceVisitor::
visit(SyntaxNodeMagnitude* node)
{

    node->left->accept(this);

    switch (node->operation)
    {
        case Operationtype::OPERATION_TYPE_POWER: std::cout << " ^ "; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

}

void ReferenceVisitor::
visit(SyntaxNodeExtraction* node)
{

    node->left->accept(this);

    switch (node->operation)
    {
        case Operationtype::OPERATION_TYPE_EXTRACTION: std::cout << " | "; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

}

void ReferenceVisitor::
visit(SyntaxNodeDerivation* node)
{

    node->left->accept(this);

    switch (node->operation)
    {
        case Operationtype::OPERATION_TYPE_DERIVATION: std::cout << " % "; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

}

void ReferenceVisitor::
visit(SyntaxNodeUnary* node)
{

    switch (node->operation)
    {
        case Operationtype::OPERATION_TYPE_NEGATION: std::cout << "-"; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->expression->accept(this);

}

void ReferenceVisitor::
visit(SyntaxNodeFunctionCall* node)
{

    std::cout << node->identifier << "(";

    for (i32 i = 0; i < node->arguments.size(); ++i)
    {
        node->arguments[i]->accept(this);
        if (i < node->arguments.size() - 1) std::cout << ", ";
    }

    std::cout << ")";

}

void ReferenceVisitor::
visit(SyntaxNodeArrayIndex* node)
{

    std::cout << node->identifier << "[";

    for (i32 i = 0; i < node->indices.size(); ++i)
    {
        node->indices[i]->accept(this);
        if (i < node->indices.size() - 1) std::cout << ", ";
    }

    std::cout << "]";

}

void ReferenceVisitor::
visit(SyntaxNodePrimary* node)
{

    switch (node->primary)
    {
        case Primarytype::PRIMARY_TYPE_REAL: 
            std::cout << "REAL " << node->primitive; break;
        case Primarytype::PRIMARY_TYPE_INTEGER: 
            std::cout << "INTEGER " << node->primitive; break;
        case Primarytype::PRIMARY_TYPE_STRING: 
            std::cout << "STRING " << node->primitive; break;
        case Primarytype::PRIMARY_TYPE_IDENTIFIER:
            std::cout << "IDENTIFIER " << node->primitive; break;
        case Primarytype::PRIMARY_TYPE_COMPLEX:
            std::cout << "COMPLEX " << node->primitive; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

}

void ReferenceVisitor::
visit(SyntaxNodeGrouping* node)
{

    std::cout << "(";
    node->expression->accept(this);
    std::cout << ")";

}
