#include "DRSA.h"

DRSA::DRSA(int nObj, int nCrt, int* dirs, int nCls, double** f)
{
    this->nObj = nObj;
    this->nCriteria = nCrt;
    this->nClasses = nCls;
    for(int q = 0; q < this->nCriteria; q++){
        this->prefDir[q] = dirs[q];
    }

    /// create universe of objects
    for (int i = 0; i < this->nObj; i++){
        string name = "Object" + to_string(i+1); ///you can use this in C++11
        this->Universe.push_back(new Obj(name, f[i], this->nCriteria));
        this->Clt[(this->Universe[i])->get_Class()].push_back(this->Universe[i]);//allocate this obj to a class
    }

    /// get granules of knowledge
    ObjSet_ptr remainingUniverse = this->Universe; //use this to reduce algorithm complexity (allows simultaneous update of Pdominating and Pdominated sets of two objects X and Y)
    for (ObjSet_ptr::iterator currentObj = this->Universe.begin(); currentObj != this->Universe.end(); ++currentObj){ //loop over all objects of the universe
        (*currentObj)->update_GranulesOfKnowledge(remainingUniverse, this->prefDir);
        remainingUniverse.erase(remainingUniverse.begin()); //erase the first element in order to reduce algorithm complexity
    }

    /// get approximations of unions of classes and boundaries
    for (int t = 1; t <= this->nClasses; t++){ //higher values of t give the most preferable classes (1 - is the worst class/ n>1 - is the best class)
        this->PLU[t] = new P_LowUpwd(t);
        this->PUU[t] = new P_UppUpwd(t);
        this->PLD[t] = new P_LowDnwd(t);
        this->PUD[t] = new P_UppDnwd(t);
        Approximation* aprx;
        for (ObjSet_ptr::iterator currentObj = this->Universe.begin(); currentObj != this->Universe.end(); ++currentObj){ //loop over all objects of the universe
            aprx = this->PLU[t]; aprx->updateMembers((*currentObj));
            aprx = this->PUU[t]; aprx->updateMembers((*currentObj));
            aprx = this->PLD[t]; aprx->updateMembers((*currentObj));
            aprx = this->PUD[t]; aprx->updateMembers((*currentObj));
        }
        ///check rough sets existence (Boundaries)
        this->PBU[t] = new P_BdryUpwd(t, this->PUU[t], this->PLU[t]);
        this->PBD[t] = new P_BdryDnwd(t, this->PUD[t], this->PLD[t]);
        /*
        /// print an approximation (PLU, PUU, PLD...BOUNDARIES...)
        aprx = this->PBU[t];
        aprx->showMembers();
        */
    }
    /*
    ///Print DRSA processed data
    cout << "\n*****************************************\n";
    cout << "number of objects: " << this->nObj << endl;
    cout << "number of criteria: " << this->nCriteria << " [";
    for (int q = 0; q < this->nCriteria; q++){
        if(this->prefDir[q] > 0) cout << "max";
        else cout << "min";
        if(q < this->nCriteria-1)
            cout << ",";
    }
    cout << "]\n";
    cout << "number of classes: " << this->nCriteria;
    cout << "\n*****************************************\n";
    */
    /*
    ///Print class partitions
    for (int t = 1; t <= this->nClasses; t++){ //if s>t, s is preferable to t
        cout << "\nClass " << t << " Contains:\n";
        for (ObjSet_ptr::iterator currentObj = this->Clt[t].begin(); currentObj != this->Clt[t].end(); ++currentObj){
            cout << endl << (*currentObj)->get_Name() << " /";
        }
    }
    */
}

void DRSA::find_rules(string name_algorithm){
    if(name_algorithm.compare("domlem") == 0){
        ///looks like only certain and approximate rules were used in the studies of slowinski, stefanowiski, greco...
        ///they exchanged both types of possible rules by approximate rules
        ///UPDATE NEEDED: you need to transform direction of decision rules with i.e:  q1*prefir[1] >= c1*prefdir[1] ... qn*prefir[n] >= cn*prefdir[n]
        rule_algo = new Domlem(this->prefDir, this->nClasses, this->nCriteria);
        rule_algo->findRules(this->PLU, this->Universe, this->nClasses); /// certain D>= rules activated
        //rule_algo->findRules(this->PUU, this->PBU, this->Universe, this->nClasses ); ///possible D>= rules activated
        rule_algo->findRules(this->PLD, this->Universe, this->nClasses); /// certain D<= rules activated
        //rule_algo->findRules(this->PUD, this->PBD, this->Universe, this->nClasses); ///possible D<= rules activated
        rule_algo->findRules(this->PUU, this->PUD, this->Universe, this->nClasses); ///approximate D=><= rules activated
        rule_algo->showRules();
        ///now you need to classify the input examples in order to TEST your rules
        /// UPDATE NEEDED: I THINK covered rules get mixed... and certain rules predominate... approximate rules are excluded
        rule_algo->classify(this->Universe);
    }
    if(name_algorithm.compare("glance") == 0){
        cout << endl << "Glance algorithm not available in this version" << endl;
    }
}

DRSA::~DRSA()
{
    cout << "\nDeleting DRSA vars...";
    //dtor
}
