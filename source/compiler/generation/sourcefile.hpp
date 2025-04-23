#ifndef SIGMAFOX_COMPILER_GENERATION_SOURCEFILE_HPP
#define SIGMAFOX_COMPILER_GENERATION_SOURCEFILE_HPP
#include <definitions.hpp>
#include <stack>

using std::stack;

class GeneratableRegion
{
    public:
                    GeneratableRegion();
        virtual    ~GeneratableRegion();

        void        add_line(const string& line);
        void        add_to_current_line(const string& append);

        string&     get_current_line();
        string      merge_all_lines() const;

    protected:
        vector<string> source_lines;

};

class GeneratableSourcefile
{

    public:
                    GeneratableSourcefile(string file_path, string file_name);
        virtual    ~GeneratableSourcefile();

        string      get_file_name() const;
        string      get_file_path() const;

        void        push_region_as_head();
        void        push_region_as_body();
        void        push_region_as_foot();
        void        pop_region();

        GeneratableSourcefile&      insert_line(const string& line);
        GeneratableSourcefile&      insert_line_with_tabs(const string& line);
        GeneratableSourcefile&      append_to_current_line(const string& contents);
        GeneratableSourcefile&      insert_blank_line();
        GeneratableSourcefile&      push_tabs();
        GeneratableSourcefile&      pop_tabs();

        string      get_source() const;

    protected:
        i32         tab_count;
        i32         tab_size;
        string      file_path;
        string      file_name;

        stack<GeneratableRegion*> region_stack;
        GeneratableRegion head;
        GeneratableRegion body;
        GeneratableRegion foot;

};

#endif
