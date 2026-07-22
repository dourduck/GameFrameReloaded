#!/usr/bin/env lua

-- os.execute("echo '[***BULIDING EVENTS LIB***]'")
-- os.execute(
-- 	"gcc ./src/event_system/events.c -c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")

os.execute("echo '[***BULIDING EVENT BUS LIB***]'")
os.execute(
	"gcc ./event_bus.c -c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")

os.execute("echo '[***BULIDING EVENT QUEUE LIB***]'")
os.execute(
	"gcc ./event_queue.c -c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")

os.execute("echo '[***BULIDING MAIN LIB***]'")
os.execute(
	"gcc ./test.c -c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")

-- linking
os.execute("echo '[***LINKING***]'")
os.execute(
	"gcc -o test test.o event_bus.o event_queue.o")

os.execute("mkdir -p ./build/")
os.execute("mv *.o ./build/")
