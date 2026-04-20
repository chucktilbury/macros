# TODO list

* **``Change the substitution strategy``** from slurping the whole file to processing it one line at a time. Such that when the input is finished, there is no more processing to be done.
* Reinstate macro parameters such that they are replaced in a reference.
    * Check for arity. Publish error if it doesn't match.
* Add syntax error if the command line option does not match the type. Specifically numbers.
* Place output in stdout if there is no output file specified.
* Search the current directory first for the input file.
* Preserve original line numbers in passes for errors.
    * Place '#' comments in the output that have the file name and line number.
* Only operate on a single buffer instead of re-reading a file. Requires making everything reentrant.


# NOTES

