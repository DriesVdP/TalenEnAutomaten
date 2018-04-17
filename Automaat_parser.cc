//
// Created by c6222 on 2018/3/10.
//modified based on l_parser.h from computer graphics
//
#include "Automaat_parser.h"
#include <iomanip>
#include <sstream>
#include <fstream>

namespace
{
    //helper class to enable / certain exceptions
    //and automatically disable them at the end of the scope
    class enable_exceptions
    {
    private:
        std::ios& ios;
        std::ios::iostate state;
    public:
        enable_exceptions(std::ios& an_ios, std::ios::iostate exceptions) :
                ios(an_ios)
        {
            state = ios.exceptions();
            ios.exceptions(exceptions);
        }
        ~enable_exceptions()
        {
            ios.exceptions(state);
        }
    };

    class stream_parser
    {
    private:
        std::istream& in;
        enable_exceptions ex;
        unsigned int line;
        unsigned int col;
        bool endOfLineRead;
    public:
        stream_parser(std::istream&stream) :
                in(stream), ex(stream, std::istream::failbit | std::istream::badbit), line(1), col(0), endOfLineRead(false)
        {
            in.clear();
        }
        virtual ~stream_parser()
        {
        }
        std::istream::int_type getString()
        {
            std::istream::int_type c = in.get();
            if (endOfLineRead)
            {
                line++;
                col = 0;
                endOfLineRead = false;
            }
            else
                col++;
            //goto next line when next char is read
            if (c == '\n')
            {
                endOfLineRead = true;
            }

            return c;
        }

        std::istream::int_type peekChar()
        {
            return in.peek();
        }

        void skip_comments_and_whitespace()
        {
            if (in.eof())
                return;
            for (; !in.eof();)
            {
                while ((!in.eof()) && isspace(peekChar()))
                    getString();
                if (in.eof())
                    break;
                if (peekChar() == '#')
                {
                    while ((!in.eof()) && peekChar() != '\n')
                        getString();
                }
                else
                    break;
            }
        }
        void assertChars(std::string const& chars)
        {
            for (char const* i = chars.c_str(); *i != 0x0; i++)
            {
                if (getString() != *i)
                    throw Automaton::ParserException(std::string("Did not find expected string\"") + chars + "\"", line, col);
            }
        }
        std::string readQuotedString()
        {
            if (((char) getString()) != '"')
                throw Automaton::ParserException("Did not find expected string char:'\"'", line, col);
            std::string value("");
            for (std::istream::int_type c = getString(); c != '"'; c = getString())
                value += c;
            return value;
        }


        string readQuotedChar()
        {
            if (((char) getString()) != '"')
                throw Automaton::ParserException("Did not find expected string char:'\"'", line, col);
            char value= getString();
            if (((char) getString()) != '"')
                throw Automaton::ParserException("Did not find expected char:'\"'", line, col);
            string strValue ;
            strValue.push_back(value);
            return strValue;
        }

        int readInt()
        {
            std::istream::int_type c = getString();
            //int sign = +1;
            if (c == '-')
            {
                //sign = -1;
                c = getString();
            }
            if (!std::isdigit(c))
                throw Automaton::ParserException("Did not find expected digit when expected", line, col);
            int value = 0;
            while (std::isdigit(c) && (!in.eof()))
            {
                value = value * 10 + c - '0';
                try
                {
                    c = peekChar();
                } catch (std::ios::failure& f)
                {
                    if (in.eof())
                        break;
                    else
                        throw f;
                }

                if (std::isdigit(c))
                    getString();
            }
            return value;
        }

        bool readBool(){
            //bool boolean;
            std::string temp="";
            for (int i = 0; i <4 ; ++i) {
                temp+= getString();
            }
            if (temp=="true"||temp=="True"){
                return true;
            }
            else{
                temp+= getString();
                if (temp!="false"&&temp!="False"){
                    throw Automaton::ParserException("boolean is not false or False",line,col);
                }
                return false;
            }
        }

