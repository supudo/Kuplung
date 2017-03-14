io.write(string.format("Hello from %s\n", _VERSION))

io.write("Calling testFromLua() ...\n")
local value = testFromLua("First", "Second", 112233)
io.write(string.format("testFromLua() returned: %s\n", tostring(value)))
