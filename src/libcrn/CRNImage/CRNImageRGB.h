/* Copyright 2006-2016 Yann LEYDIER, CoReNum, INSA-Lyon, ENS-Lyon
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
 * file: CRNImageRGB.h
 * \author Yann LEYDIER
 */

#ifndef CRNIMAGERGB_HEADER
#define CRNIMAGERGB_HEADER

#include <CRNImage/CRNImage.h>
#include <CRNUtils/CRNDefaultAction.h>

/*! \defgroup	imagergb	RGB images
 * \ingroup	image
 */
namespace crn
{
	/*! \addtogroup imagergb */
	/*@{*/

	/*************************************************************************************
	 * Edition
	 ************************************************************************************/

	/*! \brief Smooths and enhances the contrast */
	void EdgePreservingFilter(ImageRGB &img, size_t iter, uint8_t maxdist = 30);

	/*************************************************************************************
	 * Conversion
	 ************************************************************************************/

	template<typename T> Image<T> RedChannel(const Image<pixel::RGB<T>> &img)
	{
		auto out = Image<T>{img.GetWidth(), img.GetHeight()};
		for (auto tmp : Range(img))
			out.At(tmp) = img.At(tmp).r;
		return out;
	}

	template<typename T> Image<T> GreenChannel(const Image<pixel::RGB<T>> &img)
	{
		auto out = Image<T>{img.GetWidth(), img.GetHeight()};
		for (auto tmp : Range(img))
			out.At(tmp) = img.At(tmp).g;
		return out;
	}

	template<typename T> Image<T> BlueChannel(const Image<pixel::RGB<T>> &img)
	{
		auto out = Image<T>{img.GetWidth(), img.GetHeight()};
		for (auto tmp : Range(img))
			out.At(tmp) = img.At(tmp).b;
		return out;
	}

	/*! \brief Computes the saturation (S) channel */
	ImageGray SChannel(const ImageRGB &img);

	template<typename T> Image<T> VChannel(const Image<pixel::RGB<T>> &img)
	{
		auto out = Image<T>{img.GetWidth(), img.GetHeight()};
		for (auto tmp : Range(img))
			out.At(tmp) = Max(img.At(tmp).r, img.At(tmp).g, img.At(tmp).b);
		return out;
	}

	template<typename T> Image<T> YChannel(const Image<pixel::RGB<T>> &img)
	{
		auto out = Image<T>{img.GetWidth(), img.GetHeight()};
		for (auto tmp : Range(img))
			out.At(tmp) = T(DecimalType<T>(img.At(tmp).r) * 0.299 + 
					DecimalType<T>(img.At(tmp).g) * 0.587 + 
					DecimalType<T>(img.At(tmp).b) * 0.114); 
		return out;
	}

	template<typename T> Image<T> LChannel(const Image<pixel::RGB<T>> &img)
	{
		auto out = Image<T>{img.GetWidth(), img.GetHeight()};
		for (auto tmp : Range(img))
			out.At(tmp) = T((Max(img.At(tmp).r, img.At(tmp).g, img.At(tmp).b) + 
						Min(img.At(tmp).r, img.At(tmp).g, img.At(tmp).b)) / 2);
		return out;
	}

	/*! \brief Computes the pseudo-luminance (U') channel */
	ImageGray LPrime(const ImageRGB &img);
	/*! \brief Computes the pseudo-saturation channel */
	ImageGray PseudoSaturation(const ImageRGB &img);

	/*! \brief RGB to grayscale conversion action
	 * \ingroup action
	 */
	class RGB2Gray: public Action
	{
		public:
			virtual ~RGB2Gray() override { }
			/*! \brief Action = convert a colour image to grey */
			virtual ImageGray Convert(const ImageRGB &img) = 0;
	};

	/*! \brief Red pixels extraction action
	 * \ingroup action
	 */
	class RGB2GrayRed: public RGB2Gray
	{
		CRN_DECLARE_CLASS_CONSTRUCTOR(RGB2GrayRed)
			CRN_SERIALIZATION_CONSTRUCTOR(RGB2GrayRed)
		public:
			/*! \brief Default constructor */
			RGB2GrayRed() {}
			virtual ~RGB2GrayRed() override { }
			virtual const String& GetClassName() const override { static const String cn(U"RGB2GrayRed"); return cn; }
			virtual ImageGray Convert(const ImageRGB &img) override { return RedChannel(img); }
	};

	/*! \brief Green pixels extraction action
	 * \ingroup action
	 */
	class RGB2GrayGreen: public RGB2Gray
	{
		CRN_DECLARE_CLASS_CONSTRUCTOR(RGB2GrayGreen)
			CRN_SERIALIZATION_CONSTRUCTOR(RGB2GrayGreen)
		public:
			/*! \brief Default constructor */
			RGB2GrayGreen() {}
			virtual ~RGB2GrayGreen() override { }
			virtual const String& GetClassName() const override { static const String cn(U"RGB2GrayGreen"); return cn; }
			virtual ImageGray Convert(const ImageRGB &img) override { return GreenChannel(img); }
	};

