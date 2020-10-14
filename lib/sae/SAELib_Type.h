#ifndef SAE_LIB_TYPE_H
#define SAE_LIB_TYPE_H

namespace sae
{

	struct null_t
	{
		using type = void;
	};

}



/*
	Something interesting to develop into a proper view type potentially

	This is just a note for me, Jonathan Cline / Somewhat Accurate.

namespace sae::engine
{
	union fixed_t
	{
		struct
		{
			int16_t _Whole;
			int16_t _Dec;
		};
		int32_t _Rw;
	};

	static_assert(sizeof(fixed_t) == 4, "fixed_t failed a layout assert");

	static_assert(std::is_standard_layout<fixed_t>::value, "fixed_t is not standard layout");
	static_assert(std::is_trivially_constructible<fixed_t>::value, "fixed_t is not trivially constructible");
	static_assert(std::is_trivially_copyable<fixed_t>::value, "fixed_t is not trivially copyable");
	static_assert(std::is_trivially_copy_assignable<fixed_t>::value, "fixed_t is not trivially copy assignable");
	static_assert(std::is_trivially_move_constructible<fixed_t>::value, "fixed_t is not trivially moveable");
	static_assert(std::is_trivially_move_assignable<fixed_t>::value, "fixed_t is not trivially move assignable");
	static_assert(std::is_pod<fixed_t>::value, "fixed_t is not POD");

}

constexpr static inline sae::engine::fixed_t operator "" _fx(long double _d)
{
	return sae::engine::fixed_t{ (_d * 65536) };
};



*/



#endif