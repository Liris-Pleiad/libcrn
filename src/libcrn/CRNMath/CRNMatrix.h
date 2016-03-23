/* Copyright 2014-2016 INSA-Lyon, Université Paris Descartes, ENS-Lyon
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
 * file: CRNMatrix.h
 * \author Yann LEYDIER
 */

#ifndef CRNMATRIX_HEADER
#define CRNMATRIX_HEADER

#include <CRNObject.h>
#include <CRNException.h>
#include <CRNMath/CRNMath.h>
#include <CRNString.h>
#include <vector>

namespace crn
{
	/****************************************************************************/
	/*! \brief Base matrix class
	 *
	 * Matrix 
	 * 
	 * \author 	Yann LEYDIER
	 * \date		Dec 2014
	 * \version 0.1
	 * \ingroup math
	 */
	template<typename T> class Matrix: public Object
	{
		public:
			/*! \brief Constructor 
			 * \throws	ExceptionDomain	width or height is null
			 * \param[in]	nrow	number or rows
			 * \param[in]	ncol	number of columns
			 * \param[in]	value	default value for elements
			 */
			Matrix(size_t nrow, size_t ncol, const T &value = T(0)) : rows(nrow), cols(ncol)
			{
				if ((rows == 0) || (cols == 0))
					throw ExceptionDomain("Matrix::Matrix(size_t nrow, size_t ncol, double value): null or negative row or column dimensions");
				datatype(rows * cols, value).swap(data);
			}

			/*! \brief Constructor from std::vector matrix
			 * \throws	ExceptionDimension	empty matrix
			 * \throws	ExceptionInvalidArgument	the argument is not a matrix
			 * \param[in]	m	a matrix
			 */
			Matrix(const std::vector<std::vector<T>> &m)
			{
				if (m.empty())
					throw ExceptionDimension("Matrix::Matrix(m): empty matrix.");
				if (m.front().empty())
					throw ExceptionDimension("Matrix::Matrix(m): empty matrix.");
				auto r = m.size();
				auto c = m.front().size();
				for (const auto &line : m)
					if (line.size() != c)
						throw ExceptionInvalidArgument("Matrix::Matrix(m): the argument is not a matrix.");
				rows = r;
				cols = c;
				datatype(rows * cols).swap(data);
				for (size_t r = 0; r < rows; ++r)
					std::copy(m[r].begin(), m[r].end(), data.begin() + r * cols);
			}

			/*! \brief Constructor from std::vector 1D vector
			 * \throws	ExceptionDimension	empty matrix
			 * \throws	ExceptionInvalidArgument	invalid orientation
			 * \param[in]	vect	a vector
			 * \param[in]	ori	VERTICAL to create a column vector, HORIZONTAL to create a line vector
			 */
			Matrix(const std::vector<T> &vect, Orientation ori = Orientation::VERTICAL)
			{
				if (vect.empty())
					throw ExceptionDimension("Matrix::Matrix(vect, orientation): empty matrix.");
				if (ori == Orientation::VERTICAL)
				{
					rows = vect.size();
					cols = 1;
					data = vect;
				}
				else if (ori == Orientation::HORIZONTAL)
				{
					rows = 1;
					cols = vect.size();
					data = vect;
				}
				else
					throw ExceptionInvalidArgument("Matrix::Matrix(vect, orientation): invalid orientation.");
			}

			/*! \brief Constructor from std::vector 1D vector
			 * \throws	ExceptionDimension	empty matrix
			 * \throws	ExceptionInvalidArgument	invalid orientation
			 * \param[in]	vect	a vector
			 * \param[in]	ori	VERTICAL to create a column vector, HORIZONTAL to create a line vector
			*/
			Matrix(std::vector<T> &&vect, Orientation ori = Orientation::VERTICAL)
			{
				if (vect.empty())
					throw ExceptionDimension("Matrix::Matrix(vect, orientation): empty matrix.");
				if (ori == Orientation::VERTICAL)
				{
					rows = vect.size();
					cols = 1;
					data = std::move(vect);
				}
				else if (ori == Orientation::HORIZONTAL)
				{
					rows = 1;
					cols = vect.size();
					data = std::move(vect);
				}
				else
					throw ExceptionInvalidArgument("Matrix::Matrix(vect, orientation): invalid orientation.");
			}

			/*! \brief Destructor */
			virtual ~Matrix() override = default;
			Matrix(const Matrix &) = default;
			template<typename Y> Matrix(const Matrix<Y> &m) : rows(m.GetRows()), cols(m.GetCols())
			{
				datatype(rows * cols).swap(data);
				std::copy(m.Std().begin(), m.Std().end(), data.begin());
			}
			Matrix(Matrix &&) = default;
			Matrix& operator=(const Matrix &) = default;
			Matrix& operator=(Matrix &&) = default;

			inline void Swap(Matrix &m)
			{
				std::swap(rows, m.rows);
				std::swap(cols, m.cols);
				std::swap(data, m.data);
			}

			/*! \brief Returns the number of rows.
			 *
			 * \return		The number of rows.
			 */
			inline size_t GetRows() const noexcept { return rows; }

			/*! \brief Returns the number of columns.
			 *
			 * \return		The number of columns.
			 */
			inline size_t GetCols() const noexcept { return cols; }

			inline const T& At(size_t pos) const noexcept { return data[pos]; }
			inline T& At(size_t pos) noexcept { return data[pos]; }
			inline const T& At(size_t r, size_t c) const noexcept { return data[r * cols + c]; }
			inline T& At(size_t r, size_t c) noexcept { return data[r * cols + c]; }

			/*! \brief Set all elements
			 * \param[in]	v	value to set all cells
			 */
			void SetAll(const T &v)
			{
				for (size_t r = 0; r < rows; ++r)
					for (size_t c = 0; c < cols; ++c)
						At(r, c) = v;
			}

			/*! \brief Change sign for all elements
			*/
			void Negative()
			{
				for (size_t r = 0; r < rows; ++r)
					for (size_t c = 0; c < cols; ++c)
						At(r, c) = -At(r, c);
			}

			/*! \brief Sets the values of a row
			 * \throws	ExceptionDomain	index out of range
			 * \throws	ExceptionDimension	argument has incorrect size
			 *
			 * \param[in]	r	row index
			 * \param[in]	row	the row to copy
			 */
			inline void SetRow(size_t r, const std::vector<T> &row)
			{
				if (r > rows)
					throw ExceptionDomain("Matrix::SetRow(): index out of range.");
				if (row.size() != cols)
					throw ExceptionDimension("Matrix::SetRow(): argument is not of the right size.");
				for (size_t c = 0; c < cols; ++c)
					At(r, c) = row[c];
			}

			/*! \brief Sets the values of a column
			 * \throws	ExceptionDomain	index out of range
			 * \throws	ExceptionDimension	argument has incorrect size
			 *
			 * \param[in]	c	column index
			 * \param[in]	col	the column to copy
			 */
			inline void SetColum(size_t c, const std::vector<T> &col)
			{
				if (c > cols)
					throw ExceptionDomain("Matrix::SetColumn(): index out of range.");
				if (col.size() != rows)
					throw ExceptionDimension("Matrix::SetColumn(): argument is not of the right size.");
				for (size_t r = 0; r < rows; ++r)
					At(r, c) = col[r];
			}

			/*! \brief Increases the value of an element 
			 * \throws	ExceptionDomain	index out of range
			 *
			 * \param[in]	r	row index
			 * \param[in]	c	column index
			 * \param[in]	delta	to add
			 */
			inline void IncreaseElement(size_t r, size_t c, const T &delta)
			{
				if (!areValidIndexes(r, c))
					throw ExceptionDomain("Matrix::IncreaseElement(): index out of range.");
				At(r, c) += delta;
			}

			/*! \brief Increase one row from matrix
			 * \throws	ExceptionDomain	row out of bounds
			 * \param[in]	r	row index
			 * \param[in]	v	term to add to row
			 */
			void IncreaseRow(size_t r, const T &v)
			{
				if (r >= rows)
					throw ExceptionDomain("Matrix::IncreaseRow(): column index out of range");
				for (size_t c = 0; c < cols; ++c)
					At(r, c) += v;
			}

			/*! \brief Increase one column from matrix
			 * \throws	ExceptionDomain	column out of bounds
			 * \param[in]	c	column index
			 * \param[in]	v	term to add to column
			 */
			void IncreaseColumn(size_t c, const T &v)
			{
				if (c >= cols)
					throw ExceptionDomain("Matrix::IncreaseColumn(): column index out of range");
				for (size_t r = 0; r < rows; ++r)
					At(r, c) += v;
			}

			Matrix& operator+=(const Matrix &m)
			{
				if ((m.GetRows() != rows) || (m.GetCols() != cols))
					throw ExceptionDimension("Matrix::+=(): incompatible dimensions");
				for (auto i : Range(data))
					At(i) += m.At(i);
				return *this;
			}
			Matrix& operator-=(const Matrix &m)
			{
				if ((m.GetRows() != rows) || (m.GetCols() != cols))
					throw ExceptionDimension("Matrix::+=(): incompatible dimensions");
				for (auto i : Range(data))
					At(i) -= m.At(i);
				return *this;
			}
			Matrix& operator*=(const Matrix &m)
			{ 
				if (cols != m.GetRows())
					throw ExceptionDimension("Matrix::*=(): incompatible dimensions");
				auto p = datatype(rows * m.GetCols());
				for (size_t r = 0; r < rows; r++)
					for (size_t c = 0; c < m.GetCols(); c++)
						for (size_t k = 0; k < cols; k++)
							p[r * m.GetCols() + c] += At(r, k) * m[k][c];							
				cols = m.GetCols();
				data.swap(p);
				return *this;
			}
			bool operator==(const Matrix &m) const
			{
				if ((m.GetRows() != rows) || (m.GetCols() != cols))
					return false;
				return std::equal(data.begin(), data.end(), m.data.begin());
			}
			inline bool operator!=(const Matrix &m) const
			{ return !(*this == m); }

			/*! \brief Scaling
			 *
			 * Scale all the cells of the matrix by a given factor
			 * \param[in]	d	scaling factor
			 */
			Matrix& operator*=(double d) {	for (auto &v : data) v = T(v * d); return *this; }

			/*! \brief Scaling
			 *
			 * Scale all the cells of the matrix by a given factor
			 * \param[in]	factor	scaling factor
			 */
			Matrix& operator/=(double d) { for (auto &v : data) v = T(v / d); return *this; }

			/*! \brief Scale one row from matrix
			 * \throws	ExceptionDomain	row out of bounds
			 * \param[in]	r	row index
			 * \param[in]	v	ratio to scale cells of row
			 */
			void MultRow(size_t r, double v)
			{
				if (r >= rows)
					throw ExceptionDomain("Matrix::ScaleRow(): row index out of range");
				for (size_t c = 0; c < cols; ++c)
					At(r, c) = T(v * At(r, c));
			}

			/*! \brief Scale one column from matrix
			 * \throws	ExceptionDomain	column out of bounds
			 * \param[in]	c	column index
			 * \param[in]	v	ratio to scale cells of column
			 */
			void MultColumn(size_t c, double v)
			{
				if (c >= cols)
					throw ExceptionDomain("Matrix::ScaleColumn(): column index out of range");
				for (size_t r = 0; r < rows; ++r)
					At(r, c) = T(v * At(r, c));
			}

			/*! \brief Centers all columns by subtracting the mean for each column */
			void CenterColumns()
			{
				auto means = std::vector<T>(cols, T(0));
				for (size_t r = 0; r < rows; ++r)
					for (size_t c = 0; c < cols; ++c)
						means[c] += At(r, c);
				for (size_t c = 0; c < cols; ++c)
					means[c] = means[c] / T(rows);
				for (size_t r = 0; r < rows; ++r)
					for (size_t c = 0; c < cols; ++c)
						At(r, c) -= means[c];
			}

			/*! \brief Reduces all columns by dividing by the standard deviation of each column 
			 * \param[in]	centered	are the data centered?
			 */
			void ReduceColumns(bool centered = true)
			{
				auto means = std::vector<T>(cols, T(0));
				if (!centered)
				{
					for (size_t r = 0; r < rows; ++r)
						for (size_t c = 0; c < cols; ++c)
							means[c] += At(r, c);
					for (size_t c = 0; c < cols; ++c)
						means[c] = means[c] / T(rows);
				}
				auto devs = std::vector<T>(cols, T(0));
				for (size_t r = 0; r < rows; ++r)
					for (size_t c = 0; c < cols; ++c)
						devs[c] += Sqr(At(r, c) - means[c]);
				for (size_t c = 0; c < cols; ++c)
					devs[c] = sqrt(devs[c] / T(rows));
				for (size_t r = 0; r < rows; ++r)
					for (size_t c = 0; c < cols; ++c)
						At(r, c) /= devs[c];
			}

			/*! \brief Swap two rows in matrix
			 * \throws	ExceptionDomain	index out of bounds
			 * \param[in]	r1	one row index
			 * \param[in]	r2	another row index
			 */
			void SwapRows(size_t r1, size_t r2)
			{
				if ((r1 >= rows) || (r2 >= rows))
					throw ExceptionDomain("Matrix::SwapRows(): index out of range");
				if (r1 != r2)
					for (size_t c = 0; c < cols; ++c)
						std::swap(At(r1, c), At(r2, c));
			}

			/*! \brief Swap two columns in matrix
			 * \throws	ExceptionDomain	index out of bounds
			 * \param[in]	c1	one column index
			 * \param[in]	c2	another column index
			 */
			void SwapColumns(size_t c1, size_t c2)
			{
				if ((c1 >= cols) || (c2 >= cols))
					throw ExceptionDomain("Matrix::SwapColumns(): index out of range");
				if (c1 != c2)
					for (size_t r = 0; r < rows; ++r)
						std::swap(At(r, c1), At(r, c2));
			}

			/*! \brief Normalizes the matrix to be used for convolution */
			void NormalizeForConvolution()
			{
				auto sum = T(0);
				for (size_t r = 0; r < rows; ++r)
					for (size_t c = 0; c < cols; ++c)
						sum += Abs(At(r, c));
				if (sum != T(0))
					for (size_t r = 0; r < rows; ++r)
						for (size_t c = 0; c < cols; ++c)
							At(r, c) /= sum;
			}

			/*! \return	the sum of the elements */
			typename TypeInfo<T>::SumType CumulateCells() const
			{
				auto sum = typename TypeInfo<T>::SumType(0);
				for (size_t r = 0; r < rows; r++)
					for (size_t c = 0; c < cols; c++)
						sum += At(r, c);
				return sum;
			}

			/*! \return the minimal value in the matrix */
			T GetMin() const
			{
				return *std::min_element(data.begin(), data.end());
			}

			/*! \return the maximal value in the matrix */
			T GetMax() const
			{
				return *std::max_element(data.begin(), data.end());
			}

			/*! \return	row and column number of the minimal element in the matrix */
			std::pair<size_t, size_t> Argmin() const
			{
				auto pos = std::min_element(data.begin(), data.end()) - data.begin();
				return std::make_pair(pos / cols, pos % cols);
			}

			/*! \return	row and column number of the maximal element in the matrix */
			std::pair<size_t, size_t> Argmax() const
			{
				auto pos = std::max_element(data.begin(), data.end()) - data.begin();
				return std::make_pair(pos / cols, pos % cols);
			}

			/*! \brief Create transposition
			 * \return	a new transposed matrix
			 */
			Matrix MakeTranspose()
			{
				auto out = Matrix(cols, rows);
				for (size_t r = 0; r < rows; ++r)
					for (size_t c = 0; c < cols; ++c)
						out[c][r] = At(r, c);
				return out;
			}

			/*! \brief Inplace transposition
			 * \return	a reference to self
			 */
			virtual Matrix& Transpose()
			{
				datatype newdata(cols * rows);
				for (size_t r = 0; r < rows; ++r)
					for (size_t c = 0; c < cols; ++c)
						newdata[c * rows + r] = At(r, c);
				data.swap(newdata);
				std::swap(rows, cols);
				return *this;
			}

			/*! \brief Creates a column matrix from a row
			 *
			 * \throws	ExceptionDomain	row out of bounds
			 * \param[in]	r	row index
			 * \return a new column matrix containing the transposed selected row
			 */
			Matrix MakeRowAsColumn(size_t r) const
			{
				if (r >= GetRows())
					throw ExceptionDomain("Matrix::MakeRowAsColumn(): index out of range");
				return Matrix(datatype(data.begin() + r * cols, data.begin() + (r + 1) * cols), Orientation::VERTICAL);
			}

			/*! \brief Extract a row from current matrix
			 * \throws	ExceptionDomain	index out of bounds
			 * \param[in]	r	row index
			 * \return	the new row matrix
			 */
			Matrix MakeRow(size_t r) const
			{	
				if (r >= GetRows())
					throw ExceptionDomain("Matrix::MakeRow(): row index out of range");
				return Matrix(datatype(data.begin() + r * cols, data.begin() + (r + 1) * cols), Orientation::VERTICAL);
			}

			/*! \brief Extracts one column from matrix
			 * \throws	ExceptionDomain	index out of bounds
			 * \param[in]	k	column index
			 * \return	the column as a new matrix
			 */
			Matrix MakeColumn(size_t k) const
			{
				if (k >= cols)
					throw ExceptionDomain("Matrix::MakeColumn(): column index out of range");
				auto column = Matrix(rows, 1, 0.0);
				for (size_t r = 0; r < rows; r++)
					column.At(r, 0) = At(r, k);
				return column;
			}

			/*! \brief Returns the row index of the maximal element in a row
			 * \throws	ExceptionDomain	row out of bounds
			 * \return	the index of the maximal element in the row
			 */
			size_t ArgmaxInRow(size_t r) const
			{
				if (r >= rows)
					throw ExceptionDomain("Matrix::ArgmaxInRow(): index out of range");
				return std::max_element(data.begin() + r * cols, data.begin() + (r + 1) * cols) - (data.begin() + r * cols);
			}

			/*! \brief Returns the row index of the maximal element in a column
			 * \throws	ExceptionDomain	row out of bounds
			 * \return	the index of the maximal element in the column
			 */
			size_t ArgmaxInColumn(size_t c) const
			{
				if (c >= cols)
					throw ExceptionDomain("Matrix::ArgmaxInColumn(): index out of range");
				auto max = At(0, c);
				auto index = size_t(0);
				for (size_t r = 0; r < rows; r++)
				{
					const auto &mrc = At(r, c);
					if (mrc > max)
					{
						max = mrc;
						index = r;
					}
				}
				return index;
			}

			/*! \brief Counts null cells in row
			 * \throws	ExceptionDomain	index out of bounds
			 * \param[in]	r	row index
			 * \return	the number of null cells in a given row
			 */
			size_t CountNullCellsInRow(size_t r) const
			{
				if (r >= rows)
					throw ExceptionDomain("Matrix::CountNullCellsInRow(): row index out of range");
				return std::count(data.begin() + r * cols, data.begin() + (r + 1) * cols, T(0));
			}

			/*! \brief Counts null cells in column
			 * \throws	ExceptionDomain	index out of bounds
			 * \param[in]	c	column index
			 * \return	the number of null cells in a given column
			 */
			size_t CountNullCellsInColumn(size_t c) const
			{
				if (c >= rows)
					throw ExceptionDomain("Matrix::CountNullCellsInColumn(): column index out of range");
				auto count = size_t(0);
				for (size_t r = 0; r < rows; ++r)
					if (At(r, c) == T(0))
						count += 1;
				return count;
			}

			/*! \return	number of null cells in matrix */
			size_t CountNullCells() const
			{
				return std::count(data.begin(), data.end(), T(0));
			}

			/*! \return	mean of patterns stored as the rows of a data matrix as a column vector */
			Matrix MakeColumnMeans() const
			{
				auto m = Matrix(1, cols);
				for (size_t r = 0; r < rows; ++r)
					for (size_t c = 0; c < cols; ++c)
						m.At(c) += At(r, c);
				m *= 1.0 / double(rows);
				return m;
			}

			/*! \return	standard deviation of patterns stored as the rows of a data matrix */
			Matrix MakeColumnDeviations(const Matrix &means) const
			{
				auto m = Matrix(1, cols);
				for (size_t r = 0; r < rows; ++r)
					for (size_t c = 0; c < cols; ++c)
						m.At(c) += Sqr(At(r, c) - means[c]);
				m *= 1.0 / double(rows);
				return m;
			}

			/*! \brief standard deviation of patterns stored as the rows of a data matrix 
			 * \param[in]	zero_means	indicate if mean values are considered as null or to be computed
			 */
			inline Matrix MakeColumnDeviations(bool zero_means = false) const
			{
				if (zero_means)
					return MakeColumnDeviations(Matrix{1, cols});
				else
					return MakeColumnDeviations(MakeColumnMeans());
			}

			/*! \return the covariance matrix */
			Matrix MakeCovariance() const
			{
				auto cov = Matrix(cols, cols);
				for (size_t i = 0; i < cols; ++i)
					for (size_t j = i; j < cols; ++j)
					{
						auto product = T(0);
						for (size_t k = 0; k < rows; ++k)
							product += At(k, i) * At(k, j);
						// Each pattern in supposed equiprobable
						// and thus weighted with 1/nbPatterns.
						product /= T(rows);
						cov[i][j] = product;
						if (i != j)
							cov[j][i] = product;
					}
				return cov;
			}

			/*! \brief Returns a row
			 * 
			 * \param[in]		r The row number.
			 * \return		A pointer to the row.
			 */
			inline T* operator[](size_t r) { return data.data() + r * cols; }

			/*! \brief Returns a row
			 * 
			 * \param[in]		r The row number.
			 * \return		A pointer to the row.
			 */
			inline const T* operator[](size_t r) const { return data.data() + r * cols; }

			/*! \returns	the content as a std::vector */
			inline const std::vector<T>& Std() const & noexcept { return data; }
			/*! \returns	the content as a std::vector */
			inline std::vector<T> Std() && { return std::move(data); }

			/*! \brief	Converts to image */
			template<typename IMG> IMG ToImage() const
			{
				auto img = IMG{cols, rows};
				std::copy(data.begin(), data.end(), img.Begin());
				return img;
			}

			/*! \brief Prints matrix into a string
			 * \return a string containing the matrix values
			 */
			String ToString() const
			{
				String s;
				for (size_t r = 0; r < rows; r++)
				{
					for (size_t c = 0; c < cols; c++)
					{
						if (c) s += U" ";
						s += At(r, c);
					}
					s += "\n";
				}
				return s;
			}

		protected:
			/*! \brief checks the validity of indexes */
			inline bool areValidIndexes(size_t r, size_t c) const { return (r < rows) && (c < cols); }

			using datatype = std::vector<T>; /*!< Internal data representation */
			datatype data; /*!< rows, cols */
			size_t rows, cols; /*!< dimensions of the matrix */
	};
	template<typename T> struct IsClonable<Matrix<T>> : public std::true_type {};

