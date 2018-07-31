#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "TFile.h"
#include "TTree.h"

#include <vector>
#include <iostream>
const int  nMaxColHit=2000;
class InputManager{
private:
  TFile* f_Input;
  std::string treename;
  std::string filename;
public:
  TTree* t_Input;

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
  std::vector<int>                 * True_Bck_Mode            ;
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
  std::vector<double>              * True_Bck_EndVertXAll     ;
  std::vector<double>              * True_Bck_EndVertYAll     ;
  std::vector<double>              * True_Bck_EndVertZAll     ;
  std::vector<double>              * True_Bck_EndTimeAll      ;
  std::vector<double>              * True_Bck_EnergyAll       ;
  std::vector<int>                 * True_Bck_PDGAll          ;
  std::vector<int>                 * True_Bck_IDAll           ;

  int   HitView           [nMaxColHit];
  int   HitSize           [nMaxColHit];
  int   HitTPC            [nMaxColHit];
  int   HitChan           [nMaxColHit];
  float HitTime           [nMaxColHit];
  float HitRMS            [nMaxColHit];
  float HitSADC           [nMaxColHit];
  float HitInt            [nMaxColHit];
  float HitPeak           [nMaxColHit];
  int   GenType           [nMaxColHit];
  int   NCorrespondingIDEs[nMaxColHit];
  float Hit_X             [nMaxColHit];
  float Hit_Y             [nMaxColHit];
  float Hit_Z             [nMaxColHit];
  float Hit_Energy        [nMaxColHit];
  float Hit_NumElectrons  [nMaxColHit];

  int    VertexChan    ;
  int    Nu_Type       ;
  int    Nu_Lep_Type   ;
  int    Mode          ;
  int    CCNC          ;
  int    HitNucleon    ;
  int    Target        ;
  int    MarlSample    ;
  double MarlTime      ;
  double MarlWeight    ;
  double ENu           ;
  double ENu_Lep       ;
  double VertX         ;
  double VertY         ;
  double VertZ         ;
  double VertexT       ;
  double Px            ;
  double Py            ;
  double Pz            ;

