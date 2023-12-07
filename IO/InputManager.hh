#ifndef INPUTMANAGER_HH
#define INPUTMANAGER_HH

#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"

#include <vector>
#include <iostream>

#include "DataContainer/OpticalCluster.hh"
#include "DataContainer/WireCluster.hh"

class InputManager{
protected:
  TFile*      f_Input;
  std::string treename;
  std::string filename;
  
public:
  TTree* t_Input;

  InputManager(){};
  virtual ~InputManager(){};

  int GetEntries() const {
    return t_Input->GetEntries();
  };
  
  virtual void GetEntry (const int i=0) {
    t_Input->GetEntry(i);
  };

  // Dynamic backgrounds
  std::map<std::string, int> ID_map;
  bool dynamic = false;
  std::vector<std::string> AllGenTypeDynamic;
  
  std::string GetInputFile()                       const { return filename; };
  std::string GetInputTree()                       const { return treename; };
  void        SetInputFile(const std::string s="")       { filename=s; };
  void        SetInputTree(const std::string s="")       { treename=s; };
    
  virtual void LoadTree() {};
protected:
  void Initialise() {
    if (filename == "") {
      std::cerr << "Need to provide an input file" << std::endl;
      exit(1);
    }
    
    f_Input = new TFile(filename.c_str(), "READ");
    if (!f_Input->IsOpen()) {
      std::cerr << "The file " << filename.c_str() << " does not exist." << std::endl;
      exit(1);
    }

    if (f_Input->Get(treename.c_str())) {
      t_Input = (TTree*)f_Input->Get(treename.c_str());
    } else {
      std::cerr << "The requested tree (" << treename.c_str() << ") does not exist in file " << filename << std::endl;
      exit(1);
    }

    std::cout << "DYN SN Ana :: load IDs" << std::endl;

    // DYNAMIC BACKGROUNDS
    std::string delim = "/";
    std::string tree_name_token = treename.substr(0, treename.find(delim));
    std::string ID_tree = tree_name_token + "/fIDs";

    if (f_Input->Get( ID_tree.c_str() )) {
      dynamic = true;
      TTree *t_IDs = (TTree*)f_Input->Get( ID_tree.c_str() );
      TObjArray *branchList; 
      branchList  = t_IDs->GetListOfBranches();
      int nBranch = t_IDs->GetNbranches();
      TString IDtreenames[nBranch];

      std::cout << "ID trees: " << nBranch << std::endl;
      for(int i=0;i<nBranch;i++){
        IDtreenames[i] = branchList->At(i)->GetName();

        int temp_id;
        t_IDs->SetBranchAddress(IDtreenames[i], &temp_id);
        t_IDs->GetEntry(0);
        std::string delimiter = "_";
        std::string temp_string = IDtreenames[i].Data();
        std::string token = temp_string.substr(0, temp_string.find(delimiter));
        std::pair<std::string, int> temp_pair {token, temp_id};
        ID_map.insert( temp_pair );
        AllGenTypeDynamic.push_back(token);
      }
      std::pair<std::string, int> temp_pair_allbckg {"AllBackground", 99};
      ID_map.insert( temp_pair_allbckg );
      AllGenTypeDynamic.push_back("AllBackground");
      std::pair<std::string, int> temp_pair_all {"All", 100};
      ID_map.insert( temp_pair_all );
      AllGenTypeDynamic.push_back("All");

      for (auto const& x : ID_map){
        std::cout << x.first << " : " << x.second << std::endl;
      } 

    } else {
      std::cerr << "The requested tree 'fIDs' does not exist in file " << filename << std::endl;
      //exit(1);
    }

  }
};


class SNAnaInputManager: public InputManager{
private:
  std::map<int,int> fAPA_Channel;

public:
  int Run;
  int SubRun;
  int Event;
  
  int NTotHit    ;
  int NColHit    ;
  int NIndHit    ;
  int NHitNoBT   ;
  int NFlash     ;
  int NFlashNoBT ;
  int TotGen_Marl;
  int TotGen_APA ;
  int TotGen_CPA ;
  int TotGen_Ar39;
  int TotGen_Neut;
  int TotGen_Kryp;
  int TotGen_Plon;
  int TotGen_Rdon;
  int TotGen_Ar42;   


  int   * vec_Hit_View         ;
  int   * vec_Hit_Size         ;
  int   * vec_Hit_TPC          ;
  int   * vec_Hit_Chan         ;
  int   * vec_Hit_X_start      ;
  int   * vec_Hit_Y_start      ;
  int   * vec_Hit_Z_start      ;
  int   * vec_Hit_X_end        ;
  int   * vec_Hit_Y_end        ;
  int   * vec_Hit_Z_end        ;
  float * vec_Hit_Time         ;
  float * vec_Hit_RMS          ;
  float * vec_Hit_SADC         ;
  float * vec_Hit_Int          ;
  float * vec_Hit_Peak         ;
  int   * vec_Hit_True_GenType ;
  int   * vec_Hit_True_MainTrID;
  float * vec_Hit_True_EvEnergy;
  float * vec_Hit_True_X       ;                  
  float * vec_Hit_True_Y       ;                  
  float * vec_Hit_True_Z       ;                  
  float * vec_Hit_True_Energy  ;             
  float * vec_Hit_True_nElec   ;
  int   * vec_Hit_True_nIDEs   ; 
  
  std::vector<int>                 * Hit_View                 ;
  std::vector<int>                 * Hit_Size                 ;
  std::vector<int>                 * Hit_TPC                  ;
  std::vector<int>                 * Hit_Chan                 ;
  std::vector<int>                 * Hit_X_start              ;
  std::vector<int>                 * Hit_Y_start              ;
  std::vector<int>                 * Hit_Z_start              ;
  std::vector<int>                 * Hit_X_end                ;
  std::vector<int>                 * Hit_Y_end                ;
  std::vector<int>                 * Hit_Z_end                ;
  std::vector<float>               * Hit_Time                 ;
  std::vector<float>               * Hit_RMS                  ;
  std::vector<float>               * Hit_SADC                 ;
  std::vector<float>               * Hit_Int                  ;
  std::vector<float>               * Hit_Peak                 ;
  std::vector<int>                 * Hit_True_GenType         ;
  std::vector<int>                 * Hit_True_MainTrID        ;
  std::vector<float>               * Hit_True_EvEnergy        ;
  std::vector<float>               * Hit_True_X               ;                  
  std::vector<float>               * Hit_True_Y               ;                  
  std::vector<float>               * Hit_True_Z               ;                  
  std::vector<float>               * Hit_True_Energy          ;             
  std::vector<float>               * Hit_True_nElec           ;
  std::vector<int>                 * Hit_True_nIDEs           ; 
  std::vector<int>                 * Hit_True_MarleyIndex     ;
  
  std::vector<int>                 * Hit_AdjM5SADC            ;
  std::vector<int>                 * Hit_AdjM2SADC            ;
  std::vector<int>                 * Hit_AdjM1SADC            ;
  std::vector<int>                 * Hit_AdjP1SADC            ;
  std::vector<int>                 * Hit_AdjP2SADC            ;
  std::vector<int>                 * Hit_AdjP5SADC            ;
  std::vector<int>                 * Hit_AdjM5Chan            ;
  std::vector<int>                 * Hit_AdjM2Chan            ;
  std::vector<int>                 * Hit_AdjM1Chan            ;
  std::vector<int>                 * Hit_AdjP1Chan            ;
  std::vector<int>                 * Hit_AdjP2Chan            ;
  std::vector<int>                 * Hit_AdjP5Chan            ;

