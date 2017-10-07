#pragma once

#include <memory>
#include <functional>

namespace plot
{

    template < typename C >
    using ptr_t = std::shared_ptr < C > ;

    template < class C, class ... T > static ptr_t < C > create(T && ... t) { return std::make_shared < C > (std::forward < T > (t) ...); }
}
