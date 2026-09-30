#include "Event.h"

/**
 * @brief Return the run number associated with this event.
 * @return RunNumber
 */
int Event::GetRunNumber(void) const{
  return RunNumber;
}

/**
 * @brief Return the event identifier.
 * @return EventID
 */
int Event::GetEventID(void) const{
  return EventID;
}

/**
 * @brief Return the event timestamp.
 * @return TimeStamp
 */
double Event::GetTimeStamp(void) const{
  return TimeStamp;
}

/**
 * @brief Return the readout type for this event (e.g. Caen, HGCROC).
 * @return ROtype
 */
ReadOut::Type Event::GetROtype(void) const{
  return ROtype;
}

/**
 * @brief Set the run number for this event.
 * @param r Run number
 */
void Event::SetRunNumber(int r){
  RunNumber=r;
}

/**
 * @brief Set the event identifier.
 * @param ev Event ID
 */
void Event::SetEventID(int ev){
  EventID=ev;
}

/**
 * @brief Set the event timestamp.
 * @param t Timestamp value
 */
void Event::SetTimeStamp(double t){
  TimeStamp=t;
}

/**
 * @brief Set the readout type for this event.
 * @param ro ReadOut::Type value
 */
void Event::SetROtype(ReadOut::Type ro){
  ROtype=ro;
}

/**
 * @brief Return the beam name.
 * @return BeamName
 */
TString Event::GetBeamName(void)const{
  return BeamName;
}

/**
 * @brief Return the beam identifier.
 * @return BeamID
 */
int Event::GetBeamID(void) const{
  return BeamID;
}

/**
 * @brief Return the beam energy.
 * @return BeamEnergy
 */
double Event::GetBeamEnergy(void) const{
  return BeamEnergy;
}

/**
 * @brief Set the beam name for this event.
 * @param n Beam name string
 */
void Event::SetBeamName(TString n){
  BeamName=n;
}

/**
 * @brief Set the beam identifier for this event.
 * @param id Beam id
 */
void Event::SetBeamID(int id){
  BeamID=id;
}

/**
 * @brief Set the beam energy for this event.
 * @param e Beam energy
 */
void Event::SetBeamEnergy(double e){
  BeamEnergy=e;
}

/**
 * @brief Add a Tile to the event. If a Tile with the same cell ID already
 *        exists the previous Tile is deleted and replaced.
 * @param t Pointer to Tile to add (Event takes ownership).
 */
void Event::AddTile(Tile* t){
  int id=t->GetCellID();
  std::map<int, Tile*>::iterator it=Tiles.find(id);
  if(it!=Tiles.end()){
    delete it->second;
    it->second=t;
    // std::cerr<<"What the hell am I doing here?, Or did I ClearTiles before ?"<<std::endl;
  }
  else{
    Tiles[id]=t;
    TileIDs.push_back(id);
  }
}

/**
 * @brief Remove and delete a Tile from the event.
 * @param t Pointer to Tile to remove.
 */
void Event::RemoveTile(Tile* t){
  int id=t->GetCellID();
  std::map<int, Tile*>::iterator it=Tiles.find(id);
  if(it!=Tiles.end()){
    delete it->second;
    Tiles.erase(it);
    TileIDs.erase(std::find(TileIDs.begin(),TileIDs.end(),id));
  }
  else{
    std::cerr<<"Tile does not belong to the stack in this event, dunno what went wrong"<<std::endl;
  }
}

/**
 * @brief Return a Tile pointer by insertion index.
 * @param index Index into insertion order vector.
 * @return Pointer to Tile or nullptr if not present.
 */
Tile* Event::GetTile(int index){
  int tempID = TileIDs.at(index);
  return GetTileFromID(tempID);
}

/**
 * @brief Return a Tile pointer by its cell ID.
 * @param id Cell ID of the tile.
 * @return Pointer to Tile or nullptr if not found.
 */
Tile* Event::GetTileFromID(int id){
  std::map<int, Tile*>::iterator it=Tiles.find(id);
  if(it!=Tiles.end()) return it->second;
  else return nullptr;
}

