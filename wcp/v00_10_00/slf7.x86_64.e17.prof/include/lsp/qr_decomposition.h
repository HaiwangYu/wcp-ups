/*
    $Id$
    Copyright (C) 2008  Matwey V. Kornilov <matwey.kornilov@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _QR_DECOMPOSITION_H
#define _QR_DECOMPOSITION_H

#include <cmath>
#include <limits>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/storage.hpp>

#include <lsp/givens_rotation.h>
#include <lsp/householder_transform.h>

namespace lsp{

using namespace boost::numeric::ublas;

struct general_tag {};
struct bidiag_tag {};
struct tridiag_symmetric_tag {};

template< class T, class Tag = general_tag > class qr_decomposition;

namespace detail {
	template<class T> T hypot( const T& x, const T& y ) {
		typedef T value_type;

		if( std::abs(x) > std::abs(y) ) {
			const value_type t = y/x;
			return std::abs(x) * std::sqrt( value_type(1) + t*t );
		}
		if( y == value_type(0) ) {
			return 0;
		}
		const value_type t = x/y;
		return std::abs(y) * std::sqrt( value_type(1) + t*t );
	}
	template<class T> T wilkinson_shift( const T& a1, const T& b1, const T& a2 ) {
		typedef T value_type;
		const value_type d = (a1 - a2) / value_type(2);
		const value_type q = hypot(d,b1);
		return a2 - b1*b1 / ( d + ( d > 0 ? 1 : -1 ) * q );
	}
};

/**
 *  @brief A functor for the modified QR decomposition
 *
 *  Modified QR decomposition is iterative alghoritm that transformates
 *  bidiagonal banded matrix into diagonal form by the number of
 *  Givens transformations. The decomposition is one of SVD parts.
 *
 */
