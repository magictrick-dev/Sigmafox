#include <generate.h>

// --- Base Generatable Class --------------------------------------------------

void Generatable::
docbox_append(std::string& str)
{
    this->documentation_box += str;
    return;
}

std::string& Generatable::
docbox()
{
    return this->documentation_box;
}

// --- Class Generation Utilities ----------------------------------------------

ClassDefinition::
ClassDefinition(Typename signature, SF_OPTIONAL ClassDefinition *parent)
{

    // If the class is inherited, we set that here.
    if (parent != NULL)
        this->inherits = parent;
    else
        this->inherits = NULL;

    // Ensure that we have a non-empty signature. Whether or not is valide beyond
    // that isn't our concern.
    SF_ASSERT(signature.is_valid());
    this->class_signature = signature;

}

Typename& ClassDefinition::
add(Typename property, EScope scope)
{

    switch (scope)
    {
        case EScope::Public:
        {
            this->properties_public.push_back(property);
            Typename& instance = this->properties_public.back();
            return instance;
        };
        case EScope::Private:
        {
            this->properties_private.push_back(property);
            Typename &instance = this->properties_private.back();
            return instance;
        };
        case EScope::Protected:
        {
            this->properties_protected.push_back(property);
            Typename& instance = this->properties_protected.back();
            return instance;
        };
    }

    SF_NOREACH("All enum conditions must be reached, so if we're here, we bungled.");
    static Typename no_reach = {};
    return no_reach;
}

Typename& ClassDefinition::
add_typename(EScope scope)
{

    Typename *instance = NULL;

    switch (scope)
    {
        case EScope::Public:
        {
            this->properties_public.emplace_back();
            instance = &this->properties_public.back();
            break;
        }
        case EScope::Private:
        {
            this->properties_private.emplace_back();
            instance = &this->properties_private.back();
            break;
        }
        case EScope::Protected:
        {
            this->properties_protected.emplace_back();
            instance = &this->properties_protected.back();
            break;
        }
        default:
        {
            SF_NOREACH("All enum conditions must be reached, so if we're here, we bungled.");
            instance = NULL;
        }
    }

    return *instance;
}

std::string ClassDefinition::
generate(size_t tab_depth)
{

    std::stringstream output;

    // Generate the class definition header.
    output << "class " << this->class_signature.type;
    if (this->inherits != NULL)
        output << " : public " << this->inherits->signature().type;
    output << std::endl;

    output << "{" << std::endl << std::endl;

    // -------------------------------------------------------------------------
    // Add all public attributes.
    output << "    public:" << std::endl;
    for (auto method : this->methods_public)
    {
        output << "        ";
        output << method.signature.type << " ";
        output << method.signature.name;

        output << "(";
        for (size_t idx = 0; idx < method.parameters.size(); ++idx)
        {
            output  << method.parameters[idx].type << " "
                    << method.parameters[idx].name;
            if (idx < method.parameters.size() - 1)
                output << ", ";
        }
        output << ");";
    }
    
    output << std::endl;

    for (auto property : this->properties_public)
    {
        output << "        ";
        output << property.type << " ";
        output << property.name << ";";
    }

    output << std::endl;

    // -------------------------------------------------------------------------
    // Add all protected attributes.
    output << "    protected:" << std::endl;
    for (auto method : this->methods_protected)
    {
        output << "        ";
        output << method.signature.type << " ";
        output << method.signature.name;

        output << "(";
        for (size_t idx = 0; idx < method.parameters.size(); ++idx)
        {
            output  << method.parameters[idx].type << " "
                    << method.parameters[idx].name;
            if (idx < method.parameters.size() - 1)
                output << ", ";
        }
        output << ");";
    }
    
    output << std::endl;

    for (auto property : this->properties_protected)
    {
        output << "        ";
        output << property.type << " ";
        output << property.name << ";";
    }

    output << std::endl;

    // -------------------------------------------------------------------------
    // Add all private attributes.
    output << "    private:" << std::endl;
    for (auto method : this->methods_private)
    {
        output << "        ";
        output << method.signature.type << " ";
        output << method.signature.name;

        output << "(";
        for (size_t idx = 0; idx < method.parameters.size(); ++idx)
        {
            output  << method.parameters[idx].type << " "
                    << method.parameters[idx].name;
            if (idx < method.parameters.size() - 1)
                output << ", ";
        }
        output << ");";
    }
    
    output << std::endl;

    for (auto property : this->properties_private)
    {
        output << "        ";
        output << property.type << " ";
        output << property.name << ";";
    }

    output << std::endl;

    output << "};" << std::endl;

    return output.str();
}
