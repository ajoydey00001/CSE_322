#include <bits/stdc++.h>
#include <windows.h>

using namespace std;

string convert_to_binary(int n)
{
    string s;
    s.resize(8);
    int i = 7;

    while (1)
    {
        /* code */
        if (n == 0)
        {
            while (i != -1)
            {
                /* code */
                s[i] = '0';
                i--;
            }
            return s;
        }

        if (i == -1)
        {
            return s;
        }
        s[i] = n % 2 + '0';
        n /= 2;
        i--;
    }
}

string div(string dividend, string divisor)
{

    int n = dividend.size();
    for (int i = 0; i < n; i++)
    {
        if (dividend[0] == '0')
        {
            dividend.erase(0, 1);
        }
        else
        {
            break;
        }
        if (dividend.size() == 0)
            break;
    }
    // cout << dividend << endl;
    if (dividend.size() < divisor.size())
    {
        string s;
        for (int i = 0; i < divisor.size(); i++)
        {
            s.push_back('0');
        }
        for (int i = 0; i < dividend.size(); i++)
        {
            s.erase(0, 1);
            s.push_back(dividend[i]);
        }

        return s;
    }

    string remainder;
    for (int i = 0; i < divisor.size(); i++)
    {
        remainder.push_back(dividend[i]);
    }

    int l = divisor.size();
    while (1)
    {
        string temp;
        int flag = 0;
        for (int i = 0; i < divisor.size(); i++)
        {
            if (remainder[i] == divisor[i])
            {
                if (flag)
                {
                    temp.push_back('0');
                }
            }
            else
            {
                flag = 1;
                temp.push_back('1');
            }
        }

        for (int j = temp.size(); j < divisor.size(); j++)
        {
            l++;
            if (dividend.size() < l)
            {
                string s;
                for (int i = 0; i < divisor.size(); i++)
                {
                    s.push_back('0');
                }
                for (int i = 0; i < temp.size(); i++)
                {
                    s.erase(0, 1);
                    s.push_back(temp[i]);
                }

                return s;
            }
            temp.push_back(dividend[l - 1]);
        }

        remainder = temp;
    }
}

int conver_string_to_int(string s){
    int collect=0, k=1;
    for(int j=s.length()-1;j>=0;j--){
        collect = collect + ((int)(s[j]-'0'))*k;
        k*=2;
    }
   
    return collect;
}

int main()
{
    
    cout << "enter data string: ";
    string data_string;
    getline(cin, data_string);
    cout << "enter number of data bytes in a row <m>: ";
    int m;
    double probability;
    string generator_poly;
    cin >> m;
    cout << "enter probability <p>: ";
    cin >> probability;
    cout << "enter generator polynomial: ";
    cin >> generator_poly;

    unsigned my_seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine Engine(my_seed);

   

    if (data_string.size() % m != 0)
    {
        int k = m - (data_string.size() % m);
        for (int i = 0; i < k; i++)
        {
            data_string.push_back('~');
        }
    }
    cout << endl<<endl<<endl;
    cout << "data string after padding: " << data_string << endl;

//////////////////////////////data block <ascii code of m characters per row>////////////////////////

    vector<string> arr_data_block;

    int index = 0, row, column;
    row = data_string.size() / m;
    arr_data_block.resize(row);

    for (int i = 0; i < row; i++)
    {
        arr_data_block[i] = "";
        for (int j = 0; j < m; j++)
        {
            string s = convert_to_binary(data_string[index]);
            //data_arr.push_back(s);
            arr_data_block[i] = arr_data_block[i] + s;
            index++;
        }
    }
    cout << endl;
    cout << "data block <ascii code of m characters per row>:" << endl;
    for (int i = 0; i < row; i++)
    {
        cout << arr_data_block[i] << endl;
    }

    ///////////////////////////data block after adding check bits////////////////////

    int iteration = 0, factor = 1;
    index = 0;
    while (1)
    {
        /* code */
        if (8 * m + 1 <= (factor - iteration))
        {
            break;
        }
        factor = factor * 2;
        iteration = iteration + 1;
    }

    factor = 1;

    column = m * 8 + iteration;
    for (int i = 0; i < row; i++)
    {
        factor = 1;
        int k1 = 0;
        string t1;
        t1.resize(m * 8 + iteration);
        for (int j = 0; j < t1.size(); j++)
        {
            if (j + 1 == factor)
            {
                factor *= 2;
            }
            else
            {
                t1[j] = arr_data_block[i][k1];
                k1++;
            }
        }
        factor = 1;

        for (int j = 1; j <= iteration; j++)
        {

            int sum = 0;

            for (int lk = factor - 1; lk < t1.size(); lk += (2 * factor))
            {
                for (int k22 = 0; k22 < factor && (k22 + lk) < t1.size(); k22++)
                {
                    if (lk + k22 != factor - 1)
                    {
                        if (t1[lk + k22] == '1')
                        {
                            sum++;
                        }
                    }
                }
            }
            if (sum % 2 == 1)
            {
                t1[factor - 1] = '1';
            }
            else
            {
                t1[factor - 1] = '0';
            }
            factor *= 2;
        }
        arr_data_block[i] = t1;
    }
    cout << endl;
    cout << "data block after adding check bits:" << endl;
    for (int i = 0; i < row; i++)
    {
        factor = 1;
        for (int j = 0; j < column; j++)
        {
            if (factor == j + 1)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                cout << arr_data_block[i][j];
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                factor *= 2;
            }
            else
            {
                cout << arr_data_block[i][j];
            }
        }
        cout << endl;
    }

    ////////////////////////data bits after column-wise serialization//////////////////////

    string column_wise_serialization;
    for (int i = 0; i < column; i++)
    {
        for (int j = 0; j < row; j++)
        {
            column_wise_serialization.push_back(arr_data_block[j][i]);
        }
    }

    cout << endl;
    cout << "data bits after column-wise serialization:" << endl;
    cout << column_wise_serialization << endl
         << endl;

