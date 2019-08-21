#ifndef GeomWireCellData_SlimMergeCell_h
#define GeomWireCellData_SlimMergeCell_h

#include "WireCellData/GeomCell.h"
#include "WireCellData/GeomWireCellMap.h"
#include <tuple>


namespace WireCell{
class SlimMergeGeomCell : public WireCell::GeomCell{
  public:

    SlimMergeGeomCell(int ident);
   ~SlimMergeGeomCell();

    /// Unbiased "center of mass" of boundary points
   
   void AddWire(const GeomWire *wire, WirePlaneType_t plane, float charge=0, float charge_err = 0);
   void AddBoundary( const PointVector& boundary );
   void AddSamplingPoints(const PointVector& sampling_points);
   void AddSamplingPointsWires(std::vector<std::tuple<int,int,int>>& sampling_points_wires);
   void SetMaxWireInterval(WirePlaneType_t type, int value);
   void SetMinWireInterval(WirePlaneType_t type, int value);
   //Point get_sampling_points_center();
    
   GeomWireSelection& get_uwires() {return uwires;};
   GeomWireSelection& get_vwires() {return vwires;};
   GeomWireSelection& get_wwires() {return wwires;};
   
   int GetTimeSlice() { return time_slice;}; 
   void SetTimeSlice(int time){time_slice = time;}; 
   
   void OrderWires();
   
   int GetIdent() {return _ident;};
   std::vector<WirePlaneType_t> get_bad_planes(){return bad_planes;};
   void add_bad_planes(WirePlaneType_t type);
   bool Overlap(WireCell::SlimMergeGeomCell* cell, float num=0.1) ;
   bool Overlap_fast(WireCell::SlimMergeGeomCell* cell, int offset=1) ;
   
   bool Adjacent(WireCell::SlimMergeGeomCell* cell) ;
   
     
   float Get_Wire_Charge(const GeomWire *wire);
   float Get_Wire_Charge_Err(const GeomWire *wire);
   
   float Estimate_total_charge();
   float Estimate_minimum_charge();
   
   bool IsPointGood(int index_u, int index_v, int index_w, int ncut = 3);

   float get_uq(){return uq;};
   float get_vq(){return vq;};
   float get_wq(){return wq;};
   float get_udq(){return udq;};
   float get_vdq(){return vdq;};
   float get_wdq(){return wdq;};
   float get_q(){if (q==0) {return 1;}else{ return q;}}
   Point center() const;

   void set_uq(float value){uq=value;};
   void set_vq(float value){vq=value;};
   void set_wq(float value){wq=value;};
   void set_udq(float value){udq=value;};
   void set_vdq(float value){vdq=value;};
   void set_wdq(float value){wdq=value;};
   void set_q(float value){q=value;};

   bool IsSame(SlimMergeGeomCell* mcell1);

   PointVector& get_sampling_points(){return sample_points;};
   std::vector<std::tuple<int,int,int>>& get_sampling_points_wires(){return sample_points_wires;};
   int get_max_wire_interval(){return max_wire_interval;};
   int get_min_wire_interval(){return min_wire_interval;};
   WirePlaneType_t get_max_wire_type(){return max_wire_type;};
   WirePlaneType_t get_min_wire_type(){return min_wire_type;};

   WireCell::WireChargeMap& get_wirecharge_map(){return wirechargemap;};
   WireCell::WireChargeMap& get_wirechargeerr_map(){return wirechargeerrmap;};
   
  protected:
    int _ident;
    int time_slice;
    float uq, udq, vq, vdq, wq, wdq, q;

    PointVector sample_points;
    std::vector<std::tuple<int,int,int>> sample_points_wires;
    int max_wire_interval, min_wire_interval;
    WirePlaneType_t max_wire_type, min_wire_type;

    
    //int order_boundary();
    WireCell::GeomWireSelection uwires;
    WireCell::GeomWireSelection vwires;
    WireCell::GeomWireSelection wwires;
    std::vector<WirePlaneType_t> bad_planes;

    WireCell::WireChargeMap wirechargemap;
    WireCell::WireChargeMap wirechargeerrmap;
    
    std::map<int, const WireCell::GeomWire* > map_index_uwire;
    std::map<int, const WireCell::GeomWire* > map_index_vwire;
    std::map<int, const WireCell::GeomWire* > map_index_wwire;
    
  };
 typedef std::vector<SlimMergeGeomCell*> SMGCSelection;
 typedef std::set<SlimMergeGeomCell*> SMGCSet;
 
}

#endif
