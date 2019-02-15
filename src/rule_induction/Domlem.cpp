#include "RuleInductionAlgorithm.h"

Domlem::Domlem(map<int,int> prefdir, int nclass, int ncriteria):RuleInductionAlgorithm("DOMLEM", prefdir, nclass, ncriteria)
{
    //this->showName();
}

///type 1 rules
void Domlem::findRules(map<int,P_LowUpwd*> Collection_PLU, ObjSet_ptr Universe, int nClasses){
    cout << endl << "Inducing Type 1 Rules  -> certain D>= " << endl;
    /*
    Syntax:
    if f(x,q1) >= rq1 and f(x,q2) >= rq2 and ... f(x,qp) >= rqp, then x belongs to Clt>=
    */
    for (int t = nClasses; t > 1; t--){ //begin from the most strong unions
        P_LowUpwd* PLU = Collection_PLU[t];
        ObjSet_ptr G = PLU->getMembers(); //set of objects from the given approximation
        //this->printSet(G);
        int nCriteria = ((*(G.begin()))->get_g()).size(); ///gambiarra: modifica isso...
        RuleSet_ptr rulesPLU; //initialize set of rules covering PLU
        int contt = 0;
        ObjSet_ptr UniverseTemp = Universe;
        while( G.size() > 0 ){ // do until all objects are covered
            Rule complex_E(1,t); //starting complex
            ObjSet_ptr S = G; //set of objects currently covered by E
            //cout << "\nobjs covered by complex_E = " << (complex_E.getObjsCovered(PLU->getMembers())).size() << endl;
            while( (complex_E.getCoveredObjs()).size() == 0 || !(this->isContainedInPositiveExamples(complex_E, PLU->getMembers())) ){
                /*
                ///Debugging
                cout << "\n\nPress Enter to Continue\n\n";
                cin.ignore();
                */
                double bestEval = 0; //best candidate for elementary condition
                double bestnObj = 0; //cardinality of ( intersection(lEl, S) ), where lEl is the number of objects from S covered by E
                double bestElem = 0;
                int best_q = 0;
                ObjSet_ptr bestCovered;
                vector<double> cond; //f(x,q)
                vector<int> cond_q; /// gambiarra (tirar dps): condition associated criteria
                int condIdx = 0;
                int condcounter = 0;
                for(int q = 0; q < nCriteria; q++){ // for each criterion in P do...
                    for (ObjSet_ptr::iterator obj_in_S = S.begin(); obj_in_S != S.end(); ++obj_in_S){
                        map<int, double> g = (*obj_in_S)->get_g();
                        cond.push_back(g[q]); //for each positive object from S create an elementary condition
                        cond_q.push_back(q);
                    }
                }
                //delete conditions that are already contained in the complex_E elementary conditions
                //Rule complex_E_2(1,true); //create false rule to get complex_E past conditions and reset coveredObjs...
                map<int, double> elemConds = complex_E.getElemConds();
                for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                    cond.erase(find(cond.begin(),cond.end(),it->second));
                    cond_q.erase(find(cond_q.begin(),cond_q.end(),it->first));
                    //complex_E_2.setElemCond(it->first, it->second);
                }
                vector<int>::iterator q = cond_q.begin();
                for(vector<double>::iterator elem = cond.begin(); elem != cond.end(); ++elem){ //find the best condition
                    condcounter++;
                    //Rule tempcomplex_E = complex_E_2;// = complex_E;
                    Rule tempcomplex_E = complex_E;
                    tempcomplex_E.setElemCond(*q, *elem);
                    //cout << "\ncandidate cond e" << condcounter <<" f(x,q" << *q+1 << ") >= " << *elem;
                    //if( tempcomplex_E.getCoveredObjs().size() > 0 ) tempcomplex_E.resetCoveredObjs();
                    tempcomplex_E.setCoveredObjs(UniverseTemp, true);
                    //this->printSet(tempcomplex_E.getCoveredObjs());
                    double currentnObj = (this->getIntersection(tempcomplex_E, G)).size(); //get insersection cardinality
                    double eval_1 = currentnObj / (tempcomplex_E.getCoveredObjs()).size();
                    //cout << " -> eval_1: " << eval_1 << "/ eval_2: " << currentnObj;
                    if( eval_1 > bestEval ){
                        bestElem = *elem;
                        bestEval = eval_1;
                        bestnObj = currentnObj;
                        bestCovered = tempcomplex_E.getCoveredObjs();
                        best_q = *q;
                        condIdx = condcounter;
                    }
                    else{ //draw case currentEval/G.size() == bestEval
                        if( eval_1 == bestEval){
                            //cout << "\ndraw between: e" << condcounter << " and e" << condIdx << " (best)";
                            if(currentnObj > bestnObj){
                                bestElem = *elem;
                                bestEval = eval_1;
                                bestnObj = currentnObj;
                                bestCovered = tempcomplex_E.getCoveredObjs();
                                best_q = *q;
                                condIdx = condcounter;
                            }
                        }
                    }
                    ++q;
                }
                complex_E.setElemCond(best_q, bestElem); //add the best condition to the complex
                complex_E.setEvals(bestEval, bestnObj);
                complex_E.setCoveredObjs(bestCovered, false); //add covered objects
                //complex_E.print();
                S = this->getIntersection(complex_E, S); // intersection(S,best)
                /* ///for debugging
                if(!(this->isContainedInPositiveExamples(complex_E, PLU->getMembers()))){
                    cout << "\nlEl is not contained in B\n";
                }
                else{cout << "\nlEl is contained in B\n";}
                */
            }
            //complex_E.print();
            //create a rule on the basis of E and eliminate superfluous elementary conditions
            ObjSet_ptr objsCovered = complex_E.getCoveredObjs();
            map<int, double> elemConds = complex_E.getElemConds();
            Rule complex_E_2(1,t,true); //create false rule and reset covered objs (its important when more than one elem condition is used)
            for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                complex_E_2.setElemCond(it->first, it->second);
            }
            for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                Rule tempE = complex_E_2;
                tempE.delElemCond(it->first);
                tempE.setCoveredObjs(UniverseTemp, true);
                if(this->isContainedInPositiveExamples(tempE, PLU->getMembers())){
                    complex_E = tempE;
                }
            }
            objsCovered = complex_E.getCoveredObjs();
            //complex_E.print();
            Rule* final_E = new Rule(1,t,true); //create induced rule
            for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                final_E->setElemCond(it->first, it->second);
            }
            final_E->setCoveredObjs(PLU->getMembers(), true);
            double currentnObj = (this->getIntersection(*final_E, PLU->getMembers())).size(); //get insersection cardinality
            final_E->setEvals(currentnObj / (final_E->getCoveredObjs()).size(), currentnObj);
            //final_E->print();
            rulesPLU.push_back(final_E); //add the induced rule
            // remove examples (from G and Universe) covered by the rule
            for (ObjSet_ptr::iterator coveredObj = objsCovered.begin(); coveredObj != objsCovered.end(); ++coveredObj){
                G.erase(find(G.begin(), G.end(), (*coveredObj))); // if current rule covers delete this object
                UniverseTemp.erase(find(UniverseTemp.begin(), UniverseTemp.end(), (*coveredObj)));
            }
            //cout << "\npositive examples still not covered: ";
            //this->printSet(G);
            //cout << endl;
        }
        //cout << "\n\n\ndone for t = " << t << "...checking minimality" << endl << endl;
        this->updateMinimalSet(rulesPLU, PLU->getMembers());
    }
}
///type 2 rules
void Domlem::findRules(map<int,P_UppUpwd*> Collection_PUU, map<int,P_BdryUpwd*> Collection_PBU, ObjSet_ptr Universe, int nClasses){
    cout << endl << "Inducing Type 2 Rules -> possible D>=" << endl;
    /*
    Syntax:
    if f(x,q1) >= rq1 and f(x,q2) >= rq2 and ... f(x,qp) >= rqp, then x possible belongs to Clt>=
    */
    for (int t = nClasses; t > 1; t--){ //begin from the most strong unions
        P_BdryUpwd* PBU = Collection_PBU[t];
        ObjSet_ptr bndry = PBU->getMembers();
        if( bndry.size() > 0 ){ // if there is inconsistency
            P_UppUpwd* PUU = Collection_PUU[t];
            ObjSet_ptr G = PUU->getMembers(); //set of objects from the given approximation
            //this->printSet(G);
            int nCriteria = ((*(G.begin()))->get_g()).size(); ///gambiarra: modifica isso...
            RuleSet_ptr rulesPUU; //initialize set of rules covering PLU
            int contt = 0;
            ObjSet_ptr UniverseTemp = Universe;
            while( G.size() > 0 ){ // do until all objects are covered
                Rule complex_E(2,t); //starting complex
                ObjSet_ptr S = G; //set of objects currently covered by E
                //cout << "\nobjs covered by complex_E = " << (complex_E.getObjsCovered(PLU->getMembers())).size() << endl;
                while( (complex_E.getCoveredObjs()).size() == 0 || !(this->isContainedInPositiveExamples(complex_E, PUU->getMembers())) ){
                    /*
                    ///Debugging
                    cout << "\n\nPress Enter to Continue\n\n";
                    cin.ignore();
                    */
                    double bestEval = 0; //best candidate for elementary condition
                    double bestnObj = 0; //cardinality of ( intersection(lEl, S) ), where lEl is the number of objects from S covered by E
                    double bestElem = 0;
                    int best_q = 0;
                    ObjSet_ptr bestCovered;
                    vector<double> cond; //f(x,q)
                    vector<int> cond_q; /// gambiarra (tirar dps): condition associated criteria
                    int condIdx = 0;
                    int condcounter = 0;
                    for(int q = 0; q < nCriteria; q++){ // for each criterion in P do...
                        for (ObjSet_ptr::iterator obj_in_S = S.begin(); obj_in_S != S.end(); ++obj_in_S){
                            map<int, double> g = (*obj_in_S)->get_g();
                            cond.push_back(g[q]); //for each positive object from S create an elementary condition
                            cond_q.push_back(q);
                        }
                    }
                    //delete conditions that are already contained in the complex_E elementary conditions
                    //Rule complex_E_2(1,true); //create false rule to get complex_E past conditions and reset coveredObjs...
                    map<int, double> elemConds = complex_E.getElemConds();
                    for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                        cond.erase(find(cond.begin(),cond.end(),it->second));
                        cond_q.erase(find(cond_q.begin(),cond_q.end(),it->first));
                        //complex_E_2.setElemCond(it->first, it->second);
                    }
                    vector<int>::iterator q = cond_q.begin();
                    for(vector<double>::iterator elem = cond.begin(); elem != cond.end(); ++elem){ //find the best condition
                        condcounter++;
                        //Rule tempcomplex_E = complex_E_2;// = complex_E;
                        Rule tempcomplex_E = complex_E;
                        tempcomplex_E.setElemCond(*q, *elem);
                        //cout << "\ncandidate cond e" << condcounter <<" f(x,q" << *q+1 << ") >= " << *elem;
                        //if( tempcomplex_E.getCoveredObjs().size() > 0 ) tempcomplex_E.resetCoveredObjs();
                        tempcomplex_E.setCoveredObjs(UniverseTemp, true);
                        //this->printSet(tempcomplex_E.getCoveredObjs());
                        double currentnObj = (this->getIntersection(tempcomplex_E, G)).size(); //get insersection cardinality
                        double eval_1 = currentnObj / (tempcomplex_E.getCoveredObjs()).size();
                        //cout << " -> eval_1: " << eval_1 << "/ eval_2: " << currentnObj;
                        if( eval_1 > bestEval ){
                            bestElem = *elem;
                            bestEval = eval_1;
                            bestnObj = currentnObj;
                            bestCovered = tempcomplex_E.getCoveredObjs();
                            best_q = *q;
                            condIdx = condcounter;
                        }
                        else{ //draw case currentEval/G.size() == bestEval
                            if( eval_1 == bestEval){
                                //cout << "\ndraw between: e" << condcounter << " and e" << condIdx << " (best)";
                                if(currentnObj > bestnObj){
                                    bestElem = *elem;
                                    bestEval = eval_1;
                                    bestnObj = currentnObj;
                                    bestCovered = tempcomplex_E.getCoveredObjs();
                                    best_q = *q;
                                    condIdx = condcounter;
                                }
                            }
                        }
                        ++q;
                    }
                    complex_E.setElemCond(best_q, bestElem); //add the best condition to the complex
                    complex_E.setEvals(bestEval, bestnObj);
                    complex_E.setCoveredObjs(bestCovered, false); //add covered objects
                    //complex_E.print();
                    S = this->getIntersection(complex_E, S); // intersection(S,best)
                    /* ///for debugging
                    if(!(this->isContainedInPositiveExamples(complex_E, PLU->getMembers()))){
                        cout << "\nlEl is not contained in B\n";
                    }
                    else{cout << "\nlEl is contained in B\n";}
                    */
                }
                //complex_E.print();
                //create a rule on the basis of E and eliminate superfluous elementary conditions
                ObjSet_ptr objsCovered = complex_E.getCoveredObjs();
                map<int, double> elemConds = complex_E.getElemConds();
                Rule complex_E_2(2,t,true); //create false rule and reset covered objs (its important when more than one elem condition is used)
                for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                    complex_E_2.setElemCond(it->first, it->second);
                }
                for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                    Rule tempE = complex_E_2;
                    tempE.delElemCond(it->first);
                    tempE.setCoveredObjs(UniverseTemp, true);
                    if(this->isContainedInPositiveExamples(tempE, PUU->getMembers())){
                        complex_E = tempE;
                    }
                }
                objsCovered = complex_E.getCoveredObjs();
                //complex_E.print();
                Rule* final_E = new Rule(2,t,true); //create induced rule
                for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                    final_E->setElemCond(it->first, it->second);
                }
                final_E->setCoveredObjs(PUU->getMembers(), true);
                double currentnObj = (this->getIntersection(*final_E, PUU->getMembers())).size(); //get insersection cardinality
                final_E->setEvals(currentnObj / (final_E->getCoveredObjs()).size(), currentnObj);
                //final_E->print();
                rulesPUU.push_back(final_E); //add the induced rule
                // remove examples (from G and Universe) covered by the rule
                for (ObjSet_ptr::iterator coveredObj = objsCovered.begin(); coveredObj != objsCovered.end(); ++coveredObj){
                    G.erase(find(G.begin(), G.end(), (*coveredObj))); // if current rule covers delete this object
                    UniverseTemp.erase(find(UniverseTemp.begin(), UniverseTemp.end(), (*coveredObj)));
                }
                //cout << "\npositive examples still not covered: ";
                //this->printSet(G);
                //cout << endl;
            }
            //cout << "\n\n\ndone for t = " << t << "...checking minimality" << endl << endl;
            this->updateMinimalSet(rulesPUU, PUU->getMembers());
        }
    }
};
///type 3 rules
void Domlem::findRules(map<int,P_LowDnwd*> Collection_PLD, ObjSet_ptr Universe, int nClasses){
    cout << endl << "Inducing Type 3 Rules -> certain D<=" << endl;
    /*
    Syntax:
    if f(x,q1) <= rq1 and f(x,q2) <= rq2 and ... f(x,qp) <= rqp, then x belongs to Clt<=
    */
    for (int t = 1; t < nClasses; t++){ //begin from the most strong unions
        P_LowDnwd* PLD = Collection_PLD[t];
        ObjSet_ptr G = PLD->getMembers(); //set of objects from the given approximation
        //this->printSet(G);
        int nCriteria = ((*(G.begin()))->get_g()).size(); ///gambiarra: modifica isso...
        RuleSet_ptr rulesPLD; //initialize set of rules covering PLU
        int contt = 0;
        ObjSet_ptr UniverseTemp = Universe;
        while( G.size() > 0 ){ // do until all objects are covered
            Rule complex_E(3,t); //starting complex
            ObjSet_ptr S = G; //set of objects currently covered by E
            //cout << "\nobjs covered by complex_E = " << (complex_E.getObjsCovered(PLU->getMembers())).size() << endl;
            while( (complex_E.getCoveredObjs()).size() == 0 || !(this->isContainedInPositiveExamples(complex_E, PLD->getMembers())) ){
                /*
                ///Debugging
                cout << "\n\nPress Enter to Continue\n\n";
                cin.ignore();
                */
                double bestEval = 0; //best candidate for elementary condition
                double bestnObj = 0; //cardinality of ( intersection(lEl, S) ), where lEl is the number of objects from S covered by E
                double bestElem = 0;
                int best_q = 0;
                ObjSet_ptr bestCovered;
                vector<double> cond; //f(x,q)
                vector<int> cond_q; /// gambiarra (tirar dps): condition associated criteria
                int condIdx = 0;
                int condcounter = 0;
                for(int q = 0; q < nCriteria; q++){ // for each criterion in P do...
                    for (ObjSet_ptr::iterator obj_in_S = S.begin(); obj_in_S != S.end(); ++obj_in_S){
                        map<int, double> g = (*obj_in_S)->get_g();
                        cond.push_back(g[q]); //for each positive object from S create an elementary condition
                        cond_q.push_back(q);
                    }
                }
                //delete conditions that are already contained in the complex_E elementary conditions
                //Rule complex_E_2(1,true); //create false rule to get complex_E past conditions and reset coveredObjs...
                map<int, double> elemConds = complex_E.getElemConds();
                for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                    cond.erase(find(cond.begin(),cond.end(),it->second));
                    cond_q.erase(find(cond_q.begin(),cond_q.end(),it->first));
                    //complex_E_2.setElemCond(it->first, it->second);
                }
                vector<int>::iterator q = cond_q.begin();
                for(vector<double>::iterator elem = cond.begin(); elem != cond.end(); ++elem){ //find the best condition
                    condcounter++;
                    //Rule tempcomplex_E = complex_E_2;// = complex_E;
                    Rule tempcomplex_E = complex_E;
                    tempcomplex_E.setElemCond(*q, *elem);
                    //cout << "\ncandidate cond e" << condcounter <<" f(x,q" << *q+1 << ") >= " << *elem;
                    //if( tempcomplex_E.getCoveredObjs().size() > 0 ) tempcomplex_E.resetCoveredObjs();
                    tempcomplex_E.setCoveredObjs(UniverseTemp, true);
                    //this->printSet(tempcomplex_E.getCoveredObjs());
                    double currentnObj = (this->getIntersection(tempcomplex_E, G)).size(); //get insersection cardinality
                    double eval_1 = currentnObj / (tempcomplex_E.getCoveredObjs()).size();
                    //cout << " -> eval_1: " << eval_1 << "/ eval_2: " << currentnObj;
                    if( eval_1 > bestEval ){
                        bestElem = *elem;
                        bestEval = eval_1;
                        bestnObj = currentnObj;
                        bestCovered = tempcomplex_E.getCoveredObjs();
                        best_q = *q;
                        condIdx = condcounter;
                    }
                    else{ //draw case currentEval/G.size() == bestEval
                        if( eval_1 == bestEval){
                            //cout << "\ndraw between: e" << condcounter << " and e" << condIdx << " (best)";
                            if(currentnObj > bestnObj){
                                bestElem = *elem;
                                bestEval = eval_1;
                                bestnObj = currentnObj;
                                bestCovered = tempcomplex_E.getCoveredObjs();
                                best_q = *q;
                                condIdx = condcounter;
                            }
                        }
                    }
                    ++q;
                }
                complex_E.setElemCond(best_q, bestElem); //add the best condition to the complex
                complex_E.setEvals(bestEval, bestnObj);
                complex_E.setCoveredObjs(bestCovered, false); //add covered objects
                //complex_E.print();
                S = this->getIntersection(complex_E, S); // intersection(S,best)
                /* ///for debugging
                if(!(this->isContainedInPositiveExamples(complex_E, PLU->getMembers()))){
                    cout << "\nlEl is not contained in B\n";
                }
                else{cout << "\nlEl is contained in B\n";}
                */
            }
            //complex_E.print();
            //create a rule on the basis of E and eliminate superfluous elementary conditions
            ObjSet_ptr objsCovered = complex_E.getCoveredObjs();
            map<int, double> elemConds = complex_E.getElemConds();
            Rule complex_E_2(3,t,true); //create false rule and reset covered objs (its important when more than one elem condition is used)
            for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                complex_E_2.setElemCond(it->first, it->second);
            }
            for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                Rule tempE = complex_E_2;
                tempE.delElemCond(it->first);
                tempE.setCoveredObjs(UniverseTemp, true);
                if(this->isContainedInPositiveExamples(tempE, PLD->getMembers())){
                    complex_E = tempE;
                }
            }
            objsCovered = complex_E.getCoveredObjs();
            //complex_E.print();
            Rule* final_E = new Rule(3,t,true); //create induced rule
            for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                final_E->setElemCond(it->first, it->second);
            }
            final_E->setCoveredObjs(PLD->getMembers(), true);
            double currentnObj = (this->getIntersection(*final_E, PLD->getMembers())).size(); //get insersection cardinality
            final_E->setEvals(currentnObj / (final_E->getCoveredObjs()).size(), currentnObj);
            //final_E->print();
            rulesPLD.push_back(final_E); //add the induced rule
            // remove examples (from G and Universe) covered by the rule
            for (ObjSet_ptr::iterator coveredObj = objsCovered.begin(); coveredObj != objsCovered.end(); ++coveredObj){
                G.erase(find(G.begin(), G.end(), (*coveredObj))); // if current rule covers delete this object
                UniverseTemp.erase(find(UniverseTemp.begin(), UniverseTemp.end(), (*coveredObj)));
            }
            //cout << "\npositive examples still not covered: ";
            //this->printSet(G);
            //cout << endl;
        }
        //cout << "\n\n\ndone for t = " << t << "...checking minimality" << endl << endl;
        this->updateMinimalSet(rulesPLD, PLD->getMembers());
    }
};
///type 4 rules
void Domlem::findRules(map<int,P_UppDnwd*> Collection_PUD, map<int,P_BdryDnwd*> Collection_PBD, ObjSet_ptr Universe, int nClasses){
    cout << endl << "Inducing Type 4 Rules -> possible D<=" << endl;
    /*
    Syntax:
    if f(x,q1) <= rq1 and f(x,q2) <= rq2 and ... f(x,qp) <= rqp, then x possible belongs to Clt<=
    */
    for (int t = 1; t < nClasses; t++){ //begin from the most strong unions
        P_BdryDnwd* PBD = Collection_PBD[t];
        ObjSet_ptr bndry = PBD->getMembers();
        if( bndry.size() > 0 ){ // if there is inconsistency
            P_UppDnwd* PUD = Collection_PUD[t];
            ObjSet_ptr G = PUD->getMembers(); //set of objects from the given approximation
            //this->printSet(G);
            int nCriteria = ((*(G.begin()))->get_g()).size(); ///gambiarra: modifica isso...
            RuleSet_ptr rulesPUD; //initialize set of rules covering PLU
            int contt = 0;
            ObjSet_ptr UniverseTemp = Universe;
            while( G.size() > 0 ){ // do until all objects are covered
                Rule complex_E(4,t); //starting complex
                ObjSet_ptr S = G; //set of objects currently covered by E
                //cout << "\nobjs covered by complex_E = " << (complex_E.getObjsCovered(PLU->getMembers())).size() << endl;
                while( (complex_E.getCoveredObjs()).size() == 0 || !(this->isContainedInPositiveExamples(complex_E, PUD->getMembers())) ){
                    /*
                    ///Debugging
                    cout << "\n\nPress Enter to Continue\n\n";
                    cin.ignore();
                    */
                    double bestEval = 0; //best candidate for elementary condition
                    double bestnObj = 0; //cardinality of ( intersection(lEl, S) ), where lEl is the number of objects from S covered by E
                    double bestElem = 0;
                    int best_q = 0;
                    ObjSet_ptr bestCovered;
                    vector<double> cond; //f(x,q)
                    vector<int> cond_q; /// gambiarra (tirar dps): condition associated criteria
                    int condIdx = 0;
                    int condcounter = 0;
                    for(int q = 0; q < nCriteria; q++){ // for each criterion in P do...
                        for (ObjSet_ptr::iterator obj_in_S = S.begin(); obj_in_S != S.end(); ++obj_in_S){
                            map<int, double> g = (*obj_in_S)->get_g();
                            cond.push_back(g[q]); //for each positive object from S create an elementary condition
                            cond_q.push_back(q);
                        }
                    }
                    //delete conditions that are already contained in the complex_E elementary conditions
                    //Rule complex_E_2(1,true); //create false rule to get complex_E past conditions and reset coveredObjs...
                    map<int, double> elemConds = complex_E.getElemConds();
                    for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                        cond.erase(find(cond.begin(),cond.end(),it->second));
                        cond_q.erase(find(cond_q.begin(),cond_q.end(),it->first));
                        //complex_E_2.setElemCond(it->first, it->second);
                    }
                    vector<int>::iterator q = cond_q.begin();
                    for(vector<double>::iterator elem = cond.begin(); elem != cond.end(); ++elem){ //find the best condition
                        condcounter++;
                        //Rule tempcomplex_E = complex_E_2;// = complex_E;
                        Rule tempcomplex_E = complex_E;
                        tempcomplex_E.setElemCond(*q, *elem);
                        //cout << "\ncandidate cond e" << condcounter <<" f(x,q" << *q+1 << ") >= " << *elem;
                        //if( tempcomplex_E.getCoveredObjs().size() > 0 ) tempcomplex_E.resetCoveredObjs();
                        tempcomplex_E.setCoveredObjs(UniverseTemp, true);
                        //this->printSet(tempcomplex_E.getCoveredObjs());
                        double currentnObj = (this->getIntersection(tempcomplex_E, G)).size(); //get insersection cardinality
                        double eval_1 = currentnObj / (tempcomplex_E.getCoveredObjs()).size();
                        //cout << " -> eval_1: " << eval_1 << "/ eval_2: " << currentnObj;
                        if( eval_1 > bestEval ){
                            bestElem = *elem;
                            bestEval = eval_1;
                            bestnObj = currentnObj;
                            bestCovered = tempcomplex_E.getCoveredObjs();
                            best_q = *q;
                            condIdx = condcounter;
                        }
                        else{ //draw case currentEval/G.size() == bestEval
                            if( eval_1 == bestEval){
                                //cout << "\ndraw between: e" << condcounter << " and e" << condIdx << " (best)";
                                if(currentnObj > bestnObj){
                                    bestElem = *elem;
                                    bestEval = eval_1;
                                    bestnObj = currentnObj;
                                    bestCovered = tempcomplex_E.getCoveredObjs();
                                    best_q = *q;
                                    condIdx = condcounter;
                                }
                            }
                        }
                        ++q;
                    }
                    complex_E.setElemCond(best_q, bestElem); //add the best condition to the complex
                    complex_E.setEvals(bestEval, bestnObj);
                    complex_E.setCoveredObjs(bestCovered, false); //add covered objects
                    //complex_E.print();
                    S = this->getIntersection(complex_E, S); // intersection(S,best)
                    /* ///for debugging
                    if(!(this->isContainedInPositiveExamples(complex_E, PLU->getMembers()))){
                        cout << "\nlEl is not contained in B\n";
                    }
                    else{cout << "\nlEl is contained in B\n";}
                    */
                }
                //complex_E.print();
                //create a rule on the basis of E and eliminate superfluous elementary conditions
                ObjSet_ptr objsCovered = complex_E.getCoveredObjs();
                map<int, double> elemConds = complex_E.getElemConds();
                Rule complex_E_2(4,t,true); //create false rule and reset covered objs (its important when more than one elem condition is used)
                for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                    complex_E_2.setElemCond(it->first, it->second);
                }
                for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                    Rule tempE = complex_E_2;
                    tempE.delElemCond(it->first);
                    tempE.setCoveredObjs(UniverseTemp, true);
                    if(this->isContainedInPositiveExamples(tempE, PUD->getMembers())){
                        complex_E = tempE;
                    }
                }
                objsCovered = complex_E.getCoveredObjs();
                //complex_E.print();
                Rule* final_E = new Rule(4,t,true); //create induced rule
                for(map<int, double>::iterator it = elemConds.begin(); it != elemConds.end(); it++){
                    final_E->setElemCond(it->first, it->second);
                }
                final_E->setCoveredObjs(PUD->getMembers(), true);
                double currentnObj = (this->getIntersection(*final_E, PUD->getMembers())).size(); //get insersection cardinality
                final_E->setEvals(currentnObj / (final_E->getCoveredObjs()).size(), currentnObj);
                //final_E->print();
                rulesPUD.push_back(final_E); //add the induced rule
                // remove examples (from G and Universe) covered by the rule
                for (ObjSet_ptr::iterator coveredObj = objsCovered.begin(); coveredObj != objsCovered.end(); ++coveredObj){
                    G.erase(find(G.begin(), G.end(), (*coveredObj))); // if current rule covers delete this object
                    UniverseTemp.erase(find(UniverseTemp.begin(), UniverseTemp.end(), (*coveredObj)));
                }
                //cout << "\npositive examples still not covered: ";
                //this->printSet(G);
                //cout << endl;
            }
            //cout << "\n\n\ndone for t = " << t << "...checking minimality" << endl << endl;
            this->updateMinimalSet(rulesPUD, PUD->getMembers());
        }
    }
};

