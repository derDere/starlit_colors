SRC = $(wildcard *.cpp)
APP = "starlit_colors"

All: $(APP)
	#Making

$(APP): $(SRC)
	g++ -o $(APP) $(SRC) -lncurses

test: $(APP)
	./$(APP) debug