//////////////////////data bits after appending CRC checksum <sent frame>////////////////////////

    string temp11 = column_wise_serialization;
    for (int i = 0; i < generator_poly.size() - 1; i++)
    {
        temp11.push_back('0');
    }
    string remainder = div(temp11, generator_poly);
    remainder.erase(0, 1);

    cout << "data bits after appending CRC checksum <sent frame>:" << endl;

    cout << column_wise_serialization;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    cout << remainder << endl
         << endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    column_wise_serialization += remainder;

////////////////////////////////result of CRC Checksum Matching/////////////////////////////////

    
    uniform_real_distribution<double> Distribution(0.0, 1.0);
    vector<double> arr_ran;
    arr_ran.resize(column_wise_serialization.size());

    for (int j = 0; j < column_wise_serialization.size(); j++)
    {
        arr_ran[j] = Distribution(Engine);
    }

    vector<int> track;  // to track which bit is error . track[i] = 0 for error
    track.resize(column_wise_serialization.size());
    cout << "received frame:" << endl;
    for (int i = 0; i < column_wise_serialization.size(); i++)
    {
        if (probability >= arr_ran[i])
        {

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
            if (column_wise_serialization[i] == '0')
            {
                column_wise_serialization[i] = '1';
            }
            else
            {
                column_wise_serialization[i] = '0';
            }
            track[i] = 0;
            cout << column_wise_serialization[i];
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        }
        else
        {
            cout << column_wise_serialization[i];
            track[i] = 1;
        }
    }
    cout << endl
         << endl;

    string is_error = div(column_wise_serialization, generator_poly);
    cout << "result of CRC checksum matching : ";
    int err_flag = 1;
    for (int i = 0; i < is_error.size(); i++)
    {
        if (is_error[i] == '1')
        {
            err_flag = 0;
            break;
        }
    }
    if (err_flag)
    {
        cout << "no error" << endl
             << endl;
    }
    else
    {
        cout << "error detected" << endl
             << endl;
    }

    ////////////////////////data block after removing CRC Checksum Bits/////////////////////////

    for (int i = 0; i < generator_poly.size() - 1; i++)
    {
        column_wise_serialization.pop_back();
    }
    vector<int> trace_2[row];
    vector<string> arr_receive_block;
    arr_receive_block.resize(row);
    int len = 0;
    for (int i = 0; i < column; i++)
    {
        for (int j = 0; j < row; j++)
        {
            arr_receive_block[j].push_back(column_wise_serialization[len]);

            trace_2[j].push_back(track[len]);
            len++;
        }
    }

    cout << "data block after removing CRC checksum bits:" << endl;
    for (int k = 0; k < row; k++)
    {
        for (int r = 0; r < column; r++)
        {
            if (trace_2[k][r] == 0) // error
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
                cout << arr_receive_block[k][r];
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            }
            else
            {
                cout << arr_receive_block[k][r];
            }
        }
        cout << endl;
    }
    cout << endl;

    ////////////////////////////////////data block after removing check bits///////////////////////////////

    for (int i = 0; i < row; i++)
    {
    
        factor = 1;
        int correct_index=0;

        for (int j = 1; j <= iteration; j++)
        {

            int sum = 0;

            for (int lk = factor - 1; lk < arr_receive_block[i].size(); lk += (2 * factor))
            {
                for (int k22 = 0; k22 < factor && (k22 + lk) < arr_receive_block[i].size(); k22++)
                {

                    if (arr_receive_block[i][lk + k22] == '1')
                    {
                        sum++;
                    }
                }
            }
            if (sum % 2 == 1)
            {
                correct_index +=factor;
            }
           
            factor *= 2;
        }
       
        if(correct_index != 0 && correct_index <=column){
           // cout<<"row : "<<i<<"  column : "<<correct_index<<endl;
            if(arr_receive_block[i][correct_index-1] == '1'){
                arr_receive_block[i][correct_index-1] = '0';
            }
            else{
                arr_receive_block[i][correct_index-1] = '1';
            }
        }

        factor=1;
        
        string yyy;
        for(int j=0;j<column;j++){
            if(j+1==factor){
                factor *=2;
            }
            else{
                yyy.push_back(arr_receive_block[i][j]);
            }
        }
        arr_receive_block[i]=yyy;


    }
    
    cout<<"data block after removing check bits:"<<endl;
    for(int i=0;i<row;i++){
        cout<<arr_receive_block[i]<<endl;
    }

///////////////////////////output frame////////////////////////////////////

    cout<<endl<<"output frame: ";
    column = arr_receive_block[0].size();
    string decode_str;
    for(int i=0;i<row;i++){

        
        for(int j=0;j<column;j++){
            decode_str.push_back(arr_receive_block[i][j]);
            if((j+1) %8 == 0){
                
                char c = conver_string_to_int(decode_str);
                cout<<c;
                decode_str.clear();
            }
            
        }
        decode_str.clear();

    }
    cout<<endl<<endl;





    return 0;
}