#!/usr/bin/env cmsRun

import FWCore.ParameterSet.Config as cms
import os
import sys

argv = sys.argv[2:]
try:
    outfile = sys.argv[2]
    if not outfile.endswith(".root") or os.path.exists(outfile):
        raise ValueError("First argument {} should end with .root and not exist".format(sys.argv[2]))

    infiles = sys.argv[3:]
    if not infiles:
        raise ValueError("No input files!")
    for filename in infiles:
        if not filename.endswith(".root") or not os.path.exists(filename):
            raise ValueError("Third argument and further {} should end with .root and exist".format(filename))
except:
    print sys.argv
    print "cmsRun", sys.argv[1], "outputfile.root inputfile1.root inputfile2.root ..."
    raise

process = cms.Process("TreeMaker")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.load("RecoJets.Configuration.GenJetParticles_cff")
process.load("RecoJets.JetProducers.ak5GenJets_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.TFileService = cms.Service("TFileService", 
    fileName = cms.string("file:"+outfile),
    closeFileFast = cms.untracked.bool(True)
)

process.load("TreeMaker.Utils.metdouble_cfi")

process.ZProducer = cms.EDProducer('ZProducer',
                                   ElectronTag = cms.InputTag('leptonproducer:IdIsoElectron'),
                                   MuonTag = cms.InputTag('leptonproducer:IdIsoMuon')
                                   )

process.JJProducer = cms.EDProducer('JJProducer',
                                    JetTag = cms.InputTag('slimmedJets'),
                                    genCollection = cms.InputTag('prunedGenParticles'),
                                    minPt  = cms.double(30.)
                                    )

# process.ZZProducer = cms.EDProducer("ZZProducer",
#                                     Z1Tag = cms.InputTag('JJProducer'),
#                                     Z2Tag = cms.InputTag('ZProducer')
#                                      )

process.htdouble = cms.EDProducer('HTDouble',
                                  JetTag = cms.InputTag('slimmedJets'),
                                  )

process.genParticleDump = cms.EDProducer('GenParticlesProducer',
                                         genCollection = cms.InputTag('prunedGenParticles'),
                                         debug = cms.bool(True),
                                         childIds = cms.vint32(1,2,3,4,5,11,12,13,14,15,16),
                                         parentIds = cms.vint32(1,2,6,23,24,25,))

from TreeMaker.TreeMaker.treeMaker import TreeMaker

process.TreeMaker = TreeMaker.clone(TreeName = cms.string('analysisTree'),
                                    VarsBool = cms.vstring(),
                                    VarsInt = cms.vstring(),
                                    VarsFloat = cms.vstring(),
                                    VarsDouble = cms.vstring(),
                                    VarsString = cms.vstring(),
                                    VarsTLorentzVector = cms.vstring(),
                                    VectorBool = cms.vstring(),
                                    VectorInt = cms.vstring(),
                                    VectorFloat = cms.vstring(),
                                    VectorDouble = cms.vstring(),
                                    VectorString = cms.vstring(),
                                    VectorTLorentzVector = cms.vstring(),
                                    VectorVectorTLorentzVector = cms.vstring(),
                                    VectorRecoCand = cms.vstring()
                                    )

process.leptonproducer = cms.EDProducer('LeptonProducer',
                                        MuonTag = cms.InputTag('slimmedMuons'),
                                        ElectronTag = cms.InputTag('slimmedElectrons'),
                                        PrimaryVertex = cms.InputTag('offlineSlimmedPrimaryVertices'),
                                        minElecPt  = cms.double(5),
                                        maxElecEta  = cms.double(2.5),
                                        minMuPt  = cms.double(5),
                                        maxMuEta  = cms.double(2.4),
                                        muIsoValue  = cms.double(0.2),
                                        elecIsoValue  = cms.double(0.2), # only has an effect with useMiniIsolation
                                        UseMiniIsolation = cms.bool(False),
                                        METTag = cms.InputTag('slimmedMETs'), 
                                        rhoCollection = cms.InputTag("fixedGridRhoFastjetAll")
                                        )

process.TreeMaker.VectorBool.append("leptonproducer:IdMuonTightID(muon_tightID)")
process.TreeMaker.VectorBool.append("leptonproducer:IdIsoMuonTightID(muon_tightIDIso)")
process.TreeMaker.VectorInt.append("leptonproducer:MuonCharge(muon_charge)")
process.TreeMaker.VectorRecoCand.append("leptonproducer:IdMuon(muons)")
process.TreeMaker.VectorBool.append("leptonproducer:IdElectronTightID(electron_tightID)")
process.TreeMaker.VectorBool.append("leptonproducer:IdIsoElectronTightID(electron_tightIDIso)")
process.TreeMaker.VectorInt.append("leptonproducer:ElectronCharge(electron_charge)")
process.TreeMaker.VectorRecoCand.append("leptonproducer:IdElectron(electrons)")
process.TreeMaker.VectorRecoCand.append("slimmedJets")
#process.TreeMaker.VectorRecoCand.append("slimmedElectrons")
#process.TreeMaker.VectorRecoCand.append("slimmedMuons")
process.TreeMaker.VectorRecoCand.append("slimmedJetsAK8")

process.TreeMaker.VectorRecoCand.append("ZProducer:ZCandidates(Zll)")
process.TreeMaker.VectorRecoCand.append("JJProducer:ZCandidates(Zjj)")
process.TreeMaker.VectorDouble.append("JJProducer:dEta(jjDeltaEta)")
process.TreeMaker.VectorBool.append("JJProducer:matched(Zjj_matched)")

process.TreeMaker.VarsDouble.append("metdouble:Pt(MET)")
process.TreeMaker.VarsDouble.append("htdouble(HT)")

process.TreeMaker.VectorTLorentzVector.append("genParticleDump(GenParticles)")
process.TreeMaker.VectorInt.append("genParticleDump:PdgId(GenParticles_PdgId)")
process.TreeMaker.VectorInt.append("genParticleDump:Status(GenParticles_Status)")
process.TreeMaker.VectorInt.append("genParticleDump:Parent(GenParticles_ParentIdx)")
process.TreeMaker.VectorInt.append("genParticleDump:ParentId(GenParticles_ParentId)")

process.p = cms.Path(#process.genJetParticles +  
                     #process.ak4GenJets + 
                     process.genParticleDump + 
                     process.metdouble +
                     process.htdouble +
                     process.leptonproducer +
                     process.ZProducer + 
                     process.JJProducer +
                     #process.ZZProducer + 
                     process.TreeMaker)


process.schedule = cms.Schedule(
    process.p
)


myfilelist = cms.untracked.vstring()
myfilelist.extend( [
        "file:"+infile for infile in infiles
])

process.source = cms.Source("PoolSource",
    fileNames = myfilelist,
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
)
