/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
//This is the exp branch of the project
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"
#include <string>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;


void parse_Grammar();
void parse_Rule_list();
vector<string> parse_Ids(vector<string> vector1);
void parse_Rule();
vector<string> parseRHS(vector<string> v1);
void syntax_error();
void addTerminals(string element_to_check1);
void addNonTerminals(string element_to_check1);

Token element_check(TokenType expected_element_type);
LexicalAnalyzer lexer;
vector<string> terminals;
vector<string> non_terms;
vector<string> all_elements;
vector<string> universe;
vector<bool> generating_table;
vector<string> reachable_symbols;

//part2
vector<vector<string>> rules; //all the rules of the grammar
struct rule {
    string LHS;
    vector<string> RHS;
};
vector<rule> rules_struct;
//vector<vector<string>> genRules;
vector<rule> genRules;
vector<rule>final_genRules;

//part3
vector<vector<string>> first_sets;
vector<string> first_sets_index;

//part4
vector<vector<string>> follow_sets;
vector<string> follow_sets_index;

// read grammar
void ReadGrammar()
{
    parse_Grammar();
    element_check(END_OF_FILE);

    universe.push_back("#");
    universe.push_back("$");
    for (auto i: terminals)
    {
        if(!(find(non_terms.begin(), non_terms.end(), i) != non_terms.end()))  { //adding all terminals not present in non_terms in universe
            universe.push_back(i);
        }

    }
    for (auto i: all_elements)
    {
        if (find(non_terms.begin(), non_terms.end(), i) != non_terms.end()) { //adding common items in all_terms and non_terms in universe
            universe.push_back(i);
        }
    }

}

void parse_Grammar()
{
    parse_Rule_list(); //parsing
    element_check(HASH);  //end of rules
}

void parse_Rule_list()
{
    parse_Rule();
    Token temp = lexer.peek(1);
    if (temp.token_type == HASH)
    {
        return;
    }
    else if (temp.token_type == ID)
    {
        parse_Rule_list();  //recursive call
        return;
    }
    else
    {
        syntax_error();
    }

}
void parse_Rule()
{
    vector<string> rule;//create vector for rule(each line LHS->RHS)
    //LHS are non-terminals
    struct rule r;
    Token temp = element_check(ID);
    //rule.push_back(temp.lexeme); //add LHS to rule
    r.LHS = temp.lexeme;
    addNonTerminals(temp.lexeme); //add LHS to non_terminals

    element_check(ARROW); //end of LHS, begin of RHS


    r.RHS = parseRHS(rule); //add RHS to rule
   // rules.push_back(rule); //add rule to rules
    rules_struct.push_back(r); //add rule to rules

    element_check(STAR); //end of rule
}

Token element_check(TokenType expected_element_type)
{
    Token temp = lexer.GetToken(); //get token
    if (temp.token_type != expected_element_type) //check if token is expected type
        syntax_error(); //if not, throw error
    return temp; //return token
}

vector<string> parseRHS(vector<string> v1) //parsing RHS
{
    Token temp = lexer.peek(1); //get next token
    if (temp.token_type == ID) //if token is ID
    {
        vector<string> t1 = parse_Ids(v1); //parse Id list
        return t1;
    }
    else if (temp.token_type == STAR)
    {
        v1.emplace_back("#"); //push_back
        return v1;
    }
    else
        syntax_error();
}

vector<string> parse_Ids(vector<string> v1)
{
    Token temp = element_check(ID);
    v1.push_back(temp.lexeme);
    addTerminals(temp.lexeme);

    temp = lexer.peek(1);
    if (temp.token_type == ID)
    {
        vector<string> t1 = parse_Ids(v1);
        return t1;
    }
    else if (temp.token_type == STAR)
    {
        return v1;
    }
    else
        syntax_error();
}



void syntax_error()
{
    cout << "SYNTAX ERROR !!!\n";
    exit(1);
}


void addTerminals(string element)
{
    if (!(find(all_elements.begin(), all_elements.end(), element) != all_elements.end())) { //if element is not in all_elements then add it to all_elements
        all_elements.push_back(element);
    }

    if (find(terminals.begin(), terminals.end(), element) != terminals.end()) {  //if element is in terminals then return
        return;
    }
    else {
        terminals.push_back(element); //else add it is terminal
    }
}

