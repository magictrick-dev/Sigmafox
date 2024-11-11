#include <environment.h>
#include <compiler/tokenizer.h>

using namespace Sigmafox;

Tokenizer::
Tokenizer(const Filepath& path)
{

    // Set the path.
    this->path = path;

    // Ensures the file is loaded through the global resource manager.
    ResourceManager& resmanager = ApplicationParameters::get().get_resource_manager();
    ResourceHandle handle = resmanager.create_resource_handle(path);
    SF_ASSERT(resmanager.resource_handle_is_valid(handle));
    resmanager.load_synchronously(handle);
    this->resource = handle;
    this->source = resmanager.get_resouce_as_text(this->resource);

    // Set our token buffers.
    this->previous_token    = &token_buffer[0];
    this->current_token     = &token_buffer[1];
    this->next_token        = &token_buffer[2];

    // Valid initialize our tokens to EOF such that they're known values.
    for (i32 i = 0; i < 3; ++i)
    {
        this->token_buffer[i].type      = TokenType::TOKEN_EOF;
        this->token_buffer[i].resource  = this->resource;
        this->token_buffer[i].offset    = 0;
        this->token_buffer[i].length    = 0;
    }

}

Tokenizer::
~Tokenizer()
{



}


