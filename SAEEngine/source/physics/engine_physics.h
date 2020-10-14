#pragma once

#include "SAEEngine.h"

#include <cstdint>

namespace sae::engine
{
	struct position_t;
	struct distance_t;
	struct velocity_t;
	struct acceleration_t;
	struct force_t;
	struct mass_t;

	using duration_t = int32_t;
	


	struct position_t
	{
		distance_t operator-(const position_t& rhs) const noexcept;

		position_t operator+(const distance_t& rhs) const noexcept;
		position_t& operator+=(const distance_t& rhs) noexcept;

		position_t operator-(const distance_t& rhs) const noexcept;
		position_t& operator-=(const distance_t& rhs) noexcept;

		constexpr explicit position_t() noexcept = default;
		constexpr explicit position_t(int32_t _x, int32_t _y, int32_t _z) noexcept : 
			x{ _x }, y{ _y }, z{ _z } 
		{};


		int32_t x = 0;
		int32_t y = 0;
		int32_t z = 0;
	};
	
	struct distance_t
	{
		distance_t operator+(distance_t rhs) const noexcept;
		distance_t& operator+=(distance_t rhs) noexcept;

		distance_t operator-(distance_t rhs) const noexcept;
		distance_t& operator-=(distance_t rhs) noexcept;

		velocity_t operator/(duration_t _dt) const noexcept;

		duration_t operator/(velocity_t rhs) const noexcept;

		explicit distance_t() noexcept = default;
		constexpr explicit distance_t(int32_t _dx, int32_t _dy, int32_t _dz) noexcept :
			dx{ _dx }, dy{ _dy }, dz{ _dz }
		{};

		int32_t dx = 0;
		int32_t dy = 0;
		int32_t dz = 0;
	};

	struct velocity_t
	{
		distance_t operator*(duration_t _dt) const noexcept;
		acceleration_t operator/(duration_t _dt) const noexcept;

		duration_t operator/(acceleration_t rhs) const noexcept;

		velocity_t operator+(velocity_t rhs) const noexcept;
		velocity_t& operator+=(velocity_t rhs) noexcept;

		velocity_t operator-(velocity_t rhs) const noexcept;
		velocity_t& operator-=(velocity_t rhs) noexcept;

		constexpr explicit velocity_t() noexcept = default;
		constexpr explicit velocity_t(distance_t _dx, duration_t _dt) noexcept :
			dx{ _dx }, dt{ _dt }
		{};

		distance_t dx{};
		duration_t dt = 0;
	};

	struct acceleration_t
	{
		velocity_t operator*(duration_t _dt) const noexcept;

		force_t operator*(mass_t rhs) const noexcept;

		acceleration_t operator+(acceleration_t rhs) const noexcept;
		acceleration_t& operator+=(acceleration_t rhs) noexcept;

		acceleration_t operator-(acceleration_t rhs) const noexcept;
		acceleration_t& operator-=(acceleration_t rhs) noexcept;




		constexpr explicit acceleration_t() noexcept = default;
		constexpr explicit acceleration_t(velocity_t _dv, duration_t _dt) noexcept :
			dv{ _dv }, dt{ _dt }
		{};


		velocity_t dv{};
		duration_t dt = 0;
	};

	struct force_t
	{
		mass_t operator/(acceleration_t rhs) const noexcept;
		acceleration_t operator/(mass_t rhs) const noexcept;

		force_t operator+(force_t rhs) const noexcept;
		force_t& operator+=(force_t rhs) noexcept;

		force_t operator-(force_t rhs) const noexcept;
		force_t& operator-=(force_t rhs) noexcept;




		int32_t x = 0;
		int32_t y = 0;
		int32_t z = 0;
	};

	struct mass_t
	{
		force_t operator*(acceleration_t rhs) const noexcept;

		mass_t operator+(mass_t rhs) const noexcept;
		mass_t& operator+=(mass_t rhs) noexcept;

		mass_t operator-(mass_t rhs) const noexcept;
		mass_t& operator-=(mass_t rhs) noexcept;

		int32_t mass = 0;

	};

	float_t magnitude(distance_t _rhs) noexcept;
	float_t magnitude(velocity_t _rhs) noexcept;
	float_t magnitude(acceleration_t _rhs) noexcept;
	float_t magnitude(force_t _rhs) noexcept;




	

	class PhysicsObject
	{

		position_t pos_{};
		velocity_t vel_{};
		mass_t mass_{};

	};





	int luaopen_physics(lua_State* _lua);
}