void addNonTerminals(string element)
{
    if (!count(all_elements.begin(), all_elements.end(), element)) {//if element is not in all_elements then add it to all_elements
        all_elements.push_back(element);
    }

    if (count(non_terms.begin(), non_terms.end(), element)) { //if element is in non_terms then return
        return;
    }
    else {
        non_terms.push_back(element); //else add it is non-terminal
    }
}

// Task 1
void printTerminalsAndNoneTerminals()
{

    for (auto i: terminals) //
    {
        if (!count(non_terms.begin(), non_terms.end(), i)) { //if i is not in non_terms then print it (i.e. i is a terminal)
            cout << i << ' ';
        }

    }


    for (auto i: all_elements)
    {
        if (count(non_terms.begin(), non_terms.end(), i)) { //if i is in non_terms then print it (i.e. i is a non-terminal)
            cout << i << ' ';
        }
    }
    cout << " ";
}

// Task 2
void RemoveUselessSymbols()
{
    generating_table.push_back(true); //first element is true
    generating_table.push_back(false);
    int index;
    //creating generating table
    for (auto i: universe)
    {
        if (i != "#" && i != "$" ) //if symbol in universe is not # and $ then
        {
            if (!count(non_terms.begin(), non_terms.end(), i)) { //all terminals are generating

                generating_table.push_back(true);
            }
            else
            {
                generating_table.push_back(false); //all non-terminals are not generating
            }
        }

    }


    bool continue1 = true;

    //iteration of generating tables
    while (continue1)
    {
        continue1 = false;
        for (auto rule: rules_struct) //for each rule
        {
            auto temp_rule = rule;
//            vector<string> temp_rule = rule;
//            rule.erase(rule.begin());
            rule.LHS.erase(); //remove LHS
            bool isgenerating = false;
            for (auto i : rule.RHS)  //for each symbol in RHS
            {
                auto itr = find(universe.begin(), universe.end(), i);  //find the first occurrence of i in universe
                index = distance(universe.begin(), itr); //get the index of i in universe
                if (!generating_table[index]) //if the index is not generating then set isgenerating to false
                {
                    isgenerating = false;  //if we dont know if the rule is generating or not then there is no change in generating table
                    break;
                }
                else
                {
                    isgenerating = true;
                }
            }
            if (isgenerating) //if isgenerating is true then set the index of LHS to true
            {
                auto itr = find(universe.begin(), universe.end(), temp_rule.LHS); //find the first occurrence of LHS in universe
                index = distance(universe.begin(), itr);
                if (!generating_table[index]) //if the index is not generating then set it to true
                {
                    generating_table[index] = true; //set the index of LHS to true
                    continue1 = true;  //continue the loop
                }
            }
        }
    }

    for (auto rule_: rules_struct) //removing rules with non generating symbols
    {
        //cout << rule_[0] << " ";
        bool isGen = true;
        //checking if LHS is generating
//        auto itr0 = find(universe.begin(), universe.end(), rule_.LHS);
//        index = distance(universe.begin(), itr0);
//        if (!generating_table[index]) //if the index is not generating then set isgenerating to false
//        {
//            isGen = false;
//            break;
//        }

        //checking if RHS is generating
        for (auto i: rule_.RHS)
        {
            auto itr1 = find(universe.begin(), universe.end(), i);
            index = distance(universe.begin(), itr1);
            if (!generating_table[index]) //if the index is not generating then set isgenerating to false
            {
                isGen = false;
                break;
            }
        }

        if (isGen) //if LHS and RHS are generating then add the rule to genRules
        {
            genRules.push_back(rule_);
        }

    }

    if (!genRules.empty())  //if genRules is not empty then add the first nonterminal to reachable_symbols
        reachable_symbols.push_back(non_terms[0]); //add the first nonterminal to reachable_symbols

    //creating reachable symbols vector
    continue1 = true;
    while (continue1)
    {
        continue1 = false;
        for (auto rule: genRules)
        {
            if (count(reachable_symbols.begin(), reachable_symbols.end(), rule.LHS)){ //if LHS is in reachable_symbols then add RHS to reachable_symbols

                for (auto i : rule.RHS)
                {
                    if (!count(reachable_symbols.begin(), reachable_symbols.end(), i))
                    {
                        reachable_symbols.push_back(i);
                        continue1 = true;
                    }
                }
            }
        }

    }


    //deleting rules with non reachable symbols
    bool final_gen=false;
    for (auto rule: genRules)
    {

//        for (auto i: rule.RHS)
//        {
//            if (!count(reachable_symbols.begin(), reachable_symbols.end(), i)) //if RHS is not in reachable_symbols then delete the rule
//            {
//                auto itr1 = find(genRules.begin(), genRules.end(), rule.RHS);
//                index = distance(genRules.begin(), itr1);
//                genRules[index].push_back("delete");
//                break;
//            }
//        }
        for(auto i: rule.RHS) {

            if (!count(reachable_symbols.begin(), reachable_symbols.end(),i)){  //if any of the RHS symbols is not reachable the set the flag to false otherwise true
                final_gen= false;
            }
            else{
                final_gen= true;
            }
        }
        if(final_gen== true){
            final_genRules.push_back(rule);
        }
    }




    //print the rules
    for(auto rule: final_genRules){
        cout << rule.LHS << " -> ";
        for(auto i: rule.RHS){
            cout << i << " ";
        }
        cout << "\n";
    }
//    for (auto rule: genRules)
//    {
//        if (!rule.empty())
//        {
//            string var = rule.back();
//            if (var != "delete")
//            {
//                vector<string> temp_rule = rule;
//                rule.erase(rule.begin());
//                cout << temp_rule[0]  << " -> ";
//                for (auto i: rule)
//                {
//                    cout << i << " ";
//                }
//                cout << "\n";
//            }
//        }
//
//    }

}

