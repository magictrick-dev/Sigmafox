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

    // Copy the library files.
    std::filesystem::path sourcePath("./library/dvector.hpp");
    std::filesystem::path targetPath(this->output_directory + "./library/dvector.hpp");
    std::filesystem::path targetDir = targetPath.parent_path();

    if (!std::filesystem::exists(sourcePath)) {
        std::cerr << "Source file does not exist: " << sourcePath << std::endl;
        return false;
    }

    if (!targetDir.empty() && !std::filesystem::exists(targetDir)) {
        if (!std::filesystem::create_directories(targetDir)) {
            std::cerr << "Failed to create target directory: " << targetDir << std::endl;
            return false;
        }
    }

    std::ifstream inFile(sourcePath, std::ios::binary);
    if (!inFile) {
        std::cerr << "Failed to open source file: " << sourcePath << std::endl;
        return false;
    }

    std::ofstream outFile(targetPath, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open target file: " << targetPath << std::endl;
        return false;
    }

    std::cout << "-- Outputting: " << this->output_directory << "/dvector.hpp" << std::endl;
    outFile << inFile.rdbuf();

    return true;
}

