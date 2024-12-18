#ifndef SIGMAFOX_ENVIRONMENT_H
#define SIGMAFOX_ENVIRONMENT_H
#include <string>
#include <utilities/memoryalloc.hpp>

#define SF_MEMORY_ALLOC(sz) (ApplicationParameters::Allocator.allocate(sz))
#define SF_MEMORY_FREE(ptr) (ApplicationParameters::Allocator.release(ptr))

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
        static inline DefaultAllocator Allocator;

    protected:
        inline              ApplicationParameters();
        inline virtual     ~ApplicationParameters();

    protected:
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
