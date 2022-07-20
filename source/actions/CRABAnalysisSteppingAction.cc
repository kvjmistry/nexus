//
// Created by ilker on 6/28/22.
//

#include "CRABAnalysisSteppingAction.h"
#include "FactoryBase.h"

#include <G4Step.hh>
#include <G4SteppingManager.hh>
#include <G4ProcessManager.hh>
#include <G4OpticalPhoton.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4VPhysicalVolume.hh>
#include "FileHandling.h"

using namespace nexus;

REGISTER_CLASS(CRABAnalysisSteppingAction, G4UserSteppingAction)

CRABAnalysisSteppingAction::CRABAnalysisSteppingAction(): G4UserSteppingAction(),msg_(0),filePath_(""),SavetoFile_(false),NumEvents_(0),FileName_("photoncount.txt")
{
    msg_=new G4GenericMessenger(this,"/Actions/CRABAnalysisSteppingAction/");
    msg_->DeclareProperty("FilePath",filePath_,"This is the path for saving some counts to text file..");
    msg_->DeclareProperty("FileSave",SavetoFile_,"Save Counts to File");
    msg_->DeclareProperty("FileName",FileName_,"FileName To save count information");


}



CRABAnalysisSteppingAction::~CRABAnalysisSteppingAction()
{
    FileHandling *f1=new FileHandling();
    G4double total_counts = 0;
    detectorCounts::iterator it = my_counts_.begin();
    DepositedEnergy ::iterator depit=PhotonEnergy_.begin();

    while (it != my_counts_.end()) {
        G4cout << "Detector " << it->first << ": " << it->second << " counts" << G4endl;
        if(SavetoFile_){
            G4String str;
            str=it->first + "," + to_string(it->second)+","+ to_string(NumEvents_) ;
            G4String Path;
            if(filePath_.empty())
                Path=FileName_;
            else Path=filePath_+"/"+FileName_;
            f1->SaveToTextFile(Path,"Detector,Counts,Total",str);
        }
        if(DisplayPhotonEnergy_){
            for(int i=0;i<PhotonEnergy_[it->first].size();i++){
                G4cout << "Detector " << it->first << ": " << PhotonEnergy_[it->first][i] << " Energy" << G4endl;
            }
        }

        total_counts += it->second;
        it ++;
    }

    int count=0;

    G4cout << "TOTAL COUNTS: " << total_counts << G4endl;
}



void CRABAnalysisSteppingAction::UserSteppingAction(const G4Step* step)
{
    G4ParticleDefinition* pdef = step->GetTrack()->GetDefinition();

    //Check whether the track is an optical photon
    if (pdef != G4OpticalPhoton::Definition()) return;

    /*
    // example of information one can access about optical photons

    G4Track* track = step->GetTrack();
    G4int pid = track->GetParentID();
    G4int tid = track->GetTrackID();
    G4StepPoint* point1 = step->GetPreStepPoint();
    G4StepPoint* point2 = step->GetPostStepPoint();
    G4TouchableHandle touch1 = point1->GetTouchableHandle();
    G4TouchableHandle touch2 = point2->GetTouchableHandle();
    G4String vol1name = touch1->GetVolume()->GetName();
    G4String vol2name = touch2->GetVolume()->GetName();

    G4String proc_name = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    G4int copy_no = step->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(1);
    */

    // Retrieve the pointer to the optical boundary process.
    // We do this only once per run defining our local pointer as static.
    static G4OpBoundaryProcess* boundary = 0;
    if (!boundary) { // the pointer is not defined yet
        // Get the list of processes defined for the optical photon
        // and loop through it to find the optical boundary process.
        G4ProcessVector* pv = pdef->GetProcessManager()->GetProcessList();
        for (size_t i=0; i<pv->size(); i++) {
            if ((*pv)[i]->GetProcessName() == "OpBoundary") {
                boundary = (G4OpBoundaryProcess*) (*pv)[i];
                break;
            }
        }
    }

    if (step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {
        if (boundary->GetStatus() == Detection ){
            G4String detector_name = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetName();
            //G4cout << "##### Sensitive Volume: " << detector_name << G4endl;
            detectorCounts::iterator it = my_counts_.find(detector_name);
            DepositedEnergy ::iterator deit = PhotonEnergy_.find(detector_name);

            if(DisplayPhotonEnergy_){
                if(deit!=PhotonEnergy_.end())   PhotonEnergy_[it->first].push_back(step->GetTotalEnergyDeposit()/CLHEP::eV);
                else  PhotonEnergy_[detector_name].push_back(step->GetTotalEnergyDeposit()/CLHEP::eV);
            }


            //PhotonCounting
            if (it != my_counts_.end())  my_counts_[it->first] += 1;

            else my_counts_[detector_name] = 1;
            NumEvents_++;




        }

    }

    return;
}