  std::vector<int>                 * PDS_Flash_FlashID        ;
  std::vector<float>               * PDS_Flash_YCenter        ;
  std::vector<float>               * PDS_Flash_ZCenter        ;
  std::vector<float>               * PDS_Flash_YWidth         ;
  std::vector<float>               * PDS_Flash_ZWidth         ;
  std::vector<float>               * PDS_Flash_Time           ;
  std::vector<float>               * PDS_Flash_TimeWidth      ;
  std::vector<float>               * PDS_Flash_TotalPE        ;
  std::vector<float>               * PDS_Flash_True_Distance  ;
  std::vector<float>               * PDS_Flash_True_GenType   ;

  std::vector<int> 	           * PDS_OpHit_OpChannel      ;
  std::vector<double>              * PDS_OpHit_X              ;
  std::vector<double> 	           * PDS_OpHit_Y              ;
  std::vector<double> 	           * PDS_OpHit_Z              ;
  std::vector<double> 	           * PDS_OpHit_PeakTimeAbs    ;
  std::vector<double> 	           * PDS_OpHit_PeakTime       ;
  std::vector<unsigned short>      * PDS_OpHit_Frame          ;
  std::vector<double>              * PDS_OpHit_Width          ;
  std::vector<double> 	           * PDS_OpHit_Area           ;
  std::vector<double> 	           * PDS_OpHit_Amplitude      ;
  std::vector<double> 	           * PDS_OpHit_PE             ;
  std::vector<double> 	           * PDS_OpHit_FastToTotal    ;
  std::vector<int>                 * PDS_OpHit_True_GenType   ;
  std::vector<double>              * PDS_OpHit_True_Energy    ;
  std::vector<int>                 * PDS_OpHit_True_TrackID   ;
  std::vector<int>                 * PDS_OpHit_True_GenTypeAll;
  std::vector<double>              * PDS_OpHit_True_EnergyAll ;
  std::vector<int>                 * PDS_OpHit_True_TrackIDAll;
  std::vector<int>                 * PDS_OpHit_True_IndexAll  ;

  std::vector<int>                 * True_VertexChan          ;
  std::vector<int>                 * True_Nu_Type             ;
  std::vector<int>                 * True_Nu_Lep_Type         ;
  std::vector<int>                 * True_Mode                ;
  std::vector<int>                 * True_CCNC                ;
  std::vector<int>                 * True_HitNucleon          ;
  std::vector<int>                 * True_Target              ;
  std::vector<int>                 * True_MarlSample          ;
  std::vector<float>               * True_MarlTime            ;
  std::vector<float>               * True_MarlWeight          ;
  std::vector<float>               * True_ENu                 ;
  std::vector<float>               * True_ENu_Lep             ;
  std::vector<float>               * True_VertX               ;
  std::vector<float>               * True_VertY               ;
  std::vector<float>               * True_VertZ               ;
  std::vector<float>               * True_VertexT             ;
  std::vector<float>               * True_Px                  ;
  std::vector<float>               * True_Py                  ;
  std::vector<float>               * True_Pz                  ;
  std::vector<float>               * True_Dirx                ;
  std::vector<float>               * True_Diry                ;
  std::vector<float>               * True_Dirz                ;
  std::vector<float>               * True_Time                ;

  int                singles_True_VertexChan          ;
  int                singles_True_Nu_Type             ;
  int                singles_True_Nu_Lep_Type         ;
  int                singles_True_Mode                ;
  int                singles_True_CCNC                ;
  int                singles_True_HitNucleon          ;
  int                singles_True_Target              ;
  int                singles_True_MarlSample          ;
  double              singles_True_MarlTime            ;
  double              singles_True_MarlWeight          ;
  double              singles_True_ENu                 ;
  double              singles_True_ENu_Lep             ;
  double              singles_True_VertX               ;
  double              singles_True_VertY               ;
  double              singles_True_VertZ               ;
  double              singles_True_VertexT             ;
  double              singles_True_Px                  ;
  double              singles_True_Py                  ;
  double              singles_True_Pz                  ;
  double              singles_True_Dirx                ;
  double              singles_True_Diry                ;
  double              singles_True_Dirz                ;
  double              singles_True_Time                ;
                                                              
  std::vector<int>                 * True_Bck_Mode            ;
  std::vector<int>                 * True_Bck_EndProcess      ;
  std::vector<int>                 * True_Bck_Mother          ;
  std::vector<int>                 * True_Bck_ID              ;
  std::vector<double>              * True_Bck_VertX           ;
  std::vector<double>              * True_Bck_VertY           ;
  std::vector<double>              * True_Bck_VertZ           ;
  std::vector<double>              * True_Bck_Time            ;
  std::vector<double>              * True_Bck_Energy          ;
  std::vector<int>                 * True_Bck_PDG             ;

  std::vector<int>                 * True_Bck_ModeAll         ;
  std::vector<double>              * True_Bck_VertXAll        ;
  std::vector<double>              * True_Bck_VertYAll        ;
  std::vector<double>              * True_Bck_VertZAll        ;
  std::vector<double>              * True_Bck_TimeAll         ;
  // std::vector<double>              * True_Bck_EndVertXAll     ;
  // std::vector<double>              * True_Bck_EndVertYAll     ;
  // std::vector<double>              * True_Bck_EndVertZAll     ;
  // std::vector<double>              * True_Bck_EndTimeAll      ;
  std::vector<double>              * True_Bck_EnergyAll       ;
  std::vector<int>                 * True_Bck_PDGAll          ;
  std::vector<int>                 * True_Bck_IDAll           ;

  std::vector<double>              * True_Bck_EndX            ;
  std::vector<double>              * True_Bck_EndY            ;
  std::vector<double>              * True_Bck_EndZ            ;  
  std::vector<double>              * True_Bck_EndT            ;
   
  // Dynamic backgrounds
  std::vector<int> TotGen_dynamic;

  void GetWireHits(std::vector<WireHit*>& wire) {
    for(int j = 0; j < NColHit; j++) {
      int marley_index=0;
      if (Hit_True_MarleyIndex != NULL) {
        marley_index=(*Hit_True_MarleyIndex)[j];
      }
      if((*Hit_Chan)[j] > fAPA_Channel[fNAPA])
        continue;
      WireHit* wh = new WireHit((*Hit_View)[j],        (*Hit_True_GenType)[j],  (*Hit_Chan)[j],
                                (*Hit_Time)[j],        (*Hit_SADC)[j],          (*Hit_RMS)[j],
                                (*Hit_True_Energy)[j], (*Hit_True_EvEnergy)[j], (*Hit_True_MainTrID)[j],
                                0.5*((*Hit_X_start)[j]+(*Hit_X_end)[j]),
                                0.5*((*Hit_Y_start)[j]+(*Hit_Y_end)[j]),
                                0.5*((*Hit_Z_start)[j]+(*Hit_Z_end)[j]),
                                (*Hit_True_X)[j],      (*Hit_True_Y)[j],        (*Hit_True_Z)[j],
                                marley_index,          (*Hit_True_nElec)[j]);
      if (wh->GetChannel() < 200000 ||
          abs(wh->GetPosition(kX)) < 20000 ||
          abs(wh->GetPosition(kY)) < 20000 ||
          abs(wh->GetPosition(kZ)) < 20000 ||
          abs(wh->GetPosition(kT)) < 20000) {
        wire.push_back(wh);
      } else {
        std::cout << "Messed up wire hit" << std::endl;
        delete wh;
        wh = NULL;
      }
    }
  };
  
