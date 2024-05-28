To compile the python extension put your 'highscore_api.h' header file in this
folder and type the following command in the terminal:

python setup.py build_ext -b .

If the compilation succeeds a new file will be created: 'pyBand.so'.
This extension presents four functions that call your new system calls:
1) band_create
2) band_join
2) band_play
3) band_listen

You can use this functions in a python script or directly from the python
interpreter, type 'python' in the terminal and then the following commands:

>>>import pyBand
>>>import os
>>>pyBand.band_create(0)

The syntax of the command can be found by typing the following in the python
interpreter:

>>>import pyBand
>>>help(pyBand.band_listen)

You can also use the ipython interpreter (you can find the rpm package in the
course website). After running ipython (type 'ipython' in the terminal) do:

[1] import pyBand
[2] pyBand.band_join?