	template<typename T> Matrix<T> operator+(const Matrix<T> &m1, const Matrix<T> &m2)
	{ return Matrix<T>(m1) += m2; }
	template<typename T> Matrix<T> operator+(Matrix<T> &&m1, const Matrix<T> &m2)
	{ return std::move(m1 += m2); }
	template<typename T> Matrix<T> operator+(const Matrix<T> &m1, Matrix<T> &&m2)
	{ return std::move(m2 += m1); }

	template<typename T> Matrix<T> operator-(const Matrix<T> &m1, const Matrix<T> &m2)
	{ return Matrix<T>(m1) -= m2; }
	template<typename T> Matrix<T> operator-(Matrix<T> &&m1, const Matrix<T> &m2)
	{ return std::move(m1 -= m2); }

	template<typename T> Matrix<T> operator*(const Matrix<T> &m, double d)
	{ return std::move(Matrix<T>(m) *= d); }
	template<typename T> Matrix<T> operator*(Matrix<T> &&m, double d)
	{ return std::move(m *= d); }
	template<typename T> Matrix<T> operator*(double d, const Matrix<T> &m)
	{ return Matrix<T>(m) *= d; }
	template<typename T> Matrix<T> operator*(double d, Matrix<T> &&m)
	{ return std::move(m *= d); }
	
