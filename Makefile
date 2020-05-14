CC = g++
CFLAGS = -std=c++11 -Wall -pedantic  -Wmultichar
TARGET = Compiler.exe
OBJS = main.o  Scanner.o Parser.o StaticSemantics.o targetGen.o
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

Main.o: StaticSemantics.h
	$(CC) $(CFLAGS) -c main.cpp
Parser.o: Parser.h 
	$(CC) $(CFLAGS) -c Parser.cpp
Scanner.o: Scanner.h 
	$(CC) $(CFLAGS) -c Scanner.cpp
StaticSemantics.o: StaticSemantics.h
	$(CC) $(CFLAGS) -c StaticSemantics.cpp
targetGen.o: targetGen.h
	$(CC) $(CFLAGS) -c targetGen.cpp

clean:
	/bin/rm -f *.o $(TARGET)

