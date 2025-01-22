#include <iostream>
#include <algorithm>
#include <compiler/visitors/generation.hpp>

// --- Generatable Region ------------------------------------------------------
//
// A logical partition of text data in a document. Typically, we separate a document
// into three sections, the head, body, and foot. This way, we can easily manipulate
// where text is stored regardless of where we are in the tree. Certain nodes like
// procedures or functions may be lofted into the global scope, we can easily do this
// by pushing and popping regions to restore where we are at in the current document.
//

GeneratableRegion::
GeneratableRegion()
{

}

GeneratableRegion::
~GeneratableRegion()
{

}

void GeneratableRegion::
add_line(const std::string &scope)
{
    this->scope.push_back(scope);
}

void GeneratableRegion::
add_to_current_line(const std::string &scope)
{
    this->scope.back() += scope;
}

std::string GeneratableRegion::
merge_lines() const
{

    std::string output;
    for (const auto &line : this->scope)
    {
        output += line + "\n";
    }
    return output;

}

std::string GeneratableRegion::
get_current_line() const
{
    return this->scope.back();
}

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
push_region_as_head()
{

    this->region_stack.push(&head);

}

void GeneratableFile::
push_region_as_body()
{

    this->region_stack.push(&body);

}

void GeneratableFile::
push_region_as_foot()
{

    this->region_stack.push(&foot);

}

void GeneratableFile::
pop_region()
{

    // Ensure we don't over-pop the stock.
    SF_ASSERT(this->region_stack.size() > 0);
    this->region_stack.pop();

}

void GeneratableFile::
insert_line(const std::string &scope)
{
    this->region_stack.top()->add_line(scope);
}

void GeneratableFile::
insert_blank_line()
{
    this->region_stack.top()->add_line("");
}

void GeneratableFile::
append_to_current_line(const std::string &scope)
{
    this->region_stack.top()->add_to_current_line(scope);
}

void GeneratableFile::
add_tabs_to_current_line()
{
    this->region_stack.top()->add_to_current_line(this->get_tabs());
}

void GeneratableFile::
add_tabs_to_new_line()
{
    this->insert_line(this->get_tabs());
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

    // Generate the head.
    output += head.merge_lines();

    // Generate the body.
    output += body.merge_lines();

    // Generate the foot.
    output += foot.merge_lines();

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
    this->tab_size = tab_size;

    // Construct the general body of the main file, and then the appropriate spacing.
    this->current_file->push_region_as_head();
    this->current_file->insert_line("#include <iostream>");
    this->current_file->insert_line("#include <vector>");
    this->current_file->insert_line("#include <string>");
    this->current_file->insert_line("#include <functional>");

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
        //this->current_file->add_line_to_head("#ifndef " + header_guard);
        //this->current_file->add_line_to_head("#define " + header_guard);
        this->current_file->push_region_as_head();
        this->current_file->insert_line("#ifndef " + header_guard);
        this->current_file->insert_line("#define " + header_guard);
        this->current_file->pop_region();

        // Generate the module.
        node->module->accept(this);

        // Add the endif.
        //this->current_file->add_line_to_foot("");
        //this->current_file->add_line_to_foot("#endif");
        this->current_file->push_region_as_foot();
        this->current_file->insert_blank_line();
        this->current_file->insert_line("#endif");
        this->current_file->pop_region();

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
    //this->current_file->add_line_to_head("#include \"" + node->user_path + "\"");
    this->current_file->push_region_as_head();
    this->current_file->insert_line("#include \"" + node->user_path + "\"");
    this->current_file->pop_region();

    return;

}

void GenerationVisitor::
visit_SyntaxNodeModule(SyntaxNodeModule *node)
{

    // Visit all the children.
    for (auto child : node->globals)
    {
        child->accept(this);
    }

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

    this->current_file->push_region_as_body();
    this->current_file->insert_blank_line();
    this->current_file->insert_line("int main(int argc, char **argv)");
    this->current_file->insert_line("{");
    this->current_file->insert_blank_line();
    this->current_file->push_tabs();

    // Visit all the children.
    for (auto child : node->children)
    {
        child->accept(this);
    }
    
    this->current_file->pop_region();
    this->current_file->push_region_as_foot();
    this->current_file->insert_blank_line();
    this->current_file->insert_line("}");
    this->current_file->insert_blank_line();
    this->current_file->pop_region();


}

