#include <compiler/parser/node.hpp>

SyntaxNode::
SyntaxNode()
{
    this->node_type = Nodetype::NODE_TYPE_UNKNOWN;
}

SyntaxNode::
~SyntaxNode()
{

}

Nodetype SyntaxNode::
type() const
{
    return this->node_type;
}
