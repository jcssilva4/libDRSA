#ifndef RULE_H
#define RULE_H

#include "Approximation.h"

//enum RuleType = {ruletype1 = 1, ruletype2 = 2, ruletype3 = 3, ruletype4 = 4};

class Rule
{
    public:
        Rule(int, int);
        Rule(int, int, bool);
        Rule(int, int, int);
        Rule(int, int, int, bool);
        virtual ~Rule();
        void const showRule();
        bool checkIfCovers(Obj*);
        void setElemCond(int, double);
        void setLowElemCond (int, double);
        void setUpElemCond (int, double);
        void setCoveredObjs(ObjSet_ptr, bool);
        void delElemCond(int);
        void delLowElemCond(int);
        void delUpElemCond(int);
        void setEvals(double, double);
        ObjSet_ptr getCoveredObjs();
        map <int, double> getElemConds();
        map <int, double> getLowElemConds();
        map <int, double> getUpElemConds();
        void printName();
        void print();
        void print(int);
        int get_ruletype();
        int getDecision();
        vector<int> getDecision(bool);

    protected:

    private:
        map <int, double> elementaryConditions; //<criterion_index, evaluation value>
        map <int, double> lower_elementaryConditions; //associated with lower edge // only used for approximate rules
        map <int, double> upper_elementaryConditions; //associated with lower edge // only used for approximate rules
        int Clt;
        vector<int> Clts; // set of classes (for approximate rules)
        int rule_type; ///1, 2, 3, 4, 5
        bool minimal;
        ObjSet_ptr covered; //objects covered by this rule
        ObjSet_ptr supported; //objects supported by this rule
        string name;
        static int nRules;
        double eval_measure1;
        double eval_measure2;
};

typedef vector<Rule*> RuleSet_ptr;

#endif // RULE_H
