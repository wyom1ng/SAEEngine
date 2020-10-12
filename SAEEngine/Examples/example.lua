local engine = SAEEngine

engine.window.open(800, 600, "reeee")

local function close_callback()
	engine.window.close()
end

local _gfx = engine.gfx.widget.new()

engine.window.callback.close = close_callback
