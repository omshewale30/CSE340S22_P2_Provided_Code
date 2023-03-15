///*
// * Copyright (C) Mohsen Zohrevandi, 2017
// *               Rida Bazzi 2019
// * Do not share this file with anyone
// */
//#include <iostream>
//#include <cstdio>
//#include <cstdlib>
//#include "lexer.h"
//
//using namespace std;
//
//struct rule_Storage{
//    string LHS;
//    vector<string> RHS;
//};
//
//// read grammar
//void ReadGrammar()
//{
//
//    cout << "0\n";
//}
//
//// Task 1
//void printTerminalsAndNoneTerminals()
//{
//
//    cout << "1\n";
//}
//
//// Task 2
//void RemoveUselessSymbols()
//{
//    cout << "2\n";
//}
//
//// Task 3
//void CalculateFirstSets()
//{
//    cout << "3\n";
//}
//
//// Task 4
//void CalculateFollowSets()
//{
//    cout << "4\n";
//}
//
//// Task 5
//void CheckIfGrammarHasPredictiveParser()
//{
//    cout << "5\n";
//}
//
//int main (int argc, char* argv[])
//{
//    int task;
//
//    if (argc < 2)
//    {
//        cout << "Error: missing argument\n";
//        return 1;
//    }
//
//    /*
//       Note that by convention argv[0] is the name of your executable,
//       and the first argument to your program is stored in argv[1]
//     */
//
//    task = atoi(argv[1]);
//
//    ReadGrammar();  // Reads the input grammar from standard input
//                    // and represent it internally in data structures
//                    // ad described in project 2 presentation file
//
//    switch (task) {
//        case 1: printTerminalsAndNoneTerminals();
//            break;
//
//        case 2: RemoveUselessSymbols();
//            break;
//
//        case 3: CalculateFirstSets();
//            break;
//
//        case 4: CalculateFollowSets();
//            break;
//
//        case 5: CheckIfGrammarHasPredictiveParser();
//            break;
//
//        default:
//            cout << "Error: unrecognized task number " << task << "\n";
//            break;
//    }
//    return 0;
//}
//
/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
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
vector<string> parse_Id_list(vector<string> vector1);
void parse_Rule();
vector<string> parseRHS(vector<string> v1);
void syntax_error();
void addTerminals(string element_to_check1);
void addNonTerminals(string element_to_check1);

Token expect(TokenType expected_type);
LexicalAnalyzer lexer;
//string terminals_nonterminals = "";
vector<string> terminals;
vector<string> non_terms;
vector<string> all_term;
vector<string> symbols;
vector<bool> generating_table;
vector<string> reachable_symbols;

//part2
vector<vector<string>> rules; //all the rules of the grammar
vector<vector<string>> genRules;

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
    expect(END_OF_FILE);

    //define the sysmbols
    symbols.push_back("#");
    symbols.push_back("$");
    for (auto i: terminals)
    {
        if (!count(non_terms.begin(), non_terms.end(), i)) {

            symbols.push_back(i);
        }

    }

    for (auto i: all_term)
    {
        if (count(non_terms.begin(), non_terms.end(), i)) {
            symbols.push_back(i);
        }
    }

}

void parse_Grammar()
{
    parse_Rule_list();
    expect(HASH);
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
        parse_Rule_list();
        return;
    }
    else
    {
        syntax_error();
    }

}

vector<string> parse_Id_list(vector<string> vector1)
{
    Token t = expect(ID);
    vector1.push_back(t.lexeme);
    addTerminals(t.lexeme);

    t = lexer.peek(1);
    if (t.token_type == ID)
    {
        vector<string> v = parse_Id_list(vector1);
        return v;
    }
    else if (t.token_type == STAR)
    {
        return vector1;
    }
    else
        syntax_error();
}

void parse_Rule()
{
    vector<string> rule;
    Token t = expect(ID);
    rule.push_back(t.lexeme);
    addNonTerminals(t.lexeme);
    expect(ARROW);
    rule = parseRHS(rule);

    rules.push_back(rule);
    expect(STAR);
    return;
}