void GenerationVisitor::
visit_SyntaxNodeExpressionStatement(SyntaxNodeExpressionStatement *node)
{

    this->current_file->add_tabs_to_new_line();
    node->expression->accept(this);
    this->current_file->append_to_current_line(";");


}

void GenerationVisitor::
visit_SyntaxNodeWhileStatement(SyntaxNodeWhileStatement *node)
{

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("while (");
    node->condition->accept(this);
    this->current_file->append_to_current_line(")");

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("{");
    this->current_file->insert_blank_line();

    // Visit all the children.
    this->current_file->push_tabs();
    for (auto child : node->children)
    {
        child->accept(this);
    }

    this->current_file->pop_tabs();

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("}");
    this->current_file->insert_blank_line();

}

void GenerationVisitor::
visit_SyntaxNodeLoopStatement(SyntaxNodeLoopStatement *node)
{

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("for (");
    this->current_file->append_to_current_line("int ");
    this->current_file->append_to_current_line(node->identifier);
    this->current_file->append_to_current_line(" = ");
    node->initial->accept(this);
    this->current_file->append_to_current_line("; ");
    this->current_file->append_to_current_line(node->identifier);
    this->current_file->append_to_current_line(" < ");
    node->terminal->accept(this);
    this->current_file->append_to_current_line("; ");

    if (node->step != nullptr)
    {
        this->current_file->append_to_current_line(node->identifier);
        this->current_file->append_to_current_line(" += ");
        node->step->accept(this);
    }
    else
    {
        this->current_file->append_to_current_line(node->identifier);
        this->current_file->append_to_current_line("++");
    }

    this->current_file->append_to_current_line(")");

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("{");
    this->current_file->insert_blank_line();

    // Visit all the children.
    this->current_file->push_tabs();
    for (auto child : node->children)
    {
        child->accept(this);
    }

    this->current_file->pop_tabs();

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("}");
    this->current_file->insert_blank_line();

}

void GenerationVisitor::
visit_SyntaxNodeVariableStatement(SyntaxNodeVariableStatement *node)
{

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("int");
    this->current_file->append_to_current_line(" ");
    this->current_file->append_to_current_line(node->variable_name);

    if (node->right_hand_side != nullptr)
    {
        this->current_file->append_to_current_line(" = ");
        node->right_hand_side->accept(this);
    }

    this->current_file->append_to_current_line(";");

}

void GenerationVisitor::
visit_SyntaxNodeScopeStatement(SyntaxNodeScopeStatement *node)
{

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("{");

    // Visit all the children.
    this->current_file->push_tabs();
    for (auto child : node->children)
    {
        child->accept(this);
    }
    this->current_file->pop_tabs();

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("{");
    this->current_file->insert_blank_line();

}

void GenerationVisitor::
visit_SyntaxNodeFunctionStatement(SyntaxNodeFunctionStatement *node)
{

    this->current_file->push_region_as_head();
    this->current_file->insert_line("int func_");
    this->current_file->append_to_current_line(node->identifier_name);
    this->current_file->append_to_current_line("(");

    // Generate the parameters.
    for (i32 i = 0; i < node->parameters.size(); i++)
    {
        this->current_file->add_tabs_to_current_line();
        this->current_file->append_to_current_line("int ");
        this->current_file->append_to_current_line(node->parameters[i]);
        if (i < node->parameters.size() - 1)
        {
            this->current_file->append_to_current_line(", ");
        }
    }

    this->current_file->append_to_current_line(")");
    this->current_file->insert_line("{");
    this->current_file->insert_blank_line();

    // Visit all the children.
    this->current_file->push_tabs();
    for (auto child : node->body_statements)
    {
        child->accept(this);
    }

    this->current_file->insert_blank_line();
    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("return " + node->identifier_name + ";");

    this->current_file->pop_tabs();
    this->current_file->insert_blank_line();
    this->current_file->insert_line("}");
    this->current_file->insert_blank_line();

    this->current_file->pop_region();

}