template<class T> class qr_decomposition<T, bidiag_tag> {
public:
	typedef T                                  matrix_type;   //!<
	typedef typename matrix_type::value_type   value_type;    //!<
	typedef typename matrix_type::size_type    size_type;     //!<
	typedef matrix_vector_slice< matrix_type > diagonal_type; //!<
private:
	struct regular_tag {};
	struct left_tag {};
	struct right_tag {};
private:
	matrix_type& m_matrix;
	mutable diagonal_type m_super;
	mutable diagonal_type m_leading;
private:

	template<class M1, class M2> void apply( M1& left, M2& right, const range& cell, const left_tag& ) const {
		typedef givens_rotation< value_type > givens_rotation_type;
		value_type z;

		z = m_super( cell(0) );
		m_super( cell(0) ) = 0;

		for( range::const_iterator it = cell.begin() + 1; it != cell.end() ; ++it ) {
			givens_rotation_type gr( m_leading(*it), z );

			gr.apply( row(left, *it), row(left, cell(0)) );
			if( *it == cell( cell.size() - 1 ) )
				break;
			gr.apply( m_super(*it), z );
		}
	}

	template<class M1, class M2> void apply( M1& left, M2& right, const range& cell, const right_tag& ) const {
		typedef givens_rotation< value_type > givens_rotation_type;
		value_type z;

		z = m_super( cell( cell.size() - 2 ) );
		m_super( cell( cell.size() - 2 ) ) = 0;

		for( range::const_reverse_iterator it = cell.rbegin() + 1; it != cell.rend(); ++it ) {
			givens_rotation_type gr( m_leading(*it), z );

			gr.apply( column( right, *it ), column( right, cell( cell.size() - 1 ) ) );
			if( *it == cell.start() )
				break;
			gr.apply( m_super(*it-1), z );
		}
	}

	template<class M1, class M2> void apply( M1& left, M2& right, const range& cell, const regular_tag& ) const {
		typedef givens_rotation< value_type > givens_rotation_type;

		const value_type lim = std::numeric_limits< value_type >::epsilon() * norm_inf( m_matrix );

		for( range::const_reverse_iterator it = cell.rbegin() + 1; it != cell.rend(); ++it ) {
			value_type last_super; /* use differential convergation control */
			do {
				last_super = m_super( *it );

				const value_type en1 = ( *it != cell(0) ? m_super(*it - 1) : value_type(0) );
				const value_type a1 = en1*en1 + m_leading(*it)*m_leading(*it);
				const value_type a2 = m_super(*it)*m_super(*it) + m_leading(*it+1)*m_leading(*it+1);
				const value_type b1 = m_super(*it)*m_leading(*it);
				value_type e0 = m_leading( cell(0) ) - detail::wilkinson_shift( a1, b1, a2 ) / m_leading( cell(0) );
				value_type z = m_super( cell(0) );

				givens_rotation_type gr_left( e0, z );
				for( range::const_iterator it2 = cell.begin() + 1; *it2 != *it + 2; ++it2 ) {

					gr_left.apply( m_leading(*it2-1), m_super(*it2-1) );
					gr_left.apply( column(right,*it2-1), column(right,*it2) );

					z = gr_left.s() * m_leading(*it2);
					m_leading(*it2) = gr_left.c() * m_leading(*it2);

					givens_rotation_type gr_right( m_leading(*it2-1), z );
					gr_right.apply( m_super(*it2-1), m_leading(*it2) );
					gr_right.apply( row(left,*it2-1), row(left,*it2) );

					if( *it2 == *it + 1 ) break;
					z = gr_right.s() * m_super(*it2);
					m_super(*it2) = gr_right.c() * m_super(*it2);

					gr_left = givens_rotation_type( m_super(*it2-1), z );
				}
			} while( std::abs( last_super - m_super( *it ) ) > lim );
			m_super( *it ) = 0;
		}
	}

	template<class M1, class M2> void apply( M1& left, M2& right, const range& cell ) const {

		if( cell.size() == 1 ) /* Scalar is in diagonal form */
			return ;

		const value_type lim = 6 * std::numeric_limits< value_type >::epsilon() * norm_inf( m_matrix );

		/* Looking for the zero diagonal element */
		for( range::const_reverse_iterator it = cell.rbegin() + 1; it != cell.rend() ; ++it ) {
			if( m_leading( *it ) == 0 ) {
				apply( left, right, range( *it, cell.start() + cell.size() ), left_tag() );
				for( range::const_reverse_iterator it2 = cell.rbegin(); it2 != it; ++it2 ){
					if( std::abs( m_leading( *it2 ) ) < lim )  m_leading( *it2 ) = 0;
				}
				for( range::const_reverse_iterator it2 = cell.rbegin() + 1; it2 != it; ++it2 ){
					if( std::abs( m_super( *it2 ) ) < lim )    m_super( *it2 ) = 0;
				}
				apply( left, right, range( cell.start(), *it+1 ) );
				apply( left, right, range( *it+1, cell.start() + cell.size() ) );
				return ;
			}
		}
		/* Looking for the zero last diagonal element */
		if( m_leading( cell( cell.size() - 1 ) ) == 0 ) {
			apply( left, right, cell, right_tag() );
			for( range::const_reverse_iterator it2 = cell.rbegin(); it2 != cell.rend(); ++it2 ){
				if( std::abs( m_leading( *it2 ) ) < lim )  m_leading( *it2 ) = 0;
			}
			for( range::const_reverse_iterator it2 = cell.rbegin() + 1; it2 != cell.rend(); ++it2 ){
				if( std::abs( m_super( *it2 ) ) < lim )  m_super( *it2 ) = 0;
			}
			apply( left, right, range( cell.start(), cell(cell.size()-1) ) );
			return ;
		}

		/* Looking for the zero upper diagonal element */
		for( range::const_reverse_iterator it = cell.rbegin() + 1; it != cell.rend(); ++it ) {
			if( m_super( *it ) == 0 ) {
				apply( left, right, range( cell.start(), *it + 1 ) );
				apply( left, right, range( *it + 1, cell.start() + cell.size() ) );
				return ;
			}
		}

		apply( left, right, cell, regular_tag() );

	}

public:
/**
 *  @brief An object constructor
 *  @param[in,out] matrix
 *
 */
	qr_decomposition( matrix_type& matrix ):
		m_matrix( matrix ),
		m_super(   matrix, slice(0, 1, matrix.size2() - 1), slice(1, 1, matrix.size2() - 1) ),
		m_leading( matrix, slice(0, 1, matrix.size2()),     slice(0, 1, matrix.size2())     ) {

		assert( matrix.upper() == 1 && matrix.lower() == 0 );
	}

/**
 *  @brief Transformation operaton
 *  @param[out] left  The left matrix
 *  @param[out] right The right matrix
 *
 *  The routine calculates and makes transformation.
 *  Intrinsic assumption is that the all matrix are size-suitable.
 *
 *  \f$ M_{left} := Q M_{left} \f$
 *
 *  \f$ M_{right} := M_{right} H \f$
 *
 *  \f$ M_{matrix} := Q M_{matrix} H \equiv B \f$
 *
 */
	template<class M1, class M2> void apply( M1& left, M2& right ) const {
		apply( left, right, range(0, std::min( m_matrix.size1(), m_matrix.size2() ) ) );
	}

};