        double readDouble()
        {
            int whole = readInt();
            if (peekChar() != '.')
            {
                return whole;
            }
            getString();
            std::istream::int_type c = getString();
            if (!std::isdigit(c))
                throw Automaton::ParserException("Did not find expected digit when expected", line, col);
            int part = 0;
            int denom = 1;
            while (std::isdigit(c) && (!in.eof()))
            {
                part = part * 10 + c - '0';
                denom *= 10;
                try
                {
                    c = peekChar();
                } catch (std::ios::failure& f)
                {
                    if (in.eof())
                        break;
                    else
                        throw f;
                }
                if (std::isdigit(c))
                    getString();
            }
            return (double) whole + ((double) part / (double) denom);
        }
        int getLine()
        {
            return line;
        }
        int getCol()
        {
            return col;
        }

    };

    std::string parse_type(stream_parser& parser){
        parser.assertChars("{");
        parser.skip_comments_and_whitespace();
        parser.assertChars("\"type\":");
        parser.skip_comments_and_whitespace();
        std::string type=parser.readQuotedString();
        parser.skip_comments_and_whitespace();
        parser.getString();
        if (type!="eNFA"&&type!="NFA"&&type!="DFA"){
            throw Automaton::ParserException("The type of this Automaton is not eNFA, NFA or DFA",parser.getLine(),parser.getCol());
        }
        return type;
    }

    void parse_alphabet(std::set<string> &alphabet, stream_parser &parser)
    {
        parser.skip_comments_and_whitespace();
        parser.assertChars("\"alphabet\":");
        parser.skip_comments_and_whitespace();
        parser.assertChars("[");
        parser.skip_comments_and_whitespace();
        alphabet.clear();
        string c = parser.readQuotedChar();
        while (true)
        {
            if (alphabet.find(c) != alphabet.end())
                throw Automaton::ParserException(std::string("Double entry '") + c + "' in alphabet specification", parser.getLine(), parser.getCol());
            alphabet.insert(c);
            parser.skip_comments_and_whitespace();
            c = parser.getString();
            if (c == "]"){
                break;
            }
            else if (c != ",")
                throw Automaton::ParserException("Expected ','", parser.getLine(), parser.getCol());
            parser.skip_comments_and_whitespace();
            c = parser.readQuotedChar();
        }
        parser.getString();//'removing comma'
    }

    string parse_epsilon(stream_parser& parser){
        parser.skip_comments_and_whitespace();
        parser.assertChars("\"eps\":");
        parser.skip_comments_and_whitespace();
        string epsilon=parser.readQuotedChar();
        parser.getString();
        return epsilon;
    }

    void parse_States(std::vector<State *>& states, stream_parser& parser,std::vector<State*>&beginstate)
    {
        std::string name;
        bool start;
        bool end;
        char c;
        parser.skip_comments_and_whitespace();
        parser.assertChars("\"states\":");
        parser.skip_comments_and_whitespace();
        parser.assertChars("[");
        parser.skip_comments_and_whitespace();
        while (true){
            parser.assertChars("{");
            parser.skip_comments_and_whitespace();
            parser.assertChars("\"name\":");
            parser.skip_comments_and_whitespace();
            name=parser.readQuotedString();
            for (State* i:states) {
                if (i->getName()==name) {
                    throw Automaton::ParserException(std::string("Double entry '") + name + "' in state specification", parser.getLine(), parser.getCol());
                }
            }
            parser.getString();
            parser.skip_comments_and_whitespace();
            parser.assertChars("\"starting\":");
            parser.skip_comments_and_whitespace();
            start=parser.readBool();
            parser.skip_comments_and_whitespace();
            parser.getString();
            parser.skip_comments_and_whitespace();
            parser.assertChars("\"accepting\":");
            parser.skip_comments_and_whitespace();
            end=parser.readBool();
            parser.skip_comments_and_whitespace();
            parser.assertChars("}");
            State* mystate=new State(name,end);
            if (start){
                beginstate.push_back(mystate);
            }
            states.push_back(mystate);
            c = parser.getString();
            if (c != ',') {
                parser.skip_comments_and_whitespace();
                c = parser.getString();
                if (c == ']') {
                    parser.getString();
                    break;
                } else {
                    throw Automaton::ParserException("Expected ','", parser.getLine(), parser.getCol());
                }
            }
            parser.skip_comments_and_whitespace();
        }
    }

