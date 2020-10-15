#pragma once

#include <variant>
#include <type_traits>
#include <concepts>

namespace sae
{
	struct other_t {};
	constexpr static inline other_t other{};



	template <typename LT, typename RT>
	struct either
	{
	public:
		using good_type = LT;
		using bad_type = RT;
	private:
		using variant_type = std::variant<LT, RT>;
	public:

		bool good() const noexcept { return (this->vt_.index() == 0); };
		explicit operator bool() const noexcept { return this->good(); };

		LT& get() { return std::get<0>(this->vt_); };
		const LT& get() const { return std::get<0>(this->vt_); };

		LT& operator*() { return this->get(); };
		const LT& operator*() const { return this->get(); };

		RT& other() { return std::get<1>(this->vt_); };
		const RT& other() const { return std::get<1>(this->vt_); };



		either(const LT& _lt) requires std::is_copy_constructible<LT>::value :
			vt_{ std::in_place_index<0>, _lt }
		{};
		either(LT&& _lt) requires std::is_move_constructible<LT>::value :
			vt_{ std::in_place_index<0>, std::move(_lt) }
		{};
		
		either(other_t, const RT& _rt) requires std::is_copy_constructible<RT>::value :
			vt_{ std::in_place_index<1>, _rt }
		{};
		either(other_t, RT&& _rt) requires std::is_move_constructible<RT>::value :
			vt_{ std::in_place_index<1>, std::move(_rt) }
		{};

	private:
		variant_type vt_;
	};





}