/**
 *  @brief A functor for the modified QR decomposition
 *
 *  Modified QR decomposition is iterative alghoritm that transformates
 *  tridiagonal banded symmetric matrix into diagonal form by the number of
 *  Givens transformations. The decomposition is one of Schur decomposition parts.
 *
 */
template<class T> class qr_decomposition<T, tridiag_symmetric_tag> {
public:
	typedef T                                  matrix_type;   //!<
	typedef typename matrix_type::value_type   value_type;    //!<
	typedef typename matrix_type::size_type    size_type;     //!<
	typedef matrix_vector_slice< matrix_type > diagonal_type; //!<
private:
	struct regular_tag {};
	struct left_tag {};
	struct right_tag {};
private:
	matrix_type& m_matrix;
	mutable diagonal_type m_super;
	mutable diagonal_type m_leading;
private:

	template<class M1> void apply( M1& left, const range& cell, const regular_tag& ) const {
		typedef givens_rotation< value_type > givens_rotation_type;

		const value_type lim = std::numeric_limits< value_type >::epsilon() * norm_inf( m_matrix );

		for( range::const_reverse_iterator it = cell.rbegin() + 1; it != cell.rend(); ++it ) {
			value_type last_super; /* use differential convergation control */
			do {
				last_super = m_super( *it );

				value_type e0 = m_leading( cell(0) ) - detail::wilkinson_shift( m_leading(*it), m_super(*it), m_leading(*it+1) );
				value_type z = m_super( cell(0) );

				givens_rotation_type gr_left( e0, z );
				for( range::const_iterator it2 = cell.begin() + 1; *it2 != *it + 2; ++it2 ) {

					value_type t = m_super(*it2-1);
					gr_left.apply( m_leading(*it2-1), t );
					gr_left.apply( m_super(*it2-1), m_leading(*it2) );
					gr_left.apply( m_leading(*it2-1), m_super(*it2-1) );
					gr_left.apply( t, m_leading(*it2) );
					gr_left.apply( row(left,*it2-1), row(left,*it2) );

					if( *it2 == *it + 1 ) break;
					z = gr_left.s() * m_super(*it2);
					m_super(*it2) = gr_left.c() * m_super(*it2);

					gr_left = givens_rotation_type( m_super(*it2-1), z );
				}
			} while( std::abs( last_super - m_super( *it ) ) > lim );
			m_super( *it ) = 0;
		}
	}

	template<class M1> void apply( M1& left, const range& cell ) const {

		if( cell.size() == 1 ) /* Scalar is in diagonal form */
			return ;

		/* Looking for the zero upper diagonal element */
		for( range::const_reverse_iterator it = cell.rbegin() + 1; it != cell.rend(); ++it ) {
			if( m_super( *it ) == 0 ) {
				apply( left, range( cell.start(), *it + 1 ) );
				apply( left, range( *it + 1, cell.start() + cell.size() ) );
				return ;
			}
		}

		apply( left, cell, regular_tag() );

	}

public:
/**
 *  @brief An object constructor
 *  @param[in,out] matrix
 *
 */
	qr_decomposition( matrix_type& matrix ):
		m_matrix( matrix ),
		m_super(   matrix, slice(0, 1, matrix.size2() - 1), slice(1, 1, matrix.size2() - 1) ),
		m_leading( matrix, slice(0, 1, matrix.size2()),     slice(0, 1, matrix.size2())     ) {

		//assert( matrix.upper() == 1 && matrix.lower() == 0 );
	}
/**
 *  @brief Transformation operaton
 *  @param[out] left  The left matrix
 *  @param[out] right The right matrix
 *
 *  The routine calculates and makes transformation.
 *  Intrinsic assumption is that the all matrix are size-suitable.
 *
 *  \f$ M_{left} := Q M_{left} \f$
 *
 *  \f$ M_{matrix} := Q M_{matrix} Q^{T} \equiv T \f$
 *
 */
	template<class M1> void apply( M1& left ) const {
		apply( left, range(0, std::min( m_matrix.size1(), m_matrix.size2() ) ) );
	}

};

/**
 *  @brief A functor for the transformation matrix into the upper triangular form using Householder transformations.
 *
 *  Any matrix can be transformed into the upper triangular form
 *  by the //\f$ 2 \cdot n - 1 \f$ Householder transformations.
 *
 *  \f[
 *  B = Q A \quad \mbox{where} \quad A \quad \mbox{is initial matrix} \quad Q \quad \mbox{are unitary matrix}
 *  \f]
 *
 */
