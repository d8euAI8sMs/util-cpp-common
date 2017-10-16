#pragma once

#include <functional>
#include <iterator>

#include <util/common/ptr.h>

namespace util
{

    /*****************************************************/
    /*              forward_const_iterator               */
    /*****************************************************/

    template
    <
        typename _container_t,
        typename _iterator_t     = typename _container_t::const_iterator,
        typename _value_t        = typename _container_t::value_type,
        typename _pointer_t      = typename const _value_t *,
        typename _reference_t    = typename const _value_t &
    >
    class forward_const_iterator
        : public std::iterator
        <
            std::input_iterator_tag,
            _value_t,
            std::ptrdiff_t,
            _pointer_t,
            _reference_t
        >
    {

    public:

        using container_type      = _container_t;
        using iterator            = _iterator_t;
        using value_accessor_t    = std::function < reference (const container_type &, const iterator &, size_t) > ;
        using pointer_accessor_t  = std::function < pointer (const container_type &, const iterator &, size_t) > ;
        using iterator_accessor_t = std::function < iterator (const container_type &) > ;

    private:

        const container_type     * container;
        iterator                   position;
        size_t                     index;
        const value_accessor_t   * value_accessor;
        const pointer_accessor_t * pointer_accessor;

    public:

        forward_const_iterator()
            : container(nullptr)
            , position()
            , index(0)
            , value_accessor(nullptr)
            , pointer_accessor(nullptr)
        {
        }

        forward_const_iterator(
            const forward_const_iterator & it)
            : container(it.container)
            , position(it.position)
            , index(it.index)
            , value_accessor(it.value_accessor)
            , pointer_accessor(it.pointer_accessor)
        {
        }

        forward_const_iterator(
            const container_type     * container,
            iterator                   position,
            size_t                     index,
            const value_accessor_t   * value_accessor,
            const pointer_accessor_t * pointer_accessor)
            : container(container)
            , position(position)
            , index(index)
            , value_accessor(value_accessor)
            , pointer_accessor(pointer_accessor)
        {
        }

        forward_const_iterator & operator = (
            const forward_const_iterator & it)
        {
            this->container = it.container;
            this->position = it.position;
            this->index = it.index;
            this->value_accessor = it.value_accessor;
            this->pointer_accessor = it.pointer_accessor;
        }

        ~forward_const_iterator()
        {
        }

    public:

        void swap(forward_const_iterator & it)
        {
            const container_type       * c  = this->container;
            iterator                     p  = this->position;
            size_t                       i  = this->index;
            const value_accessor_t     * va = this->value_accessor;
            const pointer_accessor_t   * pa = this->pointer_accessor;

            this->container                 = it.container;
            this->position                  = it.position;
            this->index                     = it.index;
            this->value_accessor            = it.value_accessor;
            this->pointer_accessor          = it.pointer_accessor;

            it.container                    = c;
            it.position                     = p;
            it.index                        = i;
            it.value_accessor               = va;
            it.pointer_accessor             = pa;
        }

    public:

        bool operator == (const forward_const_iterator & it) const
        {
            return ((this->container == it.container) && (this->position == it.position));
        }

        bool operator != (const forward_const_iterator & it) const
        {
            return ((this->container != it.container) || (this->position != it.position));
        }

    public:

        reference operator * () const
        {
            return (*value_accessor)(*this->container, position, index);
        }

        pointer operator -> () const
        {
            return (*pointer_accessor)(*this->container, position, index);
        }

        forward_const_iterator & operator ++ ()
        {
            ++this->position;
            ++this->index;
            return *this;
        }

        forward_const_iterator operator ++ (int)
        {
            forward_const_iterator it = *this;
            ++this->position;
            ++this->index;
            return it;
        }
    };

    template
    <
        typename _container_t,
        typename _iterator_t,
        typename _value_t,
        typename _pointer_t,
        typename _reference_t
    >
    void swap(forward_const_iterator < _container_t, _iterator_t, _value_t, _pointer_t, _reference_t > & it1,
              forward_const_iterator < _container_t, _iterator_t, _value_t, _pointer_t, _reference_t > & it2)
    {
        it1.swap(it2);
    }

    template
    <
        typename _container_t,
        typename _iterator_t = typename _container_t::const_iterator,
        typename _value_t    = typename _container_t::value_type
    >
    using mapping_iterator = forward_const_iterator
    <
        _container_t, _iterator_t, _value_t, util::ptr_t < _value_t >, _value_t
    > ;

    /*****************************************************/
    /*                   iterable                        */
    /*****************************************************/

    template
    <
        typename _iterator_t
    >
    class iterable
    {

    public:

        using iterator            = _iterator_t;
        using const_iterator      = _iterator_t;
        using container_type      = typename iterator::container_type;
        using value_type          = typename iterator::value_type;
        using value_accessor_t    = typename iterator::value_accessor_t;
        using pointer_accessor_t  = typename iterator::pointer_accessor_t;
        using iterator_accessor_t = typename iterator::iterator_accessor_t;
    };

    /*****************************************************/
    /*             forward_iterator_iterable             */
    /*****************************************************/

    template
    <
        typename _iterator_t
    >
    class forward_iterable
        : public iterable < _iterator_t >
    {

    public:

        using ptr_t = util::ptr_t < forward_iterable > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    public:

        util::ptr_t < container_type > container;
        value_accessor_t               value_accessor;
        pointer_accessor_t             pointer_accessor;
        iterator_accessor_t            begin_accessor;
        iterator_accessor_t            end_accessor;

    public:

        forward_iterable()
        {
        }

        forward_iterable
        (
            util::ptr_t < container_type > container,
            iterator_accessor_t            begin_accessor,
            iterator_accessor_t            end_accessor,
            value_accessor_t               value_accessor,
            pointer_accessor_t             pointer_accessor
        )
            : container(std::move(container))
            , begin_accessor(std::move(begin_accessor))
            , end_accessor(std::move(end_accessor))
            , value_accessor(std::move(value_accessor))
            , pointer_accessor(std::move(pointer_accessor))
        {
        }

        const_iterator begin() const
        {
            return const_iterator
            (
                container.get(),
                begin_accessor(*container),
                0,
                &value_accessor,
                &pointer_accessor
            );
        }

        const_iterator end() const
        {
            return const_iterator
            (
                container.get(),
                end_accessor(*container),
                SIZE_T_MAX /* undefined for past-the-end iterator */,
                &value_accessor,
                &pointer_accessor
            );
        }
    };
}
