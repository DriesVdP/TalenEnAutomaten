//
// Created by dries Van de Putte on 14/03/18.
// Studentennummer: s0170780
//
//

#include <iostream>
#include <gtest/gtest.h>
using namespace std;

#include "Automaat_parser.h"


class DFAToRegexTest: public  ::testing::Test{
protected:

    virtual void SetUp() {
    }
    virtual void TearDown() {
    }

};

TEST_F(DFAToRegexTest, testInit){
    Automaton::Automaat DFA1("../TestsStateElimination/test1.json");

    EXPECT_EQ("DFA",DFA1.getType());
    EXPECT_EQ("../TestsStateElimination/test1.json",DFA1.getOutputfilename());
    DFA1.setOutputfilename("../TestsStateElimination/test1.dot");
    EXPECT_EQ("../TestsStateElimination/test1.dot",DFA1.getOutputfilename());

    Automaton::Automaat DFA2("../TestsStateElimination/test2.json");
    EXPECT_EQ("DFA",DFA2.getType());
    EXPECT_EQ("../TestsStateElimination/test2.json",DFA2.getOutputfilename());
    DFA2.setOutputfilename("../TestsStateElimination/test2.dot");
    EXPECT_EQ("../TestsStateElimination/test2.dot",DFA2.getOutputfilename());

    Automaton::Automaat DFA3("../TestsStateElimination/test3.json");
    EXPECT_EQ("DFA",DFA3.getType());
    EXPECT_EQ("../TestsStateElimination/test3.json",DFA3.getOutputfilename());
    DFA3.setOutputfilename("../TestsStateElimination/test3.dot");
    EXPECT_EQ("../TestsStateElimination/test3.dot",DFA3.getOutputfilename());

}

TEST_F(DFAToRegexTest, test1){
    Automaton::Automaat DFA("../TestsStateElimination/test1.json");

}