///type 5 - approximate rules
void Domlem::findRules(map<int,P_UppUpwd*> Collection_PUU, map<int,P_UppDnwd*> Collection_PUD, ObjSet_ptr Universe, int nClasses){
    cout << endl << "Inducing approximate Rules -> D=><=" << endl;
    /*
    Syntax:
    if f(x,q1) >= rq1 and ... and f(x,qm) >= rqm and ... and f(x,qp) <= rqp, and f(x,qz) <= rqz then x belongs to Clt or Clt-1 or ...
    */
    /*
    Example: Upper Boundaries bndry(Clt>=)
    for n = nClasses : 2
        Kn = P_upper(Cl>=n)
        do{
            Kn = intersection(K, P_upper(Cl<=s)), where s < n-1
        }until there is no more intersection -> K.size = 0
        if (K is not empty){
            find lower and upper edges
            induct rules
        }
    end
    */
    /// from Cln -> Cl2 intersection approximations
    for (int t = nClasses; t > 2; t--){ //begin from the most strong unions
        ///in this loop, t needs to be greater than two, because t>s and s > 1, then in the extreme case: t=3 > s=2 >1. s>1 because it cannot assume Cl>=1 (covers all the objects of the universe)
        ObjSet_ptr currentK = (Collection_PUU[t])->getMembers();//auxiliar for K
        ObjSet_ptr K; // contains accumulated intersections of inconsistent objects
        int currents = t - 1;
        int s; // index of the lower class of the union
        while ( currents > 1 && currentK.size() > 0 ){
            currentK = this->getIntersection(currentK, (Collection_PUD[currents])->getMembers());
            if (currentK.size() > 0){ //there is intersection
                K = currentK; //save this intersection
                s = currents; //save the index of the lower class of the union
            }
            currents--;
        }
        if( K.size() > 0){
            ObjSet_ptr G = K; //set of objects from the given approximation
            this->getEdges(G);//find lower and upper edges
            int nCriteria = this->nCriteria;
            RuleSet_ptr rulesK; //initialize set of rules covering K
            int contt = 0;
            ///you need identify the addresses of lower and upper edge associated elements in universetemp
            ObjSet_ptr UniverseTemp = Universe;
            ObjSet_ptr eraseFromLE;  ObjSet_ptr addLE;
            ObjSet_ptr eraseFromUE;  ObjSet_ptr addUE;
            for(ObjSet_ptr::iterator thisObj = this->lowerEdge.begin(); thisObj != this->lowerEdge.end(); thisObj++){
                eraseFromLE.push_back(*thisObj);
                for(ObjSet_ptr::iterator thisObjU = UniverseTemp.begin(); thisObjU != UniverseTemp.end(); thisObjU++){
                    if(((*thisObjU)->get_Name()).compare((*thisObj)->get_Name()) == 0){
                        //cout << endl << (*thisObjU)->get_Name() <<  " is lower edge"; ///debug
                        addLE.push_back(*thisObjU);
                    }
                }
            }
            for(ObjSet_ptr::iterator thisObj = this->upperEdge.begin(); thisObj != this->upperEdge.end(); thisObj++){
                eraseFromUE.push_back(*thisObj);
                for(ObjSet_ptr::iterator thisObjU = UniverseTemp.begin(); thisObjU != UniverseTemp.end(); thisObjU++){
                    if(((*thisObjU)->get_Name()).compare((*thisObj)->get_Name()) == 0){
                        //cout << endl << (*thisObjU)->get_Name() <<  " is upper edge"; ///debug
                        addUE.push_back(*thisObjU);
                    }
                }
            }
            ///erase elements created after calling getEdges()
            for(ObjSet_ptr::iterator thisObj = eraseFromLE.begin(); thisObj != eraseFromLE.end(); thisObj++){
                this->lowerEdge.erase(find(this->lowerEdge.begin(),this->lowerEdge.end(),*thisObj));
            }
            for(ObjSet_ptr::iterator thisObj = eraseFromUE.begin(); thisObj != eraseFromUE.end(); thisObj++){
                this->upperEdge.erase(find(this->upperEdge.begin(),this->upperEdge.end(),*thisObj));
            }
            ///add elements from univesetemp identified as lower and upper edges
            for(ObjSet_ptr::iterator thisObj = addLE.begin(); thisObj != addLE.end(); thisObj++){
                this->lowerEdge.push_back(*thisObj);
            }
            for(ObjSet_ptr::iterator thisObj = addUE.begin(); thisObj != addUE.end(); thisObj++){
                this->upperEdge.push_back(*thisObj);
            }
            while( G.size() > 0 ){ // do until all objects are covered
                Rule complex_E(5,t,s); //starting complex
                ObjSet_ptr S = G; //set of objects currently covered by E
                ObjSet_ptr lowS = this->lowerEdge;
                ObjSet_ptr upS = this->upperEdge;
                if( lowS.size() == 0 ){ lowS = upS; } //we need to do this or we would never have isContainedInPositiveExamples(complex_E, K) = TRUE
                if( upS.size() == 0 ){ upS = lowS; } //we need to do this or we would never have isContainedInPositiveExamples(complex_E, K) = TRUE
                //cout << "\nobjs covered by complex_E = " << (complex_E.getObjsCovered(PLU->getMembers())).size() << endl;
                while( (complex_E.getCoveredObjs()).size() == 0 || !(this->isContainedInPositiveExamples(complex_E, K)) ){
                    /*
                    ///Debugging
                    cout << "\n\nPress Enter to Continue\n\n";
                    cin.ignore();
                    */
                    double bestEval = 0; //best candidate for elementary condition
                    double bestnObj = 0; //cardinality of ( intersection(lEl, S) ), where lEl is the number of objects from S covered by E
                    double bestElem = 0;
                    int best_q = 0;
                    ObjSet_ptr bestCovered;
                    bool best_isLE;
                    vector<double> cond; //f(x,q)
                    vector<int> cond_q; /// gambiarra (tirar dps): condition associated criteria index
                    vector<bool> isLowerEdge; ///gambiarra (tirar dps): condition associated type of edge
                    int condIdx = 0;
                    int condcounter = 0;
                    map<int, double> Low_elemConds = complex_E.getLowElemConds();
                    map<int, double> Up_elemConds = complex_E.getUpElemConds();
                    map<int, double>::iterator condE;
                    for(int q = 0; q < nCriteria; q++){ // for each criterion in P do...
                        //get lower edge conditions
                        bool complexE_initiated = false;
                        if(Low_elemConds.find(q) != Low_elemConds.end()){ complexE_initiated = true; }
                        for (ObjSet_ptr::iterator obj_in_LE = lowS.begin(); obj_in_LE != lowS.end(); ++obj_in_LE){
                            bool feasibleCond = true;
                            map<int, double> g = (*obj_in_LE)->get_g();
                            if(complexE_initiated && g[q] == Low_elemConds.find(q)->second){ feasibleCond = false;}
                            if(feasibleCond){
                                cond.push_back(g[q]); //for each positive object from S create an elementary condition
                                cond_q.push_back(q);
                                isLowerEdge.push_back(true);
                            }
                        }
                        //get upper edge conditions
                        complexE_initiated = false;
                        if(Up_elemConds.find(q) != Up_elemConds.end()){ complexE_initiated = true; }
                        for (ObjSet_ptr::iterator obj_in_UE = upS.begin(); obj_in_UE != upS.end(); ++obj_in_UE){
                            bool feasibleCond = true;
                            map<int, double> g = (*obj_in_UE)->get_g();
                            if(complexE_initiated && g[q] == Up_elemConds.find(q)->second){ feasibleCond = false;}
                            if(feasibleCond){
                                cond.push_back(g[q]); //for each positive object from S create an elementary condition
                                cond_q.push_back(q);
                                isLowerEdge.push_back(false);
                            }
                        }
                    }
                    vector<int>::iterator q = cond_q.begin();
                    vector<bool>::iterator isLE = isLowerEdge.begin();
                    for(vector<double>::iterator elem = cond.begin(); elem != cond.end(); ++elem){ //find the best condition
                        condcounter++;
                        //Rule tempcomplex_E = complex_E_2;// = complex_E;
                        Rule tempcomplex_E = complex_E;
                        if(*isLE){
                            tempcomplex_E.setLowElemCond(*q, *elem);
                            //cout << "\ncandidate cond e" << condcounter <<" f(x,q" << *q+1 << ") >= " << *elem; ///debug
                        } //is associated with lower edge obsj
                        else{
                            tempcomplex_E.setUpElemCond(*q, *elem);
                            //cout << "\ncandidate cond e" << condcounter <<" f(x,q" << *q+1 << ") <= " << *elem; ///debug
                        } //is associated with upper edge obsj
                        //if( tempcomplex_E.getCoveredObjs().size() > 0 ) tempcomplex_E.resetCoveredObjs();
                        tempcomplex_E.setCoveredObjs(UniverseTemp, true);
                        //this->printSet(tempcomplex_E.getCoveredObjs()); ///debug
                        double currentnObj = (this->getIntersection(tempcomplex_E, G)).size(); //get insersection cardinality
                        double eval_1 = currentnObj / (tempcomplex_E.getCoveredObjs()).size();
                        //cout << " -> eval_1: " << eval_1 << "/ eval_2: " << currentnObj; ///debug
                        if( eval_1 > bestEval ){
                            bestElem = *elem;
                            bestEval = eval_1;
                            bestnObj = currentnObj;
                            bestCovered = tempcomplex_E.getCoveredObjs();
                            best_q = *q;
                            best_isLE = *isLE;
                            condIdx = condcounter;
                        }
                        else{ //draw case currentEval/G.size() == bestEval
                            if( eval_1 == bestEval){
                                //cout << "\ndraw between: e" << condcounter << " and e" << condIdx << " (best)";
                                if(currentnObj > bestnObj){
                                    bestElem = *elem;
                                    bestEval = eval_1;
                                    bestnObj = currentnObj;
                                    bestCovered = tempcomplex_E.getCoveredObjs();
                                    best_q = *q;
                                    best_isLE = *isLE;
                                    condIdx = condcounter;
                                }
                            }
                        }
                        ++q;
                        ++isLE;
                    }
                    //add the best condition to the complex
                    if(best_isLE){ complex_E.setLowElemCond(best_q, bestElem); } //lower edge object associated
                    else{ complex_E.setUpElemCond(best_q, bestElem); } //upper edge object associated
                    complex_E.setEvals(bestEval, bestnObj);
                    complex_E.setCoveredObjs(bestCovered, false); //add covered objects
                    //complex_E.print(); ///debug
                    lowS = this->getIntersection(complex_E, lowS);
                    upS = this->getIntersection(complex_E, upS);
                    /* ///for debugging
                    if(!(this->isContainedInPositiveExamples(complex_E, PLU->getMembers()))){
                        cout << "\nlEl is not contained in B\n";
                    }
                    else{cout << "\nlEl is contained in B\n";}
                    */
                    //cout << "\nsizeE: " << (complex_E.getCoveredObjs()).size(); ///debug
                }
                //cout << "ok";
                //complex_E.print();
                //create a rule on the basis of E and eliminate superfluous elementary conditions
                ObjSet_ptr objsCovered = complex_E.getCoveredObjs();
                map<int, double> Low_elemConds = complex_E.getLowElemConds();
                map<int, double> Up_elemConds = complex_E.getUpElemConds();
                Rule complex_E_2(5,t,s,true); //create false rule and reset covered objs (its important when more than one elem condition is used)
                for(map<int, double>::iterator it = Low_elemConds.begin(); it != Low_elemConds.end(); it++){
                    complex_E_2.setLowElemCond(it->first, it->second);
                }
                for(map<int, double>::iterator it = Up_elemConds.begin(); it != Up_elemConds.end(); it++){
                    complex_E_2.setUpElemCond(it->first, it->second);
                }
                for(map<int, double>::iterator it = Low_elemConds.begin(); it != Low_elemConds.end(); it++){
                    Rule tempE = complex_E_2;
                    tempE.delLowElemCond(it->first);
                    tempE.setCoveredObjs(UniverseTemp, true);
                    if(this->isContainedInPositiveExamples(tempE, K)){
                        complex_E = tempE;
                    }
                }
                for(map<int, double>::iterator it = Up_elemConds.begin(); it != Up_elemConds.end(); it++){
                    Rule tempE = complex_E_2;
                    tempE.delUpElemCond(it->first);
                    tempE.setCoveredObjs(UniverseTemp, true);
                    if(this->isContainedInPositiveExamples(tempE, K)){
                        complex_E = tempE;
                    }
                }
                objsCovered = complex_E.getCoveredObjs();
                //complex_E.print();
                Rule* final_E = new Rule(5,t,s,true); //create induced rule
                //Low_elemConds = complex_E.getLowElemConds();
                //Up_elemConds = complex_E.getUpElemConds();
                for(map<int, double>::iterator it = Low_elemConds.begin(); it != Low_elemConds.end(); it++){
                    final_E->setLowElemCond(it->first, it->second);
                }
                for(map<int, double>::iterator it = Up_elemConds.begin(); it != Up_elemConds.end(); it++){
                    final_E->setUpElemCond(it->first, it->second);
                }
                final_E->setCoveredObjs(K, true);
                double currentnObj = (this->getIntersection(*final_E, K)).size(); //get insersection cardinality
                final_E->setEvals(currentnObj / (final_E->getCoveredObjs()).size(), currentnObj);
                //final_E->print();
                rulesK.push_back(final_E); //add the induced rule
                // remove examples (from G and Universe) covered by the rule
                for (ObjSet_ptr::iterator coveredObj = objsCovered.begin(); coveredObj != objsCovered.end(); ++coveredObj){
                    G.erase(find(G.begin(), G.end(), (*coveredObj))); // if current rule covers delete this object
                    UniverseTemp.erase(find(UniverseTemp.begin(), UniverseTemp.end(), (*coveredObj)));
                    ObjSet_ptr::iterator Itlow = find(this->lowerEdge.begin(),this->lowerEdge.end(), (*coveredObj));
                    if(Itlow != this->lowerEdge.end()){ this->lowerEdge.erase(Itlow);}
                    ObjSet_ptr::iterator Itup = find(this->upperEdge.begin(),this->upperEdge.end(), (*coveredObj));
                    if(Itup != this->upperEdge.end()){ this->upperEdge.erase(Itup);}
                }
                //cout << "\npositive examples still not covered: ";
                //this->printSet(G);
                //cout << endl;
            }
            //cout << "\n\n\ndone for t = " << t << "...checking minimality" << endl << endl;
            this->updateMinimalSet(rulesK, K);
        }
        //erase lower edge
        while(this->lowerEdge.size()>0){ this->lowerEdge.erase(this->lowerEdge.begin());}
        while(this->upperEdge.size()>0){ this->upperEdge.erase(this->upperEdge.begin());}
    }
    /// from Cl1 -> Cln-1 intersection approximations
    for (int s = 1; s < nClasses-1; s++){ //begin from the most strong unions
        ///in this loop, t needs to be greater than two, because t>s and s > 1, then in the extreme case: t=3 > s=2 >1. s>1 because it cannot assume Cl>=1 (covers all the objects of the universe)
        ObjSet_ptr currentK = (Collection_PUD[s])->getMembers();//auxiliar for K
        ObjSet_ptr K; // contains accumulated intersections of inconsistent objects
        int currentt = s + 1;
        int t; // index of the lower class of the union
        while ( currentt < nClasses && currentK.size() > 0 ){
            currentK = this->getIntersection(currentK, (Collection_PUU[currentt])->getMembers());
            if (currentK.size() > 0){ //there is intersection
                K = currentK; //save this intersection
                t = currentt; //save the index of the lower class of the union
            }
            currentt++;
        }
        if( K.size() > 0){
            ObjSet_ptr G = K; //set of objects from the given approximation
            this->getEdges(G);//find lower and upper edges
            int nCriteria = this->nCriteria;
            RuleSet_ptr rulesK; //initialize set of rules covering K
            int contt = 0;
            ///you need identify the addresses of lower and upper edge associated elements in universetemp
            ObjSet_ptr UniverseTemp = Universe;
            ObjSet_ptr eraseFromLE;  ObjSet_ptr addLE;
            ObjSet_ptr eraseFromUE;  ObjSet_ptr addUE;
            for(ObjSet_ptr::iterator thisObj = this->lowerEdge.begin(); thisObj != this->lowerEdge.end(); thisObj++){
                eraseFromLE.push_back(*thisObj);
                for(ObjSet_ptr::iterator thisObjU = UniverseTemp.begin(); thisObjU != UniverseTemp.end(); thisObjU++){
                    if(((*thisObjU)->get_Name()).compare((*thisObj)->get_Name()) == 0){
                        //cout << endl << (*thisObjU)->get_Name() <<  " is lower edge"; ///debug
                        addLE.push_back(*thisObjU);
                    }
                }
            }
            for(ObjSet_ptr::iterator thisObj = this->upperEdge.begin(); thisObj != this->upperEdge.end(); thisObj++){
                eraseFromUE.push_back(*thisObj);
                for(ObjSet_ptr::iterator thisObjU = UniverseTemp.begin(); thisObjU != UniverseTemp.end(); thisObjU++){
                    if(((*thisObjU)->get_Name()).compare((*thisObj)->get_Name()) == 0){
                        //cout << endl << (*thisObjU)->get_Name() <<  " is upper edge"; ///debug
                        addUE.push_back(*thisObjU);
                    }
                }
            }
            ///erase elements created after calling getEdges()
            for(ObjSet_ptr::iterator thisObj = eraseFromLE.begin(); thisObj != eraseFromLE.end(); thisObj++){
                this->lowerEdge.erase(find(this->lowerEdge.begin(),this->lowerEdge.end(),*thisObj));
            }
            for(ObjSet_ptr::iterator thisObj = eraseFromUE.begin(); thisObj != eraseFromUE.end(); thisObj++){
                this->upperEdge.erase(find(this->upperEdge.begin(),this->upperEdge.end(),*thisObj));
            }
            ///add elements from univesetemp identified as lower and upper edges
            for(ObjSet_ptr::iterator thisObj = addLE.begin(); thisObj != addLE.end(); thisObj++){
                this->lowerEdge.push_back(*thisObj);
            }
            for(ObjSet_ptr::iterator thisObj = addUE.begin(); thisObj != addUE.end(); thisObj++){
                this->upperEdge.push_back(*thisObj);
            }
            while( G.size() > 0 ){ // do until all objects are covered
                Rule complex_E(5,t,s); //starting complex
                ObjSet_ptr S = G; //set of objects currently covered by E
                ObjSet_ptr lowS = this->lowerEdge;
                ObjSet_ptr upS = this->upperEdge;
                if( lowS.size() == 0 ){ lowS = upS; } //we need to do this or we would never have isContainedInPositiveExamples(complex_E, K) = TRUE
                if( upS.size() == 0 ){ upS = lowS; } //we need to do this or we would never have isContainedInPositiveExamples(complex_E, K) = TRUE
                //cout << "\nobjs covered by complex_E = " << (complex_E.getObjsCovered(PLU->getMembers())).size() << endl;
                while( (complex_E.getCoveredObjs()).size() == 0 || !(this->isContainedInPositiveExamples(complex_E, K)) ){
                    /*
                    ///Debugging
                    cout << "\n\nPress Enter to Continue\n\n";
                    cin.ignore();
                    */
                    double bestEval = 0; //best candidate for elementary condition
                    double bestnObj = 0; //cardinality of ( intersection(lEl, S) ), where lEl is the number of objects from S covered by E
                    double bestElem = 0;
                    int best_q = 0;
                    ObjSet_ptr bestCovered;
                    bool best_isLE;
                    vector<double> cond; //f(x,q)
                    vector<int> cond_q; /// gambiarra (tirar dps): condition associated criteria index
                    vector<bool> isLowerEdge; ///gambiarra (tirar dps): condition associated type of edge
                    int condIdx = 0;
                    int condcounter = 0;
                    map<int, double> Low_elemConds = complex_E.getLowElemConds();
                    map<int, double> Up_elemConds = complex_E.getUpElemConds();
                    map<int, double>::iterator condE;
                    for(int q = 0; q < nCriteria; q++){ // for each criterion in P do...
                        //get lower edge conditions
                        bool complexE_initiated = false;
                        if(Low_elemConds.find(q) != Low_elemConds.end()){ complexE_initiated = true; }
                        for (ObjSet_ptr::iterator obj_in_LE = lowS.begin(); obj_in_LE != lowS.end(); ++obj_in_LE){
                            bool feasibleCond = true;
                            map<int, double> g = (*obj_in_LE)->get_g();
                            if(complexE_initiated && g[q] == Low_elemConds.find(q)->second){ feasibleCond = false;}
                            if(feasibleCond){
                                cond.push_back(g[q]); //for each positive object from S create an elementary condition
                                cond_q.push_back(q);
                                isLowerEdge.push_back(true);
                            }
                        }
                        //get upper edge conditions
                        complexE_initiated = false;
                        if(Up_elemConds.find(q) != Up_elemConds.end()){ complexE_initiated = true; }
                        for (ObjSet_ptr::iterator obj_in_UE = upS.begin(); obj_in_UE != upS.end(); ++obj_in_UE){
                            bool feasibleCond = true;
                            map<int, double> g = (*obj_in_UE)->get_g();
                            if(complexE_initiated && g[q] == Up_elemConds.find(q)->second){ feasibleCond = false;}
                            if(feasibleCond){
                                cond.push_back(g[q]); //for each positive object from S create an elementary condition
                                cond_q.push_back(q);
                                isLowerEdge.push_back(false);
                            }
                        }
                    }
                    vector<int>::iterator q = cond_q.begin();
                    vector<bool>::iterator isLE = isLowerEdge.begin();
                    for(vector<double>::iterator elem = cond.begin(); elem != cond.end(); ++elem){ //find the best condition
                        condcounter++;
                        //Rule tempcomplex_E = complex_E_2;// = complex_E;
                        Rule tempcomplex_E = complex_E;
                        if(*isLE){
                            tempcomplex_E.setLowElemCond(*q, *elem);
                            //cout << "\ncandidate cond e" << condcounter <<" f(x,q" << *q+1 << ") >= " << *elem; ///debug
                        } //is associated with lower edge obsj
                        else{
                            tempcomplex_E.setUpElemCond(*q, *elem);
                            //cout << "\ncandidate cond e" << condcounter <<" f(x,q" << *q+1 << ") <= " << *elem; ///debug
                        } //is associated with upper edge obsj
                        //if( tempcomplex_E.getCoveredObjs().size() > 0 ) tempcomplex_E.resetCoveredObjs();
                        tempcomplex_E.setCoveredObjs(UniverseTemp, true);
                        //this->printSet(tempcomplex_E.getCoveredObjs()); ///debug
                        double currentnObj = (this->getIntersection(tempcomplex_E, G)).size(); //get insersection cardinality
                        double eval_1 = currentnObj / (tempcomplex_E.getCoveredObjs()).size();
                        //cout << " -> eval_1: " << eval_1 << "/ eval_2: " << currentnObj; ///debug
                        if( eval_1 > bestEval ){
                            bestElem = *elem;
                            bestEval = eval_1;
                            bestnObj = currentnObj;
                            bestCovered = tempcomplex_E.getCoveredObjs();
                            best_q = *q;
                            best_isLE = *isLE;
                            condIdx = condcounter;
                        }
                        else{ //draw case currentEval/G.size() == bestEval
                            if( eval_1 == bestEval){
                                //cout << "\ndraw between: e" << condcounter << " and e" << condIdx << " (best)";
                                if(currentnObj > bestnObj){
                                    bestElem = *elem;
                                    bestEval = eval_1;
                                    bestnObj = currentnObj;
                                    bestCovered = tempcomplex_E.getCoveredObjs();
                                    best_q = *q;
                                    best_isLE = *isLE;
                                    condIdx = condcounter;
                                }
                            }
                        }
                        ++q;
                        ++isLE;
                    }
                    //add the best condition to the complex
                    if(best_isLE){ complex_E.setLowElemCond(best_q, bestElem); } //lower edge object associated
                    else{ complex_E.setUpElemCond(best_q, bestElem); } //upper edge object associated
                    complex_E.setEvals(bestEval, bestnObj);
                    complex_E.setCoveredObjs(bestCovered, false); //add covered objects
                    //complex_E.print(); ///debug
                    lowS = this->getIntersection(complex_E, lowS);
                    upS = this->getIntersection(complex_E, upS);
                    /* ///for debugging
                    if(!(this->isContainedInPositiveExamples(complex_E, PLU->getMembers()))){
                        cout << "\nlEl is not contained in B\n";
                    }
                    else{cout << "\nlEl is contained in B\n";}
                    */
                    //cout << "\nsizeE: " << (complex_E.getCoveredObjs()).size(); ///debug
                }
                //cout << "ok";
                //complex_E.print();
                //create a rule on the basis of E and eliminate superfluous elementary conditions
                ObjSet_ptr objsCovered = complex_E.getCoveredObjs();
                map<int, double> Low_elemConds = complex_E.getLowElemConds();
                map<int, double> Up_elemConds = complex_E.getUpElemConds();
                Rule complex_E_2(5,t,s,true); //create false rule and reset covered objs (its important when more than one elem condition is used)
                for(map<int, double>::iterator it = Low_elemConds.begin(); it != Low_elemConds.end(); it++){
                    complex_E_2.setLowElemCond(it->first, it->second);
                }
                for(map<int, double>::iterator it = Up_elemConds.begin(); it != Up_elemConds.end(); it++){
                    complex_E_2.setUpElemCond(it->first, it->second);
                }
                for(map<int, double>::iterator it = Low_elemConds.begin(); it != Low_elemConds.end(); it++){
                    Rule tempE = complex_E_2;
                    tempE.delLowElemCond(it->first);
                    tempE.setCoveredObjs(UniverseTemp, true);
                    if(this->isContainedInPositiveExamples(tempE, K)){
                        complex_E = tempE;
                    }
                }
                for(map<int, double>::iterator it = Up_elemConds.begin(); it != Up_elemConds.end(); it++){
                    Rule tempE = complex_E_2;
                    tempE.delUpElemCond(it->first);
                    tempE.setCoveredObjs(UniverseTemp, true);
                    if(this->isContainedInPositiveExamples(tempE, K)){
                        complex_E = tempE;
                    }
                }
                objsCovered = complex_E.getCoveredObjs();
                //complex_E.print();
                Rule* final_E = new Rule(5,t,s,true); //create induced rule
                //Low_elemConds = complex_E.getLowElemConds();
                //Up_elemConds = complex_E.getUpElemConds();
                for(map<int, double>::iterator it = Low_elemConds.begin(); it != Low_elemConds.end(); it++){
                    final_E->setLowElemCond(it->first, it->second);
                }
                for(map<int, double>::iterator it = Up_elemConds.begin(); it != Up_elemConds.end(); it++){
                    final_E->setUpElemCond(it->first, it->second);
                }
                final_E->setCoveredObjs(K, true);
                double currentnObj = (this->getIntersection(*final_E, K)).size(); //get insersection cardinality
                final_E->setEvals(currentnObj / (final_E->getCoveredObjs()).size(), currentnObj);
                //final_E->print();
                rulesK.push_back(final_E); //add the induced rule
                // remove examples (from G and Universe) covered by the rule
                for (ObjSet_ptr::iterator coveredObj = objsCovered.begin(); coveredObj != objsCovered.end(); ++coveredObj){
                    G.erase(find(G.begin(), G.end(), (*coveredObj))); // if current rule covers delete this object
                    UniverseTemp.erase(find(UniverseTemp.begin(), UniverseTemp.end(), (*coveredObj)));
                    ObjSet_ptr::iterator Itlow = find(this->lowerEdge.begin(),this->lowerEdge.end(), (*coveredObj));
                    if(Itlow != this->lowerEdge.end()){ this->lowerEdge.erase(Itlow);}
                    ObjSet_ptr::iterator Itup = find(this->upperEdge.begin(),this->upperEdge.end(), (*coveredObj));
                    if(Itup != this->upperEdge.end()){ this->upperEdge.erase(Itup);}
                }
                //cout << "\npositive examples still not covered: ";
                //this->printSet(G);
                //cout << endl;
            }
            //cout << "\n\n\ndone for t = " << t << "...checking minimality" << endl << endl;
            this->updateMinimalSet(rulesK, K);
        }
        //erase lower edge
        while(this->lowerEdge.size()>0){ this->lowerEdge.erase(this->lowerEdge.begin());}
        while(this->upperEdge.size()>0){ this->upperEdge.erase(this->upperEdge.begin());}
    }
};

