
local scene2_ = engine.scene.new()

local testPath_ = engine.fs.path.new(engine.fs.current_path() .. "example.lua")

local function key_callback(_key, _scancode, _action, _mods)
	if(_key == 256 and _action == 0) then
		engine.fs.doAfter(tostring(testPath_))
		engine.window.pop_scene()
		return
	end
end

engine.window.set_callback("key", key_callback)

engine.window.push_scene(scene2_)
