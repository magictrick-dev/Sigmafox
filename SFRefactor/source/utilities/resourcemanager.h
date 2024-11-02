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
#include <definitions.h>
#include <utilities/path.h>
#include <vector>

namespace Sigmafox
{

    typedef ResourceHandle uint64_t;

    class Resource
    {
        public:
                        Resource(const Filepath &path);
            virtual    ~Resource();

        protected:
            Filepath    source_path;

            void       *buffer_ptr;
            u64         buffer_commit;
            u64         size;

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
            /*
            void        load_asynchronously(ResourceHandle handle, u64 preload);
            void        load_wait_until(ResourceHandle handle, u64 amount);
            void        load_wait(ResourceHandle handle);
            */
            bool        is_loaded(ResourceHandle handle) const;

            void*       get_resource(ResourceHandle handle);
            ccptr       get_resource_as_text(ResourceHandle handle);

        protected:
            std::vector<Resource> resources;

    };

}

#endif
