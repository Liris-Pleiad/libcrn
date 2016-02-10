/* Copyright 2015 INSA-Lyon
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
 * file: CRNPixel.h
 * \author Yann LEYDIER
 */

#ifndef CRNPixel_HEADER
#define CRNPixel_HEADER

#include <CRNMath/CRNMath.h>

/*! \defgroup	pixel	Pixel formats
 * \ingroup	image
 */
namespace crn
{
	/*! \addtogroup pixel */
	/*@{*/

	/*************************************************************
	 * Black & White
	 ************************************************************/
	namespace pixel
	{
		using BW = bool;
		static const BW BWWhite = true;
		static const BW BWBlack = false;
	}

	/*************************************************************
	 * RGB
	 ************************************************************/
	namespace pixel
	{
		struct HSV;
		struct XYZ;
		struct YUV;
		template<typename T> struct RGB
		{
			using type = T;
			constexpr RGB() noexcept {}
			constexpr RGB(T val) noexcept: r(val), g(val), b(val) {}
			constexpr RGB(T r_, T g_, T b_) noexcept: r(r_), g(g_), b(b_) {}
			template<typename Y> constexpr RGB(const RGB<Y> &p) noexcept: r(T(p.r)), g(T(p.g)), b(T(p.b)) {}
			RGB(const HSV &val) noexcept;
			RGB(const XYZ &p) noexcept;

			// bitwise sort
			inline bool operator<(const RGB &other) const noexcept
			{
				if (r < other.r) return true;
				else if (r == other.r)
				{
					if (g < other.g) return true;
					else if (g == other.g)
						return b < other.b;
				}
				return false;
			}
			constexpr bool operator==(const RGB &other) const noexcept { return (r == other.r) && (g == other.g) && (b == other.b); }
			constexpr bool operator!=(const RGB &other) const noexcept { return !(*this == other); }
			RGB& operator+=(const RGB &other) { r += other.r; g += other.g; b += other.b; return *this; }
			RGB& operator-=(const RGB &other) { r -= other.r; g -= other.g; b -= other.b; return *this; }

			T r = 0, g = 0, b = 0;
		};
		using RGB8 = RGB<uint8_t>;
	} // namespace pixel

	template<> template<typename I> struct TypeInfo<pixel::RGB<I>>
	{
		using SumType = pixel::RGB<typename TypeInfo<I>::SumType>;
		using DiffType = pixel::RGB<typename TypeInfo<I>::DiffType>;
		using DecimalType = pixel::RGB<typename TypeInfo<I>::DecimalType>;
	};

	template<typename T> constexpr crn::SumType<crn::pixel::RGB<T>> operator+(const crn::pixel::RGB<T> &p1, const crn::pixel::RGB<T> &p2)
	{
		return crn::SumType<crn::pixel::RGB<T>>{p1.r + p2.r, p1.g + p2.g, p1.b + p2.b};
	}
	template<typename T> constexpr crn::DiffType<crn::pixel::RGB<T>> operator-(const crn::pixel::RGB<T> &p1, const crn::pixel::RGB<T> &p2)
	{
		return crn::DiffType<crn::pixel::RGB<T>>{p1.r - p2.r, p1.g - p2.g, p1.b - p2.b};
	}
	template<typename T> constexpr crn::DecimalType<crn::pixel::RGB<T>> operator*(const crn::pixel::RGB<T> &p, double d)
	{
		return crn::DecimalType<crn::pixel::RGB<T>>{p.r * d, p.g * d, p.b * d};
	}
	template<typename T> constexpr crn::DecimalType<crn::pixel::RGB<T>> operator*(double d, const crn::pixel::RGB<T> &p)
	{
		return crn::DecimalType<crn::pixel::RGB<T>>{p.r * d, p.g * d, p.b * d};
	}
	template<typename T> constexpr crn::DecimalType<crn::pixel::RGB<T>> operator/(const crn::pixel::RGB<T> &p, double d)
	{
		return crn::DecimalType<crn::pixel::RGB<T>>{p.r / d, p.g / d, p.b / d};
	}

	template<typename T> crn::SumType<T> Abs(const crn::pixel::RGB<T> &p)
	{
		return crn::Abs(p.r) + crn::Abs(p.g) + crn::Abs(p.b);
	}
	/*@}*/
} // namespace crn

