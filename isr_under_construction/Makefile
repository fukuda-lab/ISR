PRG	= main
OBJ	=  spec_path_analy.o util.o radix.o main.o

CFLAGS  = -O2

CXX	= g++ -std=c++17 $(CFLAGS) 

$(PRG)	: $(OBJ)
		$(CXX) -o $@ $^

.c.o:        
		$(CC) -c $<

clean:
		rm -rf $(PRG) $(OBJ)

util.o : util.cc ./include/radix.h ./include/util.h
radix.o : radix.cc ./include/radix.h 
spec_path_analy.o : spec_path_analy.cc ./include/spec_path_analy.h 
main.o : main.cc  ./include/radix.h ./include/util.h
