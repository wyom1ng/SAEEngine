engine = SAEEngine

engine.window.open(800, 600, "reeee")

function close_callback()
	engine.window.close()
end

engine.window.set_callback("close", close_callback)
engine.fs.dofile(engine.fs.current_path() .. "createUIElement.lua")