  void GetOpticalHits(std::vector<OpticalHit*>& opti) {
    for(size_t j = 0; j < PDS_OpHit_OpChannel->size(); j++) {
      int marley_index=0;
      OpticalHit* oh = new OpticalHit((*PDS_OpHit_True_GenType)[j],
                                      (*PDS_OpHit_X)[j],        (*PDS_OpHit_Y)[j],     (*PDS_OpHit_Z)[j],
                                      (*PDS_OpHit_PeakTime)[j], (*PDS_OpHit_Width)[j], (*PDS_OpHit_PE)[j],
                                      (*PDS_OpHit_OpChannel)[j]);
      oh->SetTrueEnergy((*True_ENu)[marley_index]);
      oh->SetMarleyIndex(marley_index);

      if (oh->GetChannel() < 200000 ||
          abs(oh->GetPosition(kX)) < 20000 ||
          abs(oh->GetPosition(kY)) < 20000 ||
          abs(oh->GetPosition(kZ)) < 20000 ||
          abs(oh->GetPosition(kT)) < 20000) {
        opti.push_back(oh);
      } else {
        std::cout << "Messed up optical hit" << std::endl;
        delete oh;
        oh = NULL;
      }
    }
  };
  
  ~SNAnaInputManager(){
    delete Hit_View                 ; Hit_View                  = NULL;
    delete Hit_Size                 ; Hit_Size                  = NULL;
    delete Hit_TPC                  ; Hit_TPC                   = NULL;
    delete Hit_Chan                 ; Hit_Chan                  = NULL;
    delete Hit_X_start              ; Hit_X_start               = NULL;
    delete Hit_Y_start              ; Hit_Y_start               = NULL;
    delete Hit_Z_start              ; Hit_Z_start               = NULL;
    delete Hit_X_end                ; Hit_X_end                 = NULL;
    delete Hit_Y_end                ; Hit_Y_end                 = NULL;
    delete Hit_Z_end                ; Hit_Z_end                 = NULL;
    delete Hit_Time                 ; Hit_Time                  = NULL;
    delete Hit_RMS                  ; Hit_RMS                   = NULL;
    delete Hit_SADC                 ; Hit_SADC                  = NULL;
    delete Hit_Int                  ; Hit_Int                   = NULL;
    delete Hit_Peak                 ; Hit_Peak                  = NULL;
    delete Hit_True_GenType         ; Hit_True_GenType          = NULL;
    delete Hit_True_MainTrID        ; Hit_True_MainTrID         = NULL;
    delete Hit_True_EvEnergy        ; Hit_True_EvEnergy         = NULL;
    delete Hit_True_X               ; Hit_True_X                = NULL;                 
    delete Hit_True_Y               ; Hit_True_Y                = NULL;                 
    delete Hit_True_Z               ; Hit_True_Z                = NULL;                 
    delete Hit_True_Energy          ; Hit_True_Energy           = NULL;            
    delete Hit_True_nElec           ; Hit_True_nElec            = NULL;
    delete Hit_True_nIDEs           ; Hit_True_nIDEs            = NULL;
    delete Hit_True_MarleyIndex     ; Hit_True_MarleyIndex      = NULL;
                                                               
    delete Hit_AdjM5SADC            ; Hit_AdjM5SADC             = NULL;
    delete Hit_AdjM2SADC            ; Hit_AdjM2SADC             = NULL;
    delete Hit_AdjM1SADC            ; Hit_AdjM1SADC             = NULL;
    delete Hit_AdjP1SADC            ; Hit_AdjP1SADC             = NULL;
    delete Hit_AdjP2SADC            ; Hit_AdjP2SADC             = NULL;
    delete Hit_AdjP5SADC            ; Hit_AdjP5SADC             = NULL;
    delete Hit_AdjM5Chan            ; Hit_AdjM5Chan             = NULL;
    delete Hit_AdjM2Chan            ; Hit_AdjM2Chan             = NULL;
    delete Hit_AdjM1Chan            ; Hit_AdjM1Chan             = NULL;
    delete Hit_AdjP1Chan            ; Hit_AdjP1Chan             = NULL;
    delete Hit_AdjP2Chan            ; Hit_AdjP2Chan             = NULL;
    delete Hit_AdjP5Chan            ; Hit_AdjP5Chan             = NULL;
                                                               
    delete PDS_Flash_FlashID        ; PDS_Flash_FlashID         = NULL;
    delete PDS_Flash_YCenter        ; PDS_Flash_YCenter         = NULL;
    delete PDS_Flash_ZCenter        ; PDS_Flash_ZCenter         = NULL;
    delete PDS_Flash_YWidth         ; PDS_Flash_YWidth          = NULL;
    delete PDS_Flash_ZWidth         ; PDS_Flash_ZWidth          = NULL;
    delete PDS_Flash_Time           ; PDS_Flash_Time            = NULL;
    delete PDS_Flash_TimeWidth      ; PDS_Flash_TimeWidth       = NULL;
    delete PDS_Flash_TotalPE        ; PDS_Flash_TotalPE         = NULL;
    delete PDS_Flash_True_Distance  ; PDS_Flash_True_Distance   = NULL;
    delete PDS_Flash_True_GenType   ; PDS_Flash_True_GenType    = NULL;

    delete PDS_OpHit_OpChannel      ; PDS_OpHit_OpChannel       = NULL;
    delete PDS_OpHit_X              ; PDS_OpHit_X               = NULL;
    delete PDS_OpHit_Y              ; PDS_OpHit_Y               = NULL;
    delete PDS_OpHit_Z              ; PDS_OpHit_Z               = NULL;
    delete PDS_OpHit_PeakTimeAbs    ; PDS_OpHit_PeakTimeAbs     = NULL;
    delete PDS_OpHit_PeakTime       ; PDS_OpHit_PeakTime        = NULL;
    delete PDS_OpHit_Frame          ; PDS_OpHit_Frame           = NULL;
    delete PDS_OpHit_Width          ; PDS_OpHit_Width           = NULL;
    delete PDS_OpHit_Area           ; PDS_OpHit_Area            = NULL;
    delete PDS_OpHit_Amplitude      ; PDS_OpHit_Amplitude       = NULL;
    delete PDS_OpHit_PE             ; PDS_OpHit_PE              = NULL;
    delete PDS_OpHit_FastToTotal    ; PDS_OpHit_FastToTotal     = NULL;
    delete PDS_OpHit_True_GenType   ; PDS_OpHit_True_GenType    = NULL;
    delete PDS_OpHit_True_Energy    ; PDS_OpHit_True_Energy     = NULL;
    delete PDS_OpHit_True_TrackID   ; PDS_OpHit_True_TrackID    = NULL;
    delete PDS_OpHit_True_GenTypeAll; PDS_OpHit_True_GenTypeAll = NULL;
    delete PDS_OpHit_True_EnergyAll ; PDS_OpHit_True_EnergyAll  = NULL;
    delete PDS_OpHit_True_TrackIDAll; PDS_OpHit_True_TrackIDAll = NULL;
    delete PDS_OpHit_True_IndexAll  ; PDS_OpHit_True_IndexAll   = NULL;

    delete True_VertexChan          ; True_VertexChan           = NULL;
    delete True_Nu_Type             ; True_Nu_Type              = NULL;
    delete True_Nu_Lep_Type         ; True_Nu_Lep_Type          = NULL;
    delete True_Mode                ; True_Mode                 = NULL;
    delete True_CCNC                ; True_CCNC                 = NULL;
    delete True_HitNucleon          ; True_HitNucleon           = NULL;
    delete True_Target              ; True_Target               = NULL;
    delete True_MarlSample          ; True_MarlSample           = NULL;
    delete True_MarlTime            ; True_MarlTime             = NULL;
    delete True_MarlWeight          ; True_MarlWeight           = NULL;
    delete True_ENu                 ; True_ENu                  = NULL;
    delete True_ENu_Lep             ; True_ENu_Lep              = NULL;
    delete True_VertX               ; True_VertX                = NULL;
    delete True_VertY               ; True_VertY                = NULL;
    delete True_VertZ               ; True_VertZ                = NULL;
    delete True_VertexT             ; True_VertexT              = NULL;
    delete True_Px                  ; True_Px                   = NULL;
    delete True_Py                  ; True_Py                   = NULL;
    delete True_Pz                  ; True_Pz                   = NULL;
    delete True_Dirx                ; True_Dirx                 = NULL;
    delete True_Diry                ; True_Diry                 = NULL;
    delete True_Dirz                ; True_Dirz                 = NULL;
    delete True_Time                ; True_Time                 = NULL;
                                                               
    delete True_Bck_Mode            ; True_Bck_Mode             = NULL;
    delete True_Bck_EndProcess      ; True_Bck_EndProcess       = NULL;
    delete True_Bck_Mother          ; True_Bck_Mother           = NULL;
    delete True_Bck_ID              ; True_Bck_ID               = NULL;
    delete True_Bck_VertX           ; True_Bck_VertX            = NULL;
    delete True_Bck_VertY           ; True_Bck_VertY            = NULL;
    delete True_Bck_VertZ           ; True_Bck_VertZ            = NULL;
    delete True_Bck_Time            ; True_Bck_Time             = NULL;
    delete True_Bck_Energy          ; True_Bck_Energy           = NULL;
    delete True_Bck_PDG             ; True_Bck_PDG              = NULL;
                                                               
    delete True_Bck_ModeAll         ; True_Bck_ModeAll          = NULL;
    delete True_Bck_VertXAll        ; True_Bck_VertXAll         = NULL;
    delete True_Bck_VertYAll        ; True_Bck_VertYAll         = NULL;
    delete True_Bck_VertZAll        ; True_Bck_VertZAll         = NULL;
    delete True_Bck_TimeAll         ; True_Bck_TimeAll          = NULL;
    // delete True_Bck_EndVertXAll     ; True_Bck_EndVertXAll      = NULL;
    // delete True_Bck_EndVertYAll     ; True_Bck_EndVertYAll      = NULL;
    // delete True_Bck_EndVertZAll     ; True_Bck_EndVertZAll      = NULL;
    // delete True_Bck_EndTimeAll      ; True_Bck_EndTimeAll       = NULL;
    delete True_Bck_EnergyAll       ; True_Bck_EnergyAll        = NULL;
    delete True_Bck_PDGAll          ; True_Bck_PDGAll           = NULL;
    delete True_Bck_IDAll           ; True_Bck_IDAll            = NULL;

    delete True_Bck_EndX            ; True_Bck_EndX             = NULL;   
    delete True_Bck_EndY            ; True_Bck_EndY             = NULL;
    delete True_Bck_EndZ            ; True_Bck_EndZ             = NULL;
    delete True_Bck_EndT            ; True_Bck_EndT             = NULL;
    
    if (f_Input) f_Input->Close();

  };
  
