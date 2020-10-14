#include "SAEEngine.h"

#include <vector>
#include <memory>
#include <filesystem>

#include <cassert>

namespace sae::engine
{

	namespace detail
	{
		struct TextureHelper
		{
		public:
			/**
			 * @brief [b15:b8] = bitdepth    [b7:b0] = values per pixel
			*/
			enum class ENCODING : uint16_t
			{
				INVALID = 0x0000,
				R_8 = 0x0801,
				RGB_8 = 0x0803,
				RGBA_8 = 0x0804
			};

		protected:
			static std::vector<unsigned char> load_png(const std::filesystem::path& _path, unsigned& _w, unsigned& _h, ENCODING _encoding);
		};

		TextureHelper::ENCODING encoding_from_string(const std::string& _str);
		std::string encoding_to_string(TextureHelper::ENCODING _encoding);


	};

	struct Texture : public detail::TextureHelper
	{
	public:
		using value_type = uint8_t;
		using allocator_type = std::allocator<uint8_t>;
	private:
		using ContainerT = std::vector<value_type, allocator_type>;
	public:
		
		/**
		 * @brief Returns the bitdepth (bits per pixel color value) of the current encoding
		*/
		constexpr uint8_t bitdepth() const noexcept
		{
			return (uint8_t)(((uint16_t)this->encoding_) >> 8);
		};

		/**
		 * @brief Returns the distance in bytes between pixels of the current encoding ( eg. RGBA = 4, RGB = 3, ... )
		*/
		constexpr uint8_t stride() const noexcept
		{
			return (uint8_t)(((uint16_t)this->encoding_) & 0xFF);
		};

	private:
		template <typename T>
		struct base_pixel_view
		{
		protected:
			constexpr inline void _Check_Bounds() const noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
#if SAE_ENGINE_DEBUG_LEVEL > 0
				assert(this->at_ != nullptr);
				assert(this->at_ >= this->begin_);
				assert(this->at_ <= this->end_);
#endif
			};
			constexpr inline void _Check_Value() const noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
#if SAE_ENGINE_DEBUG_LEVEL > 0
				assert(this->at_ != nullptr);
				assert(this->at_ >= this->begin_);
				assert(this->at_ < this->end_);
#endif
			};

		public:
			using value_type = T;
			using pointer = T*;
			using reference = T&;
		private:
			using const_value = const std::remove_const_t<T>;
			using const_pointer = const_value*;
			using const_reference = const_value&;
		public:

			constexpr explicit operator bool() const noexcept
			{
				return this->at_ != nullptr;
			};

			constexpr pointer data() const noexcept(noexcept(_Check_Value()))
			{
				this->_Check_Bounds();
				return this->at_;
			};
			constexpr uint32_t stride() const noexcept
			{
				return this->stride_;
			};

			constexpr const_reference at(uint32_t i) const noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
#if SAE_ENGINE_DEBUG_LEVEL > 0
				this->_Check_Value();
				assert(i < this->stride_);
#endif
				return *(this->at_ + i);
			};
			constexpr const_reference operator[](uint32_t i) const noexcept(noexcept(at(i)))
			{
				return this->at(i);
			};

			friend inline constexpr bool operator==(base_pixel_view lhs, base_pixel_view rhs) noexcept
			{
				return (lhs.at_ == rhs) && (lhs.stride_ == rhs.stride_);
			};
			friend inline constexpr bool operator!=(base_pixel_view lhs, base_pixel_view rhs) noexcept
			{
				return (lhs.at_ != rhs) && (lhs.stride_ == rhs.stride_);
			};

			friend inline constexpr bool operator>(base_pixel_view lhs, base_pixel_view rhs) noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				assert(lhs.stride_ == rhs.stride_);
				return (lhs.at_ > rhs.at_);
			};
			friend inline constexpr bool operator<(base_pixel_view lhs, base_pixel_view rhs) noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				assert(lhs.stride_ == rhs.stride_);
				return (lhs.at_ < rhs.at_);
			};
			friend inline constexpr bool operator>=(base_pixel_view lhs, base_pixel_view rhs) noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				assert(lhs.stride_ == rhs.stride_);
				return (lhs.at_ >= rhs.at_);
			};
			friend inline constexpr bool operator<=(base_pixel_view lhs, base_pixel_view rhs) noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				assert(lhs.stride_ == rhs.stride_);
				return (lhs.at_ <= rhs.at_);
			};

			friend inline constexpr std::ptrdiff_t operator-(base_pixel_view lhs, base_pixel_view rhs) noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				assert(lhs.stride_ == _rhs.stride_);
				return ((lhs.at_ - rhs.at_) / lhs.stride_);
			};
	
			constexpr base_pixel_view operator+(size_t n) const noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				auto _out = *this;
				_out.at_ += (n * this->stride_);
				_out._Check_Bounds();
				return _out;
			};
			base_pixel_view& operator+=(uint32_t n) noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				this->at_ += (n * this->stride_);
				this->_Check_Bounds();
				return *this;
			};

			constexpr base_pixel_view operator-(uint32_t n) const noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				auto _out = *this;
				_out.at_ -= (n * this->stride_);
				_out._Check_Bounds();
				return _out;
			};
			base_pixel_view& operator-=(uint32_t n) noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				this->at_ -= (n * this->stride_);
				this->_Check_Bounds();
				return *this;
			};

			base_pixel_view& operator++() noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				this->at_ += this->stride_;
				this->_Check_Bounds();
				return *this;
			};
			base_pixel_view& operator++(int) noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				auto _out = *this;
				this->at_ += this->stride_;
				this->_Check_Bounds();
				return _out;
			};

			base_pixel_view& operator--() noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				this->at_ -= this->stride_;
				this->_Check_Bounds();
				return *this;
			};
			base_pixel_view& operator--(int) noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				auto _out = *this;
				this->at_ -= this->stride_;
				this->_Check_Bounds();
				return _out;
			};

			constexpr base_pixel_view() noexcept = default;

			constexpr base_pixel_view(const base_pixel_view& other) noexcept = default;
			base_pixel_view& operator=(const base_pixel_view& other) noexcept = default;

			constexpr base_pixel_view(base_pixel_view&& other) noexcept = default;
			base_pixel_view& operator=(base_pixel_view&& other) noexcept = default;

