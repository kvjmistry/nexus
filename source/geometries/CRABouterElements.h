//
// Created by ilker on 6/3/22.
//

#ifndef NEXUS_CRABOUTERELEMENTS_H
#define NEXUS_CRABOUTERELEMENTS_H


#include "GeometryBase.h"
#include <G4ThreeVector.hh>
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4GenericMessenger;

namespace nexus {
    class CRABouterElements : public GeometryBase {
    public:
        //Constructor
        CRABouterElements();

        //Destructor
        ~CRABouterElements();

        // Sets the Logical Volume where Inner Elements will be placed
        void SetMotherLogicalVolume(G4LogicalVolume *mother_logic);

        //
        void SetMotherPhysicalVolume(G4VPhysicalVolume *mother_phys);

        /// Generate a vertex within a given region of the geometry
        G4ThreeVector GenerateVertex(const G4String &region) const;

        /// Builder
        void Construct();

    private:
        G4LogicalVolume *mother_logic_;
        G4VPhysicalVolume *mother_phys_;
        G4Material *gas_;
        G4double pressure_;
        G4double temperature_;

        // Messenger for the definition of control commands
        G4GenericMessenger* msg_;

        // Detector parts    };
    };
}


#endif //NEXUS_CRABOUTERELEMENTS_H
