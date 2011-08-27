Title    : qccx v1.0 optimizing QuakeC compiler
Filename : qccx100.zip
Version  : 1.0
Date     : March 3, 2000
Author   : J.P. Grossman (a.k.a. Mephistopheles)
Email    : jpg@ai.mit.edu
URL      : http://elohim.ai.mit.edu/qccx

NOTE: Minor porting has been done so it functions on GCC 4.x
Additionally, the *.qc files that came with it have been removed.

What is qccx?
-------------

qccx is the world's first optimizing QuakeC compiler with support for string
manipulation, pointers, integers, arrays and for loops.  It is also the 
fastest QuakeC compiler out there (to my knowledge).  qccx is based on fastqcc
which was, in turn, based on qccdos.  Use it the same way you use any other 
QuakeC compiler.


Files
-----

The following files are included in qccx:

readme.txt      - this file (no, really?)
maunal.txt      - user's guide
gnu.txt         - GNU GPL
qccx.exe        - win32 qccx executable
*.cpp, *.h      - qccx source files (5 .cpp files, 4 .h files)


Getting Started
---------------

1.  Read the manual.  All of it.
2.  Have a look at the supplied bare-bones QuakeC project; it illustrates
    the features supported by qccx.
3.  Drop internal.qc into your project and harness the true power of QuakeC
    ** Caution ** internal.qc will cause some listen servers running on Intel
	platforms to crash!!  A workaround is to use -dibonly.

License Agreement
-----------------

Please read gnu.txt.  John Carmack summarized it as follows:

"The code is all licensed under the terms of the GPL (gnu public license).  
You should read the entire license, but the gist of it is that you can do 
anything you want with the code, including sell your new version.  The catch 
is that if you distribute new binary versions, you are required to make the 
entire source code available for free to everyone."


Disclaimer
----------

qccx was developed as an internal tool for CRMod++ v6.0 and was not originally
meant to be released.  It is therefore being provided with minimal
documentation and no apologies.

