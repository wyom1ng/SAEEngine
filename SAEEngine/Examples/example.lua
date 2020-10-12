local engine = SAEEngine

engine.window.open(800, 600, "reeee")

local sptr_ = engine.scene.new()
local w0_ = engine.gfx.widget.new()
sptr_:push(w0_)

local function close_callback()
	engine.window.close()
end

local sampleShader_ = engine.shader.new("sampleShader", 
	"C:\\Users\\jonat\\source\\repos\\SAEEngine\\SAEEngine\\Examples\\vertexShader.glsl",
	"C:\\Users\\jonat\\source\\repos\\SAEEngine\\SAEEngine\\Examples\\fragmentShader.glsl" )

if(sampleShader_:good() == false) then
	close_callback()
end

engine.window.push_scene(sptr_)

engine.window.callback.close = close_callback
