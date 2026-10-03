#include "Tile.h"

ClassImp(Tile);

/**
 * @brief Return the deposited energy (MIP equivalent).
 * @return Energy value stored in the Tile.
 */
double Tile::GetE(void)const {
  return E;
}

/**
 * @brief Return the integer cell identifier.
 * @return CellID
 */
int Tile::GetCellID(void)const{
  return CellID;
}

/**
 * @brief Return the column index of the cell using the global Setup.
 * @return Column index or -999 if Setup is not initialized.
 */
int Tile::GetCol(void) const{
  Setup* setup=Setup::GetInstance();
  if(!setup->IsInit()) return -999;
  else return setup->GetColumn(CellID);
}

/**
 * @brief Return the layer index of the cell using the global Setup.
 * @return Layer index or -999 if Setup is not initialized.
 */
int Tile::GetLayer(void) const{
  Setup* setup=Setup::GetInstance();
  if(!setup->IsInit()) return -999;
  else return setup->GetLayer(CellID);
}

/**
 * @brief Return the readout unit index for this cell via Setup.
 * @return Readout unit index or -999 if Setup is not initialized.
 */
int Tile::GetRU() const{
  Setup* setup=Setup::GetInstance();
  if(!setup->IsInit()) return -999;
  else return setup->GetROunit(CellID);
}

/**
 * @brief Return a short string describing the readout class for this Tile.
 * @return A string such as "Tile::Hgcroc" or "Tile::Caen".
 */
TString Tile::GetROClassName()const{
  switch(ROtype){
  case ReadOut::Type::Hgcroc:
    return "Tile::Hgcroc";
  case ReadOut::Type::Caen:
    return "Tile::Caen";
  default:
    return "Tile::Undefined";
  }
}

/**
 * @brief Return the readout channel for this cell via Setup.
 * @return Channel index or -999 if Setup is not initialized.
 */
int Tile::GetRch() const{
  Setup* setup=Setup::GetInstance();
  if(!setup->IsInit()) return -999;
  else return setup->GetROchannel(CellID);
}

/**
 * @brief Return the row index of the cell via Setup.
 * @return Row index or -999 if Setup is not initialized.
 */
int Tile::GetRow() const{
  Setup* setup=Setup::GetInstance();
  if(!setup->IsInit()) return -999;
  else return setup->GetRow(CellID);
}

/**
 * @brief Return assembly identifier for the cell via Setup.
 * @return Assembly ID string or "Undefined" if Setup is not initialized.
 */
TString Tile::GetAssemblyID() const{
  Setup* setup=Setup::GetInstance();
  if(!setup->IsInit()) return "Undefined";
  else return setup->GetAssemblyID(CellID);
}

/**
 * @brief Return the X coordinate of the cell via Setup.
 * @return X coordinate or -999. if Setup not initialized.
 */
double Tile::GetX()const{
  Setup* setup=Setup::GetInstance();
  if(!setup->IsInit()) return -999.;
  else return setup->GetX(CellID);
}

/**
 * @brief Return the Y coordinate of the cell via Setup.
 * @return Y coordinate or -999. if Setup not initialized.
 */
double Tile::GetY() const{
  Setup* setup=Setup::GetInstance();
  if(!setup->IsInit()) return -999.;
  else return setup->GetY(CellID);
}

/**
 * @brief Return the Z coordinate of the cell via Setup.
 * @return Z coordinate or -999. if Setup not initialized.
 */
double Tile::GetZ() const{
  Setup* setup=Setup::GetInstance();
  if(!setup->IsInit()) return -999.;
  else return setup->GetZ(CellID);
}

/**
 * @brief Return stored time-of-arrival (TOA) for the tile.
 * @return TOA value.
 */
double Tile::GetTOA()const{
  return TOA;
}

/**
 * @brief Return the stored local trigger primitive value.
 * @return Local trigger primitive.
 */
double Tile::GetLocalTriggerPrimitive() const{
  return lTPr;
}

/**
 * @brief Return the stored local trigger bit.
 * @return Local trigger bit.
 */
unsigned char Tile::GetLocalTriggerBit() const{
  return lTrBit;
}

/**
 * @brief Set deposited energy (MIP equivalent).
 * @param e Energy value to store.
 */
void Tile::SetE(double e){
  E=e;
}

/**
 * @brief Set the cell identifier.
 * @param i Integer cell ID.
 */
void Tile::SetCellID(int i){
  //Shall we make some checks against setup?
  CellID=i;
}

/**
 * @brief Set the readout type for this tile.
 * @param i ReadOut::Type value.
 */
void Tile::SetROtype(ReadOut::Type i){
  ROtype=i;
}

/**
 * @brief Set the time-of-arrival (TOA) value.
 * @param t TOA to store.
 */
void Tile::SetTOA(double t){
  TOA=t;
}

/**
 * @brief Set the local trigger primitive value.
 * @param tr Primitive value to store.
 */
void Tile::SetLocalTriggerPrimitive(double tr){
  lTPr=tr;
}

/**
 * @brief Set the local trigger bit.
 * @param tr Trigger bit to store.
 */
void Tile::SetLocalTriggerBit(unsigned char tr){
  lTrBit=tr;
}
