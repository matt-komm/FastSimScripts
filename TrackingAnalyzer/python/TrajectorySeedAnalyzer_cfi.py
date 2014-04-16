import FWCore.ParameterSet.Config as cms

trajectorySeedAnalyzer = cms.EDAnalyzer('TrajectorySeedAnalyzer',
    trajectorySeeds = cms.InputTag("iterativeInitialSeeds","InitialPixelTriplets","PROD")
)
