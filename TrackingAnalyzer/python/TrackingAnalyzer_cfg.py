import FWCore.ParameterSet.Config as cms

process = cms.Process("PROD")

# Number of events to be generated
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

# Include DQMStore, needed by the famosSimHits
process.DQMStore = cms.Service( "DQMStore")

# Include the RandomNumberGeneratorService definition
process.load("IOMC.RandomEngine.IOMC_cff")

# Generate ttbar events
process.load("Configuration.Generator.TTbar_cfi")

# Famos sequences (NO HLT)
#process.load("FastSimulation.Configuration.CommonInputs_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load('FastSimulation.Configuration.Geometries_cff')
process.load("FastSimulation.Configuration.FamosSequences_cff")

# Parametrized magnetic field (new mapping, 4.0 and 3.8T)
#process.load("Configuration.StandardSequences.MagneticField_40T_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.VolumeBasedMagneticFieldESProducer.useParametrizedTrackerField = True

# If you want to turn on/off pile-up
process.load('FastSimulation.PileUpProducer.PileUpSimulator_2012_Startup_inTimeOnly_cff')
#process.load('FastSimulation.PileUpProducer.mix_2012_Startup_inTimeOnly_cff')
# You may not want to simulate everything for your study
process.famosSimHits.SimulateCalorimetry = True
process.famosSimHits.SimulateTracking = True

# Get frontier conditions    - not applied in the HCAL, see below
from HLTrigger.Configuration.AutoCondGlobalTag import AutoCondGlobalTag
process.GlobalTag = AutoCondGlobalTag(process.GlobalTag,'auto:startup_GRun')
# Allow reading of the tracker geometry from the DB
process.load('CalibTracker/Configuration/Tracker_DependentRecords_forGlobalTag_nofakes_cff')

# Apply ECAL miscalibration
from FastSimulation.CaloRecHitsProducer.CaloRecHits_cff import *
if(CaloMode==0 or CaloMode==2):
    process.ecalRecHit.doMiscalib = True

# Apply Tracker misalignment
process.famosSimHits.ApplyAlignment = True
process.misalignedTrackerGeometry.applyAlignment = True
process.misalignedDTGeometry.applyAlignment = True
process.misalignedCSCGeometry.applyAlignment = True

#  Attention ! for the HCAL IDEAL==STARTUP
#process.caloRecHits.RecHitsFactory.HCAL.Refactor = 1.0
#process.caloRecHits.RecHitsFactory.HCAL.Refactor_mean = 1.0
#process.caloRecHits.RecHitsFactory.HCAL.fileNameHcal = "hcalmiscalib_0.0.xml"

# Famos with everything !
#process.p1 = cms.Path(process.ProductionFilterSequence*process.famosWithEverything)

process.TFileService = cms.Service("TFileService", 
      fileName = cms.string("histo.root"),
      closeFileFast = cms.untracked.bool(True)
)

from FastSimScripts.TrackingAnalyzer.TrackerGeometryAnalyzer_cfi import *
process.trackerGeometryAnalyzer = trackerGeometryAnalyzer.clone()

