//
// Geometry of a basic atmospheric chamber
//

#include "ATPC.h"
#include "Visibilities.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "IonizationSD.h"
#include "FactoryBase.h"
#include <G4SubtractionSolid.hh>
#include <G4GenericMessenger.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4PVParameterised.hh>
#include <G4PVReplica.hh>
#include <G4SDManager.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "XenonProperties.h"
#include "G4UnitsTable.hh"
#include "G4ExtrudedSolid.hh"

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <G4UserLimits.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include "BoxPointSampler.h"
#include "CylinderPointSampler.h"




namespace nexus{
    using namespace CLHEP;
    REGISTER_CLASS(ATPC, GeometryBase)


             ATPC::ATPC():
             GeometryBase(),
             msg_(nullptr),
             Lab_size(550. * m), cube_size (5.7 * m), chamber_thickn (50. * mm),
             vtx_(0,0,0), active_gen_(nullptr), cylinder_gen_(nullptr), cylinder_gen_flange1_(nullptr), cylinder_gen_flange2_(nullptr),cylinder_gen_active_(nullptr),
             max_step_size_(0.1*mm), 
             gas_pressure_(1. * bar),
             gastype_("xenon"),
             genvol_("Cu"),
             detgeom_("cube")

    {
        msg_ = new G4GenericMessenger(this, "/Geometry/ATPC/","Control commands of geometry of ATPC TPC");
        G4GenericMessenger::Command&  Pressure_cmd =msg_->DeclarePropertyWithUnit("gas_pressure","bar",gas_pressure_,"Pressure of Gas");
        Pressure_cmd.SetParameterName("XenonPressure", false);

        G4GenericMessenger::Command&  cube_size_cmd =msg_->DeclarePropertyWithUnit("cube_size","m",cube_size,"Cube Side Length");
        cube_size_cmd .SetParameterName("cubesize", false);

        G4GenericMessenger::Command&  chamber_thickn_cmd =msg_->DeclarePropertyWithUnit("chamber_thickn","cm",chamber_thickn,"Chamber Wall thickness");
        chamber_thickn_cmd .SetParameterName("chamberthickn", false);

        G4GenericMessenger::Command&  step_size_cmd =msg_->DeclarePropertyWithUnit("max_step_size","mm",max_step_size_,"The maximum step size");
        step_size_cmd .SetParameterName("max_step_size", false);

        msg_->DeclareProperty("gastype", gastype_, "The GAS to use in the detector");

        msg_->DeclareProperty("genvol", genvol_, "Decide whether to generate in the gas or Cu shielding");

        msg_->DeclareProperty("detgeom", detgeom_, "Decide whether to generate cube or cylinder");

    }

    ATPC::~ATPC()
    {

        delete msg_;
        delete active_gen_;
        delete cylinder_gen_;
        delete cylinder_gen_flange1_;
        delete cylinder_gen_flange2_;
        delete cylinder_gen_active_;
    }


    void ATPC::Construct(){

        // Materials
        // G4Material *GXe=G4NistManager::Instance()->FindOrBuildMaterial("G4_Xe");
        G4Material *GAS;
        
        if (gastype_ == "xenon"){
            std::cout << "Using Xenon! "<< gas_pressure_/bar << " bar"  << std::endl;
            GAS = materials::GXeEnriched(gas_pressure_, 293. * kelvin);
        }
        else if (gastype_ == "argon"){
            std::cout << "Using Argon! "<< gas_pressure_/bar << " bar"  << std::endl;
            GAS = materials::GAr(gas_pressure_, 293. * kelvin);
        }
        else 
            std::cout << "Error in specified gas" << std::endl;
        
        
        // G4Material *Steel=materials::Steel();
        G4Material *Cu = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

        //Constructing Lab Space
        G4String lab_name="LAB";
        G4Box * lab_solid_volume = new G4Box(lab_name,Lab_size/2.,Lab_size/2.,Lab_size/2.);
        G4LogicalVolume * lab_logic_volume= new G4LogicalVolume(lab_solid_volume,G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),lab_name) ;

        G4double outer_cube_size = cube_size/2. + chamber_thickn;
        
