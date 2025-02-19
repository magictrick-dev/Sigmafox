#ifndef SIGMAFOX_COMPILER_PARSER_VALIDATORS_VALASSIGN_HPP
#define SIGMAFOX_COMPILER_PARSER_VALIDATORS_VALASSIGN_HPP
#include <compiler/parser/visitor.hpp>
#include <compiler/symbols/table.hpp>
#include <compiler/environment.hpp>

// NOTE(Chris): So what we're doing here is descending into the express tree and
//              grabbing the highest order datatype corresponds to it. At the end
//              of the traversal, we need to take the left hand side of the assignment
//              expression and change the type in the node as well as the symbol table,
//              both of which require actually knowing what the node type is. The real
//              trick here is writing the visitor to do all this and actually record
//              this for us. There's gotta be an easier method for getting this info
//              without exploding out the complexity of the visitor routine.
//
//              1.  We just record the identifier in the assignment node and then
//                  we pass that to the visitor.
//              2.  We subtype of the nodes that have this property and just grab
//                  the subtype in the parser side instead of the visitor side.
//              3.  Or we raw dog it like the nasty boys we are and just do it in the
//                  visitor.

class ExpressionTypeVisitor : public SyntaxNodeVisitor
{
    public:
                        ExpressionTypeVisitor(Environment *environment, string path);
        virtual        ~ExpressionTypeVisitor();

        virtual void    visit(SyntaxNodeEquality* node) override;
        virtual void    visit(SyntaxNodeComparison* node) override;
        virtual void    visit(SyntaxNodeTerm* node) override;
        virtual void    visit(SyntaxNodeFactor* node) override;
        virtual void    visit(SyntaxNodeMagnitude* node) override;
        virtual void    visit(SyntaxNodeExtraction* node) override;
        virtual void    visit(SyntaxNodeDerivation* node) override;
        virtual void    visit(SyntaxNodeUnary* node) override;
        virtual void    visit(SyntaxNodeFunctionCall* node) override;
        virtual void    visit(SyntaxNodeArrayIndex* node) override;
        virtual void    visit(SyntaxNodePrimary* node) override;
        virtual void    visit(SyntaxNodeGrouping* node) override;

        Datatype        get_evaluated_type() const;
        void            evaluate(Datatype type);

    protected:
        Datatype            evaluated_type;
        Environment        *environment;
        string              path;

};

#endif