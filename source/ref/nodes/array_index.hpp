#ifndef SIGMAFOX_COMPILER_NODES_ARRAY_INDEX_HPP
#define SIGMAFOX_COMPILER_NODES_ARRAY_INDEX_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeArrayIndex : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeArrayIndex();
        inline virtual         ~SyntaxNodeArrayIndex();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        std::string variable_name;
        std::vector<std::shared_ptr<ISyntaxNode>> indices;

};

SyntaxNodeArrayIndex::
SyntaxNodeArrayIndex()
{

    this->type = SyntaxNodeType::NodeTypeArrayIndex;

}

SyntaxNodeArrayIndex::
~SyntaxNodeArrayIndex()
{

}

void SyntaxNodeArrayIndex::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeArrayIndex(this);

}

#endif