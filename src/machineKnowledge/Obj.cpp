#include "Obj.h"

Obj::Obj(string n, double* f, int nCriteria)
{
    this->name = n;
    for(int q = 0; q < nCriteria; q++){this->g[q] = f[q];}
    this->clss = f[nCriteria];
    /*
    /// print name + gi + class
    cout << "\nobjname: " << this->name;
    cout << "\nCriteria Eval: [";
    int q = 0;
    for (vector<double>::iterator it = this->g.begin(); it != this->g.end(); ++it){
        cout << *it;
        if(q < nCriteria-1) cout << ", ";
        ++q;
    }
    cout << "]";
    cout << "\tCl = " << this->clss;
    */
}

Obj::Obj(Obj* thisObj) ///makes a copy of an existing object
{
    this->name = thisObj->get_Name();
    this->clss = thisObj->get_Class();
    this->g = thisObj->get_g();
    /*
    /// print name + gi + class
    cout << "\nobjname: " << this->name;
    cout << "\nCriteria Eval: [";
    int q = 0;
    for (vector<double>::iterator it = this->g.begin(); it != this->g.end(); ++it){
        cout << *it;
        if(q < nCriteria-1) cout << ", ";
        ++q;
    }
    cout << "]";
    cout << "\tCl = " << this->clss;
    */
}

void Obj::update_GranulesOfKnowledge(vector<Obj*> remainingUniverse, map<int,int> prefdir){
    for(vector<Obj*>::iterator obj_ = remainingUniverse.begin(); obj_ != remainingUniverse.end(); ++obj_){
        //if(*obj_!=this){ //compare different objects only...
            //cout << endl << this->name << " Dp " << (*obj_)->get_Name() << " ?";
            //compare values of the two objects in each criterion (XDpY if XSqY for all p belonging to P (Sq is reflexive[XSqX] and transitive))
        vector<char> XSqY;
        map<int, double> otherobj_g = (*obj_)->get_g();
        for (int q = 0; q < this->g.size(); q++){
            if((this->g[q])*(prefdir[q]) < (otherobj_g[q])*(prefdir[q])) XSqY.push_back('<');
            else{
                if( (this->g[q]*prefdir[q]) > (otherobj_g[q])*(prefdir[q])) XSqY.push_back('>'); // X outranks Y in relation to q
                else XSqY.push_back('~');
            }
        }
        /// set Pdominating and Pdominated Sets simultaneously in this object and obj_
        int countTrue = 0, countFalse = 0, countIndifferent = 0;
        for(vector<char>::iterator thisSqY = XSqY.begin();  thisSqY != XSqY.end(); ++ thisSqY){
            switch( *(thisSqY) ){
                case '<': countFalse++;
                    break;
                case '>': countTrue++;
                    break;
                case '~': countIndifferent++;
                    break;
            }
        }
        if(countTrue == (this->g.size()-countIndifferent)){ /// if this object outranks obj_, then:
            this->update_PdominatedSet(*obj_);
            (*obj_)->update_PdominatingSet(this);
            //cout << endl << this->name << " outranks " << (*obj_)->get_Name();
        }
        else if(countFalse == (this->g.size()-countIndifferent)){ /// if obj_ outranks this object, then:
            this->update_PdominatingSet(*obj_);
            (*obj_)->update_PdominatedSet(this);
            //cout << endl << (*obj_)->get_Name() << " outranks " << this->name;
        }
        //}
    }
}

void Obj::update_PdominatingSet(Obj* element){
    this->PdominatingSet.push_back(element);
}

void Obj::update_PdominatedSet(Obj* element){
    this->PdominatedSet.push_back(element);
}

int Obj::get_Class(){
    return this->clss;
}

void Obj::show_PdominatingSet(){
    int countElems = 0;
    cout << this->get_Name() << "\tP-dominating set: \n ";
    cout << "{\n";
    for (vector<Obj*>::iterator currentObj = this->PdominatingSet.begin(); currentObj != this->PdominatingSet.end(); ++currentObj){
        ++countElems;
        cout << "elem" << countElems << ": " << (*currentObj)->get_Name() << "\t(Cl: " << (*currentObj)->get_Class() << ")" << endl;
    }
    cout << "}\n";
}

void Obj::show_PdominatedSet(){
    int countElems = 0;
    cout << this->get_Name() << "\tP-dominated set: \n ";
    cout << "{\n";
    for (vector<Obj*>::iterator currentObj = this->PdominatedSet.begin(); currentObj != this->PdominatedSet.end(); ++currentObj){
        ++countElems;
        cout << "elem" << countElems << ": " << (*currentObj)->get_Name() << "\t(Cl: " << (*currentObj)->get_Class() << ")" << endl;
    }
    cout << "}\n";
}

vector<Obj*> Obj::get_PdominatingSet(){
    return this->PdominatingSet;
}

vector<Obj*> Obj::get_PdominatedSet(){
    return this->PdominatedSet;
}

string Obj::get_Name(){ return this->name;}

map<int, double> Obj::get_g(){ return this->g;};

Obj::~Obj()
{
    //dtor
}