namespace std
{
	template<> template<typename T> class numeric_limits<crn::pixel::RGB<T>>
	{
		public:
			static constexpr bool is_specialized = true;
			static constexpr crn::pixel::RGB<T> min() noexcept { return numeric_limits<T>::min(); }
			static constexpr crn::pixel::RGB<T> max() noexcept { return numeric_limits<T>::max(); }
			static constexpr crn::pixel::RGB<T> lowest() noexcept { return numeric_limits<T>::lowest(); }
			static constexpr int  digits = numeric_limits<T>::digits;
			static constexpr int  digits10 = numeric_limits<T>::digits10;
			static constexpr bool is_signed = numeric_limits<T>::is_signed;
			static constexpr bool is_integer = numeric_limits<T>::is_integer;
			static constexpr bool is_exact = numeric_limits<T>::is_exact;
			static constexpr int radix = numeric_limits<T>::radix;
			static constexpr crn::pixel::RGB<T> epsilon() noexcept { return numeric_limits<T>::epsilon(); }
			static constexpr crn::pixel::RGB<T> round_error() noexcept { return numeric_limits<T>::round_error(); }

			static constexpr int  min_exponent = numeric_limits<T>::min_exponent;
			static constexpr int  min_exponent10 = numeric_limits<T>::min_exponent10;
			static constexpr int  max_exponent = numeric_limits<T>::max_exponent;
			static constexpr int  max_exponent10 = numeric_limits<T>::max_exponent10;

			static constexpr bool has_infinity = numeric_limits<T>::has_infinity;
			static constexpr bool has_quiet_NaN = numeric_limits<T>::has_quiet_NaN;
			static constexpr bool has_signaling_NaN = numeric_limits<T>::has_signaling_NaN;
			static constexpr float_denorm_style has_denorm = numeric_limits<T>::has_denorm;
			static constexpr bool has_denorm_loss = numeric_limits<T>::has_denorm_loss;
			static constexpr crn::pixel::RGB<T> infinity() noexcept { return numeric_limits<T>::infinity(); }
			static constexpr crn::pixel::RGB<T> quiet_NaN() noexcept { return numeric_limits<T>::quiet_NaN(); }
			static constexpr crn::pixel::RGB<T> signaling_NaN() noexcept { return numeric_limits<T>::signaling_NaN(); }
			static constexpr crn::pixel::RGB<T> denorm_min() noexcept { return numeric_limits<T>::denorm_min(); }

			static constexpr bool is_iec559 = numeric_limits<T>::is_iec559;
			static constexpr bool is_bounded = numeric_limits<T>::is_bounded;
			static constexpr bool is_modulo = numeric_limits<T>::is_modulo;

			static constexpr bool traps = numeric_limits<T>::traps;
			static constexpr bool tinyness_before = numeric_limits<T>::tinyness_before;
			static constexpr float_round_style round_style = numeric_limits<T>::round_style;
	};
} // namespace std

/*************************************************************
 * HSV
 ************************************************************/
namespace crn
{
	/*! \addtogroup pixel */
	/*@{*/
	namespace pixel
	{
		struct HSV
		{
			constexpr HSV() noexcept {}
			constexpr HSV(uint8_t h_, uint8_t s_, uint8_t v_) noexcept: h(h_), s(s_), v(v_) {}
			constexpr HSV(uint8_t v_) noexcept: v(v_) {}
			HSV(const RGB<uint8_t> &val) noexcept
			{
				auto max = Max(val.r, val.g, val.b);
				auto min = Min(val.r, val.g, val.b);
				auto diff = max - min;

				auto hue = 0.0;
				auto coeffh = 255.0 / 360.0;
				if (max == 0)
					hue = 0;
				else
				{
					if (max != min)
					{
						if (max == val.r)
						{
							hue = 60 * (( val.g / 255.0 - val.b / 255.0 ) / (diff / 255.0));
						}
						else if (max == val.g)
						{
							hue = 60 * ((val.b / 255.0 - val.r / 255.0) / (diff / 255.0)) + 120;
						}
						else
						{
							hue = 60 * ((val.r / 255.0 - val.g / 255.0) / (diff / 255.0)) + 240;
						}
					}
					if (hue < 0) hue += 360;
				}
				hue *= coeffh;
				h = (uint8_t)hue;
				auto sat = 0.0;
				auto coeffs = 255.0;
				if (max > 0)
					sat = 1.0 - (double(min) / max);
				s = uint8_t(sat * coeffs);
				v = uint8_t(max);
			}
			// bitwise sort
			bool operator<(const HSV &other) const noexcept
			{
				if (h < other.h) return true;
				else if (h == other.h)
				{
					if (s < other.s) return true;
					else if (s == other.s)
						return v < other.v;
				}
				return false;
			}
			constexpr bool operator==(const HSV &other) const noexcept { return (h == other.h) && (s == other.s) && (v == other.v); }
			constexpr bool operator!=(const HSV &other) const noexcept { return !(*this == other); }
			uint8_t h = 0, s = 0, v = 0;
		};