from FastSimScripts.TrackingAnalyzer.TrajectorySeedAnalyzer_cfi import *
#iter 0
process.iterativeInitialSeedsAnalyzer = trajectorySeedAnalyzer.clone()
process.iterativeInitialSeedsAnalyzer.trajectorySeeds = cms.InputTag("iterativeInitialSeeds","InitialPixelTriplets","PROD")
#iter 1
process.iterativeLowPtTripletSeedsAnalyzer = trajectorySeedAnalyzer.clone()
process.iterativeLowPtTripletSeedsAnalyzer.trajectorySeeds = cms.InputTag("iterativeLowPtTripletSeeds","LowPtPixelTriplets","PROD")
#iter 2
process.iterativePixelPairSeedsAnalyzer = trajectorySeedAnalyzer.clone()
process.iterativePixelPairSeedsAnalyzer.trajectorySeeds = cms.InputTag("iterativePixelPairSeeds","PixelPair","PROD")
#iter 3
process.iterativeDetachedTripletSeedsAnalyzer = trajectorySeedAnalyzer.clone()
process.iterativeDetachedTripletSeedsAnalyzer.trajectorySeeds = cms.InputTag("iterativeDetachedTripletSeeds","DetachedPixelTriplets","PROD")
#iter 4
process.iterativeMixedTripletStepSeedsAnalyzer = trajectorySeedAnalyzer.clone()
process.iterativeMixedTripletStepSeedsAnalyzer.trajectorySeeds = cms.InputTag("iterativeMixedTripletStepSeeds","MixedTriplets","PROD")
#iter 5
process.iterativePixelLessSeedsAnalyzer = trajectorySeedAnalyzer.clone()
process.iterativePixelLessSeedsAnalyzer.trajectorySeeds = cms.InputTag("iterativePixelLessSeeds","PixelLessPairs","PROD")
#iter 6
process.iterativeTobTecSeedsAnalyzer = trajectorySeedAnalyzer.clone()
process.iterativeTobTecSeedsAnalyzer.trajectorySeeds = cms.InputTag("iterativeTobTecSeeds","TobTecLayerPairs","PROD") 

#other
#process.ancientMuonSeedAnalyzer = trajectorySeedAnalyzer.clone()
#process.ancientMuonSeedAnalyzer.trajectorySeeds = cms.InputTag("ancientMuonSeed","","PROD")

#process.newCombinedSeedsAnalyzer = trajectorySeedAnalyzer.clone()
#process.newCombinedSeedsAnalyzer.trajectorySeeds = cms.InputTag("newCombinedSeeds","","PROD")

process.pixelTripletSeedsAnalyzer = trajectorySeedAnalyzer.clone()
process.pixelTripletSeedsAnalyzer.trajectorySeeds = cms.InputTag("pixelTripletSeeds","PixelTriplet","PROD")
    

process.trackingAnalyzerSequence=cms.Sequence(
    process.trackerGeometryAnalyzer*
    process.iterativeInitialSeedsAnalyzer*
    process.iterativeLowPtTripletSeedsAnalyzer*
    process.iterativePixelPairSeedsAnalyzer*
    process.iterativeDetachedTripletSeedsAnalyzer*
    process.iterativeMixedTripletStepSeedsAnalyzer*
    process.iterativePixelLessSeedsAnalyzer*
    process.iterativeTobTecSeedsAnalyzer*
    #process.ancientMuonSeedAnalyzer*
    #process.newCombinedSeedsAnalyzer*
    process.pixelTripletSeedsAnalyzer
)
    
process.source = cms.Source("EmptySource")
process.p1 = cms.Path(process.generator*process.famosWithEverything*process.trackingAnalyzerSequence)

'''
# To write out events
process.load("FastSimulation.Configuration.EventContent_cff")
process.o1 = cms.OutputModule(
    "PoolOutputModule",
    fileName = cms.untracked.string("MyFirstFamosFile_1.root"),
    outputCommands = cms.untracked.vstring("keep *",
                                           "drop *_mix_*_*")
    )

process.outpath = cms.EndPath(process.o1)
'''
# Keep output to a nice level
# process.Timing =  cms.Service("Timing")
# process.MessageLogger.destinations = cms.untracked.vstring("pyDetailedInfo.txt","cout")
# process.MessageLogger.categories.append("FamosManager")
# process.MessageLogger.cout = cms.untracked.PSet(threshold=cms.untracked.string("INFO"),
#                                                 default=cms.untracked.PSet(limit=cms.untracked.int32(0)),
#                                                 FamosManager=cms.untracked.PSet(limit=cms.untracked.int32(100000)))


# Make the job crash in case of missing product
process.options = cms.untracked.PSet( Rethrow = cms.untracked.vstring('ProductNotFound') )
