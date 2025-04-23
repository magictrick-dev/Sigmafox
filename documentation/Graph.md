# DependencyGraph Class Documentation

The `DependencyGraph` is a utility for managing and validating dependencies (e.g., between source files or modules). 
It models the relationships as a directed graph and provides detection for common dependency issues such as circular references or duplicates.
The dependency graph constructures a tree, where each node may be a child of another while also being
adjacent to its siblings. The dependency graph itself does not manage any state aside from what source
files are included where.

---

## Enum: `DependencyResult`

Represents the outcome of adding a dependency.

- `DEPENDENCY_SUCCESS` – Dependency was added successfully.
- `DEPENDENCY_PARENT_NOT_FOUND` – The parent node does not exist in the graph.
- `DEPENDENCY_SELF_INCLUDED` – A node attempted to include itself.
- `DEPENDENCY_ALREADY_INCLUDED` – The dependency already exists.
- `DEPENDENCY_IS_CIRCULAR` – A circular dependency was detected.

---

## Struct: `DependencyNode`

Represents a node in the dependency graph.

- `shared_ptr<DependencyNode> parent` – The node's parent in the dependency tree.
- `vector<shared_ptr<DependencyNode>> children` – List of this node’s children.
- `string path` – Path or identifier associated with the node.

---

## Class: `DependencyGraph`

Manages a tree of dependencies and provides operations to query and add relationships.

### Constructors / Destructors

- `DependencyGraph()` – Initializes an empty dependency graph.
- `~DependencyGraph()` – Virtual destructor.

### Core Methods

- `void set_root(string path)`  
  Sets the root node of the dependency graph.

- `string get_root_path() const`  
  Returns the path associated with the root node.

- `DependencyResult add_dependency(string parent, string child)`  
  Attempts to add a dependency from `parent` to `child`. Returns a `DependencyResult` describing the result.

- `bool has_dependency(string parent, string child)`  
  Returns `true` if a direct dependency exists from `parent` to `child`.

- `bool dependency_exists(string dependency)`  
  Checks whether a node with the given path exists in the graph.

---

### Protected Members

- `unordered_map<string, shared_ptr<DependencyNode>> nodes`  
  Maps paths to dependency nodes for quick lookup.

- `shared_ptr<DependencyNode> root`  
  Root node of the dependency graph.

- `vector<string> includes`  
  Tracks included paths to assist with inclusion and duplication checks.

