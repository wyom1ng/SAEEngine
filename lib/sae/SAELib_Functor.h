#ifndef SAE_FUNCTOR_H
#define SAE_FUNCTOR_H

/*
	Copyright 2020 Jonathan Cline
	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
	(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
	publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
	so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


/*
	sae::functor<> works almost identically to the std::function<> type.
	However, instead of having to use std::bind for a member function, there is builtin functionality for "binding" a
	member function.
	This has substantially less compile-time code generation when compiling in Debug mode.
	Example Code:
	#include "SAE_Functor.h"
	#include <iostream>
	int foo(const int& _a)
	{
		return _a + 1;
	};
	struct bar
	{
		int foobar(const int& _a)
		{
			return _a + 1;
		};
		bar() = default;
	};
	int main()
	{
		int i = 0;
		bar _b{};
		sae::functor<int(const int&)> _fp = &foo;
		i = _fp(i);
		std::cout << i << '\n';
		_fp = std::pair{ &bar::foobar, &_b };
		i = _fp(i);
		std::cout << i << '\n';
		return 0;
	};
*/

#include <type_traits>
#include <functional>

namespace sae
{
	template <typename _T>
	struct cxType_t
	{
		using type = _T;
		constexpr cxType_t() = default;
	};

	namespace detail
	{
		template <typename ReturnT, typename... Args>
		struct functionPtr_t
		{
		public:

			virtual inline functionPtr_t<ReturnT, Args...>* clone() const = 0;
			virtual inline ReturnT invoke(Args... _a) const = 0;

			constexpr functionPtr_t() noexcept = default;
			virtual ~functionPtr_t() {};

		};

		template <typename ReturnT, typename... Args>
		struct freeFunctionPtr_t : public functionPtr_t<ReturnT, Args...>
		{
		public:

			virtual inline functionPtr_t<ReturnT, Args...>* clone() const final
			{
				return new freeFunctionPtr_t{ *this };
			};
			virtual inline ReturnT invoke(Args... args) const final
			{
				if constexpr (std::is_same<void, ReturnT>::value)
				{
					(*fptr)(args...);
				}
				else
				{
					return (*fptr)(args...);
				};
			};

			using function_pointer = ReturnT(*)(Args...);

			constexpr freeFunctionPtr_t(const function_pointer& _p) noexcept :
				fptr{ _p }
			{};
			constexpr freeFunctionPtr_t() noexcept = default;

		private:
			function_pointer fptr;

		};
		template <typename ReturnT, class ScopeT, typename... Args>
		struct memberFunctionPtr_t : public functionPtr_t<ReturnT, Args...>
		{
		public:

			virtual inline functionPtr_t<ReturnT, Args...>* clone() const final
			{
				return new memberFunctionPtr_t{ *this };
			};
			virtual inline ReturnT invoke(Args... args) const final
			{
				if constexpr (std::is_same<void, ReturnT>::value)
				{
					(class_ptr->*fptr)(args...);
				}
				else
				{
					return (class_ptr->*fptr)(args...);
				};
			};

			using function_pointer = ReturnT(ScopeT::*)(Args...);


			constexpr memberFunctionPtr_t(const function_pointer& _f, ScopeT* _c) noexcept :
				fptr{ _f }, class_ptr{ _c }
			{};

			constexpr memberFunctionPtr_t() noexcept = default;

		private:
			function_pointer fptr;
			ScopeT* class_ptr = nullptr;

		};

		template <typename ReturnT, typename... Args>
		struct functor_impl
		{
		public:

			using return_type = ReturnT;

			inline ReturnT invoke(Args... _args) const
			{
				if constexpr (std::is_same<ReturnT, void>::value)
				{
					this->ptr_->invoke(_args...);
				}
				else
				{
					return this->ptr_->invoke(_args...);
				};
			};
			inline ReturnT operator()(Args... _args) const
			{
				if constexpr (std::is_same<ReturnT, void>::value)
				{
					this->ptr_->invoke(_args...);
				}
				else
				{
					return this->ptr_->invoke(_args...);
				};
			};

			constexpr inline bool is_member_function() const noexcept
			{
				return this->member_function_;
			};
			constexpr inline bool good_pointer() const noexcept
			{
				return (this->ptr_);
			};

			inline explicit operator bool() const noexcept
			{
				return (this->good_pointer());
			};

			constexpr functor_impl(ReturnT(*_func)(Args...)) :
				ptr_{ new freeFunctionPtr_t<ReturnT, Args...>{_func} },
				member_function_{ false }
			{};
			template <class ScopeT>
			constexpr functor_impl(ReturnT(ScopeT::* _func)(Args...), ScopeT* _p = nullptr) :
				ptr_{ new memberFunctionPtr_t<ReturnT, ScopeT, Args...>{_func, _p} },
				member_function_{ true }
			{};

			functor_impl& operator=(ReturnT(*_func)(Args...))
			{
				delete ptr_;
				ptr_ = new detail::freeFunctionPtr_t<ReturnT, Args...>{ _func };
				this->member_function_ = false;
				return *this;
			};
			template <class ScopeT>
			functor_impl& operator=(std::pair<ReturnT(ScopeT::*)(Args...), ScopeT*>&& _memberFunc)
			{
				delete ptr_;
				ptr_ = new memberFunctionPtr_t<ReturnT, ScopeT, Args...>{ _memberFunc.first, _memberFunc.second };
				this->member_function_ = true;
				return *this;
			};

			constexpr functor_impl() noexcept :
				ptr_{ nullptr }, member_function_{ false }
			{};

			constexpr explicit functor_impl(const functor_impl& _o) :
				ptr_{ _o.ptr_->clone() }
			{};
			functor_impl& operator=(const functor_impl& _o)
			{
				delete ptr_;
				ptr_ = _o.ptr_->clone();
				return *this;
			};

			constexpr explicit functor_impl(functor_impl&& _o) noexcept :
				ptr_{ std::exchange(_o.ptr_, nullptr) }
			{};
			functor_impl& operator=(functor_impl&& _o) noexcept
			{
				delete this->ptr_;
				this->ptr_ = std::exchange(_o.ptr_, nullptr);
				return *this;
			}

			~functor_impl()
			{
				delete ptr_;
			}

		private:
			bool member_function_ = false;
			functionPtr_t<ReturnT, Args...>* ptr_;
		};

		template <typename T>
		struct funcExpander;

		template <typename Ret, typename... T>
		struct funcExpander <Ret(T...)>
		{
			using type = functor_impl<Ret, T...>;
			constexpr funcExpander() = default;
		};

		template <typename FunctionT>
		constexpr static inline auto cx_make_functor_helper_v()
		{
			return funcExpander<FunctionT>{};
		};

		struct functor_type_tag {};

	};

	template <typename FunctionT>
	struct functor : detail::functor_type_tag, public decltype(detail::cx_make_functor_helper_v<FunctionT>())::type
	{
	private:
		using parent_type = typename decltype(detail::cx_make_functor_helper_v<FunctionT>())::type;
	public:
		using parent_type::parent_type;
		using parent_type::operator=;
	};

}

#endif