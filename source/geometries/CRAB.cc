//
// Created by ilker on 9/2/21.
//

#include "CRAB.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "UniformElectricDriftField.h"
#include "IonizationSD.h"
#include "FactoryBase.h"
#include <G4Box.hh>
#include <G4SubtractionSolid.hh>
#include <G4GenericMessenger.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <G4UnionSolid.hh>


namespace nexus{
    using namespace CLHEP;
    REGISTER_CLASS(CRAB, GeometryBase)


             CRAB::CRAB():
             GeometryBase(),
             msg_(nullptr),
             Lab_size(1. *m),
             chamber_diam   (15. * cm),
             chamber_length (25. * cm),
             chamber_thickn (2. * mm),
             SourceEn_offset (5.2 *cm),
             SourceEn_diam   (1. * cm),
             SourceEn_length (1 * cm),
             SourceEn_thickn (2. * mm),
             SourceEn_holedia (2. * mm),
             gas_pressure_(10. * bar),
             vtx_(0,0,0),
            sc_yield_(25510.),
            e_lifetime_(1000. * ms),
             pmt_hole_length_ (18.434 * cm),
             sapphire_window_thickness_ (6. * mm),
             sapphire_window_diam_ (85. * mm),
             wndw_ring_stand_out_ (1.5 * mm), //how much the ring around sapph windows stands out of them
             pedot_coating_thickness_ (200. * nanometer), // copied from NEW
             optical_pad_thickness_ (1. * mm), // copied from NEW
             pmt_base_diam_ (47. * mm),
             pmt_base_thickness_ (5. * mm)

    {
        msg_ = new G4GenericMessenger(this, "/Geometry/CRAB/","Control commands of geometry of CRAB TPC");
        G4GenericMessenger::Command&  Pressure_cmd =msg_->DeclarePropertyWithUnit("gas_pressure","bar",gas_pressure_,"Pressure of Gas");
        Pressure_cmd.SetParameterName("XeNonPressure", false);


        G4GenericMessenger::Command&  chamber_diam_cmd =msg_->DeclarePropertyWithUnit("chamber_diam","cm",chamber_diam,"ChamberDiam");
        chamber_diam_cmd.SetParameterName("chamberdiam", false);

        G4GenericMessenger::Command&  chamber_length_cmd =msg_->DeclarePropertyWithUnit("chamber_length","cm",chamber_length,"Chamberlength");
        chamber_length_cmd.SetParameterName("chamberlength", false);

        G4GenericMessenger::Command&  chamber_thickn_cmd =msg_->DeclarePropertyWithUnit("chamber_thickn","mm",chamber_thickn,"Chamberthickn");
        chamber_thickn_cmd .SetParameterName("chamberthickn", false);


        G4GenericMessenger::Command&  source_position_cmd =msg_->DeclarePropertyWithUnit("SourcePosition","cm",vtx_,"vtx");
        source_position_cmd.SetParameterName("vtx", false);

        G4GenericMessenger::Command&  SourceEn_offset_cmd =msg_->DeclarePropertyWithUnit("SourceEn_offset","cm",SourceEn_offset,"SourceEnDiam");
        SourceEn_offset_cmd.SetParameterName("SourceEnoffset", false);

        G4GenericMessenger::Command&  SourceEn_diam_cmd =msg_->DeclarePropertyWithUnit("SourceEn_diam","cm",SourceEn_diam,"SourceEnDiam");
        SourceEn_diam_cmd.SetParameterName("SourceEndiam", false);

        G4GenericMessenger::Command&  SourceEn_length_cmd =msg_->DeclarePropertyWithUnit("SourceEn_length","cm",SourceEn_length,"SourceEnlength");
        SourceEn_length_cmd.SetParameterName("SourceEnlength", false);

        G4GenericMessenger::Command&  SourceEn_holedi_cmd =msg_->DeclarePropertyWithUnit("SourceEn_holedi","cm",SourceEn_holedia,"SourceEnholedi");
        SourceEn_holedi_cmd.SetParameterName("SourceEnholedi", false);

        G4GenericMessenger::Command&  Active_diam_cmd =msg_->DeclarePropertyWithUnit("Active_diam","cm",Active_diam,"ActiveDiam");
        Active_diam_cmd.SetParameterName("Activediam", false);

        G4GenericMessenger::Command&  Active_length_cmd =msg_->DeclarePropertyWithUnit("Active_length","cm",Active_length,"Activelength");
        Active_length_cmd.SetParameterName("Activelength", false);

        G4GenericMessenger::Command&  eliftime_cmd =msg_->DeclarePropertyWithUnit("ElecLifTime","ms",e_lifetime_,"Electron LifeTime");
        eliftime_cmd.SetParameterName("ElecLifTime", false);
        G4GenericMessenger::Command&  scinYield_cmd =msg_->DeclareProperty("scinYield",sc_yield_,"Scintilation Yield");
        scinYield_cmd.SetParameterName("scinYield", false);
        sc_yield_=(sc_yield_* 1/MeV);
        pmt1_=new PmtR7378A();
        pmt2_=new PmtR7378A();

    }

