//
// Created by c6222 on 2018/3/10.
//

#ifndef AUTOMATON_STATE_H
#define AUTOMATON_STATE_H

#include <map>
#include <vector>
#include "string"

#include "set"
using namespace std;

class State {
    std::string name;
    map<string,std::vector<State*>>transition;
    bool endstate = false;

public:
    explicit State(const string &name);

    State(const string &name, bool endstate);

    const string &getName() const;

    vector<State*> get_transition(string symbol);

    bool isEndstate();

    void add_transition(string symbol, State *to);

    map<string, vector<State *>> &getTransition();
};


#endif //AUTOMATON_STATE_H
