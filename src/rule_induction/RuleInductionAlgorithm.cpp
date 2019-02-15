#include "RuleInductionAlgorithm.h"

RuleInductionAlgorithm::RuleInductionAlgorithm(string name, map<int,int> prefdir, int nclasses, int ncriteria)
{
    this->name = name;
    this->prefDir = prefdir;
    this->nClasses = nclasses;
    this->nCriteria = ncriteria;
}

void const RuleInductionAlgorithm::showName(){
    cout << endl << " == Rule Induction Algorithm : " << this->name << " ==" << endl << endl;;
}

void const RuleInductionAlgorithm::showRules(){
    cout << "\n\n\n == Induced Rules == \n\n";
    cout << "algorithm: " << this->name << endl;
    int rulecounter = 0;
    int numCertainR = 0;
    int numPossR = 0;
    int numApproxR = 0;
    for (RuleSet_ptr::iterator currentRule = this->inducedRules.begin(); currentRule != this->inducedRules.end(); ++currentRule){
        (*currentRule)->print(++rulecounter);
        if( (*currentRule)->get_ruletype() == 1 || (*currentRule)->get_ruletype() == 3 ){ ++numCertainR;}
        if( (*currentRule)->get_ruletype() == 2 || (*currentRule)->get_ruletype() == 4 ){ ++numPossR;}
        if( (*currentRule)->get_ruletype() == 5){ ++numApproxR;}
        //(*currentRule)->printName();
    }
    if(numCertainR > 0){ cout << "\nNUMBER of CERTAIN Rules: " << numCertainR;}
    if(numPossR > 0){ cout << "\nNUMBER of POSSIBLE Rules: " << numPossR;}
    if(numApproxR > 0){ cout << "\nNUMBER of APPROXIMATE Rules: " << numApproxR;}
    cout << "\n\n";
}

void RuleInductionAlgorithm::updateMinimalSet(RuleSet_ptr ruleSet, ObjSet_ptr Union){
    RuleSet_ptr minimalRules; //set of minimal rules
    RuleSet_ptr testRules = ruleSet;
    for (RuleSet_ptr::iterator currentTestRule = testRules.begin(); currentTestRule != testRules.end(); ++currentTestRule){
        ObjSet_ptr G = Union; // objects not covered by the active rules
        // loop over active rules (currentTestRule is inactive with the purpose of checking its redundancy)
        for (RuleSet_ptr::iterator currentRule = ruleSet.begin(); currentRule != ruleSet.end(); ++currentRule){
            if((*currentTestRule)!=(*currentRule)){ //is active rule?
                ObjSet_ptr covered = (*currentRule)->getCoveredObjs();
                ObjSet_ptr Gcovered;
                for (ObjSet_ptr::iterator thisObj = G.begin(); thisObj != G.end(); ++thisObj){
                    if( find(covered.begin(),covered.end(),*thisObj) != covered.end() ){
                        Gcovered.push_back(*thisObj);
                    }
                }
                //cout << "covered" << (*thisObj)->get_Name();
                //update G (erase covered elements)
                for (ObjSet_ptr::iterator thisObj = Gcovered.begin(); thisObj != Gcovered.end(); ++thisObj){
                    G.erase(find(G.begin(),G.end(),*thisObj));
                }
            }
        }
        if(G.size() > 0) {
            minimalRules.push_back(*currentTestRule);
        } //this rule is not redundant
    }
    ///update set of inducedRules
    for (RuleSet_ptr::iterator currentRule = minimalRules.begin(); currentRule != minimalRules.end(); ++currentRule){
        //cout << "ok";
        this->inducedRules.push_back(*currentRule);
    }
}

//dtor
RuleInductionAlgorithm::~RuleInductionAlgorithm(){}
