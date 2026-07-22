#!/usr/bin/env lua

-- game.h
os.execute("echo '[***BULIDING GAME LIB***]'")
os.execute("gcc ./src/game.c -c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")

-- -- entity.h
-- os.execute("echo '[***BULIDING ENTITY LIB***]'")
-- os.execute("gcc ./src/entity.c -c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")
--
-- -- fsm.h
-- os.execute("echo '[***BULIDING FSM LIB***]'")
-- os.execute("gcc ./src/fsm.c -c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")
--
-- -- physics.h
-- os.execute("echo '[***BULIDING PHYSICS LIB***]'")
-- os.execute("gcc ./src/physics.c -c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")
--
-- -- render.h
-- os.execute("echo '[***BULIDING RENDER LIB***]'")
-- os.execute("gcc ./src/render.c -c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")
--
-- -- event.h
-- os.execute("echo '[***BULIDING EVENT LIB***]'")
-- os.execute("gcc ./src/event.c -c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")
--
-- -- input.h
-- os.execute("echo '[***BULIDING INPUT LIB***]'")
-- os.execute("gcc ./src/input.c -c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")

-- main.c
os.execute("echo '[***BUILDING MAIN***]'")
os.execute("gcc -c ./src/main.c -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")


-- linking
-- os.execute("echo '[***LINKING***]'")
-- os.execute(
-- 	"gcc -o game main.o game.o event.o input.o fsm.o physics.o entity.o render.o -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")

os.execute("echo '[***LINKING***]'")
os.execute(
	"gcc -o game main.o game.o -I./include/ -L./lib/ -lraylib -Wl,-rpath,$(pwd)/lib/ -lm -lpthread -ldl -lrt -lX11")

os.execute("mkdir -p ./build/")
os.execute("mv *.o ./build/")