	template<typename T> Matrix<T> operator/(const Matrix<T> &m, double d)
	{ return std::move(Matrix<T>(m) /= d); }
	template<typename T> Matrix<T> operator/(Matrix<T> &&m, double d)
	{ return std::move(m /= d); }
	template<typename T> Matrix<T> operator/(double d, const Matrix<T> &m)
	{ return Matrix<T>(m) /= d; }
	template<typename T> Matrix<T> operator/(double d, Matrix<T> &&m)
	{ return std::move(m /= d); }	
	

	template<typename T> Matrix<T> operator*(const Matrix<T> &m1, const Matrix<T> &m2)
	{ return Matrix<T>(m1) *= m2; }
	template<typename T> Matrix<T> operator*(Matrix<T> &&m1, const Matrix<T> &m2)
	{ return std::move(m1 *= m2); }

	template<typename I> struct TypeInfo<Matrix<I>>
	{
		using SumType = Matrix<typename TypeInfo<I>::SumType>;
		using DiffType = Matrix<typename TypeInfo<I>::DiffType>;
		using DecimalType = Matrix<typename TypeInfo<I>::DecimalType>;
	};

}

namespace std
{
	template<typename T> void swap(crn::Matrix<T> &m1, crn::Matrix<T> &m2)
	{
		m1.Swap(m2);
	}
}
#endif

