import sys
import sqlite3
import logging
import random
from optparse import OptionParser
import ROOT
from ROOT import *
import operator

gStyle.SetOptStat(0)

class IgProfFile:
    def __init__(self, inputFile):
        self._inputFile=inputFile
        self._sqlcursor=sqlite3.connect(inputFile).cursor()
    
        self._totalCount=None
        self.getTotalCount()
        self._nameToIdDict={}
        self._idToNameDict={}
        self._idToCountDict={}
        self._idToFileDict={}
        self._idToChildrenDict={}
        self._idToParentDict={}
        #just cache the first 200 ids
        self.load(range(1,201))
        
    def getTotalCount(self):
        if self._totalCount==None:
            for row in self._sqlcursor.execute("SELECT summary.total_count FROM summary"):
                self._totalCount=int(row[0])
        return self._totalCount
        
    def addItem(self,name,idNum,count,fileName):
        self._idToNameDict[idNum]=name
        self._nameToIdDict[name]=idNum
        self._idToCountDict[idNum]=count
        self._idToFileDict[idNum]=fileName
        
    def load(self, idList):
        idsToLoad=[]
        for idNumber in idList:
            if self._idToNameDict.has_key(idNumber):
                continue
            idsToLoad.append(idNumber)
        if len(idsToLoad)==0:
            return
        strList="("
        for idNumber in idsToLoad:
            strList+="'"+str(idNumber)+"',"
        strList=strList[:-1]+")"
        print "loading: ",strList
        for row in self._sqlcursor.execute("SELECT symbols.name, mainrows.id, mainrows.cumulative_count, files.name FROM mainrows INNER JOIN symbols ON symbols.id in (mainrows.symbol_id) INNER JOIN files ON files.id in (symbols.filename_id) WHERE mainrows.id IN "+strList):
            name = row[0]
            idNum = int(row[1])
            count = int(row[2])
            fileName = str(row[3])
            self.addItem(name,idNum,count,fileName)
            
    def getNameFromId(self, idNumber):
        if not self._idToNameDict.has_key(idNumber):
            self.load([idNumber])
        return self._idToNameDict[idNumber]
        
    def getCountFromId(self,idNumber):
        if not self._idToCountDict.has_key(idNumber):
            self.load([idNumber])
        return self._idToCountDict[idNumber]
        
    def getFileNameFromId(self,idNumber):
        if not self._idToFileDict.has_key(idNumber):
            self.load([idNumber])
        return self._idToFileDict[idNumber]
        
    def findIdsFromName(self,name):
        matches={}
        if not self._nameToIdDict.has_key(name):
            for row in self._sqlcursor.execute("SELECT symbols.name, mainrows.id, mainrows.cumulative_count, files.name FROM mainrows INNER JOIN symbols ON symbols.id in (mainrows.symbol_id) INNER JOIN files ON files.id in (symbols.filename_id) WHERE symbols.name LIKE "+str(name)):
                name = row[0]
                idNum = int(row[1])
                count = int(row[2])
                fileName = str(row[3])
                self.addItem(name,idNum,count,fileName)
                matches[name]=idNum
        else:
            matches[name]=self._nameToIdDict[name]
        return matches
        
    def __str__(self):
        ret="file: "+self._inputFile+"\n"
        ret+="  total count: "+str(self._totalCount)+"\n"
        for idNumber in self._idToNameDict.keys():
            ret+="  ["+str(idNumber)+"] "+str(self._idToCountDict[idNumber])+" "+self._idToNameDict[idNumber]+"\n" 
            if self._idToParentDict.has_key(idNumber):
                ret+="      -> "+str(self._idToParentDict[idNumber])+"\n"
            else:
                ret+="      -> <not loaded>\n"
            if self._idToChildrenDict.has_key(idNumber):
                ret+="      <- "+str(self._idToChildrenDict[idNumber])+"\n"
            else:
                ret+="      <- <not loaded>\n"
        return ret[:-1]
        
    def getChildrenIds(self,idNumber):
        if not self._idToChildrenDict.has_key(idNumber):
            childIds=[]
            for row in self._sqlcursor.execute("SELECT children.self_id FROM children WHERE children.parent_id='"+str(idNumber)+"'"):
                childId=int(row[0])
                childIds.append(childId)
            self._idToChildrenDict[idNumber]=childIds
            self.load(childIds)
        return self._idToChildrenDict[idNumber]
        
    def getParentIds(self,idNumber):
        if not self._idToParentDict.has_key(idNumber):
            parentIds=[]
            for row in self._sqlcursor.execute("SELECT parents.self_id FROM parents WHERE parents.child_id='"+str(idNumber)+"'"):
                parentId=int(row[0])
                parentIds.append(parentId)
            self._idToParentDict[idNumber]=parentIds
            self.load(parentIds)
        return self._idToParentDict[idNumber]
    
    def findFunction(self,name):
        matchedNames = self.findIdsFromName(name)
        functions = {}
        for k in matchedNames.keys():
            functions[k]=Function(self,matchedNames[k])
        return functions
        if (len(matchedNames.keys())==0):
            raise Exception("no matches where found: "+name)
    
       
