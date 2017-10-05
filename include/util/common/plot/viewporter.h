#pragma once

#include <functional>
#include <memory>

#include <util/common/plot/viewport.h>
#include <util/common/plot/drawable.h>

namespace plot
{




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
