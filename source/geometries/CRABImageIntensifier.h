//
// Created by ilker on 1/14/23.
//

#ifndef NEXUS_CRABIMAGEINTENSIFIER_H
#define NEXUS_CRABIMAGEINTENSIFIER_H

#include "GeometryBase.h"

class G4GenericMessenger;
namespace  nexus {


    class CRABImageIntensifier: public GeometryBase{

    public:
        // Construct
        CRABImageIntensifier();


        // Destruct Geometry
        ~CRABImageIntensifier();


        /// Define Volumes of the geometry
        virtual void Construct();
        // Generate a vertex within a given region of geometry
       virtual G4ThreeVector GenerateVertex(const G4String &region) const;



    private:
        G4GenericMessenger * msg_;
        long PhotonGain;
        bool fvisual;
    };

} // end namespace nexus

#endif //NEXUS_CRABIMAGEINTENSIFIER_H