    void parse_transition(std::vector<State *>& states, stream_parser& parser,const std::set<string>& alphabet)
    {
        std::string from;
        std::string to;
        string input;
        //State* fromstate= nullptr;
        //State* tostate = nullptr;
        parser.skip_comments_and_whitespace();
        parser.assertChars("\"transitions\":");
        parser.skip_comments_and_whitespace();
        parser.assertChars("[");
        parser.skip_comments_and_whitespace();
        while (true){
            char c;
            State* fromstate= nullptr;
            State* tostate = nullptr;
            parser.assertChars("{");
            parser.skip_comments_and_whitespace();
            parser.assertChars("\"from\":");
            parser.skip_comments_and_whitespace();
            from=parser.readQuotedString();
            parser.getString();
            parser.skip_comments_and_whitespace();
            parser.assertChars("\"to\":");
            parser.skip_comments_and_whitespace();
            to=parser.readQuotedString();
            parser.skip_comments_and_whitespace();
            parser.getString();
            parser.skip_comments_and_whitespace();
            parser.assertChars("\"input\":");
            parser.skip_comments_and_whitespace();
            input=parser.readQuotedChar();
            if (alphabet.find(input) == alphabet.end())
                throw Automaton::ParserException(std::string("Automaton not accepting '") + input + "' in transition specification", parser.getLine(), parser.getCol());
            parser.skip_comments_and_whitespace();
            parser.assertChars("}");
            for (State* i:states) {
                if (i->getName()==from){
                    fromstate=i;
                }
                if(i->getName()==to){
                    tostate=i;
                }
                if(fromstate&&tostate){
                    break;
                }
            }
            if (fromstate&&tostate){
                fromstate->add_transition(input,tostate);
            }
            else{
                throw Automaton::ParserException(std::string("State not found with name ")+ from + " Or "+ to ,parser.getLine(),parser.getCol());
            }
            c = parser.getString();
            if (c != ','){
                parser.skip_comments_and_whitespace();
                c = parser.getString();
                if (c == ']'){
                    parser.getString();
                    break;
                }
                else{
                    throw Automaton::ParserException("Expected ','", parser.getLine(), parser.getCol());
                }
            }
            parser.skip_comments_and_whitespace();
        }
    }
}

Automaton::ParserException::ParserException(std::string const& msg, unsigned int line, unsigned int pos) :
        std::exception(), message()
{
    std::stringstream s;
    s << "Parser error at line: " << line << " pos: " << pos << " " << msg;
    message = s.str();
}

Automaton::ParserException::ParserException(const Automaton::ParserException &original) : std::exception(original), message(original.message)
{
}

Automaton::ParserException::~ParserException() throw ()
{
}

Automaton::ParserException& Automaton::ParserException::operator=(const Automaton::ParserException &original)
{
    message = original.message;
    return *this;
}

const char* Automaton::ParserException::what() const throw ()
{
    return message.c_str();
}


Automaton::Automaat::Automaat() {
    epsilon='e';
    type="";
}

Automaton::Automaat::~Automaat() {
    for (State* i:states){
        delete i;
    }
}

std::set<string> const &Automaton::Automaat::get_alphabet() const {
    return alphabet;
}

