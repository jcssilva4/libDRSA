#include "Rule.h"

int Rule::nRules = 0; //initialize static int

Rule::Rule(int _type, int t)
{
    this->rule_type = _type;
    this->Clt = t;
    this->nRules++;
    this->name = "Rule " + to_string(this->nRules);
    //cout << endl << name << " created / Rule_type = " << this->rule_type << " / nRules: " << this->nRules << endl;
}

Rule::Rule(int trash, int t, int s) //since we have t and s, where t>s, this is a approximate rule
{
    this->rule_type = 5;
    for(int i = t; i >= s; i--){
        this->Clts.push_back(i);
    }
    this->nRules++;
    this->name = "Rule " + to_string(this->nRules);
    //cout << endl << name << " created / Rule_type = " << this->rule_type << " / nRules: " << this->nRules << endl;
}

Rule::Rule(int _type, int t, bool testrule) //dont count as a rule
{
    this->rule_type = _type;
    this->Clt = t;
    this->name = "Rule " + to_string(this->nRules);
    //cout << endl << name << " created / Rule_type = " << this->rule_type << " / nRules: " << this->nRules << endl;
}

Rule::Rule(int trash, int t, int s, bool testrule) //dont count as a rule
{
    this->rule_type = 5;
    for(int i = t; i >= s; i--){
        this->Clts.push_back(i);
    }
    this->name = "Rule " + to_string(this->nRules);
    //cout << endl << name << " created / Rule_type = " << this->rule_type << " / nRules: " << this->nRules << endl;
}

bool Rule::checkIfCovers(Obj* thisObj){
    map<int,double> g = thisObj->get_g();
    if(this->rule_type != 5){ // if exact rule
        if (this->elementaryConditions.size() > 0){ //check if there is any condition available...
            map<int,double> g = thisObj->get_g();
            for(map<int, double>::iterator it = this->elementaryConditions.begin(); it != this->elementaryConditions.end(); it++){
                switch(this->rule_type){
                    case 1: // certain or possible D>= rule
                    case 2:
                        if (g[it->first] < it->second) return false; //thisObj violates this D>= certain or possible rule
                        break;
                    case 3:
                    case 4:
                        if (g[it->first] > it->second) return false; //thisObj violates this D<= certain or possible rule
                        break;
                }
            }
            return true;
        }
        else{ return false;}
    }
    else{ //if approximate rule
        if (this->lower_elementaryConditions.size() > 0 || this->upper_elementaryConditions.size() > 0 ){ //check if there is any condition available...
            for(map<int, double>::iterator it = this->lower_elementaryConditions.begin(); it != this->lower_elementaryConditions.end(); it++){
                if (g[it->first] < it->second) return false; //thisObj violates this D>= rule
            }
            for(map<int, double>::iterator it = this->upper_elementaryConditions.begin(); it != this->upper_elementaryConditions.end(); it++){
                if (g[it->first] > it->second) return false; //thisObj violates this D<= rule
            }
            return true;
        }
        else{ return false;}
    }
}

void Rule::setElemCond(int q, double elem){
    this->elementaryConditions[q] = elem;
}

void Rule::setLowElemCond(int q, double elem){
    this->lower_elementaryConditions[q] = elem;
}

void Rule::setUpElemCond(int q, double elem){
    this->upper_elementaryConditions[q] = elem;
}

void Rule::delElemCond(int q){
    this->elementaryConditions.erase(q);
}

void Rule::delLowElemCond(int q){
    this->lower_elementaryConditions.erase(q);
}

void Rule::delUpElemCond(int q){
    this->upper_elementaryConditions.erase(q);
}

void Rule::setCoveredObjs(ObjSet_ptr Universe, bool find_objs){
    if(this->rule_type !=5 && this->elementaryConditions.size() == 0){find_objs = false;} //covers the universe...because there is no elementary condition in this rule
    if(this->rule_type == 5 && this->lower_elementaryConditions.size() == 0 && this->upper_elementaryConditions.size() == 0){find_objs = false;} //covers the universe...because there is no elementary condition in this rule
    if(find_objs){
        if(this->covered.size()==0){ //initialize coveredObjs
            for (ObjSet_ptr::iterator currentObj = Universe.begin(); currentObj != Universe.end(); ++currentObj){
                if(this->checkIfCovers((*currentObj))){
                    this->covered.push_back((*currentObj)); // if current rule covers, include this object
                };
            }
        }
        else{ //THERE IS ANDs in the rule (new condition was included in the rule)
            ObjSet_ptr newCovered;
            for (ObjSet_ptr::iterator currentObj = this->covered.begin(); currentObj != this->covered.end(); ++currentObj){
                if(this->checkIfCovers((*currentObj))){
                    newCovered.push_back((*currentObj)); // if current rule doesn't cover delete this object
                };
            }
            this->covered = newCovered;
        }
    }
    else{
        this->covered = Universe;
    }
}

ObjSet_ptr Rule::getCoveredObjs(){
    return this->covered;
}

