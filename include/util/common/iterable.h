#pragma once

#include <functional>
#include <iterator>

#include <util/common/ptr.h>

namespace util
{

    /*****************************************************/
    /*                   data_mapper                     */
    /*****************************************************/

    template
    <
        typename _container_t,
        typename _iterator_t = typename _container_t::const_iterator,
        typename _mapped_t   = typename _iterator_t::value_type
    >
    using data_mapper_t = std::function < _mapped_t (const _container_t &, const _iterator_t & pos, size_t idx) > ;

    template
    <
        typename _iterator_t,
        typename _mapped_t = typename _iterator_t::value_type
    >
    using sequence_mapper_t = std::function < _mapped_t (const _iterator_t & pos, size_t idx) > ;

    template
    <
        typename _source_t,
        typename _mapped_t = typename _source_t
    >
    using value_mapper_t = std::function < _mapped_t (const _source_t & value, size_t idx) > ;

    template
    <
        typename _container_t,
        typename _iterator_t = typename _container_t::const_iterator
    >
    static inline data_mapper_t < _container_t, _iterator_t, typename _iterator_t::reference > make_identity_data_mapper()
    {
        return [] (const _container_t &, const _iterator_t & it, size_t) -> typename _iterator_t::reference
        {
            return *it;
        };
    }

    template
    <
        typename _iterator_t
    >
    static inline sequence_mapper_t < _iterator_t, typename _iterator_t::reference > make_identity_sequence_mapper()
    {
        return [] (const _iterator_t & it, size_t) -> typename _iterator_t::reference
        {
            return *it;
        };
    }

    template
    <
        typename _source_t
    >
    static inline value_mapper_t < _source_t, const _source_t & > make_identity_value_mapper()
    {
        return [] (const _source_t & value, size_t idx) -> const _source_t &
        {
            return value;
        };
    }

    template
    <
        typename _container_t,
        typename _iterator_t,
        typename _mapped_t
    >
    static inline data_mapper_t < _container_t, _iterator_t, _mapped_t > make_data_mapper_from_sequence_mapper(sequence_mapper_t < _iterator_t, _mapped_t > mapper)
    {
        if (!mapper) return {};
        return [mapper] (const _container_t &, const _iterator_t & it, size_t i) -> _mapped_t
        {
            return mapper(it, i);
        };
    }

    template
    <
        typename _container_t,
        typename _iterator_t = typename _container_t::const_iterator,
        typename _source_t,
        typename _mapped_t
    >
    static inline data_mapper_t < _container_t, _iterator_t, _mapped_t > make_data_mapper_from_value_mapper(value_mapper_t < _source_t, _mapped_t > mapper)
    {
        if (!mapper) return {};
        return [mapper] (const _container_t &, const _iterator_t & it, size_t i) -> _mapped_t
        {
            return mapper(*it, i);
        };
    }

    template
    <
        typename _iterator_t,
        typename _source_t,
        typename _mapped_t
    >
    static inline sequence_mapper_t < _iterator_t, _mapped_t > make_sequence_mapper(value_mapper_t < _source_t, _mapped_t > mapper)
    {
        if (!mapper) return {};
        return [mapper] (const _iterator_t & it, size_t i) -> _mapped_t
        {
            return mapper(*it, i);
        };
    }

    template < bool >
    struct _get_mapped_value_helper;

    template < >
    struct _get_mapped_value_helper < false >
    {

        template
        <
            typename _container_t,
            typename _iterator_t,
            typename _mapped_t
        >
        static inline _mapped_t data
        (
            const data_mapper_t < _container_t, _iterator_t, _mapped_t > & mapper,
            const _container_t & c,
            const _iterator_t  & pos,
            size_t               idx
        )
        {
            return mapper(c, pos, idx);
        }

        template
        <
            typename _iterator_t,
            typename _mapped_t
        >
        static inline _mapped_t sequence
        (
            const sequence_mapper_t < _iterator_t, _mapped_t > & mapper,
            const _iterator_t & pos,
            size_t              idx
        )
        {
            return mapper(pos, idx);
        }

