

# Detect the XOR gates from AIG


[TOC]

## 題目要求
讀進一個blif檔，轉成AIG後，從Network中偵測出XOR或XNOR，並輸出(OutputNodeId)=xor(OutputNode's LeftChildId,RightChildId)

## Problem
Q1.如何使用ABC的funtion? 
A1.其實ABC就是一個平台，提供了很多API可以使用，把需要的function 定義和實作所在的檔案include進來就可以使用

Q2.如何讀進BLIF檔? 
A2.兩個方法
(1)在自己實作的funtion中可以呼叫Abc_CommandAbc9ReadBlif( Abc_Frame_t * pAbc, int argc, char ** argv )
(2)直接在進入ABC平台前吃進一個script，使用read 4fa.blif的指令即可

Q3.如何轉成AIG?
(1)在自己實作的funtion中可以呼叫Abc_Revised_NtkStrash( Abc_Ntk_t * pNtkOld, Abc_Ntk_t * pNtkNew, int fAllNodes, int fRecord );
(2)直接在進入ABC平台前吃進一個script，使用Strash的指令即可

Q4.如何寫檔?
A4.如以下程式碼
```
FILE* fout = fopen("pa1_d06943013.txt","w");
fprintf(fout,"(%d) = xor ( %d , %d ) ",Abc_ObjId(pObj),Abc_ObjId(lclc),Abc_ObjId(lcrc));  
fclose(fout);                      
```

## 流程

### automatically performed by abc_script

1. read the BLIF.file and the file name must be changed to test file name
2. do the strash
3. type "lsv_xai" command automatically

### performed by init.cpp

4. it will output "pa1_d06943013.txt" file in the same directory
5. ./../../abc <abc_script

**記得每次程式修改完都要記得在abc重新做一次make**

## Linux Tip

### 吃script
```script=
# vim abc_script
read Xor.blif
strash
lsv_xai
```

 ```chmod 777 abc_script```
 把腳本設定成```-rwxrwxrwx 1 root  root     30  十  24 20:35 abc_script```
 直接在進入abc時打入腳本 


```shell=
./abc/abc <abc_script
```

### diff
比較兩個文件的差別，一行一行做比較
[鳥哥的Linux教學](http://linux.vbird.org/linux_basic/0330regularex.php)

## 使用的工具

可以使用Abc_Ntk_t和Abc_Obj_t寫法的function

### Abc_Ntk_t類的API
#### 判斷Network是哪一類
是否為電路圖、Logic Network、Hashed AIG
```c=
// checking the network type
static inline int         Abc_NtkIsNetlist( Abc_Ntk_t * pNtk )       { return pNtk->ntkType == ABC_NTK_NETLIST;     }
static inline int         Abc_NtkIsLogic( Abc_Ntk_t * pNtk )         { return pNtk->ntkType == ABC_NTK_LOGIC;       }
static inline int         Abc_NtkIsStrash( Abc_Ntk_t * pNtk )        { return pNtk->ntkType == ABC_NTK_STRASH;      }
```

Network的類型可以是以下幾種
```c=
//src/base/abc/abc.h
// network types
typedef enum { 
    ABC_NTK_NONE = 0,   // 0:  unknown
    ABC_NTK_NETLIST,    // 1:  network with PIs/POs, latches, nodes, and nets
    ABC_NTK_LOGIC,      // 2:  network with PIs/POs, latches, and nodes
    ABC_NTK_STRASH,     // 3:  structurally hashed AIG (two input AND gates with c-attributes on edges)
    ABC_NTK_OTHER       // 4:  unused
} Abc_NtkType_t;

```

#### 內建好Network的迴圈
```c=
// objects of the network
#define Abc_NtkForEachObj( pNtk, pObj, i )                                                         \
    for ( i = 0; (i < Vec_PtrSize((pNtk)->vObjs)) && (((pObj) = Abc_NtkObj(pNtk, i)), 1); i++ )    \
        if ( (pObj) == NULL ) {} else
#define Abc_NtkForEachObjReverse( pNtk, pNode, i )                                                 \
    for ( i = Vec_PtrSize((pNtk)->vObjs) - 1; (i >= 0) && (((pNode) = Abc_NtkObj(pNtk, i)), 1); i-- ) \
        if ( (pNode) == NULL ) {} else
```

這邊是把Network的Obj iterate的抓出來，由於C不能在迴圈中宣告參數的型別和命字，因此呼叫前必須要定義空的pObj pointer 和 iterator i，如果操作指令只有單行，那就直接縮排接在後面，但如果多行的話，記得要包在方法的本體中
```c=
Abc_Ntk_t * pNtk  = Abc_FrameReadNtk(pAbc);
Abc_Obj_t * pObj;
int i;
Abc_NtkForEachObj( pNtk, pObj, i ){
// Operation body
}
```

如果pNtk已經Strash過後，應該會按照DFS的演算法

[DFS演算法](http://simonsays-tw.com/web/DFS-BFS/DepthFirstSearch.html)
[BFS演算法](http://simonsays-tw.com/web/DFS-BFS/BreadthFirstSearch.html)


### Abc_Obj_t類的API
#### 回傳pObj的Type和Id
```c=
// reading data members of the object
static inline unsigned    Abc_ObjType( Abc_Obj_t * pObj )            { return pObj->Type;               }
static inline unsigned    Abc_ObjId( Abc_Obj_t * pObj )              { return pObj->Id;                 }
```

Type可能是以下幾種

```c=
//src/base/abc/abc.h
// object types
typedef enum { 
    ABC_OBJ_NONE = 0,   //  0:  unknown
    ABC_OBJ_CONST1,     //  1:  constant 1 node (AIG only)
    ABC_OBJ_PI,         //  2:  primary input terminal
    ABC_OBJ_PO,         //  3:  primary output terminal
    ABC_OBJ_BI,         //  4:  box input terminal
    ABC_OBJ_BO,         //  5:  box output terminal
    ABC_OBJ_NET,        //  6:  net
    ABC_OBJ_NODE,       //  7:  node
    ABC_OBJ_LATCH,      //  8:  latch
    ABC_OBJ_WHITEBOX,   //  9:  box with known contents
    ABC_OBJ_BLACKBOX,   // 10:  box with unknown contents
    ABC_OBJ_NUMBER      // 11:  unused
} Abc_ObjType_t;

```

#### 處理pObj的child
```c=
static inline Abc_Obj_t * Abc_ObjFanin( Abc_Obj_t * pObj, int i )    { return (Abc_Obj_t *)pObj->pNtk->vObjs->pArray[ pObj->vFanins.pArray[i] ];   }
static inline Abc_Obj_t * Abc_ObjFanin0( Abc_Obj_t * pObj )          { return (Abc_Obj_t *)pObj->pNtk->vObjs->pArray[ pObj->vFanins.pArray[0] ];   }
static inline Abc_Obj_t * Abc_ObjFanin1( Abc_Obj_t * pObj )          { return (Abc_Obj_t *)pObj->pNtk->vObjs->pArray[ pObj->vFanins.pArray[1] ];   }

static inline int         Abc_ObjFaninC0( Abc_Obj_t * pObj )         { return pObj->fCompl0;                                                }
static inline int         Abc_ObjFaninC1( Abc_Obj_t * pObj )         { return pObj->fCompl1;                                                }

static inline Abc_Obj_t * Abc_ObjChild( Abc_Obj_t * pObj, int i )    { return Abc_ObjNotCond( Abc_ObjFanin(pObj,i), Abc_ObjFaninC(pObj,i) );}
static inline Abc_Obj_t * Abc_ObjChild0( Abc_Obj_t * pObj )          { return Abc_ObjNotCond( Abc_ObjFanin0(pObj), Abc_ObjFaninC0(pObj) );  }
static inline Abc_Obj_t * Abc_ObjChild1( Abc_Obj_t * pObj )          { return Abc_ObjNotCond( Abc_ObjFanin1(pObj), Abc_ObjFaninC1(pObj) );  }
```

* Abc_ObjFanin(pObj,i):需要傳入一個i當index，根據傳入的回傳pObj的第i個child所在的位置
* Abc_ObjFanin0(pObj):回傳pObj的left child所在的位置
* Abc_ObjFanin1(pObj):回傳pObj的right child所在的位置
* Abc_ObjFaninC0(pObj):回傳1或0，為1代表pObj的left child以虛線接到pObj，為0代表pObj的left child以實線接到pObj，C代表complement
* Abc_ObjFaninC1(pObj):回傳1或0，為1代表pObj的right child以虛線接到pObj，為0代表pObj的right child以實線接到pObj，C代表complement
* Abc_ObjChild(pObj,i):需要傳入一個i當index，根據傳入的回傳pObj的第i個child所在的位置，包含了complement的資訊
* Abc_ObjChild0(pObj):回傳pObj的left child所在的位置，包含了complement的資訊
* Abc_ObjChild1(pObj):回傳pObj的right child所在的位置，包含了complement的資訊

* Abc_ObjFanin0(pObj)和Abc_ObjChild0(pObj)的差別是在，Abc_ObjChild0(pObj)偷偷藏了complement的資訊在pointer的末三碼，由於記憶體位置要是8的倍數，所以最後兩碼可以偷偷拿來用，但Abc_ObjChild0(pObj)回傳的記憶體位置不是合法的，如果要回傳合法的位置，要透過呼叫Abc_ObjRegular(pObj)來得到
* Abc_ObjChild0(pObj)只是用來判斷邏輯用的，可以用來比較兩個child是不是同一個child，同時考慮了要指向同一個child還有它們的edge是否極性一致

#### 與Obj極性有關的判斷
```c=
// working with complemented attributes of objects
static inline int         Abc_ObjIsComplement( Abc_Obj_t * p )       { return (int )((ABC_PTRUINT_T)p & (ABC_PTRUINT_T)01);             }
static inline Abc_Obj_t * Abc_ObjRegular( Abc_Obj_t * p )            { return (Abc_Obj_t *)((ABC_PTRUINT_T)p & ~(ABC_PTRUINT_T)01);     }
static inline Abc_Obj_t * Abc_ObjNot( Abc_Obj_t * p )                { return (Abc_Obj_t *)((ABC_PTRUINT_T)p ^  (ABC_PTRUINT_T)01);     }
static inline Abc_Obj_t * Abc_ObjNotCond( Abc_Obj_t * p, int c )     { return (Abc_Obj_t *)((ABC_PTRUINT_T)p ^  (ABC_PTRUINT_T)(c!=0)); }
```

* Abc_ObjNotCond(pObj,c)是有條件的做complement
* Abc_ObjRegular(pObj)是用來正規化pObj的記憶體位置用的



    

## other optional built-in function
```c=
//src/aig/aig/aigUtil.c
int Aig_ObjRecognizeExor( Aig_Obj_t * pObj, Aig_Obj_t ** ppFan0, Aig_Obj_t ** ppFan1 )
{
    Aig_Obj_t * p0, * p1;
    assert( !Aig_IsComplement(pObj) );
    if ( !Aig_ObjIsNode(pObj) )
        return 0;
    if ( Aig_ObjIsExor(pObj) )
    {
        *ppFan0 = Aig_ObjChild0(pObj);
        *ppFan1 = Aig_ObjChild1(pObj);
        return 1;
    }
    assert( Aig_ObjIsAnd(pObj) );
    p0 = Aig_ObjChild0(pObj);
    p1 = Aig_ObjChild1(pObj);
    if ( !Aig_IsComplement(p0) || !Aig_IsComplement(p1) )
        return 0;
    p0 = Aig_Regular(p0);
    p1 = Aig_Regular(p1);
    if ( !Aig_ObjIsAnd(p0) || !Aig_ObjIsAnd(p1) )
        return 0;
    if ( Aig_ObjFanin0(p0) != Aig_ObjFanin0(p1) || Aig_ObjFanin1(p0) != Aig_ObjFanin1(p1) )
        return 0;
    if ( Aig_ObjFaninC0(p0) == Aig_ObjFaninC0(p1) || Aig_ObjFaninC1(p0) == Aig_ObjFaninC1(p1) )
        return 0;
    *ppFan0 = Aig_ObjChild0(p0);
    *ppFan1 = Aig_ObjChild1(p0);
    return 1;
}
```

可以用這個方法，但要小心它是Aig_Obj_t的pointer，不是Abc_Obj_t，而且它不能偵測到XNOR，只能偵測XOR，所以要把程式碼，這一段的判斷改掉

```
if ( Aig_ObjFaninC0(p0) == Aig_ObjFaninC0(p1) || Aig_ObjFaninC1(p0) == Aig_ObjFaninC1(p1) )
        return 0;
```

## My implementation

```c=
//init.cpp
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

            if(Abc_ObjFaninC0(pObj) ==(Abc_ObjFaninC1(pObj)){
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


```

Abc_FrameReadNtk(pAbc)是把現在的Network從pAbc這個Framework中讀出
Abc_NtkForEachObj( pNtk, pObj, i )是一個內建寫好的迴圈，會把pNtk中的所有Object，用DFS的演算法一一取出，放到pObj指到的地方，i是迴圈的iterator，由於c的迴圈中不能宣告變數，所以必須要把pObj和i先宣告好，在call方法

```c=
Abc_Obj_t * pObj;
int i;
Abc_NtkForEachObj( pNtk, pObj, i ){}
```

要做迴圈的instructions如果不只一行時要記得包在body裡面，單行則不用考慮

Abc_ObjFanin0(pObj)會把pObj的left child取出回傳，Abc_ObjFanin1(pObj)會把pObj的right child取出回傳，要小心如果沒有child則不能call這個方法，例如ABC_OBJ_CONST1、ABC_OBJ_PI
而做完Strash之後，每個Node都是AND gate，所以通常都會有lc和rc
另外如果lc或rc屬於PI，則它絕對不會是XOR或XNOR的gate，所以可以直接找下一個node

```c=
if(Abc_ObjType(pObj) == ABC_OBJ_NODE){                           
            lc = Abc_ObjFanin0(pObj);        
            rc = Abc_ObjFanin1(pObj); 
    
            if((Abc_ObjType(lc)!=ABC_OBJ_NODE)||(Abc_ObjType(rc)!=ABC_OBJ_NODE)){
                continue;     
            }   
```

可以先從簡單的XOR或XNOR gate下手，看它被ABC吃掉之後做Strash會有甚麼結果，再去判斷它有甚麼特性


![XOR](https://i.imgur.com/GuNqps1.png)



![XNOR](https://i.imgur.com/88MQBnZ.png)



我們可以從XOR的top node開始看，它的left child和right child都必須要有相同的complement，其實也未必見得都要是虛線，都是實線時，如果在輸出再取complement，有可能是XNOR
XOR的top node，在lc和rc一定有兩組共同的輸入來源，換句話說lclc=rclc且 lcrc=rcrc 或者是 lclc=rcrc且lcrc=rclc
另外當lc、rc各自的共同輸入來源的complement相同時為XNOR，相反時為XOR

(Abc_ObjFaninC0(pObj)的0代表left child，C代表is complement?
當(Abc_ObjFaninC0(pObj)=1，代表pObj的left child是以虛線接到pObj
(Abc_ObjFaninC0(pObj)=0則是left child以實線接到pObj
而Abc_ObjFaninC1(pObj)可得知pObj的right child是以虛線(為1)還是實線(為0)接過來的
Abc_ObjId(pObj)會回傳pObj的Id，在Strash的過程中，有呼叫Abc_NtkForEachObj( pNtk, pObj, i )，因此Id大致上會按照DFS的演算法去排順序

```c=
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

            if((Abc_ObjFaninC0(pObj) == Abc_ObjFaninC1(pObj))){
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

```



## Trace 過程
```c=
//abc/src/base/main/mainInt.h
typedef void (*Abc_Frame_Callback_BmcFrameDone_Func)(int frame, int po, int status);

struct Abc_Frame_t_
{
    // general info
    char *          sVersion;      // the name of the current version
    char *          sBinary;       // the name of the binary running
    // commands, aliases, etc
    st__table *      tCommands;     // the command table
    st__table *      tAliases;      // the alias table
    st__table *      tFlags;        // the flag table
    Vec_Ptr_t *     aHistory;      // the command history
    // the functionality
    Abc_Ntk_t *     pNtkCur;       // the current network
    Abc_Ntk_t *     pNtkBestDelay; // the current network
    Abc_Ntk_t *     pNtkBestArea;  // the current network
    Abc_Ntk_t *     pNtkBackup;    // the current network
    int             nSteps;        // the counter of different network processed
    int             fSource;       // marks the source mode
    int             fAutoexac;     // marks the autoexec mode
    int             fBatchMode;    // batch mode flag
    int             fBridgeMode;   // bridge mode flag
    // output streams
    FILE *          Out;
    FILE *          Err;
    FILE *          Hst;
    // used for runtime measurement
    double          TimeCommand;   // the runtime of the last command
    double          TimeTotal;     // the total runtime of all commands
    // temporary storage for structural choices
    Vec_Ptr_t *     vStore;        // networks to be used by choice
    // decomposition package    
    void *          pManDec;       // decomposition manager
    void *          pManDsd;       // decomposition manager
    void *          pManDsd2;      // decomposition manager
    // libraries for mapping
    void *          pLibLut;       // the current LUT library
    void *          pLibBox;       // the current box library
    void *          pLibGen;       // the current genlib
    void *          pLibGen2;      // the current genlib
    void *          pLibSuper;     // the current supergate library
    void *          pLibScl;       // the current Liberty library
    void *          pAbcCon;       // constraint manager
    // timing constraints
    char *          pDrivingCell;  // name of the driving cell
    float           MaxLoad;       // maximum output load
    // inductive don't-cares
    Vec_Int_t *     vIndFlops;
    int             nIndFrames;

    // new code
    Gia_Man_t *     pGia;          // alternative current network as a light-weight AIG
    Gia_Man_t *     pGia2;         // copy of the above
    Gia_Man_t *     pGiaBest;      // copy of the above
    Gia_Man_t *     pGiaBest2;     // copy of the above
    Gia_Man_t *     pGiaSaved;     // copy of the above
    int             nBestLuts;     // best LUT count
    int             nBestEdges;    // best edge count
    int             nBestLevels;   // best level count
    int             nBestLuts2;     // best LUT count
    int             nBestEdges2;    // best edge count
    int             nBestLevels2;   // best level count
    Abc_Cex_t *     pCex;          // a counter-example to fail the current network
    Abc_Cex_t *     pCex2;         // copy of the above
    Vec_Ptr_t *     vCexVec;       // a vector of counter-examples if more than one PO fails
    Vec_Ptr_t *     vPoEquivs;     // equivalence classes of isomorphic primary outputs
    Vec_Int_t *     vStatuses;     // problem status for each output
    Vec_Int_t *     vAbcObjIds;    // object IDs
    int             Status;                // the status of verification problem (proved=1, disproved=0, undecided=-1)
    int             nFrames;               // the number of time frames completed by BMC
    Vec_Ptr_t *     vPlugInComBinPairs;    // pairs of command and its binary name
    Vec_Ptr_t *     vLTLProperties_global; // related to LTL
    void *          pSave1; 
    void *          pSave2; 
    void *          pSave3; 
    void *          pSave4; 
    void *          pAbc85Ntl;
    void *          pAbc85Ntl2;
    void *          pAbc85Best;
    void *          pAbc85Delay;
    void *          pAbcWlc;
    Vec_Int_t *     pAbcWlcInv;
    void *          pAbcBac;
    void *          pAbcCba;
    void *          pAbcPla;
    Abc_Nam_t *     pJsonStrs;
    Vec_Wec_t *     vJsonObjs;
#ifdef ABC_USE_CUDD
    DdManager *     dd;            // temporary BDD package
#endif
    Gia_Man_t *     pGiaMiniAig; 
    Gia_Man_t *     pGiaMiniLut; 
    Vec_Int_t *     vCopyMiniAig;
    Vec_Int_t *     vCopyMiniLut;
    int *           pArray;
    int *           pBoxes;

    Abc_Frame_Callback_BmcFrameDone_Func pFuncOnFrameDone;
};

typedef void (*Abc_Frame_Initialization_Func)( Abc_Frame_t * pAbc );

struct Abc_FrameInitializer_t_;
typedef struct Abc_FrameInitializer_t_ Abc_FrameInitializer_t;

struct Abc_FrameInitializer_t_
{
    Abc_Frame_Initialization_Func init;
    Abc_Frame_Initialization_Func destroy;

    Abc_FrameInitializer_t* next;
    Abc_FrameInitializer_t* prev;
};
```

pAbc是一個指向Abc_Frame_t結構體的指標，透過pAbc，我們建立起整個ABC的指令串、結構等等







在abc.c中找到進入abc.exe後，輸入strash會有反應的原因
```c=
//abc/src/base/abci/abc.c
Cmd_CommandAdd( pAbc, "Synthesis",    "strash",        Abc_CommandStrash,           1 );
```

要新增方法一定要透過呼叫Cmd_CommandAdd，

```c=
//abc/src/base/cmd/cmdApi.c
void Cmd_CommandAdd( Abc_Frame_t * pAbc, const char * sGroup, const char * sName, Cmd_CommandFuncType pFunc, int fChanges )
{
    const char * key;
    char * value;
    Abc_Command * pCommand;
    int fStatus;

    key = sName;
    if ( st__delete( pAbc->tCommands, &key, &value ) ) 
    {
        // delete existing definition for this command 
        fprintf( pAbc->Err, "Cmd warning: redefining '%s'\n", sName );
        CmdCommandFree( (Abc_Command *)value );
    }

    // create the new command
    pCommand = ABC_ALLOC( Abc_Command, 1 );
    pCommand->sName   = Extra_UtilStrsav( sName );
    pCommand->sGroup  = Extra_UtilStrsav( sGroup );
    pCommand->pFunc   = pFunc;
    pCommand->fChange = fChanges;
    fStatus = st__insert( pAbc->tCommands, pCommand->sName, (char *)pCommand );
    assert( !fStatus );  // the command should not be in the table
}
```

透過在cmd輸入的名字作為key，方法為entry，來建hash_table。sGroup是分類名，sName是在cmd上輸入的指令，Cmd_CommandFuncType是對應到程式裡是哪個方法

```c=
//abc/src/misc/st/st.c 外用的方法
int
 st__insert( st__table *table, const char *key, char *value)
{
    int hash_val;
    st__table_entry *newEntry;
    st__table_entry *ptr, **last;

    hash_val = do_hash(key, table);

    FIND_ENTRY(table, hash_val, key, ptr, last);

    if (ptr == NULL) {
    if (table->num_entries/table->num_bins >= table->max_density) {
        if (rehash(table) == st__OUT_OF_MEM) {
        return st__OUT_OF_MEM;
        }
        hash_val = do_hash(key, table);
    }
    newEntry = ABC_ALLOC( st__table_entry, 1);
    if (newEntry == NULL) {
        return st__OUT_OF_MEM;
    }
    newEntry->key = (char *)key;
    newEntry->record = value;
    newEntry->next = table->bins[hash_val];
    table->bins[hash_val] = newEntry;
    table->num_entries++;
    return 0;
    } else {
    ptr->record = value;
    return 1;
    }
}
```

entry是用串列的方式，把所有的command指令串起來，串到
```c=
//abc/src/misc/st/st.h 
typedef struct st__table_entry st__table_entry;
struct st__table_entry {
    char *key;
    char *record;
    st__table_entry *next;
};

typedef struct st__table st__table;
struct st__table {
    st__compare_func_type compare;
    st__hash_func_type hash;
    int num_bins;
    int num_entries;
    int max_density;
    int reorder_flag;
    double grow_factor;
    st__table_entry **bins;
};
```

```c=
//abc/src/base/cmd/cmdInt.h
struct MvCommand
{
    char *        sName;       // the command name  
    char *        sGroup;      // the group name  
    Cmd_CommandFuncType        pFunc;       // the function to execute the command
    int           fChange;     // set to 1 to mark that the network is changed
};
```

這是ABC自己實作的command的結構體(資料型態)


```c=
//abc/src/base/cmd/cmd.h
typedef struct MvCommand    Abc_Command;  // one command
```

Abc_Command是一種ABC自己實作MvCommand的結構體
 

在Abc_CommandStrash中有用Abc_Ntk_t和Abc_Obj_t的指標

```c=
//abc/src/base/abc/abc.h
struct Abc_Ntk_t_ 
{
    // general information 
    Abc_NtkType_t     ntkType;       // type of the network
    Abc_NtkFunc_t     ntkFunc;       // functionality of the network
    char *            pName;         // the network name
    char *            pSpec;         // the name of the spec file if present
    Nm_Man_t *        pManName;      // name manager (stores names of objects)
    // components of the network
    Vec_Ptr_t *       vObjs;         // the array of all objects (net, nodes, latches, etc)
    Vec_Ptr_t *       vPis;          // the array of primary inputs
    Vec_Ptr_t *       vPos;          // the array of primary outputs
    Vec_Ptr_t *       vCis;          // the array of combinational inputs  (PIs, latches)
    Vec_Ptr_t *       vCos;          // the array of combinational outputs (POs, asserts, latches)
    Vec_Ptr_t *       vPios;         // the array of PIOs
    Vec_Ptr_t *       vBoxes;        // the array of boxes
    Vec_Ptr_t *       vLtlProperties;
    // the number of living objects
    int nObjCounts[ABC_OBJ_NUMBER];  // the number of objects by type
    int               nObjs;         // the number of live objs
    int               nConstrs;      // the number of constraints
    int               nBarBufs;      // the number of barrier buffers
    int               nBarBufs2;     // the number of barrier buffers
    // the backup network and the step number
    Abc_Ntk_t *       pNetBackup;    // the pointer to the previous backup network
    int               iStep;         // the generation number for the given network
    // hierarchy
    Abc_Des_t *       pDesign;       // design (hierarchical networks only)     
    Abc_Ntk_t *       pAltView;      // alternative structural view of the network
    int               fHieVisited;   // flag to mark the visited network
    int               fHiePath;      // flag to mark the network on the path
    int               Id;            // model ID
    double            dTemp;         // temporary value
    // miscellaneous data members
    int               nTravIds;      // the unique traversal IDs of nodes
    Vec_Int_t         vTravIds;      // trav IDs of the objects
    Mem_Fixed_t *     pMmObj;        // memory manager for objects
    Mem_Step_t *      pMmStep;       // memory manager for arrays
    void *            pManFunc;      // functionality manager (AIG manager, BDD manager, or memory manager for SOPs)
    Abc_ManTime_t *   pManTime;      // the timing manager (for mapped networks) stores arrival/required times for all nodes
    void *            pManCut;       // the cut manager (for AIGs) stores information about the cuts computed for the nodes
    float             AndGateDelay;  // an average estimated delay of one AND gate
    int               LevelMax;      // maximum number of levels
    Vec_Int_t *       vLevelsR;      // level in the reverse topological order (for AIGs)
    Vec_Ptr_t *       vSupps;        // CO support information
    int *             pModel;        // counter-example (for miters)
    Abc_Cex_t *       pSeqModel;     // counter-example (for sequential miters)
    Vec_Ptr_t *       vSeqModelVec;  // vector of counter-examples (for sequential miters)
    Abc_Ntk_t *       pExdc;         // the EXDC network (if given)
    void *            pExcare;       // the EXDC network (if given)
    void *            pData;         // misc
    Abc_Ntk_t *       pCopy;         // copy of this network
    void *            pBSMan;        // application manager
    void *            pSCLib;        // SC library
    Vec_Int_t *       vGates;        // SC library gates
    Vec_Int_t *       vPhases;       // fanins phases in the mapped netlist
    char *            pWLoadUsed;    // wire load model used
    float *           pLutTimes;     // arrivals/requireds/slacks using LUT-delay model
    Vec_Ptr_t *       vOnehots;      // names of one-hot-encoded registers
    Vec_Int_t *       vObjPerm;      // permutation saved
    Vec_Int_t *       vTopo;
    Vec_Ptr_t *       vAttrs;        // managers of various node attributes (node functionality, global BDDs, etc)
    Vec_Int_t *       vNameIds;      // name IDs
    Vec_Int_t *       vFins;         // obj/type info
};

```


```c=
//abc/src/base/abc/abc.h
//// network types
typedef enum { 
    ABC_NTK_NONE = 0,   // 0:  unknown
    ABC_NTK_NETLIST,    // 1:  network with PIs/POs, latches, nodes, and nets
    ABC_NTK_LOGIC,      // 2:  network with PIs/POs, latches, and nodes
    ABC_NTK_STRASH,     // 3:  structurally hashed AIG (two input AND gates with c-attributes on edges)
    ABC_NTK_OTHER       // 4:  unused
} Abc_NtkType_t;

// network functionality
typedef enum { 
    ABC_FUNC_NONE = 0,  // 0:  unknown
    ABC_FUNC_SOP,       // 1:  sum-of-products
    ABC_FUNC_BDD,       // 2:  binary decision diagrams
    ABC_FUNC_AIG,       // 3:  and-inverter graphs
    ABC_FUNC_MAP,       // 4:  standard cell library
    ABC_FUNC_BLIFMV,    // 5:  BLIF-MV node functions
    ABC_FUNC_BLACKBOX,  // 6:  black box about which nothing is known
    ABC_FUNC_OTHER      // 7:  unused
} Abc_NtkFunc_t;
```

讀取現在的Network
```c=
//abc/src/base/main/mainFrame.c
Abc_Ntk_t * Abc_FrameReadNtk( Abc_Frame_t * p )
{
    return p->pNtkCur;
}
```

回到Abc_CommandStrash的方法
```c=
//abc/src/base/abci/abc.c
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "acrih" ) ) != EOF )
    {
        switch ( c )
        {
        case 'a':
            fAllNodes ^= 1;
            break;
        case 'c':
            fCleanup ^= 1;
            break;
        case 'r':
            fRecord ^= 1;
            break;
        case 'i':
            fComplOuts ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        Abc_Print( -1, "Empty network.\n" );
        return 1;
    }

    // get the new network
    pNtkRes = Abc_NtkStrash( pNtk, fAllNodes, fCleanup, fRecord );
    if ( pNtkRes == NULL )
    {
        Abc_Print( -1, "Strashing has failed.\n" );
        return 1;
    }
    if ( fComplOuts )
    Abc_NtkForEachPo( pNtkRes, pObj, c )
        Abc_ObjXorFaninC( pObj, 0 );
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    Abc_Print( -2, "usage: strash [-acrih]\n" );
    Abc_Print( -2, "\t        transforms combinational logic into an AIG\n" );
    Abc_Print( -2, "\t-a    : toggles between using all nodes and DFS nodes [default = %s]\n", fAllNodes? "all": "DFS" );
    Abc_Print( -2, "\t-c    : toggles cleanup to remove the dagling AIG nodes [default = %s]\n", fCleanup? "all": "DFS" );
    Abc_Print( -2, "\t-r    : toggles using the record of AIG subgraphs [default = %s]\n", fRecord? "yes": "no" );
    Abc_Print( -2, "\t-i    : toggles complementing the POs of the AIG [default = %s]\n", fComplOuts? "yes": "no" );
    Abc_Print( -2, "\t-h    : print the command usage\n");
    return 1;
```

上半部是在做parsing，下半部要準備做strash


```c=
//abc/src/base/abc/abc.h
extern ABC_DLL Abc_Ntk_t *        Abc_NtkStrash( Abc_Ntk_t * pNtk, int fAllNodes, int fCleanup, int fRecord );

```

```c=
//abc/src/base/abci/abcStrash.c
/**Function*************************************************************

  Synopsis    [Transforms logic network into structurally hashed AIG.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * Abc_NtkStrash( Abc_Ntk_t * pNtk, int fAllNodes, int fCleanup, int fRecord )
{
    Abc_Ntk_t * pNtkAig;
    int nNodes;
    assert( Abc_NtkIsLogic(pNtk) || Abc_NtkIsStrash(pNtk) );
    // consider the special case when the network is already structurally hashed
    if ( Abc_NtkIsStrash(pNtk) )
        return Abc_NtkRestrash( pNtk, fCleanup );
    // convert the node representation in the logic network to the AIG form
    if ( !Abc_NtkToAig(pNtk) )
    {
        printf( "Converting to AIGs has failed.\n" );
        return NULL;
    }
    // perform strashing
//    Abc_NtkCleanCopy( pNtk );
    pNtkAig = Abc_NtkStartFrom( pNtk, ABC_NTK_STRASH, ABC_FUNC_AIG );
    Abc_NtkStrashPerform( pNtk, pNtkAig, fAllNodes, fRecord );
    Abc_NtkFinalize( pNtk, pNtkAig );
    // transfer name IDs
    if ( pNtk->vNameIds )
        Abc_NtkTransferNameIds( pNtk, pNtkAig );
    // print warning about self-feed latches
//    if ( Abc_NtkCountSelfFeedLatches(pNtkAig) )
//        printf( "Warning: The network has %d self-feeding latches.\n", Abc_NtkCountSelfFeedLatches(pNtkAig) );
    // perform cleanup if requested
    nNodes = fCleanup? Abc_AigCleanup((Abc_Aig_t *)pNtkAig->pManFunc) : 0;
//    if ( nNodes )
//        printf( "Warning: AIG cleanup removed %d nodes (this is not a bug).\n", nNodes );
    // duplicate EXDC 
    if ( pNtk->pExdc )
        pNtkAig->pExdc = Abc_NtkStrash( pNtk->pExdc, fAllNodes, fCleanup, fRecord );
    // make sure everything is okay
    if ( !Abc_NtkCheck( pNtkAig ) )
    {
        printf( "Abc_NtkStrash: The network check has failed.\n" );
        Abc_NtkDelete( pNtkAig );
        return NULL;
    }
    return pNtkAig;
}

```

assert( int expression )，assert的作用是先計算表達式expression是否為偽，如果其值為偽（即為0），那麼先向stderr列印出一條錯誤訊息，然後再通過調用abort來終止程序的運行；如果其值為真，就繼續做下去


```c=
//abc/src/base/abc/abc.h

// checking the network type
 

static inline int         Abc_NtkIsLogic( Abc_Ntk_t * pNtk )         { return pNtk->ntkType == ABC_NTK_LOGIC;       }

static inline int         Abc_NtkIsStrash( Abc_Ntk_t * pNtk )        { return pNtk->ntkType == ABC_NTK_STRASH;      }

```

```c=
//abc/src/base/abc/abc.h
/*=== abcStrash.c ==========================================================*/
extern ABC_DLL Abc_Ntk_t *        Abc_NtkRestrash( Abc_Ntk_t * pNtk, int fCleanup );
extern ABC_DLL Abc_Ntk_t *        Abc_NtkRestrashZero( Abc_Ntk_t * pNtk, int fCleanup );
extern ABC_DLL Abc_Ntk_t *        Abc_NtkStrash( Abc_Ntk_t * pNtk, int fAllNodes, int fCleanup, int fRecord );
extern ABC_DLL Abc_Obj_t *        Abc_NodeStrash( Abc_Ntk_t * pNtkNew, Abc_Obj_t * pNode, int fRecord );
extern ABC_DLL int                Abc_NtkAppend( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int fAddPos );
extern ABC_DLL Abc_Ntk_t *        Abc_NtkTopmost( Abc_Ntk_t * pNtk, int nLevels );
```

```c=
//abc/src/base/abci/abcStrash.c
/**Function*************************************************************

  Synopsis    [Reapplies structural hashing to the AIG.]

  Description [Because of the structural hashing, this procedure should not 
  change the number of nodes. It is useful to detect the bugs in the original AIG.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * Abc_NtkRestrash( Abc_Ntk_t * pNtk, int fCleanup )
{
//    extern int timeRetime;
    Vec_Ptr_t * vNodes;
    Abc_Ntk_t * pNtkAig;
    Abc_Obj_t * pObj;
    int i, nNodes;//, RetValue;
    assert( Abc_NtkIsStrash(pNtk) );
//timeRetime = Abc_Clock();
    // print warning about choice nodes
    if ( Abc_NtkGetChoiceNum( pNtk ) )
        printf( "Warning: The choice nodes in the original AIG are removed by strashing.\n" );
    // start the new network (constants and CIs of the old network will point to the their counterparts in the new network)
    pNtkAig = Abc_NtkStartFrom( pNtk, ABC_NTK_STRASH, ABC_FUNC_AIG );
    // restrash the nodes (assuming a topological order of the old network)
    vNodes = Abc_NtkDfs( pNtk, 0 );
    Vec_PtrForEachEntry( Abc_Obj_t *, vNodes, pObj, i )
        pObj->pCopy = Abc_AigAnd( (Abc_Aig_t *)pNtkAig->pManFunc, Abc_ObjChild0Copy(pObj), Abc_ObjChild1Copy(pObj) );
    Vec_PtrFree( vNodes );
    // finalize the network
    Abc_NtkFinalize( pNtk, pNtkAig );
    // print warning about self-feed latches
//    if ( Abc_NtkCountSelfFeedLatches(pNtkAig) )
//        printf( "Warning: The network has %d self-feeding latches.\n", Abc_NtkCountSelfFeedLatches(pNtkAig) );
    // perform cleanup if requested
    if ( fCleanup && (nNodes = Abc_AigCleanup((Abc_Aig_t *)pNtkAig->pManFunc)) ) 
    {
//        printf( "Abc_NtkRestrash(): AIG cleanup removed %d nodes (this is a bug).\n", nNodes );
    }
    // duplicate EXDC 
    if ( pNtk->pExdc )
        pNtkAig->pExdc = Abc_NtkDup( pNtk->pExdc );
    // make sure everything is okay
    if ( !Abc_NtkCheck( pNtkAig ) )
    {
        printf( "Abc_NtkStrash: The network check has failed.\n" );
        Abc_NtkDelete( pNtkAig );
        return NULL;
    }
//timeRetime = Abc_Clock() - timeRetime;
//    if ( RetValue = Abc_NtkRemoveSelfFeedLatches(pNtkAig) )
//        printf( "Modified %d self-feeding latches. The result may not verify.\n", RetValue );
    return pNtkAig;

}

```

```c=
//abc/src/base/abc/abc.h
/*=== abcNtk.c ==========================================================*/
extern ABC_DLL Abc_Ntk_t *        Abc_NtkAlloc( Abc_NtkType_t Type, Abc_NtkFunc_t Func, int fUseMemMan );
extern ABC_DLL Abc_Ntk_t *        Abc_NtkStartFrom( Abc_Ntk_t * pNtk, Abc_NtkType_t Type, Abc_NtkFunc_t Func );

```

```c=
//abc/src/base/abc/abcNtk.c
/**Function*************************************************************

  Synopsis    [Starts a new network using existing network as a model.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * Abc_NtkStartFrom( Abc_Ntk_t * pNtk, Abc_NtkType_t Type, Abc_NtkFunc_t Func )
{
    Abc_Ntk_t * pNtkNew; 
    Abc_Obj_t * pObj;
    int fCopyNames, i;
    if ( pNtk == NULL )
        return NULL;
    // decide whether to copy the names
    fCopyNames = ( Type != ABC_NTK_NETLIST );
    // start the network
    pNtkNew = Abc_NtkAlloc( Type, Func, 1 );
    pNtkNew->nConstrs   = pNtk->nConstrs;
    pNtkNew->nBarBufs   = pNtk->nBarBufs;
    // duplicate the name and the spec
    pNtkNew->pName = Extra_UtilStrsav(pNtk->pName);
    pNtkNew->pSpec = Extra_UtilStrsav(pNtk->pSpec);
    // clean the node copy fields
    Abc_NtkCleanCopy( pNtk );
    // map the constant nodes
    if ( Abc_NtkIsStrash(pNtk) && Abc_NtkIsStrash(pNtkNew) )
        Abc_AigConst1(pNtk)->pCopy = Abc_AigConst1(pNtkNew);
    // clone CIs/CIs/boxes
    Abc_NtkForEachPi( pNtk, pObj, i )
        Abc_NtkDupObj( pNtkNew, pObj, fCopyNames );
    Abc_NtkForEachPo( pNtk, pObj, i )
        Abc_NtkDupObj( pNtkNew, pObj, fCopyNames );
    Abc_NtkForEachBox( pNtk, pObj, i )
        Abc_NtkDupBox( pNtkNew, pObj, fCopyNames );
    // transfer logic level
    Abc_NtkForEachCi( pNtk, pObj, i )
        pObj->pCopy->Level = pObj->Level;
    // transfer the names
//    Abc_NtkTrasferNames( pNtk, pNtkNew );
    Abc_ManTimeDup( pNtk, pNtkNew );
    if ( pNtk->vOnehots )
        pNtkNew->vOnehots = (Vec_Ptr_t *)Vec_VecDupInt( (Vec_Vec_t *)pNtk->vOnehots );
    if ( pNtk->pSeqModel )
        pNtkNew->pSeqModel = Abc_CexDup( pNtk->pSeqModel, Abc_NtkLatchNum(pNtk) );
    if ( pNtk->vObjPerm )
        pNtkNew->vObjPerm = Vec_IntDup( pNtk->vObjPerm );
    pNtkNew->AndGateDelay = pNtk->AndGateDelay;
    if ( pNtkNew->pManTime && Abc_FrameReadLibGen() && pNtkNew->AndGateDelay == 0.0 )
        pNtkNew->AndGateDelay = Mio_LibraryReadDelayAigNode((Mio_Library_t *)Abc_FrameReadLibGen());
    // initialize logic level of the CIs
    if ( pNtk->AndGateDelay != 0.0 && pNtk->pManTime != NULL && pNtk->ntkType != ABC_NTK_STRASH && Type == ABC_NTK_STRASH )
    {
        Abc_NtkForEachCi( pNtk, pObj, i )
            pObj->pCopy->Level = (int)(Abc_MaxFloat(0, Abc_NodeReadArrivalWorst(pObj)) / pNtk->AndGateDelay);
    }
    // check that the CI/CO/latches are copied correctly
    assert( Abc_NtkCiNum(pNtk)    == Abc_NtkCiNum(pNtkNew) );
    assert( Abc_NtkCoNum(pNtk)    == Abc_NtkCoNum(pNtkNew) );
    assert( Abc_NtkLatchNum(pNtk) == Abc_NtkLatchNum(pNtkNew) );
    return pNtkNew;
}

```


```c=
//abc/src/base/abc/abcDfs.c

/**Function*************************************************************

  Synopsis    [Returns the DFS ordered array of logic nodes.]

  Description [Collects only the internal nodes, leaving out CIs and CO.
  However it marks with the current TravId both CIs and COs.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Vec_Ptr_t * Abc_NtkDfs( Abc_Ntk_t * pNtk, int fCollectAll )
{
    Vec_Ptr_t * vNodes;
    Abc_Obj_t * pObj;
    int i;
    // set the traversal ID
    Abc_NtkIncrementTravId( pNtk );
    // start the array of nodes
    vNodes = Vec_PtrAlloc( 100 );
    if ( pNtk->nBarBufs2 > 0 )
    {
        Abc_NtkForEachBarBuf( pNtk, pObj, i )
        {
            Abc_NodeSetTravIdCurrent( pObj );
            Abc_NtkDfs_rec( Abc_ObjFanin0Ntk(Abc_ObjFanin0(pObj)), vNodes );
            Vec_PtrPush( vNodes, pObj );
        }
    }
    Abc_NtkForEachCo( pNtk, pObj, i )
    {
        Abc_NodeSetTravIdCurrent( pObj );
        Abc_NtkDfs_rec( Abc_ObjFanin0Ntk(Abc_ObjFanin0(pObj)), vNodes );
    }
    // collect dangling nodes if asked to
    if ( fCollectAll )
    {
        Abc_NtkForEachNode( pNtk, pObj, i )
            if ( !Abc_NodeIsTravIdCurrent(pObj) )
                Abc_NtkDfs_rec( pObj, vNodes );
    }
    return vNodes;
}

```





```c=
//abc/src/misc/vec/vecPtr.h

//Basic types
typedef struct Vec_Ptr_t_       Vec_Ptr_t;
struct Vec_Ptr_t_ 
{
    int              nCap;
    int              nSize;
    void **          pArray;
};

// inputs and outputs
static inline int Vec_PtrSize( Vec_Ptr_t * p )
{
    return p->nSize;
}

static inline void * Vec_PtrEntry( Vec_Ptr_t * p, int i )
{
    assert( i >= 0 && i < p->nSize );
    return p->pArray[i];
}

```

```c=
//abc/src/base/abc/abc.h

static inline int         Abc_NtkPoNum( Abc_Ntk_t * pNtk )           { return Vec_PtrSize(pNtk->vPos);            }

#define Abc_NtkForEachPo( pNtk, pPo, i )                                                           \
    for ( i = 0; (i < Abc_NtkPoNum(pNtk)) && (((pPo) = Abc_NtkPo(pNtk, i)), 1); i++ )
    

// reading objects
static inline Abc_Obj_t * Abc_NtkPo( Abc_Ntk_t * pNtk, int i )       { return (Abc_Obj_t *)Vec_PtrEntry( pNtk->vPos, i );    }
    
    
```








