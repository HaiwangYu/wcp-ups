#ifndef WIRECELLROOTVIS_VIEWMIXIN
#define WIRECELLROOTVIS_VIEWMIXIN

#include "TPad.h"

namespace WireCellRootVis {

    class ViewMixin
    {
    public:
	virtual ~ViewMixin();

	virtual void draw(TPad& pad);
	virtual void update();

    };

}

#endif
