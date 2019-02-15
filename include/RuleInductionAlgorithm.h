#ifndef RULEINDUCTIONALGORITHM_H
#define RULEINDUCTIONALGORITHM_H

#include "Rule.h"

class RuleInductionAlgorithm
{
    public:
        RuleInductionAlgorithm(string, map<int,int>, int, int);
        virtual ~RuleInductionAlgorithm();
        virtual void findRules(map<int,P_LowUpwd*>, ObjSet_ptr, int){}; //type 1 rules
        virtual void findRules(map<int,P_UppUpwd*>, map<int,P_BdryUpwd*>, ObjSet_ptr, int){}; //type 2 rules
        virtual void findRules(map<int,P_LowDnwd*>, ObjSet_ptr, int){}; //type 3 rules
        virtual void findRules(map<int,P_UppDnwd*>, map<int,P_BdryDnwd*>, ObjSet_ptr, int){}; //type 4 rules
        ///(if you want you can exchange approximate rules by possible rules (types 2 and 4))
        virtual void findRules(map<int,P_UppUpwd*>, map<int,P_UppDnwd*>, ObjSet_ptr, int){}; ///type 5, approximate rules
        virtual void updateMinimalSet(RuleSet_ptr, ObjSet_ptr);
        virtual void classify(ObjSet_ptr){};
        void const showRules();

    protected:
        string name;
        map<int,int> prefDir;
        int nClasses;
        int nCriteria;
        virtual void mainLoop(){};
        void const showName();
        RuleSet_ptr inducedRules;

    private:
};


class Domlem : public RuleInductionAlgorithm
{
    public:
        Domlem(map<int,int>, int, int);
        virtual ~Domlem();
        void findRules(map<int,P_LowUpwd*>, ObjSet_ptr, int); //type 1 rules
        void findRules(map<int,P_UppUpwd*>, map<int,P_BdryUpwd*>, ObjSet_ptr, int); //type 2 rules
        void findRules(map<int,P_LowDnwd*>, ObjSet_ptr, int); //type 3 rules
        void findRules(map<int,P_UppDnwd*>, map<int,P_BdryDnwd*>, ObjSet_ptr, int); //type 4 rules
        void findRules(map<int,P_UppUpwd*>, map<int,P_UppDnwd*>, ObjSet_ptr, int); ///type 5, approximate rules
        void classify(ObjSet_ptr);

    protected:

    private:
        void printSet(ObjSet_ptr);
        ObjSet_ptr getIntersection(Rule, ObjSet_ptr);
        ObjSet_ptr getIntersection(ObjSet_ptr K, ObjSet_ptr Union);
        bool isContainedInPositiveExamples(Rule, ObjSet_ptr);
        void getEdges(ObjSet_ptr); /// used to compute lower and upper edges
        ObjSet_ptr lowerEdge; /// set of objects from K that do not dominate any other object from K having different evaluation on considered criteria (card(P-dominated set) = 1 -> only dominates itself)
        ObjSet_ptr upperEdge; /// set of objects from K that are not dominated by any other object from K having different evaluation on considered criteria (card(P-dominating set) = 1 -> only dominated by itself)
};

#endif // RULEINDUCTIONALGORITHM_H
