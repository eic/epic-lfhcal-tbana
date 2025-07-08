#include "Event.h"

int Event::GetRunNumber(void) const{
  return RunNumber;
}

int Event::GetEventID(void) const{
  return EventID;
}

double Event::GetTimeStamp(void) const{
  return TimeStamp;
}

ReadOut::Type Event::GetROtype(void) const{
  return ROtype;
}

void Event::SetRunNumber(int r){
  RunNumber=r;
}

void Event::SetEventID(int ev){
  EventID=ev;
}

void Event::SetTimeStamp(double t){
  TimeStamp=t;
}

void Event::SetROtype(ReadOut::Type ro){
  ROtype=ro;
}

TString Event::GetBeamName(void)const{
  return BeamName;
}

int Event::GetBeamID(void) const{
  return BeamID;
}

double Event::GetBeamEnergy(void) const{
  return BeamEnergy;
}

void Event::SetBeamName(TString n){
  BeamName=n;
}

void Event::SetBeamID(int id){
  BeamID=id;
}

void Event::SetBeamEnergy(double e){
  BeamEnergy=e;
}

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

Tile* Event::GetTile(int index){
  int tempID = TileIDs.at(index);
  return GetTileFromID(tempID);
}

Tile* Event::GetTileFromID(int id){
  std::map<int, Tile*>::iterator it=Tiles.find(id);
  if(it!=Tiles.end()) return it->second;
  else return nullptr;
}

int Event::GetNTiles(void)const{
  return (int)Tiles.size();
}

void Event::ClearTiles(void){
  std::map<int, Tile*>::iterator it;
  for(it=Tiles.begin(); it!=Tiles.end(); ++it){
    delete it->second;
    it->second=NULL;
  }
  Tiles.clear();
  TileIDs.clear();
}

double Event::GetVov() const{
  return Vov;
}

double Event::GetVop()const{
  return Vop;
}

double Event::GetBeamPosX(void) {
  return BeamPosX;
}

double Event::GetBeamPosY(void){
  return BeamPosY;
}

const TTimeStamp* Event::GetBeginRunTime(void) const{
  return &BeginRun;
}

TTimeStamp Event::GetBeginRunTimeAlt(void){
  return BeginRun;
}

void Event::SetVov(double v){
  Vov=v;
}

void Event::SetVop(double v){
  Vop=v;
}
void Event::SetBeamPosX(double x){
  BeamPosX=x;
}
void Event::SetBeamPosY(double y){
  BeamPosY=y;
}
void Event::SetBeginRunTime(TTimeStamp t){
  BeginRun=t;
}

bool Event::InspectIfLocalMuonTrigg( int currTileID, 
                                     double averageScale,
                                     double minThrSc = 0.9, 
                                     double maxThrSc = 3
                                    ){
  
  double trPrim = ((Caen*)GetTileFromID(currTileID))->GetLocalTriggerPrimitive();
  // evaluate stored trigger primitive
  if (trPrim >  averageScale*minThrSc && trPrim < maxThrSc*averageScale)
    return true;
  else 
    return false;
    
}

bool Event::InspectIfNoiseTrigg( int currTileID, 
                                double averageScale,
                                double minThrSc = 0.9
                              ){
  double trPrim = ((Caen*)GetTileFromID(currTileID))->GetLocalTriggerPrimitive();
  // evaluate stored trigger primitive
  if (trPrim < averageScale*minThrSc )
    return true;
  else 
    return false;
    
}

double Event::CalculateLocalMuonTrigg(  Calib calib, 
                                        TRandom3* rand,
                                        int currTileID, 
                                        int nTiles = 4,
                                        double avLGHG = 10.
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
  for (Int_t t = 0; t < nTiles; t++){
    if ((Caen*)GetTileFromID(ids[t]) == nullptr){
      activeTiles--;
      continue;
    }
    if (calib.GetBCCalib()){
      if (calib.GetBadChannel(ids[t]) != -64  && calib.GetBadChannel(ids[t]) < 3){
        activeTiles--;
        continue;
      }
    }
    double tmpGain  = 0;
    double scale    = (calib.GetLGHGCorr(ids[t]) == -64.) ? avLGHG : calib.GetLGHGCorr(ids[t]);     // only use LG-HG corr factor if fit succeeded, otherwise use average
    // calculating combined gain
    if (((Caen*)GetTileFromID(ids[t]))->GetADCHigh() < 3800)
      tmpGain = ((Caen*)GetTileFromID(ids[t]))->GetADCHigh()-calib.GetPedestalMeanH(ids[t]); 
    else 
      tmpGain = (((Caen*)GetTileFromID(ids[t]))->GetADCLow()-calib.GetPedestalMeanL(ids[t]))*scale + rand->Rndm()*scale;
      
    if (tmpGain > 3*calib.GetPedestalSigH(ids[t]))
      avsurr +=tmpGain;
  }
  if (activeTiles > 1)
    avsurr        = avsurr/activeTiles;
  return avsurr;
}

