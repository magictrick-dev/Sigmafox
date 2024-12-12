#ifndef SIGMAFOX_COMPILER_VISITORS_REFERENCE_HPP
#define SIGMAFOX_COMPILER_VISITORS_REFERENCE_HPP
#include <compiler/syntaxnode.hpp>
#include <compiler/nodes/include.hpp>
#include <compiler/nodes/main.hpp>
#include <compiler/nodes/module.hpp>
#include <compiler/nodes/root.hpp>

class ReferenceVisitor : public ISyntaxNodeVisitor
{

    public:
        inline          ReferenceVisitor();
        inline          ReferenceVisitor(i32 tab_size);
        inline virtual ~ReferenceVisitor();

        inline virtual void visit_SyntaxNodeRoot(SyntaxNodeRoot *node)              override;
        inline virtual void visit_SyntaxNodeModule(SyntaxNodeModule *node)          override;
        inline virtual void visit_SyntaxNodeInclude(SyntaxNodeInclude *node)        override;
        inline virtual void visit_SyntaxNodeMain(SyntaxNodeMain *node)              override;

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
visit_SyntaxNodeRoot(SyntaxNodeRoot *node)
{

    std::cout << "ROOT" << std::endl;
    for (auto global_node : node->globals) global_node->accept(this);
    node->main->accept(this);

}

void ReferenceVisitor::
visit_SyntaxNodeModule(SyntaxNodeModule *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << " └─> MODULEBEGIN" << std::endl;

    this->tabs += this->tab_size;
    for (auto global_node : node->globals) global_node->accept(this);
    this->tabs -= this->tab_size;

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << " └─> MODULEEND" << std::endl;

}

void ReferenceVisitor::
visit_SyntaxNodeInclude(SyntaxNodeInclude *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << " └─> INCLUDE " << node->path << std::endl;

    this->tabs += this->tab_size;
    node->module->accept(this);
    this->tabs -= this->tab_size;

}

void ReferenceVisitor::
visit_SyntaxNodeMain(SyntaxNodeMain *node)
{

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << " └─> BEGIN" << std::endl;

    this->tabs += this->tab_size;
    for (auto child_node : node->children) child_node->accept(this);
    this->tabs -= this->tab_size;

    for (i32 i = 0; i < this->tabs; ++i) std::cout << " ";
    std::cout << " └─> END" << std::endl;

}

#endif
