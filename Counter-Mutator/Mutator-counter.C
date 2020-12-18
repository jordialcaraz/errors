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
	
/* 	Creamos el process 																						*/
	BPatch_process *proc = BPatch.processCreate(argv[1], (const char**)(argv + 1));
	
/*	Comprobamos que realmente se haya creado 																*/
	assert(proc); 
	
/* 	Creamos la imagen del Mutatee para poder acceder cuando queramos 										*/
	BPatch_image *image = proc->getImage();

/*  Registramos la función de callback que se llamará después de acabar la ejecución 						*/
	BPatch.registerExitCallback( readCounter );
	
/* 	Creamos la variable 'counter' a partir del tipo int  													*/	
	counter = proc->malloc(*image->findType("int") );
	
/*  Ponemos a cero el contador para luego insertarlo en el código del Mutatee
 *	para ello creamos una constante 0 y se la asignamos al counter  										*/
	BPatch_constExpr zero(0);
	BPatch_constExpr one(1);

/*  Registramos la expresión que sumará 1 cada vez que encontremos un foo() y la que pone el contador a cero*/
	//BPatch_arithExpr addOne (BPatch_plus, *counter, one);
    BPatch_arithExpr initSnippet( BPatch_assign , *counter , zero );
	
/*	Hacemos primero la suma de 1 al contador y luego la asignación. 
	Cada vez que se llame a addOne, se sumará 1 al contador													*/
	BPatch_arithExpr plusOne(BPatch_plus, *counter, one);
	BPatch_arithExpr addOne(BPatch_assign, *counter, plusOne);	
	
	// 	Estas dos sentencias se pueden reducir a la siguiente:
	//	BPatch_arithExpr addOne(BPatch_assign,*counter,BPatch_arithExpr(BPatch_plus,*counter,BPatch_constExpr(1)));	

/*  Buscamos el 'main' de la función con 'image' y lo guardamos en un vector 								*/
	BPatch_Vector<BPatch_function*> mainFuncs;
    image->findFunction( "main" , mainFuncs );
	assert (mainFuncs.size() != 0); 			/* Comprobamos que hay un 'main' */  
	BPatch_function *main = mainFuncs[0];		/* Asignamos el 'main' al puntero *main */  
	
/* 	Creamos una entrada a la función 'main' del Mutatee para poder insertar el snippet después 				*/
	std::vector< BPatch_point * > *mainEntryPoints = main->findPoint( BPatch_locEntry );
    BPatch_point *mainEntry = (*mainEntryPoints)[0];
	
/*  Insertamos el snippet a la entrada de 'main'. Con 'BPatch_firstSnippet' y 'BPatch_lastSnippet'
 *  lo que hacemos es decir si queremos insertar el Snippet al principio o al final de la función 'main' 	*/
	proc->insertSnippet( initSnippet, *mainEntry, BPatch_firstSnippet );

/*  Creamos un nuevo vector de funciones para mirar cuántas veces llamamos a foo() 							*/
	BPatch_Vector<BPatch_function*> fooFuncs;
    image->findFunction("foo", fooFuncs);
    assert ( fooFuncs.size() );

    BPatch_function *bp_foo = fooFuncs[0];
	
/*	Cramos de nuevo un vector de puntos de entrada, cogemos su Entry y le asignamos addOne 					*/
	std::vector< BPatch_point * > *fooEntryPoints = bp_foo->findPoint( BPatch_locEntry );
    BPatch_point *fooEntry = (*fooEntryPoints)[0];	
	proc->insertSnippet( addOne, *fooEntry, BPatch_firstSnippet );	

/*  Continuamos con la ejecución hasta que esta acabe 														*/
    proc->continueExecution();
    while (!proc->isTerminated())
        BPatch.waitForStatusChange();
    return 0;

}
