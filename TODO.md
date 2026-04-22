# TODO list

* Only operate on a single buffer instead of re-reading a file.
    * Requires making everything reentrant.
    * Make the ``process_xxx()`` functions return whether they handled the character or not and move all of the preliminaries into those functions. The put them all in a ``else if(!process_xxx()) {}`` construct. This changes the granularity from the function that calls them to the functions themselves.
* Reinstate macro parameters such that they are replaced in a reference.
    * Check for arity. Publish error if it doesn't match.
* Add syntax error if the command line option does not match the type. Specifically numbers.
* Place output in stdout if there is no output file specified.
* Search the current directory first for the input file.
* Preserve original line numbers in passes for errors.
    * Place '#' comments in the output that have the file name and line number.


# NOTES

