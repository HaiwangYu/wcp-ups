#ifndef WIRECELLROOTVIS_VIEWGDS
#define  WIRECELLROOTVIS_VIEWGDS

#include "WireCellRootVis/ViewMixin.h"
#include "WireCellNav/GeomDataSource.h"

namespace WireCellRootVis {

    class ViewGDS : public ViewMixin
    {
	const WireCell::GeomDataSource& gds;
    public:
	ViewGDS(const WireCell::GeomDataSource& gds);
	virtual ~ViewGDS();
	virtual void draw(TPad& pad);
	virtual void update();
	
    };

}

#endif