		template<typename T> RGB<T>::RGB(const HSV &val) noexcept
		{
			auto hue = double(val.h) / 42.5;
			if (hue == 6) hue = 0;
			auto i = int(hue);
			auto v1 = int(val.v) * (255 - val.s) / 255;
			auto v2 = int(val.v * (255 - val.s * (hue - i)) / 255);
			auto v3 = int(val.v * (255 - val.s * (1 - (hue - i))) / 255);
			switch (i)
			{
				case 0:
					r = T(val.v);
					g = T(v3);
					b = T(v1);
					break;
				case 1:
					r = T(v2);
					g = T(val.v);
					b = T(v1);
					break;
				case 2:
					r = T(v1);
					g = T(val.v);
					b = T(v3);
					break;
				case 3:
					r = T(v1);
					g = T(v2);
					b = T(val.v);
					break;
				case 4:
					r = T(v3);
					g = T(v1);
					b = T(val.v);
					break;
				default:
					r = T(val.v);
					g = T(v1);
					b = T(v2);
			}

		}
	} // pixel

	template<> struct TypeInfo<pixel::HSV>
	{
		using SumType = pixel::RGB<unsigned int>;
		using DiffType = pixel::RGB<int>;
		using DecimalType = pixel::RGB<double>;
	};

	inline crn::SumType<crn::pixel::HSV> operator+(const crn::pixel::HSV &p1, const crn::pixel::HSV &p2)
	{
		return pixel::RGB<uint8_t>(p1) + pixel::RGB<uint8_t>(p2);
	}
	inline crn::DiffType<crn::pixel::HSV> operator-(const crn::pixel::HSV &p1, const crn::pixel::HSV &p2)
	{
		return pixel::RGB<uint8_t>(p1) - pixel::RGB<uint8_t>(p2);
	}
	inline crn::DecimalType<crn::pixel::HSV> operator*(const crn::pixel::HSV &p, double d)
	{
		return pixel::RGB<uint8_t>(p) * d;
	}
	inline crn::DecimalType<crn::pixel::HSV> operator*(double d, const crn::pixel::HSV &p)
	{
		return pixel::RGB<uint8_t>(p) * d;
	}
	inline crn::DecimalType<crn::pixel::HSV> operator/(const crn::pixel::HSV &p, double d)
	{
		return pixel::RGB<uint8_t>(p) / d;
	}
	/*@}*/
} // crn

/*************************************************************
 * 2D Cartesian vector
 ************************************************************/
namespace crn
{
	/*! \addtogroup pixel */
	/*@{*/
	namespace pixel
	{
		template<typename R, typename T> struct Polar2D;

		template<typename T> struct Cart2D
		{
			using type = T;
			constexpr Cart2D() noexcept {}
			constexpr Cart2D(T x_, T y_ = 0) noexcept: x(x_), y(y_) {}
			Cart2D(const std::complex<double> &cx) : x(T(cx.real())), y(T(cx.imag())) {}
			template<typename Y> constexpr Cart2D(const Cart2D<Y> &p) noexcept: x(p.x), y(p.y) {}
			template<typename Y, typename Z> Cart2D(const Polar2D<Y, Z> &p);
			operator std::complex<double>() const { return {double(x), double(y)}; }

			// bitwise sort
			bool operator<(const Cart2D &other) const noexcept
			{
				if (x < other.x) return true;
				else if (x == other.x) return y < other.y;
				return false;
			}
			constexpr bool operator==(const Cart2D &other) const noexcept { return (x == other.x) && (y == other.y); }
			constexpr bool operator!=(const Cart2D &other) const noexcept { return !(*this == other); }
			Cart2D& operator+=(const Cart2D &other) { x += other.x; y += other.y; return *this; }

			T x = 0, y = 0;
		};
	} // namespace pixel

