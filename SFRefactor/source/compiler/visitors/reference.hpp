#ifndef SIGMAFOX_COMPILER_VISITORS_REFERENCE_HPP
#define SIGMAFOX_COMPILER_VISITORS_REFERENCE_HPP
#include <compiler/syntaxvisitor.hpp>
#include <compiler/syntaxnodes.hpp>

class ReferenceVisitor : public ISyntaxNodeVisitor
{

    public:
        inline          ReferenceVisitor();
        inline          ReferenceVisitor(i32 tab_size);
        inline virtual ~ReferenceVisitor();

        inline virtual void visit_SyntaxNodeRoot(SyntaxNodeRoot *node) override;
        inline virtual void visit_SyntaxNodeModule(SyntaxNodeModule *node) override;
        inline virtual void visit_SyntaxNodeMain(SyntaxNodeMain *node) override;

        inline virtual void visit_SyntaxNodeInclude(SyntaxNodeInclude *node) override;
        inline virtual void visit_SyntaxNodeExpressionStatement(SyntaxNodeExpressionStatement *node) override; 
        inline virtual void visit_SyntaxNodeWhileStatement(SyntaxNodeWhileStatement *node) override;
        inline virtual void visit_SyntaxNodeLoopStatement(SyntaxNodeLoopStatement *node) override;
        inline virtual void visit_SyntaxNodeVariableStatement(SyntaxNodeVariableStatement *node) override; 
        inline virtual void visit_SyntaxNodeScopeStatement(SyntaxNodeScopeStatement *node) override;
        inline virtual void visit_SyntaxNodeFunctionStatement(SyntaxNodeFunctionStatement *node) override;
        inline virtual void visit_SyntaxNodeProcedureStatement(SyntaxNodeProcedureStatement *node) override;
        inline virtual void visit_SyntaxNodeIfStatement(SyntaxNodeIfStatement *node) override;
        inline virtual void visit_SyntaxNodeConditional(SyntaxNodeConditional * node) override;
        inline virtual void visit_SyntaxNodeReadStatement(SyntaxNodeReadStatement *node) override;
        inline virtual void visit_SyntaxNodeWriteStatement(SyntaxNodeWriteStatement *node) override;

        inline virtual void visit_SyntaxNodeExpression(SyntaxNodeExpression *node) override;     
        inline virtual void visit_SyntaxNodeProcedureCall(SyntaxNodeProcedureCall *node) override;
        inline virtual void visit_SyntaxNodeAssignment(SyntaxNodeAssignment *node) override;     
        inline virtual void visit_SyntaxNodeEquality(SyntaxNodeEquality *node) override;         
        inline virtual void visit_SyntaxNodeComparison(SyntaxNodeComparison *node) override;     
        inline virtual void visit_SyntaxNodeTerm(SyntaxNodeTerm *node) override;                 
        inline virtual void visit_SyntaxNodeFactor(SyntaxNodeFactor *node) override;             
        inline virtual void visit_SyntaxNodeMagnitude(SyntaxNodeMagnitude *node) override;       
        inline virtual void visit_SyntaxNodeExtraction(SyntaxNodeExtraction *node) override;     
        inline virtual void visit_SyntaxNodeDerivation(SyntaxNodeDerivation *node) override;     
        inline virtual void visit_SyntaxNodeUnary(SyntaxNodeUnary *node) override;               
        inline virtual void visit_SyntaxNodeFunctionCall(SyntaxNodeFunctionCall *node) override; 
        inline virtual void visit_SyntaxNodeArrayIndex(SyntaxNodeArrayIndex *node) override; 
        inline virtual void visit_SyntaxNodePrimary(SyntaxNodePrimary *node) override;           
        inline virtual void visit_SyntaxNodeGrouping(SyntaxNodeGrouping *node) override;

    protected:
        void    push_tabs();
        void    pop_tabs();

    protected:
        i32 tabs = 0;
        i32 tab_size = 4;

};

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
push_tabs()
{
    this->tabs += this->tab_size;
}

void ReferenceVisitor::
pop_tabs()
{
    this->tabs -= this->tab_size;
}

void ReferenceVisitor::
visit_SyntaxNodeRoot(SyntaxNodeRoot *node)
{

    std::cout << "BEGIN ROOT" << std::endl;

    this->push_tabs();
    for (auto global_node : node->globals) global_node->accept(this);
    node->main->accept(this);
    this->pop_tabs();

    std::cout << "END ROOT" << std::endl;

}

void ReferenceVisitor::
visit_SyntaxNodeModule(SyntaxNodeModule *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "BEGIN MODULE" << std::endl;

    this->push_tabs();
    for (auto global_node : node->globals) global_node->accept(this);
    this->pop_tabs();

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "END MODULE" << std::endl;

}

void ReferenceVisitor::
visit_SyntaxNodeInclude(SyntaxNodeInclude *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "INCLUDE " << node->path << std::endl;

    this->push_tabs();
    node->module->accept(this);
    this->pop_tabs();

}