  void InstanciateVariables(){
    Hit_View                  = new std::vector<int>();
    Hit_Size                  = new std::vector<int>();
    Hit_TPC                   = new std::vector<int>();
    Hit_Chan                  = new std::vector<int>();
    Hit_X_start               = new std::vector<int>();
    Hit_Y_start               = new std::vector<int>();
    Hit_Z_start               = new std::vector<int>();
    Hit_X_end                 = new std::vector<int>();
    Hit_Y_end                 = new std::vector<int>();
    Hit_Z_end                 = new std::vector<int>();
    Hit_Time                  = new std::vector<float>();
    Hit_RMS                   = new std::vector<float>();
    Hit_SADC                  = new std::vector<float>();
    Hit_Int                   = new std::vector<float>();
    Hit_Peak                  = new std::vector<float>();
    Hit_True_GenType          = new std::vector<int>();
    Hit_True_MainTrID         = new std::vector<int>();
    Hit_True_EvEnergy         = new std::vector<float>();
    Hit_True_X                = new std::vector<float>();
    Hit_True_Y                = new std::vector<float>();
    Hit_True_Z                = new std::vector<float>();
    Hit_True_Energy           = new std::vector<float>();
    Hit_True_nElec            = new std::vector<float>();
    Hit_True_nIDEs            = new std::vector<int>();
    //Hit_True_MarleyIndex      = new std::vector<int>();
    Hit_AdjM5SADC             = new std::vector<int>();
    Hit_AdjM2SADC             = new std::vector<int>();
    Hit_AdjM1SADC             = new std::vector<int>();
    Hit_AdjP1SADC             = new std::vector<int>();
    Hit_AdjP2SADC             = new std::vector<int>();
    Hit_AdjP5SADC             = new std::vector<int>();
    Hit_AdjM5Chan             = new std::vector<int>();
    Hit_AdjM2Chan             = new std::vector<int>();
    Hit_AdjM1Chan             = new std::vector<int>();
    Hit_AdjP1Chan             = new std::vector<int>();
    Hit_AdjP2Chan             = new std::vector<int>();
    Hit_AdjP5Chan             = new std::vector<int>();
    PDS_Flash_FlashID         = new std::vector<int>();
    PDS_Flash_YCenter         = new std::vector<float>();
    PDS_Flash_ZCenter         = new std::vector<float>();
    PDS_Flash_YWidth          = new std::vector<float>();
    PDS_Flash_ZWidth          = new std::vector<float>();
    PDS_Flash_Time            = new std::vector<float>();
    PDS_Flash_TimeWidth       = new std::vector<float>();
    PDS_Flash_TotalPE         = new std::vector<float>();
    PDS_Flash_True_Distance   = new std::vector<float>();
    PDS_Flash_True_GenType    = new std::vector<float>();
    PDS_OpHit_OpChannel       = new std::vector<int>();
    PDS_OpHit_X               = new std::vector<double>();
    PDS_OpHit_Y               = new std::vector<double>();
    PDS_OpHit_Z               = new std::vector<double>();
    PDS_OpHit_PeakTimeAbs     = new std::vector<double>();
    PDS_OpHit_PeakTime        = new std::vector<double>();
    PDS_OpHit_Frame           = new std::vector<unsigned short>();
    PDS_OpHit_Width           = new std::vector<double>();
    PDS_OpHit_Area            = new std::vector<double>();
    PDS_OpHit_Amplitude       = new std::vector<double>();
    PDS_OpHit_PE              = new std::vector<double>();	
    PDS_OpHit_FastToTotal     = new std::vector<double>(); 
    PDS_OpHit_True_GenType    = new std::vector<int>();
    PDS_OpHit_True_Energy     = new std::vector<double>();
    PDS_OpHit_True_TrackID    = new std::vector<int>();
    PDS_OpHit_True_GenTypeAll = new std::vector<int>();
    PDS_OpHit_True_EnergyAll  = new std::vector<double>();
    PDS_OpHit_True_TrackIDAll = new std::vector<int>();
    PDS_OpHit_True_IndexAll   = new std::vector<int>();
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
    True_Bck_Mode             = new std::vector<int>();
    True_Bck_VertX            = new std::vector<double>();
    True_Bck_VertY            = new std::vector<double>();
    True_Bck_VertZ            = new std::vector<double>();
    True_Bck_Time             = new std::vector<double>();
    True_Bck_Energy           = new std::vector<double>();
    True_Bck_PDG              = new std::vector<int>();
    True_Bck_ModeAll          = new std::vector<int>();
    True_Bck_VertXAll         = new std::vector<double>();
    True_Bck_VertYAll         = new std::vector<double>();
    True_Bck_VertZAll         = new std::vector<double>();
    True_Bck_TimeAll          = new std::vector<double>();
    True_Bck_EndVertXAll      = new std::vector<double>();
    True_Bck_EndVertYAll      = new std::vector<double>();
    True_Bck_EndVertZAll      = new std::vector<double>();
    True_Bck_EndTimeAll       = new std::vector<double>();
    True_Bck_EnergyAll        = new std::vector<double>();
    True_Bck_PDGAll           = new std::vector<int>();
    True_Bck_IDAll            = new std::vector<int>();
  };
  
