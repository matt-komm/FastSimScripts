from Profile import Profile

if __name__=="__main__":
    parser = OptionParser()
    parser.add_option("-f", "--function", action="append", dest="functions",default=["edm::EDProducer::doEvent"], help="write report to FILE", metavar="FILE")
    #parser.add_option("-q", "--quiet",action="store_false", dest="verbose", default=True,help="don't print status messages to stdout")

    (options, args) = parser.parse_args()
    profile1 = IgProfFile(args[0])
    function1 = profile1.findFunction("'%edm::EDProducer::doEvent%'")
    profile2 = IgProfFile(args[1])
    function2 = profile2.findFunction("'%edm::EDProducer::doEvent%'")
    comparsionPlot=ComparisonPlot(function1,function2,match=lambda x,y: (x==y) or (x.find("TrajectorySeedProducer")!=-1 and y.find("TrajectorySeedProducer")!=-1))
    comparsionPlot.plot(displayOnly=30,log=1)
    '''
    compositionPlot = CompositionPlot(function)
    compositionPlot.plot(displayOnly=20)
    '''#!/usr/bin/python

print 'Hello World'
