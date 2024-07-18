// ----------------------------------------------------------------------------
// nexus | ElecPairGenerator.h
//
// This generator simulates two electrons from the same vertex,
// with total kinetic energy and direction settable by parameter.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef ELEC_PAIR_GEN_H
#define ELEC_PAIR_GEN_H

#include <G4VPrimaryGenerator.hh>
#include <vector>

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;


namespace nexus {

  class GeometryBase;

  // Structure to hold the values
  struct Data {
      double x1_dir, y1_dir, z1_dir, e1;
      double x2_dir, y2_dir, z2_dir, e2;
  };

  class ElecPairGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    ElecPairGenerator();
    /// Destructor
    ~ElecPairGenerator();

    /// This method is invoked at the beginning of the event. It sets
    /// a primary vertex (that is, a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);
    
    // Get an event from the file
    Data GetEvent();

    // Open the file
    void OpenFile(const std::string& filename);

  private:
    G4GenericMessenger* msg_;

    G4ParticleDefinition* particle_definition_;

    G4String dist_file_; ///< Name of file with the energy and direcional spectra
    std::vector<std::string> lines_; // lines from the file

    G4bool bInitialize_;

    const GeometryBase* geom_; ///< Pointer to the detector geometry

    G4String region_;

  };

} // end namespace nexus

#endif
