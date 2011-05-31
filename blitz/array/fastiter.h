// -*- C++ -*-
/***************************************************************************
 * blitz/array/fastiter.h  Declaration of FastArrayIterator<P_numtype,N_rank>
 *
 * $Id$
 *
 * Copyright (C) 1997-2011 Todd Veldhuizen <tveldhui@acm.org>
 *
 * This file is a part of Blitz.
 *
 * Blitz is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Blitz is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with Blitz.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Suggestions:          blitz-devel@lists.sourceforge.net
 * Bugs:                 blitz-support@lists.sourceforge.net    
 *
 * For more information, please see the Blitz++ Home Page:
 *    https://sourceforge.net/projects/blitz/
 *
 ****************************************************************************/
#ifndef BZ_ARRAY_FASTITER_H
#define BZ_ARRAY_FASTITER_H

#ifdef BZ_HAVE_STD
 #include <sstream>
#else
 #include <strstream.h>
#endif

BZ_NAMESPACE(blitz)

#ifndef BZ_ARRAY_H
 #error <blitz/array/iter.h> must be included via <blitz/array.h>
#endif

#include <blitz/array/slice.h>
#include <blitz/constpointerstack.h>

// Wrapper to turn expressions with FAIs to FACIs so they can be
// returned from a function.
template<typename T>
typename T::T_range_result safeToReturn(const T& expr) {
  return expr(expr.domain());
}


// // helper class ConstPointerStack
// template<typename P_numtype, int N_rank>
// class ConstPointerStack {
// public:
//     typedef P_numtype                T_numtype;

//     void operator=(const ConstPointerStack<P_numtype,N_rank>& rhs) 
//     {
//         for (int i=0; i<N_rank; ++i)
//             stack_[i] = rhs.stack_[i];
//     }

//     const T_numtype*& operator[](int position)
//     {
//         return stack_[position];
//     }
      
// private:
//     const T_numtype *                stack_[N_rank];
// };


// forward declaration
template<typename, int> class FastArrayIterator;
template<typename, int> class FastArrayCopyIterator;


template<typename P_numtype, int N_rank, typename P_arraytype>
class FastArrayIteratorBase {
public:
    typedef P_numtype                T_numtype;
    typedef Array<T_numtype, N_rank> T_array;
  typedef FastArrayIteratorBase<P_numtype, N_rank, P_arraytype> T_iterator;
    typedef const T_array& T_ctorArg1;
    typedef int            T_ctorArg2;    // dummy
  typedef FastArrayCopyIterator<P_numtype, N_rank> T_range_result;

    static const int 
        numArrayOperands = 1, 
        numIndexPlaceholders = 0,
        rank_ = N_rank;

    // NB: this ctor does NOT preserve stack and stride
    // parameters.  This is for speed purposes.
    FastArrayIteratorBase(const T_iterator& x)
        : data_(x.data_), array_(x.array_)
    { }

    void operator=(const T_iterator& x)
    {
      // doesn't this copy the data in x.array_ and then make data_
      // point to x's array? doesn't seem right
        array_ = x.array_;
        data_ = x.data_;
        stack_ = x.stack_;
        stride_ = x.stride_;
    }

    FastArrayIteratorBase(const T_array& array)
        : array_(array)
    {
        data_   = array.data();
    }

    ~FastArrayIteratorBase()
    { }

#ifdef BZ_ARRAY_EXPR_PASS_INDEX_BY_VALUE
    T_numtype operator()(TinyVector<int, N_rank> i) const
    { return array_(i); }
#else
    T_numtype operator()(const TinyVector<int, N_rank>& i) const
    { return array_(i); }
#endif

    int ascending(const int rank) const
    {
        if (rank < N_rank)
            return array_.isRankStoredAscending(rank);
        else
            return INT_MIN;   // tiny(int());
    }

    int ordering(const int rank) const
    {
        if (rank < N_rank)
            return array_.ordering(rank);
        else
            return INT_MIN;   // tiny(int());
    }

    int lbound(const int rank) const
    { 
        if (rank < N_rank)
            return array_.lbound(rank); 
        else
            return INT_MIN;   // tiny(int());
    }

    int ubound(const int rank) const
    { 
        if (rank < N_rank)
            return array_.ubound(rank); 
        else
            return INT_MAX;   // huge(int());
    }
    
    RectDomain<rank_> domain() const { return array_.domain(); };

    T_numtype first_value() const { return *data_; }

    T_numtype operator*() const
    { return *data_; }

    T_numtype operator[](int i) const
    { return data_[i * stride_]; }

