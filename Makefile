all:
	g++ -std=c++14 memory_checker.S iostream.cpp source.cpp -o handler
debug:
	g++ -g -std=c++14 memory_checker.S iostream.cpp source.cpp -o handler
run:
	./handler
clear:
	rm ./handler
clean:
	rm ./handler
