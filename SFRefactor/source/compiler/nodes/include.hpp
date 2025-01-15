#ifndef SIGMAFOX_COMPILER_NODES_INCLUDE_HPP
#define SIGMAFOX_COMPILER_NODES_INCLUDE_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeInclude : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeInclude();
        inline virtual         ~SyntaxNodeInclude();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        std::string path;
        std::string user_path;
        shared_ptr<ISyntaxNode> module;

};

SyntaxNodeInclude::
SyntaxNodeInclude()
{

    this->type = SyntaxNodeType::NodeTypeInclude;

}

SyntaxNodeInclude::
~SyntaxNodeInclude()
{

}

void SyntaxNodeInclude::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeInclude(this);

}

#endif