  SNAnaInputManager():
    Run        (0),
    SubRun     (0),
    Event      (0),
    NTotHit    (0),
    NColHit    (0),
    NIndHit    (0),
    NHitNoBT   (0),
    NFlash     (0),
    NFlashNoBT (0),
    TotGen_Marl(0),
    TotGen_APA (0),
    TotGen_CPA (0),
    TotGen_Ar39(0),
    TotGen_Neut(0),
    TotGen_Kryp(0),
    TotGen_Plon(0),
    TotGen_Rdon(0),
    TotGen_Ar42(0),   
    
    vec_Hit_View         (NULL),
    vec_Hit_Size         (NULL),
    vec_Hit_TPC          (NULL),
    vec_Hit_Chan         (NULL),
    vec_Hit_X_start      (NULL),
    vec_Hit_Y_start      (NULL),
    vec_Hit_Z_start      (NULL),
    vec_Hit_X_end        (NULL),
    vec_Hit_Y_end        (NULL),
    vec_Hit_Z_end        (NULL),
    vec_Hit_Time         (NULL),
    vec_Hit_RMS          (NULL),
    vec_Hit_SADC         (NULL),
    vec_Hit_Int          (NULL),
    vec_Hit_Peak         (NULL),
    vec_Hit_True_GenType (NULL),
    vec_Hit_True_MainTrID(NULL),
    vec_Hit_True_EvEnergy(NULL),
    vec_Hit_True_X       (NULL),
    vec_Hit_True_Y       (NULL),
    vec_Hit_True_Z       (NULL),
    vec_Hit_True_Energy  (NULL),
    vec_Hit_True_nElec   (NULL),
    vec_Hit_True_nIDEs   (NULL),

    Hit_View                 (NULL),
    Hit_Size                 (NULL),
    Hit_TPC                  (NULL),
    Hit_Chan                 (NULL),
    Hit_X_start              (NULL),
    Hit_Y_start              (NULL),
    Hit_Z_start              (NULL),
    Hit_X_end                (NULL),
    Hit_Y_end                (NULL),
    Hit_Z_end                (NULL),
    Hit_Time                 (NULL),
    Hit_RMS                  (NULL),
    Hit_SADC                 (NULL),
    Hit_Int                  (NULL),
    Hit_Peak                 (NULL),
    Hit_True_GenType         (NULL),
    Hit_True_MainTrID        (NULL),
    Hit_True_EvEnergy        (NULL),
    Hit_True_X               (NULL),                 
    Hit_True_Y               (NULL),                 
    Hit_True_Z               (NULL),                 
    Hit_True_Energy          (NULL),            
    Hit_True_nElec           (NULL),
    Hit_True_nIDEs           (NULL),
    Hit_True_MarleyIndex     (NULL),
    
    Hit_AdjM5SADC            (NULL),
    Hit_AdjM2SADC            (NULL),
    Hit_AdjM1SADC            (NULL),
    Hit_AdjP1SADC            (NULL),
    Hit_AdjP2SADC            (NULL),
    Hit_AdjP5SADC            (NULL),
    Hit_AdjM5Chan            (NULL),
    Hit_AdjM2Chan            (NULL),
    Hit_AdjM1Chan            (NULL),
    Hit_AdjP1Chan            (NULL),
    Hit_AdjP2Chan            (NULL),
    Hit_AdjP5Chan            (NULL),

    PDS_Flash_FlashID        (NULL),
    PDS_Flash_YCenter        (NULL),
    PDS_Flash_ZCenter        (NULL),
    PDS_Flash_YWidth         (NULL),
    PDS_Flash_ZWidth         (NULL),
    PDS_Flash_Time           (NULL),
    PDS_Flash_TimeWidth      (NULL),
    PDS_Flash_TotalPE        (NULL),
    PDS_Flash_True_Distance  (NULL),
    PDS_Flash_True_GenType   (NULL),

    PDS_OpHit_OpChannel      (NULL),
    PDS_OpHit_X              (NULL),
    PDS_OpHit_Y              (NULL),
    PDS_OpHit_Z              (NULL),
    PDS_OpHit_PeakTimeAbs    (NULL),
    PDS_OpHit_PeakTime       (NULL),
    PDS_OpHit_Frame          (NULL),
    PDS_OpHit_Width          (NULL),
    PDS_OpHit_Area           (NULL),
    PDS_OpHit_Amplitude      (NULL),
    PDS_OpHit_PE             (NULL),
    PDS_OpHit_FastToTotal    (NULL),
    PDS_OpHit_True_GenType   (NULL),
    PDS_OpHit_True_Energy    (NULL),
    PDS_OpHit_True_TrackID   (NULL),
    PDS_OpHit_True_GenTypeAll(NULL),
    PDS_OpHit_True_EnergyAll (NULL),
    PDS_OpHit_True_TrackIDAll(NULL),
    PDS_OpHit_True_IndexAll  (NULL),

