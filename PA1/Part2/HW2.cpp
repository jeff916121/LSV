#include "base/main/main.h"
#include "base/main/mainInt.h"
#include "base/abc/abc.h"

//#include "base/abci/abc.c"


#include <iostream>

namespace
{
int Abc_LsvCommandLsvXai(Abc_Frame_t * pAbc, int argc, char ** argv )
{
    

    Abc_Ntk_t * pNtk  = Abc_FrameReadNtk(pAbc);

    Abc_Obj_t * pObj,*  lc, * rc,* lclc,* lcrc,* rclc,* rcrc ;

    int i;

    FILE* fout = fopen("pa1_d06943013.txt","w");

    Abc_NtkForEachObj( pNtk, pObj, i ){ 

        if(Abc_ObjType(pObj) == ABC_OBJ_NODE){                           
            lc = Abc_ObjFanin0(pObj);        
            rc = Abc_ObjFanin1(pObj); 
    
            if((Abc_ObjType(lc)!=ABC_OBJ_NODE)||(Abc_ObjType(rc)!=ABC_OBJ_NODE)){
                continue;     
            }   

            lclc = Abc_ObjFanin0(lc);        
            lcrc = Abc_ObjFanin1(lc);         
            rclc = Abc_ObjFanin0(rc);
            rcrc = Abc_ObjFanin1(rc);

            if((Abc_ObjFaninC0(pObj) == 1) && (Abc_ObjFaninC1(pObj) == 1)){
                if((lclc == rclc)&&(lcrc == rcrc)){
                    if((Abc_ObjFaninC0(lc)==Abc_ObjFaninC1(lc))&&(Abc_ObjFaninC0(rc)==Abc_ObjFaninC1(rc))){   
                        fprintf(fout,"(%d) = xor ( %d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lclc),Abc_ObjId(lcrc));  
                        continue; 
                    }
                    if((Abc_ObjFaninC0(lc)!=Abc_ObjFaninC1(lc))&&(Abc_ObjFaninC0(rc)!=Abc_ObjFaninC1(rc))){   
                        fprintf(fout,"(%d) = xor ( %d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lclc),Abc_ObjId(lcrc));  
                        continue; 
                    }
                }
                else if((lclc==rcrc)&&(lcrc==rclc)){
                    if((Abc_ObjFaninC0(lc)==Abc_ObjFaninC1(lc))&&(Abc_ObjFaninC0(rc)==Abc_ObjFaninC1(rc))){   
                        fprintf(fout,"(%d) = xor ( %d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lclc),Abc_ObjId(lcrc));  
                        continue; 
                    }
                    if((Abc_ObjFaninC0(lc)!=Abc_ObjFaninC1(lc))&&(Abc_ObjFaninC0(rc)!=Abc_ObjFaninC1(rc))){   
                        fprintf(fout,"(%d) = xor ( %d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lclc),Abc_ObjId(lcrc));  
                        continue; 
                    }
                }            
            }
        }
    };
 
    fclose(fout);  
    return 0; 
}



// called during ABC startup
void init(Abc_Frame_t* pAbc)
{
    Cmd_CommandAdd( pAbc, "PA1", "lsv_xai", Abc_LsvCommandLsvXai, 0);
}

// called during ABC termination
void destroy(Abc_Frame_t* pAbc)
{
}

// this object should not be modified after the call to Abc_FrameAddInitializer
Abc_FrameInitializer_t frame_initializer = { init, destroy };

// register the initializer a constructor of a global object
// called before main (and ABC startup)
struct registrar
{
    registrar() 
    {
        Abc_FrameAddInitializer(&frame_initializer);
    }
} hello_registrar;

} // unnamed namespace

