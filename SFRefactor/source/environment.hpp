#ifndef SIGMAFOX_ENVIRONMENT_H
#define SIGMAFOX_ENVIRONMENT_H
#include <string>
#include <utilities/resourcemanager.hpp>

// --- ApplicationParameters Singleton -----------------------------------------
//
// The application parameters contains runtime configuration parameters and
// defaults that the core APIs need to see to perform critical procedures. The
// primary modifier for this is the CLI parser.
//

class ApplicationParameters
{

    public:
        static inline ApplicationParameters& get();

        inline ResourceManager& get_resource_manager() { return this->global_resource_manager; };

    protected:
        inline              ApplicationParameters();
        inline virtual     ~ApplicationParameters();

    protected:
        ResourceManager global_resource_manager;
        std::string output_name;
        std::string output_path;

};

inline ApplicationParameters& ApplicationParameters::
get()
{

    // Lazy-evaluated singleton.
    static ApplicationParameters* parameters;
    if (parameters == nullptr)
    {
        parameters = new ApplicationParameters();
    }

    return *parameters;

}

inline ApplicationParameters::
ApplicationParameters()
{

}

inline ApplicationParameters:: 
~ApplicationParameters()
{

}

// --- ApplicationStatistics Singleton -----------------------------------------
//
// Application statistics contains and stores loggable information.
//

class ApplicationStatistics
{

    public:
        static inline ApplicationStatistics& get();

    protected:
        inline              ApplicationStatistics();
        inline virtual     ~ApplicationStatistics();

};

inline ApplicationStatistics& ApplicationStatistics::
get()
{

    // Lazy-evaluated singleton.
    static ApplicationStatistics* parameters;
    if (parameters == nullptr)
    {
        parameters = new ApplicationStatistics();
    }

    return *parameters;

}

inline ApplicationStatistics::
ApplicationStatistics()
{

}

inline ApplicationStatistics:: 
~ApplicationStatistics()
{

}


#endif
