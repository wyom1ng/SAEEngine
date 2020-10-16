local sptr_ = engine.scene.new()
local path_ = engine.fs.current_path()

local table = require("table")

local spriteShader_ = engine.window.get_shader("spriteShader")
if(spriteShader_ == nil) then
	spriteShader_ = engine.shader.new("spriteShader", path_ .. "spriteVertex.glsl", path_ .. "spriteFragment.glsl" )
	
	if(spriteShader_:good() == false) then
		close_callback()
	end
	engine.window.push_shader(spriteShader_)
end
if(spriteShader_:good() == false) then
	close_callback()
end


local testTexture_ = engine.texture.load("RGBA_8", path_ .. "RGBATestImage.png")



local w_color_ = { r = 150, g = 255, b = 150, a = 255 }
local w_color_2_ = { r = 150, g = 150, b = 255, a = 255 }

local allWidgets_ = {}

engine.fs.dofile(path_ .. "widgetHelper.lua")

function test_button_callback(_widget, _mbutton, _action, _mods)
	if(_mbutton == 0 and _action == 0) then
		if(is_bound(_widget) == false) then
			bind_widget(_widget)
		else
			unbind_widget(_widget)
		end
	elseif(_mbutton == 1 and _action == 0 and engine.window.get_key(340) == 1) then
		unbind_widget(_widget)
		for i, widget in pairs(allWidgets_) do
			if(widget == _widget) then
				table.remove(allWidgets_, i)
				return
			end
		end
	end
end

local function new_widget(_x, _y)
	local _w = engine.gfx.sprite.new(spriteShader_, testTexture_)
	_w:set_color(w_color_2_)
	_w:set_position({ x = _x, y = _y, z = 0.0})
	sptr_:push(_w)
	sptr_:push(engine.ui.button.new(_w, test_button_callback))
	table.insert(allWidgets_, _w)
end

new_widget(0, 0)
new_widget(200, 0)


local function get_mouse_pos()
	local _w, _h = engine.window.get_size()
	local _cpos = engine.window.get_cursor_pos()
	_cpos.y = _cpos.y * -1
	_cpos.y = _cpos.y + _h
	return _cpos
end



local function cursor_callback(_x, _y)
	if(boundWidget_ ~= nil and engine.window.get_key(341) ~= 1) then
		local _w, _h = engine.window.get_size()
		_y = _y * -1
		_y = _y + _h
		set_bound_widgets(_x, _y)
	end
end

local function mouse_callback(_button, _action, _mods)
	if(_action == 0 and _button == 0 and engine.window.get_key(340) == 1) then
		local _ms = get_mouse_pos()
		new_widget(_ms.x - 50, _ms.y - 50)
	end
end

local function key_callback(_button, _scancode, _action, _mods)
	if(_button == 96 and _action == 0) then
		engine.fs.doAfter(path_ .. "createUIElement.lua")
		engine.window.pop_scene()
	elseif(_button == 341 and _action == 0 and boundWidget_ ~= nil) then
		local _ms = get_mouse_pos()	
		set_bound_widgets(_ms.x, _ms.y)
	end
end




engine.window.set_callback("key", key_callback)
engine.window.set_callback("cursor", cursor_callback)
engine.window.set_callback("mouse", mouse_callback)

engine.window.push_scene(sptr_)