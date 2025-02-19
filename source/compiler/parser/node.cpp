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


string 
nodetype_to_string(Nodetype type)
{

    string result;
    switch (type)
    {
        case Nodetype::NODE_TYPE_UNKNOWN:                   result = "UNKNOWN"; break;
        case Nodetype::NODE_TYPE_ROOT:                      result = "ROOT"; break;
        case Nodetype::NODE_TYPE_MODULE:                    result = "MODULE"; break;
        case Nodetype::NODE_TYPE_MAIN:                      result = "MAIN"; break;
        case Nodetype::NODE_TYPE_INCLUDE_STATEMENT:         result = "INCLUDE_STATEMENT"; break;
        case Nodetype::NODE_TYPE_PARAMETER:                 result = "PARAMETER"; break;
        case Nodetype::NODE_TYPE_VARIABLE_STATEMENT:        result = "VARIABLE_STATEMENT"; break;
        case Nodetype::NODE_TYPE_FUNCTION_STATEMENT:        result = "FUNCTION_STATEMENT"; break;
        case Nodetype::NODE_TYPE_PROCEDURE_STATEMENT:       result = "PROCEDURE_STATEMENT"; break;
        case Nodetype::NODE_TYPE_EXPRESSION_STATEMENT:      result = "EXPRESSION_STATEMENT"; break;
        case Nodetype::NODE_TYPE_WHILE_STATEMENT:           result = "WHILE_STATEMENT"; break;
        case Nodetype::NODE_TYPE_LOOP_STATEMENT:            result = "LOOP_STATEMENT"; break;
        case Nodetype::NODE_TYPE_SCOPE_STATEMENT:           result = "SCOPE_STATEMENT"; break;
        case Nodetype::NODE_TYPE_CONDITIONAL_STATEMENT:     result = "CONDITIONAL_STATEMENT"; break;
        case Nodetype::NODE_TYPE_READ_STATEMENT:            result = "READ_STATEMENT"; break;
        case Nodetype::NODE_TYPE_WRITE_STATEMENT:           result = "WRITE_STATEMENT"; break;
        case Nodetype::NODE_TYPE_PROCEDURE_CALL_STATEMENT:  result = "PROCEDURE_CALL_STATEMENT"; break;
        case Nodetype::NODE_TYPE_EXPRESSION:                result = "EXPRESSION"; break;
        case Nodetype::NODE_TYPE_ASSIGNMENT:                result = "ASSIGNMENT"; break;
        case Nodetype::NODE_TYPE_EQUALITY:                  result = "EQUALITY"; break;
        case Nodetype::NODE_TYPE_COMPARISON:                result = "COMPARISON"; break;
        case Nodetype::NODE_TYPE_TERM:                      result = "TERM"; break;
        case Nodetype::NODE_TYPE_FACTOR:                    result = "FACTOR"; break;
        case Nodetype::NODE_TYPE_MAGNITUDE:                 result = "MAGNITUDE"; break;
        case Nodetype::NODE_TYPE_EXTRACTION:                result = "EXTRACTION"; break;
        case Nodetype::NODE_TYPE_DERIVATION:                result = "DERIVATION"; break;
        case Nodetype::NODE_TYPE_UNARY:                     result = "UNARY"; break;
        case Nodetype::NODE_TYPE_FUNCTION_CALL:             result = "FUNCTION_CALL"; break;
        case Nodetype::NODE_TYPE_ARRAY_INDEX:               result = "ARRAY_INDEX"; break;
        case Nodetype::NODE_TYPE_PRIMARY:                   result = "PRIMARY"; break;
        case Nodetype::NODE_TYPE_GROUPING:                  result = "GROUPING"; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    return result;

}

string 
operationtype_to_string(Operationtype type)
{

    string result;
    switch (type)
    {
        case Operationtype::OPERATION_TYPE_UNKNOWN:                 result = "UNKNOWN"; break;
        case Operationtype::OPERATION_TYPE_ADDITION:                result = "ADDITION"; break;
        case Operationtype::OPERATION_TYPE_SUBTRACTION:             result = "SUBTRACTION"; break;
        case Operationtype::OPERATION_TYPE_MULTIPLICATION:          result = "MULTIPLICATION"; break;
        case Operationtype::OPERATION_TYPE_DIVISION:                result = "DIVISION"; break;
        case Operationtype::OPERATION_TYPE_POWER:                   result = "POWER"; break;
        case Operationtype::OPERATION_TYPE_ROOT:                    result = "ROOT"; break;
        case Operationtype::OPERATION_TYPE_EXTRACTION:              result = "EXTRACTION"; break;
        case Operationtype::OPERATION_TYPE_DERIVATION:              result = "DERIVATION"; break;
        case Operationtype::OPERATION_TYPE_EQUALS:                  result = "EQUALS"; break;
        case Operationtype::OPERATION_TYPE_NOT_EQUALS:              result = "NOT_EQUALS"; break;
        case Operationtype::OPERATION_TYPE_LESS_THAN:               result = "LESS_THAN"; break;
        case Operationtype::OPERATION_TYPE_LESS_THAN_OR_EQUAL:      result = "LESS_THAN_OR_EQUAL"; break;
        case Operationtype::OPERATION_TYPE_GREATER_THAN:            result = "GREATER_THAN"; break;
        case Operationtype::OPERATION_TYPE_GREATER_THAN_OR_EQUAL:   result = "GREATER_THAN_OR_EQUAL"; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    return result;

}

string 
primarytype_to_string(Primarytype type)
{

    string result;
    switch (type)
    {
        case Primarytype::PRIMARY_TYPE_UNKNOWN:       result = "UNKNOWN"; break;
        case Primarytype::PRIMARY_TYPE_INTEGER:       result = "INTEGER"; break;
        case Primarytype::PRIMARY_TYPE_REAL:          result = "REAL"; break;
        case Primarytype::PRIMARY_TYPE_COMPLEX:       result = "COMPLEX"; break;
        case Primarytype::PRIMARY_TYPE_STRING:        result = "STRING"; break;
        case Primarytype::PRIMARY_TYPE_IDENTIFIER:    result = "IDENTIFIER"; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

    return result;

}

string 
datatype_to_string(Datatype type)
{

    string result;
    switch (type)
    {
        case Datatype::DATA_TYPE_ERROR:     result = "ERROR"; break;
        case Datatype::DATA_TYPE_VOID:      result = "VOID"; break;
        case Datatype::DATA_TYPE_STRING:    result = "STRING"; break;
        case Datatype::DATA_TYPE_UNKNOWN:   result = "UNKNOWN"; break;
        case Datatype::DATA_TYPE_INTEGER:   result = "INTEGER"; break;
        case Datatype::DATA_TYPE_REAL:      result = "REAL"; break;
        case Datatype::DATA_TYPE_COMPLEX:   result = "COMPLEX"; break;
        default:
        {
            SF_ASSERT(!"Unreachable condition.");
            break;
        }
    }

}
