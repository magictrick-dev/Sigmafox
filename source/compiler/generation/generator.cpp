#include <iostream>
#include <compiler/generation/generator.hpp>

// --- Core Transpiler Routines ------------------------------------------------

TranspileCPPGenerator::
TranspileCPPGenerator()
{

    this->source_files.emplace_back("main.cpp", "main.cpp");
    this->main_file             = &this->source_files[0];
    this->current_file          = &this->source_files[0];
    this->source_stack_index    = 0;
    
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
        
        std::cout << file.get_source() << std::endl;

    }

}

void TranspileCPPGenerator::
generate_files()
{

    return;

}

// --- Visitor Routines --------------------------------------------------------

void TranspileCPPGenerator::    
visit(SyntaxNodeRoot* node)
{

    this->current_file->push_region_as_head();
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

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeModule* node)
{

    SF_NO_IMPL("Not yet.");

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeMain* node)
{

    this->current_file->push_region_as_body();
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

    SF_NO_IMPL(!"Not yet.");

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
        this->current_file->insert_line_with_tabs("static inline ");

        Datatype function_datatype = variable_node->data_type;
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
        SF_NO_IMPL("Have yet to do inlined lambdas yet.");
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
        SF_NO_IMPL("Have yet to do inlined lambdas yet.");
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
visit(SyntaxNodeLoopStatement* node)
{

    return;
}

void TranspileCPPGenerator::    
visit(SyntaxNodeVariableStatement* node)
{

    this->current_file->insert_line_with_tabs("");

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

        default:
        {
            SF_ASSERT(!"Unimplemented or invalid datatype encountered.");
        };

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


