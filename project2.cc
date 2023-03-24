/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */

/*
Author:Om Shewale
Date: 3/24/2023
Course: CSE 340
Assignment: Project 2
Partner-Jeffrey Frederic
*/

#include <iostream>
#include <cstdlib>
#include "lexer.h"
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
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
bool foundInVector(vector<string> v, string element);

//part2
struct rule { // struct to store rules
    string LHS;
    vector<string> RHS;
};
vector<rule> rules_struct; // vector of rules
vector<rule> genRules;
vector<rule>final_genRules;

//part3
int FindIndexOfElementInFirstSetsIndices(string element);
vector<vector<string> > first_sets;
vector<string> indexOf_first_sets;

//part4
vector<vector<string>> follow_sets;
vector<string> indexOf_follow_sets;
bool AddToSetIfNotPresent(vector<string> &set, string element);

// read grammar
void ReadGrammar()
{
    parse_Grammar();
    element_check(END_OF_FILE);

    universe.emplace_back("#");
    universe.emplace_back("$");
    for(auto i : all_elements) {
        if(!foundInVector(non_terms, i)) { // adding all terminals not present in non_terms in universe
            if(foundInVector(terminals, i)) {
                universe.push_back(i);
            }
        } else { // adding common items in all_terms and non_terms in universe
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
    r.LHS = temp.lexeme;
    addNonTerminals(temp.lexeme); //add LHS to non_terminals

    element_check(ARROW); //end of LHS, begin of RHS


    r.RHS = parseRHS(rule); //add RHS to rule
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
    if (!foundInVector(all_elements,element)){ //if element is not in all_elements then add it to all_elements
        all_elements.push_back(element);
    }

    if (foundInVector(terminals,element)){//if element is in terminals then return
        return;
    }
    else {
        terminals.push_back(element); //else add it is terminal
    }
}

void addNonTerminals(string element)
{
    if (!foundInVector(all_elements,element)){//if element is not in all_elements then add it to all_elements
        all_elements.push_back(element);
    }

    if (foundInVector(non_terms,element)){ //if element is in non_terms then return
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
        if (!foundInVector(non_terms,i)){//if i is not in non_terms then print it (i.e. i is a terminal)
            cout << i << ' ';
        }

    }
    for (auto i: all_elements)
    {
        if (foundInVector(non_terms,i)) { //if i is in non_terms then print it (i.e. i is a non-terminal)
            cout << i << ' ';
        }
    }
    cout << " ";
}

// Task 2
bool foundInVector(vector<string> v, string element)  //function to check if element is in vector
{
    if(count(v.begin(), v.end(), element))
        return true;
    else
        return false;
}

int GetIndex(vector<string> v,string symbol){  //function to get index of symbol in vector
    auto itr = find(v.begin(), v.end(), symbol);
    int index = distance(v.begin(), itr);
    return index;
}

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
            if (!foundInVector(non_terms,i)) { //all terminals are generating
                generating_table.push_back(true);
            }
            else
            {
                generating_table.push_back(false); //all non-terminals are not generating
            }
        }

    }
    bool continue1 = true;
    while (continue1) {  //iteration of generating tables
        continue1 = false;
        for (auto rule : rules_struct) {
            bool is_generating = all_of(rule.RHS.begin(), rule.RHS.end(), [&](string i) { // get RHS of rule and check if all symbols are generating
                return generating_table[GetIndex(universe, i)];
            });
            if (is_generating && !generating_table[GetIndex(universe, rule.LHS)]) { //if all symbols in RHS are generating and LHS is not generating then make LHS generating
                generating_table[GetIndex(universe, rule.LHS)] = true;
                continue1 = true;
            }
        }
    }
    //creating vector of generating rules
    for (auto rule_ : rules_struct) {
        if ( all_of(rule_.RHS.begin(), rule_.RHS.end(), [&](string i) { //if all symbols in RHS are generating then add rule to genRules
            return generating_table[GetIndex(universe, i)];
        })) {
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
            if (foundInVector(reachable_symbols,rule.LHS)){ //if LHS is in reachable_symbols then add RHS to reachable_symbols

                for (auto i : rule.RHS)
                {
                    if (AddToSetIfNotPresent(reachable_symbols,i))
                    {
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
        for(auto i: rule.RHS) {

            if (!foundInVector(reachable_symbols,i)) //if any of the RHS symbols is not reachable the set the flag to false otherwise true
            {
                final_gen= false;
            }
            else{
                final_gen= true;
            }
        }

        if(final_gen== true and foundInVector(reachable_symbols,rule.LHS)){ //if the flag is true and LHS is reachable then add the rule to final_genRules
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

}

// Task 3
int FindIndexOfElementInFirstSetsIndices(string element) {
    return GetIndex(indexOf_first_sets, element);
}
int FindIndexOfElementInFollowSetsIndices(string element) {
    return GetIndex(indexOf_follow_sets, element);
}

bool AddToSetIfNotPresent(vector<string>& set, string element) {
    if (find(set.begin(), set.end(), element) == set.end()) {
        set.push_back(element);
        return true;
    }
    return false;
}
void CalculateFirstSets()
{
    indexOf_first_sets.emplace_back("#");
    vector<string> one_follow_set;
    one_follow_set.emplace_back("#");
    first_sets.push_back(one_follow_set);
    one_follow_set.clear();

//applying rule I and II to get the initial FIRST sets for terminals and epsilon
    for (auto element: terminals) //for every terminal
    {
        if (!(find(non_terms.begin(), non_terms.end(), element) != non_terms.end())) //if i is not in non_terms i.e. it is a terminal
        {
            indexOf_first_sets.push_back(element);    //add each terminal to indexOf_first_sets
            one_follow_set.push_back(element);
            first_sets.push_back(one_follow_set); //add each terminal to first_sets
            one_follow_set.clear();
        }

    }

    //initialize all FIRST sets for non-terminals to empty
    for (auto element: all_elements)
    {
        if (find(non_terms.begin(), non_terms.end(), element) != non_terms.end()) { //setting first sets of non terminals to zero
            indexOf_first_sets.push_back(element);
            first_sets.emplace_back(one_follow_set);
            one_follow_set.clear();
        }
    }
    bool change = true;
    while (change)
    {
        change = false;
        for (auto rule: rules_struct)  //travering through all rules
        {
            //index of A and B
            int indexOfA = FindIndexOfElementInFirstSetsIndices(rule.LHS);

            int indexOfB = FindIndexOfElementInFirstSetsIndices(rule.RHS[0]);

            //III If A -> B alpha is a grammar rule, where B is a terminal or nonterminal, then add FIRST(B) – { Ɛ } to FIRST(A)
            auto& setA = first_sets[indexOfA]; // reference to the vector at index indexOfA
            const auto& setB = first_sets[indexOfB]; // const reference to the vector at index indexOfB

            for (const auto& element : setB) { // iterating through elements of setB
                if (element != "#") {
                    auto itr0 = find(setA.begin(), setA.end(), element); // searching for the element in setA
                    if (itr0 == setA.end()) { // if element is not found in setA
                        setA.push_back(element); // adding element to setA
                        change = true;
                    }
                }
            }


            for (int i = 0; i < rule.RHS.size(); i++)
            {
                int indexC = FindIndexOfElementInFirstSetsIndices(rule.RHS[i]);
                bool contains_epsilon = count(first_sets[indexC].begin(), first_sets[indexC].end(), "#");

                // If RHS[i] contains epsilon, add all non-epsilon elements of FIRST(RHS[i+1]) to FIRST(A)
                if (contains_epsilon && i < rule.RHS.size() - 1)
                {
                    int indexNext = FindIndexOfElementInFirstSetsIndices(rule.RHS[i+1]);
                    for (auto j : first_sets[indexNext])
                    {
                        if (j != "#")
                        {
                            if (AddToSetIfNotPresent(first_sets[indexOfA], j))
                            {
                                change = true;
                            }
                        }
                    }
                }

                    // If RHS[i] does not contain epsilon, add its elements to FIRST(A) and exit
                else
                {
                    for (auto j : first_sets[indexC])
                    {
                        if (AddToSetIfNotPresent(first_sets[indexOfA], j))
                        {
                            change = true;
                        }
                    }
                    break;
                }
            }

            // If all symbols in RHS contain epsilon, add epsilon to FIRST(A)
            if (count(rule.RHS.begin(), rule.RHS.end(), "#") == rule.RHS.size() && !count(first_sets[indexOfA].begin(), first_sets[indexOfA].end(), "#"))
            {
                first_sets[indexOfA].push_back("#");
                change = true;
            }
        }
    }
}

void printFirstSets()
{
    for (auto i: all_elements)
    {
        int num1 = 0;
        if (foundInVector(non_terms,i)){
            int indexD = GetIndex(indexOf_first_sets,i);
            cout << "FIRST(" << i << ") = { ";
            for (auto k: universe)
            {
                if (foundInVector(first_sets[indexD], k))
                {
                    num1++;
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
}

// Task 4: Calculate Follow Sets
void CalculateFollowSets(){
    CalculateFirstSets();

    // Initialize all follow sets to empty except the first set which is for the start symbol
    follow_sets.resize(non_terms.size(), {});
    indexOf_follow_sets.resize(non_terms.size());
    indexOf_follow_sets[0] = non_terms[0]; // Add the start symbol to the first index of the follow sets indices

    // Add $ to the first set which is for the start symbol
    follow_sets[0].push_back("$");

    for (size_t i = 0; i < non_terms.size(); ++i) // Add all non terminals to the follow sets indices
    {
        indexOf_follow_sets[i] = non_terms[i];
    }

    bool change = true;
    while (change)  // Repeat until no change is made
    {
        change = false;

        for (const auto& rule : rules_struct)
        {
            for (size_t j = 0; j < rule.RHS.size(); ++j)
            {
                if (foundInVector(non_terms,rule.RHS[j]))  // If the symbol is a non terminal
                {
                    size_t indexA = GetIndex(non_terms,rule.RHS[j]);
                    size_t indexB = j + 1;
                    while (indexB < rule.RHS.size())
                    {
                        size_t indexC = GetIndex(indexOf_first_sets, rule.RHS[indexB]);

                        for (const auto& symbol : first_sets[indexC])
                        {
                            if (symbol != "#")
                            {
                                if (AddToSetIfNotPresent(follow_sets[indexA], symbol))
                                {
                                    change = true;
                                }
                            }
                            else
                            {
                                if (indexB == rule.RHS.size() - 1)
                                {
                                    size_t indexD = GetIndex(non_terms,rule.LHS);
                                    for (const auto& symbol : follow_sets[indexD])
                                    {
                                        if (AddToSetIfNotPresent(follow_sets[indexA], symbol))
                                        {
                                            change = true;
                                        }
                                    }
                                }
                            }
                        }

                        if (!count(first_sets[indexC].begin(), first_sets[indexC].end(), "#"))
                        {
                            break;
                        }

                        ++indexB;
                    }

                    if (indexB == rule.RHS.size())
                    {
                        size_t indexD = GetIndex(non_terms,rule.LHS);
                        for (const auto& symbol : follow_sets[indexD])
                        {
                            if (AddToSetIfNotPresent(follow_sets[indexA], symbol))
                            {
                                change = true;
                            }
                        }
                    }
                }
            }
        }
    }
    for (auto i: all_elements)
    {
        int num1 = 0;
        if (foundInVector(non_terms,i)) {
            int indexD = GetIndex(indexOf_follow_sets,i);//      distance(indexOf_follow_sets.begin(), itr0);
            cout << "FOLLOW(" << i << ") = { ";
            for (auto k: universe)
            {
                if (foundInVector(follow_sets[indexD],k))
                {
                    num1++;
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
}

bool IsNonTerminal(string symbol){
    if(find(non_terms.begin(), non_terms.end(), symbol) != non_terms.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

vector<string> s(vector<string> const &v, int m, int n) {
    auto first = v.begin() + m;
    auto last = v.begin() + n + 1;
    vector<string> vector(first, last);
    return vector;
}



// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    bool pr= false;
    vector<string> non_reachable_symbols;
    generating_table.push_back(true); //first element is true
    generating_table.push_back(false);
    int index;
    //creating generating table
    for (auto i: universe)
    {
        if (i != "#" && i != "$" ) //if symbol in universe is not # and $ then
        {
            if (!foundInVector(non_terms,i)) { //all terminals are generating
                generating_table.push_back(true);
            }
            else
            {
                generating_table.push_back(false); //all non-terminals are not generating
            }
        }

    }
    bool continue1 = true;
    while (continue1) {  //iteration of generating tables
        continue1 = false;
        for (auto rule : rules_struct) {
            bool is_generating = all_of(rule.RHS.begin(), rule.RHS.end(), [&](string i) { // get RHS of rule and check if all symbols are generating
                return generating_table[GetIndex(universe, i)];
            });
            if (is_generating && !generating_table[GetIndex(universe, rule.LHS)]) { //if all symbols in RHS are generating and LHS is not generating then make LHS generating
                generating_table[GetIndex(universe, rule.LHS)] = true;
                continue1 = true;
            }
        }
    }
    //creating vector of generating rules
    for (auto rule_ : rules_struct) {
        if ( all_of(rule_.RHS.begin(), rule_.RHS.end(), [&](string i) { //if all symbols in RHS are generating then add rule to genRules
            return generating_table[GetIndex(universe, i)];
        })) {
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
            if (foundInVector(reachable_symbols,rule.LHS)){ //if LHS is in reachable_symbols then add RHS to reachable_symbols

                for (auto i : rule.RHS)
                {
                    if (AddToSetIfNotPresent(reachable_symbols,i))
                    {
                        continue1 = true;
                    }
                }
            }
        }
    }
    bool final_gen=false;
    for (auto rule: genRules)
    {
        for(auto i: rule.RHS) {

            if (!foundInVector(reachable_symbols,i)) //if any of the RHS symbols is not reachable the set the flag to false otherwise true
            {
                final_gen= false;
            }
            else{
                final_gen= true;
            }
        }


        if(final_gen== true and foundInVector(reachable_symbols,rule.LHS)){ //if the flag is true and LHS is reachable then add the rule to final_genRules
            final_genRules.push_back(rule);
        }
    }

    if (final_genRules.size()!=rules_struct.size()) //if final_genRules is empty then grammar is not LL(1)
    {
        cout << "NO\n";
        return;
    }
    bool has_conflict = false;
    for (auto i : non_terms)  //for each non terminal in the grammar
    {
        int indexA = GetIndex(indexOf_first_sets, i);  //first set of A
        int indexB = GetIndex(indexOf_follow_sets, i);  //follow set of A
        if (indexA < first_sets.size())
        {
            if (foundInVector(first_sets[indexA], "#")) //if A contains epsilon
            {
                for (auto k : first_sets[indexA]) //for each element in first set of A
                {
                    if (k != "#") //if the element is not epsilon
                    {
                        for (auto m : follow_sets[indexB])
                        {
                            if (k == m)
                            {
                                has_conflict = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    if (has_conflict) {
        cout << "NO\n";
    } else {
        cout << "YES\n";
    }

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

    ReadGrammar();  //Read the grammar as mentioned in the assignment

    switch (task) {
        case 1: printTerminalsAndNoneTerminals();
            break;

        case 2: RemoveUselessSymbols();
            break;

        case 3: CalculateFirstSets();
                printFirstSets();
            break;

        case 4: CalculateFollowSets();
            break;

        case 5:
            CheckIfGrammarHasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}