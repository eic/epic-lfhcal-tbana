/**
 * @file Event.h
 * @brief Definition of the Event class used to hold detector tiles and event-level metadata.
 *
 * This header declares the Event class which collects Tile objects for a single
 * detector event and provides accessors for event-level information such as run
 * number, event id, beam properties and timestamps.
 */

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
#include <algorithm>

/**
 * @class Event
 * @brief Container for detector Tiles and event-level metadata.
 *
 * The Event class owns Tile pointers added to it and is responsible for
 * managing their lifetime (deleting them on replacement or clear).
 */
class Event{

 public:

  /**
   * @brief Add a Tile to the event. The Event takes ownership of the pointer.
   * @param t Pointer to Tile to add.
   */
  void    AddTile      (Tile*);

  /**
   * @brief Remove a Tile from the event and delete it.
   * @param t Pointer to Tile to remove.
   */
  void    RemoveTile   (Tile*);

  /**
   * @brief Remove and delete all Tiles from the event.
   */
  void    ClearTiles   (void);
  
  /**
   * @brief Get the beam energy for this event.
   * @return Beam energy (units as stored in the event).
   */
  double  GetBeamEnergy(void) const;

  /**
   * @brief Get the beam ID.
   * @return Integer beam identifier.
   */
  int     GetBeamID    (void) const;

  /**
   * @brief Get the beam name as a TString.
   * @return Beam name.
   */
  TString GetBeamName  (void) const;

  /**
   * @brief Get the event identifier.
   * @return Event id.
   */
  int     GetEventID   (void) const;

  /**
   * @brief Number of Tiles stored in the event.
   * @return Number of tiles.
   */
  int     GetNTiles    (void) const;

  /**
   * @brief Get the readout type for this event (Caen / HGCROC ...).
   * @return ReadOut::Type value.
   */
  ReadOut::Type GetROtype    (void) const;

  /**
   * @brief Get the run number this event belongs to.
   * @return Run number.
   */
  int     GetRunNumber (void) const;

  /**
   * @brief Access a Tile by index (order of insertion).
   * @param index Index into internal Tile array (0..GetNTiles()-1)
   * @return Pointer to Tile or nullptr if out of range.
   */
  Tile*   GetTile      (int);

  /**
   * @brief Get a Tile by its cell ID.
   * @param id Cell ID of the tile.
   * @return Pointer to Tile or nullptr if not present.
   */
  Tile*   GetTileFromID(int);

  /**
   * @brief Get the event timestamp.
   * @return Timestamp as double (units as used elsewhere in the codebase).
   */
  double     GetTimeStamp (void) const;

  /**
   * @brief Get the Vov (operating voltage overdrive) value for this event.
   * @return Vov value.
   */
  double  GetVov       (void) const;

  /**
   * @brief Get the Vop (operating voltage) value for this event.
   * @return Vop value.
   */
  double  GetVop       (void) const;

  /**
   * @brief Get the beam X position for this event.
   * @return Beam X coordinate.
   */
  double    GetBeamPosX(void);

  /**
   * @brief Get the beam Y position for this event.
   * @return Beam Y coordinate.
   */
  double    GetBeamPosY(void);

  /**
   * @brief Get a pointer to the TTimeStamp marking the begin of the run.
   * @return Pointer to TTimeStamp.
   */
  const TTimeStamp* GetBeginRunTime(void) const;

  /**
   * @brief Alternative getter that returns the BeginRun timestamp by value.
   * @return TTimeStamp copy.
   */
  TTimeStamp GetBeginRunTimeAlt(void);
  
  /**
   * @brief Set the beam energy.
   * @param value Beam energy.
   */
  void    SetBeamEnergy(double);

  /**
   * @brief Set the beam id.
   * @param id Beam id.
   */
  void    SetBeamID    (int);

  /**
   * @brief Set the beam name.
   * @param name Beam name.
   */
  void    SetBeamName  (TString);

  /**
   * @brief Set the event id.
   * @param id Event id.
   */
  void    SetEventID   (int);

  /**
   * @brief Set the readout type for this event.
   * @param type ReadOut::Type value.
   */
  void    SetROtype    (ReadOut::Type);

  /**
   * @brief Set the run number for this event.
   * @param r Run number.
   */
  void    SetRunNumber (int);

  /**
   * @brief Set the timestamp marking begin of the run.
   * @param t TTimeStamp value.
   */
  void    SetBeginRunTime(TTimeStamp);

  /**
   * @brief Set Vov value.
   * @param v Vov value.
   */
  void    SetVov(double);

  /**
   * @brief Set Vop value.
   * @param v Vop value.
   */
  void    SetVop(double);

  /**
   * @brief Set beam X position.
   * @param x X coordinate.
   */
  void    SetBeamPosX(double);

  /**
   * @brief Set beam Y position.
   * @param y Y coordinate.
   */
  void    SetBeamPosY(double);

  /**
   * @brief Set the event timestamp.
   * @param t Timestamp value.
   */
  void    SetTimeStamp (double);

  /**
   * @brief Inspect whether a tile is compatible with a local muon trigger
   *        based on its local trigger primitive and provided thresholds.
   * @param tileID ID of the tile under inspection.
   * @param averageScale Average scale used in decision.
   * @param minThrSc Minimum threshold scale (default 0.9).
   * @param maxThrSc Maximum threshold scale (default 3).
   * @return true if compatible with local muon trigger, false otherwise.
   */
  bool    InspectIfLocalMuonTrigg(int, double, double, double);

  /**
   * @brief Inspect whether a tile is compatible with noise based on trigger primitive.
   * @param tileID ID of the tile under inspection.
   * @param averageScale Average scale used in decision.
   * @param minThrSc Minimum threshold scale (default 0.9).
   * @return true if compatible with noise, false otherwise.
   */
  bool    InspectIfNoiseTrigg(int, double, double);

  /**
   * @brief Calculate a local muon trigger primitive from neighboring tiles.
   * @param calib Calibration object used for pedestals and corrections.
   * @param rand Random number generator used for smearing in some branches.
   * @param currTileID ID of the tile under inspection.
   * @param nTiles Number of tiles to consider (depth), default 4.
   * @param avLGHG Average LG/HG correction value, default 10.
   * @param calibOption Calibration option to select calculation mode, default 2.
   * @return Calculated average surrounding signal used as local trigger primitive.
   */
  double  CalculateLocalMuonTrigg(Calib, TRandom3*, int, int, double, int);

  /**
   * @brief Check event integrity for obvious corruption based on thresholds.
   * @param calib Calibration data used to compare pedestals.
   * @param thLG Low-gain threshold (default 20).
   * @param thHG High-gain threshold (default 150).
   * @return true if event looks valid, false if corrupted.
   */
  bool CheckEventIntegrity( Calib calib, double thLG = 20, double thHG = 150);
  
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
  double                TimeStamp;
  
 protected:

};


#endif
