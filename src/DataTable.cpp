#include "DataTable.h"

DataTable::DataTable(string filename)
{
    ifstream inFile;
    inFile.open(filename);
    if (inFile.is_open()) {
        cout << "File has been opened" << endl;
        string btrash;

        ///get nobj, ncrit, nclss
        getline(inFile, btrash); // jump first #
        string e,m,n;
        getline ( inFile, e, ',' ); // get |U|
        getline ( inFile, m, ',' ); // get |C|
        getline ( inFile, n, '#' ); // get |D|
        this->nObj = stoi(e), this->nCriteria = stoi(m), this->nClasses = stoi(n); // initialize all
        //cout << "\n|U| -> " << this->nObj << endl;
        //cout << "\n|C| -> " << this->nCriteria << endl;
        //cout << "\n|D| -> " << this->nClasses << endl << endl;

        ///get preference dir
        getline(inFile, btrash); // jump second #
        //cout << endl << btrash << endl << endl << endl;
        this->prefDir = new int[this->nCriteria]; //allocate preference direction vector
        for (int q = 0; q < this->nCriteria; q++){ // loop over criteria
            char chr = '#';
            if(q < (this->nCriteria - 1)) chr = ',';
            getline ( inFile, btrash, chr );
            this->prefDir[q] = stoi(btrash);
            //cout <<this->prefDir[q];
        }

        ///get f
        getline(inFile, btrash); // jump third #
        //cout << endl << btrash << endl << endl << endl;
        int i = 0; //obj index
        this->f = new double*[nObj]; // initialize f
        for (i = 0; i < nObj; i++){
            this->f[i] = new double[nCriteria+1];
        }
        i = 0;
        while (i < nObj) {
            string val[nCriteria]; //f(x,q), where x belongs to U and q to Q = C +(union) D
            for (int q = 0; q < nCriteria+1; q++){ //loop over criteria
                char chr = '\n';
                if(q < nCriteria) chr = '\t'; // read until you find a comma
                getline ( inFile, val[q], chr ); // read until you find a newline character
                this->f[i][q] = stof(val[q]); // convert string to double
            }
            ++i;
        }
        // print UxQ
        for (int i = 0; i < nObj; i++){
            for (int q = 0; q < nCriteria+1; q++){
               //cout << "\t" << this->f[i][q];
            }
            //cout << endl;
        }
        inFile.close();
    }
    else {
        cout << "NO FILE HAS BEEN OPENED" << endl;
    }
    //system("pause");
}

int DataTable::get_nObj(){  return this->nObj;}

int DataTable::get_nCriteria(){ return this->nCriteria;}

int DataTable::get_nClasses(){  return this->nClasses;}

int* DataTable::get_prefDir(){  return this->prefDir;}

double** DataTable::get_f(){  return this->f;}

DataTable::~DataTable()
{
    cout << "\ndeleting raw data table content\n";
    for (int i = 0; i < nObj; ++i)
        delete [] this->f[i];
    delete [] this->f;

}
