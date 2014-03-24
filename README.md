Function Prototype Extractor (C)
================================

This program is intended to extract the function prototypes for global scope
functions, and export them to a .txt file, where they can either be #included
or copy and pasted.

It is designed for ANSI C, but it additionally supports single-line (//)
comments.


Usage
-----

function_prototype_extractor input_code_file_name output_file_[default = function_prototypes.txt]


Special Thanks
--------------

* Alistair Moffat - for the suggestion


Changelog
---------

### v0.1

* Initial release