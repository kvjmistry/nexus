// ----------------------------------------------------------------------------
// nexus | XeSphere.cc
//
// Sphere filled with xenon.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "XeSphere.h"

#include "SpherePointSampler.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "FactoryBase.h"

#include <G4GenericMessenger.hh>
#include <G4Orb.hh>
#include <G4Box.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4VUserDetectorConstruction.hh>
#include <G4UserLimits.hh>
#include "G4LogicalVolumeStore.hh"
#include "Visibilities.h"

#include <CLHEP/Units/SystemOfUnits.h>

using namespace nexus;
using namespace CLHEP;

REGISTER_CLASS(XeSphere, GeometryBase)

namespace nexus {

  XeSphere::XeSphere():
    GeometryBase(), liquid_(true), pressure_(STP_Pressure),
    radius_(1.*m), sphere_vertex_gen_(0)
  {
    msg_ = new G4GenericMessenger(this, "/Geometry/XeSphere/",
      "Control commands of geometry XeSphere.");

    msg_->DeclareProperty("LXe", liquid_,
      "Build the sphere with liquid xenon.");

    G4GenericMessenger::Command& pressure_cmd =
      msg_->DeclareProperty("pressure", pressure_,
      "Set pressure for gaseous xenon (if selected).");
    pressure_cmd.SetUnitCategory("Pressure");
    pressure_cmd.SetParameterName("pressure", false);
    pressure_cmd.SetRange("pressure>0.");

    G4GenericMessenger::Command& radius_cmd =
      msg_->DeclareProperty("radius", radius_, "Radius of the xenon sphere.");
    radius_cmd.SetUnitCategory("Length");
    radius_cmd.SetParameterName("radius", false);
    radius_cmd.SetRange("radius>0.");

    // Create a vertex generator for a sphere
    // sphere_vertex_gen_ = new SpherePointSampler(radius_, 0.);
    box_vertex_gen_ = new BoxPointSampler(3.5*m, 3.5*m,3.5*m, 0);
  }



  XeSphere::~XeSphere()
  {
    delete sphere_vertex_gen_;
    delete msg_;
  }



  void XeSphere::Construct()
  {
    G4String name = "XE_SPHERE";

    // Define solid volume as a sphere
    // G4Orb* sphere_solid = new G4Orb(name, radius_);
    
    G4double gas_dimentions = 5.7*m;
    G4double chamber_thick = 10*cm;

    G4Box* world = new G4Box("World", radius_/2.0,radius_/2.0,radius_/2.0);
    G4LogicalVolume* world_logic = new G4LogicalVolume(world, G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"), name);
    
    G4Box* gas = new G4Box("GAS", gas_dimentions/2, gas_dimentions/2, gas_dimentions/2);

    G4Box* chamber = new G4Box("Chamber", (gas_dimentions+chamber_thick)/2, (gas_dimentions+chamber_thick)/2, (gas_dimentions+chamber_thick)/2);
    G4LogicalVolume *chamber_logic = new G4LogicalVolume(chamber, materials::Steel(), "CHAMBER");


    // Define the material (LXe or GXe) for the sphere.
    // We use for this the NIST manager or the nexus materials list.
    G4Material* xenon = 0;
    if (liquid_)
      xenon = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    else
      xenon = materials::GXe(pressure_);

    // Define the logical volume of the sphere using the material
    // and the solid volume defined above
    G4LogicalVolume* sphere_logic = new G4LogicalVolume(gas, xenon, name);
    GeometryBase::SetLogicalVolume(world_logic);

    // Set the logical volume of the sphere as an ionization
    // sensitive detector, i.e. position, time and energy deposition
    // will be stored for each step of any charged particle crossing
    // the volume.
    IonizationSD* ionizsd = new IonizationSD("/XE_SPHERE");
    G4SDManager::GetSDMpointer()->AddNewDetector(ionizsd);
    sphere_logic->SetSensitiveDetector(ionizsd);


    G4VPhysicalVolume *chamber_phys = new G4PVPlacement(0, G4ThreeVector(0., 0., 0), chamber_logic, chamber->GetName(), world_logic, false, 0, false);
    G4VPhysicalVolume *gas_phys = new G4PVPlacement(0, G4ThreeVector(0., 0., 0), sphere_logic, gas->GetName(), chamber_logic, false, 0, false);
    

     // Chamber
    G4LogicalVolumeStore *lvStore = G4LogicalVolumeStore::GetInstance();


    // Gas
    G4LogicalVolume *Gas = lvStore->GetVolume("XE_SPHERE");
    G4VisAttributes *GasVa = new G4VisAttributes(nexus::WhiteAlpha());
    GasVa->SetForceCloud(false);
    Gas->SetVisAttributes(GasVa);
    
    //Chamber
    G4LogicalVolume *Chamber = lvStore->GetVolume("CHAMBER");
    G4VisAttributes *ChamberVa = new G4VisAttributes(nexus::TitaniumGreyAlpha());
    ChamberVa->SetForceSolid(true);
    // Chamber->SetVisAttributes(ChamberVa);


    // Limit the step size in this volume for better tracking precision
    sphere_logic->SetUserLimits(new G4UserLimits(0.1*mm));
  }
  



  G4ThreeVector XeSphere::GenerateVertex(const G4String& region) const
  {
    // return sphere_vertex_gen_->GenerateVertex(region);
    return box_vertex_gen_->GenerateVertex(region);
  }


} // end namespace nexus