class Function:
    def __init__(self,igProfFile,idNumber):
        self._igProfFile=igProfFile
        self._id=idNumber
        self._name=self._igProfFile.getNameFromId(self._id)
        self._count=self._igProfFile.getCountFromId(self._id)
        self._fileName=self._igProfFile.getFileNameFromId(self._id)
        
        self._childrenIdList=self._igProfFile.getChildrenIds(self._id)
        self._childrenNameDict={}
        self._childrenCountDict={}
        self._childrenFileDict={}
        for childIdNumber in self._childrenIdList:
            self._childrenNameDict[childIdNumber]=self._igProfFile.getNameFromId(childIdNumber)
            self._childrenCountDict[childIdNumber]=self._igProfFile.getCountFromId(childIdNumber)
            self._childrenFileDict[childIdNumber]=self._igProfFile.getFileNameFromId(childIdNumber)
            
        self._parentIdList=self._igProfFile.getParentIds(self._id)
        self._parentNameDict={}
        self._parentCountDict={}
        self._parentFileDict={}
        for parentIdNumber in self._parentIdList:
            self._parentNameDict[parentIdNumber]=self._igProfFile.getNameFromId(parentIdNumber)
            self._parentCountDict[parentIdNumber]=self._igProfFile.getCountFromId(parentIdNumber)
            self._parentFileDict[childIdNumber]=self._igProfFile.getFileNameFromId(childIdNumber)
            
    def getCount(self):
        return self._count
        
    def getName(self):
        return self._name
        
    def getFileName(self):
        return self._fileName

    def getSortedCountComposition(self):
        sortedList=[]
        for childId in self._childrenCountDict:
            sortedList.append({"id":childId,"count":self._childrenCountDict[childId],"name":self._childrenNameDict[childId],"fileName":self._childrenFileDict[childId]})
        sortedList.sort(key=lambda child: child["count"],reverse=True)
        return sortedList
        
