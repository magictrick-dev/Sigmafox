#include <cstdio>
#include <compiler/parser.h>
#include <compiler/expressions.h>
#include <compiler/statements.h>

// --- Error Handling ----------------------------------------------------------
//
// Although in its infancy, error handling is an important part of the parser that
// needs to be done with some care to ensure consistency with each error being
// displayed to the user. For now, we just let things go since we're in the
// early stages.
//

void
parser_display_error(token *location, const char *reason)
{
    
    uint32_t column = token_column_number(location);
    uint32_t line = token_line_number(location);
    printf("%s(%d,%d) ERROR: %s\n", location->location, line, column, reason);

}

void
parser_display_warning(token *location, const char *reason)
{

    uint32_t column = token_column_number(location);
    uint32_t line = token_line_number(location);
    printf("%s(%d,%d) WARNING: %s\n", location->location, line, column, reason);

}

// --- Abstract Syntax Tree ----------------------------------------------------
//
// An abstract syntax tree takes the set of linear tokens and composes them into
// a tree that corresponds to the language grammar. The basis for the language is
// expressions. We use recursive descent to the generate the required grammar.
// 
// Important Grammar Notes
//
//      comment_stm:    Comments are among a few special cases in which we actually
//                      want to carray over into the AST for transpilation. We
//                      incorporate it into the syntax for this reason. Since comments
//                      are parsed as single tokens, their formatting carries over
//                      complete with new-lines and character formatting that was
//                      initially incorporated into the comment. This could potentially
//                      pause some unusual edge-cases that may need to be considered,
//                      but for now we let this go through as-is.
//
//      block_stm:      Block statements push a new scope onto the environment.
//                      The scope pops when the appropriate "endscope" is encountered.
//
//      while_stm:      The while loop follows traditional while-loop functionality
//                      by evaluating the truthy expressions.
//
//      loop_stm:       The syntax is strange, but expression 1 is the initial state,
//                      and expression 2 is the ending state, where the identifier is set
//                      to expression 1 and incremented by 1 (or the optional incremental expression)
//                      until it reaches expression 2. Loops can not prematurely exit,
//                      therefore, at the end of each iteration, it must reset the step
//                      to its initial value. Loops always iterate n-time, however big it
//                      was initially defined to be. The generating syntax needs to
//                      mimmick this behavior.
//
//      if_stm:         Both the if and elseif statements are tied together. Interestingly,
//      elseif_stm:     there is COSY equivalent for "OR" and "AND", which is kinda wack.
//                      These can be achieved through nesting, but that's not very pretty.
//                      Another thing is that there is no plain "ELSE" statement. Therefore,
//                      it relies on a chaining of "ELSEIF"s before "ENDIF" to get the affect
//                      desired. The simplest solution is the left the "IF" tree contain
//                      a list of ELSEIF branches rather than keeping a list of lists.
//                      The recursion depth can potentially explode, but that would require
//                      some gnarly elseif-blocks to get to that point.
//
//      procedure_stm:  The procedure statement is a bit unique. The actual grammar is
//                      easy to catch. However, invoking procedures is tricky. The head
//                      of the procedure statement is an identifier, which then needs
//                      to be check if:
//
//                          a. The procedure is in-scope.
//                          b. The parameters are appropriately defined.
//
//                      So effectively, we now have a new statement grammar to procedures,
//                      an invokeproc_stm or something along those lines to actually handle
//                      this. This behavior itself is fine since procedures don't exist
//                      in mathematical expressions.
//
//      invokeproc_stm: Evaluate identifier to determine if it is procedure, validate
//                      procedure arity.
//
// Complete Language Grammar
//
//      program                 : statement* EOF
//      statement               : comment_stm | declaration_stm | expression_stm | block_stm |
//                                while_stm | loop_stm | if_stm | procedure_stm | invokeproc_stm |
//      comment_stm             : "comment" statement*
//      procedure_stm           : "procedure" identifier (identifier*) ";"
//                                statement* "endprocedure" ;
//      invokeproc_stm          : identifier (identifier*) ;
//      block_stm               : "scope" statement* "endscope" ;
//      while_stm               : "while" expression ; statement* "endwhile" ;
//      loop_stm                : "loop" identifier expression expression ( expression )? ; 
//                                statement* "endwhile" ;
//      if_stm                  : "if" expression ; statement* (elseif_stm)? "endif" ;
//      elseif_stm              : "elseif" expression ; statement* (elseif_stm)?
//      declaration_stm         : "variable" IDENTIFIER expression ( expression )* ;
//      expression_stm          : expression ;
//
// Expression Grammar
//
//      expression              : assignment
//      assignment              : identifier ":=" assignment | equality
//      equality                : comparison ( ( "=" | "#" ) comparison )*
//      comparison              : term ( ( "<" | ">" | "<=" | ">=" ) term )*
//      term                    : factor ( ( "+" | "-" ) factor )*
//      factor                  : unary ( ( "*" | "/" ) unary )*
//      unary                   : ( "-" ) unary | primary
//      primary                 : NUMBER | STRING | identifier | "(" expression ")"
// 
//
// Grammar TODO List:
//
//      - For-loop syntax.
//      - If-statement syntax.
//      - Procedure syntax.
//

bool
parser_validate_token(parser_state *state, token_type type)
{
    
    uint32_t current_type = (*state->tokens)[state->current].type;
    if (current_type == token_type::END_OF_FILE) 
    {
        return false;
    }
    return (current_type == type);

}

bool
parser_check_token(parser_state *state, token_type type)
{
    uint32_t current_type = (*state->tokens)[state->current].type;
    return (current_type == type);
}

bool
parser_match_token(parser_state *state, token_type type)
{

    if (parser_validate_token(state, type))
    {
        state->current++;
        return true;
    }

    return false;

}

token*
parser_consume_token(parser_state *state, token_type type)
{

    if (parser_validate_token(state, type))
    {
        token* current_token = &(*state->tokens)[state->current];
        state->current++;
        return current_token;
    }

    return NULL;

}

token*
parser_get_current_token(parser_state *state)
{
    token* current_token = &(*state->tokens)[state->current];
    return current_token;
}

token*
parser_get_previous_token(parser_state *state)
{
    token* previous_token = &(*state->tokens)[state->current - 1];
    return previous_token;
}


void
parser_synchronize_state(parser_state *state)
{

    // Move forward until we synchronize to a valid position.
    uint32_t current_type = (*state->tokens)[state->current].type;
    while (current_type != token_type::SEMICOLON &&
           current_type != token_type::END_OF_FILE)
    {
        state->current += 1;
        current_type = (*state->tokens)[state->current].type;
    }

    // If we hit a semicolon, move passed it.
    if (current_type == token_type::SEMICOLON)
        state->current++;

    return;
}