void ReferenceVisitor::
visit_SyntaxNodeMain(SyntaxNodeMain *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "BEGIN MAIN" << std::endl;

    this->push_tabs();
    for (auto child_node : node->children) child_node->accept(this);
    this->pop_tabs();

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "END MAIN" << std::endl;

}

void ReferenceVisitor::
visit_SyntaxNodeReadStatement(SyntaxNodeReadStatement *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "READ ";

    node->unit->accept(this);

    std::cout << " " << node->identifier << ";" << std::endl;

}

void ReferenceVisitor::
visit_SyntaxNodeWriteStatement(SyntaxNodeWriteStatement *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "WRITE ";

    node->unit->accept(this);
    std::cout << " ";

    for (i32 i = 0; i < node->expressions.size(); ++i)
    {
        node->expressions[i]->accept(this);
    }

    std::cout << ";" << std::endl;

}

void ReferenceVisitor::
visit_SyntaxNodeProcedureStatement(SyntaxNodeProcedureStatement *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "PROCEDURE " << node->identifier_name << " ";

    for (i32 i = 0; i < node->parameters.size(); ++i)
    {
        std::cout << node->parameters[i];
        if (i != node->parameters.size() - 1) std::cout << ", ";
    }

    std::cout << std::endl;

    this->push_tabs();
    for (auto body_statement : node->body_statements) body_statement->accept(this);
    this->pop_tabs();

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "ENDPROCEDURE" << std::endl;

}

void ReferenceVisitor::
visit_SyntaxNodeFunctionStatement(SyntaxNodeFunctionStatement *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "FUNCTION " << node->identifier_name << " ";

    std::cout << "(";
    for (i32 i = 0; i < node->parameters.size(); ++i)
    {
        std::cout << node->parameters[i];
        if (i != node->parameters.size() - 1) std::cout << ", ";
    }
    std::cout << ")" << std::endl;

    this->push_tabs();
    for (auto body_statement : node->body_statements) body_statement->accept(this);
    this->pop_tabs();

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "ENDFUNCTION" << std::endl;

}

void ReferenceVisitor::
visit_SyntaxNodeIfStatement(SyntaxNodeIfStatement *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "IF ";
    node->conditional->accept(this);
    std::cout << std::endl;

    this->push_tabs();
    for (auto child_node : node->children) child_node->accept(this);
    this->pop_tabs();

    if (node->conditional_else != nullptr)
        node->conditional_else->accept(this);

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "ENDIF" << std::endl;

}

void ReferenceVisitor::
visit_SyntaxNodeConditional(SyntaxNodeConditional *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "ELSEIF ";
    node->condition->accept(this);
    std::cout << std::endl;

    this->push_tabs();
    for (auto child_node : node->children) child_node->accept(this);
    this->pop_tabs();

    // Follow the chain.
    if (node->conditional_else != nullptr)
        node->conditional_else->accept(this);

    return;

}

void ReferenceVisitor::
visit_SyntaxNodeWhileStatement(SyntaxNodeWhileStatement *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "WHILE ";
    node->condition->accept(this);
    std::cout << std::endl;

    this->push_tabs();
    for (auto child_node : node->children) child_node->accept(this);
    this->pop_tabs();

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "ENDWHILE" << std::endl;

}

void ReferenceVisitor::
visit_SyntaxNodeLoopStatement(SyntaxNodeLoopStatement *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "LOOP " << node->identifier << " FROM ";
    node->initial->accept(this);
    std::cout << " TO ";
    node->terminal->accept(this);
    if (node->step != nullptr)
    {
        std::cout << " STEP ";
        node->step->accept(this);
    }
    std::cout << std::endl;

    this->push_tabs();
    for (auto child_node : node->children) child_node->accept(this);
    this->pop_tabs();

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "ENDLOOP" << std::endl;

}

void ReferenceVisitor::
visit_SyntaxNodeExpressionStatement(SyntaxNodeExpressionStatement *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    node->expression->accept(this);
    std::cout << ";" << std::endl;

    return;
}

void ReferenceVisitor::
visit_SyntaxNodeVariableStatement(SyntaxNodeVariableStatement *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "VARIABLE " << node->variable_name;

    std::cout << " ";
    node->size->accept(this);

    for (auto dimension : node->dimensions)
    {
        std::cout << "[";
        dimension->accept(this);
        std::cout << "]";
    }

    if (node->right_hand_side != nullptr)
    {
        std::cout << " = ";
        node->right_hand_side->accept(this);
    }

    std::cout << ";" << std::endl;

    return;
}

void ReferenceVisitor::
visit_SyntaxNodeScopeStatement(SyntaxNodeScopeStatement *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "BEGIN SCOPE" << std::endl;

    this->push_tabs();
    for (auto child_node : node->children) child_node->accept(this);
    this->pop_tabs();

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << "END SCOPE" << std::endl;

    return;

}

