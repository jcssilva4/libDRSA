/*
int main()
{
    // DataTable data("datafiles/datatable1.txt"); //get data table
    int i,n;
    int** p ;
    cout << "How many numbers would you like to type? ";
    cin >> n;
    p = new int*[n];
    for (int i = 0; i < n; i++){
        p[i] = new int[n];
    }

    for (i=0; i<n; i++){
        for(int j=0; j<n; j++){
            cout << "\nEnter number: ";
            p[i][j] = i;
            cout << p[i][j];
        }
    }
    cout << "\nYou have entered: ";
    for (i=0; i<n; i++){
        for(int j=0; j<n; j++){
            cout << p[i][j] << ", ";
        }
    }

    for (i=0; i<n; i++){
        delete[] p[i];
    }
    cout << "ok" << endl;
    delete[] p;
    cout << "ok" << endl;

    int* ptr1;
    int* ptr2;
    *(ptr1) = 1;
    ptr2 = ptr1;
    cout << ptr1 << endl; //memory addres
    cout << ptr2 << endl; // same mem address as ptr1
    cout << *ptr2 <<endl; // same value pointed by ptr1 (*ptr2 = ptr2[0])
    cout << ptr1[0] <<endl;
    return 0;
}
*/
