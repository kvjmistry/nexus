//
// Geometry of a basic atmospheric chamber
//

#ifndef NEXUS_ATPC_H
#define NEXUS_ATPC_H
#include "GeometryBase.h"
class G4GenericMessenger;

namespace nexus { class BoxPointSampler; }

namespace nexus {
    class ATPC: public GeometryBase
    {
        public:
            /// Constructor
            ATPC();
            /// Destructor
            ~ATPC();

            /// Return vertex within region <region> of the chamber

            virtual void Construct();
            virtual G4ThreeVector GenerateVertex(const G4String& region) const;

        private:
            /// Messenger for the definition of control commands
            G4GenericMessenger* msg_;
            G4double Lab_size;
            G4double cube_size;
            G4double chamber_thickn;
            G4double gas_pressure_;
            G4ThreeVector vtx_;
            G4ThreeVector vertex;
            G4double max_step_size_;
            BoxPointSampler* active_gen_;
            G4String gastype_;
            
            void ConstructLab();
            void PlaceVolumes();
            void AssignVisuals();
            void PrintParam();

    };

} // end namespace nexus


#endif //NEXUS_ATPCV2_H
