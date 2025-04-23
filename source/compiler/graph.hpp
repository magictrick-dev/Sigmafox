#ifndef SIGMAFOX_COMPILER_GRAPH_HPP
#define SIGMAFOX_COMPILER_GRAPH_HPP
#include <definitions.hpp>

enum class DependencyResult
{
    DEPENDENCY_SUCCESS,
    DEPENDENCY_PARENT_NOT_FOUND,
    DEPENDENCY_SELF_INCLUDED,
    DEPENDENCY_ALREADY_INCLUDED,
    DEPENDENCY_IS_CIRCULAR,
};

struct DependencyNode
{
    shared_ptr<DependencyNode> parent;
    vector<shared_ptr<DependencyNode>> children;
    string path;
};

class DependencyGraph
{
    public:
                        DependencyGraph();
        virtual        ~DependencyGraph();

        void            set_root(string path);
        string          get_root_path() const;

        DependencyResult    add_dependency(string parent, string child);
        bool                has_dependency(string parent, string child);
        bool                dependency_exists(string dependency);

    protected:
        unordered_map<string, shared_ptr<DependencyNode>> nodes;
        shared_ptr<DependencyNode> root;
        vector<string> includes;

};

#endif
