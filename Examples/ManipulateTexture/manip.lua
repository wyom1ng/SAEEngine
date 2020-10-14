engine = SAEEngine

engine.window.open(800, 600, "texture manipulation")

local function close_callback()
	engine.window.close()
end

engine.window.set_callback("close", close_callback)


local path_ = tostring(engine.fs.current_path())
local imageRGBA_ = engine.texture.load("RGBA_8", path_ .. "RGBATestImage.png")

local function print_texture_info(_texture)
	engine.io.print("Texture width = ")
	engine.io.print(_texture:width())
	engine.io.print(" height = ")
	engine.io.println(_texture:height())
	engine.io.print("\tpixel count = ")
	engine.io.println(_texture:size())
	engine.io.print("\tencoding = ")
	engine.io.println(_texture:encoding())
	engine.io.print("\tstride = ")
	engine.io.println(_texture:stride())
	engine.io.print("\tbitdepth = ")
	engine.io.println(_texture:bitdepth())
	return
end

print_texture_info(imageRGBA_)
imageRGBA_:resize(128, 128)
print_texture_info(imageRGBA_)

imageRGBA_:save(path_ .. "TextureRGBA_ButHalfIsWhite.png")

local sourceImage_ = engine.texture.load("RGB_8", path_ .. "PasteTestTexture.png")
sourceImage_:resize(32, 32)

local pasteImage_ = engine.texture.load("RGB_8", path_ .. "PasteTestTexture.png")
pasteImage_:paste(sourceImage_, 32, 0)
pasteImage_:paste(sourceImage_, 0, 32)
pasteImage_:paste(sourceImage_, 32, 32)
pasteImage_:save(path_ .. "PasteResult.png")


