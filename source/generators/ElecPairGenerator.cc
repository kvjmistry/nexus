// ----------------------------------------------------------------------------
// nexus | ElecPairGenerator.cc
//
// This generator simulates an electron and a positron from the same vertex,
// with total kinetic energy settable by parameter.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ElecPairGenerator.h"
#include "GeometryBase.h"
#include "FactoryBase.h"
#include "RandomUtils.h"
#include "DetectorConstruction.h"

#include <G4GenericMessenger.hh>
#include <G4ParticleDefinition.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>
#include <G4OpticalPhoton.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

using namespace nexus;
using namespace CLHEP;

REGISTER_CLASS(ElecPairGenerator, G4VPrimaryGenerator)

ElecPairGenerator::ElecPairGenerator():
G4VPrimaryGenerator(), msg_(0), particle_definition_(0),
dist_file_(""), bInitialize_(false), geom_(0), event_index_(0), brandomindex_(false), verbosity_(true)
{
  msg_ = new G4GenericMessenger(this, "/Generator/ElecPair/",
    "Control commands of single-particle generator.");

    msg_->DeclareProperty("dist_file", dist_file_,
      "Name of the file containing kinematic information.");

  msg_->DeclareProperty("region", region_,
                        "Region of the geometry where the vertex will be generated.");


  msg_->DeclareProperty("randomindex", brandomindex_,
                        "Select a random index from file rather than from seed");

  msg_->DeclareProperty("verbosity", verbosity_,
                        "Choose to print the index from file");

  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();
}



ElecPairGenerator::~ElecPairGenerator()
{
  delete msg_;
}


void ElecPairGenerator::GeneratePrimaryVertex(G4Event* event)
{

  // Initialize the file
  if (!bInitialize_){

    OpenFile(dist_file_);
    bInitialize_ = true;
  }
  
  // Get a new event
  Data NewEvent = GetEvent();

  // std::cout << "Randomly selected data:" << std::endl;
  // std::cout << "x1_dir: " << NewEvent.x1_dir << ", y1_dir: " << NewEvent.y1_dir << ", z1_dir: " << NewEvent.z1_dir << ", e1: " << NewEvent.e1 << std::endl;
  // std::cout << "x2_dir: " << NewEvent.x2_dir << ", y2_dir: " << NewEvent.y2_dir << ", z2_dir: " << NewEvent.z2_dir << ", e2: " << NewEvent.e2 << std::endl;

  particle_definition_ =
    G4ParticleTable::GetParticleTable()->FindParticle("e-");

  // Generate an initial position for the particle using the geometry
  G4ThreeVector pos = geom_->GenerateVertex(region_);

  // Particle generated at start-of-event
  G4double time = 0.;

  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(pos, time);

  // Set the kinetic energy particle 1
  G4double kinetic_energy = NewEvent.e1*MeV;

  // Generate random direction by default
  G4ThreeVector _momentum_direction = G4ThreeVector(NewEvent.x1_dir, NewEvent.y1_dir, NewEvent.z1_dir);

    // Calculate cartesian components of momentum
  G4double mass   = particle_definition_->GetPDGMass();
  G4double energy = kinetic_energy + mass;
  G4double pmod = std::sqrt(energy*energy - mass*mass);
  G4double px = pmod * _momentum_direction.x();
  G4double py = pmod * _momentum_direction.y();
  G4double pz = pmod * _momentum_direction.z();

  // Create the new primary particle and set it some properties
  G4PrimaryParticle* particle =
    new G4PrimaryParticle(particle_definition_, px, py, pz);

  // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle);


  G4double kinetic_energy2 = NewEvent.e2*MeV;

  // Generate second electron
  G4ThreeVector _momentum_direction2 =  G4ThreeVector(NewEvent.x2_dir, NewEvent.y2_dir, NewEvent.z2_dir);

    // Calculate cartesian components of momentum
  G4double energy2 = kinetic_energy2 + mass;
  G4double pmod2 = std::sqrt(energy2*energy2 - mass*mass);
  G4double px2 = pmod2 * _momentum_direction2.x();
  G4double py2 = pmod2 * _momentum_direction2.y();
  G4double pz2 = pmod2 * _momentum_direction2.z();

  // Create the new primary particle and set it some properties
  G4PrimaryParticle* particle2 =
    new G4PrimaryParticle(particle_definition_, px2, py2, pz2);

    // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle2);
  event->AddPrimaryVertex(vertex);
}


void ElecPairGenerator::OpenFile(const std::string& filename){
  
  std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
  }

  std::string line;

    // Skip the header
    std::getline(file, line);

    // Read the rest of the lines
    while (std::getline(file, line)) {
        lines_.push_back(line);
    }

    file.close();

    // Check if we have any data lines
    if (lines_.empty()) {
        throw std::runtime_error("No data lines in file");
    }

    std::cout << "Successfully read in the file" << std::endl;

}


Data ElecPairGenerator::GetEvent(){
    
    // std::cout << "The SEED is: "<< CLHEP::HepRandom::getTheSeed() << "  event index is: " << event_index_ << std::endl;

    // Seed the random number generator
    // Randomly select a line
    G4int randomIndex =  G4UniformRand() * lines_.size();
    
    // This will use a seed sequential from the line
    if (!brandomindex_)
      randomIndex = CLHEP::HepRandom::getTheSeed() + event_index_;
    
    if (verbosity_)
      std::cout << "The random index is: " << randomIndex << std::endl;
    
    std::istringstream selectedLine(lines_[randomIndex]);

    Data data;
    char comma;

    // Parse the selected line
    selectedLine >> data.x1_dir >> comma >> data.y1_dir >> comma >> data.z1_dir >> comma >> data.e1 >> comma
                 >> data.x2_dir >> comma >> data.y2_dir >> comma >> data.z2_dir >> comma >> data.e2;

    event_index_++;

    return data;
}
