#ifndef APPROXIMATION_H
#define APPROXIMATION_H

#include "Obj.h"

class Approximation
{
    public:
        Approximation(int, string);
        virtual ~Approximation();
        //virtual void checkMembership() = 0; //defines abstract class Approximation
        void const showMembers();
        ObjSet_ptr const getMembers();
        virtual void updateMembers(Obj*){};

    protected:
        string name;
        int t; //approximate class
        ObjSet_ptr members; //elements that belong to this set

    private:
};

///P-Lower approximation of Clt>=
class P_LowUpwd : public Approximation
{
    public:
        P_LowUpwd(int);
        virtual ~P_LowUpwd();
        void updateMembers(Obj*);

    protected:

    private:
};

///P-Upper approximation of Clt>=
class P_UppUpwd : public Approximation
{
    public:
        P_UppUpwd(int);
        virtual ~P_UppUpwd();
        void updateMembers(Obj*);

    protected:

    private:
};

///P-Boundary of Clt>=
class P_BdryUpwd : public Approximation
{
    public:
        P_BdryUpwd(int, P_UppUpwd*, P_LowUpwd*);
        virtual ~P_BdryUpwd();

    protected:

    private:
};

///P-Lower approximation of Clt>=
class P_LowDnwd : public Approximation
{
    public:
        P_LowDnwd(int);
        virtual ~P_LowDnwd();
        void updateMembers(Obj*);

    protected:

    private:
};

///P-Upper approximation of Clt<=
class P_UppDnwd : public Approximation
{
    public:
        P_UppDnwd(int);
        virtual ~P_UppDnwd();
        void updateMembers(Obj*);

    protected:

    private:
};

///P-Boundary of Clt<=
class P_BdryDnwd : public Approximation
{
    public:
        P_BdryDnwd(int, P_UppDnwd*, P_LowDnwd*);
        virtual ~P_BdryDnwd();

    protected:

    private:
};
#endif // APPROXIMATION_H
