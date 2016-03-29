/* Copyright 2009-2015 INSA Lyon, CoReNum, Université Paris Descartes
 * 
 * This file is part of libcrn.
 * 
 * libcrn is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libcrn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with libcrn.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * file: CRNType.h
 * \author Yann LEYDIER
 */

#ifndef CRNType_HEADER
#define CRNType_HEADER

#include <iterator>
#include <type_traits>
#include <memory>

#if __APPLE__
#	if	defined(__clang__) && (__clang_major__ < 7)
#		define int64_t __int64_t // MT : for Apple LLVM version 6.0 (clang-600.0.56)
#	endif
#endif

namespace crn
{
	/*! \addtogroup base */
	/*@{*/
	namespace traits
	{
		struct DummyType {};
		template<typename T> DummyType operator<(const T &, const T &) { return DummyType{}; }
		template<typename T> DummyType operator>(const T &, const T &) { return DummyType{}; }
		template<typename T> DummyType operator<=(const T &, const T &) { return DummyType{}; }
		template<typename T> DummyType operator>=(const T &, const T &) { return DummyType{}; }

		template<typename T> struct HasLT :
			public std::integral_constant<
			bool,
			!std::is_same<
			DummyType,
			decltype(std::declval<T const&>() < std::declval<T const&>())
			>::value
			>
		{};
		template<typename T> struct HasGT :
			public std::integral_constant<
			bool,
			!std::is_same<
			DummyType,
			decltype(std::declval<T const&>() > std::declval<T const&>())
			>::value
			>
		{};
		template<typename T> struct HasLE :
			public std::integral_constant<
			bool,
			!std::is_same<
			DummyType,
			decltype(std::declval<T const&>() <= std::declval<T const&>())
			>::value
			>
		{};
		template<typename T> struct HasGE :
			public std::integral_constant<
			bool,
			!std::is_same<
			DummyType,
			decltype(std::declval<T const&>() >= std::declval<T const&>())
			>::value
			>
		{};

		template<typename T> DummyType operator+(const T &, const T &) { return DummyType{}; }
		template<typename T> DummyType operator==(const T &, const T &) { return DummyType{}; }

		template<typename T> struct HasPlus :
			public std::integral_constant<
			bool,
			!std::is_same<
			DummyType,
			decltype(std::declval<T const&>() + std::declval<T const&>())
			>::value
			>
		{};
		template<typename T> struct HasEquals :
			public std::integral_constant<
			bool,
			!std::is_same<
			DummyType,
			decltype(std::declval<T const&>() == std::declval<T const&>())
			>::value
			>
		{};

		template<typename T> DummyType operator-(const T &, const T &) { return DummyType{}; }

		template<typename T> struct HasMinus :
			public std::integral_constant<
			bool,
			!std::is_same<
			DummyType,
			decltype(std::declval<T const&>() - std::declval<T const&>())
			>::value
			>
		{};

		template<typename T> DummyType operator*(const T &, const T &) { return DummyType{}; }

		template<typename T> struct HasInnerMult :
			public std::integral_constant<
			bool,
			!std::is_same<
			DummyType,
			decltype(std::declval<T const&>() * std::declval<T const&>())
			>::value
			>
		{};

		struct doubleWrapper { doubleWrapper(double) {} };
		template<typename T> DummyType operator*(const T &, doubleWrapper) { return DummyType{}; }
		template<typename T> DummyType operator*(doubleWrapper, const T &) { return DummyType{}; }

		template<typename T> struct HasRightOuterMult :
			public std::integral_constant<
			bool,
			!std::is_same<
			DummyType,
			decltype(std::declval<T const&>() * 0.0)
			>::value
			>
		{};
		template<typename T> struct HasLeftOuterMult :
			public std::integral_constant<
			bool,
			!std::is_same<
			DummyType,
			decltype(0.0 * std::declval<T const&>())
			>::value
			>
		{};

		template<typename T> DummyType operator/(const T &, const T &) { return DummyType{}; }

		template<typename T> struct HasDivide :
			public std::integral_constant<
			bool,
			!std::is_same<
			DummyType,
			decltype(std::declval<T const&>() / std::declval<T const&>())
			>::value
			>
		{};

		template<typename T> struct IsDereferenceable : public std::false_type {};
		template<typename T> struct IsDereferenceable<T*> : public std::true_type {};
		template<typename T> struct IsDereferenceable<std::shared_ptr<T>> : public std::true_type {};
		template<typename T> struct IsDereferenceable<std::unique_ptr<T>> : public std::true_type {};

	}

	/*! \brief A class containing informations on a type
	 *
	 * A class containing informations on a type
	 */
	template<typename T> struct TypeInfo
	{
		/*! \brief A safe type to compute a sum (e.g.: a larger integer type) */
		using SumType = decltype(std::declval<T>() + std::declval<T>());
		/*! \brief A safe type to compute a difference (e.g.: a signed type) */
		using DiffType = decltype(std::declval<T>() - std::declval<T>());
		/*! \brief A safe type to compute a difference (e.g.: a signed type) */
		using DecimalType = typename std::common_type<T, double>::type;
	};
	template<typename T> using SumType = typename TypeInfo<T>::SumType;
	template<typename T> using DiffType = typename TypeInfo<T>::DiffType;
	template<typename T> using DecimalType = typename TypeInfo<T>::DecimalType;

	template<
		typename P, 
		typename std::enable_if<traits::IsDereferenceable<P>::value, int>::type = 0
		> 
	typename std::pointer_traits<P>::element_type& Dereference(const P &p) { return *p; }

	template<
		typename P, 
		typename std::enable_if<!traits::IsDereferenceable<P>::value, int>::type = 0
		> 
	const P& Dereference(const P &p) { return p; }
	
