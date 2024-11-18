// --- Sigmafox Resource Manager Utility ---------------------------------------
//
// Written by Chris DeJong Nov. 2024 / Northern Illinois University
//
//      The resource manager utility abstracts away file loading processes and
//      creates an easy to use interface for managing read-only files without
//      having to perform tedious set-up and shut-down procedures.
//
// -----------------------------------------------------------------------------
#ifndef SIGMAFOX_UTILITIES_RESOURCEMANAGER_H
#define SIGMAFOX_UTILITIES_RESOURCEMANAGER_H
#include <definitions.hpp>
#include <utilities/path.hpp>
#include <iostream>
#include <vector>

typedef int64_t ResourceHandle;

class Resource
{
    public:
                    Resource(const Filepath &path);
        virtual    ~Resource();

        bool                reserve();
        bool                release();
        bool                load();

        bool                is_released() const;
        bool                is_loaded() const;

        const Filepath&     get_path() const;
        u64                 get_size() const;

        void*               get();

    protected:
        Filepath    source_path;

        void       *buffer_ptr;
        u64         buffer_size;
        u64         size;
        b32         loaded;

};

class ResourceManager
{

    public:
                    ResourceManager();
        virtual    ~ResourceManager();

        ResourceHandle  create_resource_handle(const Filepath &path);
        bool            resource_handle_is_valid(ResourceHandle handle);

        const Filepath& get_resource_path(ResourceHandle handle) const;
        u64             get_resource_size(ResourceHandle handle) const;

        void        reserve(ResourceHandle handle);
        void        release(ResourceHandle handle);
        bool        is_reserved(ResourceHandle handle) const;

        void        load_synchronously(ResourceHandle handle);
        bool        is_loaded(ResourceHandle handle) const;

        void*       get_resource(ResourceHandle handle);
        ccptr       get_resource_as_text(ResourceHandle handle);

    protected:
        Filepath null_path;
        std::vector<Resource> resources;

        friend inline std::ostream& operator<<(std::ostream& os, const ResourceManager& rhs);
};

inline std::ostream&
operator<<(std::ostream& os, const ResourceManager& rman)
{

    os << "Resource Manager State:" << std::endl;
    for (const auto& res : rman.resources)
    {
        std::cout << res.get_path() << std::endl;
        std::cout << "    - Reserved:   " << !res.is_released() << std::endl;
        std::cout << "    - Loaded:     " << res.is_loaded() << std::endl;
        std::cout << "    - Size:       " << ((double)res.get_size() / 1024.0) << "Kb" << std::endl;
    }

    return os;

}

#endif
