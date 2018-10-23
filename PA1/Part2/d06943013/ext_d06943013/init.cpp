#include "base/main/main.h"
#include "base/main/mainInt.h"
#include "base/abc/abc.h"
#include <iostream>

namespace
{
int Abc_LsvCommandLsvXai(Abc_Frame_t * pAbc, int argc, char ** argv )
{
        FILE* fout = fopen("pa1_d06943013.txt","w");
        printf("LSV PA1\n");
	fclose(fout);
	printf(" \n");
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
