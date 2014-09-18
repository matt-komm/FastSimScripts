PREFIX="testing_"
CMSSWV="CMSSW_7_2_X_2014-09-16-0200"
GITBRANCH="new_iter"
#GITBRANCH="iter5-config-fix"
BASEDIR=`pwd`
OUTPUTDIR=/afs/cern.ch/user/m/mkomm/FastSim/dev_scl6/testing
STATFILE=$BASEDIR/stat.log
GITFILE=$BASEDIR/git.log
COMPILEFILE=$BASEDIR/compile.log

echo "--------------------env------------------"
echo "--------------------env------------------" > $STATFILE 2>&1
env >> $STATFILE 2>&1

echo "--------------------pwd------------------"
echo "--------------------pwd------------------" >> $STATFILE 2>&1
pwd >> $STATFILE 2>&1

echo "--------------------ls------------------"
echo "--------------------ls------------------" >> $STATFILE 2>&1
ls -lh >> $STATFILE 2>&1

echo "--------------------setup bash------------------"
echo "--------------------setup bash------------------" >> $STATFILE 2>&1
source $HOME/.bash_profile >> $STATFILE 2>&1
source $HOME/.bashrc >> $STATFILE 2>&1

echo "--------------------setup gittools------------------"
echo "--------------------setup gittools------------------" >> $STATFILE 2>&1
git clone https://github.com/cms-sw/cms-git-tools.git > $GITFILE 2>&1
cat $GITFILE >> $STATFILE

echo "--------------------cmssw------------------"
echo "--------------------cmssw------------------" >> $STATFILE 2>&1
scram project -n ./$PREFIX$CMSSWV CMSSW $CMSSWV >> $STATFILE 2>&1
cd ./$PREFIX$CMSSWV/src >> $STATFILE 2>&1
eval `scram runtime -sh` >> $STATFILE 2>&1
export PATH=$BASEDIR/cms-git-tools:$PATH

echo "--------------------git------------------"
echo "--------------------git------------------" >> $STATFILE 2>&1
git cms-init > $GITFILE 2>&1
cat $GITFILE >> $STATFILE
git cms-merge-topic matt-komm:$GITBRANCH > $GITFILE 2>&1
cat $GITFILE >> $STATFILE
git read-tree -mu HEAD > $GITFILE 2>&1
cat $GITFILE >> $STATFILE
git clone https://github.com/matt-komm/FastSimScripts.git > $GITFILE 2>&1
cat $GITFILE >> $STATFILE

echo "--------------------compile------------------"
echo "--------------------compile------------------" >> $STATFILE 2>&1
scram b >> $COMPILEFILE 2>&1
cat $COMPILEFILE >> $STATFILE

echo "--------------------validation------------------"
echo "--------------------validation------------------" >> $STATFILE 2>&1 
$BASEDIR/$PREFIX$CMSSWV/src/FastSimScripts/Validation/runTTbar.sh $GITBRANCH 1000 2>&1 | tee -a $STATFILE 

echo "--------------------copy output------------------"
echo "--------------------copy output------------------" >> $STATFILE 2>&1 
cp -v $BASEDIR/$PREFIX$CMSSWV/src/TTbarValidation_$GITBRANCH/step2.root $OUTPUTDIR/step2_$CMSSWV_$GITBRANCH.root >> $STATFILE 2>&1 
cp -v $BASEDIR/$PREFIX$CMSSWV/src/TTbarValidation_$GITBRANCH/DQM_V0001_R000000001__Global__CMSSW_X_Y_Z__RECO.root $OUTPUTDIR/DQM_$CMSSWV_$GITBRANCH.root >> $STATFILE 2>&1 
cp $STATFILE $OUTPUTDIR/stat_$CMSSWV_$GITBRANCH.log 
