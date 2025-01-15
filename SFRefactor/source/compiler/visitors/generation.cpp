#include <iostream>
#include <algorithm>
#include <compiler/visitors/generation.hpp>

// --- Generatable File --------------------------------------------------------
//
// Essentially encapsulates the file that is being generated. Since we're using
// standard string for basically handling our line manips, it probably isn't going
// to be particular fast or efficient to run this on multiple files. However, for
// the sake of the project, it should be fine just to get things rolling.
//
// Some potential ideas for this is to use a pre-made buffer that we can write to.
// The buffer, in theory, sets up a number of columns per line, and then we can
// just line-reference where we want to place the code and fill it in. The general
// complexity of this would be a lot larger than using strings, but it would be
// something worth looking into if this is a slow going operation.
//

GeneratableFile::
GeneratableFile(const std::string& absolute_path, const std::string& filename, i32 tab_size)
{

    this->absolute_path = absolute_path;
    this->filename = filename;
    this->tabs = 0;
    this->tab_size = tab_size;

}

GeneratableFile::
~GeneratableFile()
{

}

std::string GeneratableFile::
get_filename() const
{
    return filename;
}

std::string GeneratableFile::
get_absolute_path() const
{
    return absolute_path;
}

void GeneratableFile::
add_line_to_head(const std::string &scope)
{
    global_scope.push_back(scope);
}

void GeneratableFile::
add_line_to_body(const std::string &scope)
{
    main_scope.push_back(scope);
}

void GeneratableFile::
add_line_to_foot(const std::string &scope)
{
    foot_scope.push_back(scope);
}

void GeneratableFile::
push_tabs()
{
    tabs += tab_size;
}

void GeneratableFile::
pop_tabs()
{
    tabs -= tab_size;
}

std::string GeneratableFile::
get_tabs() const
{
    std::string tabs;
    for (i32 i = 0; i < this->tabs; i++)
    {
        tabs += " ";
    }
    return tabs;
}

std::string GeneratableFile::
generate() const
{
    std::string output;

    for (const auto &scope : global_scope)
    {
        output += scope + "\n";
    }

    for (const auto &scope : main_scope)
    {
        output += scope + "\n";
    }

    for (const auto &scope : foot_scope)
    {
        output += scope + "\n";
    }

    return output;
}

// --- Generation Visitor ------------------------------------------------------
//
// Here, we just spin over all the nodes and generate valid C++ for the AST. As we
// encounter new files, we need to create a list of paths that will be needed for
// generating a cmakelists file.
//

GenerationVisitor::
GenerationVisitor(const std::string& main_absolute_path, i32 tab_size)
    : main_file(main_absolute_path, "main.cpp", tab_size)
{

    // Set the current file as the main file.
    current_file = &main_file;

    // Construct the general body of the main file, and then the appropriate spacing.
    this->current_file->add_line_to_head("#include <iostream>");

    this->current_file->add_line_to_body("");
    this->current_file->add_line_to_body("int main(int argc, char* argv[])");
    this->current_file->add_line_to_body("{");
    this->current_file->add_line_to_body("");

    this->current_file->add_line_to_foot("");
    this->current_file->add_line_to_foot("}");
    this->current_file->add_line_to_foot("");

}

GenerationVisitor::
~GenerationVisitor()
{

}

void GenerationVisitor::
dump_files()
{

    // Dump the main file.
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << main_file.get_filename() << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << main_file.generate() << std::endl;

    // Dump the rest of the files.
    for (const auto &file : this->include_files)
    {
        std::cout << "---------------------------------------------------" << std::endl;
        std::cout << file.get_filename() << std::endl;
        std::cout << "---------------------------------------------------" << std::endl;
        std::cout << file.generate() << std::endl;
    }

}

// ---- Root -------------------------------------------------------------------
//
// The initial entry point node for the AST, all it does is visit the globals listed
// in the node and then jump to main.
//

void GenerationVisitor::
visit_SyntaxNodeRoot(SyntaxNodeRoot *node)
{

    // Visit all the children.
    for (auto child : node->globals)
    {
        child->accept(this);
    }

    // Accept main.
    node->main->accept(this);

}

// --- Includes & Modules ------------------------------------------------------
//
// Include statements will generate the header definition for the file, then, if
// the file was not generated, it will then generate the module, traverse the children,
// and then pop back out.
//
// Since includes can potentially nest more includes, we will need to keep track of
// of the current file we're traversing by pushing and popping the current file.
//
// This design is not really thread-friendly, but there is potential to multi-thread
// this part here by jobbing the module traversals and generating the file asynchronously.
// Since the dependency of the files is known, there is no concern of overlap since we will
// only ever traverse the module nodes once.
//

