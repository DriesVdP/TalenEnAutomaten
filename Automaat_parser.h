

#ifndef AUTOMATON_AUTOMAAT_PARSER_H
#define AUTOMATON_AUTOMAAT_PARSER_H
#include "iostream"

#include <map>
#include <vector>
#include "set"
#include "State.h"
namespace Automaton {
    class ParserException : public std::exception {
    private:
        /**
         * \brief The message explaining what went wrong
         */
        std::string message;

    public:

        /**
         * \brief Constructor
         *
         * \param msg	String explaining what went wrong
         * \param line	The line in the file at which the parser failed
         * \param pos	The position on the line at which the parser failed
         */
        ParserException(std::string const &msg, unsigned int line, unsigned int pos);

        /**
         * \brief Copy Constructor
         *
         * \param original	The exception to be copied
         */
        ParserException(const ParserException &original);

        /**
         * \brief Destructor
         */
        virtual ~ParserException() throw();

        /**
         * \brief Assignment operator
         *
         * \param original	The original exception to be assigned to this one
         */
        ParserException &operator=(const ParserException &original);

        /**
                     * \brief Returns a description of the error hat occurred.
                     *
                     * \return A description of the error hat occurred.
                     */
        virtual const char *what() const throw();
    };

    class Automaat;

    std::istream& operator>>(std::istream& in, Automaat& system);


    class Automaat {

    public:

        Automaat();

        ~Automaat();

        Automaat(std::istream& in);

        Automaat(std::string filename);

        std::set<string> const &get_alphabet() const;

        std::vector<State *>get_transition(string c) const;

        void dotransition(string c);

        void To_Dotlanguage() const;

        string getEpsilon() const;

        std::vector<State *>getCurrentstate() const;

        std::vector<State *>getBeginstate() const;

        const vector<State *> &getStates() const;

    public:
        Automaat(string epsilon, const string &type, const set<string> &alphabet, const vector<State *> &currentstate,
                 const vector<State *> &beginstate, const vector<State *> &states);

        Automaat(string epsilon, const string &type, const set<string> &alphabet, const vector<State *> &currentstate,
                 const vector<State *> &beginstate, const string &outputfilename, const vector<State *> &states);

        const string &getType() const;

        const set<string> &getAlphabet() const;

    protected:
        string epsilon;
        std::string type;
        std::set<string> alphabet;
        std::vector<State *> currentstate;
        std::vector<State *>beginstate;
        std::string outputfilename="output.txt";
        std::vector<State *> states;
        std::vector<State *> endStates;
        friend std::istream& operator>>(std::istream& in, Automaat& system);
    public:
        const string &getOutputfilename() const;

        void setOutputfilename(const string &outputfilename);

        void searchEndstates();

        bool deleteState(State *state);

        void searchBeginForState(State *opgegevenState, stringstream &stringStream);

        void DFAToRegex(stringstream &stringStream);


    };
}
#endif //AUTOMATON_AUTOMAAT_PARSER_H