	template<> template<typename I> struct TypeInfo<pixel::Cart2D<I>>
	{
		using SumType = pixel::Cart2D<typename TypeInfo<I>::SumType>;
		using DiffType = pixel::Cart2D<typename TypeInfo<I>::DiffType>;
		using DecimalType = pixel::Cart2D<typename TypeInfo<I>::DecimalType>;
	};

	template<typename T> constexpr crn::SumType<crn::pixel::Cart2D<T>> operator+(const crn::pixel::Cart2D<T> &p1, const crn::pixel::Cart2D<T> &p2)
	{
		return crn::SumType<crn::pixel::Cart2D<T>>{p1.x + p2.x, p1.y + p2.y};
	}
	template<typename T> constexpr crn::DiffType<crn::pixel::Cart2D<T>> operator-(const crn::pixel::Cart2D<T> &p1, const crn::pixel::Cart2D<T> &p2)
	{
		return crn::DiffType<crn::pixel::Cart2D<T>>{p1.x - p2.x, p1.y - p2.y};
	}
	template<typename T> constexpr crn::DecimalType<crn::pixel::Cart2D<T>> operator*(const crn::pixel::Cart2D<T> &p, double d)
	{
		return crn::DecimalType<crn::pixel::Cart2D<T>>{p.x * d, p.y * d};
	}
	template<typename T> constexpr crn::DecimalType<crn::pixel::Cart2D<T>> operator*(double d, const crn::pixel::Cart2D<T> &p)
	{
		return crn::DecimalType<crn::pixel::Cart2D<T>>{p.x * d, p.y * d};
	}
	template<typename T> constexpr crn::DecimalType<crn::pixel::Cart2D<T>> operator/(const crn::pixel::Cart2D<T> &p, double d)
	{
		return crn::DecimalType<crn::pixel::Cart2D<T>>{p.x / d, p.y / d};
	}
	/*@}*/
} // namespace crn

#include <CRNMath/CRNMath.h>

/*************************************************************
 * 2D polar vector
 ************************************************************/
namespace crn
{
	/*! \addtogroup pixel */
	/*@{*/
	namespace pixel
	{
		template<typename R, typename T> struct Polar2D
		{
			using rho_type = R;
			using theta_type = T;
			constexpr Polar2D() noexcept {}
			constexpr Polar2D(R r, T t = 0) noexcept: rho(r), theta(t) {}
			Polar2D(const std::complex<double> &cx) : rho(R(std::abs(cx))), theta(T(std::arg(cx))) { }
			template<typename Y, typename Z> constexpr Polar2D(const Polar2D<Y, Z> &p) noexcept: rho(R(p.rho)), theta(T(p.theta)) {}
			template<typename Y> Polar2D(const Cart2D<Y> &p): rho(R(sqrt(typename std::common_type<R, double>::type(Sqr(p.x) + Sqr(p.y))))), theta(T::Atan(typename std::common_type<R, double>::type(p.y), typename std::common_type<R, double>::type(p.x))) {}
			operator std::complex<double>() const { return std::complex<double>(Cart2D<double>(*this)); }

			// bitwise sort
			bool operator<(const Polar2D &other) const noexcept
			{
				if (rho < other.rho) return true;
				else if (rho == other.rho) return theta.value < other.theta.value;
				return false;
			}
			constexpr bool operator==(const Polar2D &other) const noexcept { return (rho == other.rho) && (theta == other.theta); }
			constexpr bool operator!=(const Polar2D &other) const noexcept { return !(*this == other); }
			Polar2D& operator+=(const Polar2D &other) { return *this = Cart2D<typename std::common_type<R, double>::type>(*this) + Cart2D<typename std::common_type<R, double>::type>(other); }

			R rho = 0;
			T theta = 0;
		};

