CC=g++
SFML_FLAG= -lsfml-graphics -lsfml-window -lsfml-system
# MAIN_OUT= main.out
SFML_OUT= sfml-app.out
# INPUT_FILE= main.cpp  Entity.cpp
all : game

main.out : src/main.cpp
	$(CC) -c "src/main.cpp" -o main.out

Entity.out : src/csv/csv.cpp
	$(CC) -c "src/csv/csv.cpp" -o csv.out

game : main.out Entity.out
	$(CC) -o $(SFML_OUT) main.out csv.out  $(SFML_FLAG)
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