        template
        <
            typename _source_t,
            typename _mapped_t
        >
        static inline _mapped_t value
        (
            const value_mapper_t < _source_t, _mapped_t > & mapper,
            const _source_t & src,
            size_t            idx
        )
        {
            return mapper(src, idx);
        }
    };

    template < >
    struct _get_mapped_value_helper < true >
    {

        template
        <
            typename _container_t,
            typename _iterator_t,
            typename _mapped_t
        >
        static inline _mapped_t data
        (
            const data_mapper_t < _container_t, _iterator_t, _mapped_t > & mapper,
            const _container_t & c,
            const _iterator_t  & pos,
            size_t               idx
        )
        {
            return (!mapper) ? *pos : mapper(c, pos, idx);
        }

        template
        <
            typename _iterator_t,
            typename _mapped_t
        >
        static inline _mapped_t sequence
        (
            const sequence_mapper_t < _iterator_t, _mapped_t > & mapper,
            const _iterator_t & pos,
            size_t              idx
        )
        {
            return (!mapper) ? *pos : mapper(pos, idx);
        }

        template
        <
            typename _source_t,
            typename _mapped_t
        >
        static inline _mapped_t value
        (
            const value_mapper_t < _source_t, _mapped_t > & mapper,
            const _source_t & src,
            size_t            idx
        )
        {
            return (!mapper) ? src : mapper(src, idx);
        }
    };

    template
    <
        typename _container_t,
        typename _iterator_t = typename _container_t::const_iterator,
        typename _mapped_t   = typename _iterator_t::value_type
    >
    static inline _mapped_t get_mapped_value_or_default
    (
        const data_mapper_t < _container_t, _iterator_t, _mapped_t > & mapper,
        const _container_t & c,
        const _iterator_t  & pos,
        size_t               idx
    )
    {
        return _get_mapped_value_helper
               < std::is_convertible < typename _iterator_t::reference, _mapped_t > ::value >
               :: data < _container_t, _iterator_t, _mapped_t > (mapper, c, pos, idx);
    }

    template
    <
        typename _iterator_t,
        typename _mapped_t = typename _iterator_t::value_type
    >
    static inline _mapped_t get_mapped_value_or_default
    (
        const sequence_mapper_t < _iterator_t, _mapped_t > & mapper,
        const _iterator_t  & pos,
        size_t               idx
    )
    {
        return _get_mapped_value_helper
               < std::is_convertible < typename _iterator_t::reference, _mapped_t > ::value >
               :: sequence < _iterator_t, _mapped_t > (mapper, pos, idx);
    }

    template
    <
        typename _source_t,
        typename _mapped_t = typename _source_t
    >
    static inline _mapped_t get_mapped_value_or_identity
    (
        const value_mapper_t < _source_t, _mapped_t > & mapper,
        const _source_t  & value,
        size_t             idx
    )
    {
        return _get_mapped_value_helper
               < std::is_convertible < _source_t, _mapped_t > ::value >
               :: value < _source_t, _mapped_t > (mapper, value, idx);
    }

    template
    <
        typename _iterator_t,
        typename _mapped_t = typename _iterator_t::value_type
    >
    static inline bool is_identity_mappable()
    {
        return std::is_convertible < typename _iterator_t::reference, _mapped_t > ::value;
    }

    /*****************************************************/
    /*                 iterator_source                   */
    /*****************************************************/

    template
    <
        typename _container_t,
        typename _iterator_t = typename _container_t::const_iterator
    >
    using iterator_source_t = std::function < _iterator_t (const _container_t &) > ;

    template
    <
        typename _container_t,
        typename _iterator_t = typename _container_t::const_iterator
    >
    static inline iterator_source_t < _container_t, _iterator_t > make_begin_iterator_source()
    {
        return [] (const _container_t & c) -> typename _iterator_t
        {
            return c.begin();
        };
    }

    template
    <
        typename _container_t,
        typename _iterator_t = typename _container_t::const_iterator
    >
    static inline iterator_source_t < _container_t, _iterator_t > make_end_iterator_source()
    {
        return [] (const _container_t & c) -> typename _iterator_t
        {
            return c.end();
        };
    }

    /*****************************************************/
    /*              generic_proxy_iterator               */
    /*****************************************************/

