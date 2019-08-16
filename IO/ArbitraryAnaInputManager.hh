#ifndef ARBITRARYANAINPUTMANAGER_HH
#define ARBITRARYANAINPUTMANAGER_HH
#include "InputManager.hh"

#include "TTree.h"


class ArbitraryAnaInputManager: public InputManager {

public:
  int Run;
  int SubRun;
  int Event;
  
  int NTotHit    ;
  int NColHit    ;
  int NIndHit    ;
  int NHitNoBT   ;
  int TotGen;

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

  std::vector<int>    * True_Prim_Index     ;
  std::vector<int>    * True_Prim_Mode      ;
  std::vector<double> * True_Prim_VertStartX;
  std::vector<double> * True_Prim_VertStartY;
  std::vector<double> * True_Prim_VertStartZ;
  std::vector<double> * True_Prim_VertStartT;
  std::vector<double> * True_Prim_DirStartX ;
  std::vector<double> * True_Prim_DirStartY ;
  std::vector<double> * True_Prim_DirStartZ ;
  std::vector<double> * True_Prim_VertEndX  ;
  std::vector<double> * True_Prim_VertEndY  ;
  std::vector<double> * True_Prim_VertEndZ  ;
  std::vector<double> * True_Prim_VertEndT  ;
  std::vector<double> * True_Prim_Energy    ;
  std::vector<int>    * True_Prim_PDG       ;
  std::vector<int>    * True_Prim_ID        ;
  std::vector<int>    * True_Sec_Index      ;
  std::vector<int>    * True_Sec_Mode       ;
  std::vector<double> * True_Sec_VertX      ;
  std::vector<double> * True_Sec_VertY      ;
  std::vector<double> * True_Sec_VertZ      ;
  std::vector<double> * True_Sec_Time       ;
  std::vector<double> * True_Sec_Energy     ;
  std::vector<int>    * True_Sec_PDG        ;
  std::vector<int>    * True_Sec_ID         ;
  
  ArbitraryAnaInputManager():
    Run        (0),
    SubRun     (0),
    Event      (0),
    NTotHit    (0),
    NColHit    (0),
    NIndHit    (0),
    NHitNoBT   (0),
    TotGen(0),
    
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
    True_Prim_Index          (NULL),
    True_Prim_Mode           (NULL),
    True_Prim_VertStartX     (NULL),
    True_Prim_VertStartY     (NULL),
    True_Prim_VertStartZ     (NULL),
    True_Prim_VertStartT     (NULL),
    True_Prim_DirStartX      (NULL),
    True_Prim_DirStartY      (NULL),
    True_Prim_DirStartZ      (NULL),
    True_Prim_VertEndX       (NULL),
    True_Prim_VertEndY       (NULL),
    True_Prim_VertEndZ       (NULL),
    True_Prim_VertEndT       (NULL),
    True_Prim_Energy         (NULL),
    True_Prim_PDG            (NULL),
    True_Prim_ID             (NULL),
    True_Sec_Index           (NULL),
    True_Sec_Mode            (NULL),
    True_Sec_VertX           (NULL),
    True_Sec_VertY           (NULL),
    True_Sec_VertZ           (NULL),
    True_Sec_Time            (NULL),
    True_Sec_Energy          (NULL),
    True_Sec_PDG             (NULL),
    True_Sec_ID              (NULL){};
  