	/*! \brief Blue pixels extraction action
	 * \ingroup action
	 */
	class RGB2GrayBlue: public RGB2Gray
	{
		CRN_DECLARE_CLASS_CONSTRUCTOR(RGB2GrayBlue)
			CRN_SERIALIZATION_CONSTRUCTOR(RGB2GrayBlue)
		public:
			/*! \brief Default constructor */
			RGB2GrayBlue() {}
			virtual ~RGB2GrayBlue() override { }
			virtual const String& GetClassName() const override { static const String cn(U"RGB2GrayBlue"); return cn; }
			virtual ImageGray Convert(const ImageRGB &img) override { return BlueChannel(img); }
	};

	/*! \brief Saturation pixels extraction action
	 * \ingroup action
	 */
	class RGB2GraySaturation: public RGB2Gray
	{
		CRN_DECLARE_CLASS_CONSTRUCTOR(RGB2GraySaturation)
			CRN_SERIALIZATION_CONSTRUCTOR(RGB2GraySaturation)
		public:
			/*! \brief Default constructor */
			RGB2GraySaturation() {}
			virtual ~RGB2GraySaturation() override { }
			virtual const String& GetClassName() const override { static const String cn(U"RGB2GraySaturation"); return cn; }
			virtual ImageGray Convert(const ImageRGB &img) override { return SChannel(img); }
	};

	/*! \brief Value pixels extraction action
	 * \ingroup action
	 */
	class RGB2GrayValue: public RGB2Gray
	{
		CRN_DECLARE_CLASS_CONSTRUCTOR(RGB2GrayValue)
			CRN_SERIALIZATION_CONSTRUCTOR(RGB2GrayValue)
		public:
			/*! \brief Default constructor */
			RGB2GrayValue() {}
			virtual ~RGB2GrayValue() override { }
			virtual const String& GetClassName() const override { static const String cn(U"RGB2GrayValue"); return cn; }
			virtual ImageGray Convert(const ImageRGB &img) override { return VChannel(img); }
	};

	/*! \brief Y pixels extraction action
	 * \ingroup action
	 */
	class RGB2GrayY: public RGB2Gray
	{
		CRN_DECLARE_CLASS_CONSTRUCTOR(RGB2GrayY)
			CRN_SERIALIZATION_CONSTRUCTOR(RGB2GrayY)
		public:
			/*! \brief Default constructor */
			RGB2GrayY() {}
			virtual ~RGB2GrayY() override { }
			virtual const String& GetClassName() const override { static const String cn(U"RGB2GrayY"); return cn; }
			virtual ImageGray Convert(const ImageRGB &img) override { return YChannel(img); }
	};

	/*! \brief L pixels extraction action
	 * \ingroup action
	 */
	class RGB2GrayL: public RGB2Gray
	{
		CRN_DECLARE_CLASS_CONSTRUCTOR(RGB2GrayL)
			CRN_SERIALIZATION_CONSTRUCTOR(RGB2GrayL)
		public:
			/*! \brief Default constructor */
			RGB2GrayL() {}
			virtual ~RGB2GrayL() override { }
			virtual const String& GetClassName() const override { static const String cn(U"RGB2GrayL"); return cn; }
			virtual ImageGray Convert(const ImageRGB &img) override { return LChannel(img); }
	};

	/*! \brief LPrime pixels extraction action
	 * \ingroup action
	 */
	class RGB2GrayLPrime: public RGB2Gray
	{
		CRN_DECLARE_CLASS_CONSTRUCTOR(RGB2GrayLPrime)
			CRN_SERIALIZATION_CONSTRUCTOR(RGB2GrayLPrime)
		public:
			/*! \brief Default constructor */
			RGB2GrayLPrime() {}
			virtual ~RGB2GrayLPrime() override { }
			virtual const String& GetClassName() const override { static const String cn(U"RGB2GrayLPrime"); return cn; }
			virtual ImageGray Convert(const ImageRGB &img) override { return LPrime(img); }
	};

	CRN_ALIAS_SMART_PTR(RGB2Gray)

	namespace protocol
	{
		template<> struct IsSerializable<RGB2Gray> : public std::true_type {};
		template<> struct IsSerializable<RGB2GrayRed> : public std::true_type {};
		template<> struct IsSerializable<RGB2GrayGreen> : public std::true_type {};
		template<> struct IsSerializable<RGB2GrayBlue> : public std::true_type {};
		template<> struct IsSerializable<RGB2GraySaturation> : public std::true_type {};
		template<> struct IsSerializable<RGB2GrayValue> : public std::true_type {};
		template<> struct IsSerializable<RGB2GrayY> : public std::true_type {};
		template<> struct IsSerializable<RGB2GrayL> : public std::true_type {};
		template<> struct IsSerializable<RGB2GrayLPrime> : public std::true_type {};
	}

	template<typename T> Image<T> MakeImageGray(const Image<pixel::RGB<T>> &img)
	{
		auto act = std::dynamic_pointer_cast<RGB2Gray>(DefaultAction::GetAction(U"RGB2Gray"));
		if (!act)
		{
			act = std::make_shared<RGB2GrayL>();
			DefaultAction::SetAction(U"RGB2Gray", act);
		}
		return act->Convert(img);
	}

	/*@}*/
}
#endif