		template<typename T> template<typename Y, typename Z> Cart2D<T>::Cart2D(const Polar2D<Y, Z> &p):
			x(T(p.rho * Cos(p.theta))),
			y(T(p.rho * Sin(p.theta)))
		{ }

	} // namespace pixel

	template<> template<typename I, typename J> struct TypeInfo<pixel::Polar2D<I, J>>
	{
		using SumType = pixel::Polar2D<typename TypeInfo<I>::SumType, J>;
		using DiffType = pixel::Polar2D<typename TypeInfo<I>::DiffType, J>;
		using DecimalType = pixel::Polar2D<typename TypeInfo<I>::DecimalType, J>;
	};

	template<typename R, typename T> constexpr crn::SumType<crn::pixel::Polar2D<R, T>> operator+(const crn::pixel::Polar2D<R, T> &p1, const crn::pixel::Polar2D<R, T> &p2)
	{
		return crn::TypeInfo<crn::pixel::Polar2D<R, T>>{crn::pixel::Cart2D<double>{p1} + crn::pixel::Cart2D<double>{p2}};
	}
	template<typename R, typename T> constexpr crn::DiffType<crn::pixel::Polar2D<R, T>> operator-(const crn::pixel::Polar2D<R, T> &p1, const crn::pixel::Polar2D<R, T> &p2)
	{
		return crn::DiffType<crn::pixel::Polar2D<R, T>>{crn::pixel::Cart2D<double>{p1} - crn::pixel::Cart2D<double>{p2}};
	}
	template<typename R, typename T> constexpr crn::DecimalType<crn::pixel::Polar2D<R, T>> operator*(const crn::pixel::Polar2D<R, T> &p, double d)
	{
		return crn::DecimalType<crn::pixel::Polar2D<R, T>>{p.rho * d, p.theta};
	}
	template<typename R, typename T> constexpr crn::DecimalType<crn::pixel::Polar2D<R, T>> operator*(double d, const crn::pixel::Polar2D<R, T> &p)
	{
		return crn::DecimalType<crn::pixel::Polar2D<R, T>>{p.rho * d, p.theta};
	}
	template<typename R, typename T> constexpr crn::DecimalType<crn::pixel::Polar2D<R, T>> operator/(const crn::pixel::Polar2D<R, T> &p, double d)
	{
		return crn::DecimalType<crn::pixel::Polar2D<R, T>>{p.rho / d, p.theta};
	}
	/*@}*/
} // namespace crn

/*************************************************************
 * XYZ real values from CIE 1931 model. Observer. = 2°, Illuminant = D65
 ************************************************************/
namespace crn
{
	/*! \addtogroup pixel */
	/*@{*/
	namespace pixel
	{
		struct Lab;
		struct Luv;
				
		struct XYZ
		{
			constexpr XYZ() {}
			constexpr XYZ(double x_, double y_ = 0.0, double z_ = 0.0): x(x_), y(y_), z(z_) {}
			XYZ(const RGB<uint8_t> &p);
			XYZ(const Lab &p);
			XYZ(const Luv &p);

			operator RGB<uint8_t>() const
			{
				auto var_X = x / 100;
				auto var_Y = y / 100;
				auto var_Z = z / 100;

				auto var_R = var_X *  3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
				auto var_G = var_X * -0.9689 + var_Y *  1.8758 + var_Z *  0.0415;
				auto var_B = var_X *  0.0557 + var_Y * -0.2040 + var_Z *  1.0570;

				if (var_R > 0.0031308) 
					var_R = 1.055 * pow(var_R, 1 / 2.4) - 0.055;
				else
					var_R *= 12.92;
				if (var_G > 0.0031308) 
					var_G = 1.055 * pow(var_G, 1 / 2.4) - 0.055;
				else
					var_G *= 12.92;
				if (var_B > 0.0031308) 
					var_B = 1.055 * pow(var_B, 1 / 2.4) - 0.055;
				else
					var_B *= 12.92;

				return RGB<uint8_t>(uint8_t(Cap((int)(var_R * 255), 0, 255)),
						uint8_t(Cap((int)(var_G * 255), 0, 255)),
						uint8_t(Cap((int)(var_B * 255), 0, 255)));
			}

