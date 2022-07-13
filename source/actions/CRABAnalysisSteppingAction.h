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
            typedef std::map<G4String,std::vector<double>> DepositedEnergy;
            detectorCounts my_counts_;
            DepositedEnergy PhotonEnergy_;

            G4GenericMessenger *msg_;
            G4String filePath_;
            int NumEvents_;
            bool SavetoFile_;
            bool DisplayPhotonEnergy_;
            G4String FileName_;

    };

} //namespace nexus
#endif //NEXUS_CRABANALYSISSTEPPINGACTION_H
