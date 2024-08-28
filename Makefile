minesweeper: minesweeper.c
	cc -lraylib -DRAYGUI_IMPLEMENTATION -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o minesweeper minesweeper.c


