#!/usr/bin/env lua

local CompileCommand = {}
CompileCommand.__index = CompileCommand

function CompileCommand:new()
	local command_table = {
		CC = "gcc",
		CFLAGS = "-c",
		SOURCE = "./main.c",
		INCLUDE = "",
		LIB = "",
		ARGS = "",
		DEBUG = true,
	}
	return setmetatable(command_table, CompileCommand)
end

function CompileCommand:tostring()
	local commands = {
		self.CC,
		self.CFLAGS,
		self.SOURCE,
		self.INCLUDE,
		self.LIB,
		self.ARGS,
	}

	return table.concat(commands, " ")
end

function CompileCommand:print()
	print(self:tostring())
end

function CompileCommand:exec()
	if self.DEBUG then
		self:print()
	else
		os.execute(self:tostring())
	end
end

local cc_command = CompileCommand:new()

cc_command.CC = "gcc"
cc_command.INCLUDE = "-I./external/raysan5/include/"
cc_command.LIB = "-L./external/raysan5/lib/"
cc_command.ARGS = "-lraylib -Wl,-rpath,$(pwd)/external/raysan5/lib/ -lm -lpthread -ldl -lrt -lX11"
cc_command.DEBUG = false

print("[***BUILDING MAIN***]")
cc_command.SOURCE = "./src/main.c"
cc_command.CFLAGS = "-c"
cc_command:exec()

-- *** ---

print("[***BUILDING EVENT SYSTEM***]")
cc_command.SOURCE = "./src/engine/event_system/event_bus.c"
cc_command:exec()

cc_command.SOURCE = "./src/engine/event_system/event_queue.c"
cc_command:exec()

-- *** ---

print("[***LINKING***]")
cc_command.CFLAGS = "-o game"
cc_command.SOURCE = "main.o event_bus.o event_queue.o"
cc_command:exec()

-- -- cleanup object files
os.execute("mkdir -p ./build/")
os.execute("mv *.o ./build/")
os.execute("mv game ./build/")
os.execute("./build/game")
