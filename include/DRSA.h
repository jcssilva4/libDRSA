#ifndef DRSA_H
#define DRSA_H

#include "RuleInductionAlgorithm.h"

class DRSA
{
    public:
        DRSA(int, int, int*, int, double**);
        virtual ~DRSA();
        ObjSet_ptr get_upClt(int); // compute upward union of classes s>=t
        ObjSet_ptr get_downClt(int);
        void showAprxSets(int t);
        void find_rules(string);
    protected:

    private:
        int nObj;
        int nCriteria;
        int nClasses;
        map<int, int> prefDir; //criteria monotonic restrictions directions (preference direction - max=1,min=-1)
        ObjSet_ptr Universe;
        // Universe Partitions
        map<int,ObjSet_ptr> Clt;
        // approximations of unions of classes
        map<int,P_LowUpwd*> PLU; // P-lower approximation of upward union of classes (Clt = {x | x belongs to Cls, s>=t})
        map<int,P_UppUpwd*> PUU; // P-upper approximation of upward union of classes (Clt = {x | x could belong to Cls, s>=t})
        map<int,P_LowDnwd*> PLD; // P-lower approximation of downward union of classes (Clt = {x | x belong to Cls, s<=t})
        map<int,P_UppDnwd*> PUD; // P-upper approximation of downward union of classes (Clt = {x | x could belong to Cls, s<=t})
        //rough sets
        map<int,P_BdryUpwd*> PBU;// boundary  (Clt = {x | x could belong to Cls, s>=t})
        map<int,P_BdryDnwd*> PBD; // boundary (Clt = {x | x could belong to Cls, s<=t})
        // rule induction
        RuleInductionAlgorithm* rule_algo;
};

#endif // DRSA_H
