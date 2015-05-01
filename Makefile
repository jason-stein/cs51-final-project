#
# Makefile
# CS51 Final Project
# 
# Jason Stein and Mason Hale
#


# Rule to make executables
final: final.o
	gcc -o final final.o
	
temp: temp.o
	gcc -o temp temp.o
	
temp2: temp2.o
	gcc -o temp2 final.o

# Rule to clean all files created my compiler
clean:
	rm -f final temp temp2 core *.o
