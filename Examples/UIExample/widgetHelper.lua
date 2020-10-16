table = require("table")

boundWidget_ = {}

function bind_widget(_widget)
	local _size = _widget:get_size()
	table.insert(boundWidget_, { 
		ptr = _widget,
		x = _size.w / 2,
		y = _size.h / 2
	})
end

function unbind_widget(_widget)
	for i, widget in pairs(boundWidget_) do
		if(widget.ptr == _widget) then
			table.remove(boundWidget_, i)
			return
		end
	end
end

function set_bound_widgets(_x, _y)
	for i, widget in pairs(boundWidget_) do
		widget.ptr:set_position({ x = _x - widget.x, y = _y - widget.y, 0.0 })
	end
end

function is_bound(_widget)
	for i, widget in pairs(boundWidget_) do
		if(widget.ptr == _widget) then
			return true
		end
	end
	return false
end
