local sptr_ = engine.scene.new()

local sampleShader_ = engine.window.get_shader("sampleShader")

local path_ = engine.fs.current_path()

if(sampleShader_ == nil) then
	sampleShader_ = engine.shader.new("sampleShader", path_ .. "vertexShader.glsl", path_ .. "fragmentShader.glsl" )
	
	if(sampleShader_:good() == false) then
		close_callback()
	end
	engine.window.push_shader(sampleShader_)
end

if(sampleShader_:good() == false) then
	close_callback()
end

local w0_ = engine.gfx.rectangle.new(sampleShader_)
sptr_:push(w0_)

local function key_callback(_key, _scancode, _action, _mods)
	if(_key == 256 and _action == 0) then
		engine.fs.doAfter(path_ .. "otherScene.lua")
		engine.window.pop_scene()
	end
end

engine.window.set_callback("key", key_callback)

engine.io.print("Scene stack size = ")
engine.io.println(engine.window.get_scene_stack_size())

engine.window.push_scene(sptr_)