ObjSet_ptr Domlem::getIntersection(Rule testRule, ObjSet_ptr Union){
    ObjSet_ptr coveredByE = testRule.getCoveredObjs();
    ObjSet_ptr intersec;
    for (ObjSet_ptr::iterator thisObj = Union.begin(); thisObj != Union.end(); ++thisObj){
        if( find(coveredByE.begin(),coveredByE.end(),*thisObj) != coveredByE.end() ){
            intersec.push_back(*thisObj); // thisObj belongs to the intersection
        }
    }
    return intersec;
}

bool Domlem::isContainedInPositiveExamples(Rule testRule, ObjSet_ptr PositiveExamples){
    if((testRule.getCoveredObjs()).size() == 0){
            //cout << "\ncovers the Universe...\n";
            return false;
    } //if the complex is equal to empty set, then it covers all Universe, which is not contained in B
    ObjSet_ptr coveredByE = testRule.getCoveredObjs();
    for (ObjSet_ptr::iterator thisObj= coveredByE.begin(); thisObj != coveredByE.end(); ++thisObj){
        if( find(PositiveExamples.begin(),PositiveExamples.end(),*thisObj) == PositiveExamples.end() ){
            return false; // an element is not contained in positive examples -> coveredByE is not contained in Positive Examples
        }
    }
    return true; //all objs covered by testRule belong to the positive example set
}

