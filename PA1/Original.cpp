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

   

    int visitandMark [Abc_NtkObjNum(pNtk)];
    int j;
    for(j=0;j<Abc_NtkObjNum(pNtk);j++){
        visitandMark[j]=0;
    }

    j=0;


    Abc_NtkForEachObj( pNtk, pObj, i ){ 
        // printf("There are %d nodes here to visit",Abc_NtkObjNum(pNtk));
        printf("Hi, it's turn to visit ID=%d , Complement=%d, and Type=%d\n",Abc_ObjId(pObj),Abc_ObjIsComplement(pObj),Abc_ObjType(pObj));

        if(Abc_ObjType(pObj)==ABC_OBJ_NODE){                           
            lc = Abc_ObjFanin0(pObj);        
            rc = Abc_ObjFanin1(pObj); 
            // printf("lc = %d and rc = %d constructed\n", Abc_ObjId(lc) , Abc_ObjId(rc) );
    
            if((Abc_ObjType(lc)!=ABC_OBJ_NODE)||(Abc_ObjType(rc)!=ABC_OBJ_NODE)){
                if((Abc_ObjType(lc)==ABC_OBJ_PI)||(Abc_ObjType(rc)==ABC_OBJ_PI)){
                    visitandMark[j]=2;
                    continue;
                }        
            }
            else if((Abc_ObjType(lc)==ABC_OBJ_NODE) && (Abc_ObjType(rc)==ABC_OBJ_NODE) ){      
                lclc = Abc_ObjFanin0(lc);        
                lcrc = Abc_ObjFanin1(lc);         
                rclc = Abc_ObjFanin0(rc);
                rcrc = Abc_ObjFanin1(rc);
            }
            // printf("lclc to rcrc constructed\n");
            printf("Abc_ObjFaninC0(pObj)=%d Abc_ObjFaninC1(pObj)=%d\n",Abc_ObjFaninC0(pObj),Abc_ObjFaninC1(pObj));
            if( ( Abc_ObjFaninC0(pObj) ==1) && (Abc_ObjFaninC1(pObj)==1)){
                if((lclc==rclc)&&(lcrc==rcrc)){
                    if((Abc_ObjFaninC0(lc)!=Abc_ObjFaninC0(rc))&&(Abc_ObjFaninC1(lc)!=Abc_ObjFaninC1(rc))){   
                        if(visitandMark[Abc_ObjId(pObj)]>=3){continue; }
                        else if((Abc_ObjFaninC0(lc)==Abc_ObjFaninC1(lc))&&(Abc_ObjFaninC0(rc)==Abc_ObjFaninC1(rc))){
                            // if(Abc_ObjIsComplement(pObj)==0){
                            //     visitandMark[Abc_ObjId(pObj)]=3; 
                            // }
                            // else{
                                visitandMark[Abc_ObjId(pObj)]=4;
                            // }
                        }
                        else {
                            // if(Abc_ObjIsComplement(pObj)==0){
                            //     visitandMark[Abc_ObjId(pObj)]=4; 
                            // }
                            // else{
                                visitandMark[Abc_ObjId(pObj)]=3;
                            // }                               
                        }   
                        visitandMark[Abc_ObjId(lc)]=1;                   
                        visitandMark[Abc_ObjId(rc)]=1;                                                           
                        continue;
                    }
                }
                else if((lclc==rcrc)&&(lcrc==rclc)){
                    if((Abc_ObjFaninC0(lc)!=Abc_ObjFaninC1(rc))&&(Abc_ObjFaninC1(lc)!=Abc_ObjFaninC0(rc))){
                        if(visitandMark[Abc_ObjId(pObj)]>=3){continue; }
                        else if((Abc_ObjFaninC0(lc)==Abc_ObjFaninC1(lc))&&(Abc_ObjFaninC0(rc)==Abc_ObjFaninC1(rc))){

                            // if(Abc_ObjIsComplement(pObj)==0){
                            //     visitandMark[Abc_ObjId(pObj)]=3; 
                            // }
                            // else{
                                visitandMark[Abc_ObjId(pObj)]=4;
                            // }
                            
                        }
                        else {
                            // if(Abc_ObjIsComplement(pObj)==0){
                            //     visitandMark[Abc_ObjId(pObj)]=4; 
                            // }
                            // else{
                                visitandMark[Abc_ObjId(pObj)]=3;
                            // }                          
                        }   
                        visitandMark[Abc_ObjId(lc)]=1;
                        visitandMark[Abc_ObjId(rc)]=1;                   
                        continue;
                    }
                }


                if(visitandMark[Abc_ObjId(lc)]<=2){
                    visitandMark[Abc_ObjId(lc)]=2;
                }
                if(visitandMark[Abc_ObjId(rc)]<=2){
                    visitandMark[Abc_ObjId(rc)]=2;  
                    continue;
                }                
            }
            else{

                if(visitandMark[Abc_ObjId(lc)]<=2){
                    visitandMark[Abc_ObjId(lc)]=2;
                }
                if(visitandMark[Abc_ObjId(rc)]<=2){
                    visitandMark[Abc_ObjId(rc)]=2;  
                    continue;
                }   

                // if((visitandMark[Abc_ObjId(lc)]==3)&&(Abc_ObjFaninC0(lc)==0)){
                //     visitandMark[Abc_ObjId(lc)]=4;
                // }
                // else if((visitandMark[Abc_ObjId(lc)]==4)&&(Abc_ObjFaninC0(lc)==0)){
                //     visitandMark[Abc_ObjId(lc)]=3;
                // }

                // if((visitandMark[Abc_ObjId(rc)]==3)&&(Abc_ObjFaninC0(rc)==0)){
                //     visitandMark[Abc_ObjId(rc)]=4;
                // }
                // else if((visitandMark[Abc_ObjId(rc)]==4)&&(Abc_ObjFaninC0(rc)==0)){
                //     visitandMark[Abc_ObjId(rc)]=3;
                // }
          
            }
        }
        else {
            if(Abc_ObjType(pObj)!=ABC_OBJ_NODE){
                visitandMark[Abc_ObjId(pObj)]=1;
                continue;
            }          
        }
    };
    // printf("hello\n");
    i=0;
    Abc_NtkForEachObj( pNtk, pObj, i ){ 

        if(visitandMark[Abc_ObjId(pObj)]==1){
            continue;
        }

        // printf("hello0moretime\n");

        if(Abc_ObjType(pObj)!=ABC_OBJ_NODE){continue;}

        lc = Abc_ObjFanin0(pObj);        
        rc = Abc_ObjFanin1(pObj); 


        printf("Abc_ObjId(pObj)=%d,Abc_ObjId(Abc_ObjFanin0(pObj))=%d,Abc_ObjId(Abc_ObjFanin1(pObj))=%d,Abc_ObjFaninC0(pObj)=%d,Abc_ObjFaninC1(pObj)=%d\n",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc),Abc_ObjFaninC0(pObj),Abc_ObjFaninC1(pObj));

        // printf("hello1\n");

        if((visitandMark[Abc_ObjId(pObj)]==0)|| (visitandMark[Abc_ObjId(pObj)]==2)){

            //both lc and rc are xor
            if((visitandMark[Abc_ObjId(lc)]>=3)&&(visitandMark[Abc_ObjId(rc)]>=3)){
                if(Abc_ObjFaninC0(pObj) == 0 && (Abc_ObjFaninC1(pObj) == 0)){
                    fprintf(fout,"(%d) = AND ( ~%d , ~%d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
                else if(Abc_ObjFaninC0(pObj) == 0 && (Abc_ObjFaninC1(pObj) == 1)){
                    fprintf(fout,"(%d) = AND ( ~%d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
                else if(Abc_ObjFaninC0(pObj) == 1 && (Abc_ObjFaninC1(pObj) == 0)){
                    fprintf(fout,"(%d) = AND ( %d , ~%d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
                else if(Abc_ObjFaninC0(pObj) == 1 && (Abc_ObjFaninC1(pObj) == 1)){
                    fprintf(fout,"(%d) = AND ( %d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
            }
            //lc is xor while rc is not
            else if((visitandMark[Abc_ObjId(lc)]>=3)&&(visitandMark[Abc_ObjId(rc)]<3)){
                if(Abc_ObjFaninC0(pObj) == 0 && (Abc_ObjFaninC1(pObj) == 0)){
                    fprintf(fout,"(%d) = AND ( ~%d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
                else if(Abc_ObjFaninC0(pObj) == 0 && (Abc_ObjFaninC1(pObj) == 1)){
                    fprintf(fout,"(%d) = AND ( ~%d , ~%d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
                else if(Abc_ObjFaninC0(pObj) == 1 && (Abc_ObjFaninC1(pObj) == 0)){
                    fprintf(fout,"(%d) = AND ( %d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
                else if(Abc_ObjFaninC0(pObj) == 1 && (Abc_ObjFaninC1(pObj) == 1)){
                    fprintf(fout,"(%d) = AND ( %d , ~%d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
            }
            //rc is xor while lc is not
            else if((visitandMark[Abc_ObjId(lc)]<3)&&(visitandMark[Abc_ObjId(rc)]>=3)){
                if(Abc_ObjFaninC0(pObj) == 0 && (Abc_ObjFaninC1(pObj) == 0)){
                    fprintf(fout,"(%d) = AND ( %d , ~%d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
                else if(Abc_ObjFaninC0(pObj) == 0 && (Abc_ObjFaninC1(pObj) == 1)){
                    fprintf(fout,"(%d) = AND ( %d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
                else if(Abc_ObjFaninC0(pObj) == 1 && (Abc_ObjFaninC1(pObj) == 0)){
                    fprintf(fout,"(%d) = AND ( ~%d , ~%d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
                else if(Abc_ObjFaninC0(pObj) == 1 && (Abc_ObjFaninC1(pObj) == 1)){
                    fprintf(fout,"(%d) = AND ( ~%d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
            }
            //both lc and rc are not xor
            else if((visitandMark[Abc_ObjId(lc)]<3)&&(visitandMark[Abc_ObjId(rc)]<3)){
                if(Abc_ObjFaninC0(pObj) == 0 && (Abc_ObjFaninC1(pObj) == 0)){
                    fprintf(fout,"(%d) = AND ( %d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
                else if(Abc_ObjFaninC0(pObj) == 0 && (Abc_ObjFaninC1(pObj) == 1)){
                    fprintf(fout,"(%d) = AND ( %d , ~%d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
                else if(Abc_ObjFaninC0(pObj) == 1 && (Abc_ObjFaninC1(pObj) == 0)){
                    fprintf(fout,"(%d) = AND ( ~%d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
                else if(Abc_ObjFaninC0(pObj) == 1 && (Abc_ObjFaninC1(pObj) == 1)){
                    fprintf(fout,"(%d) = AND ( ~%d , ~%d ) ",Abc_ObjId(pObj),Abc_ObjId(lc),Abc_ObjId(rc));  
                    continue;
                }
            }
            else{
                printf("Something goes wrong");
                continue; 
            }

            
        }

        // printf("hello2\n");

        lclc = Abc_ObjFanin0(lc);        
        lcrc = Abc_ObjFanin1(lc);         
        rclc = Abc_ObjFanin0(rc);
        rcrc = Abc_ObjFanin1(rc);

        // printf("hello3\n");
 
        if(visitandMark[Abc_ObjId(pObj)]==3){
            fprintf(fout,"(%d) = XOR ( %d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lclc),Abc_ObjId(lcrc));  
            continue; 
        }
        if(visitandMark[Abc_ObjId(pObj)]==4){
            fprintf(fout,"(%d) = XNOR ( %d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lclc),Abc_ObjId(lcrc));  
            continue; 
        }

    };

    for (int i = 0; i < Abc_NtkObjNum(pNtk); ++i)
    {
        printf("visitandMark[%d]=%d\n",i,visitandMark[i]);
    }


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

