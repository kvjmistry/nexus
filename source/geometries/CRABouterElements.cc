//
// Created by ilker on 6/3/22.
//

#include "CRABouterElements.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4NistManager.hh>
#include <G4VPhysicalVolume.hh>
#include <G4TransportationManager.hh>
#include <Randomize.hh>
#include <G4RotationMatrix.hh>

namespace  nexus{
    CRABouterElements::CRABouterElements()
    {
        msg_= new G4GenericMessenger(this,"/Geometry/CRAB/","Control commands of the GRAB geometry");
    }


}