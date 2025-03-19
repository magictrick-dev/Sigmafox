#include <compiler/visitors/assignments.hpp>

AssignmentsVisitor::
AssignmentsVisitor(SymboltableStack<Symbol> *symbol_stack) 
    : symbol_stack(symbol_stack)
{


}

AssignmentsVisitor::
~AssignmentsVisitor()
{


}

bool AssignmentsVisitor::
is_valid_expression() const
{

    return this->valid_expression;

}

Symbolclass AssignmentsVisitor::
get_highest_type() const
{

    return this->highest_type;

}

void AssignmentsVisitor::
visit_SyntaxNodeExpression(SyntaxNodeExpression *node)
{

    node->expression->accept(this);

}   

void AssignmentsVisitor::
visit_SyntaxNodeAssignment(SyntaxNodeAssignment *node)
{

    node->left->accept(this);

}
     
void AssignmentsVisitor::
visit_SyntaxNodeEquality(SyntaxNodeEquality *node)
{

    node->left->accept(this);
    node->right->accept(this);

}
         
void AssignmentsVisitor::
visit_SyntaxNodeComparison(SyntaxNodeComparison *node)
{

    node->left->accept(this);
    node->right->accept(this);

}
     
void AssignmentsVisitor::
visit_SyntaxNodeTerm(SyntaxNodeTerm *node)
{

    node->left->accept(this);
    node->right->accept(this);

}
                 
void AssignmentsVisitor::
visit_SyntaxNodeFactor(SyntaxNodeFactor *node)
{

    node->left->accept(this);
    node->right->accept(this);

}
             
void AssignmentsVisitor::
visit_SyntaxNodeMagnitude(SyntaxNodeMagnitude *node)
{

    node->left->accept(this);
    node->right->accept(this);

}        
       
void AssignmentsVisitor::
visit_SyntaxNodeExtraction(SyntaxNodeExtraction *node)
{

    node->left->accept(this);
    node->right->accept(this);

}
     
void AssignmentsVisitor::
visit_SyntaxNodeDerivation(SyntaxNodeDerivation *node)
{

    node->left->accept(this);
    node->right->accept(this);

}
     
void AssignmentsVisitor::
visit_SyntaxNodeUnary(SyntaxNodeUnary *node)
{

    node->right->accept(this);

}
               
void AssignmentsVisitor::
visit_SyntaxNodeFunctionCall(SyntaxNodeFunctionCall *node)
{

    // TODO(Chris): Implement this. Functions should be able to validate their
    //              return types as well.
    this->valid_expression = false;

}
 
void AssignmentsVisitor::
visit_SyntaxNodeArrayIndex(SyntaxNodeArrayIndex *node)
{

    // Get the symbol from the symbol table.
    Symbol *symbol = this->symbol_stack->get_symbol(node->variable_name);
    if (symbol == nullptr)
    {
        this->valid_expression = false;
        return;
    }

    // Check the symbol type.
    if (symbol->classification == Symbolclass::SYMBOL_CLASS_STRING
        && this->highest_type != Symbolclass::SYMBOL_CLASS_STRING)
    {
        this->valid_expression = false;
        return;
    }

    if (symbol->classification == Symbolclass::SYMBOL_CLASS_INTEGER
        && this->highest_type != Symbolclass::SYMBOL_CLASS_STRING)
    {
        if (this->highest_type <= Symbolclass::SYMBOL_CLASS_INTEGER)
        {
            this->highest_type = Symbolclass::SYMBOL_CLASS_INTEGER;
            return;
        }
    }
    else
    {
        this->valid_expression = false;
        return;
    }

    if (symbol->classification == Symbolclass::SYMBOL_CLASS_REAL
        && this->highest_type != Symbolclass::SYMBOL_CLASS_STRING)
    {
        if (this->highest_type <= Symbolclass::SYMBOL_CLASS_REAL)
        {
            this->highest_type = Symbolclass::SYMBOL_CLASS_REAL;
            return;
        }
    }
    else
    {
        this->valid_expression = false;
        return;
    }

}
 
void AssignmentsVisitor::
visit_SyntaxNodePrimary(SyntaxNodePrimary *node)
{

    if (this->valid_expression == false) return;

    if (node->literal_type == TokenType::TOKEN_STRING)
    {
        this->highest_type = Symbolclass::SYMBOL_CLASS_STRING;
    }

    if (node->literal_type == TokenType::TOKEN_INTEGER && 
        this->highest_type != Symbolclass::SYMBOL_CLASS_STRING)
    {
        if (this->highest_type <= Symbolclass::SYMBOL_CLASS_INTEGER)
        {
            this->highest_type = Symbolclass::SYMBOL_CLASS_INTEGER;
            return;
        }
    }
    else
    {
        this->valid_expression = false;
        return;
    }

    if (node->literal_type == TokenType::TOKEN_REAL && 
        this->highest_type != Symbolclass::SYMBOL_CLASS_STRING)
    {
        if (this->highest_type <= Symbolclass::SYMBOL_CLASS_REAL)
        {
            this->highest_type = Symbolclass::SYMBOL_CLASS_REAL;
            return;
        }
    }
    else
    {
        this->valid_expression = false;
        return;
    }

    if (node->literal_type == TokenType::TOKEN_IDENTIFIER)
    {

        Symbol *symbol = this->symbol_stack->get_symbol(node->literal_reference);
        if (symbol == nullptr)
        {
            this->valid_expression = false;
            return;
        }

        if (symbol->classification == Symbolclass::SYMBOL_CLASS_STRING
            && this->highest_type != Symbolclass::SYMBOL_CLASS_STRING)
        {
            this->valid_expression = false;
            return;
        }

        if (symbol->classification == Symbolclass::SYMBOL_CLASS_INTEGER
            && this->highest_type != Symbolclass::SYMBOL_CLASS_STRING)
        {
            if (this->highest_type <= Symbolclass::SYMBOL_CLASS_INTEGER)
            {
                this->highest_type = Symbolclass::SYMBOL_CLASS_INTEGER;
                return;
            }
        }
        else
        {
            this->valid_expression = false;
            return;
        }

        if (symbol->classification == Symbolclass::SYMBOL_CLASS_REAL
            && this->highest_type != Symbolclass::SYMBOL_CLASS_STRING)
        {
            if (this->highest_type <= Symbolclass::SYMBOL_CLASS_REAL)
            {
                this->highest_type = Symbolclass::SYMBOL_CLASS_REAL;
                return;
            }
        }
        else
        {
            this->valid_expression = false;
            return;
        }

    }

}
           
void AssignmentsVisitor::
visit_SyntaxNodeGrouping(SyntaxNodeGrouping *node)
{

    node->grouping->accept(this);

}
