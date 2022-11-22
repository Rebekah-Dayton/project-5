//
// Created by Rebek on 11/1/2022.
//

#ifndef PROJECT_1_INTERPRETER_H
#define PROJECT_1_INTERPRETER_H
#include "DatalogProgram.h"
#include "Database.h"
#include "Graph.h"


class Interpreter {
private:
    DatalogProgram database;
    Database data;
    bool addedTuple = false;
    Graph dependency;
    std::vector<int> postorder;
public:
    Interpreter(DatalogProgram datalogInfo);
    ~Interpreter() = default;

    void DatabaseToString() {std::cout << data.To_String() << std::endl;}
    void CreateGraph();
    void EvaluateRules();
    void EvaluateRule(Rule* rule);
    void EvaluateQueries();

    Relation* EvaluatePredicate(Predicate* p);
    int FindVariable(std::string variable, std::vector<std::string> variables);

};


#endif //PROJECT_1_INTERPRETER_H