class CompositionPlot:
    def __init__(self,function):
        self._function=function
        self._sortedComposition=self._function.getSortedCountComposition()
        
    def plot(self, scaleFactor=1.0, log=True,displayOnly=-1,formatName=lambda name: name.split("(")[0]):
        strippedCountList=[]
        maximum=0.0
        minimum=1.0
        if displayOnly<0:
            for index in range(len(self._sortedComposition)):
                strippedCountList.append(self._sortedComposition[index])
                maximum=max(maximum,self._sortedComposition[index]["count"])
        else:
            for index in range(displayOnly):
                strippedCountList.append(self._sortedComposition[index])
                maximum=max(maximum,self._sortedComposition[index]["count"])
            remainingCounts=0.0
            for index in range(displayOnly,len(self._sortedComposition)):
                remainingCounts+=self._sortedComposition[index]["count"]
            strippedCountList.append({"id":-1,"count":remainingCounts,"name":"other","fileName":"other"})
            maximum=max(maximum,remainingCounts)
        n=len(strippedCountList)
        axis=ROOT.TH2F("axis"+str(random.random()),";counts;methods",500,minimum*scaleFactor,maximum*1.1*scaleFactor,n,0.5,n+0.5)
        canvas=ROOT.TCanvas("composition"+str(random.random()),"",900,800)
        axis.Draw("AXIS")
        
        rootObj=[]
        
        for index in range(len(strippedCountList)):
            posy=index+1.0
            posx=strippedCountList[index]["count"]*scaleFactor
            box=ROOT.TBox(minimum*scaleFactor,posy-0.3,posx,posy+0.3)
            rootObj.append(box)
            box.SetFillColor(ROOT.kOrange+6)
            box.Draw("SameF")
            pText=ROOT.TPaveText(minimum*scaleFactor,posy-0.3,maximum*1.1*scaleFactor,posy+0.3)
            pText.SetFillStyle(0)
            rootObj.append(pText)
            pText.AddText(formatName(strippedCountList[index]["fileName"]))
            pText.SetTextAlign(12)
            pText.Draw("Same")
        canvas.SetLogx(log)
        axis.Draw("AXIS Same")
        canvas.Update()
        canvas.WaitPrimitive()
        
class ComparisonPlot:
    def __init__(self,function1,function2,match=lambda x,y: x==y):
        self._function1=function1
        self._function2=function2
        self._sortedComposition1=self._function1.getSortedCountComposition()
        self._sortedComposition2=self._function2.getSortedCountComposition()
        
        self._matchedFunctions=[]
        # note: the operator [:] copies the list
        for item1 in self._sortedComposition1[:]:
            for item2 in self._sortedComposition2[:]:
                if match(item1["name"],item2["name"]):
                    self._matchedFunctions.append({"name":item1["name"],"count1":item1["count"],"count2":item2["count"]})
                    self._sortedComposition1.remove(item1)
                    self._sortedComposition2.remove(item2)
                    break
        '''                 
        for item in self._matchedFunctions:
            print "matched: ",item["name"]#.split("(")[0]
        '''
        for item in self._sortedComposition1:
            print "unmatched1: ",item["name"]#.split("(")[0]
            
        for item in self._sortedComposition2:
            print "unmatched2: ",item["name"]#.split("(")[0]
            
            
    def plot(self, scaleFactor1=1.0, scaleFactor2=1.0, log=True,displayOnly=-1,formatName=lambda name: name.split("(")[0], formatDiff=lambda x,y: str(round((y-x)/x*100.0,1))+"%"):
        strippedCountList=[]
        maximum=0.0
        minimum=1.0
        minimum=min(minimum*scaleFactor1,minimum*scaleFactor2)
        if displayOnly<0:
            for index in range(len(self._matchedFunctions)):
                strippedCountList.append(self._matchedFunctions[index])
                maximum=max(maximum,self._matchedFunctions[index]["count1"]*scaleFactor1)
                maximum=max(maximum,self._matchedFunctions[index]["count2"]*scaleFactor2)
        else:
            for index in range(displayOnly):
                strippedCountList.append(self._matchedFunctions[index])
                maximum=max(maximum,self._matchedFunctions[index]["count1"]*scaleFactor1)
                maximum=max(maximum,self._matchedFunctions[index]["count2"]*scaleFactor2)
            remainingCounts1=0.0
            remainingCounts2=0.0
            for index in range(displayOnly,len(self._matchedFunctions)):
                remainingCounts1+=self._matchedFunctions[index]["count1"]
                remainingCounts2+=self._matchedFunctions[index]["count2"]
            #add the unmatched as well
            for item in self._sortedComposition1:
                remainingCounts1+=item["count"]
            for item in self._sortedComposition2:
                remainingCounts2+=item["count"]
                
            strippedCountList.append({"count1":remainingCounts1,"count2":remainingCounts2,"name":"other"})
            maximum=max(maximum,remainingCounts1*scaleFactor1)
            maximum=max(maximum,remainingCounts2*scaleFactor2)
        n=len(strippedCountList)
        axis=ROOT.TH2F("axis"+str(random.random()),";counts;methods",500,minimum,maximum*1.1,n,0.5,n+0.5)
        canvas=ROOT.TCanvas("composition"+str(random.random()),"",900,800)
        axis.Draw("AXIS")
        
        rootObj=[]
        
        for index in range(len(strippedCountList)):
            posy=index+1.0
            posx1=strippedCountList[index]["count1"]*scaleFactor1
            posx2=strippedCountList[index]["count2"]*scaleFactor2
            box=ROOT.TBox(minimum,posy-0.3,posx1,posy+0.3)
            rootObj.append(box)
            box.SetFillColor(ROOT.kOrange+6)
            box.Draw("SameF")
            marker=ROOT.TMarker(posx2,posy,21)
            rootObj.append(marker)
            marker.SetMarkerColor(ROOT.kBlue)
            marker.SetMarkerSize(1.2)
            marker.Draw("Same")
            pText=ROOT.TPaveText(minimum,posy-0.3,maximum*1.1,posy+0.3)
            pText.SetFillStyle(0)
            rootObj.append(pText)
            pText.AddText(formatName(strippedCountList[index]["name"]))
            pText.SetTextAlign(12)
            pText.Draw("Same")
            
            pPercentage=ROOT.TPaveText(minimum,posy-0.3,maximum*1.1,posy+0.3)
            pPercentage.SetFillStyle(0)
            pPercentage.SetBorderSize(0)
            rootObj.append(pPercentage)
            pPercentage.AddText(formatDiff(posx1,posx2))
            pPercentage.SetTextAlign(32)
            pPercentage.Draw("Same")
        canvas.SetLogx(log)
        axis.Draw("AXIS Same")
        canvas.Update()
        canvas.WaitPrimitive()
            