void GenerationVisitor::
visit_SyntaxNodeInclude(SyntaxNodeInclude *node)
{

    // Check if the file has already been generated.
    bool exists = false;
    for (const auto &file : this->include_files)
    {
        if (file.get_absolute_path() == node->path)
        {
            exists = true;
            break;
        }
    }

    // Create a new file.
    if (exists == false)
    {
        GeneratableFile new_file(node->path, node->user_path, this->tab_size);
        this->include_files.push_back(new_file);
        this->stack_index++;

        // Set the current file to the new file.
        this->current_file = this->include_files.data() + this->stack_index;

        // Strip punction from the node's user path, convert slashes to underscores,
        // and then fully-capitalize it for the header guard.
        std::string header_guard = node->user_path;
        std::replace(header_guard.begin(), header_guard.end(), '/', '_');
        std::replace(header_guard.begin(), header_guard.end(), '\\', '_');
        std::replace(header_guard.begin(), header_guard.end(), '.', '_');
        std::transform(header_guard.begin(), header_guard.end(), header_guard.begin(), ::toupper);

        // Create the header guard.
        this->current_file->add_line_to_head("#ifndef " + header_guard);
        this->current_file->add_line_to_head("#define " + header_guard);

        // Generate the module.
        node->module->accept(this);

        // Add the endif.
        this->current_file->add_line_to_foot("");
        this->current_file->add_line_to_foot("#endif");

        // Pop the current file.
        this->stack_index--;
        SF_ASSERT(this->stack_index >= -1); // Should never be less than -1.
        if (this->stack_index >= 0)
        {
            this->current_file = this->include_files.data() + this->stack_index;
        }
        else
        {
            this->current_file = &main_file;
        }

    }
    
    // Whether or not the file needed to be generated isn't important at this point,
    // we can just include the file here.
    this->current_file->add_line_to_head("#include \"" + node->user_path + "\"");

    return;

}

void GenerationVisitor::
visit_SyntaxNodeModule(SyntaxNodeModule *node)
{

    // Visit all the children, no additional processing necessary at this point.
    for (auto child : node->globals)
    {
        child->accept(this);
    }

    return;

}

// --- Main Processing ---------------------------------------------------------
//
// Main processing occurs within the main body of the .fox files, but doesn't
// necessarily mean code generation will occur within C++'s code generation locale.
// One of the problems that we will immediately come across is that we allow nested
// functions within the scope of the main body. This means that we will need to generate
// global functions that are accessible from the main body to make it valid C++.
//
// As you might imagine, we also have some additional problems where functions that
// are created within nested scopes may override functions that are created in upper
// scopes. For now, this case isn't handled in the parser, so we will need to be careful
// with how we handle this.
//

void GenerationVisitor::
visit_SyntaxNodeMain(SyntaxNodeMain *node)
{

    // Visit all the children.
    for (auto child : node->children)
    {
        child->accept(this);
    }

    return;

}

void GenerationVisitor::
visit_SyntaxNodeExpressionStatement(SyntaxNodeExpressionStatement *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeWhileStatement(SyntaxNodeWhileStatement *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeLoopStatement(SyntaxNodeLoopStatement *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeVariableStatement(SyntaxNodeVariableStatement *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeScopeStatement(SyntaxNodeScopeStatement *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeFunctionStatement(SyntaxNodeFunctionStatement *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeProcedureStatement(SyntaxNodeProcedureStatement *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeIfStatement(SyntaxNodeIfStatement *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeConditional(SyntaxNodeConditional * node)
{



}

void GenerationVisitor::
visit_SyntaxNodeReadStatement(SyntaxNodeReadStatement *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeWriteStatement(SyntaxNodeWriteStatement *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeExpression(SyntaxNodeExpression *node)    
{



}

void GenerationVisitor::
visit_SyntaxNodeProcedureCall(SyntaxNodeProcedureCall *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeAssignment(SyntaxNodeAssignment *node)    
{



}

void GenerationVisitor::
visit_SyntaxNodeEquality(SyntaxNodeEquality *node)        
{



}

void GenerationVisitor::
visit_SyntaxNodeComparison(SyntaxNodeComparison *node)    
{



}

void GenerationVisitor::
visit_SyntaxNodeTerm(SyntaxNodeTerm *node)                
{



}

void GenerationVisitor::
visit_SyntaxNodeFactor(SyntaxNodeFactor *node)            
{



}

void GenerationVisitor::
visit_SyntaxNodeMagnitude(SyntaxNodeMagnitude *node)     
{



}

void GenerationVisitor::
visit_SyntaxNodeExtraction(SyntaxNodeExtraction *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeDerivation(SyntaxNodeDerivation *node)    
{



}

void GenerationVisitor::
visit_SyntaxNodeUnary(SyntaxNodeUnary *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeFunctionCall(SyntaxNodeFunctionCall *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeArrayIndex(SyntaxNodeArrayIndex *node)
{



}

void GenerationVisitor::
visit_SyntaxNodePrimary(SyntaxNodePrimary *node)
{



}

void GenerationVisitor::
visit_SyntaxNodeGrouping(SyntaxNodeGrouping *node)
{

}

