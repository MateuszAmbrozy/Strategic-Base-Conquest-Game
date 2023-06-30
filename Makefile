all: 
		g++ Mediator.cpp -o mediator
		g++ Player.cpp -o player
		g++ Enemy.cpp -o enemy
runPlayer:
		./mediator $(shell pwd)/player map.txt status.txt rozkazy.txt 10
runEnemy:
		./mediator $(shell pwd)/enemy map.txt status.txt rozkazy.txt 10
