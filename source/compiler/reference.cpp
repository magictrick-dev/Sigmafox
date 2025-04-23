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

    this->print_tabs();
    std::cout << "ROOT " << node->relative_base << std::endl;

    this->push_tabs();
    for (auto child : node->children) child->accept(this);
    this->pop_tabs();

    this->print_tabs();
    std::cout << "ENDROOT" << std::endl;

}

void ReferenceVisitor::
visit(SyntaxNodeModule* node)
{

    this->print_tabs();
    std::cout << "MODULE " << node->relative_path << std::endl;
    this->push_tabs();

    node->root->accept(this);

    this->pop_tabs();
    this->print_tabs();
    std::cout << "ENDMODULE" << std::endl;

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

    this->print_tabs();
    std::cout << "INCLUDE ";
    std::cout << node->relative_path << std::endl;

    if (node->module != nullptr)
    {
        node->module->accept(this);
    }

    return;

}

void ReferenceVisitor::
visit(SyntaxNodeFunctionStatement* node)
{

    string datatype_string = datatype_to_string(node->variable_node->data_type);
    string structuretype_string = structuretype_to_string(node->variable_node->structure_type);
    i32 structure_length = node->variable_node->structure_length;

    this->print_tabs();
    std::cout << "FUNCTION " << node->variable_node->identifier << " [TYPE: " 
        << datatype_string << ":" << structuretype_string << ":" << structure_length << "] ";

    for (auto parameter : node->parameters)
    {

        string datatype_string = datatype_to_string(parameter->data_type);
        string structuretype_string = structuretype_to_string(parameter->structure_type);
        i32 structure_length = parameter->structure_length;
        std::cout << parameter->identifier << " [TYPE: " << datatype_string << ":" << structuretype_string 
            << ":" << structure_length << "] ";

    }

    std::cout << std::endl;

    this->push_tabs();
    for (auto child : node->children)
    {
        child->accept(this);
    }
    this->pop_tabs();

    this->print_tabs();
    std::cout << "ENDFUNCTION" << std::endl;

}

void ReferenceVisitor::
visit(SyntaxNodeProcedureStatement* node)
{

    string datatype_string = datatype_to_string(node->variable_node->data_type);
    string structuretype_string = structuretype_to_string(node->variable_node->structure_type);
    i32 structure_length = node->variable_node->structure_length;

    this->print_tabs();
    std::cout << "PROCEDURE " << node->variable_node->identifier << " [TYPE: " 
        << datatype_string << ":" << structuretype_string << ":" << structure_length << "] ";

    for (auto parameter : node->parameters)
    {

        string datatype_string = datatype_to_string(parameter->data_type);
        string structuretype_string = structuretype_to_string(parameter->structure_type);
        i32 structure_length = parameter->structure_length;
        std::cout << parameter->identifier << " [TYPE: " << datatype_string << ":" << structuretype_string 
            << ":" << structure_length << "] ";

    }

    std::cout << std::endl;

    this->push_tabs();
    for (auto child : node->children)
    {
        child->accept(this);
    }
    this->pop_tabs();

    this->print_tabs();
    std::cout << "ENDFUNCTION" << std::endl;

}

void ReferenceVisitor::
visit(SyntaxNodeExpressionStatement* node)
{

    this->print_tabs();
    node->expression->accept(this);
    std::cout << ";" << std::endl;

}

void ReferenceVisitor::
visit(SyntaxNodeWhileStatement* node)
{
    
    this->print_tabs();
    std::cout << "WHILE ";
    node->expression->accept(this);
    std::cout << std::endl;
    
    this->push_tabs();
    for (auto child : node->children)
    {
        child->accept(this);
    }
    this->pop_tabs();
    
    this->print_tabs();
    std::cout << "ENDWHILE" << std::endl;
    
}