    CRAB::~CRAB()
    {

        delete msg_;
        delete pmt1_;
        delete pmt2_;
    }

    void CRAB::Construct(){


        //Materials
        G4Material* gxe = materials::GXe(gas_pressure_,68);
        G4Material *Saphire=materials::Sapphire();
        G4Material *vacuum=G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

        // Optical Properties Assigned here
        Saphire->SetMaterialPropertiesTable(opticalprops::Sapphire());
        vacuum->SetMaterialPropertiesTable(opticalprops::Vacuum());
        gxe->SetMaterialPropertiesTable(opticalprops::GXe(gas_pressure_, 68,sc_yield_,e_lifetime_));



        //Constructing Lab Space
        G4String lab_name="LAB";
        G4Box * lab_solid_volume = new G4Box(lab_name,Lab_size/2,Lab_size/2,Lab_size/2);
        G4LogicalVolume * lab_logic_volume= new G4LogicalVolume(lab_solid_volume,G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),lab_name) ;

        // PMT Holes
        G4Tubs * pmthole=new G4Tubs("Pmthole",0,sapphire_window_diam_/2,(chamber_thickn+1*cm)/2,0,twopi);



        //lab_logic_volume->SetVisAttributes(G4VisAttributes::Invisible);

        //Creating the Steel Cylinder that we need

        /// First Creating the Ends of the Cylinder with Proper Holes
        G4double hole_diam=2.54*cm;
        G4Tubs* chamber_solid_end =new G4Tubs("CHAMBER_END", hole_diam/2, (chamber_diam/2. + chamber_thickn),( chamber_thickn), 0.,twopi);
        G4LogicalVolume* chamber_logic_end =new G4LogicalVolume(chamber_solid_end,materials::Steel(), "CHAMBER_END");
        // Now Creating The Chamber with Without Ends
        G4Tubs* chamber_solid =new G4Tubs("CHAMBER", (chamber_diam/2. - chamber_thickn), (chamber_diam/2. + chamber_thickn),(chamber_length/2. + chamber_thickn), 0.,twopi);

        G4LogicalVolume* chamber_logic =new G4LogicalVolume(chamber_solid,materials::Steel(), "CHAMBER"); //
        ///////////////////////// From Next Energy Plane ////////////////
        /// Vacuum volume that encapsulates all elements related to PMTs. ///
        G4double hole_length_front_,pmt_stand_out_;


        G4double vacuum_front_length = hole_length_front_ + pmt_stand_out_+ sapphire_window_thickness_;
        G4Tubs* vacuum_front_solid =
                new G4Tubs("HOLE_FRONT", 0., +hole_diam/2., vacuum_front_length/2., 0., twopi);

        G4Tubs* vacuum_rear_solid =
                new G4Tubs("HOLE_REAR", 0., hole_diam_rear_/2., (hole_length_rear_+offset)/2., 0., twopi);

        G4Tubs* vacuum_hut_solid =
                new G4Tubs("HOLE_HUT", 0., hut_int_diam_/2., hut_hole_length_/2., 0., twopi);

        G4UnionSolid* vacuum_solid =
                new G4UnionSolid("EP_HOLE", vacuum_front_solid, vacuum_rear_solid, 0,
                                 G4ThreeVector(0., 0., (vacuum_front_length+hole_length_rear_)/2.));