/**
 * @brief Return number of Tiles stored in the event.
 * @return Number of tiles
 */
int Event::GetNTiles(void)const{
  return (int)Tiles.size();
}

/**
 * @brief Delete and remove all Tiles from the event.
 */
void Event::ClearTiles(void){
  std::map<int, Tile*>::iterator it;
  for(it=Tiles.begin(); it!=Tiles.end(); ++it){
    delete it->second;
    it->second=NULL;
  }
  Tiles.clear();
  TileIDs.clear();
}

/**
 * @brief Return the Vov (voltage overdrive) value.
 * @return Vov
 */
double Event::GetVov() const{
  return Vov;
}

/**
 * @brief Return the Vop (operating voltage) value.
 * @return Vop
 */
double Event::GetVop()const{
  return Vop;
}

/**
 * @brief Return beam X position for this event.
 * @return BeamPosX
 */
double Event::GetBeamPosX(void) {
  return BeamPosX;
}

/**
 * @brief Return beam Y position for this event.
 * @return BeamPosY
 */
double Event::GetBeamPosY(void){
  return BeamPosY;
}

/**
 * @brief Get pointer to BeginRun TTimeStamp.
 * @return pointer to BeginRun
 */
const TTimeStamp* Event::GetBeginRunTime(void) const{
  return &BeginRun;
}

/**
 * @brief Return a copy of the BeginRun timestamp.
 * @return BeginRun (by value)
 */
TTimeStamp Event::GetBeginRunTimeAlt(void){
  return BeginRun;
}

/**
 * @brief Set Vov (voltage overdrive) value.
 * @param v value to set
 */
void Event::SetVov(double v){
  Vov=v;
}

/**
 * @brief Set Vop (operating voltage) value.
 * @param v value to set
 */
void Event::SetVop(double v){
  Vop=v;
}
/**
 * @brief Set beam X position for this event.
 * @param x X coordinate
 */
void Event::SetBeamPosX(double x){
  BeamPosX=x;
}
/**
 * @brief Set beam Y position for this event.
 * @param y Y coordinate
 */
void Event::SetBeamPosY(double y){
  BeamPosY=y;
}
/**
 * @brief Set the BeginRun timestamp for the event's run.
 * @param t TTimeStamp value
 */
void Event::SetBeginRunTime(TTimeStamp t){
  BeginRun=t;
}

//**********************************************************************************
// Check if tile is compatible with mip only based on trigger primitive
//**********************************************************************************
/**
 * @brief Inspect whether the specified tile's local trigger primitive is
 *        compatible with a muon-like (MIP) signature.
 * @param currTileID Cell ID of tile to inspect.
 * @param averageScale Reference average scale used to form thresholds.
 * @param minThrSc Minimum threshold scale (default 0.9).
 * @param maxThrSc Maximum threshold scale (default 3).
 * @return true if the tile's local trigger primitive is within thresholds,
 *         false otherwise.
 */
bool Event::InspectIfLocalMuonTrigg( int currTileID, 
                                     double averageScale,
                                     double minThrSc = 0.9, 
                                     double maxThrSc = 3
                                    ){
  Setup* setup = Setup::GetInstance();
  double trPrim = ((Tile*)GetTileFromID(currTileID))->GetLocalTriggerPrimitive();
  // std::cout << "Trigg Primitive & decision: " << averageScale*minThrSc  << "\t" << maxThrSc*averageScale << "\t" << trPrim << std::endl;
  // evaluate stored trigger primitive
  if (trPrim >  averageScale*minThrSc && trPrim < maxThrSc*averageScale)
    return true;
  else 
    return false;
    
}

//**********************************************************************************
// Check if tile is compatible with noise only based on trigger primitive
//**********************************************************************************
/**
 * @brief Inspect whether the specified tile's local trigger primitive is
 *        compatible with noise (below threshold).
 * @param currTileID Cell ID of tile to inspect.
 * @param averageScale Reference average scale used to form threshold.
 * @param minThrSc Minimum threshold scale (default 0.9).
 * @return true if the tile's local trigger primitive is below threshold.
 */
