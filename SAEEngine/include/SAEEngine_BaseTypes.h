#pragma once

#include "SAEEngine.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

namespace sae::engine
{
	struct GFXObject
	{
		virtual bool good() const = 0;
		virtual void update() = 0;
		virtual void destroy() = 0;
		virtual ~GFXObject() = default;
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