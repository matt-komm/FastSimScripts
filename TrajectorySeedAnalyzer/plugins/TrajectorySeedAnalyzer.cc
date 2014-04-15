
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

        edm::InputTag _trajectorySeedInputTag;
        edm::Service<TFileService> _fs;
        TTree* _seedTree;
        TH2F* _hits;
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
            _hits->Fill(globalPoint.z(),std::sqrt(globalPoint.x()*globalPoint.x()+globalPoint.y()*globalPoint.y()));
        }
        
    }
}


// ------------ method called once each job just before starting event loop  ------------
void 
TrajectorySeedAnalyzer::beginJob()
{
     TFileDirectory subDir = _fs->mkdir( "TrajectorySeedAnalyzer" );
     _seedTree = subDir.make<TTree>(_trajectorySeedInputTag.label().c_str(),_trajectorySeedInputTag.label().c_str());
     _hits = subDir.make<TH2F>(_trajectorySeedInputTag.label().c_str(),_trajectorySeedInputTag.label().c_str(),300,-300.0,300.0,600,0.0,60.0);
     _cv = subDir.make<TCanvas>(_trajectorySeedInputTag.label().c_str(),_trajectorySeedInputTag.label().c_str(),1200,900);
    
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
    
    _cv->cd();
    TH2F* axis = new TH2F("axis","",100,-320,320,100,0,130);
    axis->Draw("AXIS");
    double xmax=300.0;
    double ymax=120.0;
    for (double eta=0; eta<3.1; eta+=0.2)
    {
        double theta=2.0*atan(exp(-eta));
        double vy=std::sin(theta);
        double vx=std::cos(theta);
        
        double ry=ymax/vy;
        double rx=xmax/vx;
        
        double r=std::min(rx,ry);
        
        TLine* line1 = new TLine(0,0,r*vx,r*vy);
        TLine* line2 = new TLine(0,0,-r*vx,r*vy);
        line1->SetLineStyle(2);
        line1->SetLineWidth(2);
        line1->SetLineColor(kGray);
        
        line2->SetLineStyle(2);
        line2->SetLineWidth(2);
        line2->SetLineColor(kGray);
        
        line1->Draw("Same");
        line2->Draw("Same");
        
        TPaveText* pave1 = new TPaveText(r*vx-12,r*vy-1,r*vx+12,r*vy+1);
        //pave1->SetBorderSize(1);
        pave1->SetFillColor(kWhite);
        char* buf1 = new char[20];
        sprintf(buf1,"#eta=%2.1f",eta);
        pave1->AddText(buf1);
        pave1->Draw("Same");
        
        TPaveText* pave2 = new TPaveText(-r*vx-12,r*vy-1,-r*vx+12,r*vy+1);
        //pave2->SetBorderSize(1);
        pave2->SetFillColor(kWhite);
        char* buf2 = new char[20];
        sprintf(buf2,"#eta=%2.1f",-eta);
        pave2->AddText(buf2);
        pave2->Draw("Same");
    }
    const std::vector<DetId> detIdList = _trackerGeometry->detIds();
    for (unsigned int idet = 0; idet<detIdList.size();++idet)
    {
        const DetId& detId = detIdList[idet];
        std::cout<<"idet: "<<idet<<", name="<<_trackerTopology->print(detId)<<std::endl;
        const GeomDet* geomDet = _trackerGeometry->idToDet(detId);
        const Plane& plane = geomDet->surface();
        const Plane::PositionType& position = plane.position();
        /*
        std::cout<<"    r="<<plane.rSpan().first<<","<<plane.rSpan().second<<std::endl;
        std::cout<<"    z="<<plane.zSpan().first<<","<<plane.zSpan().second<<std::endl;
        std::cout<<"    phi="<<plane.phiSpan().first<<","<<plane.phiSpan().second<<std::endl;
        std::cout<<"    x="<<position.x()<<std::endl;
        std::cout<<"    y="<<position.y()<<std::endl;
        std::cout<<"    z="<<position.z()<<std::endl;
        std::cout<<"    eta="<<position.eta()<<std::endl;
        std::cout<<"    phi="<<position.phi()<<std::endl;
        std::cout<<"    perp="<<position.perp()<<std::endl;
        std::cout<<"    length="<<plane.bounds().length()<<std::endl;
        std::cout<<"    width="<<plane.bounds().width()<<std::endl;
        std::cout<<"    thickness="<<plane.bounds().thickness()<<std::endl;
        */
        const Plane::RotationType& rotation = plane.rotation();
        Plane::RotationType::BasicVector vectorLength(0.0,plane.bounds().length()*0.5,0.0);
        Plane::RotationType::BasicVector vectorWidth(plane.bounds().width()*0.5,0.0,0.0);
        Plane::RotationType::BasicVector rotVectorLength = rotation.rotateBack(vectorLength);
        Plane::RotationType::BasicVector rotVectorWidth = rotation.rotateBack(vectorWidth);
        Plane::RotationType::BasicVector positionVector(position.x(),position.y(),position.z());
        Plane::RotationType::BasicVector pos1 = positionVector+rotVectorLength+rotVectorWidth; 
        Plane::RotationType::BasicVector pos2 = positionVector+rotVectorLength-rotVectorWidth; 
        Plane::RotationType::BasicVector pos3 = positionVector-rotVectorLength-rotVectorWidth; 
        Plane::RotationType::BasicVector pos4 = positionVector-rotVectorLength+rotVectorWidth;
        /*
        printf("    <<%4.3f,  %4.3f,  %4.3f>,\n     <%4.3f,  %4.3f,  %4.3f>,\n     <%4.3f,  %4.3f,  %4.3f>>\n",
            rotation.xx(), rotation.xy(), rotation.xz(),
            rotation.yx(), rotation.yy(), rotation.yz(),
            rotation.zx(), rotation.zy(), rotation.zz()
        );
        //std::cout<<"    rotX="<<rotation.x()<<std::endl;
        //std::cout<<"    rotY="<<rotation.y()<<std::endl;
        //std::cout<<"    rotZ="<<rotation.z()<<std::endl;
        */
        double xpos[5]={pos1.z(),pos2.z(),pos3.z(),pos4.z(),pos1.z()};
        double ypos[5]={pos1.perp(),pos2.perp(),pos3.perp(),pos4.perp(),pos1.perp()};
        TPolyLine* line = new TPolyLine(5,xpos,ypos);
        line->Draw("LSame");
    }
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
