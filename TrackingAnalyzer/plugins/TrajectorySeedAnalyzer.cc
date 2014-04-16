
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
#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "DataFormats/GeometrySurface/interface/GloballyPositioned.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "FastSimulation/TrackerSetup/interface/TrackerInteractionGeometry.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "DataFormats/GeometrySurface/interface/Bounds.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"

#include "DataFormats/TrajectorySeed/interface/TrajectorySeed.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TROOT.h"
#include "TColor.h"
#include "TPaveText.h"
#include "TPolyLine.h"
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
        
        void drawEtaGrid(double etamax, double xmax, double ymax, bool log);
        void drawDetComponentZPerp(const DetId& detId);
        void drawDetComponentXY(const DetId& detId);
        const Plane::PositionType& getPosition(const DetId& detId);
    
        edm::InputTag _trajectorySeedInputTag;
        edm::Service<TFileService> _fs;
        TTree* _seedTree;
        TH2F* _hitsZPerp;
        TH2F* _hitsXY;
        TCanvas* _cv;
        edm::ESHandle<TrackerGeometry> _trackerGeometry;
        edm::ESHandle<TrackerTopology> _trackerTopology;
};


TrajectorySeedAnalyzer::TrajectorySeedAnalyzer(const edm::ParameterSet& iConfig):
    _seedTree(nullptr)
{
    _trajectorySeedInputTag = iConfig.getParameter<edm::InputTag>("trajectorySeeds");
}


TrajectorySeedAnalyzer::~TrajectorySeedAnalyzer()
{


}

const Plane::PositionType& TrajectorySeedAnalyzer::getPosition(const DetId& detId)
{
    const GeomDet* geomDet = _trackerGeometry->idToDet(detId);
    const Plane& plane = geomDet->surface();
    return plane.position();
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
            _hitsZPerp->Fill(globalPoint.z(),std::sqrt(globalPoint.x()*globalPoint.x()+globalPoint.y()*globalPoint.y()));
            _hitsXY->Fill(globalPoint.x(),globalPoint.y());
        }
        
    }
}


// ------------ method called once each job just before starting event loop  ------------
void 
TrajectorySeedAnalyzer::beginJob()
{
     //TFileDirectory subDir = _fs->mkdir( "TrajectorySeedAnalyzer" );
     //_seedTree = subDir.make<TTree>(_trajectorySeedInputTag.label().c_str(),_trajectorySeedInputTag.label().c_str());
     _hitsZPerp = _fs->make<TH2F>("hitsZPerp","",1280,-320,320,520,0,130);
     _hitsZPerp->SetMarkerColor(kRed);
     _hitsZPerp->SetMarkerStyle(20);
     _hitsZPerp->SetMarkerSize(0.2);
     _hitsXY = _fs->make<TH2F>("hitsXY","",750,-130,130,750,-130,130);
     _hitsXY->SetMarkerColor(kRed);
     _hitsXY->SetMarkerStyle(20);
     _hitsXY->SetMarkerSize(0.2);
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
    es.get<IdealGeometryRecord>().get(_trackerTopology);
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