void ReferenceVisitor::
visit(SyntaxNodePloopStatement* node)
{

    this->print_tabs();
    std::cout << "PLOOP " << node->iterator << " ";
    node->start->accept(this);
    std::cout << " ";
    node->end->accept(this);
    std::cout << " ";
    node->step->accept(this);
    std::cout << std::endl;


    this->push_tabs();   
    node->variable->accept(this);
    for (auto child : node->children) 
    {
        child->accept(this);
    }
    this->pop_tabs();

    this->print_tabs();
    std::cout << "ENDPLOOP SHARING " << node->share_name << std::endl;

}

void ReferenceVisitor::
visit(SyntaxNodeLoopStatement* node)
{

    this->print_tabs();
    std::cout << "LOOP " << node->iterator << " ";
    node->start->accept(this);
    std::cout << " ";
    node->end->accept(this);
    std::cout << " ";
    node->step->accept(this);
    std::cout << std::endl;


    this->push_tabs();   
    node->variable->accept(this);
    for (auto child : node->children) 
    {
        child->accept(this);
    }
    this->pop_tabs();

    this->print_tabs();
    std::cout << "ENDLOOP" << std::endl;

}

void ReferenceVisitor::
visit(SyntaxNodeVariableStatement* node)
{

    string datatype_string = datatype_to_string(node->data_type);
    string structuretype_string = structuretype_to_string(node->structure_type);

    this->print_tabs();
    std::cout << "VARIABLE " << "[TYPE: " << datatype_string << ":" << structuretype_string
        << ":" << node->structure_length << "] " << node->identifier << " ";
    node->storage->accept(this);

    for (auto dimension : node->dimensions)
    {
        std::cout << " [";
        dimension->accept(this);
        std::cout << "]";
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
    
    this->print_tabs();
    std::cout << "SCOPE" << std::endl;

    this->push_tabs();
    for (auto child : node->children) child->accept(this);
    this->pop_tabs();

    this->print_tabs();
    std::cout << "ENDSCOPE" << std::endl;
    
}

void ReferenceVisitor::
visit(SyntaxNodeConditionalStatement* node)
{

    this->print_tabs();
    std::cout << "IF ";
    node->expression->accept(this);
    std::cout << std::endl;

    this->push_tabs();
    for (auto child : node->children)
    {

        child->accept(this);

    }
    this->pop_tabs();

    auto current_chain = node->next;
    while (current_chain != nullptr)
    {

        this->print_tabs();
        std::cout << "ELSEIF ";
        current_chain->expression->accept(this);
        std::cout << std::endl;

        this->push_tabs();
        for (auto child : current_chain->children)
        {
            child->accept(this);
        }
        this->pop_tabs();

        current_chain = current_chain->next;

    }

    this->print_tabs();
    std::cout << "ENDIF" << std::endl;

}

void ReferenceVisitor::
visit(SyntaxNodeReadStatement* node)
{

    this->print_tabs();
    std::cout << "READ ";
    node->location->accept(this);
    std::cout << " TO " << node->identifier << ";" << std::endl;



}

void ReferenceVisitor::
visit(SyntaxNodeWriteStatement* node)
{
    
    this->print_tabs();
    std::cout << "WRITE ";
    node->location->accept(this);

    for (auto current_node : node->expressions)
    {
        std::cout << " ";
        current_node->accept(this);
    }

    std::cout << ";" << std::endl;
    
}

void ReferenceVisitor::
visit(SyntaxNodeExpression* node)
{

    node->expression->accept(this);

}

void ReferenceVisitor::
visit(SyntaxNodeProcedureCall* node)
{

    std::cout << "PROCEDURE " << node->identifier << "(";

    for (i32 i = 0; i < node->arguments.size(); ++i)
    {
        node->arguments[i]->accept(this);
        if (i < node->arguments.size() - 1) std::cout << ", ";
    }

    std::cout << ")";

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
visit(SyntaxNodeConcatenation* node)
{

    node->left->accept(this);

    switch (node->operation)
    {
        case Operationtype::OPERATION_TYPE_CONCATENATE: std::cout << " & "; break;
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

    std::cout << "FUNCTION " << node->identifier << "(";

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

    std::cout << node->primitive;

}

void ReferenceVisitor::
visit(SyntaxNodeGrouping* node)
{

    std::cout << "(";
    node->expression->accept(this);
    std::cout << ")";

}
