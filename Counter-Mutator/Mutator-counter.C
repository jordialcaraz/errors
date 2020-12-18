#include <stdio.h>
#include "BPatch.h"
#include "BPatch_function.h"

using namespace std;
using namespace Dyninst;

BPatch BPatch;
BPatch_variableExpr *counter = NULL;

static void readCounter(BPatch_thread*, BPatch_exitType) {

	int finalCounterVal;	
	counter->readValue(&finalCounterVal);
	cout << "The number of times called to the same function was: " << finalCounterVal << endl;

}

int main(int argc, char *argv[])
{
	if (argc < 2) {
        fprintf(stderr, "Usage: %s prog_filename [args]\n",argv[0]);
        return 1;
    }
	
/* 	Create process																						*/
	BPatch_process *proc = BPatch.processCreate(argv[1], (const char**)(argv + 1));
	
/*	Check if it was created 																*/
	assert(proc); 
	
/* 	Create image										*/
	BPatch_image *image = proc->getImage();

/*  Register function callback */
	BPatch.registerExitCallback( readCounter );
	
/* 	Create counter variable													*/	
	counter = proc->malloc(*image->findType("int") );
	

	BPatch_constExpr zero(0);
	BPatch_constExpr one(1);

/*  Create expression to increase by one each time foo() is called*/
	//BPatch_arithExpr addOne (BPatch_plus, *counter, one);
    BPatch_arithExpr initSnippet( BPatch_assign , *counter , zero );
	
/*	First add 1 then assign. 
	Each time addOne is called, the counter increases by 1													*/
	BPatch_arithExpr plusOne(BPatch_plus, *counter, one);
	BPatch_arithExpr addOne(BPatch_assign, *counter, plusOne);	
	
	/

/*  Look for 'main' in 'image' and save in a vector 								*/
	BPatch_Vector<BPatch_function*> mainFuncs;
    image->findFunction( "main" , mainFuncs );
	assert (mainFuncs.size() != 0); 			/* Check if there is one 'main' */  
	BPatch_function *main = mainFuncs[0];		/* Assign pointer to 'main' */  
	

	std::vector< BPatch_point * > *mainEntryPoints = main->findPoint( BPatch_locEntry );
    BPatch_point *mainEntry = (*mainEntryPoints)[0];
	

	proc->insertSnippet( initSnippet, *mainEntry, BPatch_firstSnippet );


	BPatch_Vector<BPatch_function*> fooFuncs;
    image->findFunction("foo", fooFuncs);
    assert ( fooFuncs.size() );

    BPatch_function *bp_foo = fooFuncs[0];
	

	std::vector< BPatch_point * > *fooEntryPoints = bp_foo->findPoint( BPatch_locEntry );
    BPatch_point *fooEntry = (*fooEntryPoints)[0];	
	proc->insertSnippet( addOne, *fooEntry, BPatch_firstSnippet );	

/*  Continue execution until the end 														*/
    proc->continueExecution();
    while (!proc->isTerminated())
        BPatch.waitForStatusChange();
    return 0;

}
