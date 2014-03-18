#!/usr/bin/python
import sys
import sqlite3
import logging
from optparse import OptionParser
import ROOT
from ROOT import *
import operator



gROOT.Reset()
gROOT.SetBatch(True)
gROOT.SetStyle("Plain")
gStyle.SetOptStat(0)
gStyle.SetOptFit(1111)
gStyle.SetPadLeftMargin(0.1)
gStyle.SetPadRightMargin(0.1)
gStyle.SetPadBottomMargin(0.1)
gStyle.SetMarkerSize(0.16)
gStyle.SetHistLineWidth(1)
gStyle.SetStatFontSize(0.025)
gStyle.SetTitleFontSize(0.05)
gStyle.SetTitleSize(0.06, "XYZ")
gStyle.SetLabelSize(0.05, "XYZ")
gStyle.SetNdivisions(510, "XYZ")

gStyle.SetOptFit()
gStyle.SetOptStat(0)

# For the canvas:
gStyle.SetCanvasBorderMode(0)
gStyle.SetCanvasColor(TStyle.kWhite)
gStyle.SetCanvasDefH(600) #Height of canvas
gStyle.SetCanvasDefW(600) #Width of canvas
gStyle.SetCanvasDefX(0)   #POsition on screen
gStyle.SetCanvasDefY(0)

# For the Pad:
gStyle.SetPadBorderMode(0)
# gStyle.SetPadBorderSize(Width_t size = 1)
gStyle.SetPadColor(TStyle.kWhite)
gStyle.SetPadGridX(False)
gStyle.SetPadGridY(False)
gStyle.SetGridColor(1)
gStyle.SetGridStyle(2)
gStyle.SetGridWidth(1)

# For the frame:

gStyle.SetFrameBorderMode(0)
gStyle.SetFrameBorderSize(0)
gStyle.SetFrameFillColor(0)
gStyle.SetFrameFillStyle(0)
gStyle.SetFrameLineColor(1)
gStyle.SetFrameLineStyle(1)
gStyle.SetFrameLineWidth(0)

# For the histo:
# gStyle.SetHistFillColor(1)
# gStyle.SetHistFillStyle(0)
gStyle.SetHistLineColor(1)
gStyle.SetHistLineStyle(0)
gStyle.SetHistLineWidth(1)
# gStyle.SetLegoInnerR(Float_t rad = 0.5)
# gStyle.SetNumberContours(Int_t number = 20)

gStyle.SetEndErrorSize(2)
#gStyle.SetErrorMarker(20)
gStyle.SetErrorX(0.)

gStyle.SetMarkerStyle(20)
#gStyle.SetMarkerStyle(20)

#For the fit/function:
gStyle.SetOptFit(1)
gStyle.SetFitFormat("5.4g")
gStyle.SetFuncColor(2)
gStyle.SetFuncStyle(1)
gStyle.SetFuncWidth(1)

#For the date:
gStyle.SetOptDate(0)
# gStyle.SetDateX(Float_t x = 0.01)
# gStyle.SetDateY(Float_t y = 0.01)

# For the statistics box:
gStyle.SetOptFile(0)
gStyle.SetOptStat(0) # To display the mean and RMS:   SetOptStat("mr")
gStyle.SetStatColor(TStyle.kWhite)
gStyle.SetStatFont(42)
gStyle.SetStatFontSize(0.025)
gStyle.SetStatTextColor(1)
gStyle.SetStatFormat("6.4g")
gStyle.SetStatBorderSize(1)
gStyle.SetStatH(0.1)
gStyle.SetStatW(0.15)
# gStyle.SetStatStyle(Style_t style = 1001)
# gStyle.SetStatX(Float_t x = 0)
# gStyle.SetStatY(Float_t y = 0)


#gROOT.ForceStyle(True)
#end modified

# For the Global title:

gStyle.SetOptTitle(0)
gStyle.SetTitleFont(42)
gStyle.SetTitleColor(1)
gStyle.SetTitleTextColor(1)
gStyle.SetTitleFillColor(10)
gStyle.SetTitleFontSize(0.03)
# gStyle.SetTitleH(0) # Set the height of the title box
# gStyle.SetTitleW(0) # Set the width of the title box
#gStyle.SetTitleX(0.35) # Set the position of the title box
#gStyle.SetTitleY(0.986) # Set the position of the title box
# gStyle.SetTitleStyle(Style_t style = 1001)
#gStyle.SetTitleBorderSize(0)

# For the axis titles:
gStyle.SetTitleColor(1, "XYZ")
gStyle.SetTitleFont(42, "XYZ")
#gStyle.SetTitleSize(0.06, "XYZ")
gStyle.SetTitleSize(0.04, "XYZ")
# gStyle.SetTitleXSize(Float_t size = 0.02) # Another way to set the size?
# gStyle.SetTitleYSize(Float_t size = 0.02)
gStyle.SetTitleXOffset(0.95)
gStyle.SetTitleYOffset(1.2)
#gStyle.SetTitleOffset(1.1, "Y") # Another way to set the Offset

# For the axis labels:

gStyle.SetLabelColor(1, "XYZ")
gStyle.SetLabelFont(42, "XYZ")
gStyle.SetLabelOffset(0.0075, "XYZ")
gStyle.SetLabelSize(0.03, "XYZ")
#gStyle.SetLabelSize(0.04, "XYZ")

# For the axis:

gStyle.SetAxisColor(1, "XYZ")
gStyle.SetStripDecimals(True)
gStyle.SetTickLength(0.03, "XYZ")
gStyle.SetNdivisions(510, "XYZ")

gStyle.SetPadTickX(1)  # To get tick marks on the opposite side of the frame
gStyle.SetPadTickY(1)



#gStyle.SetPalette(1) #(1,0)

# another top group addition
gStyle.SetHatchesSpacing(1.0)

# Postscript options:
#gStyle.SetPaperSize(20., 20.)
#gStyle.SetPaperSize(TStyle.kA4)
#gStyle.SetPaperSize(27., 29.7)
#gStyle.SetPaperSize(27., 29.7)
TGaxis.SetMaxDigits(3)
gStyle.SetLineScalePS(2)

# gStyle.SetLineStyleString(Int_t i, const char* text)
# gStyle.SetHeaderPS(const char* header)
# gStyle.SetTitlePS(const char* pstitle)
#gStyle.SetColorModelPS(1)

# gStyle.SetBarOffset(Float_t baroff = 0.5)
# gStyle.SetBarWidth(Float_t barwidth = 0.5)
# gStyle.SetPaintTextFormat(const char* format = "g")
# gStyle.SetPalette(Int_t ncolors = 0, Int_t* colors = 0)
# gStyle.SetTimeOffset(Double_t toffset)
# gStyle.SetHistMinimumZero(kTRUE)
gStyle.SetPalette(1)

