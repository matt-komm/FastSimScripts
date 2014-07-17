#!/bin/bash
FOLDER=TTbarValidation_$1
mkdir $FOLDER
cd $FOLDER

cmsDriver.py TTbar_8TeV_cfi.py \
-s GEN,SIM,RECO,HLT,VALIDATION \
--fast \
--pileup=NoPileUp \
--conditions auto:startup_GRun \
--beamspot Realistic8TeVCollision \
--eventcontent=FEVTDEBUGHLT \
--datatier GEN-SIM-DIGI-RECO \
-n 10 \
--fileout=file:TTbarValidation_$1.root \
--python_filename=TTbarValidation_$1_cfg.py 

cmsDriver.py step3 \
-s HARVESTING:validationHarvestingFS \
--harvesting AtRunEnd \
--conditions auto:startup \
--mc \
--filein file:TTbarValidation_$1.root \
--fileout file:TTbarValidation_DQMPlots_$1.root \
--python_filename=TTbarValidation_DQMPlots_$1_cfg.py 
