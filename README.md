# Macros
This is a simple macro language that performs a similar function to the C pre-processor. The reason that I am making this is that I am not happy with the way the C pre-processor operates for the assembler I am working on. There are a lot of features that I don't need and some of the features do not really work the way I want them to. For example, a multi-line macro should stay multi-line with newlines preserved according to the original text. Also, tracking line numbers and the open file is not detailed enough to publish good error messages.

The macros are processed iteratively. The file is processed repeatedly until there are no more changes possible. This allows macros to be nested and means that a name does not need to be defined before it is referenced. As long as a name is defined somewhere, it will be used according to its definition.

Take a look at the TODO.md file to check for current bugs or other actions that are pending.

These simple items are all that I need for the assembler that I am working on.
* **``.define``** -- Define a parameterized macro.
* **``.import``** -- Include a file as if it was part of the current one.
* **``.if/.else/``** -- Implement the if/else construct.
* C and C++ comments are supported.
* Gratuitous blank lines are not omitted.
* Output is on stdout if not otherwise specified by a command line option.
* Input is from a file that is required on the command line.
* The macro processor is multi-pass. Symbols do not have to be defined before they are referenced.
* All symbols a global in scope and must be globally unique.
* When the ``@`` character is encountered in normal text, it is taken to be a macro. If there is no macro defined for the name, then it is simply copied to the output without modification.
    * If a macro has blank text and it is expanded in the normal text then the reference it simply deleted.
* Errors abort processing.

## Command line
* Both long and short options are accepted.
  * Short options have an optional space or ``=`` between the name and the value.
  * Long options require a space or a ``=`` between the name and the value.
* Command line elements can appear in any order.

The third column is the type expected.
* ``[num]`` expects a signed decimal number.
* ``[strg]`` expects a string. If you give a number, it will be handled as a string.
* ``[sw]`` takes no value and will give a syntax error if one is provided.
* ``[list of xxx]`` takes a list of the specified type. If the option is provided multiple times then each one will be added to the list. Or one may use a ``:`` to separate items, same as a UNIX shell path variable.
* The ``(reqd)`` specifier means that the item is required and a syntax error is generated if it is missing.

```

$ macros -h

macros: v0.1
simple macro processor

Use: macros [options] [file name]

--------------------------------------------------------------------------------
-v --verbosity  [num]  Print more information
-I              [list of strg] Add to the import path
-D              [list of strg] Add a macro to the text
-o --ofile      [strg] Specify the output file name
-h --help       [sw]   Print this helpful information
-V --version    [sw]   Show the program version
--------------------------------------------------------------------------------
File name(s) to input (reqd)
--------------------------------------------------------------------------------

```
* The ``-v`` option sets the verbosity of the output. The default value is ``1``.
  * Values >= 50 prints more than you care to know.
  * Value of 0 is completely silent except for errors and warnings.
* The ``-I`` command line parameter is supported.
  * The path and directories are recursively searched for files.
  * The environment variable ``MACRO_PATH`` is added to the import path.
  * The current directory is the first place to look by default.
  * Example: ``-I../..``
    * A list can be specified using the ``:`` or multiple instances can create a list.
    * Space after the ``-I`` is optional.
* The ``-D`` command line parameter is supported.
  * Multi-line macros are not supported from the command line.
  * Parameterized macros are not supported from the command line.
  * A macro may be defined with or without a value.
  * Macros defined on the command line don't have a parameter list, but they must be referenced with an empty parameter list.
  * Example: ``-Dmacro_name=macro_value``
    * Space after the ``-D`` is optional.
    * The value is optional.
    * The ``=`` is required if a value is present.
    * In the example, the macro must be referenced as ``macro_name()`` with a blank parameter list.
* The ``-o`` option specifies the output file. The default is set in macros.c and it's ``output.tmp``.
* The ``-h`` option prints the version and help and exists.
* The ``-V`` option prints the version for scripting and exists.
* This input file appears on the command line without any option identifier. It is required. The default file extension (that is, the part after the dot) is ``txt``. If you specify the file name without it, then it will be added while searching for the file.

