
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014, 2015 - 2016 Axel Menzel <info@rttr.org>                     *
*                                                                                   *
*   This file is part of RTTR (Run Time Type Reflection)                            *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining           *
*   a copy of this software and associated documentation files (the "Software"),    *
*   to deal in the Software without restriction, including without limitation       *
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,        *
*   and/or sell copies of the Software, and to permit persons to whom the           *
*   Software is furnished to do so, subject to the following conditions:            *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifndef RTTR_ARRAY_RANGE_H_
#define RTTR_ARRAY_RANGE_H_

#include "rttr/detail/base/core_prerequisites.h"
#include <vector>
#include <cstddef>

namespace rttr
{
class property;
class method;
class constructor;
class enumeration;
class type;
class parameter_info;

template<typename T, typename Predicate>
class array_range;

namespace detail
{
    struct no_predicate {};

    template<typename T>
    struct default_predicate
    {
        using predicate_func = bool(*)(const T&);

        RTTR_FORCE_INLINE default_predicate() : m_func([](const T&){ return true; }) {}
        RTTR_FORCE_INLINE default_predicate(const predicate_func& func) : m_func(func) {}
        RTTR_FORCE_INLINE bool operator()(const T& obj) const  { return m_func(obj); }

        predicate_func m_func;
    };

    template<typename T>
    array_range<T, detail::no_predicate> create_array_range(T* const begin, T* const end);

    template<typename T, typename Predicate = no_predicate>
    array_range<T, Predicate> create_array_range();

    template<typename T, typename Predicate = default_predicate<T>>
    array_range<T, Predicate> create_array_range_with_predicate(T* const begin, T* const end, const Predicate& pred = Predicate());

} // end namespace detail


/*!
 * The \ref array_range class provides a view into an underlying data structure with lower and upper limits.
 * In order to iterate over the data structure use the returned forward iterators from begin() or rbegin().
 * To check whether the end of the range has reached use end() or rend().
 * Using the standard C++ range-based for loop will implicit do all this work.
 *
 * \remark When the underlying data structure gets modified while holding a range,
 *         its iterators and the range itself will be invalidated.
 */

template<typename T, typename Predicate = detail::no_predicate>
class array_range
{
    using value_type = T;
    using bounds_type = T*;

public:

#ifndef DOXYGEN
    /*!
     * The base class for all item forward iterators.
     */
    template<typename DataType>
    class array_iterator_base
    {
        public:
            using self_type = array_iterator_base<DataType>;
            using value_type = DataType;
            using reference = DataType&;
            using pointer = DataType*;
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;

            reference operator*();
            pointer operator->();
            bool operator==(const self_type& rhs) const;
            bool operator!=(const self_type& rhs) const;

            array_iterator_base& operator=(const self_type& other);

        protected:
            friend class array_range<T, Predicate>;
            array_iterator_base();
            array_iterator_base(pointer ptr, const array_range<T, Predicate>* const range);

            pointer m_ptr;
            const array_range<T, Predicate>* m_range;
    };

    /*!
     * A forward iterator for items of type \p DataType.
     */
    template<typename DataType>
    class array_iterator : public array_iterator_base<DataType>
    {
        public:
            using self_type = array_iterator<DataType>;

            array_iterator();
            array_iterator(const array_iterator<DataType>& other);


            self_type& operator++();
            self_type operator++(int index);

        private:
            array_iterator(typename array_iterator_base<DataType>::pointer ptr,
                           const array_range<T, Predicate>* const range);
            friend class array_range<T, Predicate>;
    };

    /*!
     * A forward reverse iterator for items of type \p DataType.
     */
    template<typename DataType>
    class array_reverse_iterator : public array_iterator_base<DataType>
    {
        public:
            using self_type = array_reverse_iterator<DataType>;

            array_reverse_iterator();
            array_reverse_iterator(const array_reverse_iterator<DataType>& other);

            self_type& operator++();
            self_type operator++(int index);

        private:
            array_reverse_iterator(typename array_iterator_base<DataType>::pointer ptr,
                                   const array_range<T, Predicate>* const range);
            friend class array_range<T, Predicate>;
    };
#endif

    //! A forward iterator.
    using iterator = array_iterator<T>;
    //! A constant forward iterator.
    using const_iterator = array_iterator<const T>;

    //! A forward iterator that reverses the direction.
    using reverse_iterator = array_reverse_iterator<T>;
    //! A constant forward iterator that reverses the direction.
    using const_reverse_iterator = array_reverse_iterator<const T>;

    /*!
     * \brief Returns an iterator to the first element of the range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref end().
     *
     * \return Iterator to the first element.
     */
    iterator begin();

    /*!
     * \brief Returns an iterator to the element following the last element of the range.
     *
     * \remark This element acts as placeholder, attempting to access it results in **undefined behavior**.
     *         If the range is empty, the returned iterator will be equal to \ref begin().
     *
     * \return Iterator to the element following the last element.
     */
    iterator end();

