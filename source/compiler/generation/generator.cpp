#include <iostream>
#include <algorithm>
#include <compiler/generation/generator.hpp>
#include <utilities/path.hpp>

// --- Core Transpiler Routines ------------------------------------------------

TranspileCPPGenerator::
TranspileCPPGenerator()
{

    this->output = "./output";

}

TranspileCPPGenerator::
TranspileCPPGenerator(string output)
{

    this->output = output;

}

TranspileCPPGenerator::
~TranspileCPPGenerator()
{

}

void TranspileCPPGenerator::
dump_output()
{

    for (auto file : this->source_files)
    {
        
        std::cout << "------------------------------------------------------------------" << std::endl;
        std::cout << file->get_file_path() << std::endl;
        std::cout << "------------------------------------------------------------------" << std::endl;
        std::cout << file->get_source() << std::endl;

    }

}

void TranspileCPPGenerator::
generate_files()
{

    Sourcetree source_tree(this->output);
    for (auto source : this->source_files)
    {
        if (!source_tree.insert_source(source.get()))
        {
            std::cout << "-- Unable to insert source file: " << source->get_file_name() << std::endl;
        }
    }

    if (source_tree.commit())
    {
        std::cout << "-- Generation complete." << std::endl;
    }
    else
    {
        std::cout << "-- Generation failed." << std::endl;
    }

    return;

}

// --- Visitor Routines --------------------------------------------------------