  void ResetVariables(){
    if (Hit_View                 ) { delete Hit_View                 ; Hit_View                  = NULL; }
    if (Hit_Size                 ) { delete Hit_Size                 ; Hit_Size                  = NULL; }
    if (Hit_TPC                  ) { delete Hit_TPC                  ; Hit_TPC                   = NULL; }
    if (Hit_Chan                 ) { delete Hit_Chan                 ; Hit_Chan                  = NULL; }
    if (Hit_X_start              ) { delete Hit_X_start              ; Hit_X_start               = NULL; }
    if (Hit_Y_start              ) { delete Hit_Y_start              ; Hit_Y_start               = NULL; }
    if (Hit_Z_start              ) { delete Hit_Z_start              ; Hit_Z_start               = NULL; }
    if (Hit_X_end                ) { delete Hit_X_end                ; Hit_X_end                 = NULL; }
    if (Hit_Y_end                ) { delete Hit_Y_end                ; Hit_Y_end                 = NULL; }
    if (Hit_Z_end                ) { delete Hit_Z_end                ; Hit_Z_end                 = NULL; }
    if (Hit_Time                 ) { delete Hit_Time                 ; Hit_Time                  = NULL; }
    if (Hit_RMS                  ) { delete Hit_RMS                  ; Hit_RMS                   = NULL; }
    if (Hit_SADC                 ) { delete Hit_SADC                 ; Hit_SADC                  = NULL; }
    if (Hit_Int                  ) { delete Hit_Int                  ; Hit_Int                   = NULL; }
    if (Hit_Peak                 ) { delete Hit_Peak                 ; Hit_Peak                  = NULL; }
    if (Hit_True_GenType         ) { delete Hit_True_GenType         ; Hit_True_GenType          = NULL; }
    if (Hit_True_MainTrID        ) { delete Hit_True_MainTrID        ; Hit_True_MainTrID         = NULL; }
    if (Hit_True_EvEnergy        ) { delete Hit_True_EvEnergy        ; Hit_True_EvEnergy         = NULL; }
    if (Hit_True_X               ) { delete Hit_True_X               ; Hit_True_X                = NULL; }                 
    if (Hit_True_Y               ) { delete Hit_True_Y               ; Hit_True_Y                = NULL; }                 
    if (Hit_True_Z               ) { delete Hit_True_Z               ; Hit_True_Z                = NULL; }                 
    if (Hit_True_Energy          ) { delete Hit_True_Energy          ; Hit_True_Energy           = NULL; }            
    if (Hit_True_nElec           ) { delete Hit_True_nElec           ; Hit_True_nElec            = NULL; }
    if (Hit_True_nIDEs           ) { delete Hit_True_nIDEs           ; Hit_True_nIDEs            = NULL; }
    if (Hit_True_MarleyIndex     ) { delete Hit_True_MarleyIndex     ; Hit_True_MarleyIndex      = NULL; }

    if (Hit_AdjM5SADC            ) { delete Hit_AdjM5SADC            ; Hit_AdjM5SADC             = NULL; }
    if (Hit_AdjM2SADC            ) { delete Hit_AdjM2SADC            ; Hit_AdjM2SADC             = NULL; }
    if (Hit_AdjM1SADC            ) { delete Hit_AdjM1SADC            ; Hit_AdjM1SADC             = NULL; }
    if (Hit_AdjP1SADC            ) { delete Hit_AdjP1SADC            ; Hit_AdjP1SADC             = NULL; }
    if (Hit_AdjP2SADC            ) { delete Hit_AdjP2SADC            ; Hit_AdjP2SADC             = NULL; }
    if (Hit_AdjP5SADC            ) { delete Hit_AdjP5SADC            ; Hit_AdjP5SADC             = NULL; }
    if (Hit_AdjM5Chan            ) { delete Hit_AdjM5Chan            ; Hit_AdjM5Chan             = NULL; }
    if (Hit_AdjM2Chan            ) { delete Hit_AdjM2Chan            ; Hit_AdjM2Chan             = NULL; }
    if (Hit_AdjM1Chan            ) { delete Hit_AdjM1Chan            ; Hit_AdjM1Chan             = NULL; }
    if (Hit_AdjP1Chan            ) { delete Hit_AdjP1Chan            ; Hit_AdjP1Chan             = NULL; }
    if (Hit_AdjP2Chan            ) { delete Hit_AdjP2Chan            ; Hit_AdjP2Chan             = NULL; }
    if (Hit_AdjP5Chan            ) { delete Hit_AdjP5Chan            ; Hit_AdjP5Chan             = NULL; }

    if (PDS_Flash_FlashID        ) { delete PDS_Flash_FlashID        ; PDS_Flash_FlashID         = NULL; }
    if (PDS_Flash_YCenter        ) { delete PDS_Flash_YCenter        ; PDS_Flash_YCenter         = NULL; }
    if (PDS_Flash_ZCenter        ) { delete PDS_Flash_ZCenter        ; PDS_Flash_ZCenter         = NULL; }
    if (PDS_Flash_YWidth         ) { delete PDS_Flash_YWidth         ; PDS_Flash_YWidth          = NULL; }
    if (PDS_Flash_ZWidth         ) { delete PDS_Flash_ZWidth         ; PDS_Flash_ZWidth          = NULL; }
    if (PDS_Flash_Time           ) { delete PDS_Flash_Time           ; PDS_Flash_Time            = NULL; }
    if (PDS_Flash_TimeWidth      ) { delete PDS_Flash_TimeWidth      ; PDS_Flash_TimeWidth       = NULL; }
    if (PDS_Flash_TotalPE        ) { delete PDS_Flash_TotalPE        ; PDS_Flash_TotalPE         = NULL; }
    if (PDS_Flash_True_Distance  ) { delete PDS_Flash_True_Distance  ; PDS_Flash_True_Distance   = NULL; }
    if (PDS_Flash_True_GenType   ) { delete PDS_Flash_True_GenType   ; PDS_Flash_True_GenType    = NULL; }

    if (PDS_OpHit_OpChannel      ) { delete PDS_OpHit_OpChannel      ; PDS_OpHit_OpChannel       = NULL; }
    if (PDS_OpHit_X              ) { delete PDS_OpHit_X              ; PDS_OpHit_X               = NULL; }
    if (PDS_OpHit_Y              ) { delete PDS_OpHit_Y              ; PDS_OpHit_Y               = NULL; }
    if (PDS_OpHit_Z              ) { delete PDS_OpHit_Z              ; PDS_OpHit_Z               = NULL; }
    if (PDS_OpHit_PeakTimeAbs    ) { delete PDS_OpHit_PeakTimeAbs    ; PDS_OpHit_PeakTimeAbs     = NULL; }
    if (PDS_OpHit_PeakTime       ) { delete PDS_OpHit_PeakTime       ; PDS_OpHit_PeakTime        = NULL; }
    if (PDS_OpHit_Frame          ) { delete PDS_OpHit_Frame          ; PDS_OpHit_Frame           = NULL; }
    if (PDS_OpHit_Width          ) { delete PDS_OpHit_Width          ; PDS_OpHit_Width           = NULL; }
    if (PDS_OpHit_Area           ) { delete PDS_OpHit_Area           ; PDS_OpHit_Area            = NULL; }
    if (PDS_OpHit_Amplitude      ) { delete PDS_OpHit_Amplitude      ; PDS_OpHit_Amplitude       = NULL; }
    if (PDS_OpHit_PE             ) { delete PDS_OpHit_PE             ; PDS_OpHit_PE              = NULL; }
    if (PDS_OpHit_FastToTotal    ) { delete PDS_OpHit_FastToTotal    ; PDS_OpHit_FastToTotal     = NULL; }
    if (PDS_OpHit_True_GenType   ) { delete PDS_OpHit_True_GenType   ; PDS_OpHit_True_GenType    = NULL; }
    if (PDS_OpHit_True_Energy    ) { delete PDS_OpHit_True_Energy    ; PDS_OpHit_True_Energy     = NULL; }
    if (PDS_OpHit_True_TrackID   ) { delete PDS_OpHit_True_TrackID   ; PDS_OpHit_True_TrackID    = NULL; }
    if (PDS_OpHit_True_GenTypeAll) { delete PDS_OpHit_True_GenTypeAll; PDS_OpHit_True_GenTypeAll = NULL; }
    if (PDS_OpHit_True_EnergyAll ) { delete PDS_OpHit_True_EnergyAll ; PDS_OpHit_True_EnergyAll  = NULL; }
    if (PDS_OpHit_True_TrackIDAll) { delete PDS_OpHit_True_TrackIDAll; PDS_OpHit_True_TrackIDAll = NULL; }
    if (PDS_OpHit_True_IndexAll  ) { delete PDS_OpHit_True_IndexAll  ; PDS_OpHit_True_IndexAll   = NULL; }
    
    if (True_VertexChan          ) { delete True_VertexChan          ; True_VertexChan           = NULL; }
    if (True_Nu_Type             ) { delete True_Nu_Type             ; True_Nu_Type              = NULL; }
    if (True_Nu_Lep_Type         ) { delete True_Nu_Lep_Type         ; True_Nu_Lep_Type          = NULL; }
    if (True_Mode                ) { delete True_Mode                ; True_Mode                 = NULL; }
    if (True_CCNC                ) { delete True_CCNC                ; True_CCNC                 = NULL; }
    if (True_HitNucleon          ) { delete True_HitNucleon          ; True_HitNucleon           = NULL; }
    if (True_Target              ) { delete True_Target              ; True_Target               = NULL; }
    if (True_MarlSample          ) { delete True_MarlSample          ; True_MarlSample           = NULL; }
    if (True_MarlTime            ) { delete True_MarlTime            ; True_MarlTime             = NULL; }
    if (True_MarlWeight          ) { delete True_MarlWeight          ; True_MarlWeight           = NULL; }
    if (True_ENu                 ) { delete True_ENu                 ; True_ENu                  = NULL; }
    if (True_ENu_Lep             ) { delete True_ENu_Lep             ; True_ENu_Lep              = NULL; }
    if (True_VertX               ) { delete True_VertX               ; True_VertX                = NULL; }
    if (True_VertY               ) { delete True_VertY               ; True_VertY                = NULL; }
    if (True_VertZ               ) { delete True_VertZ               ; True_VertZ                = NULL; }
    if (True_VertexT             ) { delete True_VertexT             ; True_VertexT              = NULL; }
    if (True_Px                  ) { delete True_Px                  ; True_Px                   = NULL; }
    if (True_Py                  ) { delete True_Py                  ; True_Py                   = NULL; }
    if (True_Pz                  ) { delete True_Pz                  ; True_Pz                   = NULL; }
    if (True_Dirx                ) { delete True_Dirx                ; True_Dirx                 = NULL; }
    if (True_Diry                ) { delete True_Diry                ; True_Diry                 = NULL; }
    if (True_Dirz                ) { delete True_Dirz                ; True_Dirz                 = NULL; }
    if (True_Time                ) { delete True_Time                ; True_Time                 = NULL; }

    if (True_Bck_Mode            ) { delete True_Bck_Mode            ; True_Bck_Mode             = NULL; }
    if (True_Bck_VertX           ) { delete True_Bck_VertX           ; True_Bck_VertX            = NULL; }
    if (True_Bck_VertY           ) { delete True_Bck_VertY           ; True_Bck_VertY            = NULL; }
    if (True_Bck_VertZ           ) { delete True_Bck_VertZ           ; True_Bck_VertZ            = NULL; }
    if (True_Bck_Time            ) { delete True_Bck_Time            ; True_Bck_Time             = NULL; }
    if (True_Bck_Energy          ) { delete True_Bck_Energy          ; True_Bck_Energy           = NULL; }
    if (True_Bck_PDG             ) { delete True_Bck_PDG             ; True_Bck_PDG              = NULL; }

    if (True_Bck_ModeAll         ) { delete True_Bck_ModeAll         ; True_Bck_ModeAll          = NULL; }
    if (True_Bck_VertXAll        ) { delete True_Bck_VertXAll        ; True_Bck_VertXAll         = NULL; }
    if (True_Bck_VertYAll        ) { delete True_Bck_VertYAll        ; True_Bck_VertYAll         = NULL; }
    if (True_Bck_VertZAll        ) { delete True_Bck_VertZAll        ; True_Bck_VertZAll         = NULL; }
    if (True_Bck_TimeAll         ) { delete True_Bck_TimeAll         ; True_Bck_TimeAll          = NULL; }
    if (True_Bck_EndVertXAll     ) { delete True_Bck_EndVertXAll     ; True_Bck_EndVertXAll      = NULL; }
    if (True_Bck_EndVertYAll     ) { delete True_Bck_EndVertYAll     ; True_Bck_EndVertYAll      = NULL; }
    if (True_Bck_EndVertZAll     ) { delete True_Bck_EndVertZAll     ; True_Bck_EndVertZAll      = NULL; }
    if (True_Bck_EndTimeAll      ) { delete True_Bck_EndTimeAll      ; True_Bck_EndTimeAll       = NULL; }
    if (True_Bck_EnergyAll       ) { delete True_Bck_EnergyAll       ; True_Bck_EnergyAll        = NULL; }
    if (True_Bck_PDGAll          ) { delete True_Bck_PDGAll          ; True_Bck_PDGAll           = NULL; }
    if (True_Bck_IDAll           ) { delete True_Bck_IDAll           ; True_Bck_IDAll            = NULL; }
  };
    
