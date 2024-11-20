*[Return to Index](../README.md)*

# Scanner API

The scanner API is the first stage of the transpiler.

The tokenization process begins by taking entire string that composes the source file and
looks for a set characters that match a particular token specification. This process is
rather efficient in performance (roughly equates to O(n) where n is the
number of characters in the source file).

<p align="center"><img src="./images/scanner_diagram.svg" /></p>

In the above example, the string `24 + 2 - (4 * 3) + -2` is broken down into 12 tokens. The
first token the scanner matched is 24, which the scanner matched by examining the first 
character in the sequence, a number. It will then continue to consume characters, 
appending additional numbers until it finds the first non-numerical character and creates 
a token. A similar  process is done for each type of token that fits the language 
specification. Most tokens are easily identifiable because they are composed of single
characters, such as `+`, `-`, `*`, `/`, `<` `>`, etc. Other tokens require additional
logic to properly determine if it fits the language specification such as strings, identifiers,
keywords, and multi-character symbols like `<=` and `>=`.

In the event that the scanner matches a token it doesn't recognize, the token
is placed in an error list and continues scanning ahead.

<p align="center"><img src="./images/scanner_error.svg" /></p>

When the scanner is complete, it returns a boolean which indicates if there are any errors.
It's beneficial to display the entire list of errors that the scanner generated, including
line numbers and the symbol it recognized so the programmer can quickly find and remedy them.
Generally, the list of tokens that is generated after parsing holds little meaning to the user
if there are any errors at this stage of the transpiler, so we can safely exit the program after
displaying the errors out to the user.


