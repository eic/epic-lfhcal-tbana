/**
 * @file Setup.h
 * @brief Declaration of the central geometry and detector configuration singleton.
 *
 * The Setup class stores the mapping between cell IDs and detector geometry,
 * provides coordinate conversion utilities, and exposes the detector configuration
 * used by the rest of the LFHCal analysis code.
 */

#ifndef SETUP_H
#define SETUP_H

#include <cstddef>
#include <map>
#include <vector>
#include "TString.h"

class RootSetupWrapper;

/**
 * @brief Detector configuration identifiers used to describe the active geometry.
 */
namespace DetConf{
  enum Type { Undef,
              Unset,
              Single8M,
              Single4M,
              Single2MH,
              Single2MV,
              SingleTile,
              Dual8M,
              MediumTB,
              LargeTB,
              Asic,
              FocalH,
            };
}

/**
 * @class Setup
 * @brief Singleton containing the detector mapping and geometry information.
 *
 * Setup is initialized once from the detector mapping file or a root wrapper and
 * then provides constant-time lookup for cell geometries, coordinates and readout
 * mappings throughout the analysis workflow.
 */
class Setup{
  
 public:
  // deleting copy constructor.
  Setup(const Setup &)=delete;
  Setup& operator=(const Setup&)=delete;
   
  /**
   * @brief Return the singleton instance, creating it on demand.
   * @return Pointer to the Setup singleton.
   */
  static Setup *GetInstance(){
    if(instancePtr == NULL) instancePtr = new Setup();
    return instancePtr;
  }
   
  /**
   * @brief Return the assembly identifier for a cell ID.
   * @param cellID Integer cell identifier.
   * @return Assembly identifier string or empty string if not found.
   */
  TString GetAssemblyID(int /**/) const;

  /**
   * @brief Return the assembly identifier for a given row/column/layer/module.
   * @param row Row index.
   * @param col Column index.
   * @param lay Layer index.
   * @param mod Module index.
   * @return Assembly identifier string.
   */
  TString GetAssemblyID(int /**/, int /**/, int /**/, int /**/) const;

  /**
   * @brief Convert a readout-board/channel pair to a cell ID.
   * @param roboard Readout unit identifier.
   * @param roch Readout channel identifier.
   * @return Cell ID or -1 if not found.
   */
  int     GetCellID    (int /**/, int /**/) const;

  /**
   * @brief Construct a cell ID from row/column/layer/module coordinates.
   * @param row Row index.
   * @param col Column index.
   * @param lay Layer index.
   * @param mod Module index.
   * @return Cell ID.
   */
  int     GetCellID    (int /**/, int /**/, int /**/, int /**/)const;

  /** @brief Return the column index for a cell ID. */
  int     GetColumn    (int /**/) const;

  /** @brief Return the channel number within a layer for a given cell. */
  int     GetChannelInLayer(int /**/) const;

  /** @brief Return the channel number within a layer taking detector configuration into account. */
  int     GetChannelInLayerFull(int cellID /**/, DetConf::Type type = DetConf::Type::Unset ) const;

  /** @brief Return the maximum channel count within a full layer. */
  int     GetMaxChannelInLayerFull(void) const;

  /** @brief Count the number of active detector layers. */
  int     GetNActiveLayers(void) const;

  /** @brief Count the number of active cells in the setup. */
  int     GetNActiveCells(void) const;

  /** @brief Return the layer number for a cell ID. */
  int     GetLayer     (int /**/) const;

  /** @brief Return the module number for a cell ID. */
  int     GetModule    (int /**/) const;

  /** @brief Return the X position of the module center. */
  double  GetModuleX   (int /**/) const;

  /** @brief Return the Y position of the module center. */
  double  GetModuleY   (int /**/) const;

  /** @brief Return the readout channel for a cell ID. */
  int     GetROchannel (int /**/) const;

  /** @brief Return the readout channel for explicit geometry coordinates. */
  int     GetROchannel (int /**/, int /**/, int /**/, int /**/) const;

  /** @brief Return the readout unit for a cell ID. */
  int     GetROunit    (int /**/) const;

  /** @brief Return the readout unit for explicit geometry coordinates. */
  int     GetROunit    (int /**/, int /**/, int /**/, int /**/) const;

  /** @brief Return the row index for a cell ID. */
  int     GetRow       (int /**/) const;

  /** @brief Return the total number of configured channels. */
  int     GetTotalNbChannels(void) const;

  /** @brief Return the maximum layer index. */
  int     GetNMaxLayer  (void) const;

  /** @brief Return the maximum row index. */
  int     GetNMaxRow    (void) const;

  /** @brief Return the maximum column index. */
  int     GetNMaxColumn (void) const;

  /** @brief Return the maximum module index. */
  int     GetNMaxModule (void) const;

  /** @brief Return the maximum readout unit number. */
  int     GetNMaxROUnit (void) const;

  /** @brief Return the maximum number of KCUs. */
  int     GetNMaxKCUs   (void) const;

