#!/bin/bash                                                                                                                                                                                                                            

#SBATCH -J Qpix_Diffusion      # A single job name for the array
#SBATCH -p debug         # Partition
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --mem 5000         # Memory request (Mb)
#SBATCH -t 0-1:08           # Maximum execution time (D-HH:MM)
#SBATCH -o %A_%a.out        # Standard output                                                                                                                                                                                          
#SBATCH -e %A_%a.err        # Standard error

## Job options                                                                                                                                                                                                                         
JOBNUMBER=${SLURM_ARRAY_TASK_ID}
SEED=`echo "scale = 2;  $JOBNUMBER" | bc`

## Path to output files and G4Executable
outputDir="/media/ilker/Ilker/CRAB/Simulation/S1"
PathToG4Executable="/home/ilker/Projects/latest_Nexus/build/nexus"

## Events
NumberOfEvents=100


### Options
drift=false
EL=false
cluster=false

Pressure=10
Run=S1

###Info
ANumber=82
Mass=210


## Source Position
pos="-3 0 -4"
## Naming Macro and Root Files
NAME="${Run}_${SEED}"
OUT="${NAME}_${Pressure}_bar"
InitMACRO="${NAME}_${Pressure}_bar_init.mac"
configMACRO="${NAME}_${Pressure}_bar_config.mac"
PhotonFile="Photon_${NAME}.txt"
## Paths to the Filles

RootFile="${outputDir}/output/${OUT}"

## InitMacro

## making the macro
init_MACRO="${outputDir}/macros/${InitMACRO}"

## making the macro
config_MACRO="${outputDir}/macros/${configMACRO}"

## Counts 
PathToCounts="${outputDir}/counts"



echo $init_MACRO

###P
#Physics
echo "/PhysicsList/RegisterPhysics G4EmStandardPhysics_option4"  >>${init_MACRO}
echo "/PhysicsList/RegisterPhysics G4DecayPhysics"  >>${init_MACRO}
echo "/PhysicsList/RegisterPhysics G4RadioactiveDecayPhysics"  >>${init_MACRO}
echo "/PhysicsList/RegisterPhysics G4OpticalPhysics"  >>${init_MACRO}
echo "/PhysicsList/RegisterPhysics NexusPhysics"  >>${init_MACRO}
echo "/PhysicsList/RegisterPhysics G4StepLimiterPhysics"  >>${init_MACRO}

#Geometry
echo "/nexus/RegisterGeometry CRAB"  >>${init_MACRO}
#Generator
echo "/nexus/RegisterGenerator CrabSourceGenerator"  >>${init_MACRO}


#File Saving
echo "/nexus/RegisterPersistencyManager PersistencyManager"  >>${init_MACRO}

#Actions
echo "/nexus/RegisterTrackingAction DefaultTrackingAction"  >>${init_MACRO}
echo "/nexus/RegisterEventAction DefaultEventAction"  >>${init_MACRO}
echo "/nexus/RegisterRunAction DefaultRunAction"  >>${init_MACRO}
echo "/nexus/RegisterSteppingAction CRABAnalysisSteppingAction"  >>${init_MACRO}
echo "/nexus/RegisterMacro ${config_MACRO}"  >>${init_MACRO}


## Remove the Config File if it exists
rm $config_MACRO
echo $config_MACRO

## Configurations
echo  "/run/verbose 1"     >>${config_MACRO}
echo  "/event/verbose 1" >>${config_MACRO}
echo  "/tracking/verbose 0" >>${config_MACRO}
echo  "/process/em/verbose 1" >>${config_MACRO}




echo "/PhysicsList/Nexus/clustering ${cluster}"  >>${config_MACRO}
echo "/PhysicsList/Nexus/drift ${drift}"  >>${config_MACRO}
echo "/PhysicsList/Nexus/electroluminescence ${EL}"  >>${config_MACRO}


#GEOMETRY
echo "/Geometry/CRAB/gas_pressure 10. bar"  >>${config_MACRO}
echo "#/Geometry/CRAB/scinYield 10 1/MeV"  >>${config_MACRO}
echo "/Geometry/CRAB/chamber_diam  15. cm"  >>${config_MACRO}
echo "/Geometry/CRAB/chamber_length 43.18 cm"  >>${config_MACRO}
echo "/Geometry/CRAB/chamber_thickn 2. mm"  >>${config_MACRO}
echo "#/Geometry/CRAB/SourcePosition -4.5 -4.5 -4.5 cm"  >>${config_MACRO}
echo "/Geometry/CRAB/SourcePosition ${pos} cm"  >>${config_MACRO}
echo "#/Geometry/CRAB/SourcePosition 0 0 0 cm"  >>${config_MACRO}
echo "/Actions/CRABAnalysisSteppingAction/FileSave true"  >>${config_MACRO}
echo "/Actions/CRABAnalysisSteppingAction/FileName ${PhotonFile}"  >>${config_MACRO}
echo "/Actions/CRABAnalysisSteppingAction/FilePath ${PathToCounts}"  >>${config_MACRO}


#Active
echo "/Geometry/CRAB/Active_diam 8.5 cm"  >>${config_MACRO}
echo "/Geometry/CRAB/Active_length 42 cm"  >>${config_MACRO}

#SourceEncloser
echo "/Geometry/CRAB/SourceEn_diam 10 mm"  >>${config_MACRO}
echo "/Geometry/CRAB/SourceEn_holedi 5 mm"  >>${config_MACRO}
echo "/Geometry/CRAB/SourceEn_offset 5.7 cm"  >>${config_MACRO}

# GENERATOR
echo "/Generator/CrabSourceGenerator/region FIELDCAGE"  >>${config_MACRO}
#Pb_210
echo "/Generator/CrabSourceGenerator/atomic_number ${ANumber}"  >>${config_MACRO}
echo "/Generator/CrabSourceGenerator/mass_number ${Mass}"  >>${config_MACRO}


echo "/Generator/CrabSourceGenerator/decay_rate 100000 nCi"  >>${config_MACRO}
echo "#/Generator/CrabSourceGenerator/event_window 13"  >>${config_MACRO}
echo "#works only if event_window is zero"  >>${config_MACRO}
echo "#/Generator/CrabSourceGenerator/NDecays 1"  >>${config_MACRO}

#Actions
echo "#/Actions/DefaultEventAction/energy_threshold 100 eV"  >>${config_MACRO}
echo "#/Actions/DefaultEventAction/max_energy 1.00 GeV"  >>${config_MACRO}
echo "#/Actions/DefaultEventAction/min_energy 100 eV"  >>${config_MACRO}

# PERSISTENCY
echo "/nexus/persistency/eventType background"  >>${config_MACRO}
echo "/nexus/persistency/outputFile ${RootFile}"  >>${config_MACRO}
echo "#/run/printProgress 100"  >>${config_MACRO}



${PathToG4Executable} -n ${NumberOfEvents} ${init_MACRO}

#mv "${OUTFILE}" "${OUTPUTDIR}/"
