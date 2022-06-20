// ----------------------------------------------------------------------------
// nexus | PmtR7378A.h
//
// Geometry of the Hamamatsu R7378A photomultiplier.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef PMT_R7378A_H
#define PMT_R7378A_H

#include "GeometryBase.h"

#include <G4ThreeVector.hh>


namespace nexus {

  /// Geometry model for the Hamamatsu R7378A photomultiplier (PMT).

  class PmtR7378A: public GeometryBase
  {
  public:
    /// Constructor
    PmtR7378A();
    /// Destructor
    ~PmtR7378A();

    /// Returns the PMT diameter
    G4double Diameter() const;
    /// Returns the PMT length
    G4double Length() const;

    /// Sets a sensitive detector associated to the

    void Construct();

    void SetPMTName(G4String PMTName);
    G4String GetPMTName();


  private:
    G4double pmt_diam_, pmt_length_; ///< PMT dimensions

      G4String PmtName;

  };

  inline G4double PmtR7378A::Diameter() const { return pmt_diam_; }
  inline G4double PmtR7378A::Length() const { return pmt_length_; }
  inline void PmtR7378A::SetPMTName(G4String PMTName)  { PmtName=PMTName ; }
  inline G4String PmtR7378A::GetPMTName()  { return PmtName ; }



} // end namespace nexus

#endif