        G4LogicalVolume* gas_logic;
        if (detgeom_ == "cube"){
            G4cout << "Using Cube Geometry" << G4endl;
            // Vessel
            G4Box* outer_cube_solid = new G4Box("CHAMBER", outer_cube_size, outer_cube_size, outer_cube_size);
            G4LogicalVolume* outer_cube_logic = new G4LogicalVolume(outer_cube_solid, Cu, "CHAMBER");

            // Gas Volume
            G4Box* gas_solid = new G4Box("GAS", cube_size/2., cube_size/2., cube_size/2.);
            gas_logic = new G4LogicalVolume(gas_solid, GAS, "GAS");

            // Place the Volumes

            // LAB
            auto labPhysical= new G4PVPlacement(0,G4ThreeVector(),lab_logic_volume,lab_logic_volume->GetName(),0, false, 0, false);

            // Place the Outer Cu Cube (Chamber)
            G4VPhysicalVolume * chamber_phys = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), outer_cube_logic, outer_cube_solid->GetName(), lab_logic_volume, false, 0, false);

            // Xenon Gas in Active Area and Non-Active Area
            G4VPhysicalVolume * gas_phys= new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), gas_logic, gas_solid->GetName(),outer_cube_logic, false, 0, false);

            // VERTEX GENERATORS /////////////////////////////////////
            if (genvol_ == "Cu"){
                active_gen_ = new BoxPointSampler(cube_size/2.,
                                                cube_size/2.,
                                                cube_size/2.,
                                                chamber_thickn);
            }
            else {
                active_gen_ = new BoxPointSampler(cube_size/2.,
                                                cube_size/2.,
                                                cube_size/2.,
                                                0.);
            }
        }
        else {
            G4cout << "Using Cylinder Geometry" << G4endl;
            // Vessel

            G4Tubs* outer_cylinder_solid = new G4Tubs("CHAMBER", 0, outer_cube_size, outer_cube_size, 0, twopi);
            G4LogicalVolume* outer_cylinder_logic = new G4LogicalVolume(outer_cylinder_solid, Cu, "CHAMBER");

            // Gas Volume
            G4Tubs* gas_solid = new G4Tubs("GAS", 0, cube_size/2., cube_size/2., 0, twopi);
            gas_logic = new G4LogicalVolume(gas_solid, GAS, "GAS");

            // Place the Volumes

            // LAB
            auto labPhysical= new G4PVPlacement(0,G4ThreeVector(),lab_logic_volume,lab_logic_volume->GetName(),0, false, 0, false);

            // Place the Outer Cu Cube (Chamber)
            G4VPhysicalVolume * chamber_phys = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), outer_cylinder_logic, outer_cylinder_solid->GetName(), lab_logic_volume, false, 0, false);

            // Xenon Gas in Active Area and Non-Active Area
            G4VPhysicalVolume * gas_phys= new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), gas_logic, gas_solid->GetName(),outer_cylinder_logic, false, 0, false);

            // VERTEX GENERATORS /////////////////////////////////////
            cylinder_gen_active_  = new CylinderPointSampler(0., cube_size/2., cube_size/2., 0., twopi, nullptr, G4ThreeVector (0,0,0));
            cylinder_gen_         = new CylinderPointSampler(cube_size/2., outer_cube_size, outer_cube_size, 0., twopi, nullptr, G4ThreeVector (0,0,0));
            cylinder_gen_flange1_ = new CylinderPointSampler(0, cube_size/2., chamber_thickn/2.0, 0., twopi, nullptr, G4ThreeVector (0,0,cube_size/2.+chamber_thickn/2.0));
            cylinder_gen_flange2_ = new CylinderPointSampler(0, cube_size/2., chamber_thickn/2.0, 0., twopi, nullptr, G4ThreeVector (0,0,-cube_size/2.-chamber_thickn/2.0));

        }

        // Xenon Gas in Active Area and Non-Active Area
        IonizationSD* gasSD = new IonizationSD("/ATPC/GAS");
        gas_logic->SetSensitiveDetector(gasSD);
        G4SDManager::GetSDMpointer()->AddNewDetector(gasSD);

        /// Limit the step size in this volume for better tracking precision
        gas_logic->SetUserLimits(new G4UserLimits(max_step_size_));


        this->SetLogicalVolume(lab_logic_volume);

        AssignVisuals();

    }

    void ATPC::AssignVisuals() {
        // Chamber
        G4LogicalVolumeStore* lvStore = G4LogicalVolumeStore::GetInstance();

        // Lab
        G4LogicalVolume* Lab = lvStore->GetVolume("LAB");
        Lab->SetVisAttributes(G4VisAttributes::GetInvisible());


        //Chamber
        G4LogicalVolume* Chamber = lvStore->GetVolume("CHAMBER");
        G4VisAttributes *ChamberVa=new G4VisAttributes(nexus::TitaniumGreyAlpha());
        ChamberVa->SetForceSolid(true);
        Chamber->SetVisAttributes(ChamberVa);
        // Chamber->SetVisAttributes(G4VisAttributes::GetInvisible());

        //GAS
        G4LogicalVolume* Gas = lvStore->GetVolume("GAS");
        G4VisAttributes *GasVa=new G4VisAttributes(nexus::BlueAlpha());
        GasVa->SetForceCloud(true);
        Gas->SetVisAttributes(GasVa);

    }
    void ATPC::PrintParam() {
    }
    G4ThreeVector ATPC::GenerateVertex(const G4String& region) const
    {
            G4ThreeVector pos;
            if (region == "AD_HOC") {
                pos = vtx_;
            }else if((region=="ACTIVE")){
                pos= active_gen_->GenerateVertex(INSIDE);
            }else if((region=="Cu")){
                pos= active_gen_->GenerateVertex(VOLUME);
            }else if((region=="CuCylinder")){

                // pos= cylinder_gen_->GenerateVertex(VOLUME);

                G4double tot_len = cube_size+2*chamber_thickn;
                G4double V_cyl = (pi/4) * ( std::pow(tot_len, 3) -  cube_size*cube_size*tot_len );
                G4double V_flange = (pi/4) * ( cube_size*cube_size*chamber_thickn );
                G4double tot_vol = V_cyl + 2*V_flange;
                // G4cout << V_cyl/tot_vol << ", " << V_flange/tot_vol << G4endl;
        
                G4double rand = G4UniformRand();
                if (rand<0.67){
                    pos= cylinder_gen_->GenerateVertex(VOLUME);
                }
                else if (rand>=0.67 && rand < 0.83){
                    pos= cylinder_gen_flange1_->GenerateVertex(VOLUME);
                }
                else {
                    pos= cylinder_gen_flange2_->GenerateVertex(VOLUME);
                }

            }else if((region=="CuCylinderActive")){
                pos= cylinder_gen_active_->GenerateVertex(VOLUME);
            }else{
                G4Exception("[ATPC]", "GenerateVertex()", JustWarning,
                            "Unknown vertex generation region. setting default region as FIELDCAGE..");
                pos=vtx_;
            }
        return pos;
    }
}
