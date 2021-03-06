#pragma once

#include "SAEEngine.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <unordered_set>

namespace sae::engine
{

	struct GFXObject
	{
	public:
		
		std::vector<int>& get_decorators() noexcept { return this->decorators_; };
		const std::vector<int>& get_decorators() const noexcept { return this->decorators_; };

		int lua_self() const noexcept { return this->self_; };
		void set_self(int _self) noexcept { this->self_ = _self; };

		virtual bool good() const = 0;
		virtual void update() = 0;
		virtual void destroy() = 0;
		virtual ~GFXObject() 
		{
#ifdef SAE_ENGINE_DESTRUCTOR_DEBUG
			std::cout << "~GFXObject()"; 
#endif
		};

	private:
		int self_ = 0;
		std::vector<int> decorators_{};
	};

	struct color_rgba
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t a = 0;
	};

	struct position2D
	{
		int16_t x = 0;
		int16_t y = 0;
		float_t z = 0.0f;
	};

	struct size2D
	{
		int16_t width = 0;
		int16_t height = 0;
	};

	struct WidgetObject : public GFXObject
	{
	public:

		void set_color(color_rgba _col) noexcept;
		color_rgba get_color() const noexcept;

		void set_position(position2D _pos) noexcept;
		position2D get_position() const noexcept;

		void set_size(size2D _s) noexcept;
		size2D get_size() const noexcept;

		bool intersects(int16_t _x, int16_t _y) const noexcept;

		virtual bool good() const = 0;
		virtual void update() = 0;
		virtual void draw(const glm::mat4& _projectionMat) = 0;
		virtual void destroy() = 0;

		virtual ~WidgetObject() = default;

	private:
		color_rgba color_{};
		position2D position_{ 0, 0, 0.0f };
		size2D size_{ 100, 100 };
	};

	struct WorldObject : public GFXObject
	{
		virtual bool good() const = 0;
		virtual void update() = 0;
		virtual void draw(const glm::mat4& _projectionMat, const glm::mat4& _cameraMat) = 0;
	};






}