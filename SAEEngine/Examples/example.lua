local engine = SAEEngine

engine.window.open(800, 600, "test")

local function close_callback()
	engine.window.close()
end

engine.window.callback.close = close_callback
