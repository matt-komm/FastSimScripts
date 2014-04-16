
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

class TrackerGeometryAnalyzer : public edm::EDAnalyzer {
    public:
        explicit TrackerGeometryAnalyzer(const edm::ParameterSet&);
        ~TrackerGeometryAnalyzer();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


    private:
        virtual void beginJob() override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        virtual void endJob() override;
        
        virtual void beginRun(edm::Run const&, edm::EventSetup const& es);
        
        void drawEtaGrid(double etamax, double xmax, double ymax);
        void drawPhiGrid(double xmax, double ymax, int N=10);
        void drawDetComponentZPerp(const DetId& detId);
        void drawDetComponentXY(const DetId& detId);
        const Plane::PositionType& getPosition(const DetId& detId);
    
        edm::Service<TFileService> _fs;
        TCanvas* _cvZPerp;
        TCanvas* _cvXY;
        edm::ESHandle<TrackerGeometry> _trackerGeometry;
        edm::ESHandle<TrackerTopology> _trackerTopology;
};


TrackerGeometryAnalyzer::TrackerGeometryAnalyzer(const edm::ParameterSet& iConfig)
{
}


TrackerGeometryAnalyzer::~TrackerGeometryAnalyzer()
{
}

const Plane::PositionType& TrackerGeometryAnalyzer::getPosition(const DetId& detId)
{
    const GeomDet* geomDet = _trackerGeometry->idToDet(detId);
    const Plane& plane = geomDet->surface();
    return plane.position();
}

void TrackerGeometryAnalyzer::drawDetComponentXY(const DetId& detId)
{
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
    double xpos[5]={pos1.x(),pos2.x(),pos3.x(),pos4.x(),pos1.x()};
    double ypos[5]={pos1.y(),pos2.y(),pos3.y(),pos4.y(),pos1.y()};
    TPolyLine* line = new TPolyLine(5,xpos,ypos);
    line->SetLineColor(kBlack);
    line->SetFillStyle(1001);
    line->SetFillColor(kBlack);
    line->Draw("LSame");
    //line->Draw("FSame");
}

void TrackerGeometryAnalyzer::drawDetComponentZPerp(const DetId& detId)
{
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
    line->SetLineColor(kBlack);
    line->SetFillStyle(1001);
    line->SetFillColor(kBlack);
    line->Draw("LSame");
    line->Draw("FSame");
}

void TrackerGeometryAnalyzer::drawEtaGrid(double etamax, double xmax, double ymax)
{
    for (double eta=0.0; eta<etamax; eta+=0.2)
    {
        double theta=2.0*atan(exp(-eta));
        double vy=std::sin(theta);
        double vx=std::cos(theta);
        
        double ry=ymax/fabs(vy); //length to the y-plane
        double rx=xmax/fabs(vx); //length to the x-plane
        
        double r=std::min(rx,ry);
        
        double xspace=8;
        double yspace=2;
        
        TLine* line1 = new TLine(0,0,r*vx,r*vy);
        line1->SetLineStyle(2);
        line1->SetLineWidth(2);
        line1->SetLineColor(kGray);
        line1->Draw("Same");
        TPaveText* pave1 = new TPaveText(r*vx-xspace,r*vy-yspace,r*vx+xspace,r*vy+yspace);
        //pave1->SetBorderSize(1);
        pave1->SetFillColor(kWhite);
        char* buf1 = new char[20];
        sprintf(buf1,"#eta=%3.2f",eta);
        pave1->AddText(buf1);
        pave1->Draw("Same");
        
        TLine* line2 = new TLine(0,0,-r*vx,r*vy);
        line2->SetLineStyle(2);
        line2->SetLineWidth(2);
        line2->SetLineColor(kGray);
        line2->Draw("Same");
        TPaveText* pave2 = new TPaveText(-r*vx-xspace,r*vy-yspace,-r*vx+xspace,r*vy+yspace);
        //pave2->SetBorderSize(1);
        pave2->SetFillColor(kWhite);
        char* buf2 = new char[20];
        sprintf(buf2,"#eta=%3.2f",-eta);
        pave2->AddText(buf2);
        pave2->Draw("Same");
    }
}

void TrackerGeometryAnalyzer::drawPhiGrid(double xmax, double ymax, int N)
{
    double PI = std::atan(1)*4;
    for (int iphi=0.0; iphi<N; ++iphi)
    {
        for (int isector=0; isector<4;++isector)
        {
            double phiDeg = 1.0*iphi/N*90.0+isector*90.0;
            double phiRad=1.0*iphi/N*PI*0.5+isector*PI*0.5;
            double vy=std::sin(phiRad);
            double vx=std::cos(phiRad);
            
            double ry=ymax/fabs(vy); //length to the y-plane
            double rx=xmax/fabs(vx); //length to the x-plane
            
            double r=std::min(rx,ry);
            
            double xspace=8;
            double yspace=2;
        
        
            TLine* line1 = new TLine(0,0,r*vx,r*vy);
            line1->SetLineStyle(2);
            line1->SetLineWidth(2);
            line1->SetLineColor(kGray);
            line1->Draw("Same");
            TPaveText* pave1 = new TPaveText(r*vx-xspace,r*vy-yspace,r*vx+xspace,r*vy+yspace);
            //pave1->SetBorderSize(1);
            pave1->SetFillColor(kWhite);
            char* buf1 = new char[20];
            sprintf(buf1,"#phi=%3.2f",phiDeg);
            pave1->AddText(buf1);
            pave1->Draw("Same");
            
        }
    }
}
 

void
TrackerGeometryAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
}


// ------------ method called once each job just before starting event loop  ------------
void 
TrackerGeometryAnalyzer::beginJob()
{
    _cvZPerp = _fs->make<TCanvas>("detectorZPerp","",1200,900);
    _cvXY = _fs->make<TCanvas>("detectorXY","",900,900);
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TrackerGeometryAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------

void 
TrackerGeometryAnalyzer::beginRun(edm::Run const&, edm::EventSetup const& es)
{
    es.get<TrackerDigiGeometryRecord>().get(_trackerGeometry);
    es.get<IdealGeometryRecord>().get(_trackerTopology);
    
    _cvZPerp->cd();
    TH2F* axisZPerp = new TH2F("axisZPerp","",100,-320,320,100,0,130);
    axisZPerp->Draw("AXIS");
    //_cv->SetLogx();
    //_cv->SetLogy();
    drawEtaGrid(3.1, 300, 120);
    
    _cvXY->cd();
    TH2F* axisXY = new TH2F("axisXY","",100,-130,130,100,-130,130);
    axisXY->Draw("AXIS");
    drawPhiGrid(120, 120, 9);
    
    const std::vector<DetId> detIdList = _trackerGeometry->detIds();
    for (unsigned int idet = 0; idet<detIdList.size();++idet)
    {
        const DetId& detId = detIdList[idet];
        //std::cout<<"idet: "<<idet<<", name="<<_trackerTopology->print(detId)<<std::endl;
        _cvZPerp->cd();
        drawDetComponentZPerp(detId);
        _cvXY->cd();
        drawDetComponentXY(detId);
    }
}

void
TrackerGeometryAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TrackerGeometryAnalyzer);
