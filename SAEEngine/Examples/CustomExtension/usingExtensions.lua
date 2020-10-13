local engine = SAEEngine

engine.window.open(800, 600, "reeee")

local exampleObject_ = engine.ext.example.new()

local exampleScene_ = engine.scene.new()
exampleScene_:push(exampleObject_)
engine.window.push_scene(exampleScene_)

local function close_callback()
	engine.window.close()
end
local function key_callback(_key, _scancode, _action, _mods)
	if(_key == 84 and _action == 0) then
		exampleObject_:test()
	end
end

engine.window.set_callback("close", close_callback)
engine.window.set_callback("key", key_callback)
