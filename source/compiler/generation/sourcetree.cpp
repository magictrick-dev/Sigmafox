#include <compiler/generation/sourcetree.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

Sourcetree::
Sourcetree(string output_directory)
{
    this->output_directory = output_directory;
}

Sourcetree::
~Sourcetree()
{

}

bool Sourcetree::
insert_source(GeneratableSourcefile *source)
{

    if (this->map.find(source->get_file_path()) == this->map.end())
    {

        this->map[source->get_file_path()] = source;
        return true;

    }

    return false;
}

bool Sourcetree::
source_exists(string source_name) const
{

    if (this->map.find(source_name) == this->map.end())
    {

        return false;

    }


    return true;
}

bool Sourcetree::
commit() const
{

    for (auto source_file : this->map)
    {

        string output_path = this->output_directory;
        output_path += "/";
        output_path += source_file.second->get_file_path();
        std::cout << "-- Outputting: " << output_path << std::endl;

        std::filesystem::path fpath(output_path);
        std::filesystem::path parent_fpath = fpath.parent_path();
        if (!parent_fpath.empty() && !std::filesystem::exists(parent_fpath))
        {
            if (!std::filesystem::create_directories(parent_fpath))
            {
                std::cout << "-- Unable to create required directories for source." << std::endl;
            }
        }

        // Check if file exists and delete it
        if (std::filesystem::exists(output_path)) 
        {
            std::filesystem::remove(output_path);
        }

        std::ofstream output_file(output_path);
        if (!output_file.is_open()) return false;

        output_file << source_file.second->get_source();
        output_file.close();

    }

    return true;
}

