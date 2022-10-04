//
// Created by ilker on 6/28/22.
//

#ifndef NEXUS_CRABANALYSISSTEPPINGACTION_H
#define NEXUS_CRABANALYSISSTEPPINGACTION_H

#include <G4UserSteppingAction.hh>
#include <globals.hh>
#include <map>
#include <G4GenericMessenger.hh>
#include "G4String.hh"

class G4Step;
namespace nexus{
    class CRABAnalysisSteppingAction: public  G4UserSteppingAction{
        public:
            // Constructor
            CRABAnalysisSteppingAction();
            //Destructor
            ~CRABAnalysisSteppingAction();

             virtual void UserSteppingAction(const G4Step*);

        private:
            typedef std::map<G4String, int> detectorCounts;
            typedef std::map<G4int, int> PhotonCount;
            typedef std::map<G4int, int> ieCount;


            typedef std::map<G4String,std::vector<double>> DepositedEnergy;
            typedef std::map<G4String,detectorCounts> DetailedPhotonCounts;
            typedef std::map<G4String,int> DetailedElectronCounts;
            typedef std::map<G4String,int> ProducedPhotonCount;
            detectorCounts my_counts_;
            detectorCounts ObservedPhotons;
            G4double Bi210_FinalTime;
            G4double Alpha_FinalTime;
            G4int TempTrackID;
            DetailedPhotonCounts DPhotonCounts;
            DetailedPhotonCounts DAbsPhotonCounts;
            ProducedPhotonCount DTotalPhotonCounts;
            DetailedElectronCounts DElectronCounts;

            G4String TempName;
            std::vector<G4String> Names;
            PhotonCount Photons;
            PhotonCount Parents;
            int TotalPhotons;
            int TotalIonizationElectron;
            DepositedEnergy PhotonEnergy_;

            G4GenericMessenger *msg_;
            G4String filePath_;
            int NumEvents_;
            bool SavetoFile_;
            bool DisplayPhotonEnergy_;
            G4String FileName_;
            bool isLead210Decay_;


    };

} //namespace nexus
#endif //NEXUS_CRABANALYSISSTEPPINGACTION_H