void TranspileCPPGenerator::    
visit(SyntaxNodeRoot* node)
{

    bool is_entry = false;
    if (this->source_files.empty())
    {

        string output_name = node->relative_base;
        string extension = ".cpp";
        output_name.replace(output_name.find(".fox"), extension.length(), extension);
        std::replace(output_name.begin(), output_name.end(), '\\', '/');

        string cmake_path = "./CMakeLists.txt";

        this->source_files.push_back(std::make_shared<GeneratableSourcefile>(cmake_path, cmake_path));
        this->cmake_file = this->source_files[this->source_files.size()-1];
        this->current_file = this->cmake_file;

        this->current_file->push_region_as_head();
        this->current_file->insert_line_with_tabs("CMAKE_MINIMUM_REQUIRED(VERSION 3.21)");
        this->current_file->insert_blank_line();
        this->current_file->insert_line_with_tabs("PROJECT(cosyproject)");
        this->current_file->insert_blank_line();
        this->current_file->insert_line_with_tabs("SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY \"./bin\")");
        this->current_file->insert_line_with_tabs("SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)");
        this->current_file->insert_line_with_tabs("SET(CMAKE_BUILD_TYPE Debug)");
        this->current_file->insert_blank_line();
        this->current_file->insert_line_with_tabs("ADD_EXECUTABLE(cosyproject");
        this->current_file->pop_region();

        this->current_file->push_region_as_body();
        this->current_file->push_tabs();
        this->current_file->insert_line_with_tabs("\"");
        this->current_file->append_to_current_line(output_name);
        this->current_file->append_to_current_line("\"");
        this->current_file->pop_tabs();
        this->current_file->pop_region();

        this->current_file->push_region_as_foot();
        this->current_file->insert_line_with_tabs(")");
        this->current_file->pop_region();
        
        this->source_files.push_back(std::make_shared<GeneratableSourcefile>(output_name, output_name));
        this->main_file             = this->source_files[this->source_files.size()-1];
        this->current_file          = this->source_files[this->source_files.size()-1];
        this->source_stack.push(this->current_file);
        is_entry = true;

    }

    else
    {

        string output_name = node->relative_base;
        string extension = ".hpp";
        output_name.replace(output_name.find(".fox"), extension.length(), extension);
        std::replace(output_name.begin(), output_name.end(), '\\', '/');

        this->current_file = this->cmake_file;
        this->current_file->push_region_as_body();
        this->current_file->push_tabs();
        this->current_file->insert_line_with_tabs("\"");
        this->current_file->append_to_current_line(output_name);
        this->current_file->append_to_current_line("\"");
        this->current_file->pop_tabs();
        this->current_file->pop_region();

        this->source_files.push_back(std::make_shared<GeneratableSourcefile>(output_name, output_name));
        this->current_file = this->source_files[this->source_files.size() - 1];
        this->source_stack.push(this->current_file);

    }

    // Create a header guard.
    this->current_file->push_region_as_head();
    if (!is_entry)
    {

        std::string header_guard = node->relative_base;
        std::replace(header_guard.begin(), header_guard.end(), '/', '_');
        std::replace(header_guard.begin(), header_guard.end(), '\\', '_');
        std::replace(header_guard.begin(), header_guard.end(), '.', '_');
        std::transform(header_guard.begin(), header_guard.end(), header_guard.begin(), ::toupper);

        // Header guards.
        this->current_file->insert_line("#ifndef ");
        this->current_file->append_to_current_line(header_guard);
        this->current_file->insert_line("#define ");
        this->current_file->append_to_current_line(header_guard);

    }



    this->current_file->insert_line("#include <iostream>");
    this->current_file->insert_line("#include <complex>");
    this->current_file->insert_line("#include <vector>");
    this->current_file->insert_line("#include <string>");
    this->current_file->insert_line("#include <cstdint>");
    this->current_file->insert_blank_line();
    this->current_file->insert_line("typedef std::complex<double> complexd;");
    this->current_file->insert_blank_line();
    this->current_file->pop_region();

    // Visiting all global nodes.
    for (auto child : node->children)
    {

        child->accept(this);

    }

    if (!is_entry)
    {
        // Closing header guard.
        this->current_file->push_region_as_foot();
        this->current_file->insert_line("#endif");
        this->current_file->pop_region();
    }

    SF_ASSERT(!this->source_stack.empty());
    this->source_stack.pop();
    if (this->source_stack.empty()) return;
    this->current_file = this->source_stack.top();

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeModule* node)
{

    node->root->accept(this);

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeMain* node)
{

    this->current_file->push_region_as_body();
    this->current_file->insert_blank_line();
    this->current_file->insert_line("int");
    this->current_file->insert_line("main(int argc, char **argv)");
    this->current_file->insert_line("{");

    this->current_file->push_tabs();
    this->current_file->insert_blank_line();

    for (auto child : node->children) child->accept(this);
    this->current_file->insert_blank_line();

    this->current_file->insert_line_with_tabs("return 0;");

    this->current_file->insert_blank_line();
    this->current_file->pop_tabs();

    this->current_file->insert_line("}");

    this->current_file->pop_region();

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeIncludeStatement* node)
{

    string output_name = node->relative_path;
    string extension = ".hpp";
    output_name.replace(output_name.find(".fox"), extension.length(), extension);
    std::replace(output_name.begin(), output_name.end(), '\\', '/');

    this->current_file->insert_line_with_tabs("#include \"");
    this->current_file->append_to_current_line(output_name);
    this->current_file->append_to_current_line("\"");
    this->current_file->insert_blank_line();

    if (node->module != nullptr) node->module->accept(this);

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeFunctionStatement* node)
{

    if (node->is_global)
    {

        SyntaxNodeVariableStatement *variable_node =
            dynamic_cast<SyntaxNodeVariableStatement*>(node->variable_node);
        SF_ENSURE_PTR(variable_node);

        this->current_file->push_region_as_head();
        this->current_file->insert_line_with_tabs("inline ");

        Datatype function_datatype = variable_node->data_type;
        Structuretype function_structure_type = variable_node->structure_type;
        i32 function_structure_length = variable_node->structure_length;
        if (function_structure_type == Structuretype::STRUCTURE_TYPE_SCALAR ||
            function_structure_type == Structuretype::STRUCTURE_TYPE_STRING)
        {
            switch (function_datatype)
            {

                case Datatype::DATA_TYPE_STRING:
                {
                    this->current_file->append_to_current_line("std::string ");
                } break;

                case Datatype::DATA_TYPE_INTEGER:
                {
                    this->current_file->append_to_current_line("int64_t ");
                } break;

                case Datatype::DATA_TYPE_REAL:
                {
                    this->current_file->append_to_current_line("double ");
                } break;

                case Datatype::DATA_TYPE_COMPLEX:
                {
                    this->current_file->append_to_current_line("std::complex<double> ");
                } break;

                case Datatype::DATA_TYPE_UNKNOWN:
                {
                    this->current_file->append_to_current_line("/*unknown*/ int64_t ");
                } break;

                default:
                {
                    this->current_file->append_to_current_line("/*unknown*/ int64_t ");
                    //SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
                };

            }
        }
        else
        {

            this->current_file->append_to_current_line("dvector<double, ");
            this->current_file->append_to_current_line(std::to_string(function_structure_length));
            this->current_file->append_to_current_line("> ");

        }

        
        this->current_file->insert_line_with_tabs("fn_");
        this->current_file->append_to_current_line(variable_node->identifier);
        this->current_file->append_to_current_line("(");

        for (int i = 0; i < node->parameters.size(); ++i)
        {

            SyntaxNodeVariableStatement *parameter_variable_node = 
                dynamic_cast<SyntaxNodeVariableStatement*>(node->parameters[i]);
            SF_ENSURE_PTR(parameter_variable_node);

            Datatype parameter_datatype = parameter_variable_node->data_type;
            Structuretype parameter_structure_type = parameter_variable_node->structure_type;
            i32 parameter_structure_length = parameter_variable_node->structure_length;
            if (parameter_structure_type == Structuretype::STRUCTURE_TYPE_SCALAR ||
                parameter_structure_type == Structuretype::STRUCTURE_TYPE_STRING)
            {
                switch (parameter_datatype)
                {

                    case Datatype::DATA_TYPE_STRING:
                    {
                        this->current_file->append_to_current_line("std::string ");
                    } break;

                    case Datatype::DATA_TYPE_INTEGER:
                    {
                        this->current_file->append_to_current_line("int64_t ");
                    } break;

                    case Datatype::DATA_TYPE_REAL:
                    {
                        this->current_file->append_to_current_line("double ");
                    } break;

                    case Datatype::DATA_TYPE_COMPLEX:
                    {
                        this->current_file->append_to_current_line("std::complex<double> ");
                    } break;

                    case Datatype::DATA_TYPE_UNKNOWN:
                    {
                        this->current_file->append_to_current_line("/*unknown*/ int64_t ");
                    } break;

                    default:
                    {
                        this->current_file->append_to_current_line("/*unknown*/ int64_t ");
                        //SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
                    };

                }
            }
            else
            {

                this->current_file->append_to_current_line("dvector<double, ");
                this->current_file->append_to_current_line(std::to_string(parameter_structure_length));
                this->current_file->append_to_current_line("> ");

            }

            this->current_file->append_to_current_line(parameter_variable_node->identifier);
            if (i < node->parameters.size() - 1)
            {
                this->current_file->append_to_current_line(", ");

            }

        }

        this->current_file->append_to_current_line(")");

        this->current_file->insert_line_with_tabs("{");
        this->current_file->insert_blank_line();
        this->current_file->push_tabs();

        switch (function_datatype)
        {

            case Datatype::DATA_TYPE_STRING:
            {
                this->current_file->insert_line_with_tabs("std::string ");
            } break;

            case Datatype::DATA_TYPE_INTEGER:
            {
                this->current_file->insert_line_with_tabs("int64_t ");
            } break;

            case Datatype::DATA_TYPE_REAL:
            {
                this->current_file->insert_line_with_tabs("double ");
            } break;

            case Datatype::DATA_TYPE_COMPLEX:
            {
                this->current_file->insert_line_with_tabs("std::complex<double> ");
            } break;

            default:
            {
                SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
            };

        }

        this->current_file->append_to_current_line(variable_node->identifier);
        this->current_file->append_to_current_line(";");
        this->current_file->insert_blank_line();

        for (auto child : node->children) child->accept(this);

        this->current_file->insert_blank_line();

        this->current_file->insert_line_with_tabs("return ");
        this->current_file->append_to_current_line(variable_node->identifier);
        this->current_file->append_to_current_line(";");
        this->current_file->insert_blank_line();
        this->current_file->pop_tabs();

        this->current_file->insert_line_with_tabs("}");
        this->current_file->insert_blank_line();

        this->current_file->pop_region();

    }
    else
    {

        SyntaxNodeVariableStatement *variable_node =
            dynamic_cast<SyntaxNodeVariableStatement*>(node->variable_node);
        SF_ENSURE_PTR(variable_node);

/*
auto myLambda = [](int x, int y) -> double {
    return x + 0.5 * y;
};
*/
        //
//

        this->current_file->insert_line_with_tabs("auto ");
        this->current_file->append_to_current_line("fn_");
        this->current_file->append_to_current_line(variable_node->identifier);
        this->current_file->append_to_current_line(" = []");
        this->current_file->append_to_current_line("(");

        for (int i = 0; i < node->parameters.size(); ++i)
        {

            SyntaxNodeVariableStatement *parameter_variable_node = 
                dynamic_cast<SyntaxNodeVariableStatement*>(node->parameters[i]);
            SF_ENSURE_PTR(parameter_variable_node);

            Datatype parameter_datatype = parameter_variable_node->data_type;
            switch (parameter_datatype)
            {

                case Datatype::DATA_TYPE_STRING:
                {
                    this->current_file->append_to_current_line("std::string ");
                } break;

                case Datatype::DATA_TYPE_INTEGER:
                {
                    this->current_file->append_to_current_line("int64_t ");
                } break;

                case Datatype::DATA_TYPE_REAL:
                {
                    this->current_file->append_to_current_line("double ");
                } break;

                case Datatype::DATA_TYPE_COMPLEX:
                {
                    this->current_file->append_to_current_line("std::complex<double> ");
                } break;

                default:
                {
                    SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
                };

            }

            this->current_file->append_to_current_line(parameter_variable_node->identifier);
            if (i < node->parameters.size() - 1)
            {
                this->current_file->append_to_current_line(", ");

            }

        }

        this->current_file->append_to_current_line(") -> ");

        Datatype function_datatype = variable_node->data_type;
        Structuretype function_structure_type = variable_node->structure_type;
        i32 function_structure_length = variable_node->structure_length;
        if (function_structure_type == Structuretype::STRUCTURE_TYPE_SCALAR ||
            function_structure_type == Structuretype::STRUCTURE_TYPE_STRING)
        {
            switch (function_datatype)
            {

                case Datatype::DATA_TYPE_STRING:
                {
                    this->current_file->append_to_current_line("std::string ");
                } break;

                case Datatype::DATA_TYPE_INTEGER:
                {
                    this->current_file->append_to_current_line("int64_t ");
                } break;

                case Datatype::DATA_TYPE_REAL:
                {
                    this->current_file->append_to_current_line("double ");
                } break;

                case Datatype::DATA_TYPE_COMPLEX:
                {
                    this->current_file->append_to_current_line("std::complex<double> ");
                } break;

                case Datatype::DATA_TYPE_UNKNOWN:
                {
                    this->current_file->append_to_current_line("/*unknown*/ int64_t ");
                } break;

                default:
                {
                    this->current_file->append_to_current_line("/*unknown*/ int64_t ");
                    //SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
                };

            }
        }
        else
        {

            this->current_file->append_to_current_line("dvector<double, ");
            this->current_file->append_to_current_line(std::to_string(function_structure_length));
            this->current_file->append_to_current_line("> ");

        }


        this->current_file->insert_line_with_tabs("{");
        this->current_file->insert_blank_line();
        this->current_file->push_tabs();

        switch (function_datatype)
        {

            case Datatype::DATA_TYPE_STRING:
            {
                this->current_file->insert_line_with_tabs("std::string ");
            } break;

            case Datatype::DATA_TYPE_INTEGER:
            {
                this->current_file->insert_line_with_tabs("int64_t ");
            } break;

            case Datatype::DATA_TYPE_REAL:
            {
                this->current_file->insert_line_with_tabs("double ");
            } break;

            case Datatype::DATA_TYPE_COMPLEX:
            {
                this->current_file->insert_line_with_tabs("std::complex<double> ");
            } break;

            default:
            {
                SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
            };

        }

        this->current_file->append_to_current_line(variable_node->identifier);
        this->current_file->append_to_current_line(";");
        this->current_file->insert_blank_line();

        for (auto child : node->children) child->accept(this);

        this->current_file->insert_blank_line();

        this->current_file->insert_line_with_tabs("return ");
        this->current_file->append_to_current_line(variable_node->identifier);
        this->current_file->append_to_current_line(";");
        this->current_file->insert_blank_line();
        this->current_file->pop_tabs();

        this->current_file->insert_line_with_tabs("};");
        this->current_file->insert_blank_line();

    }

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeProcedureStatement* node)
{

    if (node->is_global)
    {

        SyntaxNodeVariableStatement *variable_node =
            dynamic_cast<SyntaxNodeVariableStatement*>(node->variable_node);
        SF_ENSURE_PTR(variable_node);

        this->current_file->push_region_as_head();
        this->current_file->insert_line_with_tabs("static inline ");

        Datatype function_datatype = variable_node->data_type;
        switch (function_datatype)
        {

            case Datatype::DATA_TYPE_VOID:
            {
                this->current_file->append_to_current_line("void ");
            } break;

            default:
            {
                SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
            };

        }
        
        this->current_file->insert_line_with_tabs(variable_node->identifier);
        this->current_file->append_to_current_line("(");

        for (int i = 0; i < node->parameters.size(); ++i)
        {

            SyntaxNodeVariableStatement *parameter_variable_node = 
                dynamic_cast<SyntaxNodeVariableStatement*>(node->parameters[i]);
            SF_ENSURE_PTR(parameter_variable_node);

            Datatype parameter_datatype = parameter_variable_node->data_type;
            switch (parameter_datatype)
            {

                case Datatype::DATA_TYPE_STRING:
                {
                    this->current_file->append_to_current_line("std::string ");
                } break;

                case Datatype::DATA_TYPE_INTEGER:
                {
                    this->current_file->append_to_current_line("int64_t ");
                } break;

                case Datatype::DATA_TYPE_REAL:
                {
                    this->current_file->append_to_current_line("double ");
                } break;

                case Datatype::DATA_TYPE_COMPLEX:
                {
                    this->current_file->append_to_current_line("std::complex<double> ");
                } break;

                default:
                {
                    SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
                };

            }

            this->current_file->append_to_current_line(parameter_variable_node->identifier);
            if (i < node->parameters.size() - 1)
            {
                this->current_file->append_to_current_line(", ");

            }

        }

        this->current_file->append_to_current_line(")");

        this->current_file->insert_line_with_tabs("{");
        this->current_file->insert_blank_line();

        this->current_file->push_tabs();
        for (auto child : node->children) child->accept(this);
        this->current_file->pop_tabs();

        this->current_file->insert_blank_line();
        this->current_file->insert_line_with_tabs("}");
        this->current_file->insert_blank_line();

        this->current_file->pop_region();

    }
    else
    {
        SyntaxNodeVariableStatement *variable_node =
            dynamic_cast<SyntaxNodeVariableStatement*>(node->variable_node);
        SF_ENSURE_PTR(variable_node);

        this->current_file->insert_line_with_tabs("auto ");
        this->current_file->append_to_current_line(variable_node->identifier);
        this->current_file->append_to_current_line(" = []");
        this->current_file->append_to_current_line("(");

        for (int i = 0; i < node->parameters.size(); ++i)
        {

            SyntaxNodeVariableStatement *parameter_variable_node = 
                dynamic_cast<SyntaxNodeVariableStatement*>(node->parameters[i]);
            SF_ENSURE_PTR(parameter_variable_node);

            Datatype parameter_datatype = parameter_variable_node->data_type;
            switch (parameter_datatype)
            {

                case Datatype::DATA_TYPE_STRING:
                {
                    this->current_file->append_to_current_line("std::string ");
                } break;

                case Datatype::DATA_TYPE_INTEGER:
                {
                    this->current_file->append_to_current_line("int64_t ");
                } break;

                case Datatype::DATA_TYPE_REAL:
                {
                    this->current_file->append_to_current_line("double ");
                } break;

                case Datatype::DATA_TYPE_COMPLEX:
                {
                    this->current_file->append_to_current_line("std::complex<double> ");
                } break;

                default:
                {
                    SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
                };

            }

            this->current_file->append_to_current_line(parameter_variable_node->identifier);
            if (i < node->parameters.size() - 1)
            {
                this->current_file->append_to_current_line(", ");

            }

        }

        this->current_file->append_to_current_line(") -> ");

        Datatype function_datatype = variable_node->data_type;
        switch (function_datatype)
        {

            case Datatype::DATA_TYPE_VOID:
            {
                this->current_file->append_to_current_line("void ");
            } break;

            default:
            {
                SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
            };

        }

        this->current_file->insert_line_with_tabs("{");
        this->current_file->insert_blank_line();

        this->current_file->push_tabs();
        for (auto child : node->children) child->accept(this);
        this->current_file->pop_tabs();

        this->current_file->insert_blank_line();
        this->current_file->insert_line_with_tabs("};");
        this->current_file->insert_blank_line();

    }

    return;

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeExpressionStatement* node)
{

    this->current_file->insert_line_with_tabs("");
    node->expression->accept(this);
    this->current_file->append_to_current_line(";");

    return;

}

