// ----------------------------------------------------------------------------
// nexus | AnalysisSteppingAction.h
//
// This class allows the user to print the total number of photons detected by
// all kinds of photosensors at the end of the run.
// It also shows examples of information that can be accessed at the stepping
// level, so it is useful for debugging.
//
// The  NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef ANALYSIS_STEPPING_ACTION_H
#define ANALYSIS_STEPPING_ACTION_H

#include <G4UserSteppingAction.hh>
#include <globals.hh>
#include <map>
#include <vector>

class G4Step;


namespace nexus {

  //  Stepping action to analyze the behaviour of optical photons

  class AnalysisSteppingAction: public G4UserSteppingAction
  {
  public:
    /// Constructor
    AnalysisSteppingAction();
    /// Destructor
    ~AnalysisSteppingAction();

    virtual void UserSteppingAction(const G4Step*);

    // Nested struct for a data point
    struct DataPoint {
        G4int    event_id;
        G4double x, y, z, energy; // ending of the photon energy/positions
        G4String mode, preVolumeName, postVolumeName;


        // Constructor for easy initialization
        DataPoint(G4int event_id, G4double x, G4double y, G4double z, G4double energy, G4String mode, G4String preVolumeName, G4String postVolumeName)
            : event_id (event_id), x(x), y(y), z(z), energy(energy), mode(mode), preVolumeName(preVolumeName), postVolumeName(postVolumeName) {}
    };

  private:
    typedef std::map<G4String, int> detectorCounts;
    detectorCounts my_counts_;
    std::vector<DataPoint> data_test; // Vector to store data points
  
    // Method to write all data points to the file
    void writeToFile();

  };

} // namespace nexus

#endif
