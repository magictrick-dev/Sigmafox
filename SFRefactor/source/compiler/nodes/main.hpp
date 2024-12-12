#ifndef SIGMAFOX_COMPILER_NODES_MAIN_HPP
#define SIGMAFOX_COMPILER_NODES_MAIN_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeMain : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeMain();
        inline virtual         ~SyntaxNodeMain();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        std::vector<shared_ptr<ISyntaxNode>> children;

};

SyntaxNodeMain::
SyntaxNodeMain()
{

    this->type = SyntaxNodeType::NodeTypeMain;

}

SyntaxNodeMain::
~SyntaxNodeMain()
{

}

void SyntaxNodeMain::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeMain(this);

}

#endif
