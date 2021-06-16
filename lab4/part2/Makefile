TARGET := matmul
OBJS := main.o matrix.o
CXX := mpicxx

matmul:main.o matrix.o
	$(CXX) -o matmul $(OBJS)
%.o: %.cc test.h
	$(CXX) -c $<
clean:
	rm *.o

