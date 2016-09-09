/* Copyright 2011-2016 CoReNum
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
 * file: CRNAltoLayout.h
 * \author Yann LEYDIER
 */

/*! \cond */
#ifdef CRNAlto_HEADER
/*! \endcond */

/*! \brief XML Alto layout element
 * \ingroup xml
 * \author Yann LEYDIER
 */
class Layout: public Element
{
	public:
		Layout(const Layout&) = delete;
		Layout(Layout&&) = default;
		virtual ~Layout() override {}
		Layout& operator=(const Layout&) = delete;
		Layout& operator=(Layout&&) = default;

		/*! \brief Returns the list of style references */
		std::vector<Id> GetStyles() const;
		/*! \brief Adds a reference to a style */
		void AddStyle(const Id &styleid);
		/*! \brief Removes a reference to a style */
		void RemoveStyle(const Id &styleid);

		/*! \brief XML Alto Page element
		 * \ingroup xml
		 * \author Yann LEYDIER
		 */
		class Page: public Element
		{
			public:
				enum class Position { Undef = 0, Left, Right, Foldout, Single, Cover };

				Page(const Page&) = delete;
				Page(Page&&) = default;
				virtual ~Page() override {}
				Page& operator=(const Page&) = delete;
				Page& operator=(Page&&) = default;

				/*! \brief Returns the id of the element */
				const Id& GetId() const { return id; }
				/*! \brief Returns the class of the page (user defined class such as "title") */
				Option<StringUTF8> GetPageClass() const;
				/*! \brief Sets the class of the page (user defined class such as "title") */
				void SetPageClass(const StringUTF8 &s);
				/*! \brief Returns the list of style references */
				std::vector<Id> GetStyles() const;
				/*! \brief Adds a reference to a style */
				void AddStyle(const Id &styleid);
				/*! \brief Removes a reference to a style */
				void RemoveStyle(const Id &styleid);
				/*! \brief Returns the height of the page */
				Option<int> GetHeight() const;
				/*! \brief Sets the height of the page */
				void SetHeight(int i);
				/*! \brief Returns the width of the page */
				Option<int> GetWidth() const;
				/*! \brief Sets the width of the page */
				void SetWidth(int i);
				/*! \brief Returns the number of the page within the document */
				int GetPhysicalImageNumber() const;
				/*! \brief Sets the number of the page within the document */
				void SetPhysicalImageNumber(int pnum);
				/*! \brief Returns the page number that is printed on the document */
				Option<StringUTF8> GetPrintedImageNumber() const;
				/*! \brief Sets the page number that is printed on the document */
				void SetPrintedImageNumber(const StringUTF8 &s);
				enum class Quality { Undef = 0, Ok, Missing, MissingInOriginal, Damaged, Retained, Target, AsInOriginal };
				/*! \brief Returns the quality of the original page */
				Option<Quality> GetQuality() const;
				/*! \brief Sets the quality of the original page */
				void SetQuality(Quality q);
				/*! \brief Returns details on the quality of the original page */
				Option<StringUTF8> GetQualityDetail() const;
				/*! \brief Sets the details on the quality of the original page */
				void SetQualityDetail(const StringUTF8 &s);
				/*! \brief Returns the position of the page */
				Option<Position> GetPosition() const;
				/*! \brief Returns the id of the processing applied to the page */
				Option<Id> GetProcessing() const;
				/*! \brief Returns the estimated % of OCR accuracy on the page [0, 100] */
				Option<double> GetAccuracy() const;
				/*! \brief Sets the estimated % of OCR accuracy on the page [0, 100] */
				void SetAccuracy(double acc);
				/*! \brief Returns the confidence of OCR on the page [0, 1] */
				Option<double> GetPageConfidence() const;
				/*! \brief Sets the confidence of OCR on the page [0, 1] */
				void SetPageConfidence(double c);

#include <CRNXml/CRNAltoSpace.hpp>
				typedef std::weak_ptr<Space> SpacePtr;
				/*! \brief Returns the top margin */
				SpacePtr GetTopMargin() { return topMargin; }
				/*! \brief Returns the left margin */
				SpacePtr GetLeftMargin() { return leftMargin; }
				/*! \brief Returns the right margin */
				SpacePtr GetRightMargin() { return rightMargin; }
				/*! \brief Returns the bottom margin */
				SpacePtr GetBottomMargin() { return bottomMargin; }
				/*! \brief Returns the main print space */
				SpacePtr GetPrintSpace() { return printSpace; }
				/*! \brief Returns a space */
				Space& GetSpace(const Id &sid);

				/*! \brief Adds a top margin */
				Space& AddTopMargin(const Id &id_, double x, double y, double w, double h);
				/*! \brief Adds a left margin */
				Space& AddLeftMargin(const Id &id_, double x, double y, double w, double h);
				/*! \brief Adds a right margin */
				Space& AddRightMargin(const Id &id_, double x, double y, double w, double h);
				/*! \brief Adds a bottom margin */
				Space& AddBottomMargin(const Id &id_, double x, double y, double w, double h);
				/*! \brief Adds the print space */
				Space& AddPrintSpace(const Id &id_, double x, double y, double w, double h);
				/*! \brief Returns the list of all spaces */
				std::vector<SpacePtr> GetSpaces() const;
				
				/*! \brief Removes a space */
				void RemoveSpace(const Id &sid);

			private:
				/*! \brief Constructor from file */
				Page(const Element &el);
				/*! \brief Constructor */
				Page(const Element &el, const Id &id_, int image_number, Option<int> width, Option<int> height, Option<Position> position);
				/*! Updates the space cache */
				void update_subelements();

				Id id;
				SpacePtr topMargin, leftMargin, rightMargin, bottomMargin, printSpace;
				std::vector<std::shared_ptr<Space> > spaces;
				std::map<Id, std::weak_ptr<Space> > id_spaces;
				
			friend class Layout;
		};
		typedef std::weak_ptr<Page> PagePtr;

		/*! \brief Returns a page */
		Page& GetPage(const Id &pid);
		/*! \brief Returns the list of all pages */
		std::vector<PagePtr> GetPages() const;
		/*! \brief Adds a page */
		Page& AddPage(const Id &id_, int image_number, Option<int> width_ = Option<int>(), Option<int> height_ = Option<int>(), Option<Page::Position> pos = Option<Page::Position>());
		/*! \brief Adds a page after another */
		Page& AddPageAfter(const Id &pred, const Id &id_, int image_number, Option<int> width_ = Option<int>(), Option<int> height_ = Option<int>(), Option<Page::Position> pos = Option<Page::Position>());
		/*! \brief Adds a page before another */
		Page& AddPageBefore(const Id &next, const Id &id_, int image_number, Option<int> width_ = Option<int>(), Option<int> height_ = Option<int>(), Option<Page::Position> pos = Option<Page::Position>());

		/*! \brief Removes a page */
		void RemovePage(const Id &pid);

	private:
		/*! \brief Constructor from file */
		Layout(const Element &el);
		/*! Updates the page cache */
		void update_subelements();

		std::vector<std::shared_ptr<Page> > pages;
		std::map<Id, std::weak_ptr<Page> > id_pages;

	friend class Root;
};

#else
#error	you cannot include this file directly
#endif
