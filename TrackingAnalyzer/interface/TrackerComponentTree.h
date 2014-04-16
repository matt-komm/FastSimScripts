#ifndef TRACKERCOMPONENTTREE_H
#define TRACKERCOMPONENTTREE_H

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/SiStripDetId/interface/SiStripDetId.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/SiStripDetId/interface/StripSubdetector.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "FastSimScripts/TrackingAnalyzer/interface/TrackerComponentIdentifier.h"
#include "FWCore/Utilities/interface/Exception.h"
#include <sstream>
#include <vector>
#include <iostream>

class TrackerComponentTree
{
    private:
        class Node
        {
            private:
                std::string _group;
                unsigned int _id;
                unsigned int _currentDepth;
                std::vector<TrackerComponentTree::Node*> _children;
                std::vector<const TrackerComponentIdentifier*> _leafs;
            public:
                Node(std::string group, unsigned int id, unsigned int depth):
                    _group(group),
                    _id(id),
                    _currentDepth(depth)
                {
                }
                
                bool insert(const TrackerComponentIdentifier* tci)
                {
                    std::pair<std::string,unsigned int> group = tci->getGroup(_currentDepth);
                    if (group.first!=_group || group.second!=_id)
                    {
                        //wrong Node
                        return false;
                    }
                    if (_currentDepth<(tci->getMaxDepth()-1))
                    {
                        //this is only a node and not a leafs
                        
                        //check first for an appropriate child
                        bool inserted = false;
                        for (unsigned int ichild=0; ichild<_children.size(); ++ichild)
                        {
                            inserted=inserted || _children[ichild]->insert(tci);
                        }
                        
                        //not inserted - create child
                        if (!inserted)
                        {
                            std::pair<std::string,unsigned int> nextGroup = tci->getGroup(_currentDepth+1);
                            TrackerComponentTree::Node* node = new TrackerComponentTree::Node(nextGroup.first,nextGroup.second,_currentDepth+1);
                            _children.push_back(node);
                            return node->insert(tci);
                        }
                        return true;
                    }
                    else
                    {
                        _leafs.push_back(tci);
                        return true;
                    }
                }
                
                void print()
                {
                    
                    for (unsigned int i=0;i<_currentDepth;++i)
                    {
                        std::cout<<"  ";
                    }
                    std::cout<<"group="<<_group<<", id="<<_id<<", leafs="<<_leafs.size()<<std::endl;
                    for (unsigned int ichild=0; ichild<_children.size(); ++ichild)
                    {
                        _children[ichild]->print();
                    }
                    
                }
        };
        
        std::vector<TrackerComponentTree::Node*> _subdets;
        
    public:
        TrackerComponentTree()
        {
            
        } 
          
        ~TrackerComponentTree()
        {
        }
        
        void insert(const TrackerComponentIdentifier* tci)
        {
            bool inserted=false;
            for (unsigned int isubdet=0;isubdet<_subdets.size();++isubdet)
            {
                inserted=inserted || _subdets[isubdet]->insert(tci);
            }
            if (!inserted)
            {
                std::pair<std::string,unsigned int> group = tci->getGroup(0);
                TrackerComponentTree::Node* node = new TrackerComponentTree::Node(group.first,group.second,0);
                _subdets.push_back(node);
                node->insert(tci);
            }
        }
        
        void print()
        {

            std::cout<<"subdets="<<_subdets.size()<<std::endl;
            for (unsigned int isubdet=0;isubdet<_subdets.size();++isubdet)
            {
                _subdets[isubdet]->print();
            }
        }
        
        
};

#endif
