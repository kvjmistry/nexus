// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 4 Dic 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNewRnTube.h"
#include <G4GenericMessenger.hh>

#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4UnionSolid.hh>
#include <G4VisAttributes.hh>
#include <G4Tubs.hh>
#include <G4NistManager.hh>

namespace nexus {

  NextNewRnTube::NextNewRnTube():
    BaseGeometry(),
    // Body dimensions
    _inner_diam (970. *mm),
    _length (1800. *mm),
    _thickness (1. *mm)
    
  {
   /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    _msg->DeclareProperty("RnTube_vis", _visibility, "Radon Tube Visibility");  
  }
   
  void NextNewRnTube::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }

  void NextNewRnTube::Construct()
  {
    ////// RADON TUBE ///////////
    G4Tubs* tube_solid = new G4Tubs("RN_BODY", _inner_diam/2., _inner_diam/2.+_thickness, _length/2.,0.,twopi);
    G4Tubs* endcap_solid =new G4Tubs("RN_ENDCAP", 0., _inner_diam/2.+_thickness, _thickness/2.,0.,twopi);
    G4UnionSolid* rn_solid = new G4UnionSolid("RN_TUBE", tube_solid,endcap_solid,
					      0, G4ThreeVector(0.,0.,-_length/2.-_thickness/2.));
    rn_solid = new G4UnionSolid("RN_TUBE", rn_solid,endcap_solid, 0, G4ThreeVector(0.,0.,_length/2.+_thickness/2.) );
    G4Material* mother_material = _mother_logic->GetMaterial();
    G4LogicalVolume* tube_logic = new G4LogicalVolume(rn_solid,mother_material,"RN_TUBE");
    G4PVPlacement* _tube_physi = 
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), tube_logic, "RN_TUBE", _mother_logic, false, 0,false);
   
    // SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes steel_col(G4Colour(.88, .87, .86));
      //steel_col.SetForceSolid(true);
      tube_logic->SetVisAttributes(steel_col);
    }
    else {
      tube_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    // VERTEX GENERATORS   //////////
    _tube_gen = new CylinderPointSampler(_inner_diam/2.,_length,_thickness,_thickness, G4ThreeVector(0.,0.,0.), 0);
   		 
    // Calculating some probs
    G4double tube_vol = tube_solid->GetCubicVolume();
    std::cout<<"RADON TUBE VOLUME:\t"<<tube_vol<<std::endl;
  }

  NextNewRnTube::~NextNewRnTube()
  {
    delete _tube_gen;
  }
  
  G4ThreeVector NextNewRnTube::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    if (region == "RN_TUBE") {
      vertex = _tube_gen->GenerateVertex("WHOLE_VOL");
    }
    return vertex;
  }

} //end namespace nexus