  ~InputManager(){
    ResetVariables();
    if (f_Input) f_Input->Close();
  };
  
  InputManager():
    f_Input(NULL),
    t_Input(NULL),
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

    True_Bck_Mode            (NULL),
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
    True_Bck_EnergyAll       (NULL),
    True_Bck_EndVertXAll     (NULL),
    True_Bck_EndVertYAll     (NULL),
    True_Bck_EndVertZAll     (NULL),
    True_Bck_EndTimeAll      (NULL),
    True_Bck_PDGAll          (NULL),
    True_Bck_IDAll           (NULL){};
  
  int GetEntries() {
    int return_val = t_Input->GetEntries();
    return return_val;
  };
  
  void GetEntry (const int i=0) {
    t_Input->GetEntry(i);
    ResetVariables();
    InstanciateVariables();
    True_VertexChan ->push_back(VertexChan );
    True_Nu_Type    ->push_back(Nu_Type    );
    True_Nu_Lep_Type->push_back(Nu_Lep_Type);
    True_Mode       ->push_back(Mode       );
    True_CCNC       ->push_back(CCNC       );
    True_HitNucleon ->push_back(HitNucleon );
    True_Target     ->push_back(Target     );
    True_MarlSample ->push_back(MarlSample );
    True_MarlTime   ->push_back(MarlTime   );
    True_MarlWeight ->push_back(MarlWeight );
    True_ENu        ->push_back(ENu        );
    True_ENu_Lep    ->push_back(ENu_Lep    );
    True_VertX      ->push_back(VertX      );
    True_VertY      ->push_back(VertY      );
    True_VertZ      ->push_back(VertZ      );
    True_VertexT    ->push_back(VertexT    );
    True_Px         ->push_back(Px         );
    True_Py         ->push_back(Py         );
    True_Pz         ->push_back(Pz         );
    True_Dirx       ->push_back(0          );
    True_Diry       ->push_back(0          );
    True_Dirz       ->push_back(0          );

    for (int i=0; i<NColHit; ++i) {
      Hit_View         ->push_back(HitView           [i]);
      Hit_Size         ->push_back(HitSize           [i]);
      Hit_TPC          ->push_back(HitTPC            [i]);
      Hit_Chan         ->push_back(HitChan           [i]);   
      Hit_Time         ->push_back(HitTime           [i]);
      Hit_RMS          ->push_back(HitRMS            [i]);
      Hit_SADC         ->push_back(HitSADC           [i]);
      Hit_Int          ->push_back(HitInt            [i]);
      Hit_Peak         ->push_back(HitPeak           [i]);
      Hit_True_GenType ->push_back(GenType           [i]);
      Hit_Time         ->push_back(NCorrespondingIDEs[i]);
      Hit_True_Energy  ->push_back(Hit_Energy        [i]);
      Hit_True_EvEnergy->push_back(Hit_Energy        [i]);
      Hit_True_MainTrID->push_back(0);
      Hit_True_nElec   ->push_back(Hit_NumElectrons  [i]);
      Hit_True_X       ->push_back(Hit_X             [i]);
      Hit_True_Y       ->push_back(Hit_Y             [i]);
      Hit_True_Z       ->push_back(Hit_Z             [i]);
      Hit_X_start      ->push_back(Hit_X             [i]);
      Hit_Y_start      ->push_back(Hit_Y             [i]);
      Hit_Z_start      ->push_back(Hit_Z             [i]);
      Hit_X_end        ->push_back(Hit_X             [i]);
      Hit_Y_end        ->push_back(Hit_Y             [i]);
      Hit_Z_end        ->push_back(Hit_Z             [i]);
    }
    
  };
  std::string GetInputFile()                       const { return filename; };
  std::string GetInputTree()                       const { return treename; };
  void        SetInputFile(const std::string s="")       { filename=s; };
  void        SetInputTree(const std::string s="")       { treename=s; };
    
