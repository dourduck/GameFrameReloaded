#!/usr/bin/env lua

-- main.c
os.execute("echo '[***BUILDING MAIN***]'")
os.execute("gcc -c ./src/main.c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")

os.execute("echo '[***BUILDING EVENT LIB***]'")
os.execute(
	"gcc -c ./src/engine/event_system/event_bus.c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")
os.execute(
	"gcc -c ./src/engine/event_system/event_queue.c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")

-- linking
os.execute("echo '[***LINKING***]'")
os.execute(
	"gcc -o game main.o event_bus.o event_queue.o -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")

-- cleanup object files
os.execute("mkdir -p ./build/")
os.execute("mv *.o ./build/")