ObjSet_ptr Domlem::getIntersection(ObjSet_ptr K, ObjSet_ptr Union){
    ObjSet_ptr intersec;
    for (ObjSet_ptr::iterator thisObj = Union.begin(); thisObj != Union.end(); ++thisObj){
        if( find(K.begin(),K.end(),*thisObj) != K.end() ){
            intersec.push_back(*thisObj); // thisObj belongs to the intersection
        }
    }
    return intersec;
}

void Domlem::getEdges(ObjSet_ptr K){
    ObjSet_ptr Kobjs;
    /// create K objects (reset granules of knowledge)
    for (ObjSet_ptr::iterator currentObj = K.begin(); currentObj != K.end(); ++currentObj){ //loop over all objects of the universe
        Kobjs.push_back(new Obj(*currentObj));
    }
    /// get granules of knowledge
    ObjSet_ptr remainingK = Kobjs;
    for (ObjSet_ptr::iterator currentObj = Kobjs.begin(); currentObj != Kobjs.end(); ++currentObj){ //loop over all objects of the universe
        (*currentObj)->update_GranulesOfKnowledge(remainingK, this->prefDir);
        remainingK.erase(remainingK.begin()); //erase the first element in order to reduce algorithm complexity
    }
    /// get lower and upper edges
    for (ObjSet_ptr::iterator currentObj = Kobjs.begin(); currentObj != Kobjs.end(); ++currentObj){ //loop over all objects of the universe
        //check if currentObj belongs to the lower edge
        if (((*currentObj)->get_PdominatedSet()).size() == 1){
            this->lowerEdge.push_back(*currentObj);
        }
        //check if currentObj belongs to the upper edge
        if (((*currentObj)->get_PdominatingSet()).size() == 1){
            this->upperEdge.push_back(*currentObj);
        }
        //vector<Obj*> get_PdominatedSet();
    }
}