void GenerationVisitor::
visit_SyntaxNodeProcedureStatement(SyntaxNodeProcedureStatement *node)
{

    this->current_file->push_region_as_head();
    this->current_file->insert_line("void proc_");
    this->current_file->append_to_current_line(node->identifier_name);
    this->current_file->append_to_current_line("(");

    // Generate the parameters.
    for (i32 i = 0; i < node->parameters.size(); i++)
    {
        this->current_file->add_tabs_to_current_line();
        this->current_file->append_to_current_line("int ");
        this->current_file->append_to_current_line(node->parameters[i]);
        if (i < node->parameters.size() - 1)
        {
            this->current_file->append_to_current_line(", ");
        }
    }

    this->current_file->append_to_current_line(")");
    this->current_file->insert_line("{");
    this->current_file->insert_blank_line();

    // Visit all the children.
    this->current_file->push_tabs();
    for (auto child : node->body_statements)
    {
        child->accept(this);
    }

    this->current_file->pop_tabs();
    this->current_file->insert_blank_line();
    this->current_file->insert_line("}");
    this->current_file->insert_blank_line();

    this->current_file->pop_region();

}

void GenerationVisitor::
visit_SyntaxNodeIfStatement(SyntaxNodeIfStatement *node)
{

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("if (");
    node->conditional->accept(this);
    this->current_file->append_to_current_line(")");

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("{");
    this->current_file->insert_blank_line();

    // Visit all the children.
    this->current_file->push_tabs();
    for (auto child : node->children)
    {
        child->accept(this);
    }

    this->current_file->pop_tabs();

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("}");
    this->current_file->insert_blank_line();

    // Visit the else statement.
    if (node->conditional_else != nullptr)
        node->conditional_else->accept(this);
    
}

void GenerationVisitor::
visit_SyntaxNodeConditional(SyntaxNodeConditional * node)
{

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("else if (");
    node->condition->accept(this);
    this->current_file->append_to_current_line(")");

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("{");
    this->current_file->insert_blank_line();

    // Visit all the children.
    this->current_file->push_tabs();
    for (auto child : node->children)
    {
        child->accept(this);
    }

    this->current_file->pop_tabs();

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("}");
    this->current_file->insert_blank_line();

    if (node->conditional_else != nullptr)
        node->conditional_else->accept(this);

}

void GenerationVisitor::
visit_SyntaxNodeReadStatement(SyntaxNodeReadStatement *node)
{

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("std::cin >> ");
    this->current_file->append_to_current_line(node->identifier);
    this->current_file->append_to_current_line(";");

}

void GenerationVisitor::
visit_SyntaxNodeWriteStatement(SyntaxNodeWriteStatement *node)
{

    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line("std::cout << ");
    
    for (i32 i = 0; i < node->expressions.size(); i++)
    {

        node->expressions[i]->accept(this);
        if (i < node->expressions.size() - 1)
        {
            this->current_file->push_tabs();
            this->current_file->add_tabs_to_new_line();
            this->current_file->append_to_current_line(" << ");
            this->current_file->pop_tabs();
        }

    }

    this->current_file->push_tabs();
    this->current_file->add_tabs_to_new_line();
    this->current_file->append_to_current_line(" << std::endl;");
    this->current_file->pop_tabs();

}

void GenerationVisitor::
visit_SyntaxNodeExpression(SyntaxNodeExpression *node)    
{

    node->expression->accept(this);

}

void GenerationVisitor::
visit_SyntaxNodeProcedureCall(SyntaxNodeProcedureCall *node)
{

    this->current_file->append_to_current_line("proc_" + node->procedure_name);
    this->current_file->append_to_current_line("(");

    // Generate the parameters.
    for (i32 i = 0; i < node->parameters.size(); i++)
    {
        node->parameters[i]->accept(this);
        if (i < node->parameters.size() - 1)
        {
            this->current_file->append_to_current_line(", ");
        }
    }

    this->current_file->append_to_current_line(")");

}

void GenerationVisitor::
visit_SyntaxNodeAssignment(SyntaxNodeAssignment *node)    
{

    node->left->accept(this);
    this->current_file->append_to_current_line(" = ");
    node->right->accept(this);

}

void GenerationVisitor::
visit_SyntaxNodeEquality(SyntaxNodeEquality *node)        
{

    node->left->accept(this);
    this->current_file->append_to_current_line(" == ");
    node->right->accept(this);

}