class FunctionProfile:
    def __init__(self,sqlcursor,functionName):
        self._sqlcursor=sqlcursor
        self._functionName=functionName
        
        functionId = self.getIdFromName(functionName)
        if len(functionId)==0:
            raise Exception("'"+functionName+"' not found")
        self._functionId = functionId[0]
        self._functionTitle = self.getNamesFromIdList([self._functionId])[self._functionId]
        
        self._idChildrenList = self.getChildrenIds(self._functionId)
        self._idToNamesDict = self.getNamesFromIdList(self._idChildrenList)
        self._namesToIdDict = {}
        for idKey in self._idToNamesDict.keys():
           self._namesToIdDict[self._idToNamesDict[idKey]]=idKey
        self._idToCountDict = self.getCountFromIdList(self._idChildrenList)
        
        self._totalChildrenCount=0.0
        self._numChildren=len(self._idChildrenList)
        self._sortedByCount = sorted(self._idToCountDict.iteritems(), key=operator.itemgetter(1),reverse=True)
        self._maxSingleCount=self._sortedByCount[0][1]
        
        for value in self._idToCountDict.values():
            self._totalChildrenCount+=value
            
        self._totalCount=self._getTotalCount()
            
    def getChildrenIdDict(self):
        return self._idToNamesDict
        
    def getChildrenNamesDict(self):
        return self._namesToIdDict
        
    def getChildrenCountDict(self):
        return self._idToCountDict
        
    def _getTotalCount(self):
        for row in self._sqlcursor.execute("SELECT summary.total_count FROM summary"):
            return int(row[0])
        
    def _getParentIds(nameid):
        parentIds=[]
        for row in self._sqlcursor.execute("SELECT parents.self_id FROM parents WHERE parents.child_id='"+str(nameid)+"' AND parents.to_child_count>"+str(threshold)):
            parentIds.append(row[0])
        return parentIds
        
    def _getChildrenIds(self,nameid):
        childIds=[]
        for row in self._sqlcursor.execute("SELECT children.self_id FROM children WHERE children.parent_id='"+str(nameid)+"' AND children.from_parent_count>"+str(threshold)):
            childIds.append(row[0])
        return childIds
      
    def getNameFromId(self,nameid):
        result=[]
        for row in self._sqlcursor.execute("SELECT symbols.name, mainrows.id FROM mainrows INNER JOIN symbols ON symbols.id in (mainrows.symbol_id) WHERE mainrows.id LIKE "+str(nameid)):
            result.append(row[0])
        return result
        
    def getIdFromName(self,name):
        result=[]
        for row in self._sqlcursor.execute("SELECT symbols.name, mainrows.id FROM mainrows INNER JOIN symbols ON symbols.id in (mainrows.symbol_id) WHERE symbols.name LIKE "+str(name)):
            result.append(row[1])
        return result

    def _getCountFromIdList(self,nameidList):
        result={}
        cmd="SELECT mainrows.cumulative_count, mainrows.id FROM mainrows WHERE "
        for index in range(len(nameidList)):
            nameid=nameidList[index]
            if index==0:
                cmd+="mainrows.id="+str(nameid)+" "
            else:
                cmd+="OR mainrows.id="+str(nameid)+" "
        for row in self._sqlcursor.execute(cmd):
            result[int(row[1])]=row[0]
        return result
        
    def _getNamesFromIdList(self,nameidList):
        result={}
        cmd="SELECT symbols.name, mainrows.id FROM mainrows INNER JOIN symbols ON symbols.id in (mainrows.symbol_id) WHERE "
        for index in range(len(nameidList)):
            nameid=nameidList[index]
            if index==0:
                cmd+="mainrows.id="+str(nameid)+" "
            else:
                cmd+="OR mainrows.id="+str(nameid)+" "
        total=0.0
        for row in self._sqlcursor.execute(cmd):
            result[int(row[1])]=row[0]
        return result
        
    def _getIdsFromNameList(self,nameList):
        result={}
        cmd="SELECT symbols.name, mainrows.id FROM mainrows INNER JOIN symbols ON symbols.id in (mainrows.symbol_id) WHERE "
        for index in range(len(nameList)):
            nameid=nameList[index]
            if index==0:
                cmd+="symbols.name LIKE "+str(nameid)+" "
            else:
                cmd+="OR symbols.name LIKE "+str(nameid)+" "
        total=0.0
        for row in self._sqlcursor.execute(cmd):
            result[row[0]]=int(row[1])
        return result
        
def plotComparison(self,anotherProfile,outName="out.pdf"):
    comparison=[]
    
    for i in range(len(self._sortedByCount)):
        thisId = self._sortedByCount[i][0]
        name = self._idToNamesDict[thisId]
        thisCount=self._idToCountDict[thisId]
        otherCount=0
        if anotherProfile._namesToIdDict.has_key(name):
            otherId = anotherProfile._namesToIdDict[name]
            otherCount=scalefactor*anotherProfile._idToCountDict[thisId]
        print name,thisCount,otherCount
        
#def plotComposition(self,stopAt=0.99,outName="profile.pdf"):
    
    
if __name__=="__main__":
    pass
    
