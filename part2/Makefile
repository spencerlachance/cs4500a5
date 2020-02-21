build:
	g++ -std=c++11 -o server server.cpp
	g++ -std=c++11 -o client client.cpp

run:
	./server -ip 127.0.0.1 & echo $$! > server.PID
	./client -ip 127.0.0.2 &
	./client -ip 127.0.0.3 &
	./client -ip 127.0.0.4 &
	sleep 5
	echo "All done"

clean:
	kill `cat server.PID` && rm server.PID
	rm server client