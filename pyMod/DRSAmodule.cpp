/* https://docs.python.org/3/extending/extending.html#writing-extensions-in-c */
/* https://www.tutorialspoint.com/python3/python_further_extensions.htm */
/* https://stackabuse.com/enhancing-python-with-custom-c-extensions */ //this teaches how to manipulate lists and pass them to c++
/* https://docs.python.org/3/c-api/ */ //full documentation
/* https://docs.python.org/3/extending/newtypes.html */ //how to create new types and make python understand your c++ objects (classes)

#include <Python.h> ///pulls in the python API
#include "../include/DRSA.h"

DRSA* RS_global; //DRSA global variable defined

void errorHandling(int errorCode){
    switch(errorCode){
        case 1:
            cout << "\n[modDRSA_ERROR] nClasses = 0... supply a preference direction vector!\n";
            break;
    }
}

/*
The next thing we add to our module file is the C function that will be called when the Python
 expression DRSA.functionname(args) is evaluated:
*/
static PyObject* DRSA_initialize(PyObject *self, PyObject *args){
    int nObj;
    int nCriteria;
    PyObject* prefDir_temp; //using len(prefDir) you can find the number of classes
    PyObject* f_temp;
    if(!PyArg_ParseTuple(args, "iiOO", &nObj, &nCriteria, &prefDir_temp, &f_temp))
        return NULL; ///An important convention throughout the Python interpreter is the following: when a function fails, it should set an exception condition and return an error value (usually a NULL pointer).
    ///get n_Classes and prefDir
    int nClasses = int (PyObject_Length(prefDir_temp)); //convert from long to int
    if (nClasses < 1){
        errorHandling(1);
        return NULL;
    }
    int* prefDir = new int[nClasses]; //all integers are implemented as long integer objects
    for (int d = 0; d < nClasses; d++){
        PyObject* item = PyList_GetItem(prefDir_temp, d);
        prefDir[d] = (int) PyLong_AsLong(item); //convert from long to int
        //cout << "preference direction of C" << d << ": " << prefDir[d] << endl;
    }
    /// get f
    double** f = new double*[nObj]; //initialize f
    for (int i = 0; i < nObj; i++){
        PyObject* objData = PyList_GetItem(f_temp, i);
        f[i] = new double[nCriteria+1]; //create new data space for object i
        //cout << "obj" << i + 1 << ": ";
        for(int j = 0; j < nCriteria + 1; j++){
            PyObject* val = PyList_GetItem(objData, j);
            f[i][j] = PyFloat_AsDouble(val);
          //  cout << f[i][j] << "\t";
        }
        //cout << endl;
    }
    RS_global = new DRSA(nObj, nCriteria, prefDir, nClasses, f);  //initialize DRSA global variable
    /// // get REDUCTS (still not implemented)
    return PyLong_FromLong(0);
    ///to return the result of the sum:  https://docs.python.org/2.0/ext/buildValue.html
}

static PyObject* DRSA_findRules(PyObject *self, PyObject *args){
    const char* ruleIndAlgo; //rule induction algorithm name
    if(!PyArg_ParseTuple(args, "s", &ruleIndAlgo))
        return NULL;
    RS_global->find_rules(ruleIndAlgo); // find rules
    return PyLong_FromLong(0);
}

///The Module's METHOD TABLE  and INITIALIZATION FUNCTION
//METHOD TABLE
static PyMethodDef DRSAMethods[] = {//I promised to show how spam_system() is called from Python programs. First, we need to list its name and address in a “method table”:
    {"initialize",  DRSA_initialize, METH_VARARGS, "process data and find rough sets"},
    {"findRules",  DRSA_findRules, METH_VARARGS, "rule induction process"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};
/*
Note the third entry (METH_VARARGS). This is a flag telling the interpreter the calling
convention to be used for the C function.
When using only METH_VARARGS, the function should expect the Python-level parameters to
be passed in as a tuple acceptable for parsing via PyArg_ParseTuple();
*/

/*The method table must be referenced in the module definition structure:*/
static struct PyModuleDef DRSAmodule = {
    PyModuleDef_HEAD_INIT,
    "DRSA",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    DRSAMethods
};
/*This structure, in turn, must be passed to the interpreter in the module’s
initialization function. The initialization function must be named PyInit_name(),
 where name is the name of the module, and should be the only non-static item defined in the module file:*/

PyMODINIT_FUNC PyInit_DRSA(void){
    printf("start DRSA moddd\n");
    return PyModule_Create(&DRSAmodule);
    printf("end of module\n");
    ///Note that PyMODINIT_FUNC declares the function as PyObject * return type,
    /// declares any special linkage declarations required by the platform,
    ///and for C++ declares the function as extern "C".
}

///HOW TO BUILD THIS
/*
TERMINAL:
python3.7 setup.py build
sudo python3.7 setup.py install (ready to use...use 'import nameofmodule' [ex.: 'import spam'] in your python file header)
python3.7 testModule.py
*/


