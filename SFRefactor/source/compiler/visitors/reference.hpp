#ifndef SIGMAFOX_COMPILER_VISITORS_REFERENCE_HPP
#define SIGMAFOX_COMPILER_VISITORS_REFERENCE_HPP
#include <compiler/syntaxnode.hpp>
#include <compiler/nodes/include.hpp>
#include <compiler/nodes/main.hpp>
#include <compiler/nodes/module.hpp>
#include <compiler/nodes/root.hpp>
#include <compiler/nodes/expression_statement.hpp>
#include <compiler/nodes/assignment.hpp>
#include <compiler/nodes/equality.hpp>
class SyntaxNodeComparison;
class SyntaxNodeTerm;
class SyntaxNodeFactor;
class SyntaxNodeMagnitude;
class SyntaxNodeExtraction;
class SyntaxNodeDerivation;
class SyntaxNodeUnary;
class SyntaxNodeFunctionCall;
class SyntaxNodePrimary;
class SyntaxNodeGrouping;


class ReferenceVisitor : public ISyntaxNodeVisitor
{

    public:
        inline          ReferenceVisitor();
        inline          ReferenceVisitor(i32 tab_size);
        inline virtual ~ReferenceVisitor();

        inline virtual void visit_SyntaxNodeRoot(SyntaxNodeRoot *node)                  override;
        inline virtual void visit_SyntaxNodeModule(SyntaxNodeModule *node)              override;
        inline virtual void visit_SyntaxNodeInclude(SyntaxNodeInclude *node)            override;
        inline virtual void visit_SyntaxNodeMain(SyntaxNodeMain *node)                  override;
        inline virtual void visit_SyntaxNodeExpressionStatement(SyntaxNodeExpressionStatement *node) override; 
        inline virtual void visit_SyntaxNodeExpression(SyntaxNodeExpression *node)      override;     
        inline virtual void visit_SyntaxNodeAssignment(SyntaxNodeAssignment *node)      override;     
        inline virtual void visit_SyntaxNodeEquality(SyntaxNodeEquality *node)          override;         
        inline virtual void visit_SyntaxNodeComparison(SyntaxNodeComparison *node)      override;     
        inline virtual void visit_SyntaxNodeTerm(SyntaxNodeTerm *node)                  override;                 
        inline virtual void visit_SyntaxNodeFactor(SyntaxNodeFactor *node)              override;             
        inline virtual void visit_SyntaxNodeMagnitude(SyntaxNodeMagnitude *node)        override;       
        inline virtual void visit_SyntaxNodeExtraction(SyntaxNodeExtraction *node)      override;     
        inline virtual void visit_SyntaxNodeDerivation(SyntaxNodeDerivation *node)      override;     
        inline virtual void visit_SyntaxNodeUnary(SyntaxNodeUnary *node)                override;               
        inline virtual void visit_SyntaxNodeFunctionCall(SyntaxNodeFunctionCall *node)  override; 
        inline virtual void visit_SyntaxNodePrimary(SyntaxNodePrimary *node)            override;           

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
visit_SyntaxNodeExpressionStatement(SyntaxNodeExpressionStatement *node)
{

    SF_NO_IMPL(!"Not yet.");
    return;
}

void ReferenceVisitor::
visit_SyntaxNodeExpression(SyntaxNodeExpression *node)     
{

    SF_NO_IMPL(!"Not yet.");
    return;
}

void ReferenceVisitor::
visit_SyntaxNodeAssignment(SyntaxNodeAssignment *node)     
{

    SF_NO_IMPL(!"Not yet.");
    return;
}

void ReferenceVisitor::
visit_SyntaxNodeEquality(SyntaxNodeEquality *node)         
{

    SF_NO_IMPL(!"Not yet.");
    return;
}

void ReferenceVisitor::
visit_SyntaxNodeComparison(SyntaxNodeComparison *node)     
{

    SF_NO_IMPL(!"Not yet.");
    return;
}

void ReferenceVisitor::
visit_SyntaxNodeTerm(SyntaxNodeTerm *node)                 
{

    SF_NO_IMPL(!"Not yet.");
    return;
}

void ReferenceVisitor::
visit_SyntaxNodeFactor(SyntaxNodeFactor *node)             
{

    SF_NO_IMPL(!"Not yet.");
    return;
}

void ReferenceVisitor::
visit_SyntaxNodeMagnitude(SyntaxNodeMagnitude *node)       
{

    SF_NO_IMPL(!"Not yet.");
    return;
}

void ReferenceVisitor::
visit_SyntaxNodeExtraction(SyntaxNodeExtraction *node)     
{

    SF_NO_IMPL(!"Not yet.");
    return;
}

void ReferenceVisitor::
visit_SyntaxNodeDerivation(SyntaxNodeDerivation *node)     
{

    SF_NO_IMPL(!"Not yet.");
    return;
}

void ReferenceVisitor::
visit_SyntaxNodeUnary(SyntaxNodeUnary *node)               
{

    SF_NO_IMPL(!"Not yet.");
    return;
}

void ReferenceVisitor::
visit_SyntaxNodeFunctionCall(SyntaxNodeFunctionCall *node) 
{

    SF_NO_IMPL(!"Not yet.");
    return;
}

void ReferenceVisitor::
visit_SyntaxNodePrimary(SyntaxNodePrimary *node)           
{

    SF_NO_IMPL(!"Not yet.");
    return;
}

#endif