vector<string> parseRHS(vector<string> v1)
{
    Token t = lexer.peek(1);
    if (t.token_type == ID)
    {
        vector<string> v = parse_Id_list(v1);
        return v;
    }
    else if (t.token_type == STAR)
    {
        v1.push_back("#");
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

Token expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

void addTerminals(string element_to_check1)
{
    if (!count(all_term.begin(), all_term.end(), element_to_check1)) {
        all_term.push_back(element_to_check1);
    }

    if (count(terminals.begin(), terminals.end(), element_to_check1)) {
        return;
        //cout << "Element found";
    }
    else {
        terminals.push_back(element_to_check1);

    }
}

void addNonTerminals(string element_to_check1)
{
    if (!count(all_term.begin(), all_term.end(), element_to_check1)) {
        all_term.push_back(element_to_check1);
    }

    if (count(non_terms.begin(), non_terms.end(), element_to_check1)) {
        return;
        //cout << "Element found";
    }
    else {
        non_terms.push_back(element_to_check1);
    }
}

// Task 1
void printTerminalsAndNoneTerminals()
{

    for (auto i: terminals)
    {
        if (!count(non_terms.begin(), non_terms.end(), i)) {

            std::cout << i << ' ';
        }

    }
    //cout << " ";
    for (auto i: all_term)
    {
        if (count(non_terms.begin(), non_terms.end(), i)) {
            std::cout << i << ' ';
        }
    }

    cout << " ";
}

// Task 2
void RemoveUselessSymbols()
{
    generating_table.push_back(true);
    generating_table.push_back(false);
    int index;
    for (auto i: symbols)
    {
        if (i != "#" && i != "$" )
        {
            if (!count(non_terms.begin(), non_terms.end(), i)) {

                generating_table.push_back(true);
            }
            else
            {
                generating_table.push_back(false);
            }
        }

    }
    bool continue1 = true;
    //iteration of generating tables
    while (continue1)
    {
        continue1 = false;
        for (auto rule: rules)
        {
            vector<string> temp_rule = rule;
            rule.erase(rule.begin());
            bool isgenerating = false;
            for (auto i: rule)
            {
                vector<string>::iterator itr = find(symbols.begin(), symbols.end(), i);
                index = distance(symbols.begin(), itr);
                if (!generating_table[index])
                {
                    isgenerating = false;
                    break;
                }
                else
                {
                    isgenerating = true;
                }
            }
            if (isgenerating)
            {
                vector<string>::iterator itr = find(symbols.begin(), symbols.end(), temp_rule[0]);
                index = distance(symbols.begin(), itr);
                if (!generating_table[index])
                {
                    generating_table[index] = true;
                    continue1 = true;
                }

            }
        }
    }
    for (auto rule_: rules)
    {
        //cout << rule_[0] << " ";
        bool isGen = true;
        for (auto i: rule_)
        {
            vector<string>::iterator itr0 = find(symbols.begin(), symbols.end(), i);
            index = distance(symbols.begin(), itr0);
            if (!generating_table[index])
            {
                isGen = false;
                break;
            }
        }

        if (isGen)
        {
            genRules.push_back(rule_);
        }

    }
    if (!genRules.empty())
        reachable_symbols.push_back(non_terms[0]);

    continue1 = true;
    while (continue1)
    {
        continue1 = false;
        for (auto rule: genRules)
        {
            if (count(reachable_symbols.begin(), reachable_symbols.end(), rule[0])) {
                for (auto i: rule)
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
    for (auto rule: genRules)
    {
        for (auto i: rule)
        {
            if (!count(reachable_symbols.begin(), reachable_symbols.end(), i))
            {
                vector<vector<string>>::iterator itr1 = find(genRules.begin(), genRules.end(), rule);
                index = distance(genRules.begin(), itr1);
                genRules[index].push_back("delete");
                break;
            }
        }
    }
    //print the rules
    for (auto rule: genRules)
    {
        if (!rule.empty())
        {
            string var = rule.back();
            if (var != "delete")
            {
                vector<string> temp_rule = rule;
                rule.erase(rule.begin());
                cout << temp_rule[0]  << " -> ";
                for (auto i: rule)
                {
                    cout << i << " ";
                }
                cout << "\n";
            }
        }

    }
}

// Task 3
void CalculateFirstSets()
{
    //cout << "hi";
    //I add first(#) = {#}
    vector<string> a_follow_set;
    first_sets_index.push_back("#");
    a_follow_set.push_back("#");
    first_sets.push_back(a_follow_set);
    a_follow_set.clear();

    //II first(a) = {a} for every terminal
    for (auto i: terminals)
    {
        if (!count(non_terms.begin(), non_terms.end(), i))
        {
            //cout << "hi";
            first_sets_index.push_back(i);
            a_follow_set.push_back(i);
            first_sets.push_back(a_follow_set);
            a_follow_set.clear();
        }

    }

    //initialize first sets for non_terms
    for (auto i: all_term)
    {
        if (count(non_terms.begin(), non_terms.end(), i)) {
            first_sets_index.push_back(i);
            first_sets.push_back(a_follow_set);
            a_follow_set.clear();
        }
    }

    /*
    for (auto i: first_sets_index)
    {
        cout << i << " ";
        cout << "\n";
    }*/

    bool change = true;
    //cout << "hi";
    //calculate
    while (change)
    {
        change = false;
        for (auto rule: rules)
        {
            //get index of AB in first_sets
            vector<string>::iterator itr = find(first_sets_index.begin(), first_sets_index.end(), rule[0]);
            int indexA = distance(first_sets_index.begin(), itr);

            vector<string>::iterator itr1 = find(first_sets_index.begin(), first_sets_index.end(), rule[1]);
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
                vector<string>::iterator itr2 = find(first_sets_index.begin(), first_sets_index.end(), rule[count1]);
                string str = "#";
                int indexC = distance(first_sets_index.begin(), itr2);
                if (count(first_sets[indexC].begin(), first_sets[indexC].end(), str))
                {
                    //V
                    if (count1 + 1 == rule.size())
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
                        vector<string>::iterator itr2 = find(first_sets_index.begin(), first_sets_index.end(), rule[count1 +1]);
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
    for (auto i: all_term)
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
    for (auto rule: rules)
    {
        if (rule.size() >= 3)
        {
            int index_count = 1;
            while (index_count>0)
            {
                vector<string>::iterator itr = find(follow_sets_index.begin(), follow_sets_index.end(), rule[index_count]);
                int indexA = distance(follow_sets_index.begin(), itr);


                if (index_count + 1 == rule.size())
                {
                    vector<string>::iterator itr1 = find(first_sets_index.begin(), first_sets_index.end(), rule[index_count + 1]);
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
                    vector<string>::iterator itr1 = find(first_sets_index.begin(), first_sets_index.end(), rule[index_count + 1]);
                    int indexB = distance(first_sets_index.begin(), itr1);
                    for (auto i: first_sets[indexB])
                    {
                        if (i != "#")
                        {
                            //cout << indexA << "\n";
                            //cout << follow_sets.size() << "\n";

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
                vector<string>::iterator itr2 = find(follow_sets_index.begin(), follow_sets_index.end(), rule[index_count]);
                int indexA = distance(follow_sets_index.begin(), itr2);
                int count2 = 1;
                while(count2 >0)
                {
                    if (index_count + count2 == rule.size())
                    {
                        count2 = -1;
                        break;
                    }
                    else
                    {
                        vector<string>::iterator itr2 = find(first_sets_index.begin(), first_sets_index.end(), rule[index_count + count2]);
                        int indexB = distance(first_sets_index.begin(), itr2);

                        if (count(first_sets[indexB].begin(), first_sets[indexB].end(), "#"))
                        {
                            vector<string>::iterator itr6 = find(first_sets_index.begin(), first_sets_index.end(), rule[index_count + count2 + 1]);
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

                if (index_count + 1 >= rule.size())
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
        for (auto rule: rules)
        {
            vector<string>::iterator itr2 = find(follow_sets_index.begin(), follow_sets_index.end(), rule[0]);
            int indexA = distance(follow_sets_index.begin(), itr2);

            if (rule.size() > 1)
            {
                vector<string>::iterator itr2 = find(follow_sets_index.begin(), follow_sets_index.end(), rule.back());
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

                if (rule.size() > 2)
                {
                    int index_back = rule.size() -1;;
                    while (index_back > 0)
                    {
                        if (index_back > 1)
                        {
                            vector<string>::iterator itr7 = find(first_sets_index.begin(), first_sets_index.end(), rule[index_back]);
                            int indexC = distance(first_sets_index.begin(), itr7);
                            vector<string>::iterator itr8 = find(follow_sets_index.begin(), follow_sets_index.end(), rule[index_back]);
                           // int indexD = distance(follow_sets_index.begin(), itr8);
                            if (count(first_sets[indexC].begin(), first_sets[indexC].end(), "#"))
                            {
                                vector<string>::iterator itr9 = find(follow_sets_index.begin(), follow_sets_index.end(), rule[index_back -1]);
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
            for (auto i: all_term)
            {
                int num1 = 0;
                if (count(non_terms.begin(), non_terms.end(), i)) {
                    vector<string>::iterator itr4 = find(first_sets_index.begin(), first_sets_index.end(), i);
                    int indexD = distance(first_sets_index.begin(), itr4);
                    cout << "FIRST(" << i << ") = { ";
                    for (auto k: symbols)
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
            for (auto i: all_term)
            {
                int num1 = 0;
                if (count(non_terms.begin(), non_terms.end(), i)) {
                    vector<string>::iterator itr4 = find(follow_sets_index.begin(), follow_sets_index.end(), i);
                    int indexD = distance(follow_sets_index.begin(), itr4);
                    cout << "FOLLOW(" << i << ") = { ";
                    for (auto k: symbols)
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