void TranspileCPPGenerator::    
visit(SyntaxNodeWhileStatement* node)
{

    this->current_file->insert_line_with_tabs("while (");
    node->expression->accept(this);
    this->current_file->append_to_current_line(")");
    this->current_file->insert_line_with_tabs("{");
    this->current_file->insert_blank_line();

    this->current_file->push_tabs();
    for (auto child : node->children) child->accept(this);
    this->current_file->pop_tabs();

    this->current_file->insert_blank_line();
    this->current_file->insert_line_with_tabs("}");
    this->current_file->insert_blank_line();

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodePloopStatement* node)
{

    this->current_file->insert_line_with_tabs("// This is a ploop statement, but ploop is not supported.");
    this->current_file->insert_line_with_tabs("for (");

    Datatype data_type = node->variable->data_type;
    switch (data_type)
    {

        case Datatype::DATA_TYPE_STRING:
        {
            this->current_file->append_to_current_line("std::string ");
        } break;

        case Datatype::DATA_TYPE_INTEGER:
        {
            this->current_file->append_to_current_line("int64_t ");
        } break;

        case Datatype::DATA_TYPE_REAL:
        {
            this->current_file->append_to_current_line("double ");
        } break;

        case Datatype::DATA_TYPE_COMPLEX:
        {
            this->current_file->append_to_current_line("std::complex<double> ");
        } break;

        default:
        {
            SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
        };

    }
    
    this->current_file->append_to_current_line(node->variable->identifier);
    this->current_file->append_to_current_line(" = ");
    node->start->accept(this);
    this->current_file->append_to_current_line("; ");
    this->current_file->append_to_current_line(node->variable->identifier);
    this->current_file->append_to_current_line(" < ");
    node->end->accept(this);
    this->current_file->append_to_current_line("; ");
    this->current_file->append_to_current_line(node->variable->identifier);
    this->current_file->append_to_current_line(" += ");
    node->step->accept(this);
    this->current_file->append_to_current_line(")");

    this->current_file->insert_line_with_tabs("{");
    this->current_file->insert_blank_line();
    this->current_file->push_tabs();

    for (auto child : node->children) child->accept(this);

    this->current_file->pop_tabs();
    this->current_file->insert_blank_line();
    this->current_file->insert_line_with_tabs("}");
    this->current_file->insert_blank_line();

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeLoopStatement* node)
{

    this->current_file->insert_line_with_tabs("for (");

    Datatype data_type = node->variable->data_type;
    switch (data_type)
    {

        case Datatype::DATA_TYPE_STRING:
        {
            this->current_file->append_to_current_line("std::string ");
        } break;

        case Datatype::DATA_TYPE_INTEGER:
        {
            this->current_file->append_to_current_line("int64_t ");
        } break;

        case Datatype::DATA_TYPE_REAL:
        {
            this->current_file->append_to_current_line("double ");
        } break;

        case Datatype::DATA_TYPE_COMPLEX:
        {
            this->current_file->append_to_current_line("std::complex<double> ");
        } break;

        default:
        {
            SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
        };

    }
    
    this->current_file->append_to_current_line(node->variable->identifier);
    this->current_file->append_to_current_line(" = ");
    node->start->accept(this);
    this->current_file->append_to_current_line("; ");
    this->current_file->append_to_current_line(node->variable->identifier);
    this->current_file->append_to_current_line(" < ");
    node->end->accept(this);
    this->current_file->append_to_current_line("; ");
    this->current_file->append_to_current_line(node->variable->identifier);
    this->current_file->append_to_current_line(" += ");
    node->step->accept(this);
    this->current_file->append_to_current_line(")");

    this->current_file->insert_line_with_tabs("{");
    this->current_file->insert_blank_line();
    this->current_file->push_tabs();

    for (auto child : node->children) child->accept(this);

    this->current_file->pop_tabs();
    this->current_file->insert_blank_line();
    this->current_file->insert_line_with_tabs("}");
    this->current_file->insert_blank_line();

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeVariableStatement* node)
{

    this->current_file->insert_line_with_tabs("");

    if (node->structure_type == Structuretype::STRUCTURE_TYPE_SCALAR ||
        node->structure_type == Structuretype::STRUCTURE_TYPE_STRING)
    {
        switch (node->data_type)
        {

            case Datatype::DATA_TYPE_STRING:
            {
                this->current_file->append_to_current_line("std::string ");
            } break;

            case Datatype::DATA_TYPE_INTEGER:
            {
                this->current_file->append_to_current_line("int64_t ");
            } break;

            case Datatype::DATA_TYPE_REAL:
            {
                this->current_file->append_to_current_line("double ");
            } break;

            case Datatype::DATA_TYPE_COMPLEX:
            {
                this->current_file->append_to_current_line("std::complex<double> ");
            } break;

            case Datatype::DATA_TYPE_UNKNOWN:
            {
                this->current_file->append_to_current_line("/*unknown*/ int64_t ");
            } break;

            default:
            {
                this->current_file->append_to_current_line("/*unknown*/ int64_t ");
                //SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
            };

        }
    }
    else
    {

        this->current_file->append_to_current_line("dvector<double, ");
        this->current_file->append_to_current_line(std::to_string(node->structure_length));
        this->current_file->append_to_current_line("> ");

    }

    this->current_file->append_to_current_line(node->identifier);

    if (node->expression != nullptr)
    {

        this->current_file->append_to_current_line(" = ");
        node->expression->accept(this);

    }

    this->current_file->append_to_current_line(";");

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeScopeStatement* node)
{

    this->current_file->insert_blank_line();
    this->current_file->insert_line_with_tabs("{");
    this->current_file->insert_blank_line();
    this->current_file->push_tabs();

    for (auto child : node->children) child->accept(this);

    this->current_file->pop_tabs();
    this->current_file->insert_blank_line();
    this->current_file->insert_line_with_tabs("}");
    this->current_file->insert_blank_line();

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeConditionalStatement* node)
{

    this->current_file->insert_line_with_tabs("if (");
    node->expression->accept(this);
    this->current_file->append_to_current_line(")");
    this->current_file->insert_line_with_tabs("{");
    this->current_file->insert_blank_line();

    this->current_file->push_tabs();
    for (auto child : node->children) child->accept(this);
    this->current_file->pop_tabs();

    this->current_file->insert_blank_line();
    this->current_file->insert_line_with_tabs("}");

    SyntaxNodeConditionalStatement *current = node->next;
    while (current != nullptr)
    {

        
        this->current_file->insert_line_with_tabs("else if (");
        current->expression->accept(this);
        this->current_file->append_to_current_line(")");
        this->current_file->insert_line_with_tabs("{");
        this->current_file->insert_blank_line();
        this->current_file->push_tabs();
        for (auto child : current->children) child->accept(this);
        this->current_file->pop_tabs();

        this->current_file->insert_blank_line();
        this->current_file->insert_line_with_tabs("}");

        current = current->next;

    }

    this->current_file->insert_blank_line();

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeReadStatement* node)
{

    this->current_file->insert_line_with_tabs("std::cin >> ");
    this->current_file->append_to_current_line(node->identifier);

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeWriteStatement* node)
{

    for (auto child : node->expressions)
    {

        this->current_file->insert_line_with_tabs("std::cout << ");
        child->accept(this);
        this->current_file->append_to_current_line(";");

    }

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeExpression* node)
{

    node->expression->accept(this);

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeProcedureCall* node)
{

    this->current_file->append_to_current_line(node->identifier);
    this->current_file->append_to_current_line("(");

    for (i32 i = 0; i < node->arguments.size(); ++i)
    {

        SyntaxNode *argument = node->arguments[i];
        argument->accept(this);

        if (i < node->arguments.size() - 1)
            this->current_file->append_to_current_line(", ");

    }

    this->current_file->append_to_current_line(")");

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeAssignment* node)
{

    node->left->accept(this);

    this->current_file->append_to_current_line(" = ");

    node->right->accept(this);

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeEquality* node)
{

    node->left->accept(this);

    Operationtype operation_type = node->operation;
    switch (operation_type)
    {

        case Operationtype::OPERATION_TYPE_EQUALS:
        {

            this->current_file->append_to_current_line(" == ");

        } break;

        case Operationtype::OPERATION_TYPE_NOT_EQUALS:
        {

            this->current_file->append_to_current_line(" != ");

        } break;

        default:
        {
            SF_ASSERT(!"Invalid operation type defined for this expression type.");
        };

    }

    node->right->accept(this);
    

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeComparison* node)
{

    node->left->accept(this);

    Operationtype operation_type = node->operation;
    switch (operation_type)
    {

        case Operationtype::OPERATION_TYPE_LESS_THAN:
        {

            this->current_file->append_to_current_line(" < ");

        } break;

        case Operationtype::OPERATION_TYPE_LESS_THAN_OR_EQUAL:
        {

            this->current_file->append_to_current_line(" <= ");

        } break;

        case Operationtype::OPERATION_TYPE_GREATER_THAN:
        {

            this->current_file->append_to_current_line(" > ");

        } break;

        case Operationtype::OPERATION_TYPE_GREATER_THAN_OR_EQUAL:
        {

            this->current_file->append_to_current_line(" >= ");

        } break;

        default:
        {
            SF_ASSERT(!"Invalid operation type defined for this expression type.");
        };

    }

    node->right->accept(this);
    

    return;

}

