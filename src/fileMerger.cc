/*
 *  fileMerger.cc
 *  g4ana
 *
 *  Created by Auke Colijn on 1/8/12.
 *  Copyright 2012 University of Amsterdam. All rights reserved.
 *
 */

#include "fileMerger.hh"

//_________________________________________________________________________________________________________

fileMerger::fileMerger()
{
  
}

//_________________________________________________________________________________________________________

fileMerger::fileMerger(std::string file0, std::vector<std::string> files1)
{
  masterfile = file0;
  auxfiles   = files1;
  
}

//_________________________________________________________________________________________________________


void fileMerger::CopyDir(TDirectory *source, Int_t ilevel) {
  Int_t ntree = 0;
  
  //copy all objects and subdirs of directory source as a subdir of the current directory   
  //source->ls();
  TDirectory *savdir = gDirectory;
  TDirectory *adir; 
  if(ilevel == MAKE_NEW_FOLDER ) 
  {
    adir   = savdir->mkdir(source->GetName());
    adir->cd();
  } else {
    adir   = savdir;
    adir->cd();
  }
  //loop on all entries of this directory
  TKey *key; 
  TIter nextkey(source->GetListOfKeys());
  
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname);
    if (!cl) continue;
    if (cl->InheritsFrom(TDirectory::Class())) {
      std::string dirname = key->GetName();
      if(dirname != "events"){
        source->cd(key->GetName());
        TDirectory *subdir = gDirectory;
        adir->cd();
        CopyDir(subdir, MAKE_NEW_FOLDER);
        adir->cd();
      }
    } else if (cl->InheritsFrom(TTree::Class())) {
      std::string treename = key->GetName();
      if(treename == "events" && ntree==0){
        ntree++;
        TTree *T = (TTree*)source->Get(key->GetName());
        adir->cd();
        TTree *newT = T->CloneTree(-1,"fast");
        newT->Write();
      }
    } else {
      source->cd();
      TObject *obj = key->ReadObj();
      adir->cd();
      obj->Write();
      delete obj;
    }
  }
  adir->SaveSelf(kTRUE);
  savdir->cd();
}

//_________________________________________________________________________________________________________

void fileMerger::CopyFile(const char *fname) {
  //Copy all objects and subdirs of file fname as a subdir of the current directory
  TDirectory *target = gDirectory;
  TFile *f = TFile::Open(fname);
  if (!f || f->IsZombie()) {
    printf("Cannot copy file: %s\n",fname);
    target->cd();
    return;
  }
  target->cd();
  CopyDir(f,NO_NEW_FOLDER);
  delete f;
  target->cd();
}  

//_________________________________________________________________________________________________________

void fileMerger::Merge(){
  //main function copying 4 files as subdirectories of a new file
  
  std::cout << "fileMerger::Merge() Merging root files .... "<<std::endl;
  TFile *f = new TFile(masterfile.c_str(),"update");
  std::cout << "fileMerger::Merge()          master file:  "<<masterfile.c_str()<<std::endl;

  for(int i=0; i<(int)auxfiles.size(); i++){
    std::string auxfile = auxfiles[i];
    std::cout << "fileMerger::Merge()          adding file:  "<<auxfile.c_str()<<std::endl;
    CopyFile(auxfile.c_str());
  }

  delete f;
  std::cout << "fileMerger::Merge() Done "<<std::endl;
}

//_________________________________________________________________________________________________________

void fileMerger::CleanUp(){
  //main function copying 4 files as subdirectories of a new file

  //char shellCommand[100]; 
  //std::string rm_cmd = "rm -f ";
  for(int i=0; i<(int)auxfiles.size(); i++){
    std::string auxfile = auxfiles[i];
    std::cout << "fileMerger::CleanUp()          removefile:  "<<auxfile.c_str()<<std::endl;
    // sprintf(shellCommand,"rm -f %s",auxfile.c_str());
    //system(shellCommand);
    unlink(auxfile.c_str());
    // system((rm_cmd + auxfile).c_str());
  }
  
  std::cout << "fileMerger::CleanUp() Done "<<std::endl;
}

//_________________________________________________________________________________________________________
