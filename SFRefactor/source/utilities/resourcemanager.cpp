#include <platform/filesystem.h>
#include <platform/system.h>
#include <utilities/resourcemanager.h>

using namespace Sigmafox;

// --- Resource Implementation -------------------------------------------------
//
// Resources use virtual allocation over standard memory allocation (SF_MEMORY_ALLOC)
// since they will persist longer than usual.
//

Resource::
Resource(const Filepath& path)
{

    SF_ASSERT(path.is_valid_file());
    
    this->source_path   = path;
    this->buffer_ptr    = nullptr;
    this->buffer_size   = 0;
    this->loaded        = 0;
    this->size          = file_size(path.c_str());

}

Resource::
~Resource()
{

    this->release();

}

bool Resource::
load()
{

    // Loading when its released is bad. Gobble some memory first.
    if (this->is_released()) this->reserve();
    SF_ASSERT(this->buffer_ptr != nullptr);
    
    // TODO(Chris): We should read-stream this to ensure we get the read size.
    //              The platform may not guarantee we get a full write, so we let it
    //              assert on the edge case.
    u64 read = file_read_all(this->source_path.c_str(), this->buffer_ptr, this->buffer_size);
    SF_ASSERT(read == this->size);
    this->loaded = true;
    return true;

}

bool Resource::
reserve()
{

    // Always reserve one extra byte and enforce it to be null-terminated block.
    // We make this assumption here since we are primarily focused on text files.
    // We don't supply a memory offset; let the OS figure that out.
    this->buffer_ptr = system_virtual_alloc(0, this->size + 1);
    if (this->buffer_ptr == nullptr) return false;
    ((u8*)this->buffer_ptr)[this->size] = '\0';
    return true;

}

bool Resource::
release()
{

    if (buffer_ptr != nullptr)
    {
        system_virtual_free(this->buffer_ptr);
        this->buffer_ptr = nullptr;
        this->buffer_size = 0;
        this->loaded = 0;
    }

    return true;

}

bool Resource::
is_loaded() const
{

    return this->loaded;

}

bool Resource::
is_released() const
{

    bool result = (this->buffer_ptr == nullptr);
    return result;

}

const Filepath& Resource::
get_path() const
{
    return this->source_path;
}

u64 Resource::
get_size() const
{

    return this->size;

}

void* Resource::
get()
{

    return this->buffer_ptr;

}

// --- Resource Manager Implementation -----------------------------------------

ResourceManager::
ResourceManager()
{

    return;
}

ResourceManager::
~ResourceManager()
{

    // Release all resources.
    for (auto& res : this->resources) res.release();
    return;

}

ResourceHandle ResourceManager::
create_resource_handle(const Filepath &path)
{

    if (!path.is_valid_file()) return -1;

    // Search if the resource exists. We only need one loaded at a time.
    for (i64 i = 0; i < this->resources.size(); ++i)
    {
        if (this->resources[i].get_path() == path)
        {
            return i;
        }
    }

    // Generate the resource otherwise.
    i64 result = this->resources.size();
    this->resources.emplace_back(path);
    Resource& current_resource = this->resources.back();

    return result;
}

bool ResourceManager::
resource_handle_is_valid(ResourceHandle handle)
{

    if (handle < 0 || handle >= this->resources.size())
        return false;

    if (!this->resources[handle].get_path().is_valid_file())
        return false;

    return true;
}

const Filepath& ResourceManager::
get_resource_path(ResourceHandle handle) const
{
    
    SF_ASSERT(handle >= 0);
    SF_ASSERT(handle < this->resources.size());

    return this->resources[handle].get_path();

}

u64 ResourceManager::
get_resource_size(ResourceHandle handle) const
{

    SF_ASSERT(handle >= 0);
    SF_ASSERT(handle < this->resources.size());

    return this->resources[handle].get_size();

}

void ResourceManager::
reserve(ResourceHandle handle)
{

    SF_ASSERT(handle >= 0);
    SF_ASSERT(handle < this->resources.size());
    SF_ASSERT(this->resources[handle].reserve());


}

void ResourceManager::
release(ResourceHandle handle)
{

    SF_ASSERT(handle >= 0);
    SF_ASSERT(handle < this->resources.size());
    SF_ASSERT(this->resources[handle].release());

}

bool ResourceManager::
is_reserved(ResourceHandle handle) const
{

    SF_ASSERT(handle >= 0);
    SF_ASSERT(handle < this->resources.size());
    return !this->resources[handle].is_released();

}

void ResourceManager::
load_synchronously(ResourceHandle handle)
{

    SF_ASSERT(handle >= 0);
    SF_ASSERT(handle < this->resources.size());
    SF_ASSERT(this->resources[handle].load());

}

bool ResourceManager::
is_loaded(ResourceHandle handle) const
{

    SF_ASSERT(handle >= 0);
    SF_ASSERT(handle < this->resources.size());
    return this->resources[handle].is_loaded();
    
}

void* ResourceManager::
get_resource(ResourceHandle handle)
{

    SF_ASSERT(handle >= 0);
    SF_ASSERT(handle < this->resources.size());
    return this->resources[handle].get();

}

ccptr ResourceManager::
get_resource_as_text(ResourceHandle handle)
{

    return (ccptr)this->get_resource(handle);

}