void TranspileCPPGenerator::    
visit(SyntaxNodeConcatenation* node)
{

    node->left->accept(this);

    Operationtype operation_type = node->operation;
    switch (operation_type)
    {

        case Operationtype::OPERATION_TYPE_CONCATENATE:
        {

            this->current_file->append_to_current_line(" << ");

        } break;

        default:
        {
            SF_ASSERT(!"Invalid operation type defined for this expression type.");
        };

    }

    node->right->accept(this);

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeTerm* node)
{

    node->left->accept(this);

    Operationtype operation_type = node->operation;
    switch (operation_type)
    {

        case Operationtype::OPERATION_TYPE_ADDITION:
        {

            this->current_file->append_to_current_line(" + ");

        } break;

        case Operationtype::OPERATION_TYPE_SUBTRACTION:
        {

            this->current_file->append_to_current_line(" - ");

        } break;

        default:
        {
            SF_ASSERT(!"Invalid operation type defined for this expression type.");
        };

    }

    node->right->accept(this);

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeFactor* node)
{

    node->left->accept(this);

    Operationtype operation_type = node->operation;
    switch (operation_type)
    {

        case Operationtype::OPERATION_TYPE_MULTIPLICATION:
        {

            this->current_file->append_to_current_line(" * ");

        } break;

        case Operationtype::OPERATION_TYPE_DIVISION:
        {

            this->current_file->append_to_current_line(" / ");

        } break;

        default:
        {
            SF_ASSERT(!"Invalid operation type defined for this expression type.");
        };

    }

    node->right->accept(this);

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeMagnitude* node)
{

    this->current_file->append_to_current_line("std::pow(");
    node->left->accept(this);

    this->current_file->append_to_current_line(", ");
    node->right->accept(this);
    this->current_file->append_to_current_line(")");

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeExtraction* node)
{

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeDerivation* node)
{

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeUnary* node)
{

    this->current_file->append_to_current_line(" -");
    node->expression->accept(this);

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeFunctionCall* node)
{

    this->current_file->append_to_current_line("fn_");
    this->current_file->append_to_current_line(node->identifier);
    this->current_file->append_to_current_line("(");

    for (i32 i = 0; i < node->arguments.size(); ++i)
    {

        SyntaxNode *argument = node->arguments[i];
        argument->accept(this);

        if (i < node->arguments.size() - 1)
            this->current_file->append_to_current_line(", ");

    }

    this->current_file->append_to_current_line(")");

    return;

}