// Task 3
void CalculateFirstSets()
{
    vector<string> a_follow_set;
    first_sets_index.emplace_back("#");
    a_follow_set.emplace_back("#");
    first_sets.push_back(a_follow_set);
    a_follow_set.clear();

    //II first(a) = {a} for every terminal a
    for (auto i: terminals) //for every terminal
    {
        if (!count(non_terms.begin(), non_terms.end(), i)) //if i is not in non_terms i.e. it is a terminal
        {
            first_sets_index.push_back(i);    //add i to first_sets_index
            a_follow_set.push_back(i);
            first_sets.push_back(a_follow_set); //add a_follow_set to first_sets
            a_follow_set.clear();
        }

    }
    //initialize first sets for non_terms
    for (auto i: all_elements)
    {
        if (count(non_terms.begin(), non_terms.end(), i)) {
            first_sets_index.push_back(i);
            first_sets.push_back(a_follow_set);
            a_follow_set.clear();
        }
    }

    bool change = true;
    while (change)
    {
        change = false;
        for (auto rule: rules_struct)
        {
            //get index of AB in first_sets
            auto itr = find(first_sets_index.begin(), first_sets_index.end(), rule.LHS); //find the index of LHS in first_sets_index
            int indexA = distance(first_sets_index.begin(), itr);

            auto itr1 = find(first_sets_index.begin(), first_sets_index.end(), rule.RHS[0]); //find the index of RHS[0] in first_sets_index
            int indexB = distance(first_sets_index.begin(), itr1);

            //III If A -> Bx is a grammar rule, where B is a terminal or nonterminal, then add FIRST(B) – { Ɛ } to FIRST(A)
            for (auto i: first_sets[indexB])
            {
                if (i != "#")
                {
                    if (!count(first_sets[indexA].begin(), first_sets[indexA].end(), i))
                    {
                        first_sets[indexA].push_back(i);
                        change = true;
                    }

                }
            }

            int count1 = 1;
            while (count1 > 0)
            {
                auto itr2 = find(first_sets_index.begin(), first_sets_index.end(), rule.RHS[count1-1]);
                string str = "#";
                int indexC = distance(first_sets_index.begin(), itr2);
                if (count(first_sets[indexC].begin(), first_sets[indexC].end(), str))
                {
                    //V
                    if (count1  == rule.RHS.size())
                    {
                        if (!count(first_sets[indexA].begin(), first_sets[indexA].end(), str))
                        {
                            first_sets[indexA].push_back("#");
                            change = true;
                        }
                        count1 = -1;
                    }
                        //IV
                    else
                    {
                        auto itr2 = find(first_sets_index.begin(), first_sets_index.end(), rule.RHS[count1]);
                        indexC = distance(first_sets_index.begin(), itr2);
                        for (auto i: first_sets[indexC])
                        {
                            if (i != "#")
                            {
                                if (!count(first_sets[indexA].begin(), first_sets[indexA].end(), i))
                                {
                                    first_sets[indexA].push_back(i);
                                    change = true;
                                }

                            }
                        }
                        count1++;

                    }
                }
                else
                {
                    count1 = -1;
                }
            }
        }
    }
}