  /** @brief Return the maximum readout channel index over the full setup. */
  int     GetAbsNMaxROChannel(void) const;

  /** @brief Return the maximum cell ID currently defined. */
  int     GetMaxCellID (void) const;

  /** @brief Return the number of layers covered by the segment at a given layer index. */
  int     GetLayersInSegmentFromLayer(int) const;

  /** @brief Return the segment depth for a cell ID. */
  int     GetLayersInSegment(int) const;

  /** @brief Return the physical thickness of the segment containing a cell. */
  double  GetSegmentDepth(int cellID) const;

  /** @brief Return true if all segments have the same depth. */
  bool    HasSameSegmentDepth() const;
   
  /** @brief Return a human-readable text description of a cell ID. */
  TString DecodeCellID(int /**/) const;

  /** @brief Return the X position of a cell. */
  double  GetX         (int /**/) const;

  /** @brief Return the Y position of a cell. */
  double  GetY         (int /**/) const;

  /** @brief Return the Z position of a cell. */
  double  GetZ         (int /**/) const;

  /**
   * @brief Initialize the setup from a geometry text file.
   * @param file Path to the detector mapping file.
   * @param debug Verbosity level for diagnostic output.
   * @return true if initialization succeeded, false otherwise.
   */
  bool    Initialize   (TString, int);

  /**
   * @brief Initialize the setup from a RootSetupWrapper payload.
   * @param rsw Wrapped setup data source.
   * @return true if initialization succeeded.
   */
  bool    Initialize   (RootSetupWrapper&);

  /** @brief Return whether the setup has been initialized. */
  bool    IsInit       (void) const;

  /** @brief Check whether a layer is active in the requested module. */
  bool    IsLayerOn     (int /**/, int /**/) const;

  /** @brief Check whether an ASIC is active. */
  bool    IsAsicOn      (int /**/) const;

  /** @brief Return the minimum X coordinate in the setup. */
  float     GetMinX       (void) const;

  /** @brief Return the maximum X coordinate in the setup. */
  float     GetMaxX       (void) const;

  /** @brief Return the minimum Y coordinate in the setup. */
  float     GetMinY       (void) const;

  /** @brief Return the maximum Y coordinate in the setup. */
  float     GetMaxY       (void) const;

  /** @brief Return the minimum Z coordinate in the setup. */
  float     GetMinZ       (void) const;

  /** @brief Return the maximum Z coordinate in the setup. */
  float     GetMaxZ       (void) const;

  /** @brief Return the nominal cell width. */
  float     GetCellWidth  (void) const;

  /** @brief Return the nominal cell height. */
  float     GetCellHeight (void) const;

  /** @brief Return the nominal cell depth. */
  float     GetCellDepth  (void) const;

  /** @brief Return bin edges for Z-binning across the setup. */
  std::vector<double> GetZBinEdges(void) const;
 
  /** @brief Detect the detector configuration type from the current geometry. */
  DetConf::Type GetDetectorConfig(void) const;

  /** @brief Return the absolute maximum row count for a given detector configuration. */
  int GetAbsMaxRowsSetup( DetConf::Type);

  /** @brief Return the absolute maximum column count for a given detector configuration. */
  int GetAbsMaxColumnsSetup( DetConf::Type);

  /**
   * @brief Return the absolute column index for a cell under a detector configuration.
   * @param cellID Cell ID to convert.
   * @param type Detector configuration override.
   * @return Absolute column index.
   */
  int GetAbsColumn(int cellID /**/, DetConf::Type type = DetConf::Type::Unset ) const;

  /**
   * @brief Return the absolute row index for a cell under a detector configuration.
   * @param cellID Cell ID to convert.
   * @param type Detector configuration override.
   * @return Absolute row index.
   */
  int GetAbsRow(int cellID /**/, DetConf::Type type = DetConf::Type::Unset ) const;
   
  /** @brief Check whether a cell ID is contained in the current setup. */
  bool ContainedInSetup(int /**/) const;
  friend class RootSetupWrapper;
  
 private:
  static Setup* instancePtr;
  Setup(){}
  ~Setup() {}
  
  
  bool isInit=false;
  //key is CellID
  std::map<int, TString> assemblyID;
  std::map<int, int>     ROunit;
  std::map<int, int>     ROchannel;
  std::map<int, int>     Board;
  //key is module number
  std::map<int, std::pair<float,float>> ModPos;
  //Inverse mapping
  std::map< std::pair<int, int>, int> CellIDfromRO;
  // key is layerNr
  std::map<int,int>     SegmentSum;
  int nMaxLayer;
  int nMaxRow;
  int nMaxColumn;
  int nMaxModule;
  int nMaxROUnit;
  int maxCellID;
  float cellW = 5.;/*cm, width*/
  float cellH = 5.;/*cm, height*/
  float cellD = 2.;/*cm, depth*/
  int sumOpt  = 0;  
  
  ClassDef(Setup,4)
};


#endif
