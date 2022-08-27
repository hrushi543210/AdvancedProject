#include <iostream>
#include <map>
#include <string>
#include <stack>
#include <fstream>
using namespace std;

//Mapping operands to replace and remember.
map<string, string> mymap;
map<string, string> pcomp_rules;

//Array of replacement characters. Will substitute this for ASCII chars so that I dont have to maintain a list.
char mychar[] = { '1', '2', '3', '4', '5', '6' ,'7','8','9', '!', '@', '#', '$', '%', '^', '&', '*'};
int i = 0;

//Struct for storing Grammar Rules.
struct myGrammarRules{
    int rule_type = 0;
    string left = "";
    string right_first = "";
    int right_second = 0;
};


//Creating a stack to store Grammar Rule objects during compression.
stack<myGrammarRules> rule_stack;


//P.Comp. compression (Single iteration).
string doPComp( string inputstr ) {

    string c = "";
    //Assigning 0's and 1's randomly to characters of string to split and form pairs.
    map<string, int> pcompmap;
    for(int j = 0; j<inputstr.length(); j++){
        c = inputstr[j];
        if(pcompmap.find(c) != pcompmap.end()){
            continue;
        }
        else{
            pcompmap[c] = rand() % 2;
        }
    }

    c="";
    //Calculating Pair frequencies.
    map<string, int> pcompmap_pair;
    for(int j = 0; j<inputstr.length()-1; j++){
        c = inputstr[j]+inputstr[j+1];
        if(pcompmap_pair.find(c) != pcompmap_pair.end()){
            pcompmap_pair[c]++;
            continue;
        }
        else{
            pcompmap_pair[c] = 1;
        }
    }

    c="";
    string d = "";
    //Replacing 01 pairs with new symbols and noting down the rules.
    for(int j=0; j<inputstr.length()-1; j++){
        c = inputstr[j];
        d = inputstr[j+1];
        if(pcompmap[c]==0 && pcompmap[d]==1){
            string x = inputstr.substr(0,j) + mychar[i] + inputstr.substr(j+2);
            inputstr = x;
            myGrammarRules temp_rule;
            temp_rule.rule_type = 1;
            temp_rule.left = c+d;
            temp_rule.right_first = mychar[i];
            temp_rule.right_second = -1;
            rule_stack.push(temp_rule);
            pcomp_rules[c+d] = mychar[i];
            i++;
        }
    }

    return inputstr;


}


//B.Comp. compression (Single iteration).
string doBComp( string inputstr ) {

    //Look for blocks of consecutive same characters and identify the block length.
    string temp = "";
    int start = 0;
    int end = 0;

    //Parsing till end of string.
    while(start<inputstr.length()){
        //While the encountered character is same, keep parsing.
        while(inputstr[start]==inputstr[end]){
            end++;
        }

        //If only one character block, then leave it unchanged and move on.
        if(end == start+1){
            temp+=inputstr[start];
            start++;
            end++;
            continue;
        }

        //Get the block string.
        string mymapkey = inputstr.substr(start, end-start);

        //If it is last block, then use appropriate rule to compress or create a new rule if block is new.
        if(end == inputstr.length()){
            if(mymap.find(mymapkey)!= mymap.end()){
                temp+= mymap[mymapkey];
                start = end;
            }
            else{
                myGrammarRules rule_temp;
                rule_temp.rule_type = 3;
                rule_temp.left = mymapkey;
                rule_temp.right_first = mychar[i];
                rule_temp.right_second = mymapkey.length();
                rule_stack.push(rule_temp);
                mymap[mymapkey] = mychar[i];
                i++;
                temp+=mymap[mymapkey];
                start = end;
            }
            break;
        }

        //Same as above but for blocks which are not the last one.
        if(mymap.find(mymapkey)!= mymap.end()){
            temp+= mymap[mymapkey];
            start = end;
            continue;
        }
        else{
            mymap[mymapkey] = mychar[i];
            myGrammarRules rule_temp;
            rule_temp.rule_type = 3;
            rule_temp.left = mymapkey;
            rule_temp.right_first = mychar[i];
            rule_temp.right_second = mymapkey.length();
            rule_stack.push(rule_temp);
            i++;
            temp+=mymap[mymapkey];
            start = end;
            continue;
        }
    }
    return temp;
}

int main() {

    // Create and open a text file
    ofstream MyFile("GrammarRules.txt");

    string inputstr;
    cout<<"Enter a String"<<endl;
    cin>>inputstr;
    cout<<"Input String: "<<inputstr<<endl;
    while(inputstr.length()>=2){
        inputstr = doBComp(inputstr);
        cout<<"After B.Comp. : "<<inputstr<<endl;
        inputstr = doPComp(inputstr);
        cout<<"After P.Comp. : "<<inputstr<<endl;
    }

    cout<<endl;
    cout<<"P.Comp. Rules: "<<endl;
    for(map<string, string>::const_iterator it = pcomp_rules.begin(); it != pcomp_rules.end(); ++it){
        cout << it->first << " " << it->second << "\n";
    }
    cout<<endl;
    cout<<endl;
    while(!rule_stack.empty()){
        myGrammarRules x = rule_stack.top();
        cout<<"Rule Type: "<<to_string(x.rule_type)<<endl;
        cout<<"Left operand: "<<x.left<<endl;
        cout<<"Right Operand (First): "<<x.right_first<<endl;
        cout<<"Right Operand (Second) (Only valid for B.Comp. Rules) : "<<to_string(x.right_second)<<endl;
        cout<<endl;

        MyFile<<"Rule Type: "<<to_string(x.rule_type)<<endl;
        MyFile<<"Left operand: "<<x.left<<endl;
        MyFile<<"Right Operand (First): "<<x.right_first<<endl;
        MyFile<<"Right Operand (Second) (Only valid for B.Comp. Rules) : "<<to_string(x.right_second)<<endl;
        MyFile<<endl;

        rule_stack.pop();
    }
    return 0;
}