#if SAE_ENGINE_DEBUG_LEVEL > 0
			constexpr explicit base_pixel_view(pointer _at, uint32_t _stride, pointer _begin, pointer _end) :
				at_{ _at }, stride_{ _stride }, begin_{ _begin }, end_{ _end }
			{
				assert(_stride != 0);
			};
#else
			constexpr explicit base_pixel_view(pointer _at, uint8_t _stride) noexcept :
				at_{ _at }, stride_{ _stride }
			{};
#endif
		protected:
			uint32_t stride_ = 0;
			pointer at_ = 0;
#if SAE_ENGINE_DEBUG_LEVEL > 0
			pointer begin_ = 0;
			pointer end_ = 0;
#endif
		};

	public:

		struct pixel_view;

		struct const_pixel_view : public base_pixel_view<const uint8_t>
		{
			constexpr const_pixel_view() noexcept = default;
#if SAE_ENGINE_DEBUG_LEVEL > 0
			constexpr explicit const_pixel_view(pointer _at, uint8_t _stride, pointer _begin, pointer _end) :
				base_pixel_view{ _at, _stride, _begin, _end }
			{};
#else
			constexpr explicit const_pixel_view(pointer _at, uint8_t _stride) noexcept :
				base_pixel_view{ _at, _stride }
			{};
#endif
			constexpr const_pixel_view(base_pixel_view _pxv) :
				base_pixel_view{ _pxv }
			{};

			constexpr const_pixel_view(Texture::pixel_view _pxv);
			constexpr const_pixel_view(base_pixel_view<uint8_t> _pxv);


		};
		struct pixel_view : public base_pixel_view<uint8_t>
		{

			reference at(uint8_t i) noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
#if SAE_ENGINE_DEBUG_LEVEL > 0
				this->_Check_Value();
				assert(i < this->stride_);
#endif
				return *(this->at_ + i);
			};
			reference operator[](uint8_t i) noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
			{
				return this->at(i);
			};

			constexpr pixel_view() noexcept = default;
#if SAE_ENGINE_DEBUG_LEVEL > 0
			constexpr pixel_view(pointer _at, uint8_t _stride, pointer _begin, pointer _end) :
				base_pixel_view{ _at, _stride, _begin, _end }
			{};
#else
			constexpr pixel_view(pointer _at, uint8_t _stride) noexcept :
				base_pixel_view{ _at, _stride }
			{};