// Task 4
void CalculateFollowSets()
{
    CalculateFirstSets();
    //I add $ to S
    vector<string> a_follow_set;
    follow_sets_index.push_back(non_terms[0]);
    a_follow_set.push_back("$");
    follow_sets.push_back(a_follow_set);
    a_follow_set.clear();

    //initailize all to empty
    for (auto i: all_elements)
    {
        if (count(non_terms.begin(), non_terms.end(), i)) {
            if (i != non_terms[0])
            {
                follow_sets_index.push_back(i);
                follow_sets.push_back(a_follow_set);
                a_follow_set.clear();
            }

        }
    }

    //apply IV and V to all rules
    for (auto rule: rules_struct)
    {
        if (rule.RHS.size() >= 2)
        {
            int index_count = 1;
            while (index_count>0)
            {
                vector<string>::iterator itr = find(follow_sets_index.begin(), follow_sets_index.end(), rule.RHS[index_count-1]);
                int indexA = distance(follow_sets_index.begin(), itr);


                if (index_count  == rule.RHS.size())
                {
                    vector<string>::iterator itr1 = find(first_sets_index.begin(), first_sets_index.end(), rule.RHS[index_count]);
                    int indexB = distance(first_sets_index.begin(), itr1);
                    //cout << indexB << "\n";
                    //cout << first_sets.size() << "\n";
                    if (indexB < first_sets.size())
                    {
                        for (auto i: first_sets[indexB])
                        {
                            if (i != "#")
                            {
                                if (!count(follow_sets[indexA].begin(), follow_sets[indexA].end(), i))
                                {
                                    follow_sets[indexA].push_back(i);
                                }

                            }
                        }
                    }

                    index_count = -1;
                }
                else
                {
                    vector<string>::iterator itr1 = find(first_sets_index.begin(), first_sets_index.end(), rule.RHS[index_count]);
                    int indexB = distance(first_sets_index.begin(), itr1);
                    for (auto i: first_sets[indexB])
                    {
                        if (i != "#")
                        {
                            if (indexA < follow_sets.size())
                            {
                                if (!count(follow_sets[indexA].begin(), follow_sets[indexA].end(), i))
                                {
                                    follow_sets[indexA].push_back(i);
                                }
                            }

                        }
                    }

                    index_count++;

                }
            }

            index_count = 1;
            while (index_count>0)
            {
                vector<string>::iterator itr2 = find(follow_sets_index.begin(), follow_sets_index.end(), rule.RHS[index_count-1]);
                int indexA = distance(follow_sets_index.begin(), itr2);
                int count2 = 1;
                while(count2 >0)
                {
                    if ((index_count + count2)-1 == rule.RHS.size())
                    {
                        count2 = -1;
                        break;
                    }
                    else
                    {
                        vector<string>::iterator itr2 = find(first_sets_index.begin(), first_sets_index.end(), rule.RHS[(index_count + count2)-1]);
                        int indexB = distance(first_sets_index.begin(), itr2);

                        if (count(first_sets[indexB].begin(), first_sets[indexB].end(), "#"))
                        {
                            vector<string>::iterator itr6 = find(first_sets_index.begin(), first_sets_index.end(), rule.RHS[index_count + count2 ]);
                            int indexC = distance(first_sets_index.begin(), itr6);
                            for (auto i: first_sets[indexC])
                            {
                                if (i != "#")
                                {
                                    if (indexA < follow_sets.size())
                                    {
                                        if (!count(follow_sets[indexA].begin(), follow_sets[indexA].end(), i))
                                        {
                                            follow_sets[indexA].push_back(i);
                                        }
                                    }

                                }
                            }
                            count2++;

                        }
                        else
                        {
                            count2 = -1;
                            break;
                        }

                    }
                }

                if (index_count >= rule.RHS.size())
                {
                    index_count = -1;
                    break;
                }
                else
                {
                    index_count++;
                }

            }
        }
    }

    //apply II iii
    bool change = true;
    while(change)
    {
        change = false;
        for (auto rule: rules_struct)
        {
            vector<string>::iterator itr2 = find(follow_sets_index.begin(), follow_sets_index.end(), rule.LHS);
            int indexA = distance(follow_sets_index.begin(), itr2);

            if (rule.RHS.size() > 0)
            {
                vector<string>::iterator itr2 = find(follow_sets_index.begin(), follow_sets_index.end(), rule.RHS.back());
                int indexB = distance(follow_sets_index.begin(), itr2);
                if (indexB < follow_sets.size())
                {
                    for (auto i: follow_sets[indexA])
                    {
                        if (!count(follow_sets[indexB].begin(), follow_sets[indexB].end(), i))
                        {
                            follow_sets[indexB].push_back(i);
                            change = true;
                        }
                    }
                }

                if (rule.RHS.size() > 1)
                {
                    int index_back = rule.RHS.size() ;
                    while (index_back > 0)
                    {
                        if (index_back > 1)
                        {
                            vector<string>::iterator itr7 = find(first_sets_index.begin(), first_sets_index.end(), rule.RHS[index_back-1]);
                            int indexC = distance(first_sets_index.begin(), itr7);
                            vector<string>::iterator itr8 = find(follow_sets_index.begin(), follow_sets_index.end(), rule.RHS[index_back-1]);
                            // int indexD = distance(follow_sets_index.begin(), itr8);
                            if (count(first_sets[indexC].begin(), first_sets[indexC].end(), "#"))
                            {
                                vector<string>::iterator itr9 = find(follow_sets_index.begin(), follow_sets_index.end(), rule.RHS[index_back -1]);
                                int indexE = distance(follow_sets_index.begin(), itr9);
                                if (indexE < follow_sets.size())
                                {
                                    for (auto i: follow_sets[indexA])
                                    {
                                        if (!count(follow_sets[indexE].begin(), follow_sets[indexE].end(), i))
                                        {
                                            follow_sets[indexE].push_back(i);
                                            change = true;
                                        }
                                    }
                                }
                                index_back--;
                            }
                            else
                            {
                                index_back = -1;
                                break;
                            }

                        }
                        else
                        {
                            index_back = -1;
                            break;
                        }
                    }
                }
            }
        }
    }
}


// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    for (auto i: non_terms)
    {
        vector<string>::iterator itr2 = find(first_sets_index.begin(), first_sets_index.end(), i);
        int indexA = distance(first_sets_index.begin(), itr2);

        vector<string>::iterator itr3 = find(follow_sets_index.begin(), follow_sets_index.end(), i);
        int indexB = distance(follow_sets_index.begin(), itr3);

        if (indexA < first_sets.size())
        {
            if (count(first_sets[indexA].begin(), first_sets[indexA].end(), "#"))
            {
                for (auto k: first_sets[indexA])
                {
                    for (auto m: follow_sets[indexB])
                    {
                        if (k == m)
                        {
                            cout << "NO\n";
                            return;
                        }
                    }
                }
            }
        }
        int numx = 0;
        for (auto rule1: rules)
        {
            numx++;
            if (rule1[0] == i)
            {
                //vector<string>::iterator itr2 = find(rules.begin(), rules.end(), rule1);
                vector<string> temp_rule1 = rule1;
                //int index1 = distance(rules.begin(), itr2);
                int numy = 0;
                for (auto rule2: rules)
                {
                    numy++;
                    if (rule2[0] == i)
                    {
                        vector<string> temp_rule2 = rule2;
                        //vector<string>::iterator itr3 = find(rules.begin(), rules.end(), rule2);
                        //int index2 = distance(rules.begin(), itr3);
                        if (numx != numy)
                        {
                            temp_rule1.erase(temp_rule1.begin());
                            temp_rule2.erase(temp_rule2.begin());
                            vector<string> test1;
                            for (auto xy: rule1)
                            {
                                vector<string>::iterator itr10 = find(first_sets_index.begin(), first_sets_index.end(), xy);
                                int indexq = distance(first_sets_index.begin(), itr10);
                                if (indexq < first_sets.size())
                                {
                                    for (auto hxy: first_sets[indexq])
                                    {
                                        if (!count(test1.begin(), test1.end(), hxy))
                                        {
                                            test1.push_back(hxy);
                                        }
                                    }
                                    if (!count(first_sets[indexq].begin(), first_sets[indexq].end(), "#"))
                                    {
                                        break;
                                    }
                                }
                            }
                            vector<string> test2;
                            for (auto xy: rule2)
                            {
                                vector<string>::iterator itr10 = find(first_sets_index.begin(), first_sets_index.end(), xy);
                                int indexq = distance(first_sets_index.begin(), itr10);
                                if (indexq < first_sets.size())
                                {
                                    for (auto hxy: first_sets[indexq])
                                    {
                                        if (!count(test2.begin(), test2.end(), hxy))
                                        {
                                            test2.push_back(hxy);
                                        }
                                    }
                                    if (!count(first_sets[indexq].begin(), first_sets[indexq].end(), "#"))
                                    {
                                        break;
                                    }
                                }

                            }
                            for (auto x: test1)
                            {
                                for(auto y: test2)
                                {
                                    if (x == y)
                                    {
                                        cout << "NO\n";
                                        return;
                                    }
                                }
                            }
                        }
                        else
                            break;
                    }
                    else
                        break;
                }
            }
            else
                break;
        }
    }
    cout << "YES\n";
}



