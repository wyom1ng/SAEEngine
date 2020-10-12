local engine = SAEEngine

engine.io.print("Input window width : ")
local _w = engine.io.readln()
engine.io.println()
engine.io.print("Input window height : ")
local _h = engine.io.readln()
engine.io.println()

engine.window.open(_w, _h, "test")
engine.window.focus()

local function close_callback()
	engine.window.close()
end

engine.window.callback.close = close_callback

engine.window.attention()
