all: tp_1

tp_1: imageBase.o tp1_compression.o
	g++ -o tp_1 imageBase.o tp1_compression.o

imageBase.o: ImageBase.cpp
	g++ -o imageBase.o -c ImageBase.cpp -W -Wall -ansi -pedantic

tp1_compression.o: tp1_compression.cpp ImageBase.h
	g++ -o tp1_compression.o -c tp1_compression.cpp -W -Wall -ansi -pedantic

clean:
	rm -rf *.o

mrproper: clean
	rm -rf tp_1