        vacuum_solid =
                new G4UnionSolid("EP_HOLE", vacuum_solid, vacuum_hut_solid, 0,
                                 G4ThreeVector(0., 0., vacuum_front_length/2.+hole_length_rear_+hut_hole_length_/2.));

        G4LogicalVolume* vacuum_logic = new G4LogicalVolume(vacuum_solid, vacuum, "EP_HOLE");

        /// Sapphire window ///
        G4Tubs* sapphire_window_solid = new G4Tubs("SAPPHIRE_WINDOW", 0., hole_diam_front_/2.,
                                                   (sapphire_window_thickness_ )/2., 0., twopi);

        G4LogicalVolume* sapphire_window_logic
                = new G4LogicalVolume(sapphire_window_solid, Saphire, "SAPPHIRE_WINDOW");

        G4double window_posz = -vacuum_front_length/2. + (sapphire_window_thickness_ )/2.;

        G4VPhysicalVolume* sapphire_window_phys =
                new G4PVPlacement(0, G4ThreeVector(0., 0., window_posz), sapphire_window_logic,
                                  "SAPPHIRE_WINDOW", vacuum_logic, false, 0, false);


        //////////////////////////////////////////


        // Placing the gas in the chamber

        G4Tubs* gas_solid =new G4Tubs("GAS", 0., chamber_diam/2., chamber_length/2., 0., twopi);
        G4Tubs* Active_solid =new G4Tubs("ACTIVE", 0., Active_diam/2., Active_length/2., 0., twopi);

        // Radioactive Source Encloser
        //Source
        G4Tubs* SourceHolChamber_solid =new G4Tubs("SourceHolChamber", SourceEn_holedia/2, (SourceEn_diam/2. + SourceEn_thickn),(SourceEn_length/2. + SourceEn_thickn),0,twopi);
        G4Tubs* SourceHolChamberBlock_solid =new G4Tubs("SourceHolChBlock",0,(SourceEn_holedia/2),( SourceEn_thickn/2), 0.,twopi);

        //G4VSolid *SourceHolderGas_solid= new G4SubtractionSolid("SourceHolderGas",Source_Chm_solid,SourceHolder_solid);


        //G4Material* gxe = materials::GXe(gas_pressure_);
        //gxe->SetMaterialPropertiesTable(opticalprops::GXe(gas_pressure_, 68));




