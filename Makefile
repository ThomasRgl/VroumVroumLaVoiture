CC=g++
SFML_FLAG= -lsfml-graphics -lsfml-window -lsfml-system
# MAIN_OUT= main.out
SFML_OUT= sfml-app.out
# INPUT_FILE= main.cpp  Entity.cpp
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

game : main.out csv.out voiture.out circuit.out intersect.out
	$(CC) -o $(SFML_OUT) main.out csv.out voiture.out circuit.out intersect.out $(SFML_FLAG)
	rm main.out
	rm csv.out

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