    /*!
     * \brief Returns a constant iterator to the first element of the range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref end().
     *
     * \return Constant iterator to the first element.
     */
    const_iterator begin() const;

    /*!
     * \brief Returns a constant iterator to the element following the last element of the range.
     *
     * \remark This element acts as placeholder, attempting to access it results in **undefined behavior**.
     *         If the range is empty, the returned iterator will be equal to \ref begin().
     *
     * \return Constant iterator to the element following the last element.
     */
    const_iterator end() const;

     /*!
     * \brief Returns a constant iterator to the first element of the  range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref end().
     *
     * \return Constant iterator to the first element.
     */
    const_iterator cbegin() const;

    /*!
     * \brief Returns a constant iterator to the element following the last element of the range.
     *
     * \remark This element acts as placeholder, attempting to access it results in **undefined behavior**.
     *         If the range is empty, the returned iterator will be equal to \ref begin().
     *
     * \return Constant iterator to the element following the last element.
     */
    const_iterator cend() const;

    /*!
     * \brief Returns a reverse iterator to the first element of the reversed range.
     *        It corresponds to the last element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rend().
     *
     * \return Reverse iterator to the first element.
     */
    reverse_iterator rbegin();

    /*!
     * \brief Returns a reverse iterator to the element following the last element of the reversed range.
     *        It corresponds to the element preceding the first element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rbegin().
     *
     * \return Reverse iterator to the element following the last element.
     */
    reverse_iterator rend();

    /*!
     * \brief Returns a constant reverse iterator to the first element of the reversed range.
     *        It corresponds to the last element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rend().
     *
     * \return Constant reverse iterator to the first element.
     */
    const_reverse_iterator rbegin() const;

    /*!
     * \brief Returns a constant reverse iterator to the element following the last element of the reversed range.
     *        It corresponds to the element preceding the first element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rbegin().
     *
     * \return Constant reverse iterator to the element following the last element.
     */
    const_reverse_iterator rend() const;

    /*!
     * \brief Returns a constant reverse iterator to the first element of the reversed range.
     *        It corresponds to the last element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rend().
     *
     * \return Constant reverse iterator to the first element.
     */
    const_reverse_iterator crbegin() const;

    /*!
     * \brief Returns a constant reverse iterator to the element following the last element of the reversed range.
     *        It corresponds to the element preceding the first element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rbegin().
     *
     * \return Constant reverse iterator to the element following the last element.
     */
    const_reverse_iterator crend() const;

    /*!
     * \brief Returns the number of elements in the range.
     *
     * \return The number of elements in the range.
     */
    size_t size() const;

    /*!
     * \brief Checks if the range has no elements, i.e. whether `begin() == end()`.
     *
     * \return `True` if this range is empty, otherwise `false`.
     */
    bool empty() const;

private:

    array_range();
    array_range(bounds_type begin, bounds_type end, const Predicate& pred);

    template<typename DataType>
    void next(array_iterator<DataType>& itr) const;

    template<typename DataType>
    void prev(array_reverse_iterator<DataType>& itr) const;

    template<typename U, typename P>
    friend array_range<U, P> detail::create_array_range();
    template<typename U, typename P>
    friend array_range<U, P> detail::create_array_range_with_predicate(U* const begin, U* const end, const P& pred);

private:
    bounds_type const   m_begin;
    bounds_type const   m_end;
    const Predicate     m_pred;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN

template<typename T>
class array_range<T, detail::no_predicate>
{
    using value_type = T;
    using bounds_type = T*;

public:

#ifndef DOXYGEN
    /*!
     * The base class for all item forward iterators.
     */
    template<typename DataType>
    class array_iterator_base
    {
        public:
            using self_type = array_iterator_base<DataType>;
            using value_type = DataType;
            using reference = DataType&;
            using pointer = DataType*;
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;

            reference operator*();
            pointer operator->();
            bool operator==(const self_type& rhs) const;
            bool operator!=(const self_type& rhs) const;

            array_iterator_base& operator=(const self_type& other);

        protected:
            friend class array_range<T>;
            array_iterator_base();
            array_iterator_base(pointer ptr);

            pointer m_ptr;
    };

    /*!
     * A forward iterator for items of type \p DataType.
     */
    template<typename DataType>
    class array_iterator : public array_iterator_base<DataType>
    {
        public:
            using self_type = array_iterator<DataType>;

            array_iterator();
            array_iterator(const array_iterator<DataType>& other);


            self_type& operator++();
            self_type operator++(int index);

        private:
            array_iterator(typename array_iterator_base<DataType>::pointer ptr);
            friend class array_range<T>;
    };

    /*!
     * A forward reverse iterator for items of type \p DataType.
     */
    template<typename DataType>
    class array_reverse_iterator : public array_iterator_base<DataType>
    {
        public:
            using self_type = array_reverse_iterator<DataType>;

