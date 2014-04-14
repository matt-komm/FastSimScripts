
// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/Common/interface/OwnVector.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "FastSimulation/TrackerSetup/interface/TrackerInteractionGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"

#include "DataFormats/TrajectorySeed/interface/TrajectorySeed.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "TTree.h"
#include "TH2F.h"
#include <iostream>
#include <cmath>

class TrajectorySeedAnalyzer : public edm::EDAnalyzer {
    public:
        explicit TrajectorySeedAnalyzer(const edm::ParameterSet&);
        ~TrajectorySeedAnalyzer();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


    private:
        virtual void beginJob() override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        virtual void endJob() override;
        
        virtual void beginRun(edm::Run const&, edm::EventSetup const& es);

        edm::InputTag _trajectorySeedInputTag;
        edm::Service<TFileService> _fs;
        TTree* _seedTree;
        TH2F* _hits;
        edm::ESHandle<TrackerGeometry> _trackerGeometry;
};


TrajectorySeedAnalyzer::TrajectorySeedAnalyzer(const edm::ParameterSet& iConfig):
    _seedTree(nullptr)
{
    _trajectorySeedInputTag = iConfig.getParameter<edm::InputTag>("trajectorySeeds");
}


TrajectorySeedAnalyzer::~TrajectorySeedAnalyzer()
{


}

void
TrajectorySeedAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    edm::Handle<std::vector<TrajectorySeed>> seedList;
    iEvent.getByLabel(_trajectorySeedInputTag,seedList);
    
    
    
    //std::cout<<"----------------------"<<std::endl;
    for (unsigned int iseed = 0; iseed<seedList->size();++iseed)
    {
        //std::cout<<"seed: "<<iseed<<std::endl;
        const TrajectorySeed& trajectorySeed = (*seedList)[iseed];
        const TrajectorySeed::range& hitRange = trajectorySeed.recHits();
        for (TrajectorySeed::const_iterator it = hitRange.first; it != hitRange.second; ++it)
        {
            const TrackingRecHit& hit = *it;
            const  LocalPoint& localPoint = hit.localPosition();
            const DetId& theDetId = hit.geographicalId();
            const GeomDet* theGeomDet = _trackerGeometry->idToDet(theDetId);
            const GlobalPoint& globalPoint = theGeomDet->toGlobal (localPoint);
            //std::cout<<"    hit: "<<it - hitRange.first<<", lx: "<<localPoint.x()<<", gx: "<<globalPoint.x()<<std::endl;
            if (fabs(globalPoint.phi())<10.0/360.0*3.1416*2);
            {
                _hits->Fill(globalPoint.z(),std::sqrt(globalPoint.x()*globalPoint.x()+globalPoint.y()*globalPoint.y()));
            }
        }
        
    }
}


// ------------ method called once each job just before starting event loop  ------------
void 
TrajectorySeedAnalyzer::beginJob()
{
     TFileDirectory subDir = _fs->mkdir( "TrajectorySeedAnalyzer" );
     _seedTree = subDir.make<TTree>(_trajectorySeedInputTag.label().c_str(),_trajectorySeedInputTag.label().c_str());
     _hits = subDir.make<TH2F>(_trajectorySeedInputTag.label().c_str(),_trajectorySeedInputTag.label().c_str(),400,-200.0,200.0,600,0.0,60.0);
    
    
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TrajectorySeedAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------

void 
TrajectorySeedAnalyzer::beginRun(edm::Run const&, edm::EventSetup const& es)
{
    es.get<TrackerDigiGeometryRecord>().get(_trackerGeometry);
}


// ------------ method called when ending the processing of a run  ------------
/*
void 
TrajectorySeedAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
TrajectorySeedAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
TrajectorySeedAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TrajectorySeedAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TrajectorySeedAnalyzer);