			double x = 0.0, y = 0.0, z = 0.0;
		};

	}
	/*@}*/
	
} // namespace crn

/*************************************************************
 * YUV real values
 ************************************************************/

namespace crn
{
	/*! \addtogroup pixel */
	/*@{*/
	namespace pixel
	{	
		struct YUV
		{
			constexpr YUV() {}
			constexpr YUV(double y_, double u_ = 0.0, double v_ = 0.0): y(y_), u(u_), v(v_) {}
			YUV(const RGB<uint8_t> &p);
			
			operator RGB<uint8_t>() const
			{
				auto var_R = y + 1.13983 * v;
				auto var_G = y - 0.39465 * u - 0.58060 * v;
				auto var_B = y + 2.03211 * u;
				
				return RGB<uint8_t>(uint8_t(Cap((int)(var_R * 255), 0, 255)),
						uint8_t(Cap((int)(var_G * 255), 0, 255)),
						uint8_t(Cap((int)(var_B * 255), 0, 255)));
			}
			
			constexpr bool operator==(const YUV &other) const noexcept { return (y == other.y) && (u == other.u) && (v == other.v); }
			constexpr bool operator!=(const YUV &other) const noexcept { return !(*this == other); }
			YUV& operator+=(const YUV &other) { y += other.y; u += other.u; v += other.v; return *this; }
			YUV& operator-=(const YUV &other) { y -= other.y; u -= other.u; v -= other.v; return *this; }
			
			
			double y = 0.0, u = 0.0, v = 0.0;
		};
	}
	/*@}*/
	
	template<> struct TypeInfo<pixel::YUV>
	{
		using SumType = pixel::YUV;
		using DiffType = pixel::YUV;
		using DecimalType = pixel::YUV;
	};
	
	constexpr crn::pixel::YUV operator+(const crn::pixel::YUV &p1, const crn::pixel::YUV &p2)
	{
		return crn::pixel::YUV{p1.y - p2.y, p1.u - p2.u, p1.v - p2.v};
	}
	constexpr crn::pixel::YUV operator-(const crn::pixel::YUV &p1, const crn::pixel::YUV &p2)
	{
		return crn::pixel::YUV{p1.y - p2.y, p1.u - p2.u, p1.v - p2.v};
	}
	constexpr crn::pixel::YUV operator*(const crn::pixel::YUV &p, double d)
	{
		return crn::pixel::YUV{p.y * d, p.u * d, p.v * d};
	}
	constexpr crn::pixel::YUV operator*(double d, const crn::pixel::YUV &p)
	{
		return crn::pixel::YUV{p.y * d, p.u * d, p.v * d};
	}
	constexpr crn::pixel::YUV operator/(const crn::pixel::YUV &p, double d)
	{
		return crn::pixel::YUV{p.y / d, p.u / d, p.v / d};
	}
	

} // namespace crn

/*************************************************************
 * Lab real values from CIE 1976 L*a*b* model. Observer= 2°, Illuminant= D65
 ************************************************************/
namespace crn
{
	/*! \addtogroup pixel */
	/*@{*/
	namespace pixel
	{
		struct Lab
		{
			constexpr Lab() {}
			constexpr Lab(double l_, double a_ = 0.0, double b_ = 0.0): l(l_), a(a_), b(b_) {}
			Lab(const XYZ &p);
			operator RGB<uint8_t>() const { return RGB<uint8_t>(XYZ(*this)); }

			double l = 0.0, a = 0.0, b = 0.0;
		};
	}
	/*@}*/
} // namespace crn

/*************************************************************
 * Luv real values from CIE 1976 L*u*v* model. Observer= 2°, Illuminant= D65
 ************************************************************/
namespace crn
{
	/*! \addtogroup pixel */
	/*@{*/
	namespace pixel
	{
		struct Luv
		{
			constexpr Luv() {}
			constexpr Luv(double l_, double u_ = 0.0, double v_ = 0.0): l(l_), u(u_), v(v_) {}
			Luv(const XYZ &p);
			operator RGB<uint8_t>() const { return RGB<uint8_t>(XYZ(*this)); }

			double l = 0.0, u = 0.0, v = 0.0;
		};
	}
	/*@}*/
} // namespace crn

#endif


