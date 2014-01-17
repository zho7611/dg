.h and .c files in this directory are generated from glLoadGen_2_0_2 (https://bitbucket.org/alfonse/glloadgen) with the following command line options:

> lua LoadGen.lua -style=pointer_c -spec=gl -version=3.3 -profile=core core_3_3
> lua LoadGen.lua -style=pointer_c -spec=wgl -profile=core -stdext=wgl_common.txt

I changed the functions names that have the same names in both .c files to avoid compile error.