void GenerationVisitor::
visit_SyntaxNodeComparison(SyntaxNodeComparison *node)    
{

    node->left->accept(this);

    switch (node->operation_type)
    {
        case TokenType::TOKEN_GREATER_THAN:
        {
            this->current_file->append_to_current_line(" > ");
            break;
        }
        case TokenType::TOKEN_GREATER_THAN_EQUALS:
        {
            this->current_file->append_to_current_line(" >= ");
            break;
        }
        case TokenType::TOKEN_LESS_THAN:
        {
            this->current_file->append_to_current_line(" < ");
            break;
        }
        case TokenType::TOKEN_LESS_THAN_EQUALS:
        {
            this->current_file->append_to_current_line(" <= ");
            break;
        }
        default:
        {
            SF_ASSERT(!"Invalid comparison operator.");
            break;
        }
    }

    node->right->accept(this);

}

void GenerationVisitor::
visit_SyntaxNodeTerm(SyntaxNodeTerm *node)                
{

    node->left->accept(this);

    switch (node->operation_type)
    {
        case TokenType::TOKEN_PLUS:
        {
            this->current_file->append_to_current_line(" + ");
            break;
        }
        case TokenType::TOKEN_MINUS:
        {
            this->current_file->append_to_current_line(" - ");
            break;
        }
        default:
        {
            SF_ASSERT(!"Invalid term operator.");
            break;
        }
    }

    node->right->accept(this);

}

void GenerationVisitor::
visit_SyntaxNodeFactor(SyntaxNodeFactor *node)            
{

    node->left->accept(this);

    switch (node->operation_type)
    {
        case TokenType::TOKEN_STAR:
        {
            this->current_file->append_to_current_line(" * ");
            break;
        }
        case TokenType::TOKEN_FORWARD_SLASH:
        {
            this->current_file->append_to_current_line(" / ");
            break;
        }
        default:
        {
            SF_ASSERT(!"Invalid factor operator.");
            break;
        }
    }

    node->right->accept(this);

}

void GenerationVisitor::
visit_SyntaxNodeMagnitude(SyntaxNodeMagnitude *node)     
{

    this->current_file->append_to_current_line(" pow(");
    node->left->accept(this);
    this->current_file->append_to_current_line(", ");
    node->right->accept(this);
    this->current_file->append_to_current_line(") ");

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

    switch (node->operation_type)
    {
        case TokenType::TOKEN_MINUS:
        {
            this->current_file->append_to_current_line("-");
            break;
        }
        case TokenType::TOKEN_PLUS:
        {
            this->current_file->append_to_current_line("+");
            break;
        }
        default:
        {
            SF_ASSERT(!"Invalid unary operator.");
            break;
        }
    }

    node->right->accept(this);

}

void GenerationVisitor::
visit_SyntaxNodeFunctionCall(SyntaxNodeFunctionCall *node)
{

    this->current_file->append_to_current_line("func_" + node->function_name);
    this->current_file->append_to_current_line("(");

    // Generate the parameters.
    for (i32 i = 0; i < node->parameters.size(); i++)
    {
        node->parameters[i]->accept(this);
        if (i < node->parameters.size() - 1)
        {
            this->current_file->append_to_current_line(", ");
        }
    }

    this->current_file->append_to_current_line(")");

}

void GenerationVisitor::
visit_SyntaxNodeArrayIndex(SyntaxNodeArrayIndex *node)
{

    this->current_file->append_to_current_line(node->variable_name);

    for (int i = 0; i < node->indices.size(); i++)
    {
        this->current_file->append_to_current_line("[");
        node->indices[i]->accept(this);
        this->current_file->append_to_current_line("]");
    }

}

void GenerationVisitor::
visit_SyntaxNodePrimary(SyntaxNodePrimary *node)
{

    switch (node->literal_type)
    {
        case TokenType::TOKEN_IDENTIFIER:
        {
            this->current_file->append_to_current_line(node->literal_reference.c_str());
            break;
        }
        case TokenType::TOKEN_INTEGER:
        {
            this->current_file->append_to_current_line(node->literal_reference.c_str());
            break;
        }
        case TokenType::TOKEN_REAL:
        {
            this->current_file->append_to_current_line(node->literal_reference.c_str());
            break;
        }
        case TokenType::TOKEN_STRING:
        {
            this->current_file->append_to_current_line("\"" + node->literal_reference + "\"");
            break;
        }
        default:
        {
            SF_ASSERT(!"Invalid primary type.");
            break;
        }
    }

}

void GenerationVisitor::
visit_SyntaxNodeGrouping(SyntaxNodeGrouping *node)
{

    this->current_file->append_to_current_line("(");
    node->grouping->accept(this);
    this->current_file->append_to_current_line(")");

}

