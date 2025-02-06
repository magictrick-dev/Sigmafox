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
        static inline DefaultAllocator  Allocator;

        static inline bool              runtime_warnings_as_errors;
        static inline std::string       runtime_path;
        static inline std::string       output_path;

};

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