            array_reverse_iterator();
            array_reverse_iterator(const array_reverse_iterator<DataType>& other);

            self_type& operator++();
            self_type operator++(int index);

        private:
            array_reverse_iterator(typename array_iterator_base<DataType>::pointer ptr);
            friend class array_range<T>;
    };
#endif

    //! A forward iterator.
    using iterator = array_iterator<T>;
    //! A constant forward iterator.
    using const_iterator = array_iterator<const T>;

    //! A forward iterator that reverses the direction.
    using reverse_iterator = array_reverse_iterator<T>;
    //! A constant forward iterator that reverses the direction.
    using const_reverse_iterator = array_reverse_iterator<const T>;

    /*!
     * \brief Returns an iterator to the first element of the range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref end().
     *
     * \return Iterator to the first element.
     */
    iterator begin();

    /*!
     * \brief Returns an iterator to the element following the last element of the range.
     *
     * \remark This element acts as placeholder, attempting to access it results in **undefined behavior**.
     *         If the range is empty, the returned iterator will be equal to \ref begin().
     *
     * \return Iterator to the element following the last element.
     */
    iterator end();

    /*!
     * \brief Returns a constant iterator to the first element of the range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref end().
     *
     * \return Constant iterator to the first element.
     */
    const_iterator begin() const;

    /*!
     * \brief Returns a constant iterator to the element following the last element of the range.
     *
     * \remark This element acts as placeholder, attempting to access it results in **undefined behavior**.
     *         If the range is empty, the returned iterator will be equal to \ref begin().
     *
     * \return Constant iterator to the element following the last element.
     */
    const_iterator end() const;

     /*!
     * \brief Returns a constant iterator to the first element of the  range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref end().
     *
     * \return Constant iterator to the first element.
     */
    const_iterator cbegin() const;

    /*!
     * \brief Returns a constant iterator to the element following the last element of the range.
     *
     * \remark This element acts as placeholder, attempting to access it results in **undefined behavior**.
     *         If the range is empty, the returned iterator will be equal to \ref begin().
     *
     * \return Constant iterator to the element following the last element.
     */
    const_iterator cend() const;

    /*!
     * \brief Returns a reverse iterator to the first element of the reversed range.
     *        It corresponds to the last element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rend().
     *
     * \return Reverse iterator to the first element.
     */
    reverse_iterator rbegin();

    /*!
     * \brief Returns a reverse iterator to the element following the last element of the reversed range.
     *        It corresponds to the element preceding the first element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rbegin().
     *
     * \return Reverse iterator to the element following the last element.
     */
    reverse_iterator rend();

    /*!
     * \brief Returns a constant reverse iterator to the first element of the reversed range.
     *        It corresponds to the last element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rend().
     *
     * \return Constant reverse iterator to the first element.
     */
    const_reverse_iterator rbegin() const;

    /*!
     * \brief Returns a constant reverse iterator to the element following the last element of the reversed range.
     *        It corresponds to the element preceding the first element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rbegin().
     *
     * \return Constant reverse iterator to the element following the last element.
     */
    const_reverse_iterator rend() const;

    /*!
     * \brief Returns a constant reverse iterator to the first element of the reversed range.
     *        It corresponds to the last element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rend().
     *
     * \return Constant reverse iterator to the first element.
     */
    const_reverse_iterator crbegin() const;

    /*!
     * \brief Returns a constant reverse iterator to the element following the last element of the reversed range.
     *        It corresponds to the element preceding the first element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rbegin().
     *
     * \return Constant reverse iterator to the element following the last element.
     */
    const_reverse_iterator crend() const;

    /*!
     * \brief Returns the number of elements in the range.
     *
     * \return The number of elements in the range.
     */
    size_t size() const;

    /*!
     * \brief Checks if the range has no elements, i.e. whether `begin() == end()`.
     *
     * \return `True` if this range is empty, otherwise `false`.
     */
    bool empty() const;

private:

    array_range();
    array_range(bounds_type begin, bounds_type end);

    template<typename U>
    friend array_range<U> detail::create_array_range(U* const, U* const);
    template<typename U, typename P>
    friend array_range<U, P> detail::create_array_range();

private:
    bounds_type const   m_begin;
    bounds_type const   m_end;
};
#endif


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief A range of properties.
 *
 */
using property_range = array_range<property>;

/*!
 * \brief A range of methods.
 *
 */
using method_range = array_range<method>;

/*!
 * \brief A range of constructors.
 *
 */
using constructor_range = array_range<constructor>;

/*!
 * \brief A range of enumerations.
 *
 */
using enumeration_range = array_range<enumeration>;

/*!
 * \brief A range of types.
 *
 */
using type_range = array_range<type>;

/*!
 * \brief A range of parameter infos.
 *
 */
using parameter_info_range = array_range<parameter_info>;


} // end namespace rttr

#include "rttr/detail/impl/array_range_impl.h"

#endif // RTTR_ARRAY_RANGE_H_