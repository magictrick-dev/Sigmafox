#ifndef SIGMAFOX_COMPILER_VISITORS_GENERATION_HPP
#define SIGMAFOX_COMPILER_VISITORS_GENERATION_HPP
#include <compiler/syntaxvisitor.hpp>
#include <compiler/syntaxnodes.hpp>
#include <vector>
#include <stack>

// --- C++ Transpilation Generation Visitor ------------------------------------
//
// This visitor is responsible for generating the valid C++ code from the AST.
// The code for this is generally pretty straightforward, as it will simply
// run throw the AST, generate the code, and then write it to a file.
//
// The visitor will also keep track of the files that are being generated, and
// will generate a CMakeLists.txt file that will be used to compile the project.
//
// Since it is possible to include files that are already generated, the visitor
// is responsible for ensuring that no duplicate includes are handled.
//

class GeneratableRegion
{
    public:
                    GeneratableRegion();
        virtual    ~GeneratableRegion();

        void        add_line(const std::string &scope);
        void        add_to_current_line(const std::string &scope);

        std::string get_current_line() const;
    
        std::string merge_lines() const;

    protected:
        std::vector<std::string> scope;

};

class GeneratableFile
{

    public:
                    GeneratableFile(const std::string& absolute_path, 
                        const std::string& filename, i32 tab_size);
        virtual    ~GeneratableFile();

        std::string get_filename() const;
        std::string get_absolute_path() const;

        void        push_region_as_head();
        void        push_region_as_body();
        void        push_region_as_foot();
        void        pop_region();

        void        insert_line(const std::string &scope);
        void        insert_blank_line();
        void        append_to_current_line(const std::string &scope);

        void        push_tabs();
        void        pop_tabs();

        std::string get_tabs() const;
        void        add_tabs_to_current_line();
        void        add_tabs_to_new_line();

        std::string generate() const;

    protected:
        i32 tabs;
        i32 tab_size;
        std::string filename;
        std::string absolute_path;
        std::stack<GeneratableRegion*> region_stack;
        GeneratableRegion head;
        GeneratableRegion body;
        GeneratableRegion foot;

};

class GenerationVisitor : public ISyntaxNodeVisitor
{

    public:
                        GenerationVisitor(const std::string& main_absolute_path, i32 tab_size);   
        virtual        ~GenerationVisitor();

        void            dump_files();

    public:
        virtual void visit_SyntaxNodeRoot(SyntaxNodeRoot *node) override;
        virtual void visit_SyntaxNodeModule(SyntaxNodeModule *node) override;
        virtual void visit_SyntaxNodeMain(SyntaxNodeMain *node) override;
        virtual void visit_SyntaxNodeInclude(SyntaxNodeInclude *node) override;
        virtual void visit_SyntaxNodeExpressionStatement(SyntaxNodeExpressionStatement *node) override; 
        virtual void visit_SyntaxNodeWhileStatement(SyntaxNodeWhileStatement *node) override;
        virtual void visit_SyntaxNodeLoopStatement(SyntaxNodeLoopStatement *node) override;
        virtual void visit_SyntaxNodeVariableStatement(SyntaxNodeVariableStatement *node) override; 
        virtual void visit_SyntaxNodeScopeStatement(SyntaxNodeScopeStatement *node) override;
        virtual void visit_SyntaxNodeFunctionStatement(SyntaxNodeFunctionStatement *node) override;
        virtual void visit_SyntaxNodeProcedureStatement(SyntaxNodeProcedureStatement *node) override;
        virtual void visit_SyntaxNodeIfStatement(SyntaxNodeIfStatement *node) override;
        virtual void visit_SyntaxNodeConditional(SyntaxNodeConditional * node) override;
        virtual void visit_SyntaxNodeReadStatement(SyntaxNodeReadStatement *node) override;
        virtual void visit_SyntaxNodeWriteStatement(SyntaxNodeWriteStatement *node) override;
        virtual void visit_SyntaxNodeExpression(SyntaxNodeExpression *node) override;     
        virtual void visit_SyntaxNodeProcedureCall(SyntaxNodeProcedureCall *node) override;
        virtual void visit_SyntaxNodeAssignment(SyntaxNodeAssignment *node) override;     
        virtual void visit_SyntaxNodeEquality(SyntaxNodeEquality *node) override;         
        virtual void visit_SyntaxNodeComparison(SyntaxNodeComparison *node) override;     
        virtual void visit_SyntaxNodeTerm(SyntaxNodeTerm *node) override;                 
        virtual void visit_SyntaxNodeFactor(SyntaxNodeFactor *node) override;             
        virtual void visit_SyntaxNodeMagnitude(SyntaxNodeMagnitude *node) override;       
        virtual void visit_SyntaxNodeExtraction(SyntaxNodeExtraction *node) override;     
        virtual void visit_SyntaxNodeDerivation(SyntaxNodeDerivation *node) override;     
        virtual void visit_SyntaxNodeUnary(SyntaxNodeUnary *node) override;               
        virtual void visit_SyntaxNodeFunctionCall(SyntaxNodeFunctionCall *node) override; 
        virtual void visit_SyntaxNodeArrayIndex(SyntaxNodeArrayIndex *node) override; 
        virtual void visit_SyntaxNodePrimary(SyntaxNodePrimary *node) override;           
        virtual void visit_SyntaxNodeGrouping(SyntaxNodeGrouping *node) override;

    protected:
        i32 tab_size;
        i32 stack_index = -1;
        GeneratableFile main_file;
        GeneratableFile *current_file;
        std::vector<GeneratableFile> include_files;

};

#endif