void ReferenceVisitor::
visit_SyntaxNodeExpression(SyntaxNodeExpression *node)     
{

    node->expression->accept(this);
    return;

}

void ReferenceVisitor::
visit_SyntaxNodeProcedureCall(SyntaxNodeProcedureCall *node)
{

    std::cout << "CALL " << node->procedure_name << " ";

    std::cout << "(";
    for (i32 i = 0; i < node->parameters.size(); ++i)
    {
        node->parameters[i]->accept(this);
        if (i != node->parameters.size() - 1) std::cout << ", ";
    }
    std::cout << ")";

    return;

}


void ReferenceVisitor::
visit_SyntaxNodeAssignment(SyntaxNodeAssignment *node)     
{

    node->left->accept(this);
    std::cout << " = ";
    node->right->accept(this);

    return;
}

void ReferenceVisitor::
visit_SyntaxNodeEquality(SyntaxNodeEquality *node)         
{

    node->left->accept(this);
    
    switch (node->operation_type)
    {
        case TokenType::TOKEN_EQUALS:   std::cout << " == ";     break;
        case TokenType::TOKEN_HASH:     std::cout << " != ";    break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

    return;
}

void ReferenceVisitor::
visit_SyntaxNodeComparison(SyntaxNodeComparison *node)     
{

    node->left->accept(this);

    switch (node->operation_type)
    {
        case TokenType::TOKEN_LESS_THAN:             std::cout << " < ";     break;
        case TokenType::TOKEN_LESS_THAN_EQUALS:      std::cout << " <= ";    break;
        case TokenType::TOKEN_GREATER_THAN:          std::cout << " > ";     break;
        case TokenType::TOKEN_GREATER_THAN_EQUALS:   std::cout << " >= ";    break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

    return;
}

void ReferenceVisitor::
visit_SyntaxNodeTerm(SyntaxNodeTerm *node)                 
{

    node->left->accept(this);

    switch (node->operation_type)
    {
        case TokenType::TOKEN_PLUS:     std::cout << " + ";     break;
        case TokenType::TOKEN_MINUS:    std::cout << " - ";     break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

    return;
}

void ReferenceVisitor::
visit_SyntaxNodeFactor(SyntaxNodeFactor *node)             
{

    node->left->accept(this);

    switch (node->operation_type)
    {
        case TokenType::TOKEN_STAR:             std::cout << " * ";     break;
        case TokenType::TOKEN_FORWARD_SLASH:    std::cout << " / ";     break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

    return;

}

void ReferenceVisitor::
visit_SyntaxNodeMagnitude(SyntaxNodeMagnitude *node)       
{

    node->left->accept(this);

    switch (node->operation_type)
    {
        case TokenType::TOKEN_CARROT:   std::cout << " ^ ";     break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);
    return;

}

void ReferenceVisitor::
visit_SyntaxNodeExtraction(SyntaxNodeExtraction *node)     
{

    node->left->accept(this);

    switch (node->operation_type)
    {
        case TokenType::TOKEN_PIPE:  std::cout << " | ";     break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

}

void ReferenceVisitor::
visit_SyntaxNodeDerivation(SyntaxNodeDerivation *node)     
{

    node->left->accept(this);

    switch (node->operation_type)
    {
        case TokenType::TOKEN_PERCENT: std::cout << " % ";     break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

    return;
}

void ReferenceVisitor::
visit_SyntaxNodeUnary(SyntaxNodeUnary *node)               
{

    switch (node->operation_type)
    {
        case TokenType::TOKEN_MINUS: std::cout << "-"; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    node->right->accept(this);

    return;
}

void ReferenceVisitor::
visit_SyntaxNodeFunctionCall(SyntaxNodeFunctionCall *node) 
{

    std::cout << node->function_name;
    std::cout << "(";
    for (i32 i = 0; i < node->parameters.size(); ++i)
    {
        node->parameters[i]->accept(this);
        if (i != node->parameters.size() - 1) std::cout << ", ";
    }
    std::cout << ")";

    return;

}

void ReferenceVisitor::
visit_SyntaxNodeArrayIndex(SyntaxNodeArrayIndex *node)     
{

    std::cout << node->variable_name;
    std::cout << "(";
    for (i32 i = 0; i < node->indices.size(); ++i)
    {
        node->indices[i]->accept(this);
        if (i != node->indices.size() - 1) std::cout << ", ";
    }
    std::cout << ")";

    return;

}

void ReferenceVisitor::
visit_SyntaxNodePrimary(SyntaxNodePrimary *node)           
{

    std::cout << node->literal_reference;
    return;
}

void ReferenceVisitor::
visit_SyntaxNodeGrouping(SyntaxNodeGrouping *node)           
{

    std::cout << "(";
    node->grouping->accept(this);
    std::cout << ")";

    return;

}

#endif