bool Event::InspectIfNoiseTrigg( int currTileID, 
                                double averageScale,
                                double minThrSc = 0.9
                              ){
  double trPrim = ((Tile*)GetTileFromID(currTileID))->GetLocalTriggerPrimitive();
  // evaluate stored trigger primitive
  if (trPrim < averageScale*minThrSc )
    return true;
  else 
    return false;
    
}

//**********************************************************************************
// Local trigger primitive calculation using cells/ segments surrounding the cell 
// under investigation
// *nTiles* defines how many tiles/segmetns in depth will be evaluated
//    - 2 : the one before and after
//    - 4 : 2 before and 2 after
//    - 6 : 3 before and 3 after
// for z close to 0 or the maxLength of module same number of tiles will be kept, 
// but successively adding before or after until it fits as described above
//**********************************************************************************
/**
 * @brief Calculate a local muon trigger primitive from neighbouring tiles.
 *
 * The routine determines a set of neighbouring cell IDs according to the
 * requested depth (nTiles) and then computes an average signal using the
 * calibrated ADC/TOT values depending on the readout type.
 *
 * @param calib Calibration object used for pedestal and correction lookups.
 * @param rand Random number generator used for random smearing in some branches.
 * @param currTileID Cell ID of the tile under inspection.
 * @param nTiles Number of tiles to consider (depth). Typical values: 2,4,6.
 * @param avLGHG Fallback LG/HG scale used when calibration correction is not available.
 * @param calibOption Choice of calibration combination algorithm (>=2 uses both HG and LG when possible).
 * @return Average surrounding signal (avsurr) used as a local trigger primitive.
 */
