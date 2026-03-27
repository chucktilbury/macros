# Macros
This is a simple macro language that performs a similar function to the C pre-processor. The reason that I am making this is that I am not happy with the way the C pre-processor operates for the assembler I am working on. There are a lot of features that I don't need and some of the features do not really work the way I want them to. For example, a multi-line macro should stay multi-line with newlines preserved according to the original text. Also, tracking line numbers and the open file is not detailed enough to publish good error messages.

These simple items are all that I need for the assembler that I am working on.
* **``.define``** -- Define a parameterized macro.
* **``.import``** -- Include a file as if it was part of the current one.
* **``.if/.else/``** -- Implement the if/else construct.
* C and C++ comments are supported.
* Gratuitous blank lines are not emitted.
* Output is on stdout if not otherwise specified.
* Input is from stdin unless otherwise specified.
* The macro processor is single-pass. Symbols must be defined before they are referenced.
* All symbols a global in scope and must be globally unique.
* All macros must have a parameter list, which could be empty.
* When a macro is expanded, the line number and file name of the file where it was defined are given in a ``#`` comment.
* When the ``@`` character is encountered in normal text, it is taken to be a macro. If there is no macro defined for the name, then it is simply copied to the output without modification.
  * If a macro has blank text and it is expanded in the normal text then the reference it simply deleted.
* The ``-I`` command line parameter is supported.
  * There are no "system" include directories. (except the current directory)
  * The current directory is the first place to look by default.
  * Example: ``-I../..``
    * A list can be specified using the ``:`` or multiple instances can create a list.
    * Space after the ``-I`` is optional.
* The ``-D`` command line parameter is supported.
  * Multi-line macros are not supported from the command line.
  * Parameterized macros are not supported from the command line.
  * A macro may be defined with or without a value.
  * Macros deifined on the command line don't have a parameter list, but they must be referenced with an empty parameter list.
  * Example: ``-Dmacro_name=macro_value``
    * Space after the ``-D`` is optional.
    * The value is optional.
    * The ``=`` is required if a value is present.
    * In the example, the macro must be referenced as ``macro_name()`` with a blank parameter list.
* ``.define`` statements can have ``.if`` statements in it and ``.if`` statements can have ``.define`` statements in it. But ``.define`` statements cannot have ``.define``.

## .define
This defines a parameterized macro, similar to the C pre-processor. It only does simple text substitutions.
* The ``-D`` command line parameter is supported.
* Does not support varadic macros
* Does not have "stringify"
* Does allow expansion of other macros as part of the definition.
* The at-sign ``@`` appends text. (instead of the CPP ``#``)
* All macros must have a parameter list that can be empty.
* Symbols must be defined before referencing. If a symbol is not defined, then it is taken as normal text.

```
.define blank_macro {}
.define another_blank
.define a_macro {_with_more_text}
.define another(a,b,c) {
  a_line_for (a)
  a_line_for (b)
  a_line_for (c @a_macro)
  @not_defined
  @blank_macro
}

\\ @another(foo, bar, baz) expands to
  a_line_for (foo)
  a_line_for (bar)
  a_line_for (baz_with_more_text)
  @not_defined

```

## .import
The brings another file into the input stream exactly as if it was part of it. The current input stream is suspended until the imported file has been completely read and then the original stream is resumed.
* The ``-I`` command line parameter is supported.
* There are no "standard" include directories like in the C pre-processor.

```
.import "openable_file_name.xxx"

```

## .if/.else
This allows text to be included or excluded from the input stream based upon a comparison expression. If the expression does not evaluate to TRUE then the text is ignored as if it was a comment.
* If a name is not defined then it is FALSE. If a reference in an expression has been defined, then it's TRUE.
* All comparisons are binary. All words in an expression are taken to be macros.
* The '@' symbol is a syntax error in an expression.
* A expression blank operand evaluates to false.
* Operators are ``AND``, ``OR``, and ``NOT``.

```
// since the first clause is true, the other clauses are ignored
.if (a_macro) {
// include this text because a_macro has been defined
}
.else(NOT a_macro) {
// exclude this text because a_macro has been defined and the NOT operator applies
}
.else(blart AND a_macro) {
// exclude this text because "blart" is not defined as a macro.
}
.else {
// include this text only if all of the other clauses evaluate to FALSE
}
.endif

.if(NOT blart AND a_macro) {
// This evaluates to TRUE because (blart AND _a_macro) is FALSE and
// the NOT operator has the lowest precedence.
}

.if(NOT (a_macro) AND blart) {
// This evaluates to FALSE because the parenthesis changes the precedence of the operands.
}

```


