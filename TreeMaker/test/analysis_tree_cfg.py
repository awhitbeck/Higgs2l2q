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

process.genParticleDump = cms.EDProducer('GenParticlesProducer',
                                         genCollection = cms.InputTag('genParticles'),
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

process.TreeMaker.VectorRecoCand.append("ak4GenJetsNoNu")
process.TreeMaker.VectorRecoCand.append("ak8GenJetsNoNu")

process.TreeMaker.VectorTLorentzVector.append("genParticleDump(GenParticles)")
process.TreeMaker.VectorInt.append("genParticleDump:PdgId(GenParticles_PdgId)")
process.TreeMaker.VectorInt.append("genParticleDump:Status(GenParticles_Status)")
process.TreeMaker.VectorInt.append("genParticleDump:Parent(GenParticles_ParentIdx)")
process.TreeMaker.VectorInt.append("genParticleDump:ParentId(GenParticles_ParentId)")

process.p = cms.Path(process.genJetParticles + process.ak5GenJets 
                     + process.genParticleDump + process.TreeMaker)


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
