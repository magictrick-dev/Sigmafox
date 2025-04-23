#ifndef SF_COMPILER_GENERATION_GENERATOR_HPP
#define SF_COMPILER_GENERATION_GENERATOR_HPP
#include <definitions.hpp>
#include <compiler/parser/visitor.hpp>
#include <compiler/generation/sourcefile.hpp>
#include <compiler/generation/sourcetree.hpp>

class TranspileCPPGenerator : public SyntaxNodeVisitor
{
    public:
                        TranspileCPPGenerator();
                        TranspileCPPGenerator(string output);
        virtual        ~TranspileCPPGenerator();

        void            dump_output();
        void            generate_files();

    public:
        virtual void    visit(SyntaxNodeRoot* node)                     override;
        virtual void    visit(SyntaxNodeModule* node)                   override;
        virtual void    visit(SyntaxNodeMain* node)                     override;
        virtual void    visit(SyntaxNodeIncludeStatement* node)         override;
        virtual void    visit(SyntaxNodeFunctionStatement* node)        override;
        virtual void    visit(SyntaxNodeProcedureStatement* node)       override;
        virtual void    visit(SyntaxNodeExpressionStatement* node)      override;
        virtual void    visit(SyntaxNodeWhileStatement* node)           override;
        virtual void    visit(SyntaxNodePloopStatement* node)           override;
        virtual void    visit(SyntaxNodeLoopStatement* node)            override;
        virtual void    visit(SyntaxNodeVariableStatement* node)        override;
        virtual void    visit(SyntaxNodeScopeStatement* node)           override;
        virtual void    visit(SyntaxNodeConditionalStatement* node)     override;
        virtual void    visit(SyntaxNodeReadStatement* node)            override;
        virtual void    visit(SyntaxNodeWriteStatement* node)           override;
        virtual void    visit(SyntaxNodeExpression* node)               override;
        virtual void    visit(SyntaxNodeProcedureCall* node)            override;
        virtual void    visit(SyntaxNodeAssignment* node)               override;
        virtual void    visit(SyntaxNodeEquality* node)                 override;
        virtual void    visit(SyntaxNodeComparison* node)               override;
        virtual void    visit(SyntaxNodeConcatenation* node)            override;
        virtual void    visit(SyntaxNodeTerm* node)                     override;
        virtual void    visit(SyntaxNodeFactor* node)                   override;
        virtual void    visit(SyntaxNodeMagnitude* node)                override;
        virtual void    visit(SyntaxNodeExtraction* node)               override;
        virtual void    visit(SyntaxNodeDerivation* node)               override;
        virtual void    visit(SyntaxNodeUnary* node)                    override;
        virtual void    visit(SyntaxNodeFunctionCall* node)             override;
        virtual void    visit(SyntaxNodeArrayIndex* node)               override;
        virtual void    visit(SyntaxNodePrimary* node)                  override;
        virtual void    visit(SyntaxNodeGrouping* node)                 override;

    protected:
        string output;
        vector<shared_ptr<GeneratableSourcefile>> source_files;
        shared_ptr<GeneratableSourcefile> main_file;
        shared_ptr<GeneratableSourcefile> current_file;
        shared_ptr<GeneratableSourcefile> cmake_file;
        stack<shared_ptr<GeneratableSourcefile>> source_stack;
    
};

#endif
