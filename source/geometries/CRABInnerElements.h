//
// Created by ilker on 6/3/22.
//

#ifndef NEXUS_CRABINNERELEMENTS_H
#define NEXUS_CRABINNERELEMENTS_H


#include "GeometryBase.h"
#include <G4ThreeVector.hh>
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;

namespace nexus{
    class CRABInnerElements:public GeometryBase
    {
    public:
        //Constructor
        CRABInnerElements();
        //Destructor
        ~CRABInnerElements();

        // Set the logical Volume for the inner Elements
        void SetMotherLogicalVolume(G4LogicalVolume *mother_logic);
        void SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys);
        /// Generate a vertex within a given region of the geometry
        G4ThreeVector GenerateVertex(const G4String& region) const;

        /// Builder
        void Construct();

    private:
        G4LogicalVolume* mother_logic_;
        G4VPhysicalVolume* mother_phys_;
        G4Material* gas_;
        G4double pressure_;
        //Detector parts here FieldCage
    };
} // end namespace nexus

#endif //NEXUS_CRABINNERELEMENTS_H