    T_numtype fastRead(sizeType i) const
    { return data_[i]; }

    int suggestStride(int rank) const
    { return array_.stride(rank); }

    bool isStride(int rank, diffType stride) const
    { return array_.stride(rank) == stride; }

    void push(int position)
    {
        stack_[position] = data_;
    }
  
    void pop(int position)
    { 
        data_ = stack_[position];
    }

    void advance()
    {
      data_ += stride_;
    }

    void advance(int n)
    {
      data_ += n * stride_;
    }

    void loadStride(int rank)
    {
        stride_ = array_.stride(rank);
    }

    const T_numtype * restrict data() const
    { return data_; }

  const Array<T_numtype, N_rank>& array() const
  { return array_; }

    void _bz_setData(const T_numtype* ptr)
    { data_ = ptr; }

    // this is needed for the stencil expression fastRead to work
    void _bz_offsetData(sizeType i)
    { data_ += i;}

    // and these are needed for stencil expression shift to work
    void _bz_offsetData(sizeType offset, int dim)
    { data_ += offset*array_.stride(dim); }
  
    void _bz_offsetData(sizeType offset1, int dim1, sizeType offset2, int dim2)
    { data_ += offset1*array_.stride(dim1); 
      data_ += offset2*array_.stride(dim2); }

    int stride() const
    { return stride_; }

    bool isUnitStride(int rank) const
    { return array_.stride(rank) == 1; }

    void advanceUnitStride()
    { ++data_; }

    bool canCollapse(int outerLoopRank, int innerLoopRank) const
    { return array_.canCollapse(outerLoopRank, innerLoopRank); }

    void prettyPrint(BZ_STD_SCOPE(string) &str, 
        prettyPrintFormat& format) const
    {
        if (format.tersePrintingSelected())
            str += format.nextArrayOperandSymbol();
        else if (format.dumpArrayShapesMode())
        {
#ifdef BZ_HAVE_STD
	    BZ_STD_SCOPE(ostringstream) ostr;
#else
            ostrstream ostr;
#endif
            ostr << array_.shape();
            str += ostr.str();
        }
        else {
            str += "Array<";
            str += BZ_DEBUG_TEMPLATE_AS_STRING_LITERAL(T_numtype);
            str += ",";

            char tmpBuf[10];
            sprintf(tmpBuf, "%d", N_rank);

            str += tmpBuf;
            str += ">";
        }
    }

    template<typename T_shape>
    bool shapeCheck(const T_shape& shape) const
    { return areShapesConformable(shape, array_.length()); }


    // Experimental
    T_numtype& operator()(int i) const
    {
      return (T_numtype&)data_[i*array_.stride(0)];
    }

    // Experimental
    T_numtype& operator()(int i, int j) const
    {
      return (T_numtype&)data_[i*array_.stride(0) + j*array_.stride(1)];
    }

    // Experimental
    T_numtype& operator()(int i, int j, int k) const
    {
      return (T_numtype&)data_[i*array_.stride(0)
			       + j*array_.stride(1)
			       + k*array_.stride(2)];
    }

    // Experimental

    void moveTo(int i)
    {
        data_ = &const_cast<T_array&>(array_)(i);
    }

    void moveTo(int i, int j)
    {
        data_ = &const_cast<T_array&>(array_)(i,j);
    }

    void moveTo(int i, int j, int k)
    {
        data_ = &const_cast<T_array&>(array_)(i,j,k);
    }

    template<int N_rank2>
    void moveTo(const TinyVector<int,N_rank2>& i)
    {
        data_ = &const_cast<T_array&>(array_)(i);
    }

    // Experimental
    void operator=(T_numtype x)
    {   *const_cast<T_numtype*>(data_) = x; }

    // Experimental
    template<typename T_value>
    void operator=(T_value x)
    {   *const_cast<T_numtype*>(data_) = x; }

    // Experimental
    template<typename T_value>
    void operator+=(T_value x)
    { *const_cast<T_numtype*>(data_) += x; }

    // NEEDS_WORK: other operators
  
    // Experimental
    operator T_numtype() const
    { return *data_; }

    // Experimental
    T_numtype shift(int offset, int dim) const
    {
      return data_[offset*array_.stride(dim)];
    }

    // Experimental
    T_numtype shift(int offset1, int dim1, int offset2, int dim2) const
    {
      return data_[offset1*array_.stride(dim1) 
		   + offset2*array_.stride(dim2)];
    }

