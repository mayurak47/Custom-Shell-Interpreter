shell:	util.c shell.c global_variables.h
		gcc -o shell shell.c
		@echo "Start shell using ./shell."

clean:
		rm -f *.o shell