	template<
		typename P, 
		typename std::enable_if<!traits::IsDereferenceable<P>::value, int>::type = 0
		> 
	P& Dereference(P &p) { return p; }

	/*! \brief	Returns an object of the same type that represents 0 */
	template<
		typename T, 
		typename std::enable_if<std::is_constructible<T, int>::value, int>::type = 0
		> 
	T Zero(const T &) { return T(0); }
	/*! \brief	Returns an object of the same type that represents 0 */
	template<
		typename T, 
		typename std::enable_if<
			!std::is_constructible<T, int>::value && traits::HasMinus<T>::value,
	 		int>::type = 0
		> 
	T Zero(const T &val) { return T(val - val); }

	/*! \brief A class that represents scalar values in [[b, e[[ */
	template<typename S> class ScalarRange
	{
		public:
			/*! \brief Creates a range [[be, en[[
			 * \param[in]	be	the first value	
			 * \param[in]	en	the value after the last value
			 */
			constexpr ScalarRange(S be, S en) noexcept : b(be), e(en) {}
			struct iterator: public std::iterator<std::input_iterator_tag, S>
			{
				constexpr iterator(S id = 0, int i = 1) noexcept : index(id), inc(i) {}
				inline S operator*() const noexcept { return index; }
				inline iterator& operator++() noexcept { index += inc; return *this; }
				inline bool operator==(const iterator &other) noexcept { return index == other.index; }
				inline bool operator!=(const iterator &other) noexcept { return index != other.index; }
				S index;
				int inc;
			};
			constexpr iterator begin() const { return iterator{b, (b <= e) ? 1 : -1}; }
			constexpr iterator end() const { return iterator{e, (b <= e) ? 1 : -1}; }
		private:
			S b, e;
	};

	/*! \brief Returns the size of a container
	 * \param[in]	cont	a container
	 * \return	the number of elements in the container
	 */
	template<class T> inline auto Size(const T &cont) noexcept(noexcept(cont.size())) -> decltype(cont.size()) { return cont.size(); }

	/*! \brief Creates a range [[b, e[[
	 * \param[in]	b	the first value	
	 * \param[in]	e	the value after the last value
	 */
	template<typename T> inline ScalarRange<T> Range(T b, T e) { return ScalarRange<T>(b, e); }
	/*! \brief Creates a range [[0, cont.size()[[
	 * \param[in]	cont	a container that define a method size()
	 */
	template<typename T> inline auto Range(const T &cont) -> ScalarRange<decltype(Size(cont))>
	{
		using stype = decltype(Size(cont));
		return ScalarRange<stype>(stype(0), Size(cont));
	}
	/*! \brief Creates a range ]]cont.size(), 0]]
	 * \param[in]	cont	a container that define a method size()
	 */
	template<typename T> inline ScalarRange<int64_t> ReverseRange(const T &cont)
	{ return ScalarRange<int64_t>(int64_t(Size(cont)) - 1, int64_t(-1)); }

	/*@}*/
}

#define CRN_DECLARE_ENUM_OPERATORS(Type) \
	constexpr Type operator|(Type e1, Type e2) noexcept\
	{\
		using inner = std::underlying_type<Type>::type;\
		return static_cast<Type>(static_cast<inner>(e1) | static_cast<inner>(e2));\
	}\
	inline Type& operator|=(Type &e1, Type e2) noexcept { return e1 = e1 | e2; }\
	constexpr Type operator&(Type e1, Type e2) noexcept \
	{\
		using inner = std::underlying_type<Type>::type;\
		return static_cast<Type>(static_cast<inner>(e1) & static_cast<inner>(e2));\
	}\
	inline Type& operator&=(Type &e1, Type e2) noexcept { return e1 = e1 & e2; }\
	constexpr Type operator^(Type e1, Type e2) noexcept\
	{\
		using inner = std::underlying_type<Type>::type;\
		return static_cast<Type>(static_cast<inner>(e1) ^ static_cast<inner>(e2));\
	}\
	inline Type& operator^=(Type &e1, Type e2) noexcept { return e1 = e1 ^ e2; }\
	constexpr bool operator!(Type e) noexcept\
	{\
		using inner = std::underlying_type<Type>::type;\
		return static_cast<inner>(e) == inner{0};\
	}

#define CRN_ALIAS_SMART_PTR(classname)\
	using S##classname = std::shared_ptr<classname>;\
	using SC##classname = std::shared_ptr<const classname>;\
	using W##classname = std::weak_ptr<classname>;\
	using WC##classname = std::weak_ptr<const classname>;\
	using U##classname = std::unique_ptr<classname>;\
	using UC##classname = std::unique_ptr<const classname>;

// define std::make_unique if needed
#if (__cplusplus == 201103L)
#	include <utility>
namespace std
{
	//Default init
	template<class T> inline unique_ptr<T> make_unique()
	{ return unique_ptr<T>(new T); }

	//Argument init
	template<class T, class... Args> inline unique_ptr<T> make_unique(Args&&... args)
	{ return unique_ptr<T>(new T(forward<Args>(args)...)); }
}
#endif

namespace crn
{
	template<
		typename T,
		typename std::enable_if<!std::is_lvalue_reference<T>::value, int>::type = 0
		> 
	inline std::shared_ptr<T> MoveShared(T &&v)
	{
		return std::make_shared<T>(std::move(v));
	}
	template<
		typename T,
		typename std::enable_if<!std::is_lvalue_reference<T>::value, int>::type = 0
		>
	inline std::unique_ptr<T> MoveUnique(T &&v)
	{
		return std::make_unique<T>(std::move(v));
	}
}

#endif


