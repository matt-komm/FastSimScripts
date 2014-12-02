#!/bin/bash
FOLDER=TTbarValidation_$1
mkdir $FOLDER
cd $FOLDER

NSIZE=$2

if [[ -z "$NSIZE" ]]; then NSIZE=10; fi
echo $NSIZE


cmsDriver.py TTbar_Tauola_13TeV_cfi  \
--conditions auto:run2_mc \
--fast  \
-n $NSIZE \
--eventcontent DQM \
--relval None \
-s GEN,SIM,RECO,EI,HLT:@relval,VALIDATION \
--datatier DQMIO \
--customise SLHCUpgradeSimulations/Configuration/postLS1Customs.customisePostLS1 \
--magField 38T_PostLS1 \
--fileout file:step2.root

cmsDriver.py step2  \
--scenario pp \
--filetype DQM \
--conditions auto:run2_mc \
--mc \
-s HARVESTING:validationHarvestingFS \
-n $NSIZE \
--filein file:step2.root \
--fileout file:step3.root 