void Rule::print(){
    //cout << this->rule_type << endl;
    if(this->rule_type != 5){
        cout << endl << name << " ";
        int cntConds = 0; //count number of elementary conditions printed...
        for(map<int, double>::iterator it = this->elementaryConditions.begin(); it != this->elementaryConditions.end(); it++){
            ++cntConds;
            switch(this->rule_type){
                case 1:
                    cout << "q_" << it->first + 1 << " >= " << it->second;
                    if( cntConds < this->elementaryConditions.size() ){ cout << " AND ";}
                    else{cout << " =>  Cl >= " << Clt;}
                    break;
                case 2:
                    cout << "q_" << it->first + 1 << " >= " << it->second;
                    if( cntConds < this->elementaryConditions.size() ){ cout << " AND ";}
                    else{cout << " =>  (possible)Cl >= " << Clt;}
                    break;
                    break;
                case 3:
                    cout << "q_" << it->first + 1 << " <= " << it->second;
                    if( cntConds < this->elementaryConditions.size() ){ cout << " AND ";}
                    else{cout << " =>  Cl <= " << Clt;}
                    break;
                case 4:
                    cout << "q_" << it->first + 1 << " <= " << it->second;
                    if( cntConds < this->elementaryConditions.size() ){ cout << " AND ";}
                    else{cout << " =>  (possible)Cl <= " << Clt; }
                    break;
            }
        }
    }
    else{
        cout << endl << name << " ";
        int cntConds = 0; //count number of elementary conditions printed...
        for(map<int, double>::iterator it = this->lower_elementaryConditions.begin(); it != this->lower_elementaryConditions.end(); it++){
            ++cntConds;
            cout << "q_" << it->first + 1 << " >= " << it->second;
            if( cntConds < (this->lower_elementaryConditions.size() + this->upper_elementaryConditions.size()) ){ cout << " AND ";}
        }
        for(map<int, double>::iterator it = this->upper_elementaryConditions.begin(); it != this->upper_elementaryConditions.end(); it++){
            ++cntConds;
            cout << "q_" << it->first + 1 << " <= " << it->second;
            if( cntConds < (this->lower_elementaryConditions.size() + this->upper_elementaryConditions.size()) ){ cout << " AND ";}
        }
        cout << " => ";
        int cntClass = 0;
        for(vector<int>::iterator t = this->Clts.begin(); t != this->Clts.end(); t++){
            ++cntClass;
            cout << "Cl" << *t;
            if( cntClass < this->Clts.size() ){ cout << " OR ";}
        }
    }
    cout << "( eval1 = " << this->eval_measure1 << ", eval2 = " << this->eval_measure2 <<" )" << endl;
    cout << "covers: { ";
    for(ObjSet_ptr::iterator thisObj = this->covered.begin(); thisObj != this->covered.end(); thisObj++){
        cout << (*thisObj)->get_Name() << " ";
    }
    cout << "}\n";
}

void Rule::print(int n){
    //cout << this->rule_type << endl;
    if(this->rule_type != 5){
        cout << endl << "Rule " << n << " ";
        int cntConds = 0; //count number of elementary conditions printed...
        for(map<int, double>::iterator it = this->elementaryConditions.begin(); it != this->elementaryConditions.end(); it++){
            ++cntConds;
            switch(this->rule_type){
                case 1:
                    cout << "q_" << it->first + 1 << " >= " << it->second;
                    if( cntConds < this->elementaryConditions.size() ){ cout << " AND ";}
                    else{cout << " =>  Cl >= " << Clt;}
                    break;
                case 2:
                    cout << "q_" << it->first + 1 << " >= " << it->second;
                    if( cntConds < this->elementaryConditions.size() ){ cout << " AND ";}
                    else{cout << " =>  (possible)Cl >= " << Clt;}
                    break;
                    break;
                case 3:
                    cout << "q_" << it->first + 1 << " <= " << it->second;
                    if( cntConds < this->elementaryConditions.size() ){ cout << " AND ";}
                    else{cout << " =>  Cl <= " << Clt;}
                    break;
                case 4:
                    cout << "q_" << it->first + 1 << " <= " << it->second;
                    if( cntConds < this->elementaryConditions.size() ){ cout << " AND ";}
                    else{cout << " =>  (possible)Cl <= " << Clt; }
                    break;
            }
        }
    }
    else{
        cout << endl << "Rule " << n << " ";
        int cntConds = 0; //count number of elementary conditions printed...
        for(map<int, double>::iterator it = this->lower_elementaryConditions.begin(); it != this->lower_elementaryConditions.end(); it++){
            ++cntConds;
            cout << "q_" << it->first + 1 << " >= " << it->second;
            if( cntConds < (this->lower_elementaryConditions.size() + this->upper_elementaryConditions.size()) ){ cout << " AND ";}
        }
        for(map<int, double>::iterator it = this->upper_elementaryConditions.begin(); it != this->upper_elementaryConditions.end(); it++){
            ++cntConds;
            cout << "q_" << it->first + 1 << " <= " << it->second;
            if( cntConds < (this->lower_elementaryConditions.size() + this->upper_elementaryConditions.size()) ){ cout << " AND ";}
        }
        cout << " => ";
        int cntClass = 0;
        for(vector<int>::iterator t = this->Clts.begin(); t != this->Clts.end(); t++){
            ++cntClass;
            cout << "Cl" << *t;
            if( cntClass < this->Clts.size() ){ cout << " OR ";}
        }
    }
    cout << "( eval1 = " << this->eval_measure1 << ", eval2 = " << this->eval_measure2 <<" )" << endl;
    cout << "covers: { ";
    for(ObjSet_ptr::iterator thisObj = this->covered.begin(); thisObj != this->covered.end(); thisObj++){
        cout << (*thisObj)->get_Name() << " ";
    }
    cout << "}\n";
}


map<int, double> Rule::getElemConds(){ return this->elementaryConditions;}

map<int, double> Rule::getLowElemConds(){ return this->lower_elementaryConditions;}

map<int, double> Rule::getUpElemConds(){ return this->upper_elementaryConditions;}

void Rule::setEvals(double eval1, double eval2){
    this->eval_measure1 = eval1;
    this->eval_measure2 = eval2;
}

void Rule::printName(){ cout << endl << this->name; }

int Rule::get_ruletype(){ return this->rule_type; }

int Rule::getDecision() { return this->Clt; }

vector<int> Rule::getDecision(bool isApproximate) { return this->Clts; }

Rule::~Rule()
{
    //dtor
}
