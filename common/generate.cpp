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
    SF_NOIMPL("To do.");
    return "";
}
