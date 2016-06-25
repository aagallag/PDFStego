# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

# library flags
LFLAGS = -lpodofo -lfreetype -lfontconfig -ljpeg -lz -lpthread -lcrypto

# build targets
STEGLIB = stegolib
PDFSTEGO = pdfstego


all: clean $(PDFSTEGO)

$(PDFSTEGO): $(PDFSTEGO).cpp
	$(CC) $(CFLAGS) -o $(PDFSTEGO) $(STEGLIB).cpp $(PDFSTEGO).cpp $(LFLAGS)

clean:
	$(RM) $(PDFSTEGO)
