#ifndef OBJ_H
#define OBJ_H

#include "includes.h"

class Obj
{
    public:
        Obj(string, double*, int);
        Obj(Obj*);
        virtual ~Obj();
        string get_Name();
        void update_GranulesOfKnowledge(vector<Obj*>, map<int,int>);
        void update_PdominatingSet(Obj*);
        void update_PdominatedSet(Obj*);
        void show_PdominatingSet();
        void show_PdominatedSet();
        vector<Obj*> get_PdominatingSet();
        vector<Obj*> get_PdominatedSet();
        map<int,double> get_g();
        int get_Class();

    protected:

    private:
        string name; // object's name
        int clss; // object's class
        map<int,double> g; // criteria evaluation on this object
        vector<Obj*> PdominatingSet; // set of objects Obj' such that thisObj_g[i] <= Obj'_g[i] for all i
        vector<Obj*> PdominatedSet; // set of objects Obj' such that thisObj_g[i] >= Obj'_g[i] for all i
};

typedef vector<Obj*> ObjSet_ptr;

#endif // OBJ_H
