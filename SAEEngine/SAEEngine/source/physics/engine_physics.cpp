#include "engine_physics.h"

namespace sae::engine
{

	distance_t position_t::operator-(const position_t& rhs) const noexcept
	{
		return distance_t{ this->x - rhs.x, this->y - rhs.y, this->z - rhs.z };
	};

	position_t position_t::operator+(const distance_t& rhs) const noexcept
	{
		return position_t{ this->x + rhs.dx, this->y + rhs.dy, this->z + rhs.dz };
	};
	position_t& position_t::operator+=(const distance_t& rhs) noexcept
	{
		this->x += rhs.dx;
		this->y += rhs.dy;
		this->z += rhs.dz;
		return *this;
	};

	position_t position_t::operator-(const distance_t& rhs) const noexcept
	{
		return position_t{ this->x - rhs.dx, this->y - rhs.dy, this->z - rhs.dz };
	};
	position_t& position_t::operator-=(const distance_t& rhs) noexcept
	{
		this->x -= rhs.dx;
		this->y -= rhs.dy;
		this->z -= rhs.dz;
		return *this;
	};



	distance_t distance_t::operator+(distance_t rhs) const noexcept
	{
		return distance_t{ this->dx + rhs.dx, this->dy + rhs.dy, this->dz + rhs.dz };
	};
	distance_t& distance_t::operator+=(distance_t rhs) noexcept
	{
		this->dx += rhs.dx;
		this->dy += rhs.dy;
		this->dz += rhs.dz;
		return *this;
	};

	distance_t distance_t::operator-(distance_t rhs) const noexcept
	{
		return distance_t{ this->dx - rhs.dx, this->dy - rhs.dy, this->dz - rhs.dz };
	};
	distance_t& distance_t::operator-=(distance_t rhs) noexcept
	{
		this->dx -= rhs.dx;
		this->dy -= rhs.dy;
		this->dz -= rhs.dz;
		return *this;
	};

	velocity_t distance_t::operator/(duration_t _dt) const noexcept
	{
		return velocity_t{ distance_t{this->dx / _dt, this->dy / _dt, this->dz / _dt}, _dt };
	};
	





	distance_t velocity_t::operator*(duration_t _dt) const noexcept
	{
		return distance_t{ this->dx.dx * _dt, this->dx.dy * _dt, this->dx.dz * _dt };
	};
	acceleration_t velocity_t::operator/(duration_t _dt) const noexcept
	{
		return acceleration_t{ velocity_t{ distance_t{ this->dx.dx / _dt, this->dx.dy / _dt, this->dx.dz / _dt}, this->dt }, _dt };
	};




	

}