void TranspileCPPGenerator::    
visit(SyntaxNodeArrayIndex* node)
{

    // Multidimensional indexing is kinda weird.
    // TODO(Chris): z * (y*x) + y * (x) + x eg. for third dimensionals.
    //              We will need to do this later, for now assume single dimension.
    this->current_file->append_to_current_line(node->identifier);
    this->current_file->append_to_current_line("[");
    for (auto child : node->indices) child->accept(this);
    this->current_file->append_to_current_line("]");

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodePrimary* node)
{

    Primarytype primary_type = node->primarytype;
    switch (primary_type)
    {

        case Primarytype::PRIMARY_TYPE_INTEGER:
        {
            this->current_file->append_to_current_line(node->primitive);
        } break;

        case Primarytype::PRIMARY_TYPE_REAL:
        {
            this->current_file->append_to_current_line(node->primitive);
        } break;

        case Primarytype::PRIMARY_TYPE_COMPLEX:
        {
            this->current_file->append_to_current_line("std::complex<double>(0.0, ");
            string complex_primitive = node->primitive;
            complex_primitive.pop_back();
            this->current_file->append_to_current_line(complex_primitive);
            this->current_file->append_to_current_line(")");
        } break;

        case Primarytype::PRIMARY_TYPE_IDENTIFIER:
        {
            this->current_file->append_to_current_line(node->primitive);
        } break;

        case Primarytype::PRIMARY_TYPE_STRING:
        {
            this->current_file->append_to_current_line("\"");
            this->current_file->append_to_current_line(node->primitive);
            this->current_file->append_to_current_line("\"");
        } break;
    }

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeGrouping* node)
{

    this->current_file->append_to_current_line("( ");
    node->expression->accept(this);
    this->current_file->append_to_current_line(" )");

    return;
}


