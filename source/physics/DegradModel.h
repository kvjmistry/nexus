/*
 * DegradModel.h
 *
 *  Created on: Apr 9, 2014
 *      Author: dpfeiffe
 */

#ifndef DEGRADMODEL_H_
#define DEGRADMODEL_H_

#include "IonizationSD.h"

#include <G4VFastSimulationModel.hh>
#include <G4PhysicsTable.hh>
#include <G4PhysicsOrderedFreeVector.hh>
#include <G4RotationMatrix.hh>

namespace nexus{
    class DegradModel : public G4VFastSimulationModel {
     public:
        //-------------------------
        // Constructor, destructor
        //-------------------------
        DegradModel(G4String, G4Region*, G4double GasPressure_, G4double Efield_);
        G4ThreeVector GetTrackEndPoint(G4int trk_id);
        G4double GetTrackEndTime(G4int trk_id);
        G4int GetCurrentTrackIndex(G4int trk_id); // Get the index of the track in the vector
        G4double GetAvgIoniEnergy(G4int trk_id);
        G4int GetTotIonizations(G4int trk_id);
        G4double GetTrackLength(G4int trk_id);
        ~DegradModel();


        virtual G4bool IsApplicable(const G4ParticleDefinition&);
        virtual G4bool ModelTrigger(const G4FastTrack&);
        virtual void DoIt(const G4FastTrack&, G4FastStep&);
        void Reset();
        void AddTrack(G4int trk_id);

        void AddSensitiveVolume(IonizationSD* sd, G4String name);

        private:
        void GetElectronsFromDegrad(G4FastStep& fastStep, G4ThreeVector G4Pos, G4ThreeVector G4Dir, G4double G4Time, G4int trk_id);
        void SetTrackEndPoint(G4ThreeVector pos, G4double time, G4int trk_index);
        void SetNioni(G4int Ne, G4int trk_index); // Set the number of ionization particles
        void AddTrackLength(G4int trk_id); // Get track length
        G4double GetScintTime(); // Get timing delay from scintillaiton
        
        void BuildThePhysicsTable();
        void ComputeCumulativeDistribution(const G4PhysicsOrderedFreeVector&, G4PhysicsOrderedFreeVector&);
        void GetPhotonPol(G4ThreeVector &momentum, G4ThreeVector &polarization);

        G4int run_degrad();

        G4double fPrimKE;      // Primary kinetic energy of the particle
        G4double GasPressure_; // bar
        G4double Efield_;      // V/cm  

        G4int event_id_;
        std::vector<G4double>      end_times_;
        std::vector<G4int>         N_ioni_;
        std::vector<G4double>      ke_vec_;
        std::vector<G4double>      trk_len_vec_;
        std::vector<G4ThreeVector> track_end_pos_;
        std::vector<std::vector<G4double>> time_vec_;
        G4bool degrad_status_; // Checks if degrad has been run
        G4RotationMatrix RotMatrix_;

        std::vector<G4int> track_ids_;

        // Scintillation timing 
        G4double slow_comp_; // ns
        G4double slow_prob_; // %
        G4double fast_comp_; // ns
        G4double fast_prob_; // %

        // The sensitive detector to fill hits into
        IonizationSD *SD_active_, *SD_buffer_;

        G4PhysicsTable* theFastIntegralTable_;
        G4PhysicsOrderedFreeVector* spectrum_integral;

        G4bool verbose_;

    };
}
#endif /* DegradModel_H_ */
