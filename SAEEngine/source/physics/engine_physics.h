#pragma once

#include "SAEEngine.h"

#include <cstdint>

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

#include <chrono>

namespace sae::engine
{

	struct position_t;
	struct distance_t;
	struct velocity_t;
	struct acceleration_t;
	struct force_t;
	struct mass_t;




	using direction_t = glm::vec3;
	using magnitude_t = float_t;







	class physics_clock
	{
	public:
		using rep = int64_t;
		using period = std::nano;
		using duration = std::chrono::duration<rep, period>;
		using time_point = std::chrono::time_point<physics_clock>;
		static constexpr bool is_steady = false;
		static time_point now() noexcept;
	};

	using duration_t = typename physics_clock::duration;
	using time_point_t = typename physics_clock::time_point;



	struct basic_unit
	{
		constexpr auto x() const noexcept { return (this->mag * this->dir.x); };
		constexpr auto y() const noexcept { return (this->mag * this->dir.y); };
		constexpr auto z() const noexcept { return (this->mag * this->dir.z); };

		constexpr glm::vec3 as_vec() const noexcept { return (this->dir * this->mag); };

		constexpr basic_unit() noexcept = default;
		constexpr basic_unit(const direction_t& _d, magnitude_t _mag) noexcept : 
			dir{ _d }, mag{ _mag }
		{};

		direction_t dir{};
		magnitude_t mag{};
	};



	struct position_t
	{
		magnitude_t x{};
		magnitude_t y{};
		magnitude_t z{};
	};
	
	struct distance_t : public basic_unit 
	{
		using basic_unit::basic_unit;
	};

	struct velocity_t : public basic_unit 
	{
		using basic_unit::basic_unit;
	};

	struct acceleration_t : public basic_unit
	{
		using basic_unit::basic_unit;
	};

	struct force_t : public basic_unit
	{
		using basic_unit::basic_unit;
	};

	struct mass_t
	{
		magnitude_t mag;
	};



	distance_t operator-(const position_t& lhs, const position_t& rhs) noexcept;

	constexpr static inline position_t operator+(const position_t& lhs, const distance_t& rhs) noexcept
	{
		return position_t
		{
			lhs.x + rhs.x(),
			lhs.y + rhs.y(),
			lhs.z + rhs.z()
		};
	};
	position_t& operator+=(position_t& lhs, const distance_t& rhs) noexcept;

	static inline position_t operator-(const position_t& lhs, const distance_t& rhs) noexcept
	{
		return position_t
		{
			lhs.x - rhs.x(),
			lhs.y - rhs.y(),
			lhs.z - rhs.z()
		};
	}; 
	position_t& operator-=(position_t& lhs, const distance_t& rhs) noexcept;



	distance_t operator+(const distance_t& rhs, const distance_t& lhs) noexcept;
	distance_t& operator+=(distance_t& rhs, const distance_t& lhs) noexcept;

	distance_t operator-(const distance_t& rhs, const distance_t& lhs) noexcept;
	distance_t& operator-=(distance_t& rhs, const distance_t& lhs) noexcept;



	velocity_t operator+(const velocity_t& rhs, const velocity_t& lhs) noexcept;
	velocity_t& operator+=(velocity_t& rhs, const velocity_t& lhs) noexcept;

	velocity_t operator-(const velocity_t& rhs, const velocity_t& lhs) noexcept;
	velocity_t& operator-=(velocity_t& rhs, const velocity_t& lhs) noexcept;

	template <typename _Rep, typename _Period>
	velocity_t operator/(const distance_t& lhs, std::chrono::duration<_Rep, _Period> rhs)
	{
		return velocity_t{ lhs.dir, lhs.mag / (magnitude_t)std::chrono::duration_cast<duration_t>(rhs).count() };
	};

	template <typename _Rep, typename _Period>
	distance_t operator*(const velocity_t& lhs, std::chrono::duration<_Rep, _Period> rhs)
	{
		auto _adjustedMag = lhs.mag * (magnitude_t)std::chrono::duration_cast<duration_t>(rhs).count();
		return distance_t{ lhs.dir, _adjustedMag };
	};



	acceleration_t operator+(const acceleration_t& rhs, const acceleration_t& lhs) noexcept;
	acceleration_t& operator+=(acceleration_t& rhs, const acceleration_t& lhs) noexcept;

	acceleration_t operator-(const acceleration_t& rhs, const acceleration_t& lhs) noexcept;
	acceleration_t& operator-=(acceleration_t& rhs, const acceleration_t& lhs) noexcept;

	template <typename _Rep, typename _Period>
	acceleration_t operator/(const velocity_t& lhs, std::chrono::duration<_Rep, _Period> rhs)
	{
		return acceleration_t{ lhs.dir, lhs.mag / (magnitude_t)std::chrono::duration_cast<duration_t>(rhs).count() };
	};

	template <typename _Rep, typename _Period>
	velocity_t operator*(const acceleration_t& lhs, std::chrono::duration<_Rep, _Period> rhs)
	{
		auto _adjustedMag = lhs.mag * (magnitude_t)std::chrono::duration_cast<duration_t>(rhs).count();
		return acceleration_t{ lhs.dir, _adjustedMag };
	};



	force_t operator+(const force_t& rhs, const force_t& lhs) noexcept;
	force_t& operator+=(force_t& rhs, const force_t& lhs) noexcept;

	force_t operator-(const force_t& rhs, const force_t& lhs) noexcept;
	force_t& operator-=(force_t& rhs, const force_t& lhs) noexcept;


	force_t operator*(const acceleration_t& lhs, const mass_t& rhs);
	force_t operator*(const mass_t& lhs, const acceleration_t& rhs);

	acceleration_t operator/(const force_t& lhs, const mass_t& rhs);
	mass_t operator/(const force_t& lhs, const acceleration_t& rhs);






	mass_t operator+(const mass_t& rhs, const mass_t& lhs) noexcept;
	mass_t& operator+=(mass_t& rhs, const mass_t& lhs) noexcept;

	mass_t operator-(const mass_t& rhs, const mass_t& lhs) noexcept;
	mass_t& operator-=(mass_t& rhs, const mass_t& lhs) noexcept;






	struct PhysicsObject
	{
		position_t pos_;
		velocity_t vel_;
		mass_t mass_;
	};

	template <typename _Rep, typename _Period>
	PhysicsObject& apply(PhysicsObject& lhs, const acceleration_t& rhs, std::chrono::duration<_Rep, _Period> dt)
	{
		lhs.vel_ += (rhs * dt);
		return lhs;
	};

	template <typename _Rep, typename _Period>
	PhysicsObject& apply(PhysicsObject& lhs, const force_t& rhs, std::chrono::duration<_Rep, _Period> dt)
	{
		return apply(lhs, rhs / lhs.mass_, dt);
	};

	template <typename _Rep, typename _Period>
	PhysicsObject& update(PhysicsObject& lhs, std::chrono::duration<_Rep, _Period> dt)
	{
		lhs.pos_ += (lhs.vel_ * dt);
		return lhs;
	};




}