  void LoadTree(){
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
    
    
    t_Input->SetBranchAddress("Run"                 , &Run              );
    t_Input->SetBranchAddress("SubRun"              , &SubRun           );
    t_Input->SetBranchAddress("Event"               , &Event            );
    t_Input->SetBranchAddress("NTotHits"            , &NTotHit          );
    t_Input->SetBranchAddress("NColHits"            , &NColHit          );
    t_Input->SetBranchAddress("NIndHits"            , &NIndHit          );
    t_Input->SetBranchAddress("nHitsNotBackTracked" , &NHitNoBT         );
    t_Input->SetBranchAddress("HitView"             , HitView           );
    t_Input->SetBranchAddress("HitSize"             , HitSize           );
    t_Input->SetBranchAddress("HitTPC"              , HitTPC            );
    t_Input->SetBranchAddress("HitChan"             , HitChan           );
    t_Input->SetBranchAddress("HitTime"             , HitTime           );
    t_Input->SetBranchAddress("HitRMS"              , HitRMS            );
    t_Input->SetBranchAddress("HitSADC"             , HitSADC           );
    t_Input->SetBranchAddress("HitInt"              , HitInt            );
    t_Input->SetBranchAddress("HitPeak"             , HitPeak           );
    t_Input->SetBranchAddress("GenType"             , GenType           );
    t_Input->SetBranchAddress("NCorrespondingIDEs"  , NCorrespondingIDEs);
    t_Input->SetBranchAddress("Hit_X"               , Hit_X             );
    t_Input->SetBranchAddress("Hit_Y"               , Hit_Y             );
    t_Input->SetBranchAddress("Hit_Z"               , Hit_Z             );
    t_Input->SetBranchAddress("Hit_Energy"          , Hit_Energy        );
    t_Input->SetBranchAddress("Hit_NumElectrons"    , Hit_NumElectrons  );
                                                                                     
    t_Input->SetBranchAddress("VertexChan"          , &VertexChan  );
    t_Input->SetBranchAddress("Nu_Type"             , &Nu_Type     );
    t_Input->SetBranchAddress("Nu_Lep_Type"         , &Nu_Lep_Type );
    t_Input->SetBranchAddress("Mode"                , &Mode        );
    t_Input->SetBranchAddress("CCNC"                , &CCNC        );
    t_Input->SetBranchAddress("HitNucleon"          , &HitNucleon  );
    t_Input->SetBranchAddress("Target"              , &Target      );
    t_Input->SetBranchAddress("MarlSample"          , &MarlSample  );
    t_Input->SetBranchAddress("MarlTime"            , &MarlTime    );
    t_Input->SetBranchAddress("MarlWeight"          , &MarlWeight  );
    t_Input->SetBranchAddress("ENu"                 , &ENu         );
    t_Input->SetBranchAddress("ENu_Lep"             , &ENu_Lep     );
    t_Input->SetBranchAddress("VertX"               , &VertX       );
    t_Input->SetBranchAddress("VertY"               , &VertY       );
    t_Input->SetBranchAddress("VertZ"               , &VertZ       );
    t_Input->SetBranchAddress("VertexT"             , &VertexT     );
        
    t_Input->SetBranchAddress("TotGen_Marl", &TotGen_Marl);
    t_Input->SetBranchAddress("TotGen_APA" , &TotGen_APA );
    t_Input->SetBranchAddress("TotGen_CPA" , &TotGen_CPA );
    t_Input->SetBranchAddress("TotGen_Ar39", &TotGen_Ar39);
    t_Input->SetBranchAddress("TotGen_Neut", &TotGen_Neut);
    t_Input->SetBranchAddress("TotGen_Kryp", &TotGen_Kryp);
    t_Input->SetBranchAddress("TotGen_Plon", &TotGen_Plon);
    t_Input->SetBranchAddress("TotGen_Rdon", &TotGen_Rdon);
    t_Input->SetBranchAddress("TotGen_Ar42", &TotGen_Ar42);
    
  };
  
  
};
  
#endif