    True_VertexChan          (NULL),
    True_Nu_Type             (NULL),
    True_Nu_Lep_Type         (NULL),
    True_Mode                (NULL),
    True_CCNC                (NULL),
    True_HitNucleon          (NULL),
    True_Target              (NULL),
    True_MarlSample          (NULL),
    True_MarlTime            (NULL),
    True_MarlWeight          (NULL),
    True_ENu                 (NULL),
    True_ENu_Lep             (NULL),
    True_VertX               (NULL),
    True_VertY               (NULL),
    True_VertZ               (NULL),
    True_VertexT             (NULL),
    True_Px                  (NULL),
    True_Py                  (NULL),
    True_Pz                  (NULL),
    True_Dirx                (NULL),
    True_Diry                (NULL),
    True_Dirz                (NULL),
    True_Time                (NULL),

    singles_True_VertexChan       (-1),
    singles_True_Nu_Type          (-1),
    singles_True_Nu_Lep_Type      (-1),
    singles_True_Mode             (-1),
    singles_True_CCNC             (-1),
    singles_True_HitNucleon       (-1),
    singles_True_Target           (-1),
    singles_True_MarlSample       (-1),
    singles_True_MarlTime         (-1),
    singles_True_MarlWeight       (-1),
    singles_True_ENu              (-1),
    singles_True_ENu_Lep          (-1),
    singles_True_VertX            (-1),
    singles_True_VertY            (-1),
    singles_True_VertZ            (-1),
    singles_True_VertexT          (-1),
    singles_True_Px               (-1),
    singles_True_Py               (-1),
    singles_True_Pz               (-1),
    singles_True_Dirx             (-1),
    singles_True_Diry             (-1),
    singles_True_Dirz             (-1),
    singles_True_Time             (-1),
    
    True_Bck_Mode            (NULL),
    True_Bck_EndProcess      (NULL),
    True_Bck_Mother          (NULL),
    True_Bck_ID              (NULL),
    True_Bck_VertX           (NULL),
    True_Bck_VertY           (NULL),
    True_Bck_VertZ           (NULL),
    True_Bck_Time            (NULL),
    True_Bck_Energy          (NULL),
    True_Bck_PDG             (NULL),

    True_Bck_ModeAll         (NULL),
    True_Bck_VertXAll        (NULL),
    True_Bck_VertYAll        (NULL),
    True_Bck_VertZAll        (NULL),
    True_Bck_TimeAll         (NULL),
    // True_Bck_EndVertXAll     (NULL),
    // True_Bck_EndVertYAll     (NULL),
    // True_Bck_EndVertZAll     (NULL),
    // True_Bck_EndTimeAll      (NULL),
    True_Bck_EnergyAll       (NULL),
    True_Bck_PDGAll          (NULL),
    True_Bck_IDAll           (NULL),
    True_Bck_EndX            (NULL),
    True_Bck_EndY            (NULL),
    True_Bck_EndZ            (NULL),
    True_Bck_EndT            (NULL){

    f_Input = NULL;
    t_Input = NULL;
    treename = "";
    filename = "";
  };
  
  void GetEntry (const int i=0) {
    t_Input->GetEntry(i);
    if (vec_Hit_True_nIDEs!=NULL) {
      Hit_View         ->clear();
      Hit_Size         ->clear();
      Hit_TPC          ->clear();
      Hit_Chan         ->clear();
      Hit_X_start      ->clear();
      Hit_Y_start      ->clear();
      Hit_Z_start      ->clear();
      Hit_X_end        ->clear();
      Hit_Y_end        ->clear();
      Hit_Z_end        ->clear();
      Hit_Time         ->clear();
      Hit_RMS          ->clear();
      Hit_SADC         ->clear();
      Hit_Int          ->clear();
      Hit_Peak         ->clear();
      Hit_True_GenType ->clear();
      Hit_True_MainTrID->clear();
      Hit_True_EvEnergy->clear();
      Hit_True_X       ->clear();
      Hit_True_Y       ->clear();
      Hit_True_Z       ->clear();
      Hit_True_Energy  ->clear();
      Hit_True_nElec   ->clear();
      Hit_True_nIDEs   ->clear();

      for (int i=0; i<NColHit; ++i) {
        Hit_View         ->push_back(vec_Hit_View         [i]);
        Hit_Size         ->push_back(vec_Hit_Size         [i]);
        Hit_TPC          ->push_back(vec_Hit_TPC          [i]);
        Hit_Chan         ->push_back(vec_Hit_Chan         [i]);
        Hit_X_start      ->push_back(vec_Hit_X_start      [i]);
        Hit_Y_start      ->push_back(vec_Hit_Y_start      [i]);
        Hit_Z_start      ->push_back(vec_Hit_Z_start      [i]);
        Hit_X_end        ->push_back(vec_Hit_X_end        [i]);
        Hit_Y_end        ->push_back(vec_Hit_Y_end        [i]);
        Hit_Z_end        ->push_back(vec_Hit_Z_end        [i]);
        Hit_Time         ->push_back(vec_Hit_Time         [i]);
        Hit_RMS          ->push_back(vec_Hit_RMS          [i]);
        Hit_SADC         ->push_back(vec_Hit_SADC         [i]);
        Hit_Int          ->push_back(vec_Hit_Int          [i]);
        Hit_Peak         ->push_back(vec_Hit_Peak         [i]);
        Hit_True_GenType ->push_back(vec_Hit_True_GenType [i]);
        Hit_True_MainTrID->push_back(vec_Hit_True_MainTrID[i]);
        Hit_True_EvEnergy->push_back(vec_Hit_True_EvEnergy[i]);
        Hit_True_X       ->push_back(vec_Hit_True_X       [i]);
        Hit_True_Y       ->push_back(vec_Hit_True_Y       [i]);
        Hit_True_Z       ->push_back(vec_Hit_True_Z       [i]);
        Hit_True_Energy  ->push_back(vec_Hit_True_Energy  [i]);
        Hit_True_nElec   ->push_back(vec_Hit_True_nElec   [i]);
        Hit_True_nIDEs   ->push_back(vec_Hit_True_nIDEs   [i]);
      }
    }
    
    if (singles_True_VertexChan != -1) {
      True_VertexChan  ->clear();
      True_Nu_Type     ->clear();
      True_Nu_Lep_Type ->clear();
      True_Mode        ->clear();
      True_CCNC        ->clear();
      True_HitNucleon  ->clear();
      True_Target      ->clear();
      True_MarlSample  ->clear();
      True_MarlTime    ->clear();
      True_MarlWeight  ->clear();
      True_ENu         ->clear();
      True_ENu_Lep     ->clear();
      True_VertX       ->clear();
      True_VertY       ->clear();
      True_VertZ       ->clear();
      True_VertexT     ->clear();
      True_Px          ->clear();
      True_Py          ->clear();
      True_Pz          ->clear();
      True_Dirx        ->clear();
      True_Diry        ->clear();
      True_Dirz        ->clear();
      True_Time        ->clear();
      
      True_VertexChan        ->push_back(singles_True_VertexChan      );  
      True_Nu_Type           ->push_back(singles_True_Nu_Type         );  
      True_Nu_Lep_Type       ->push_back(singles_True_Nu_Lep_Type     );  
      True_Mode              ->push_back(singles_True_Mode            );  
      True_CCNC              ->push_back(singles_True_CCNC            );  
      True_HitNucleon        ->push_back(singles_True_HitNucleon      );  
      True_Target            ->push_back(singles_True_Target          );  
      True_MarlSample        ->push_back(singles_True_MarlSample      );  
      True_MarlTime          ->push_back(singles_True_MarlTime        );  
      True_MarlWeight        ->push_back(singles_True_MarlWeight      );  
      True_ENu               ->push_back(singles_True_ENu             );  
      True_ENu_Lep           ->push_back(singles_True_ENu_Lep         );  
      True_VertX             ->push_back(singles_True_VertX           );  
      True_VertY             ->push_back(singles_True_VertY           );  
      True_VertZ             ->push_back(singles_True_VertZ           );  
      True_VertexT           ->push_back(singles_True_VertexT         );  
      True_Px                ->push_back(singles_True_Px              );  
      True_Py                ->push_back(singles_True_Py              );  
      True_Pz                ->push_back(singles_True_Pz              );  
      True_Dirx              ->push_back(singles_True_Dirx            );  
      True_Diry              ->push_back(singles_True_Diry            );  
      True_Dirz              ->push_back(singles_True_Dirz            );  
      True_Time              ->push_back(singles_True_Time            );  
    }

       
  };

