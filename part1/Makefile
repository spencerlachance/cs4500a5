build:
	g++ -pthread -std=c++11 bench.cpp

run:
	# Download datafile from github
	wget https://raw.githubusercontent.com/spencerlachance/cs4500a5/master/part1/datafile.zip
	unzip datafile.zip

	# First Rower
	-time -p ./a.out -e 1 -f datafile.txt -len 5000000
	@echo -e '\n'
	-time -p ./a.out -p -e 1 -f datafile.txt -len 5000000
	@echo -e '\n'
	-time -p ./a.out -e 1 -f datafile.txt -len 10000000
	@echo -e '\n'
	-time -p ./a.out -p -e 1 -f datafile.txt -len 10000000
	@echo -e '\n'
	-time -p ./a.out -e 1 -f datafile.txt -len 50000000
	@echo -e '\n'
	-time -p ./a.out -p -e 1 -f datafile.txt -len 50000000
	@echo -e '\n'
	-time -p ./a.out -e 1 -f datafile.txt -len 100000000
	@echo -e '\n'
	-time -p ./a.out -p -e 1 -f datafile.txt -len 100000000
	@echo -e '\n'
	
	# Second Rower
	-time -p ./a.out -e 2 -f datafile.txt -len 50000
	@echo -e '\n'
	-time -p ./a.out -p -e 2 -f datafile.txt -len 50000
	@echo -e '\n'
	-time -p ./a.out -e 2 -f datafile.txt -len 100000
	@echo -e '\n'
	-time -p ./a.out -p -e 2 -f datafile.txt -len 100000
	@echo -e '\n'
	-time -p ./a.out -e 2 -f datafile.txt -len 500000
	@echo -e '\n'
	-time -p ./a.out -p -e 2 -f datafile.txt -len 500000
	@echo -e '\n'
	-time -p ./a.out -e 2 -f datafile.txt -len 1000000
	@echo -e '\n'
	-time -p ./a.out -p -e 2 -f datafile.txt -len 1000000
	@echo -e '\n'

clean:
	rm a.out
	rm datafile.zip
	rm datafile.txt