    template
    <
        typename _container_t,
        typename _iterator_t     = typename _container_t::const_iterator,
        typename _value_t        = typename _container_t::value_type,
        typename _pointer_t      = typename const _value_t *,
        typename _reference_t    = typename const _value_t &
    >
    class generic_proxy_iterator
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
        using value_mapper_t      = data_mapper_t < container_type, iterator, reference > ;
        using pointer_mapper_t    = data_mapper_t < container_type, iterator, pointer > ;
        using iterator_source_t   = iterator_source_t < container_type, iterator > ;

    private:

        const container_type     * container;
        iterator                   position;
        size_t                     index;
        const value_mapper_t     * value_mapper;
        const pointer_mapper_t   * pointer_mapper;

    public:

        generic_proxy_iterator()
            : container(nullptr)
            , position()
            , index(0)
            , value_mapper(nullptr)
            , pointer_mapper(nullptr)
        {
        }

        generic_proxy_iterator(
            const generic_proxy_iterator & it)
            : container(it.container)
            , position(it.position)
            , index(it.index)
            , value_mapper(it.value_mapper)
            , pointer_mapper(it.pointer_mapper)
        {
        }

        generic_proxy_iterator(
            const container_type     * container,
            iterator                   position,
            size_t                     index,
            const value_mapper_t     * value_mapper,
            const pointer_mapper_t   * pointer_mapper)
            : container(container)
            , position(position)
            , index(index)
            , value_mapper(value_mapper)
            , pointer_mapper(pointer_mapper)
        {
        }

        generic_proxy_iterator & operator = (
            const generic_proxy_iterator & it)
        {
            this->container = it.container;
            this->position = it.position;
            this->index = it.index;
            this->value_mapper = it.value_mapper;
            this->pointer_mapper = it.pointer_mapper;
        }

        ~generic_proxy_iterator()
        {
        }

    public:

        void swap(generic_proxy_iterator & it)
        {
            const container_type       * c  = this->container;
            iterator                     p  = this->position;
            size_t                       i  = this->index;
            const value_mapper_t       * va = this->value_mapper;
            const pointer_mapper_t     * pa = this->pointer_mapper;

            this->container                 = it.container;
            this->position                  = it.position;
            this->index                     = it.index;
            this->value_mapper              = it.value_mapper;
            this->pointer_mapper            = it.pointer_mapper;

            it.container                    = c;
            it.position                     = p;
            it.index                        = i;
            it.value_mapper                 = va;
            it.pointer_mapper               = pa;
        }

    public:

        bool operator == (const generic_proxy_iterator & it) const
        {
            return ((this->container == it.container) && (this->position == it.position));
        }

        bool operator != (const generic_proxy_iterator & it) const
        {
            return ((this->container != it.container) || (this->position != it.position));
        }

        bool operator == (const _iterator_t & it) const
        {
            return (this->position == it);
        }

        bool operator != (const _iterator_t & it) const
        {
            return (this->position != it);
        }

    public:

        reference operator * () const
        {
            return (*value_mapper)(*this->container, position, index);
        }

        pointer operator -> () const
        {
            return (*pointer_mapper)(*this->container, position, index);
        }

        generic_proxy_iterator & operator ++ ()
        {
            ++this->position;
            ++this->index;
            return *this;
        }

        generic_proxy_iterator operator ++ (int)
        {
            generic_proxy_iterator it = *this;
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
    void swap(generic_proxy_iterator < _container_t, _iterator_t, _value_t, _pointer_t, _reference_t > & it1,
              generic_proxy_iterator < _container_t, _iterator_t, _value_t, _pointer_t, _reference_t > & it2)
    {
        it1.swap(it2);
    }

    template
    <
        typename _container_t,
        typename _iterator_t = typename _container_t::const_iterator,
        typename _value_t    = typename _container_t::value_type
    >
    using mapping_iterator = generic_proxy_iterator
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
        using value_mapper_t      = typename iterator::value_mapper_t;
        using pointer_mapper_t    = typename iterator::pointer_mapper_t;
        using iterator_source_t   = typename iterator::iterator_source_t;
    };

    /*****************************************************/
    /*                 generic_iterable                  */
    /*****************************************************/

