#ifndef SIGAMFOX_COMPILER_PARSER_VALIDATORS_EVALUTOR_HPP
#define SIGAMFOX_COMPILER_PARSER_VALIDATORS_EVALUTOR_HPP
#include <definitions.hpp>
#include <compiler/parser/visitor.hpp>
#include <compiler/environment.hpp>

class ExpressionEvaluator : public SyntaxNodeVisitor
{
    public:
                        ExpressionEvaluator(Environment *environment);
                        ExpressionEvaluator(Environment *environment, Datatype initial_type);
        virtual        ~ExpressionEvaluator();
        
        Datatype        operator()() const; // Funky functor behavior.
                                            // NOTE(Chris): Is this even necessary??

        Datatype        get_data_type() const;
        Structuretype   get_structure_type() const;
        i32             get_structure_length() const;

        virtual void    visit(SyntaxNodeEquality* node)         override;
        virtual void    visit(SyntaxNodeComparison* node)       override;
        virtual void    visit(SyntaxNodeConcatenation* node)    override;
        virtual void    visit(SyntaxNodeTerm* node)             override;
        virtual void    visit(SyntaxNodeFactor* node)           override;
        virtual void    visit(SyntaxNodeMagnitude* node)        override;
        virtual void    visit(SyntaxNodeExtraction* node)       override;
        virtual void    visit(SyntaxNodeDerivation* node)       override;
        virtual void    visit(SyntaxNodeUnary* node)            override;
        virtual void    visit(SyntaxNodeFunctionCall* node)     override;
        virtual void    visit(SyntaxNodeArrayIndex* node)       override;
        virtual void    visit(SyntaxNodePrimary* node)          override;
        virtual void    visit(SyntaxNodeGrouping* node)         override;

    
    protected:
        void            evaluate(Datatype type);
        
    protected:
        Environment    *environment;
        Datatype        evaluated_type;
        Structuretype   structure_type;
        i32             structure_length;

};

#endif
