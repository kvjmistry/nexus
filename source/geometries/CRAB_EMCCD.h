//
// Created by ilker on 1/14/23.
//

#ifndef NEXUS_CRAB_EMCCD_H
#define NEXUS_CRAB_EMCCD_H
#include "GeometryBase.h"

class G4GenericMessenger;
namespace nexus {
    class CRAB_EMCCD : public GeometryBase{
    public:
        // Construct EMCCD_Class
        CRAB_EMCCD();

        // Destruct
        ~CRAB_EMCCD();

        // Define the volumes of the geometry
        virtual void Construct ();

        // Generate a vertex within a given region of the geometry
        virtual G4ThreeVector GenerateVertex(const G4String &region) const;


    private:
        G4GenericMessenger *msg_;
        bool fvisual;

    };

}

#endif //NEXUS_CRAB_EMCCD_H
