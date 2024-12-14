#ifndef SIGMAFOX_COMPILER_NODES_MODULE_HPP
#define SIGMAFOX_COMPILER_NODES_MODULE_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeModule : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeModule();
        inline virtual         ~SyntaxNodeModule();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        std::vector<shared_ptr<ISyntaxNode>> globals;

};

SyntaxNodeModule::
SyntaxNodeModule()
{

    this->type = SyntaxNodeType::NodeTypeModule;

}

SyntaxNodeModule::
~SyntaxNodeModule()
{

}

void SyntaxNodeModule::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeModule(this);

}

#endif
