#ifndef TRACKERCOMPONENTIDENTIFIER_H
#define TRACKERCOMPONENTIDENTIFIER_H

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/SiStripDetId/interface/SiStripDetId.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/SiStripDetId/interface/StripSubdetector.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "FWCore/Utilities/interface/Exception.h"
#include <sstream>

class TrackerComponentIdentifier
{
    private:
        std::string _subDetName;
        std::vector<std::pair<std::string,unsigned int>> _subDetGroupList;
        
        TrackerComponentIdentifier()
        {
        }
        
        void addSubDetGroup(std::string group, unsigned int id)
        {
            _subDetGroupList.push_back(std::pair<std::string,unsigned int>(group,id));
        }
        
    public:
        ~TrackerComponentIdentifier()
        {
        }
        
        unsigned int getMaxDepth() const
        {
            return _subDetGroupList.size();
        }
        
        std::pair<std::string,unsigned int> getGroup(unsigned int depth) const
        {
            if (depth>_subDetGroupList.size())
            {
                return std::pair<std::string,unsigned int>("undefined",-1);
            }
            return _subDetGroupList[depth];
        }
        
        std::string print() const
        {
            std::stringstream ss;
            ss<<_subDetName;
            for (unsigned int igroup = 0; igroup<_subDetGroupList.size();++igroup)
            {
                ss<<", "+_subDetGroupList[igroup].first+"=";
                ss<<_subDetGroupList[igroup].second;
            }
            return ss.str();
        }

        static const TrackerComponentIdentifier* create(const DetId& id, const TrackerTopology& topology) 
        {
            TrackerComponentIdentifier* tci = new TrackerComponentIdentifier();
            uint32_t subdet=id.subdetId();
            tci->addSubDetGroup("SubDet",subdet);

            if ( subdet == PixelSubdetector::PixelBarrel ) 
            {
                tci->_subDetName="PixelBarrel"; 
                tci->addSubDetGroup("Layer",topology.pxbLayer(id));
                tci->addSubDetGroup("Ladder",topology.pxbLadder(id));
                tci->addSubDetGroup("Module",topology.pxbModule(id));
                return tci;
            } 
            else if ( subdet == PixelSubdetector::PixelEndcap ) 
            {
                tci->_subDetName="PixelEndcap";
                tci->addSubDetGroup("Disk",topology.pxfDisk(id));
                tci->addSubDetGroup("Blade",topology.pxfBlade(id));
                tci->addSubDetGroup("Panel",topology.pxfPanel(id));
                tci->addSubDetGroup("Module",topology.pxfModule(id));
                return tci;
            }
            
            if ( subdet == StripSubdetector::TIB ) 
            {
                tci->_subDetName="TIB";
                
                std::vector<unsigned int> infoString = topology.tibStringInfo(id);
                if (infoString[0] == 1 )
                {
                    tci->addSubDetGroup("negSide",0);
                }
                else
                {
                    tci->addSubDetGroup("posSide",1);
                }
                if (infoString[1] == 1 )
                {
                    tci->addSubDetGroup("intPart",0);
                }
                else
                {
                    tci->addSubDetGroup("extPart",1);
                }
                tci->addSubDetGroup("Layer",topology.tibLayer(id));
                tci->addSubDetGroup("Module",topology.tibModule(id));
                
                /*
                std::string side;
                std::string part;
                side = (theString[0] == 1 ) ? "-" : "+";
                part = (theString[1] == 1 ) ? "int" : "ext";
                std::string type;
                type = (tibStereo(id) == 0) ? "r-phi" : "stereo";
                type = (tibGlued(id) == 0) ? type : type+" glued";
                type = (tibIsDoubleSide(id)) ? "double side" : type;
                strstr << "TIB" << side
                << " Layer " << theLayer << " " << part
                << " String " << theString[2]
                << " Module " << theModule << " " << type
                << " (" << id.rawId() << ")";
                */
                return tci;
            }
            return nullptr;
            /*
            if ( subdet == StripSubdetector::TID ) 
            {
            unsigned int         theDisk   = tidWheel(id);
            unsigned int         theRing   = tidRing(id);
            std::vector<unsigned int> theModule = tidModuleInfo(id);
            std::string side;
            std::string part;
            side = (tidSide(id) == 1 ) ? "-" : "+";
            part = (theModule[0] == 1 ) ? "back" : "front";
            std::string type;
            type = (tidStereo(id) == 0) ? "r-phi" : "stereo";
            type = (tidGlued(id) == 0) ? type : type+" glued";
            type = (tidIsDoubleSide(id)) ? "double side" : type;
            strstr << "TID" << side
            << " Disk " << theDisk
            << " Ring " << theRing << " " << part
            << " Module " << theModule[1] << " " << type
            << " (" << id.rawId() << ")";
            return strstr.str();
            }

            if ( subdet == StripSubdetector::TOB ) 
            {
            unsigned int              theLayer  = tobLayer(id);
            std::vector<unsigned int> theRod    = tobRodInfo(id);
            unsigned int              theModule = tobModule(id);
            std::string side;
            std::string part;
            side = (theRod[0] == 1 ) ? "-" : "+";
            std::string type;
            type = (tobStereo(id) == 0) ? "r-phi" : "stereo";
            type = (tobGlued(id) == 0) ? type : type+" glued";
            type = (tobIsDoubleSide(id)) ? "double side" : type;
            strstr << "TOB" << side
            << " Layer " << theLayer
            << " Rod " << theRod[1]
            << " Module " << theModule << " " << type
            << " (" << id.rawId() << ")";
            return strstr.str();
            }

            if ( subdet == StripSubdetector::TEC ) 
            {
            unsigned int              theWheel  = tecWheel(id);
            unsigned int              theModule = tecModule(id);
            std::vector<unsigned int> thePetal  = tecPetalInfo(id);
            unsigned int              theRing   = tecRing(id);
            std::string side;
            std::string petal;
            side  = (tecSide(id) == 1 ) ? "-" : "+";
            petal = (thePetal[0] == 1 ) ? "back" : "front";
            std::string type;
            type = (tecStereo(id) == 0) ? "r-phi" : "stereo";
            type = (tecGlued(id) == 0) ? type : type+" glued";
            type = (tecIsDoubleSide(id)) ? "double side" : type;
            strstr << "TEC" << side
            << " Wheel " << theWheel
            << " Petal " << thePetal[1] << " " << petal
            << " Ring " << theRing
            << " Module " << theModule << " " << type
            << " (" << id.rawId() << ")";

            return strstr.str();
            }*/


            throw cms::Exception("Invalid DetId") << "Unsupported DetId in TrackerTopology::module";
            return nullptr;
        }
};

#endif
