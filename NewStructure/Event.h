#ifndef EVENT_H
#define EVENT_H

#include "TString.h"
#include "TTimeStamp.h"
#include <map>
#include <iostream>
#include "Caen.h"
#include "HGCROC.h"
#include "Setup.h"
#include "Calib.h"
#include "TRandom3.h"

class Event{

 public:

  void    AddTile      (Tile*);
  void    RemoveTile   (Tile*);
  void    ClearTiles   (void);
  
  double  GetBeamEnergy(void) const;
  int     GetBeamID    (void) const;
  TString GetBeamName  (void) const;
  int     GetEventID   (void) const;
  int     GetNTiles    (void) const;
  ReadOut::Type GetROtype    (void) const;
  int     GetRunNumber (void) const;
  Tile*   GetTile      (int);
  Tile*   GetTileFromID(int);
  int     GetTimeStamp (void) const;
  double  GetVov       (void) const;
  double  GetVop       (void) const;
  double    GetBeamPosX(void);
  double    GetBeamPosY(void);
  const TTimeStamp* GetBeginRunTime(void) const;
  TTimeStamp GetBeginRunTimeAlt(void);
  
  void    SetBeamEnergy(double);
  void    SetBeamID    (int);
  void    SetBeamName  (TString);
  void    SetEventID   (int);
  void    SetROtype    (ReadOut::Type);
  void    SetRunNumber (int);
  void    SetBeginRunTime(TTimeStamp);
  void    SetVov(double);
  void    SetVop(double);
  void    SetBeamPosX(double);
  void    SetBeamPosY(double);
  void    SetTimeStamp (int);

  bool    InspectIfLocalMuonTrigg(int, double, double, double);
  bool    InspectIfNoiseTrigg(int, double, double);
  double  CalculateLocalMuonTrigg(Calib, TRandom3*, int, int, double);

 private:

  double                BeamEnergy;
  int                   BeamID;
  TString               BeamName;
  int                   EventID;
  ReadOut::Type         ROtype;
  int                   RunNumber;
  TTimeStamp            BeginRun;
  double                Vov;
  double                Vop;
  double                BeamPosX;
  double                BeamPosY;
  std::map<int, Tile* > Tiles;
  std::vector<int>      TileIDs;
  int                   TimeStamp;
  
 protected:

};


#endif
