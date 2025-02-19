#include <compiler/parser/node.hpp>

SyntaxNode::
SyntaxNode()
{
    this->nodetype = Nodetype::NODE_TYPE_UNKNOWN;
    this->datatype = Datatype::DATA_TYPE_UNKNOWN;
}

SyntaxNode::
~SyntaxNode()
{

}

Nodetype SyntaxNode::
get_nodetype() const
{
    return this->nodetype;
}

Datatype SyntaxNode::
get_datatype() const
{
    return this->datatype;
}

void SyntaxNode::
set_datatype(Datatype type)
{
    this->datatype = type;
}