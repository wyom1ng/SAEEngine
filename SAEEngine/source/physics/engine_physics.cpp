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



	velocity_t operator+(const velocity_t& rhs, const velocity_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() + lhs.as_vec();
		return velocity_t{ glm::normalize(_vec), glm::distance(_vec, {}) };
	};
	velocity_t& operator+=(velocity_t& rhs, const velocity_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() + lhs.as_vec();
		rhs.dir = glm::normalize(_vec);
		rhs.mag = glm::distance(_vec, {});
		return rhs;
	};

	velocity_t operator-(const velocity_t& rhs, const velocity_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() - lhs.as_vec();
		return velocity_t{ glm::normalize(_vec), glm::distance(_vec, {}) };
	};
	velocity_t& operator-=(velocity_t& rhs, const velocity_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() - lhs.as_vec();
		rhs.dir = glm::normalize(_vec);
		rhs.mag = glm::distance(_vec, {});
		return rhs;
	};


	acceleration_t operator+(const acceleration_t& rhs, const acceleration_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() + lhs.as_vec();
		return acceleration_t{ glm::normalize(_vec), glm::distance(_vec, {}) };
	};
	acceleration_t& operator+=(acceleration_t& rhs, const acceleration_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() + lhs.as_vec();
		rhs.dir = glm::normalize(_vec);
		rhs.mag = glm::distance(_vec, {});
		return rhs;
	};

	acceleration_t operator-(const acceleration_t& rhs, const acceleration_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() - lhs.as_vec();
		return acceleration_t{ glm::normalize(_vec), glm::distance(_vec, {}) };
	};
	acceleration_t& operator-=(acceleration_t& rhs, const acceleration_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() - lhs.as_vec();
		rhs.dir = glm::normalize(_vec);
		rhs.mag = glm::distance(_vec, {});
		return rhs;
	};






	force_t operator+(const force_t& rhs, const force_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() + lhs.as_vec();
		return force_t{ glm::normalize(_vec), glm::distance(_vec, {}) };
	};
	force_t& operator+=(force_t& rhs, const force_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() + lhs.as_vec();
		rhs.dir = glm::normalize(_vec);
		rhs.mag = glm::distance(_vec, {});
		return rhs;
	};

	force_t operator-(const force_t& rhs, const force_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() - lhs.as_vec();
		return force_t{ glm::normalize(_vec), glm::distance(_vec, {}) };
	};
	force_t& operator-=(force_t& rhs, const force_t& lhs) noexcept
	{
		auto _vec = rhs.as_vec() - lhs.as_vec();
		rhs.dir = glm::normalize(_vec);
		rhs.mag = glm::distance(_vec, {});
		return rhs;
	};

	force_t operator*(const acceleration_t& lhs, const mass_t& rhs)
	{
		return force_t{ lhs.dir, (lhs.mag * rhs.mag) };
	};
	force_t operator*(const mass_t& lhs, const acceleration_t& rhs)
	{
		return rhs * lhs;
	};

	acceleration_t operator/(const force_t& lhs, const mass_t& rhs)
	{
		return acceleration_t{ lhs.dir , lhs.mag / rhs.mag };
	};
	mass_t operator/(const force_t& lhs, const acceleration_t& rhs)
	{
		
	};


	mass_t operator+(const mass_t& rhs, const mass_t& lhs) noexcept
	{
		return mass_t{ rhs.mag + lhs.mag };
	};
	mass_t& operator+=(mass_t& rhs, const mass_t& lhs) noexcept
	{
		rhs.mag += lhs.mag;
		return rhs;
	};

	mass_t operator-(const mass_t& rhs, const mass_t& lhs) noexcept
	{
		return mass_t{ rhs.mag - lhs.mag };
	};
	mass_t& operator-=(mass_t& rhs, const mass_t& lhs) noexcept
	{
		rhs.mag -= lhs.mag;
		return rhs;
	};

}