/**
 * @file Tile.h
 * @brief Declaration of the Tile base class which represents a single detector cell.
 *
 * Tile stores basic per-cell information such as cell identifier, deposited
 * energy (in MIP equivalent), time-of-arrival and local trigger primitives.
 */

#ifndef TILE_H
#define TILE_H

#include "TString.h"
#include "Setup.h"

namespace ReadOut{
  enum Type {Undef, Hgcroc, Caen};
}

/**
 * @class Tile
 * @brief Base representation of a detector tile / cell.
 *
 * Tile provides lightweight storage for cell-level quantities and convenience
 * accessors for geometry properties using the global Setup singleton.
 * The class is intended to be subclassed by readout-specific implementations
 * (e.g., Caen, Hgcroc) which add ADC/TOT specific data and behavior.
 */
class Tile{

 public:
  /**
   * @brief Default constructor - initializes members to sensible defaults.
   */
  Tile():E(0.),CellID(-1),ROtype(ReadOut::Type::Undef),TOA(0.),lTPr(0.),lTrBit(0){}

  /**
   * @brief Constructor that sets the readout type.
   * @param RO ReadOut type (Hgcroc/Caen/...)
   */
  Tile(ReadOut::Type RO):E(0.),CellID(-1),ROtype(RO),TOA(0.),lTPr(0.),lTrBit(0){}

  /**
   * @brief Virtual destructor to allow safe subclassing.
   */
  virtual ~Tile(){}
  
  /** @brief Return the assembly identifier string for this cell (via Setup). */
  TString       GetAssemblyID             (void) const;

  /** @brief Return the integer cell identifier. */
  int           GetCellID                 (void) const;

  /** @brief Return the column index of the cell (via Setup). */
  int           GetCol                    (void) const;

  /** @brief Return the deposited energy in MIP equivalent. */
  double        GetE                      (void) const;

  /** @brief Return the layer index of the cell (via Setup). */
  int           GetLayer                  (void) const;

  /** @brief Return the readout channel index for the cell (via Setup). */
  int           GetRch                    (void) const;

  /** @brief Return a short string describing the readout class for this Tile. */
  TString       GetROClassName            (void) const;

  /** @brief Return the row index of the cell (via Setup). */
  int           GetRow                    (void) const;

  /** @brief Return the readout unit index for the cell (via Setup). */
  int           GetRU                     (void) const;

  /** @brief Return the time-of-arrival (TOA) value for this cell. */
  double        GetTOA                    (void) const;

  /** @brief Return the X coordinate of the cell (via Setup). */
  double        GetX                      (void) const;

  /** @brief Return the Y coordinate of the cell (via Setup). */
  double        GetY                      (void) const;

  /** @brief Return the Z coordinate of the cell (via Setup). */
  double        GetZ                      (void) const;

  /** @brief Return the local trigger primitive value for this cell. */
  double        GetLocalTriggerPrimitive  (void) const;

  /** @brief Return the local trigger bit for this cell. */
  unsigned char GetLocalTriggerBit        (void) const;
  
  /** @brief Set the cell identifier. */
  void    SetCellID                 (int);

  /** @brief Set the deposited energy (MIP equivalent). */
  void    SetE                      (double);

  /** @brief Set the readout type for this tile. */
  void    SetROtype                 (ReadOut::Type);

  /** @brief Set the time-of-arrival (TOA). */
  void    SetTOA                    (double);

  /** @brief Set the local trigger primitive value. */
  void    SetLocalTriggerPrimitive  (double);

  /** @brief Set the local trigger bit. */
  void    SetLocalTriggerBit        (unsigned char);
  
  
 protected:
  double           E;            ///< Energy deposited in MIP equivalent
  int              CellID;       ///< Unique cell identifier
  ReadOut::Type    ROtype;       ///< Readout unit type
  double           TOA;          ///< Time of arrival
  double           lTPr;         ///< Local trigger primitive
  unsigned char    lTrBit;       ///< Local trigger bit
  
  ClassDef(Tile,2)

};


#endif
