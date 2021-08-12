CC=g++
GCC = gcc
SFML_FLAG= -lsfml-graphics -lsfml-window -lsfml-system
# MAIN_OUT= main.out
SFML_OUT= sfml-app.out
# INPUT_FILE= main.cpp  Entity.cpp
FLAG_OPTI = -O3 -finline-functions -funroll-loops -march=native -mtune=native -flto -lpthread
LFLAGS=-lm -lpthread


all : game

main.out : src/main.cpp
	$(CC) -c "src/main.cpp" -o main.out

csv.out : src/csv/csv.cpp
	$(CC) -c "src/csv/csv.cpp" -o csv.out

voiture.out : src/voiture/Voiture.cpp
	$(CC) -c "src/voiture/Voiture.cpp" -o voiture.out

circuit.out : src/circuit/Circuit.cpp
	$(CC) -c "src/circuit/Circuit.cpp" -o circuit.out

intersect.out : src/intersect/intersect.cpp
	$(CC) -c "src/intersect/intersect.cpp" -o intersect.out

nn.out : src/neuralNetwork/neuralNetwork.c
	$(GCC) -c $(FLAG_OPTI) -o nn.out "src/neuralNetwork/neuralNetwork.c"   $(LFLAGS)

game : main.out csv.out voiture.out circuit.out intersect.out nn.out
	$(CC) -o $(SFML_OUT) $(FLAG_OPTI) main.out csv.out voiture.out circuit.out intersect.out  nn.out $(SFML_FLAG)
	rm main.out
	rm csv.out
	rm voiture.out
	rm circuit.out
	rm intersect.out
	rm nn.out

clear :
	rm *.out






	# CC=g++
	# SFML_FLAG= -lsfml-graphics -lsfml-window -lsfml-system
	# MAIN_OUT= main.out
	# SFML_OUT= sfml-app.out
	# INPUT_FILE= main.cpp  Entity.cpp
	# all :
	# 	$(CC)  -o $(MAIN_OUT) $(INPUT_FILE)
	# 	$(CC) $(MAIN_OUT) -o $(SFML_OUT) $(SFML_FLAG)
	# 	rm $(MAIN_OUT)
	#
	# clear :
	# 	rm *.out