    template
    <
        typename _iterator_t
    >
    class generic_iterable
        : public iterable < _iterator_t >
    {

    public:

        using ptr_t = util::ptr_t < generic_iterable > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    public:

        const container_type         * container;
        value_mapper_t                 value_mapper;
        pointer_mapper_t               pointer_mapper;
        iterator_source_t              begin_source;
        iterator_source_t              end_source;

    public:

        generic_iterable()
        {
        }

        generic_iterable
        (
            const container_type       & container,
            iterator_source_t            begin_source,
            iterator_source_t            end_source,
            value_mapper_t               value_mapper,
            pointer_mapper_t             pointer_mapper
        )
            : container(&container)
            , begin_source(std::move(begin_source))
            , end_source(std::move(end_source))
            , value_mapper(std::move(value_mapper))
            , pointer_mapper(std::move(pointer_mapper))
        {
        }

        bool empty() const
        {
            return (begin_source(*container) == end_source(*container));
        }

        const_iterator begin() const
        {
            return const_iterator
            (
                container,
                begin_source(*container),
                0,
                &value_mapper,
                &pointer_mapper
            );
        }

        const_iterator end() const
        {
            return const_iterator
            (
                container,
                end_source(*container),
                SIZE_T_MAX /* undefined for past-the-end iterator */,
                &value_mapper,
                &pointer_mapper
            );
        }

        const_iterator cbegin() const
        {
            return begin();
        }

        const_iterator cend() const
        {
            return end();
        }
    };

    template
    <
        typename _container_t,
        typename _value_t,
        typename _iterator_t = typename _container_t::const_iterator,
        typename _generic_proxy_iterator = generic_proxy_iterator
        <
            _container_t,
            _iterator_t,
            _value_t
        >
    >
    using container_iterable_t = generic_iterable < _generic_proxy_iterator > ;

    template
    <
        typename _container_t,
        typename _value_t,
        typename _iterator_t = typename _container_t::const_iterator,
        typename _generic_proxy_iterator = mapping_iterator
        <
            _container_t,
            _iterator_t,
            _value_t
        >
    >
    using container_mapping_iterable_t = generic_iterable < _generic_proxy_iterator > ;

    /*****************************************************/
    /*                  make_iterable                    */
    /*****************************************************/

    template
    <
        typename _value_t,
        typename _container_t,
        typename _container_iterable_t = container_iterable_t < _container_t, _value_t >,
        typename _iterator_t = typename _container_t::const_iterator
    >
    static inline typename _container_iterable_t::ptr_t make_container_iterable
    (
        const _container_t                                      & container,
        const typename _container_iterable_t::value_mapper_t    & value_mapper,
        const typename _container_iterable_t::iterator_source_t & begin_source = make_begin_iterator_source < _container_t, _iterator_t > (),
        const typename _container_iterable_t::iterator_source_t & end_source   = make_end_iterator_source < _container_t, _iterator_t > ()
    )
    {
        return _container_iterable_t::create
        (
            container,
            begin_source,
            end_source,
            value_mapper,
            [value_mapper] (const _container_t & c, const _iterator_t & pos, size_t idx)
            {
                return &value_mapper(c, pos, idx);
            }
        );
    }

    template
    <
        typename _value_t,
        typename _container_t,
        typename _container_iterable_t = container_mapping_iterable_t < _container_t, _value_t >,
        typename _iterator_t = typename _container_t::const_iterator
    >
    static inline typename _container_iterable_t::ptr_t make_container_mapping_iterable
    (
        const _container_t                                      & container,
        const typename _container_iterable_t::value_mapper_t    & value_mapper,
        const typename _container_iterable_t::iterator_source_t & begin_source = make_begin_iterator_source < _container_t, _iterator_t > (),
        const typename _container_iterable_t::iterator_source_t & end_source   = make_end_iterator_source < _container_t, _iterator_t > ()
    )
    {
        return _container_iterable_t::create
        (
            container,
            begin_source,
            end_source,
            value_mapper,
            [value_mapper] (const _container_t & c, const _iterator_t & pos, size_t idx)
            {
                return util::create < _value_t > (value_mapper(c, pos, idx));
            }
        );
    }
}
