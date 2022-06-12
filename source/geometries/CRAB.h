//
// Created by ilker on 9/2/21.
//

#ifndef NEXUS_CRAB_H
#define NEXUS_CRAB_H
#include "GeometryBase.h"
#include "PmtR7378A.h"
class G4GenericMessenger;

namespace nexus {

    class CRAB: public GeometryBase
    {
        public:
            /// Constructor
            CRAB();
            /// Destructor
            ~CRAB();

            /// Return vertex within region <region> of the chamber

            virtual void Construct();
            virtual G4ThreeVector GenerateVertex(const G4String& region) const;

        private:
            /// Messenger for the definition of control commands
            G4GenericMessenger* msg_;
            G4double Lab_size;
            G4double chamber_diam   ;
            G4double chamber_length ;
            G4double chamber_thickn ;
            G4double SourceEn_offset ;
            G4double SourceEn_diam   ;
            G4double SourceEn_length ;
            G4double SourceEn_thickn ;
            G4double SourceEn_holedia ;
            G4double gas_pressure_;
            G4ThreeVector vtx_;
            G4double Active_diam;
            G4double Active_length;
            G4double sc_yield_;
            G4double e_lifetime_;
            G4ThreeVector vertex;
            PmtR7378A *pmt1_;
            PmtR7378A *pmt2_;
            G4double sapphire_window_thickness_;
            G4double sapphire_window_diam_;
            G4double pmt_hole_length_ ;
            G4double wndw_ring_stand_out_;
            G4double pedot_coating_thickness_;
            G4double optical_pad_thickness_;
            G4double pmt_base_diam_;
            G4double pmt_base_thickness_;
            G4double efield_;


            void ConstructLab();
            void PlaceVolumes();
            void AssignVisuals();
            void PrintParam();


    };

} // end namespace nexus


#endif //NEXUS_CRAB_H