        G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, gxe, "GAS");
        G4LogicalVolume* Active_logic = new G4LogicalVolume(Active_solid, gxe, "ACTIVE");


        //G4LogicalVolume* SourceHolderGas_logic = new G4LogicalVolume(SourceHolderGas_solid, gxe, "SourceHolderGAS_logic");

        G4LogicalVolume* SourceHolChamber_logic = new G4LogicalVolume(SourceHolChamber_solid,materials::Steel(), "SourceHolChamber_logic");
        G4LogicalVolume* SourceHolChamberBlock_logic = new G4LogicalVolume(SourceHolChamberBlock_solid,materials::Steel(), "SourceHolChBlock_logic");





        //Rotation Matrix
        G4RotationMatrix* rm = new G4RotationMatrix();
        rm->rotateY(90.*deg);


        // Place the Volumes
        new G4PVPlacement(0,G4ThreeVector(0,0,-((chamber_length/2-chamber_thickn))),chamber_logic_end,chamber_solid_end->GetName(),lab_logic_volume,false,0,
                          false);
        new G4PVPlacement(0,G4ThreeVector(),lab_logic_volume,lab_logic_volume->GetName(),0,false,0, false);
        new G4PVPlacement(0,G4ThreeVector(0.,0.,0.) ,chamber_logic, chamber_solid->GetName(), lab_logic_volume, false, 0,false);
        new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), gas_logic, gas_solid->GetName(),chamber_logic, false, 0, false);
        new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), Active_logic, Active_solid->GetName(),gas_logic, false, 0, false);
        //new G4PVPlacement(rm, G4ThreeVector(-SourceEn_offset,-SourceEn_offset,-SourceEn_offset), SourceHolChamber_logic, SourceHolChamber_solid->GetName(),gas_logic, false, 0, false);
        //new G4PVPlacement(rm, G4ThreeVector(-SourceEn_offset-SourceEn_length/2,-SourceEn_offset-SourceEn_length/2,-SourceEn_offset), SourceHolChamberBlock_logic, SourceHolChamberBlock_solid->GetName(),gas_logic, false, 0, false);
        new G4PVPlacement(rm, G4ThreeVector(-SourceEn_offset,0,0), SourceHolChamber_logic, SourceHolChamber_solid->GetName(),gas_logic, false, 0, false);
        new G4PVPlacement(rm, G4ThreeVector(-SourceEn_offset-SourceEn_length/2,0,0), SourceHolChamberBlock_logic, SourceHolChamberBlock_solid->GetName(),gas_logic, false, 0, false);


        // Define this volume as an ionization sensitive detector
        IonizationSD* sensdet = new IonizationSD("/CRAB/ACTIVE");
        Active_logic->SetSensitiveDetector(sensdet);
        G4SDManager::GetSDMpointer()->AddNewDetector(sensdet);




        //Adding the PMTs in here
        pmt1_->Construct();
        pmt2_->Construct();

        G4LogicalVolume * pmt1_logic=pmt1_->GetLogicalVolume();
        G4LogicalVolume * pmt2_logic=pmt2_->GetLogicalVolume();


        G4RotationMatrix* pmt1rotate = new G4RotationMatrix();
        pmt1rotate->rotateY(180.*deg);
        new G4PVPlacement(0,G4ThreeVector (0,0,-((chamber_length/2)+chamber_thickn+(pmt1_->Length()/2))),pmt1_logic,"PMT1",lab_logic_volume,false,0,false);
        new G4PVPlacement(pmt1rotate,G4ThreeVector (0,0,((chamber_length/2)+chamber_thickn+((pmt2_->Length()/2)))),pmt2_logic,"PMT2",lab_logic_volume,false,0,false);

        AssignVisuals();
        this->SetLogicalVolume(lab_logic_volume);
        //this->SetLogicalVolume(chamber_logic);

    }


    void CRAB::AssignVisuals() {
        // Chamber
        G4LogicalVolumeStore* lvStore = G4LogicalVolumeStore::GetInstance();
        G4LogicalVolume* Chamber = lvStore->GetVolume("CHAMBER");
        G4LogicalVolume* Lab = lvStore->GetVolume("LAB");
        G4LogicalVolume* Active = lvStore->GetVolume("ACTIVE");
        G4LogicalVolume* Gas = lvStore->GetVolume("GAS");


        G4LogicalVolume* SourceHolder = lvStore->GetVolume("SourceHolChamber_logic");
        G4LogicalVolume* SourceHolderBlock = lvStore->GetVolume("SourceHolChBlock_logic");
        G4VisAttributes *SourceHolderVa=new G4VisAttributes(G4Colour(2,2,2));
        G4VisAttributes *ChamberVa=new G4VisAttributes(G4Colour(1,1,1));
        G4VisAttributes *LabVa=new G4VisAttributes(G4Colour(2,2,2));
        G4VisAttributes *ActiveVa=new G4VisAttributes(G4Colour(1,1,1));
        G4VisAttributes *GasVa=new G4VisAttributes(G4Colour(2,2,2));
        ChamberVa->SetForceSolid(true);
        //ChamberVa->SetLineStyle(G4VisAttributes::unbroken);
        Chamber->SetVisAttributes(ChamberVa);

        LabVa->SetForceWireframe(false);
        //GasVa->SetForceWireframe(false);
        GasVa->SetForceWireframe(true);
        ActiveVa->SetForceCloud(true);
        Active->SetVisAttributes(ActiveVa);
        Gas->SetVisAttributes(GasVa);
        SourceHolderVa->SetForceSolid(true);
        //SourceHolderVa->SetForceSolid(true);

        SourceHolder->SetVisAttributes(SourceHolderVa);
        SourceHolderBlock->SetVisAttributes(SourceHolderVa);
        Lab->SetVisAttributes(G4VisAttributes::GetInvisible());

    }


    void CRAB::PrintParam() {

    }

    G4ThreeVector CRAB::GenerateVertex(const G4String& region) const
    {

        if(!(region=="LAB" || region=="GAS" || region=="ACTIVE" || region=="SourceHolChamber")){

            G4Exception("[CRAB]", "GenerateVertex()", FatalException,
                        "Unknown vertex generation region.");
        }
        return vtx_;
    }


}
