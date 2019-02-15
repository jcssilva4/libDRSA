#include "Approximation.h"

///###################################### BASE CLASS ############################################################################
Approximation::Approximation(int t, string name){
    this->name = name;
    this->t = t;
    //cout << endl << this->name << " approximation created\n";
}

void const Approximation::showMembers(){
    cout << endl << endl << this->name << "\n{\n";
    for (vector<Obj*>::iterator currentObj = this->members.begin(); currentObj != this->members.end(); ++currentObj){
        cout << (*currentObj)->get_Name() << endl;
    }
    cout << "}\n";
}

vector<Obj*> const Approximation::getMembers(){
    return this->members;
}

//dtor
Approximation::~Approximation(){}
///##############################################################################################################################

///###################################### P-Lower approximation of Clt>= ############################################################################
//ctor
P_LowUpwd::P_LowUpwd(int t):Approximation(t, "P-Lower(Cl>=" + to_string(t) + ")"){}
//update members
void P_LowUpwd::updateMembers(Obj* thisObj){
    //check if Dp+ is contained in Clt
    bool obj_belongs = true;
    vector<Obj*> PdomSet = thisObj->get_PdominatingSet();
    for (vector<Obj*>::iterator currentObj = PdomSet.begin(); currentObj != PdomSet.end(); ++currentObj){
        //cout << "\nelement: " << (*currentObj)->get_Class();
        if((*currentObj)->get_Class() < this->t){
            //cout << endl << "Name: " << (*currentObj)->get_Name() << " class is " << (*currentObj)->get_Class() << " < " << t;
            obj_belongs = false;
            break;
        }
    }
    if(obj_belongs) this->members.push_back(thisObj);
}
//dtor
P_LowUpwd::~P_LowUpwd(){}
///##################################################################################################################################################

///###################################### P-Upper approximation of Clt>= ############################################################################
//ctor
P_UppUpwd::P_UppUpwd(int t):Approximation(t, "P-Upper(Cl>=" + to_string(t) + ")"){}
//update members
void P_UppUpwd::updateMembers(Obj* thisObj){
    //check if Obj belongs to Clt>=
    if(thisObj->get_Class() >= this->t){
        vector<Obj*> PdomSet = thisObj->get_PdominatingSet();
        for (vector<Obj*>::iterator currentObj = PdomSet.begin(); currentObj != PdomSet.end(); ++currentObj){
            //cout << endl << " obj being analysed -> " << (*currentObj)->get_Name();
            if(find(this->members.begin(),this->members.end(),(*currentObj)) == this->members.end()){ //check if element is not contained in P-upperClt>=
                //cout << endl << (*currentObj)->get_Name() << " is not contained in " << this->name;
                this->members.push_back((*currentObj));
            }
        }
        //this->showMembers();
        //cout << endl;
    }
}
//dtor
P_UppUpwd::~P_UppUpwd(){}
///##################################################################################################################################################

///###################################### P-Boundary of Clt>= ############################################################################
//ctor
P_BdryUpwd::P_BdryUpwd(int t, P_UppUpwd* PUU, P_LowUpwd* PLU):Approximation(t, "P-Boundary(Cl>=" + to_string(t) + ")"){
    //check if Obj belongs to Clt>=
    vector<Obj*> PUU_members = PUU->getMembers();
    vector<Obj*> PLU_members = PLU->getMembers();
    for (vector<Obj*>::iterator currentObj = PUU_members.begin(); currentObj != PUU_members.end(); ++currentObj){
        if(find(PLU_members.begin(),PLU_members.end(),(*currentObj)) == PLU_members.end()){ //check if element is not contained in P-lowerClt>=
            //x belong to PUU[t] and x do not belong to PLU[t] => x belong do BnU
            this->members.push_back((*currentObj));
        }
    }
    //this->showMembers();
    //cout << endl;
}
//dtor
P_BdryUpwd::~P_BdryUpwd(){}
///##################################################################################################################################################

///###################################### P-Lower approximation of Clt<= ############################################################################
//ctor
P_LowDnwd::P_LowDnwd(int t):Approximation(t, "P-Lower(Cl<=" + to_string(t) + ")"){}
//update members
void P_LowDnwd::updateMembers(Obj* thisObj){
    //check if Dp+ is contained in Clt
    bool obj_belongs = true;
    vector<Obj*> PdomSet = thisObj->get_PdominatedSet();
    for (vector<Obj*>::iterator currentObj = PdomSet.begin(); currentObj != PdomSet.end(); ++currentObj){
        //cout << "\nelement: " << (*currentObj)->get_Class();
        if((*currentObj)->get_Class() > this->t){
            //cout << endl << "Name: " << (*currentObj)->get_Name() << " class is " << (*currentObj)->get_Class() << " < " << t;
            obj_belongs = false;
            break;
        }
    }
    if(obj_belongs) this->members.push_back(thisObj);
}
//dtor
P_LowDnwd::~P_LowDnwd(){}
///##################################################################################################################################################

///###################################### P-Upper approximation of Clt<= ############################################################################
//ctor
P_UppDnwd::P_UppDnwd(int t):Approximation(t, "P-Upper(Cl<=" + to_string(t) + ")"){}
//update members
void P_UppDnwd::updateMembers(Obj* thisObj){
    //check if Obj belongs to Clt>=
    if(thisObj->get_Class() <= this->t){
        vector<Obj*> PdomSet = thisObj->get_PdominatedSet();
        for (vector<Obj*>::iterator currentObj = PdomSet.begin(); currentObj != PdomSet.end(); ++currentObj){
            //cout << endl << " obj being analysed -> " << (*currentObj)->get_Name();
            if(find(this->members.begin(),this->members.end(),(*currentObj)) == this->members.end()){ //check if element is not contained in P-upperClt>=
                //cout << endl << (*currentObj)->get_Name() << " is not contained in " << this->name;
                this->members.push_back((*currentObj));
            }
        }
        //this->showMembers();
        //cout << endl;
    }
}
//dtor
P_UppDnwd::~P_UppDnwd(){}
///##################################################################################################################################################

///###################################### P-Boundary of Clt<= ############################################################################
//ctor
P_BdryDnwd::P_BdryDnwd(int t, P_UppDnwd* PUD, P_LowDnwd* PLD):Approximation(t, "P-Boundary(Cl<=" + to_string(t) + ")"){
    //check if Obj belongs to Clt>=
    vector<Obj*> PUD_members = PUD->getMembers();
    vector<Obj*> PLD_members = PLD->getMembers();
    for (vector<Obj*>::iterator currentObj = PUD_members.begin(); currentObj != PUD_members.end(); ++currentObj){
        if(find(PLD_members.begin(),PLD_members.end(),(*currentObj)) == PLD_members.end()){ //check if element is not contained in P-lowerClt>=
            //x belong to PUU[t] and x do not belong to PLU[t] => x belong do BnU
            this->members.push_back((*currentObj));
        }
    }
    //this->showMembers();
    //cout << endl;
}
//dtor
P_BdryDnwd::~P_BdryDnwd(){}
///##################################################################################################################################################