  ~ArbitraryAnaInputManager() {
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

    delete True_Prim_Index       ; True_Prim_Index      = NULL;
    delete True_Prim_Mode        ; True_Prim_Mode       = NULL;
    delete True_Prim_VertStartX  ; True_Prim_VertStartX = NULL;
    delete True_Prim_VertStartY  ; True_Prim_VertStartY = NULL;
    delete True_Prim_VertStartZ  ; True_Prim_VertStartZ = NULL;
    delete True_Prim_VertStartT  ; True_Prim_VertStartT = NULL;
    delete True_Prim_DirStartX   ; True_Prim_DirStartX  = NULL;
    delete True_Prim_DirStartY   ; True_Prim_DirStartY  = NULL;
    delete True_Prim_DirStartZ   ; True_Prim_DirStartZ  = NULL;
    delete True_Prim_VertEndX    ; True_Prim_VertEndX   = NULL;
    delete True_Prim_VertEndY    ; True_Prim_VertEndY   = NULL;
    delete True_Prim_VertEndZ    ; True_Prim_VertEndZ   = NULL;
    delete True_Prim_VertEndT    ; True_Prim_VertEndT   = NULL;
    delete True_Prim_Energy      ; True_Prim_Energy     = NULL;
    delete True_Prim_PDG         ; True_Prim_PDG        = NULL;
    delete True_Prim_ID          ; True_Prim_ID         = NULL;
    delete True_Sec_Index        ; True_Sec_Index       = NULL;
    delete True_Sec_Mode         ; True_Sec_Mode        = NULL;
    delete True_Sec_VertX        ; True_Sec_VertX       = NULL;
    delete True_Sec_VertY        ; True_Sec_VertY       = NULL;
    delete True_Sec_VertZ        ; True_Sec_VertZ       = NULL;
    delete True_Sec_Time         ; True_Sec_Time        = NULL;
    delete True_Sec_Energy       ; True_Sec_Energy      = NULL;
    delete True_Sec_PDG          ; True_Sec_PDG         = NULL;
    delete True_Sec_ID           ; True_Sec_ID          = NULL;
    if (f_Input) f_Input->Close();
  };


  
  void LoadTree(){
    Initialise();   
    
    t_Input->SetBranchAddress("Run"       , &Run       );
    t_Input->SetBranchAddress("SubRun"    , &SubRun    );
    t_Input->SetBranchAddress("Event"     , &Event     );

    t_Input->SetBranchAddress("NTotHit"   , &NTotHit   );
    t_Input->SetBranchAddress("NColHit"   , &NColHit   );
    t_Input->SetBranchAddress("NIndHit"   , &NIndHit   );
    t_Input->SetBranchAddress("NHitNoBT"  , &NHitNoBT  );
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
    
    t_Input->SetBranchAddress("TotGen", &TotGen);
    t_Input->SetBranchAddress("True_Prim_Index"     , &True_Prim_Index     );
    t_Input->SetBranchAddress("True_Prim_Mode"      , &True_Prim_Mode      );
    t_Input->SetBranchAddress("True_Prim_VertStartX", &True_Prim_VertStartX);
    t_Input->SetBranchAddress("True_Prim_VertStartY", &True_Prim_VertStartY);
    t_Input->SetBranchAddress("True_Prim_VertStartZ", &True_Prim_VertStartZ);
    t_Input->SetBranchAddress("True_Prim_VertStartT", &True_Prim_VertStartT);
    t_Input->SetBranchAddress("True_Prim_DirStartX" , &True_Prim_DirStartX );
    t_Input->SetBranchAddress("True_Prim_DirStartY" , &True_Prim_DirStartY );
    t_Input->SetBranchAddress("True_Prim_DirStartZ" , &True_Prim_DirStartZ );
    t_Input->SetBranchAddress("True_Prim_VertEndX"  , &True_Prim_VertEndX  );
    t_Input->SetBranchAddress("True_Prim_VertEndY"  , &True_Prim_VertEndY  );
    t_Input->SetBranchAddress("True_Prim_VertEndZ"  , &True_Prim_VertEndZ  );
    t_Input->SetBranchAddress("True_Prim_VertEndT"  , &True_Prim_VertEndT  );
    t_Input->SetBranchAddress("True_E"    , &True_Prim_Energy    );
    t_Input->SetBranchAddress("True_Prim_PDG"       , &True_Prim_PDG       );
    t_Input->SetBranchAddress("True_Prim_ID"        , &True_Prim_ID        );
    t_Input->SetBranchAddress("True_Sec_Index"      , &True_Sec_Index      );
    t_Input->SetBranchAddress("True_Sec_Mode"       , &True_Sec_Mode       );
    t_Input->SetBranchAddress("True_Sec_VertX"      , &True_Sec_VertX      );
    t_Input->SetBranchAddress("True_Sec_VertY"      , &True_Sec_VertY      );
    t_Input->SetBranchAddress("True_Sec_VertZ"      , &True_Sec_VertZ      );
    t_Input->SetBranchAddress("True_Sec_Time"       , &True_Sec_Time       );
    t_Input->SetBranchAddress("True_Sec_Energy"     , &True_Sec_Energy     );
    t_Input->SetBranchAddress("True_Sec_PDG"        , &True_Sec_PDG        );
    t_Input->SetBranchAddress("True_Sec_ID"         , &True_Sec_ID         );













  };
  
};
#endif