std::vector<State *>Automaton::Automaat::get_transition(string c) const {
    vector<State *> newstate;
    if (currentstate.size()==0){
        throw "Current state is empty.";
    }
    if (alphabet.find(c) == alphabet.end())
        throw std::string("Automaton not accepting '") + c + "' in transition specification";
    for(State* currentone:currentstate){
        for (State* newone:currentone->get_transition(c)) {
            newstate.push_back(newone);
        }
    }
    if (newstate.size()==0){
        std::string tempstring="Currentstate \"";
        for (State* i:currentstate) {
            tempstring+=" "+i->getName()+",";
        }
        tempstring=tempstring.substr(0,tempstring.size()-1);
        tempstring+="\" has no defined next state for alphabet "+c;
        throw tempstring;
    }
    return newstate;
}

void Automaton::Automaat::dotransition(string c) {
    if (currentstate.size()==0){
        throw "Current state is empty.";
    }
    vector<State*> newstate;
    if (alphabet.find(c) == alphabet.end())
        throw std::string("Automaton not accepting '") + c + "' in transition specification";
    for(State* currentone:currentstate){
        for (State* newone:currentone->get_transition(c)) {
            newstate.push_back(newone);
        }
    }
    if (newstate.size()==0){
        std::string tempstring="Currentstate \"";
        for (State* i:currentstate) {
            tempstring+=" "+i->getName()+",";
        }
        tempstring=tempstring.substr(0,tempstring.size()-1);
        tempstring+="\" has no defined next state for alphabet "+c;
        throw tempstring;
    }
    currentstate=newstate;
}


Automaton::Automaat::Automaat(std::istream &in) {
    in >> *this;
}

std::istream& Automaton::operator>>(std::istream& in, Automaton::Automaat& system)
{
    stream_parser parser(in);
    system.type=parse_type(parser);
    parse_alphabet(system.alphabet, parser);
    if (system.type=="eNFA"){
        system.epsilon=parse_epsilon(parser);
        system.alphabet.insert(system.epsilon);
    }
    parse_States(system.states,parser,system.currentstate);
    system.beginstate=system.currentstate;
    parse_transition(system.states,parser,system.alphabet);
    return in;
}

void Automaton::Automaat::To_Dotlanguage() const {
    ofstream myfile;
    std::stringstream ss;
    myfile.open(outputfilename);
    myfile<<"digraph finite_state_machine {\n"
            "\trankdir=LR;\n"
            "\tsize=\"8,5\"\n"
            "\t";
    if(beginstate[0]->isEndstate()){
        myfile<<"node [shape = doublecircle];";
        for (auto i:states) {
            if (i->isEndstate()){
                myfile<<" \""+i->getName()+"\"";
            }
        }

        myfile<<";\n\tnode [shape = circle];\n\t";
    }
    else{
        myfile<<"node [shape = circle];";
        for (auto i:states) {
            if (!(i->isEndstate())){
                myfile<<" \""+i->getName()+"\"";
            }
        }

        myfile<<";\n\tnode [shape = doublecircle];\n\t";
    }

    for (auto i:states) {
        for (auto j:i->getTransition()) {
            for (auto k:j.second){
                myfile<<"\""+i->getName()+"\""+" ->"+" \""+k->getName()+"\""+" [ label = \""+j.first+"\" ];\n\t";
            }
        }
    }
    myfile<<"}";
    myfile.close();

}

vector<State *>Automaton::Automaat::getCurrentstate() const {
    return currentstate;
}

vector<State *>Automaton::Automaat::getBeginstate() const {
    return beginstate;
}

const vector<State *> &Automaton::Automaat::getStates() const {
    return states;
}

Automaton::Automaat::Automaat(std::string filename) {
    ifstream temp(filename);
    outputfilename="o_"+filename;
    temp>>*this;
    temp.close();
}

void Automaton::Automaat::setOutputfilename(const string &outputfilename) {
    Automaat::outputfilename = outputfilename;
}

const string &Automaton::Automaat::getOutputfilename() const {
    return outputfilename;
}

string Automaton::Automaat::getEpsilon() const {
    return epsilon;
}

