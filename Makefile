#
# Makefile
# CS51 Final Project
# 
# Jason Stein and Mason Hale
#


# Rule to make 'final' executable
final: final.o
	gcc -o final final.o

# Rule to clean all files created my compiler
clean:
	rm -f final core *.o