#endif
			constexpr pixel_view(base_pixel_view _pxv) :
				base_pixel_view{ _pxv }
			{};
			
			friend const_pixel_view;

		};


		inline pixel_view view_begin() noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
		{
			auto _at = this->data_.data();
			return pixel_view{ _at, this->stride()
#if SAE_ENGINE_DEBUG_LEVEL > 0
				, _at, _at + this->data_.size()
#endif		
			};
		};
		inline const_pixel_view view_begin() const noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
		{
			auto _at = this->data_.data();
			return const_pixel_view{ _at, this->stride()
#if SAE_ENGINE_DEBUG_LEVEL > 0
				, _at, _at + this->data_.size()
#endif		
			};
		};
		inline const_pixel_view view_cbegin() const noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
		{
			auto _at = this->data_.data();
			return const_pixel_view{ _at, this->stride()
#if SAE_ENGINE_DEBUG_LEVEL > 0
				, _at, _at + this->data_.size()
#endif		
			};
		};

		inline pixel_view view_end() noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
		{
			auto _at = this->data_.data() + this->data_.size();
			return pixel_view{ _at, this->stride()
#if SAE_ENGINE_DEBUG_LEVEL > 0
				, _at - this->data_.size(), _at
#endif		
			};
		};
		inline const_pixel_view view_end() const noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
		{
			auto _at = this->data_.data() + this->data_.size();
			return const_pixel_view{ _at, this->stride()
#if SAE_ENGINE_DEBUG_LEVEL > 0
				, _at - this->data_.size(), _at
#endif		
			};
		};
		inline const_pixel_view view_cend() const noexcept((SAE_ENGINE_DEBUG_LEVEL == 0))
		{
			auto _at = this->data_.data() + this->data_.size();
			return const_pixel_view{ _at, this->stride()
#if SAE_ENGINE_DEBUG_LEVEL > 0
				, _at - this->data_.size(), _at
#endif		
			};
		};

		using iterator = typename ContainerT::iterator;
		using const_iterator = typename ContainerT::const_iterator;
		using reverse_iterator = typename ContainerT::reverse_iterator;
		using const_reverse_iterator = typename ContainerT::const_reverse_iterator;

		inline iterator begin() noexcept { return this->data_.begin(); };
		inline const_iterator begin() const noexcept { return this->data_.cbegin(); };
		inline const_iterator cbegin() const noexcept { return this->data_.cbegin(); };

		inline reverse_iterator rbegin() noexcept { return this->data_.rbegin(); };
		inline const_reverse_iterator rbegin() const noexcept { return this->data_.crbegin(); };
		inline const_reverse_iterator crbegin() const noexcept { return this->data_.crbegin(); };

		inline iterator end() noexcept { return this->data_.end(); };
		inline const_iterator end() const noexcept { return this->data_.cend(); };
		inline const_iterator cend() const noexcept { return this->data_.cend(); };

		inline reverse_iterator rend() noexcept { return this->data_.rend(); };
		inline const_reverse_iterator rend() const noexcept { return this->data_.crend(); };
		inline const_reverse_iterator crend() const noexcept { return this->data_.crend(); };

		/**
		 * @brief Returns the height of the texture in pixels
		*/
		size_t height() const noexcept;

		/**
		 * @brief Returns the width of the texture in pixels
		*/
		size_t width() const noexcept;

		/**
		 * @brief Returns the encoding of the texture
		*/
		ENCODING encoding() const noexcept;

		/**
		 * @brief Returns the size of the image in pixels, multiply by stride() to get size in bytes
		*/
		size_t size() const noexcept;

		/**
		 * @brief Sets the color of the pixels in the range [_begin, _end)
		 * @return pixel after the final pixel erased
		*/
		pixel_view set_color(pixel_view _begin, pixel_view _end, std::vector<uint8_t> _newColor)
		{
			assert(_end.stride() == this->stride());
			assert(_end.stride() == _begin.stride());
			assert(_newColor.size() == this->stride());

			auto _at = _begin.data();
			auto _afterLast = _end.data();
			auto _it = _newColor.begin();

			for (_at; _at != _afterLast; ++_at)
			{
				(*_at) = (*_it);
				++_it;
				if (_it == _newColor.end())
					_it = _newColor.begin();
			};

			return ++_end;
		};

		/**
		 * @brief Sets the color of the provided pixel 
		 * @param _at pixel to erase
		 * @return pixel after the one that was erased
		*/
		pixel_view set_color(pixel_view _at, std::vector<uint8_t> _newColor)
		{
			assert(_at.stride() == this->stride());
			assert(_newColor.size() == this->stride());

			for (size_t n = 0; n < _at.stride(); ++n)
			{
				_at[n] = _newColor[n];
			};

			return _at + 1;
		};



		/**
		 * @brief Returns pointer to the first value in the texture
		*/
		value_type* data() noexcept
		{
			return this->data_.data();
		};
		
		/**
		 * @brief Returns const pointer to the first value in the texture 
		*/
		const value_type* data() const noexcept
		{
			return this->data_.data();
		};


		/**
		 * @brief Resizes the texture to be _width by _height pixels in size, new values added are set to 0x00, if the
		 * new size is smaller than the old one, the image will be cropped
		 * @param _width new width in pixels
		 * @param _height new height in pixels
		*/
		void resize(size_t _width, size_t _height);


		/**
		 * @brief Attempts to paste another texture into this one at the provided offset, fails if other texture 
		 * has a different image encoding
		 * @param _other texture to paste into this
		 * @param _x x-axis offset to paste at ([image left] = [x = 0])
		 * @param _y y-axis offset to paste at ([image top] = [y = 0])
		 * @return succeed / fail
		*/
		bool paste(const Texture& _other, size_t _x, size_t _y);







		void save_to_file(const std::filesystem::path& _path) const;


		template <typename _FwdIter>
		explicit Texture(ENCODING _encoding, size_t _width, size_t _height, _FwdIter _begin, _FwdIter _end, const allocator_type& _alloc = allocator_type{}) :
			encoding_{ _encoding }, width_{ _width }, height_{ _height }, data_{ _begin, _end }, alloc_{ _alloc }
		{};

		explicit Texture(ENCODING _encoding, size_t _width, size_t _height, const allocator_type& _alloc = allocator_type{});
		explicit Texture(ENCODING _encoding, const allocator_type& _alloc = allocator_type{});
		explicit Texture(ENCODING _encoding, const std::filesystem::path& _pngPath, const allocator_type& _alloc = allocator_type{});


	private:
		ENCODING encoding_ = ENCODING::INVALID;
		allocator_type alloc_;
		size_t height_ = 0;
		size_t width_ = 0;
		std::vector<value_type, allocator_type> data_{};
	};

	constexpr Texture::const_pixel_view::const_pixel_view(Texture::pixel_view _pxv) :
		base_pixel_view{ _pxv.at_, _pxv.stride_ 
#if SAE_ENGINE_DEBUG_LEVEL > 0
		, _pxv.begin_, _pxv.end_
#endif
	}
	{};

	constexpr Texture::const_pixel_view::const_pixel_view(base_pixel_view<uint8_t> _pxv) :
		const_pixel_view{ pixel_view{_pxv } }
	{};


	struct lib_texture
	{
	private:

		// texture.new(encoding, [width, height]) -> Texture_Data
		static int new_f(lua_State* _lua);

		// texture.new(encoding, path) -> Texture_Data
		static int load_f(lua_State* _lua);


		// texture:width() -> integer
		static int width(lua_State* _lua);

		// texture:height() -> integer
		static int height(lua_State* _lua);

		// texture:size() -> integer
		static int size(lua_State* _lua);

		// texture:encoding() -> string
		static int encoding(lua_State* _lua);

		// texture:stride() -> integer
		static int stride(lua_State* _lua);

		// texture:bitdepth() -> integer
		static int bitdepth(lua_State* _lua);


		// texture:erase(offset, count, color (check encoding))
		static int set_color(lua_State* _lua);

		// texture:resize(width, height)
		static int resize(lua_State* _lua);

		// texture:paste(Texture, x, y)
		static int paste(lua_State* _lua);



		// texture:save(path) -> bool
		static int save(lua_State* _lua);



		// texture:__gc()
		static int gc(lua_State* _lua);


		const static inline luaL_Reg funcs_m[] =
		{
			luaL_Reg{ "width", &width },
			luaL_Reg{ "height", &height },
			luaL_Reg{ "size", &size },
			luaL_Reg{ "encoding", &encoding },
			luaL_Reg{ "stride", &stride },
			luaL_Reg{ "bitdepth", &bitdepth },

			luaL_Reg{ "set_color", &set_color },
			luaL_Reg{ "resize", &resize },
			luaL_Reg{ "paste", &paste },


			luaL_Reg{ "save", &save },


			luaL_Reg{ "__gc", &gc },
			luaL_Reg{ NULL, NULL }
		};

		const static inline luaL_Reg funcs_f[] =
		{
			luaL_Reg{ "new", &new_f },
			luaL_Reg{ "load", &load_f },
			luaL_Reg{ NULL, NULL }
		};

		constexpr static inline auto TYPENAME = "SAEEngine.Texture_Data";

	public:

		static Texture* to_texture(lua_State* _lua, int _idx);
		
		constexpr static inline const char* tname() noexcept
		{
			return TYPENAME;
		};

		static int lua_open(lua_State* _lua);


	};


}