#ifndef DATATABLE_H
#define DATATABLE_H

#include "includes.h"

class DataTable
{
    public:
        DataTable(string);
        virtual ~DataTable();
        int get_nObj();
        int get_nCriteria();
        int get_nClasses();
        int* get_prefDir();
        double** get_f();

    protected:

    private:
        int nObj;
        int nCriteria;
        int nClasses;
        int* prefDir; //criteria monotonic restrictions directions (preference direction - max=1,min=-1)
        double** f; // f: UXQ -> V / f(x,q)
};

#endif // DATATABLE_H