int main (int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);

    //LexicalAnalyzer lexer;

    ReadGrammar();  // Reads the input grammar from standard input
    // and represent it internally in data structures
    // ad described in project 2 presentation file
    //cout << "Parser Success!\n";

    switch (task) {
        case 1: printTerminalsAndNoneTerminals();
            break;

        case 2: RemoveUselessSymbols();
            break;

        case 3: CalculateFirstSets();
            //print
            for (auto i: all_elements)
            {
                int num1 = 0;
                if (count(non_terms.begin(), non_terms.end(), i)) {
                    vector<string>::iterator itr4 = find(first_sets_index.begin(), first_sets_index.end(), i);
                    int indexD = distance(first_sets_index.begin(), itr4);
                    cout << "FIRST(" << i << ") = { ";
                    for (auto k: universe)
                    {
                        if (count(first_sets[indexD].begin(), first_sets[indexD].end(), k))
                        {
                            num1++;
                            //not the last one
                            if (num1 != first_sets[indexD].size())
                            {
                                cout << k << ", ";
                            }
                            else
                            {
                                cout << k << " ";
                                break;
                            }
                        }

                    }
                    cout << "}\n";
                }
            }

            break;

        case 4: CalculateFollowSets();
            //print
            for (auto i: all_elements)
            {
                int num1 = 0;
                if (count(non_terms.begin(), non_terms.end(), i)) {
                    vector<string>::iterator itr4 = find(follow_sets_index.begin(), follow_sets_index.end(), i);
                    int indexD = distance(follow_sets_index.begin(), itr4);
                    cout << "FOLLOW(" << i << ") = { ";
                    for (auto k: universe)
                    {
                        if (count(follow_sets[indexD].begin(), follow_sets[indexD].end(), k))
                        {
                            num1++;
                            //not the last one
                            if (num1 != follow_sets[indexD].size())
                            {
                                cout << k << ", ";
                            }
                            else
                            {
                                cout << k << " ";
                                break;
                            }
                        }

                    }
                    cout << "}\n";
                }
            }
            break;

        case 5: CheckIfGrammarHasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}