**Example**
```

$ macros -v50 test1

```
This example finds ``test1.txt`` in the tests directory and processes it into ``output.tmp`` into the current directory. It prints out most operations that are carried out on the input file as they happen. (it's a lot)

## .import
The brings another file into the input stream exactly as if it was part of it. The current input stream is suspended until the imported file has been completely read and then the original stream is resumed.
* The ``-I`` command line parameter is supported.
* The input file is searched for using the input path. First the current directory is searched, then the directories specified in on the command line, then the directories specified in the ``MACRO_PATH`` environment variable, then the directories in the shell's ``PATH`` environment variable. If the file cannot be found, then an error is published and processing is aborted.

**Example**
```

.import "openable_file_name.xxx"

```

## .define
A macro definition can contain any other macro construct.

### Definition
This defines a parameterized macro, similar to the C pre-processor. It only does simple text substitutions.
* The ``-D`` command line parameter is supported. If the macro name is already defined, then a warning is published and processing continues.
* Does not support variadic macros
* Does not have "stringify"
* Does allow expansion of other macros as part of the definition.
* The at-sign ``@`` appends text. (instead of the CPP ``#``)
* All macros must have a parameter list that can be empty.
* Symbols must be defined before referencing. If a symbol is not defined, then it is taken as normal text.

```

.define blank_macro
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
### Expansion
A ``@`` character in front of a macro name specifies that it is to be expanded. If the macro was defined with parameters, then The parameters are expanded accordingly. Due to the multi-pass nature of the processor, it can nest macros and expand them iteratively.

**Example:**
```

.define one {
  .define two {123}
}
.define three(p) {
  @two
}
// this expands to "123"
@three(@one)

```

## .if/.else
This allows text to be included or excluded from the input stream based upon a comparison expression. If the expression does not evaluate to TRUE then the text is ignored as if it was a comment.
* If a name is not defined then it is FALSE. If a reference in an expression has been defined, then it's TRUE.
* All comparisons are binary. All words in an expression are taken to be macros.
* A ``@`` causes the word to be taken as a macro and it is expanded **before** the expression is evaluated.
  * If the macro expands to nothing then it is taken to be false.
  * If the macro expands to something, then it is taken to be true
    * If it is a number and it is being compared to a number, then it is evaluated numerically. This is intended to be used with things like version numbers.
    * If it is any text that is not number, then it is taken to be true. The text itself is not expanded into the expression.
* A blank expression in an ``else`` clause evaluates to ``true``.
* Numeric operators are ``LT``, ``GT``, ``GTE``, ``LTE``.
  * Relational operators are precedence of (3).
  * Also ``<``, ``>``, ``<=``, and ``>=``.
* Binary logical operators are ``AND`` and ``OR``.
  * ``OR`` precedence is lowest (0)
  * ``AND`` precedence is (1)
  * Also ``&&``, and ``||``.
* Binary operators that are both numeric and logical are  ``EQ``, ``NEQ``.
  * ``EQ`` and ``NEQ`` precedence is (2).
  * Also ``==``, and ``!=``.
* Unary negation operator is ``NOT``.
  * ``NOT`` operator is precedence (4).
  * Also ``!``.
* Word operators are preferred over punctuation operators. Word operators are not case sensitive, but upper case is convention.

```

.define a_macro
// The if clause is true
.if (a_macro) {
  include this text because a_macro has been defined
}

// This if clause is false since the macro expands to nothing
.if(@a_macro) {
  Do not include this text in the output
}

.if(@a_macro) {
  Do not include this text
} .else(NOT a_macro) {
  Do not include this text
} .else(blart AND a_macro) {
  exclude this text because "blart" is not defined as a macro.
} .else {
  include this text because the other clauses are all false and this expression is blank.
}

.if(NOT (a_macro AND @a_macro) OR blart) {
// This evaluates to false because the negation operator has the highest precedence.
}

```

## Building it
This requires for following to build:
* A recent ANSI C compiler. Tested with GCC and CLANG.
* ``git``
* ``cmake``
* ``doxygen`` and ``graphviz``
* ``clang-format``
* Any UNIX-like operating system. There are no plans to port it to any version of Windows or MAC-OS.

You can clone the archive as
```
$ git clone https://github.com/chucktilbury/macros.git
$ cd macros
$ ./scripts/setup
(debug):$ macros -v50 test1

```
The default compiler is clang but you can change it by editing the file macros/.project_env at the end. Comment out the clang part and comment in the gcc part.

To leave the build environment, simply ``exit`` the shell.


