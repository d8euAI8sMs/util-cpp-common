#pragma once

#include <functional>
#include <memory>

#include <util/common/plot/viewport.h>
#include <util/common/plot/drawable.h>

namespace plot
{

    // viewport mapper




    using viewport_mapper_t = std::function < viewport (const viewport &) > ;



    static inline viewport_mapper_t create_viewport_mapper(const viewport & vp)
    {
        return [vp] (const viewport &) { return vp; };
    }

    static inline viewport_mapper_t create_viewport_mapper(const world_t & w)
    {
        return [w] (const viewport & vp) { return viewport(vp.screen, w); };
    }

    static inline viewport_mapper_t create_viewport_mapper(const screen_t & s)
    {
        return [s] (const viewport & vp) { return viewport(s, vp.world); };
    }

    static inline viewport_mapper_t create_viewport_mapper(std::shared_ptr < viewport > vp)
    {
        return [vp] (const viewport &) { return *vp; };
    }

    static inline viewport_mapper_t create_viewport_mapper(std::shared_ptr < world_t > w)
    {
        return [w] (const viewport & vp) { return viewport(vp.screen, *w); };
    }

    static inline viewport_mapper_t create_viewport_mapper(std::shared_ptr < screen_t > s)
    {
        return [s] (const viewport & vp) { return viewport(*s, vp.world); };
    }




    // viewporter




    class viewporter : public drawable
	{

    private:

		drawable_ptr      layer;
		viewport_mapper_t mapper;

	public:

		viewporter(drawable_ptr layer, viewport_mapper_t mapper)
			: layer(std::move(layer))
			, mapper(std::move(mapper))
		{
		}

		virtual void draw(CDC &dc, const viewport &bounds) override
		{
			layer->draw(dc, mapper(bounds));
		}
	};
};