template<class T> class qr_decomposition<T, general_tag> {
public:
	typedef T                                matrix_type; //!< The type of the matrix object to be trasformed
	typedef typename matrix_type::value_type value_type;  //!< The type of the elements stored in the matrix_type
	typedef typename matrix_type::size_type  size_type;   //!< The type for seeking in the matrix object
	typedef vector< value_type >             vector_type; //!< The type of the vector object of matrix elements

	typedef matrix_range< matrix_type > result_type;

private:
	matrix_type& m_matrix;

public:
/**
 *  @brief An object constructor
 *  @param[in,out] matrix The reference to matrix object to be transformed
 *
 *  Actual transformation will be performed as soon as apply(M1& left) will be called.
 *
 */
	qr_decomposition( matrix_type& matrix ):
		m_matrix( matrix ) {
	}

/**
 *  @brief Transformation operaton
 *  @param[out] left  The left matrix
 *
 *  The routine calculates and makes transformation.
 *  Intrinsic assumption is that the all matrix are size-suitable.
 *
 *  \f$ M_{left} := Q M_{left} \f$
 *
 *  \f$ M_{matrix} := Q M_{matrix} \equiv B \f$
 *
 */
        template<class M1> bool apply( M1& left ) const {
		typedef vector< value_type >                 vector_type;
		typedef householder_transform< vector_type > householder_transform_type;

		size_type min_size = std::min( m_matrix.size1(), m_matrix.size2() );

		assert( left.size1() == m_matrix.size1() );

		size_type i = 0;

		for( i = 0; i < min_size; ++i ) {
			householder_transform_type hleft( i+1, i, column(m_matrix,i) );
			hleft.apply( left, row_major_tag() );
			hleft.apply( m_matrix, row_major_tag() );
		}

		value_type lim = matrix_error() * norm_frobenius( m_matrix );
		size_type  rank = min_size;
		for( i = 0; i < min_size; ++i ){
			if( std::abs( m_matrix(i,i) ) < lim ) {
				m_matrix(i,i) = value_type(0); /* overwhelming rounding errors */
				rank--;
			}
		}
		return (rank != min_size);
	}

	template<class M1> void remove( M1& left ) const {
		typedef givens_rotation< value_type > givens_rotation_type;

		size_type min_size = std::min( m_matrix.size1() - 1, m_matrix.size2() );

		assert( left.size1() == m_matrix.size1() );
		for( size_type i = 0; i < min_size; ++i ) {
			if( m_matrix(i+1,i) == value_type(0) ) continue;

			value_type x = m_matrix(i,i);
			value_type y = m_matrix(i+1,i);
			givens_rotation_type gr( x, y );

			gr.apply( row(m_matrix,i), row(m_matrix,i+1) );
			gr.apply( row(left,i), row(left,i+1) );

			m_matrix(i+1,i) = value_type(0); /* overwhelming rounding errors */
		}
	}

	template<class M1> bool insert( M1& left ) const {
		typedef householder_transform< vector_type > householder_transform_type;

		size_type min_size = std::min( m_matrix.size1(), m_matrix.size2() );

		assert( left.size1()  == m_matrix.size1() );
		assert( min_size == m_matrix.size2() );

		householder_transform_type hleft( min_size, min_size - 1, column( m_matrix, min_size - 1 ) );

		if( hleft.s() == value_type(0) ) /* singularity checking */
			return true;

		hleft.apply( left, row_major_tag() );
		hleft.apply( m_matrix, row_major_tag() );

		for( size_type i = min_size; i < m_matrix.size1(); ++i )
			m_matrix(i, min_size - 1) = value_type(0); /* overwhelming rounding errors */

		return false;
	}

	value_type left_error() const {
		size_type min_size = std::min( m_matrix.size1(), m_matrix.size2() );
		return std::abs( ( 4 * m_matrix.size1() + 32 ) * ( 2 * min_size - 1 ) * std::numeric_limits< value_type >::epsilon() );
	}
	value_type matrix_error() const {
		size_type min_size = std::min( m_matrix.size1(), m_matrix.size2() );
		return std::abs( ( 6 * m_matrix.size1() - 3 * min_size + 40 ) * ( 2 * min_size - 1 ) * std::numeric_limits< value_type >::epsilon() );
	}
};

};

#endif // _QR_DECOMPOSITION_H

