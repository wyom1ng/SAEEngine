#include "engine_physics.h"

namespace sae::engine
{




	physics_clock::time_point physics_clock::now() noexcept
	{
		return time_point{ std::chrono::steady_clock::now().time_since_epoch() };
	};



	distance_t operator-(const position_t& lhs, const position_t& rhs) noexcept
	{
		glm::vec3 _vec{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
		distance_t _out{};
		_out.dir = glm::normalize(_vec);
		_out.mag = glm::distance(glm::vec3{}, _vec);
		return _out;
	};

	position_t& operator+=(position_t& lhs, const distance_t& rhs) noexcept
	{
		lhs.x += rhs.x();
		lhs.y += rhs.y();
		lhs.z += rhs.z();
		return lhs;
	};	
	position_t& operator-=(position_t& lhs, const distance_t& rhs) noexcept
	{
		lhs.x -= rhs.x();
		lhs.y -= rhs.y();
		lhs.z -= rhs.z();
		return lhs;
	};



	distance_t operator+(const distance_t& rhs, const distance_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() + lhs.as_vec();
		return distance_t
		{
			glm::normalize(_vec),
			glm::distance(_vec, {})
		};
	};
	distance_t& operator+=(distance_t& rhs, const distance_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() + lhs.as_vec();
		rhs.dir = glm::normalize(_vec);
		rhs.mag = glm::distance(_vec, {});
		return rhs;
	};

	distance_t operator-(const distance_t& rhs, const distance_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() - lhs.as_vec();
		return distance_t
		{
			glm::normalize(_vec),
			glm::distance(_vec, {})
		};
	};
	distance_t& operator-=(distance_t& rhs, const distance_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() - lhs.as_vec();
		rhs.dir = glm::normalize(_vec);
		rhs.mag = glm::distance(_vec, {});
		return rhs;
	};








}