Automaton::Automaat::Automaat(string epsilon, const string &type, const set<string> &alphabet,
                              const vector<State *> &currentstate, const vector<State *> &beginstate,
                              const vector<State *> &states) : epsilon(epsilon), type(type), alphabet(alphabet),
                                                               currentstate(currentstate), beginstate(beginstate),
                                                               states(states) {}

const string &Automaton::Automaat::getType() const {
    return type;
}

const set<string> &Automaton::Automaat::getAlphabet() const {
    return alphabet;
}

Automaton::Automaat::Automaat(string epsilon, const string &type, const set<string> &alphabet,
                              const vector<State *> &currentstate, const vector<State *> &beginstate,
                              const string &outputfilename, const vector<State *> &states) : epsilon(epsilon),
                                                                                             type(type),
                                                                                             alphabet(alphabet),
                                                                                             currentstate(currentstate),
                                                                                             beginstate(beginstate),
                                                                                             outputfilename(
                                                                                                     outputfilename),
                                                                                             states(states) {}

void Automaton::Automaat::searchEndstates() {
    for(auto state:states){
        if(state->isEndstate()){
            endStates.emplace_back(state);
        }
    }
}

bool Automaton::Automaat::deleteState(State *state) {


    for(auto i:states){
        //alle transities van de state i:
        map<string, vector<State*>> iTransitions = i->getTransition();

        //ga elke transitie af:
        for(auto iTransition:iTransitions){

            //de vector van states naar waar i wijst (deze vector is een apparte state!! kunnen meerdere zijn door het samenvoegen van states bij TF):
            vector<State*> iTransitionStates = iTransition.second;

            //ga elke state af in de vector
            for(auto iTransitionState:iTransitionStates){
                if(iTransitionState->getName() == state->getName()){
                    //hier vinden we de states die naar de opgegevenState gaan
                    cout<<i->getName()<<" gaat naar "<<state->getName()<<endl;

                    //nu proberen we state i te deleten maar eerst moeten we de transities vervangen

                    map<string, vector<State*>> stateTransitions = state->getTransition();

                    for(auto stateTransition:stateTransitions){
                        vector<State*> stateTransitionStates = stateTransition.second;

                        //for(auto )

                    }

                }
            }
        }
    }
    return true;

}

void Automaton::Automaat::searchBeginForState(State *opgegevenState, stringstream &stringStream) {

    //ik ga alle states af en zoek alle states die naar de opgegevenState gaan met een transitie
    for(auto i:states){


        //alle transities van de state i:
        map<string, vector<State*>> iTransitions = i->getTransition();

        //ga elke transitie af:
        for(auto iTransition:iTransitions){

            //de vector van states naar waar i wijst (deze vector is een apparte state!! kunnen meerdere zijn door het samenvoegen van states bij TF):
            vector<State*> iTransitionStates = iTransition.second;

            //ga elke state af in de vector
            for(auto iTransitionState:iTransitionStates){
                if(iTransitionState->getName() == opgegevenState->getName()){
                    //hier vinden we de states die naar de opgegevenState gaan
                    cout<<i->getName()<<" gaat naar "<<opgegevenState->getName()<<endl;

                    //check eerst of state i geen beginstate is
                    for(auto beginState:getBeginstate()){
                        if(i == beginState){
                            return;
                        }
                    }

                    cout<<"we willen "<<i->getName()<<" deleten"<<endl;
                    deleteState(i);
                    //nu proberen we state i te deleten
                }
            }
        }
    }
}

void Automaton::Automaat::DFAToRegex(stringstream &stringStream) {

    //de automaat moet een dfa zijn om te kunnen overzetten naar een regex
    if(type != "DFA"){
        string s = "Automate Type Invalid For StateElimination";
        throw s;
    }

    searchEndstates();

//    zoek een pad van elke eindstates naar de beginstate door te backtracken
    for(auto endState:endStates){

        stringStream<< endState->getName()<<": ";

        searchBeginForState(endState, stringStream);


    }





}

