SHELL=C:/Windows/System32/cmd.exe
objects = main.o lz_Ploaj.o
LIBPATH = -L./lib
LIB = ./lib/FreeImage.lib
HEADERPATH = -I./include
STATICGCC = -static-libgcc -static-libstdc++

all : BlobExtract.exe
 
BlobExtract.exe : $(objects)
	g++ -Wall -g -ggdb -O2 -o $@ $(objects) $(LIBPATH) $(LIB) $(STATICGCC) $(HEADERPATH)
	
%.o: %.cpp
	g++ -O2 -g -ggdb -c -MMD -o $@ $< $(HEADERPATH)

-include $(objects:.o=.d)

.PHONY : clean
clean :
	rm -rf BlobExtract.exe *.o *.d
