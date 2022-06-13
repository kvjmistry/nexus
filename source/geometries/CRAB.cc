//
// Created by ilker on 9/2/21.
//

#include "CRAB.h"
#include "Visibilities.h"
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
#include "XenonProperties.h"

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
             sapphire_window_diam_ (25.4 * mm),
             wndw_ring_stand_out_ (1.5 * mm), //how much the ring around sapph windows stands out of them
             pedot_coating_thickness_ (200. * nanometer), // copied from NEW
             optical_pad_thickness_ (1. * mm), // copied from NEW
             pmt_base_diam_ (47. * mm),
             pmt_base_thickness_ (5. * mm),
             efield_(true)

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


        //lab_logic_volume->SetVisAttributes(G4VisAttributes::Invisible);

        //Creating the Steel Cylinder that we need

        /// First Creating the Ends of the Cylinder with Proper Holes
        G4Tubs* chamber_flange_solid =new G4Tubs("CHAMBER_FLANGE", sapphire_window_diam_/2, (chamber_diam/2. + chamber_thickn),( chamber_thickn), 0.,twopi);
        G4LogicalVolume* chamber_flange_logic =new G4LogicalVolume(chamber_flange_solid,materials::Steel(), "CHAMBER_FLANGE");

        // Now Creating The Chamber with Without Ends
        G4Tubs* chamber_solid =new G4Tubs("CHAMBER", chamber_diam/2., (chamber_diam/2. + chamber_thickn),(chamber_length/2. + chamber_thickn), 0.,twopi);
        G4LogicalVolume* chamber_logic =new G4LogicalVolume(chamber_solid,materials::Steel(), "CHAMBER"); //



        /// Sapphire window ///
        G4Tubs* sapphire_window_solid = new G4Tubs("SAPPHIRE_WINDOW", 0., sapphire_window_diam_/2.,
                                                   (sapphire_window_thickness_ )/2., 0., twopi);
        G4LogicalVolume* sapphire_window_logic= new G4LogicalVolume(sapphire_window_solid, Saphire, "SAPPHIRE_WINDOW");



        //////////////////////////////////////////

        // Placing the gas in the chamber
        G4Tubs* gas_solid =new G4Tubs("GAS", 0., chamber_diam/2., chamber_length/2., 0., twopi);
        G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, gxe, "GAS");

        //Defining the Detection Area
        G4Tubs* Active_solid =new G4Tubs("ACTIVE", 0., Active_diam/2., Active_length/2., 0., twopi);
        G4LogicalVolume* Active_logic = new G4LogicalVolume(Active_solid, gxe, "ACTIVE");



        // Radioactive Source Encloser
        //Source
        G4Tubs* SourceHolChamber_solid =new G4Tubs("SourceHolChamber", SourceEn_holedia/2, (SourceEn_diam/2. + SourceEn_thickn),(SourceEn_length/2. + SourceEn_thickn),0,twopi);
        G4LogicalVolume* SourceHolChamber_logic = new G4LogicalVolume(SourceHolChamber_solid,materials::Steel(), "SourceHolChamber_logic");

        G4Tubs* SourceHolChamberBlock_solid =new G4Tubs("SourceHolChBlock",0,(SourceEn_holedia/2),( SourceEn_thickn/2), 0.,twopi);
        G4LogicalVolume* SourceHolChamberBlock_logic = new G4LogicalVolume(SourceHolChamberBlock_solid,materials::Steel(), "SourceHolChBlock_logic");

        //G4LogicalVolume* SourceHolderGas_logic = new G4LogicalVolume(SourceHolChamber_solid, gxe, "SourceHolderGAS_logic");







        //Adding the PMTs in here
        pmt1_->Construct();
        pmt2_->Construct();

        // Adding Logical Volumes for PMTs
        G4LogicalVolume * pmt1_logic=pmt1_->GetLogicalVolume();
        G4LogicalVolume * pmt2_logic=pmt2_->GetLogicalVolume();

        // Particle Source Holder
        //Rotation Matrix
        G4RotationMatrix* rotateHolder = new G4RotationMatrix();
        rotateHolder->rotateY(90.*deg);
        new G4PVPlacement(rotateHolder, G4ThreeVector(-SourceEn_offset,0,0), SourceHolChamber_logic, SourceHolChamber_solid->GetName(),gas_logic, false, 0, false);
        new G4PVPlacement(rotateHolder, G4ThreeVector(-SourceEn_offset-SourceEn_length/2,0,0), SourceHolChamberBlock_logic, SourceHolChamberBlock_solid->GetName(),gas_logic, false, 0, false);



        // PMTs
        G4double PMT_offset=0.2*cm;
        G4double PMT_pos=(chamber_length/2)+chamber_thickn+(pmt1_->Length()/2)+sapphire_window_thickness_+PMT_offset;
        G4RotationMatrix* pmt1rotate = new G4RotationMatrix();
        pmt1rotate->rotateY(180.*deg);

        //// PMT Covering Tube ///
        G4double PMT_Tube_Length=sapphire_window_thickness_+(pmt1_->Length()+0.5*cm)/2;
        G4double PMT_Tube_Block_Thickness=0.2*cm;
        G4Tubs * PMT_Tube_solid=new G4Tubs("PMT_TUBE",(sapphire_window_diam_/2),(sapphire_window_diam_/2)+0.2*cm,PMT_Tube_Length,0,twopi);
        G4LogicalVolume * PMT_Tube_Logic=new G4LogicalVolume(PMT_Tube_solid,materials::Steel(),PMT_Tube_solid->GetName());
        G4Tubs * PMT_Block_solid=new G4Tubs("PMT_TUBE_BLOCK",0,(sapphire_window_diam_/2),PMT_Tube_Block_Thickness,0,twopi);
        G4LogicalVolume * PMT_Block_Logic=new G4LogicalVolume(PMT_Block_solid,materials::Steel(),PMT_Block_solid->GetName());
        G4Tubs * InsideThePMT_Tube_solid=new G4Tubs("PMT_TUBE_VACUUM",0,(sapphire_window_diam_/2),PMT_Tube_Length,0,twopi);
        G4LogicalVolume * InsideThePMT_Tube_Logic=new G4LogicalVolume(InsideThePMT_Tube_solid,vacuum,InsideThePMT_Tube_solid->GetName());


        // Place the Volumes

        //LAB
        new G4PVPlacement(0,G4ThreeVector(),lab_logic_volume,lab_logic_volume->GetName(),0,false,0, false);

        //Flanges on the Chamber
        new G4PVPlacement(0,G4ThreeVector(0,0,(chamber_length/2)),chamber_flange_logic,chamber_flange_solid->GetName(),lab_logic_volume,true,0,false);
        new G4PVPlacement(0,G4ThreeVector(0,0,-(chamber_length/2)),chamber_flange_logic,chamber_flange_solid->GetName(),lab_logic_volume,true,1,false);

        //Chamber
        new G4PVPlacement(0,G4ThreeVector(0.,0.,0.) ,chamber_logic, chamber_solid->GetName(), lab_logic_volume, false, 0,false);

        // Xenon Gas in Active Area and Non-Active Area
        new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), gas_logic, gas_solid->GetName(),lab_logic_volume, false, 0, false);
        new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), Active_logic, Active_solid->GetName(),gas_logic, false, 0, false);

        // PMTs
        new G4PVPlacement(0,G4ThreeVector (0,0,-PMT_pos),pmt1_logic,"PMT",lab_logic_volume,false,0,false);
        new G4PVPlacement(pmt1rotate,G4ThreeVector (0,0,PMT_pos),pmt2_logic,"PMT",lab_logic_volume,false,1,false);

        //  Saphire Windows
        //G4double window_posz = chamber_length/2+chamber_thickn/2;
        G4double window_posz = chamber_length/2;
        new G4PVPlacement(0, G4ThreeVector(0., 0., window_posz), sapphire_window_logic,"SAPPHIRE_WINDOW", lab_logic_volume,false, 0, false);
        new G4PVPlacement(pmt1rotate, G4ThreeVector(0., 0., -window_posz), sapphire_window_logic,"SAPPHIRE_WINDOW", lab_logic_volume, false, 1, false);

        //PMT Tubes
        new G4PVPlacement(0,G4ThreeVector(0,0,PMT_pos-PMT_offset),PMT_Tube_Logic,PMT_Tube_Logic->GetName(),lab_logic_volume,false,0,false);
        new G4PVPlacement(0,G4ThreeVector(0,0,PMT_pos-PMT_offset+PMT_Tube_Length-PMT_Tube_Block_Thickness/2),PMT_Block_Logic,PMT_Block_Logic->GetName(),lab_logic_volume,false,0,false);

        new G4PVPlacement(pmt1rotate,G4ThreeVector(0,0,-(PMT_pos-PMT_offset)),PMT_Tube_Logic,PMT_Tube_Logic->GetName(),lab_logic_volume,false,1,false);
        new G4PVPlacement(pmt1rotate,G4ThreeVector(0,0,-(PMT_pos-PMT_offset+PMT_Tube_Length-PMT_Tube_Block_Thickness/2)),PMT_Block_Logic,PMT_Block_Logic->GetName(),lab_logic_volume,false,1,false);




        // Define this volume as an ionization sensitive detector
        IonizationSD* sensdet = new IonizationSD("/CRAB/ACTIVE");
        Active_logic->SetSensitiveDetector(sensdet);
        G4SDManager::GetSDMpointer()->AddNewDetector(sensdet);

        // Electrical Field
        if(efield_){
            /*UniformElectricDriftField * EfieldForEL=new UniformElectricDriftField();
            EfieldForEL->SetCathodePosition(chamber_length/2);
            EfieldForEL->SetAnodePosition(-chamber_length/2);
            EfieldForEL->SetDriftVelocity(2.5*mm/microsecond);
            EfieldForEL->SetTransverseDiffusion(1.*mm/sqrt(cm));
            EfieldForEL->SetLongitudinalDiffusion(.5*mm/sqrt(cm));
            // ELRegion->SetLightYield(xgp.ELLightYield(24.8571*kilovolt/cm));//value for E that gives Y=1160 photons per ie- in normal conditions
            EfieldForEL->SetLightYield(XenonELLightYield(23.2857*kilovolt/cm, gas_pressure_));
            G4Region* el_region = new G4Region("EL_REGION");
            el_region->SetUserInformation(EfieldForEL);
           // el_region->AddRootLogicalVolume(Active_logic);*/
            //Define a drift field for this volume
            UniformElectricDriftField* field = new UniformElectricDriftField();
            field->SetCathodePosition(chamber_length/2);
            field->SetAnodePosition(-chamber_length/2);
            field->SetDriftVelocity(1.*mm/microsecond);
            field->SetTransverseDiffusion(1.*mm/sqrt(cm));
            field->SetLongitudinalDiffusion(.3*mm/sqrt(cm));

            G4Region* drift_region = new G4Region("DRIFT");
            drift_region->SetUserInformation(field);
            drift_region->SetUserInformation(field);
            drift_region->AddRootLogicalVolume(Active_logic);
        }
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
        Chamber->SetVisAttributes(G4VisAttributes::GetInvisible());

        LabVa->SetForceWireframe(false);
        //GasVa->SetForceWireframe(false);
        GasVa->SetForceWireframe(true);
        ActiveVa->SetForceCloud(true);
        Active->SetVisAttributes(ActiveVa);
        Gas->SetVisAttributes(GasVa);
        SourceHolderVa->SetForceSolid(true);
        //SourceHolderVa->SetForceSolid(true);

        // Flange
        G4LogicalVolume* flangeLog = lvStore->GetVolume("CHAMBER_FLANGE");
        G4VisAttributes flangeVis=nexus::DarkGreyAlpha();
        flangeVis.SetForceSolid(true);
        flangeLog->SetVisAttributes(ChamberVa);


        //PMT TUBE AND PMT BLOCK
        G4LogicalVolume * PmttubeLog=lvStore->GetVolume("PMT_TUBE");
        PmttubeLog->SetVisAttributes(ChamberVa);
        G4LogicalVolume * PmttubeBlockLog=lvStore->GetVolume("PMT_TUBE_BLOCK");
        PmttubeBlockLog->SetVisAttributes(ChamberVa);
        G4LogicalVolume * PmttubeVacuumLog=lvStore->GetVolume("PMT_TUBE_VACUUM");
        G4VisAttributes PmttubeVacuumVis=nexus::Red();
        PmttubeVacuumLog->SetVisAttributes(PmttubeVacuumVis);


        //SaphireWindow
        G4LogicalVolume* SaphireWindowLog = lvStore->GetVolume("SAPPHIRE_WINDOW");
        G4VisAttributes  SaphireWindowVis=nexus::LillaAlpha();
        SaphireWindowVis.SetForceSolid(true);
        SaphireWindowLog->SetVisAttributes(SaphireWindowVis);


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