void Domlem::printSet(ObjSet_ptr thisSet){
    cout << " {";
    for(ObjSet_ptr::iterator thisObj = thisSet.begin(); thisObj != thisSet.end(); thisObj++){
        cout << (*thisObj)->get_Name() <<" "; //name
        ///cout << (*thisObj)->get_Name() <<"(" << *thisObj << ") "; ///debug (name + address)
    }
    cout << "} ";
}

void Domlem::classify(ObjSet_ptr Universe){ /// use this to test if all objects were reallocated to their original classes
    for (ObjSet_ptr::iterator thisObj = Universe.begin(); thisObj != Universe.end(); ++thisObj){
        int currentClass = 0; //class which this object belongs to after classifying it with the rules induced by Domlem
        vector<int> currentApproxClass; /// current Approximate Classes
        int t,s; /// range of classes (approximate rules)
        for (RuleSet_ptr::iterator currentRule = this->inducedRules.begin(); currentRule != this->inducedRules.end(); ++currentRule){
            if((*currentRule)->checkIfCovers(*thisObj)){
                switch((*currentRule)->get_ruletype()){
                    case 1: /// D>= Rules (Certain + Possible)
                    case 2:
                        if( currentApproxClass.size() == 0 ){ /// if not covered by any approximate rule
                            if( currentClass == 0 ){ currentClass = (*currentRule)->getDecision(); } //initialize currentClass
                            else{
                                if( (*currentRule)->getDecision() > currentClass ){ currentClass = (*currentRule)->getDecision(); }
                            }
                        }
                        break;
                    case 3: /// D<= Rules (Certain + Possible)
                    case 4:
                        if( currentApproxClass.size() == 0 ){ /// if not covered by any approximate rule
                            if( currentClass == 0 ){ currentClass = (*currentRule)->getDecision(); } //initialize currentClass
                            else{
                                if( (*currentRule)->getDecision() < currentClass ){ currentClass = (*currentRule)->getDecision(); }
                            }
                        }
                        break;
                    case 5: /// D=><= approximate Rules
                        currentClass = 0; ///now this object cannot be covered by certain rules
                        if( currentApproxClass.size() > 0){
                                currentApproxClass = (*currentRule)->getDecision(true);
                                t = currentApproxClass[0];
                                s = currentApproxClass[1];
                        } //initialize currentClass vector
                        else{
                            currentApproxClass = (*currentRule)->getDecision(true);
                            t = currentApproxClass[0];
                            s = currentApproxClass[1];
                            if( currentApproxClass[0] > t){ t = currentApproxClass[0]; }
                            if( currentApproxClass[1] < s){ s = currentApproxClass[1]; }
                        }
                }
            }
        }
        if(currentClass != 0){ //was not classified by approximate rules
            cout << endl << (*thisObj)->get_Name() << "-> DomlemClss: Cl" << currentClass << " | OriginalClss: Cl" << (*thisObj)->get_Class();
            if ( currentClass == (*thisObj)->get_Class() ){ cout << " (OK!)";}
        }
        else{
            cout << endl << (*thisObj)->get_Name() << "-> DomlemClss: ";
            for(int c = s; c <= t; c++){
                    cout << "Cl" << c;
                    if(c < t){ cout << "U"; }
            }
            cout << " | OriginalClss: Cl" << (*thisObj)->get_Class();
            if ( s <= (*thisObj)->get_Class() && t >= (*thisObj)->get_Class() ){ cout << " (approximately OK!)";}
        }
    }
    cout << endl << endl;
}

//dtor
Domlem::~Domlem(){}