  // sliceinfo for expressions
  template<typename T1, typename T2 = nilArraySection, 
	   class T3 = nilArraySection, typename T4 = nilArraySection, 
	   class T5 = nilArraySection, typename T6 = nilArraySection, 
	   class T7 = nilArraySection, typename T8 = nilArraySection, 
	   class T9 = nilArraySection, typename T10 = nilArraySection, 
	   class T11 = nilArraySection>
  class SliceInfo {
  public:    
    typedef FastArrayCopyIterator<T_numtype, blitz::SliceInfo<T_numtype, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::rank> T_slice;
};

protected:
  const T_numtype * restrict           data_;
  P_arraytype                          array_;
  ConstPointerStack<T_numtype,N_rank>  stack_;
  diffType                             stride_;
};


template<typename P_numtype, int N_rank> class FastArrayCopyIterator;

template<typename P_numtype, int N_rank>
class FastArrayIterator : 
  public FastArrayIteratorBase<P_numtype, N_rank, const Array<P_numtype, N_rank>&> 
{
public:
  typedef FastArrayIteratorBase<P_numtype, N_rank, 
				const Array<P_numtype, N_rank>&> T_base;
  typedef typename T_base::T_numtype T_numtype;
  typedef typename T_base::T_array T_array;
  typedef typename T_base::T_iterator T_iterator;
  typedef typename T_base::T_ctorArg1 T_ctorArg1;
  typedef typename T_base::T_ctorArg2 T_ctorArg2;
  typedef typename T_base::T_range_result T_range_result;
  
  using T_base::rank;
  using T_base::numArrayOperands;
  using T_base::numIndexPlaceholders;

  // NB: this ctor does NOT preserve stack and stride
  // parameters.  This is for speed purposes.
  FastArrayIterator(const FastArrayIterator<P_numtype, N_rank>& x) 
    : T_base(x)
  { }

  FastArrayIterator(const T_array& array) : T_base(array) {}
  
  using T_base::operator=;
  void operator=(const FastArrayIterator<P_numtype, N_rank>& x)
  {
    T_base::operator=(x);
  }
  
  using T_base::operator();

  template<int N>
  T_range_result operator()(const RectDomain<N>& d) const
  {
    return T_range_result(T_base::array_(d));
  }

  template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
	   typename T7, typename T8, typename T9, typename T10, typename T11>
  FastArrayCopyIterator<T_numtype, T_base::template SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice::rank>
  operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6, T7 r7, T8 r8, T9 r9, T10 r10, T11 r11) const
  {
    typedef FastArrayCopyIterator<T_numtype, T_base::template SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice::rank> slice;

    return slice(array_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11));
  }
  
};

/* This version of the FastArrayIterator makes a COPY of the array
   it's pointing to. This makes it possible to return expressions of
   arrays that have gone out of scope, or to slice expressions. */
template<typename P_numtype, int N_rank>
class FastArrayCopyIterator : 
  public FastArrayIteratorBase<P_numtype, N_rank, const Array<P_numtype, N_rank> >
{
public:
  typedef FastArrayIteratorBase<P_numtype, N_rank, 
				const Array<P_numtype, N_rank> > T_base;
  typedef typename T_base::T_numtype T_numtype;
  typedef typename T_base::T_array T_array;
  typedef typename T_base::T_iterator T_iterator;
  typedef typename T_base::T_ctorArg1 T_ctorArg1;
  typedef typename T_base::T_ctorArg2 T_ctorArg2;
  typedef typename T_base::T_range_result T_range_result;

  using T_base::rank;
  using T_base::numArrayOperands;
  using T_base::numIndexPlaceholders;


  // NB: this ctor does NOT preserve stack and stride
  // parameters.  This is for speed purposes.
  FastArrayCopyIterator(const FastArrayCopyIterator<P_numtype, N_rank>& x) 
    : T_base(x)
  { }

  FastArrayCopyIterator(const T_array& array) : T_base(array) { }
  
  using T_base::operator=;
  void operator=(const FastArrayCopyIterator& x)
  {
    T_base::operator=(x);
  }
  
  using T_base::operator();

  template<int N>
  T_range_result operator()(const RectDomain<N>& d) const
  {
    return T_range_result(T_base::array_(d));
  }

  template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
	   typename T7, typename T8, typename T9, typename T10, typename T11>
  FastArrayCopyIterator<T_numtype, T_base::template SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice::rank>
  operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6, T7 r7, T8 r8, T9 r9, T10 r10, T11 r11) const
  {
    typedef FastArrayCopyIterator<T_numtype, T_base::template SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice::rank> slice;

    return slice(array_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11));
  }
};


BZ_NAMESPACE_END

#endif // BZ_ARRAY_FASTITER_H
