
local scene2_ = engine.scene.new()

local function key_callback(_key, _scancode, _action, _mods)
	if(_key == 256 and _action == 0) then
		engine.os.doAfter("C:\\Users\\jonat\\source\\repos\\SAEEngine\\SAEEngine\\Examples\\LuaAPI\\example.lua")
		engine.window.pop_scene()
		return
	end
end

engine.window.set_callback("key", key_callback)

engine.window.push_scene(scene2_)
