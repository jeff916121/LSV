Instructions for adding commands in ABC:


step 1: 
	
Create a directory "ext-xxx" in the src directory, where "xxx" is the name of the package.
Directory which is named starting with "ext" will be automaticlly included in compiling.

step 2:

Download "init.cpp" and "module.make" from https://github.com/berkeley-abc/ext-hello-abc and put them in "ext-xxx" 


step 3: 
Find "Cmd_CommandAdd(...)" in "init.cpp" and change the command to what you want.

step 4: 
It is better to rename function and varialbe in "init.cpp".

step 5: 
If you have many files, say, f1.c and f2.c, to compile, modify "module.make" and add lines as "SRC += src/ext-xxx/f1.c \ src/ext-xxx/f2.c"
and so on to compile them.




If you have any question about these steps, please contact Kuan-Hua at "f00943146@ntu.edu.tw".

