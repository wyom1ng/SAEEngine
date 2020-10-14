#include "engine_texture.h"

#include <lodepng.h>
#include <cassert>
#include <unordered_map>

namespace sae::engine
{
	namespace detail
	{

		std::vector<unsigned char> TextureHelper::load_png(const std::filesystem::path& _path, unsigned& _w, unsigned& _h, ENCODING _encoding)
		{
			LodePNGColorType _colorType = LodePNGColorType::LCT_RGBA;
			switch (_encoding)
			{
			case ENCODING::RGBA_8:
				break;
			case ENCODING::RGB_8:
				_colorType = LodePNGColorType::LCT_RGB;
				break;
			case ENCODING::R_8:
				_colorType = LodePNGColorType::LCT_GREY;
				break;
			default:
#ifdef SAE_ENGINE_HARD_ERRORS
				abort();
#elif defined(SAE_ENGINE_USE_EXCEPTIONS)
				throw std::runtime_error{ "INVALID encoding" };
#else
				return {};
#endif
			};

			unsigned int _bitDepth = (unsigned)(((uint16_t)_encoding) >> 8);
			std::vector<unsigned char> _out{};

			auto _res = lodepng::decode(_out, _w, _h, _path.string(), _colorType, _bitDepth);

#ifdef SAE_ENGINE_HARD_ERRORS
			assert(!_res);
#else
			if (_res)
			{
#ifdef SAE_ENGINE_USE_EXCEPTIONS
				throw std::runtime_error{ std::string{ "LodePNG error : "} + std::to_string(_res) + "\n  description: " + lodepng_error_text(_res) };
#else
				return {};
#endif
			};
#endif

			return _out;

		};

		using ENCODING = TextureHelper::ENCODING;

		namespace
		{

			static inline const std::unordered_map<std::string, ENCODING> STR_TO_ENCODING
			{
				{ "R_8", ENCODING::R_8 },
				{ "RGB_8", ENCODING::RGB_8 },
				{ "RGBA_8", ENCODING::RGBA_8 }
			};

		}
		
		TextureHelper::ENCODING encoding_from_string(const std::string& _str)
		{
			if (STR_TO_ENCODING.contains(_str))
			{
				return STR_TO_ENCODING.at(_str);
			}
			else
			{
				return ENCODING::INVALID;
			};
		};

		std::string encoding_to_string(TextureHelper::ENCODING _encoding)
		{
			using ENCODING = TextureHelper::ENCODING;
			switch (_encoding)
			{
			case ENCODING::R_8:
				return "R_8";
			case ENCODING::RGB_8:
				return "RGB_8";
			case ENCODING::RGBA_8:
				return "RGBA_8";
			case ENCODING::INVALID:
				return "";
			default:
				abort(); // hard error because this simply should never happen, ill write a test to make sure of it 
			};
		};

		
	}
	





	size_t Texture::height() const noexcept
	{
		return this->height_;
	};

	size_t Texture::width() const noexcept
	{
		return this->width_;
	};

	Texture::ENCODING Texture::encoding() const noexcept
	{
		return this->encoding_;
	};


	size_t Texture::size() const noexcept
	{
		return this->width() * this->height();
	};



	void Texture::resize(size_t _width, size_t _height)
	{
		auto _v = this->view_begin();
		int _dw = (_width - this->width());
		int _dh = (_height - this->height());

		auto _stride = this->stride();

		std::vector<value_type> _newImage(_width * _height * _stride);

		size_t _inRow = 0;
		size_t _rowLen = (std::min(_width, this->width_) * _stride);

		const value_type* _dIn = this->data();
		value_type* _dOut = _newImage.data();

		for (size_t r = 0; r < _height; ++r)
		{
			std::memcpy(_dOut, _dIn, sizeof(value_type) * _rowLen);

			_dIn += (this->width_ * _stride);
			_dOut += (_width * _stride);

			++_inRow;
			if (_inRow == this->height_)
				break;
		};

		this->data_ = std::move(_newImage);

		this->width_ = _width;
		this->height_ = _height;

	};

	bool Texture::paste(const Texture& _other, size_t _x, size_t _y)
	{
		// Check that encodings are the same
		if (this->encoding() != _other.encoding())
			return false;

		// Resize image if needed
		if ((_x + _other.width()) > this->width() || (_y + _other.height()) > this->height())
			this->resize(_x + _other.width(), _y + _other.height());	

		// Grab stride and shove it into a temporary
		auto _stride = this->stride();

		// Setup destination / source data pointers
		const value_type* _dIn = _other.data();
		value_type* _dOut = this->data() + (_x * _stride) + (_y * this->width() * _stride); // account for _x and _y

		// Loop through each row from _other and copy it into this at the correct offset
		for (size_t r = 0; r < _other.height(); ++r)
		{
			std::memcpy(_dOut, _dIn, sizeof(value_type) * _other.width() * _stride);

			_dIn += (_other.width() * _stride);
			_dOut += (this->width() * _stride);
		};

		return true;
	};









	void Texture::save_to_file(const std::filesystem::path& _path) const
	{
		LodePNGColorType _colorType = LodePNGColorType::LCT_RGBA;
		switch (this->encoding())
		{
		case ENCODING::RGBA_8:
			break;
		case ENCODING::RGB_8:
			_colorType = LodePNGColorType::LCT_RGB;
			break;
		case ENCODING::R_8:
			_colorType = LodePNGColorType::LCT_GREY;
			break;
		default:
#ifdef SAE_ENGINE_HARD_ERRORS
			abort();
#elif defined(SAE_ENGINE_USE_EXCEPTIONS)
			throw std::runtime_error{ "INVALID encoding" };
#else
			return {};
#endif
		};
		unsigned int _bitDepth = this->bitdepth();
		auto _res = lodepng::encode(_path.string(), this->data_.data(), this->width(), this->height(), _colorType, _bitDepth);

#ifdef SAE_ENGINE_HARD_ERRORS
		assert(!_res);
#else
		if (_res)
		{
#ifdef SAE_ENGINE_USE_EXCEPTIONS
			throw std::runtime_error{ std::string{ "LodePNG error : "} + std::to_string(_res) + "\n  description: " + lodepng_error_text(_res) };
#else
			return {};
#endif
		};
#endif

	};



	Texture::Texture(ENCODING _encoding, size_t _width, size_t _height, const allocator_type& _alloc) :
		encoding_{ _encoding }, width_{ _width }, height_{ _height }, data_{ (_width * _height) * (size_t)((uint8_t)_encoding), _alloc }, alloc_{ _alloc }
	{};
	Texture::Texture(ENCODING _encoding, const allocator_type& _alloc) :
		encoding_{ _encoding }, alloc_{ _alloc }
	{};

	Texture::Texture(ENCODING _encoding, const std::filesystem::path& _pngPath, const allocator_type& _alloc) :
		Texture{ _encoding, _alloc }
	{
		unsigned _w = 0;
		unsigned _h = 0;
		this->data_ = this->load_png(_pngPath, _w, _h, _encoding);
		this->width_ = (size_t)_w;
		this->height_ = (size_t)_h;
	};


}