double Event::CalculateLocalMuonTrigg(  Calib calib, 
                                        TRandom3* rand,
                                        int currTileID, 
                                        int nTiles      = 4,
                                        double avLGHG   = 10.,
                                        int calibOption = 2
                                      ){
  
  // figure out surrounding tiles for local mip selection
  Setup* setup = Setup::GetInstance();
  long ids[6]  = {-1, -1, -1, -1, -1, -1};
  int layer     = setup->GetLayer(currTileID);
  int row       = setup->GetRow(currTileID);
  int col       = setup->GetColumn(currTileID);
  int mod       = setup->GetModule(currTileID);
  if (nTiles == 4){
    if (layer == 0){
      ids[0] = setup->GetCellID(row, col, layer+1,mod);
      ids[1] = setup->GetCellID(row, col, layer+2,mod);
      ids[2] = setup->GetCellID(row, col, layer+3,mod);
      ids[3] = setup->GetCellID(row, col, layer+4,mod);
    }else if (layer == 1){
      ids[0] = setup->GetCellID(row, col, layer-1,mod);
      ids[1] = setup->GetCellID(row, col, layer+1,mod);
      ids[2] = setup->GetCellID(row, col, layer+2,mod);
      ids[3] = setup->GetCellID(row, col, layer+3,mod);
    } else if (layer == setup->GetNMaxLayer()-1){
      ids[0] = setup->GetCellID(row, col, layer-3,mod);
      ids[1] = setup->GetCellID(row, col, layer-2,mod);
      ids[2] = setup->GetCellID(row, col, layer-1,mod);
      ids[3] = setup->GetCellID(row, col, layer+1,mod);      
    } else if (layer == setup->GetNMaxLayer()){
      ids[0] = setup->GetCellID(row, col, layer-4,mod);
      ids[1] = setup->GetCellID(row, col, layer-3,mod);
      ids[2] = setup->GetCellID(row, col, layer-2,mod);
      ids[3] = setup->GetCellID(row, col, layer-1,mod);      
    } else {
      ids[0] = setup->GetCellID(row, col, layer-2,mod);
      ids[1] = setup->GetCellID(row, col, layer-1,mod);
      ids[2] = setup->GetCellID(row, col, layer+1,mod);
      ids[3] = setup->GetCellID(row, col, layer+2,mod);
    }
  } else if (nTiles == 6){  
    if (layer == 0){
      ids[0] = setup->GetCellID(row, col, layer+1,mod);
      ids[1] = setup->GetCellID(row, col, layer+2,mod);
      ids[2] = setup->GetCellID(row, col, layer+3,mod);
      ids[3] = setup->GetCellID(row, col, layer+4,mod);
      ids[4] = setup->GetCellID(row, col, layer+5,mod);
      ids[5] = setup->GetCellID(row, col, layer+6,mod);
    }else if (layer == 1){
      ids[0] = setup->GetCellID(row, col, layer-1,mod);
      ids[1] = setup->GetCellID(row, col, layer+1,mod);
      ids[2] = setup->GetCellID(row, col, layer+2,mod);
      ids[3] = setup->GetCellID(row, col, layer+3,mod);
      ids[4] = setup->GetCellID(row, col, layer+4,mod);
      ids[5] = setup->GetCellID(row, col, layer+5,mod);
    }else if (layer == 2){
      ids[0] = setup->GetCellID(row, col, layer-2,mod);
      ids[1] = setup->GetCellID(row, col, layer-1,mod);
      ids[2] = setup->GetCellID(row, col, layer+1,mod);
      ids[3] = setup->GetCellID(row, col, layer+2,mod);
      ids[4] = setup->GetCellID(row, col, layer+3,mod);
      ids[5] = setup->GetCellID(row, col, layer+4,mod);
    } else if (layer == setup->GetNMaxLayer()-2){
      ids[0] = setup->GetCellID(row, col, layer-4,mod);
      ids[1] = setup->GetCellID(row, col, layer-3,mod);
      ids[2] = setup->GetCellID(row, col, layer-2,mod);
      ids[3] = setup->GetCellID(row, col, layer-1,mod);
      ids[4] = setup->GetCellID(row, col, layer+1,mod);
      ids[5] = setup->GetCellID(row, col, layer+2,mod);      
    } else if (layer == setup->GetNMaxLayer()-1){
      ids[0] = setup->GetCellID(row, col, layer-5,mod);
      ids[1] = setup->GetCellID(row, col, layer-4,mod);
      ids[2] = setup->GetCellID(row, col, layer-3,mod);
      ids[3] = setup->GetCellID(row, col, layer-2,mod);
      ids[4] = setup->GetCellID(row, col, layer-1,mod);
      ids[5] = setup->GetCellID(row, col, layer+1,mod);      
    } else if (layer == setup->GetNMaxLayer()){
      ids[0] = setup->GetCellID(row, col, layer-6,mod);
      ids[1] = setup->GetCellID(row, col, layer-5,mod);
      ids[2] = setup->GetCellID(row, col, layer-4,mod);
      ids[3] = setup->GetCellID(row, col, layer-3,mod);
      ids[4] = setup->GetCellID(row, col, layer-2,mod);
      ids[5] = setup->GetCellID(row, col, layer-1,mod);      
    } else {
      ids[0] = setup->GetCellID(row, col, layer-3,mod);
      ids[1] = setup->GetCellID(row, col, layer-2,mod);
      ids[2] = setup->GetCellID(row, col, layer-1,mod);
      ids[3] = setup->GetCellID(row, col, layer+1,mod);
      ids[4] = setup->GetCellID(row, col, layer+2,mod);
      ids[5] = setup->GetCellID(row, col, layer+3,mod);
    }
    
  } else if (nTiles == 2){
    if (layer == 0){
      ids[0] = setup->GetCellID(row, col, layer+1,mod);
      ids[1] = setup->GetCellID(row, col, layer+2,mod);            
    } else if (layer == setup->GetNMaxLayer()){
      ids[0] = setup->GetCellID(row, col, layer-2,mod);
      ids[1] = setup->GetCellID(row, col, layer-1,mod);          
    } else {
      ids[0] = setup->GetCellID(row, col, layer-1,mod);
      ids[1] = setup->GetCellID(row, col, layer+1,mod);      
    }
  }
  // calculate average sum of surrounding tiles (nominally 2 in the front + 2 in the back)
  double avsurr = 0;
  Int_t activeTiles = nTiles;
  Int_t tilesWTOA   = 0;
  // run over tiles in array
  for (Int_t t = 0; t < nTiles; t++){
    // check if cells were active
    // cell not even contained in event
    if ((Tile*)GetTileFromID(ids[t]) == nullptr){
      activeTiles--;
      continue;
    }
    // cell flagged as bad (1), off (0) or funky (2)
    if (calib.GetBCCalib()){
      if (calib.GetBadChannel(ids[t]) != -64  && calib.GetBadChannel(ids[t]) < 3){
        activeTiles--;
        continue;
      }
    }
    double tmpGain  = 0;
    double scale    = (calib.GetLGHGCorr(ids[t]) == -64.) ? avLGHG : calib.GetLGHGCorr(ids[t]);     // only use LG-HG corr factor if fit succeeded, otherwise use average
    //==========================================================================================
    // distinguish calculation for different readout types
    //==========================================================================================
    // CAEN evaluation 
    //==========================================================================================
    if (GetROtype() == ReadOut::Type::Caen){
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // need to distinguish between calibration options (as HG in physics data might be corrupted)
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // Using combined information from HG & LG (should be giving best resolution)
      if (calibOption >= 2){
        // calculating combined gain
        if (((Caen*)GetTileFromID(ids[t]))->GetADCHigh() < 3800)
          tmpGain = ((Caen*)GetTileFromID(ids[t]))->GetADCHigh()-calib.GetPedestalMeanH(ids[t]); 
        else 
          // needs radom smearing for the LG equivalent otherwise wouldn't be able to have finer values than scale
          tmpGain = (((Caen*)GetTileFromID(ids[t]))->GetADCLow()-calib.GetPedestalMeanL(ids[t]))*scale + rand->Rndm()*scale;
        if (tmpGain > 3*calib.GetPedestalSigH(ids[t]))
          avsurr +=tmpGain;

      // Using only low gain information (probably less corrupted in physics events)
      } else {
        tmpGain = (((Caen*)GetTileFromID(ids[t]))->GetADCLow()-calib.GetPedestalMeanL(ids[t]));
        if (tmpGain > 1.5*calib.GetPedestalSigL(ids[t])) // needs lower threshold than HG due to worse separation
          avsurr +=tmpGain;
      }
    //==========================================================================================
    // HGCROC evaluation 
    //==========================================================================================
    } else {
      // TODO: THIS NEEDS TO BE REWORKED ONCE WE HAVE THE CROSS CALIB BETWEEN ADC & TOT
      tmpGain = ((Hgcroc*)GetTileFromID(ids[t]))->GetIntegratedADC()/setup->GetLayersInSegment(ids[t]);
      avsurr +=tmpGain;
      // std:: cout << tmpGain << "\t";
    }
  }
  // calculate average if more than one cell was active
  if (activeTiles > 1)
    avsurr        = avsurr/activeTiles;
  
  return avsurr;
}

//**********************************************************************************
// Check if event is corrupted
//**********************************************************************************
/**
 * @brief Basic integrity check for the event data.
 *
 * For CAEN readout the function verifies that signals are within expected
 * bounds by comparing ADC values to pedestal-corrected thresholds. If any
 * tile violates the expected relationship the event is considered corrupted.
 *
 * @param calib Calibration data to use for pedestal and status checks.
 * @param thLG Low-gain threshold used for corruption detection.
 * @param thHG High-gain threshold used for corruption detection.
 * @return true if event passes integrity checks, false if corruption detected.
 */
bool Event::CheckEventIntegrity( Calib calib, double thLG, double thHG){
  if (GetROtype() == ReadOut::Type::Caen){
    for(int j=0; j<GetNTiles(); j++){
      long cellID = TileIDs.at(j);
      if ((((Caen*)GetTileFromID(cellID))->GetADCLow() - calib.GetPedestalMeanL(cellID) > thLG)  && 
          (((Caen*)GetTileFromID(cellID))->GetADCHigh() - calib.GetPedestalMeanL(cellID) < thHG)  ){
          return false;
      }
    }
    return true;
  } else {
    return true;
  }
  return true;
}
