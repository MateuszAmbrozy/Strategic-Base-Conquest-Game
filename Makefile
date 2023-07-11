all: 
		g++ Mediator.cpp -o mediator
		g++ Player1.cpp -o player1
		g++ Player2.cpp -o player2
runPlayer1:
		./mediator $(shell pwd)/player1 map.txt status.txt rozkazy.txt 10
runPlayer2:
		./mediator $(shell pwd)/player2 map.txt status.txt rozkazy.txt 10
