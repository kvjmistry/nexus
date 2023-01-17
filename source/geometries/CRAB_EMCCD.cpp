//
// Created by ilker on 1/14/23.
//

#include "CRAB_EMCCD.h"


namespace nexus {

    using namespace CLHEP;

    //Construct
    CRAB_EMCCD::CRAB_EMCCD() :GeometryBase(){

    }

    // Destruct
    CRAB_EMCCD::~CRAB_EMCCD() noexcept {}

    void CRAB_EMCCD::Construct() {

    }

    G4ThreeVector CRAB_EMCCD::GenerateVertex(const G4String &region) const {
        return G4ThreeVector (0,0,0);
    }
}