#include <compiler/parser/visitor.hpp>

SyntaxNodeVisitor::SyntaxNodeVisitor()                                  { return; }
SyntaxNodeVisitor::~SyntaxNodeVisitor()                                 { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeRoot* node)                     { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeModule* node)                   { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeMain* node)                     { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeIncludeStatement* node)         { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeParameter* node)                { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeFunctionStatement* node)        { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeProcedureStatement* node)       { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeExpressionStatement* node)      { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeProcedureCallStatement* node)   { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeWhileStatement* node)           { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeLoopStatement* node)            { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeVariableStatement* node)        { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeScopeStatement* node)           { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeConditionalStatement* node)     { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeReadStatement* node)            { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeWriteStatement* node)           { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeExpression* node)               { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeAssignment* node)               { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeEquality* node)                 { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeComparison* node)               { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeTerm* node)                     { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeFactor* node)                   { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeMagnitude* node)                { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeExtraction* node)               { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeDerivation* node)               { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeUnary* node)                    { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeFunctionCall* node)             { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeArrayIndex* node)               { return; }
void SyntaxNodeVisitor::visit(SyntaxNodePrimary* node)                  { return; }
void SyntaxNodeVisitor::visit(SyntaxNodeGrouping* node)                 { return; }