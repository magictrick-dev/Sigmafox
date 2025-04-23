#include <compiler/generation/sourcefile.hpp>
#include <iostream>

// --- Generatable Region Implementation ---------------------------------------

GeneratableRegion::
GeneratableRegion()
{

    return;

}

GeneratableRegion::
~GeneratableRegion()
{

    return;

}

void GeneratableRegion::
add_line(const string& line)
{

    source_lines.push_back(line);
    return;

}

void GeneratableRegion::
add_to_current_line(const string& append)
{

    if (source_lines.empty())
    {
        this->add_line(append);
    }

    this->source_lines[this->source_lines.size()-1] += append;
    return;

}

string& GeneratableRegion::
get_current_line()
{

    if (source_lines.empty())
    {
        this->add_line("");
    }

    string& result = this->source_lines[this->source_lines.size() - 1];
    return result;

}

string GeneratableRegion::
merge_all_lines() const
{

    string result;
    for (auto line : this->source_lines)
    {

        result += line;
        result += "\n";

    }

    return result;

}

// --- Generatable Sourcefile Implementation -----------------------------------

GeneratableSourcefile::
GeneratableSourcefile(string file_path, string file_name)
{

    this->tab_count     = 0;
    this->tab_size      = 4;
    this->file_path     = file_path;
    this->file_name     = file_name;

    this->region_stack.push(&this->head);

}

GeneratableSourcefile::
~GeneratableSourcefile()
{

    SF_ASSERT(this->region_stack.size() >= 1);
    this->region_stack.pop();
    SF_ASSERT(this->region_stack.empty());

}

string GeneratableSourcefile::
get_file_name() const
{

    return this->file_name;

}

string GeneratableSourcefile::
get_file_path() const
{

    return this->file_path;

}

void GeneratableSourcefile::
push_region_as_head()
{

    this->region_stack.push(&this->head);

}

void GeneratableSourcefile::
push_region_as_body()
{

    this->region_stack.push(&this->body);

}

void GeneratableSourcefile::
push_region_as_foot()
{

    this->region_stack.push(&this->foot);

}

void GeneratableSourcefile::
pop_region()
{

    this->region_stack.pop();

}


GeneratableSourcefile& GeneratableSourcefile::
insert_line(const string& line)
{

    SF_ASSERT(this->region_stack.size() > 0);

    GeneratableRegion *current_region = this->region_stack.top();
    SF_ENSURE_PTR(current_region);

    current_region->add_line(line);

    return *this;

}

GeneratableSourcefile& GeneratableSourcefile::
insert_line_with_tabs(const string& line)
{

    SF_ASSERT(this->region_stack.size() > 0);

    GeneratableRegion *current_region = this->region_stack.top();
    SF_ENSURE_PTR(current_region);

    current_region->add_line("");

    // NOTE(Chris): This is just bad performance, but I threw that idea out the
    //              window with this design.
    string tab_string;
    for (int i = 0; i < this->tab_count * this->tab_size; ++i)
    {
        tab_string += " ";
    }

    current_region->add_to_current_line(tab_string);
    current_region->add_to_current_line(line);

    return *this;

}

GeneratableSourcefile& GeneratableSourcefile::
append_to_current_line(const string& contents)
{

    SF_ASSERT(this->region_stack.size() > 0);

    GeneratableRegion *current_region = this->region_stack.top();
    SF_ENSURE_PTR(current_region);

    current_region->add_to_current_line(contents);

    return *this;

}

GeneratableSourcefile& GeneratableSourcefile::
insert_blank_line()
{

    SF_ASSERT(this->region_stack.size() > 0);

    GeneratableRegion *current_region = this->region_stack.top();
    SF_ENSURE_PTR(current_region);

    current_region->add_line("");

    return *this;

}

GeneratableSourcefile& GeneratableSourcefile::
push_tabs()
{

    this->tab_count++;

    return *this;

}

GeneratableSourcefile& GeneratableSourcefile::
pop_tabs()
{

    this->tab_count--;
    SF_ASSERT(this->tab_count >= 0); // Oopsie, mismatch tab push/pop.

    return *this;

}

string GeneratableSourcefile::
get_source() const
{

    string result;
    result += this->head.merge_all_lines();
    result += this->body.merge_all_lines();
    result += this->foot.merge_all_lines();

    return result;

}

