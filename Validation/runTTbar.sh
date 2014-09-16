#!/bin/bash
FOLDER=TTbarValidation_$1
mkdir $FOLDER
cd $FOLDER

NSIZE=$2

if [[ -z "$NSIZE" ]]; then NSIZE=10; fi
echo $NSIZE

cmsDriver.py TTbar_Tauola_8TeV_cfi  \
--conditions auto:run1_mc \
--fast  \
-n $NSIZE \
-s GEN,SIM,RECO,EI,HLT:@relval,VALIDATION \
--eventcontent DQM \
--relval None \
--datatier DQMIO \
--fileout file:step2.root

cmsDriver.py step3  \
--scenario pp \
--filetype DQM \
--conditions auto:run1_mc \
--mc  \
-s HARVESTING:validationHarvestingFS \
-n $NSIZE  \
--filein file:step2.root \
--fileout file:step3.root 