if __name__=="__main__":
    parser = OptionParser()
    #parser.add_option("-f", "--file", dest="filename",help="write report to FILE", metavar="FILE")
    #parser.add_option("-q", "--quiet",action="store_false", dest="verbose", default=True,help="don't print status messages to stdout")

    (options, args) = parser.parse_args()
    profile1 = IgProfFile(args[0])
    #functions = profile1.findFunction("'%CalorimetryManager::reconstruct%'")
    functions = profile1.findFunction("'%edm::stream::EDProducerAdaptorBase::doEvent(edm::EventPrincipal%'")
    print functions.keys()
    #functions = profile1.findFunction("'%edm::%::doEvent(edm::EventPrincipal%'")
    
    #profile2 = IgProfFile(args[1])
    #function2 = profile2.findFunction("'%edm::EDProducer::doEvent%'")
    
    #print args[1],": contribution: ",round(
    #print "TrajectorySeedProducer2::produce: ",profile2.getCountFromId(profile2.findIdsFromName("'%TrajectorySeedProducer2::produce%'").values()[0])
    #print "total: ",profile2.getTotalCount(), function2.getCount()
    #comparsionPlot=ComparisonPlot(function1,function2,match=lambda x,y: (x==y) or (x.find("TrajectorySeedProducer")!=-1 and y.find("TrajectorySeedProducer")!=-1))
    #comparsionPlot.plot(displayOnly=30,log=1)
    '''
    for k in functions.keys():
        print k
        compositionPlot = CompositionPlot(functions[k])
        compositionPlot.plot()
    '''
    compositionPlot = CompositionPlot(functions.values()[1])
    compositionPlot.plot(displayOnly=35,scaleFactor=0.006/20.)