  void LoadTree(){
    Initialise();   
    
    t_Input->SetBranchAddress("Run"       , &Run       );
    t_Input->SetBranchAddress("SubRun"    , &SubRun    );
    t_Input->SetBranchAddress("Event"     , &Event     );

    if(t_Input->GetListOfBranches()->FindObject("NTotHit")) {
      t_Input->SetBranchAddress("NTotHit"   , &NTotHit   );
      t_Input->SetBranchAddress("NColHit"   , &NColHit   );
      t_Input->SetBranchAddress("NIndHit"   , &NIndHit   );
      t_Input->SetBranchAddress("NHitNoBT"  , &NHitNoBT  );
      t_Input->SetBranchAddress("NFlash"    , &NFlash    );
      t_Input->SetBranchAddress("NFlashNoBT", &NFlashNoBT);
      t_Input->SetBranchAddress("Hit_View"                 , &Hit_View                 );
      t_Input->SetBranchAddress("Hit_Size"                 , &Hit_Size                 );
      t_Input->SetBranchAddress("Hit_TPC"                  , &Hit_TPC                  );
      t_Input->SetBranchAddress("Hit_Chan"                 , &Hit_Chan                 );
      t_Input->SetBranchAddress("Hit_X_start"              , &Hit_X_start              );
      t_Input->SetBranchAddress("Hit_Y_start"              , &Hit_Y_start              );
      t_Input->SetBranchAddress("Hit_Z_start"              , &Hit_Z_start              );
      t_Input->SetBranchAddress("Hit_X_end"                , &Hit_X_end                );
      t_Input->SetBranchAddress("Hit_Y_end"                , &Hit_Y_end                );
      t_Input->SetBranchAddress("Hit_Z_end"                , &Hit_Z_end                );
      t_Input->SetBranchAddress("Hit_Time"                 , &Hit_Time                 );
      t_Input->SetBranchAddress("Hit_RMS"                  , &Hit_RMS                  );
      t_Input->SetBranchAddress("Hit_SADC"                 , &Hit_SADC                 );
      t_Input->SetBranchAddress("Hit_Int"                  , &Hit_Int                  );
      t_Input->SetBranchAddress("Hit_Peak"                 , &Hit_Peak                 );
      t_Input->SetBranchAddress("Hit_True_GenType"         , &Hit_True_GenType         );
      t_Input->SetBranchAddress("Hit_True_MainTrID"        , &Hit_True_MainTrID        );
      t_Input->SetBranchAddress("Hit_True_EvEnergy"        , &Hit_True_EvEnergy        );
      t_Input->SetBranchAddress("Hit_True_X"               , &Hit_True_X               );
      t_Input->SetBranchAddress("Hit_True_Y"               , &Hit_True_Y               );
      t_Input->SetBranchAddress("Hit_True_Z"               , &Hit_True_Z               );
      t_Input->SetBranchAddress("Hit_True_Energy"          , &Hit_True_Energy          );
      t_Input->SetBranchAddress("Hit_True_nElec"           , &Hit_True_nElec           );
      t_Input->SetBranchAddress("Hit_True_nIDEs"           , &Hit_True_nIDEs           );
      t_Input->SetBranchAddress("True_VertexChan"          , &True_VertexChan          );
      t_Input->SetBranchAddress("True_Nu_Type"             , &True_Nu_Type             );
      t_Input->SetBranchAddress("True_Nu_Lep_Type"         , &True_Nu_Lep_Type         );
      t_Input->SetBranchAddress("True_Mode"                , &True_Mode                );
      t_Input->SetBranchAddress("True_CCNC"                , &True_CCNC                );
      t_Input->SetBranchAddress("True_HitNucleon"          , &True_HitNucleon          );
      t_Input->SetBranchAddress("True_Target"              , &True_Target              );
      t_Input->SetBranchAddress("True_MarlSample"          , &True_MarlSample          );
      t_Input->SetBranchAddress("True_MarlTime"            , &True_MarlTime            );
      t_Input->SetBranchAddress("True_MarlWeight"          , &True_MarlWeight          );
      t_Input->SetBranchAddress("True_ENu"                 , &True_ENu                 );
      t_Input->SetBranchAddress("True_ENu_Lep"             , &True_ENu_Lep             );
      t_Input->SetBranchAddress("True_VertX"               , &True_VertX               );
      t_Input->SetBranchAddress("True_VertY"               , &True_VertY               );
      t_Input->SetBranchAddress("True_VertZ"               , &True_VertZ               );
      t_Input->SetBranchAddress("True_VertexT"             , &True_VertexT             );
      t_Input->SetBranchAddress("True_Px"                  , &True_Px                  );
      t_Input->SetBranchAddress("True_Py"                  , &True_Py                  );
      t_Input->SetBranchAddress("True_Pz"                  , &True_Pz                  );
      t_Input->SetBranchAddress("True_Dirx"                , &True_Dirx                );
      t_Input->SetBranchAddress("True_Diry"                , &True_Diry                );
      t_Input->SetBranchAddress("True_Dirz"                , &True_Dirz                );
      t_Input->SetBranchAddress("True_Time"                , &True_Time                );
    
      t_Input->SetBranchAddress("Hit_AdjM5SADC"            , &Hit_AdjM5SADC            );
      t_Input->SetBranchAddress("Hit_AdjM2SADC"            , &Hit_AdjM2SADC            );
      t_Input->SetBranchAddress("Hit_AdjM1SADC"            , &Hit_AdjM1SADC            );
      t_Input->SetBranchAddress("Hit_AdjP1SADC"            , &Hit_AdjP1SADC            );
      t_Input->SetBranchAddress("Hit_AdjP2SADC"            , &Hit_AdjP2SADC            );
      t_Input->SetBranchAddress("Hit_AdjP5SADC"            , &Hit_AdjP5SADC            );
      t_Input->SetBranchAddress("Hit_AdjM5Chan"            , &Hit_AdjM5Chan            );
      t_Input->SetBranchAddress("Hit_AdjM2Chan"            , &Hit_AdjM2Chan            );
      t_Input->SetBranchAddress("Hit_AdjM1Chan"            , &Hit_AdjM1Chan            );
      t_Input->SetBranchAddress("Hit_AdjP1Chan"            , &Hit_AdjP1Chan            );
      t_Input->SetBranchAddress("Hit_AdjP2Chan"            , &Hit_AdjP2Chan            );
      t_Input->SetBranchAddress("Hit_AdjP5Chan"            , &Hit_AdjP5Chan            );
                                                                                     
      t_Input->SetBranchAddress("PDS_Flash_FlashID"        , &PDS_Flash_FlashID        );
      t_Input->SetBranchAddress("PDS_Flash_YCenter"        , &PDS_Flash_YCenter        );
      t_Input->SetBranchAddress("PDS_Flash_ZCenter"        , &PDS_Flash_ZCenter        );
      t_Input->SetBranchAddress("PDS_Flash_YWidth"         , &PDS_Flash_YWidth         );
      t_Input->SetBranchAddress("PDS_Flash_ZWidth"         , &PDS_Flash_ZWidth         );
      t_Input->SetBranchAddress("PDS_Flash_Time"           , &PDS_Flash_Time           );
      t_Input->SetBranchAddress("PDS_Flash_TimeWidth"      , &PDS_Flash_TimeWidth      );
      t_Input->SetBranchAddress("PDS_Flash_TotalPE"        , &PDS_Flash_TotalPE        );
      t_Input->SetBranchAddress("PDS_Flash_True_Distance"  , &PDS_Flash_True_Distance  );
      t_Input->SetBranchAddress("PDS_Flash_True_GenType"   , &PDS_Flash_True_GenType   );
                                                                                     
      t_Input->SetBranchAddress("PDS_OpHit_OpChannel"      , &PDS_OpHit_OpChannel      );
      t_Input->SetBranchAddress("PDS_OpHit_X"              , &PDS_OpHit_X              );
      t_Input->SetBranchAddress("PDS_OpHit_Y"              , &PDS_OpHit_Y              );
      t_Input->SetBranchAddress("PDS_OpHit_Z"              , &PDS_OpHit_Z              );
      t_Input->SetBranchAddress("PDS_OpHit_PeakTimeAbs"    , &PDS_OpHit_PeakTimeAbs    );
      t_Input->SetBranchAddress("PDS_OpHit_PeakTime"       , &PDS_OpHit_PeakTime       );
      t_Input->SetBranchAddress("PDS_OpHit_Frame"          , &PDS_OpHit_Frame          );
      t_Input->SetBranchAddress("PDS_OpHit_Width"          , &PDS_OpHit_Width          );
      t_Input->SetBranchAddress("PDS_OpHit_Area"           , &PDS_OpHit_Area           );
      t_Input->SetBranchAddress("PDS_OpHit_Amplitude"      , &PDS_OpHit_Amplitude      );
      t_Input->SetBranchAddress("PDS_OpHit_PE"             , &PDS_OpHit_PE             );
      t_Input->SetBranchAddress("PDS_OpHit_FastToTotal"    , &PDS_OpHit_FastToTotal    );
      t_Input->SetBranchAddress("PDS_OpHit_True_GenType"   , &PDS_OpHit_True_GenType   );
      t_Input->SetBranchAddress("PDS_OpHit_True_Energy"    , &PDS_OpHit_True_Energy    );
      t_Input->SetBranchAddress("PDS_OpHit_True_TrackID"   , &PDS_OpHit_True_TrackID   );
      t_Input->SetBranchAddress("PDS_OpHit_True_GenTypeAll", &PDS_OpHit_True_GenTypeAll);
      t_Input->SetBranchAddress("PDS_OpHit_True_EnergyAll" , &PDS_OpHit_True_EnergyAll );
      t_Input->SetBranchAddress("PDS_OpHit_True_TrackIDAll", &PDS_OpHit_True_TrackIDAll);
      t_Input->SetBranchAddress("PDS_OpHit_True_IndexAll"  , &PDS_OpHit_True_IndexAll  );
                                                                                     
      t_Input->SetBranchAddress("True_Bck_Mode"            , &True_Bck_Mode            );
      t_Input->SetBranchAddress("True_Bck_EndProcess"      , &True_Bck_EndProcess      );
      t_Input->SetBranchAddress("True_Bck_Mother"          , &True_Bck_Mother          );
      t_Input->SetBranchAddress("True_Bck_ID"              , &True_Bck_ID              );
      t_Input->SetBranchAddress("True_Bck_VertX"           , &True_Bck_VertX           );
      t_Input->SetBranchAddress("True_Bck_VertY"           , &True_Bck_VertY           );
      t_Input->SetBranchAddress("True_Bck_VertZ"           , &True_Bck_VertZ           );
      t_Input->SetBranchAddress("True_Bck_Time"            , &True_Bck_Time            );
      t_Input->SetBranchAddress("True_Bck_Energy"          , &True_Bck_Energy          );
      t_Input->SetBranchAddress("True_Bck_PDG"             , &True_Bck_PDG             );

      t_Input->SetBranchAddress("True_Bck_ModeAll"         , &True_Bck_ModeAll         );
      t_Input->SetBranchAddress("True_Bck_VertXAll"        , &True_Bck_VertXAll        );
      t_Input->SetBranchAddress("True_Bck_VertYAll"        , &True_Bck_VertYAll        );
      t_Input->SetBranchAddress("True_Bck_VertZAll"        , &True_Bck_VertZAll        );
      t_Input->SetBranchAddress("True_Bck_TimeAll"         , &True_Bck_TimeAll         );
      // t_Input->SetBranchAddress("True_Bck_EndVertXAll"     , &True_Bck_EndVertXAll     );
      // t_Input->SetBranchAddress("True_Bck_EndVertYAll"     , &True_Bck_EndVertYAll     );
      // t_Input->SetBranchAddress("True_Bck_EndVertZAll"     , &True_Bck_EndVertZAll     );
      // t_Input->SetBranchAddress("True_Bck_EndTimeAll"      , &True_Bck_EndTimeAll      );
      t_Input->SetBranchAddress("True_Bck_EnergyAll"       , &True_Bck_EnergyAll       );
      t_Input->SetBranchAddress("True_Bck_PDGAll"          , &True_Bck_PDGAll          );
      t_Input->SetBranchAddress("True_Bck_IDAll"           , &True_Bck_IDAll           );
      t_Input->SetBranchAddress("True_Bck_EndX"            , &True_Bck_EndX            );
      t_Input->SetBranchAddress("True_Bck_EndY"            , &True_Bck_EndY            );      
      t_Input->SetBranchAddress("True_Bck_EndZ"            , &True_Bck_EndZ            );
      t_Input->SetBranchAddress("True_Bck_EndT"            , &True_Bck_EndT            );
      
    } else {
      t_Input->SetBranchAddress("NTotHits"   , &NTotHit   );
      t_Input->SetBranchAddress("NColHits"   , &NColHit   );
      t_Input->SetBranchAddress("NIndHits"   , &NIndHit   );
      t_Input->SetBranchAddress("NHitNoBTs"  , &NHitNoBT  );
      // t_Input->SetBranchAddress("NFlash"    , &NFlash    );
      // t_Input->SetBranchAddress("NFlashNoBT", &NFlashNoBT);
      t_Input->SetBranchAddress("HitView"                 , &vec_Hit_View         );
      t_Input->SetBranchAddress("HitSize"                 , &vec_Hit_Size         );
      t_Input->SetBranchAddress("HitTPC"                  , &vec_Hit_TPC          );
      t_Input->SetBranchAddress("HitChan"                 , &vec_Hit_Chan         );
      // t_Input->SetBranchAddress("Hit_X_start"              , &vec_Hit_X_start      );
      // t_Input->SetBranchAddress("Hit_Y_start"              , &vec_Hit_Y_start      );
      // t_Input->SetBranchAddress("Hit_Z_start"              , &vec_Hit_Z_start      );
      // t_Input->SetBranchAddress("Hit_X_end"                , &vec_Hit_X_end        );
      // t_Input->SetBranchAddress("Hit_Y_end"                , &vec_Hit_Y_end        );
      // t_Input->SetBranchAddress("Hit_Z_end"                , &vec_Hit_Z_end        );
      t_Input->SetBranchAddress("HitTime"                 , &vec_Hit_Time         );
      t_Input->SetBranchAddress("HitRMS"                  , &vec_Hit_RMS          );
      t_Input->SetBranchAddress("HitSADC"                 , &vec_Hit_SADC         );
      t_Input->SetBranchAddress("HitInt"                  , &vec_Hit_Int          );
      t_Input->SetBranchAddress("HitPeak"                 , &vec_Hit_Peak         );
      t_Input->SetBranchAddress("GenType"         , &vec_Hit_True_GenType );
      // t_Input->SetBranchAddress("Hit_True_MainTrID"        , &vec_Hit_True_MainTrID);
      // t_Input->SetBranchAddress("Hit_True_EvEnergy"        , &vec_Hit_True_EvEnergy);
      // t_Input->SetBranchAddress("Hit_True_X"               , &vec_Hit_True_X       );
      // t_Input->SetBranchAddress("Hit_True_Y"               , &vec_Hit_True_Y       );
      // t_Input->SetBranchAddress("Hit_True_Z"               , &vec_Hit_True_Z       );
      // t_Input->SetBranchAddress("Hit_True_Energy"          , &vec_Hit_True_Energy  );
      // t_Input->SetBranchAddress("Hit_True_nElec"           , &vec_Hit_True_nElec   );
      // t_Input->SetBranchAddress("Hit_True_nIDEs"           , &vec_Hit_True_nIDEs   );

      t_Input->SetBranchAddress("VertexChan"          , &singles_True_VertexChan          );
      t_Input->SetBranchAddress("Nu_Type"             , &singles_True_Nu_Type             );
      t_Input->SetBranchAddress("Nu_Lep_Type"         , &singles_True_Nu_Lep_Type         );
      t_Input->SetBranchAddress("Mode"                , &singles_True_Mode                );
      t_Input->SetBranchAddress("CCNC"                , &singles_True_CCNC                );
      t_Input->SetBranchAddress("HitNucleon"          , &singles_True_HitNucleon          );
      t_Input->SetBranchAddress("Target"              , &singles_True_Target              );
      t_Input->SetBranchAddress("MarlSample"          , &singles_True_MarlSample          );
      t_Input->SetBranchAddress("MarlTime"            , &singles_True_MarlTime            );
      t_Input->SetBranchAddress("MarlWeight"          , &singles_True_MarlWeight          );
      t_Input->SetBranchAddress("ENu"                 , &singles_True_ENu                 );
      t_Input->SetBranchAddress("ENu_Lep"             , &singles_True_ENu_Lep             );
      t_Input->SetBranchAddress("VertX"               , &singles_True_VertX               );
      t_Input->SetBranchAddress("VertY"               , &singles_True_VertY               );
      t_Input->SetBranchAddress("VertZ"               , &singles_True_VertZ               );
      t_Input->SetBranchAddress("VertexT"             , &singles_True_VertexT             );
      t_Input->SetBranchAddress("Px"                  , &singles_True_Px                  );
      t_Input->SetBranchAddress("Py"                  , &singles_True_Py                  );
      t_Input->SetBranchAddress("Pz"                  , &singles_True_Pz                  );
      t_Input->SetBranchAddress("Dirx"                , &singles_True_Dirx                );
      t_Input->SetBranchAddress("Diry"                , &singles_True_Diry                );
      t_Input->SetBranchAddress("Dirz"                , &singles_True_Dirz                );
      t_Input->SetBranchAddress("Time"                , &singles_True_Time                );

      True_VertexChan           = new std::vector<int>();
      True_Nu_Type              = new std::vector<int>();
      True_Nu_Lep_Type          = new std::vector<int>();
      True_Mode                 = new std::vector<int>();
      True_CCNC                 = new std::vector<int>();
      True_HitNucleon           = new std::vector<int>();
      True_Target               = new std::vector<int>();
      True_MarlSample           = new std::vector<int>();
      True_MarlTime             = new std::vector<float>();
      True_MarlWeight           = new std::vector<float>();
      True_ENu                  = new std::vector<float>();
      True_ENu_Lep              = new std::vector<float>();
      True_VertX                = new std::vector<float>();
      True_VertY                = new std::vector<float>();
      True_VertZ                = new std::vector<float>();
      True_VertexT              = new std::vector<float>();
      True_Px                   = new std::vector<float>();
      True_Py                   = new std::vector<float>();
      True_Pz                   = new std::vector<float>();
      True_Dirx                 = new std::vector<float>();
      True_Diry                 = new std::vector<float>();
      True_Dirz                 = new std::vector<float>();
      True_Time                 = new std::vector<float>();

      
    }
    
    if(t_Input->GetListOfBranches()->FindObject("Hit_True_MarleyIndex")) {
      t_Input->SetBranchAddress("Hit_True_MarleyIndex", &Hit_True_MarleyIndex);
    } else {
      std::cout << "Hit_True_MarleyIndex branch doesn't exist, assigning NULL for this vector!" << std::endl;      
      Hit_True_MarleyIndex=NULL;
    }

    // if(MyTree->GetBranch("SomeBranchName")) {
    //   MyTree->SetBranchAddress("SomeBranchName", &SomeVabiable);
    // }
      
    t_Input->SetBranchAddress("TotGen_Marl", &TotGen_Marl);
    t_Input->SetBranchAddress("TotGen_APA" , &TotGen_APA );
    t_Input->SetBranchAddress("TotGen_CPA" , &TotGen_CPA );
    t_Input->SetBranchAddress("TotGen_Ar39", &TotGen_Ar39);
    t_Input->SetBranchAddress("TotGen_Neut", &TotGen_Neut);
    t_Input->SetBranchAddress("TotGen_Kryp", &TotGen_Kryp);
    t_Input->SetBranchAddress("TotGen_Plon", &TotGen_Plon);
    t_Input->SetBranchAddress("TotGen_Rdon", &TotGen_Rdon);
    t_Input->SetBranchAddress("TotGen_Ar42", &TotGen_Ar42);
 
    if (dynamic) {
      TotGen_dynamic.resize(ID_map.size());
      for (auto const& x : ID_map){
        std::stringstream ss;
        ss << "TotGen_" + x.first;
        t_Input->SetBranchAddress(ss.str().c_str(), &TotGen_dynamic[x.second]);
      }
    } 

    // TESTING
    //std::cout << ConvertIDIntToString(0) << std::endl;
    //std::cout << ConvertIDStringToInt("All") << std::endl;
    //t_Input->GetEntry(0);
    //for (auto const& x : ID_map){
    //  if (x.second < 99){
    //    std::cout << TotGen_dynamic[x.second] << std::endl;
    //  }
    //}
    //t_Input->GetEntry(1);
    //for (auto const& x : ID_map){
    //  if (x.second < 99){
    //    std::cout << TotGen_dynamic[x.second] << std::endl;
    //  }
    //}

  };
  
  inline std::string ConvertIDIntToString(int i) {
  std::string key;
    for (auto &entry : ID_map) {
      if (entry.second == i) {
         key = entry.first;
         break; // to stop searching
      }
    }
    return key;
  };

  inline int ConvertIDStringToInt(std::string id_name) {
    return ID_map[id_name];
  };

public:
  void SetAPAChannelMap(std::map<int,int>& m) { fAPA_Channel = m; };
  std::map<int,int>  GetAPAChannelMap() const { return fAPA_Channel; };
  void SetNAPA(int s=12) { fNAPA =s; };
  int  GetNAPA() const { return fNAPA; };
  
private:
  int fNAPA;
};


#endif

