//
// Created by c6222 on 2018/3/10.
//

#include "State.h"


std::vector<State *>State::get_transition(string symbol) {
        if (transition.find(symbol)!=transition.end()){
            return transition.find(symbol)->second;
        }
        else{
            return std::vector<State*>();
            //throw "State with name "+name+" has no defined next state for alphabet "+symbol;
        }
}

bool State::isEndstate() {
    return endstate;
}

State::State(const string &name) : name(name) {}

State::State(const string &name, bool endstate) : name(name), endstate(endstate) {}

void State::add_transition(string symbol, State *to) {
    if(transition.find(symbol)!=transition.end()){
        transition[symbol].push_back(to);
    }
    else{
        transition[symbol]=std::vector<State*>({to});
    }
}

const string &State::getName() const {
    return name;
}

map<string, vector<State *>> &State::getTransition(){
    return transition;
}
