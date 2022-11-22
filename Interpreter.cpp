//
// Created by Rebek on 11/1/2022.
//

#include "Interpreter.h"

Interpreter::Interpreter(DatalogProgram datalogInfo) {
    database = datalogInfo;
    std::vector<Predicate*> schemes = database.GetSchemes();
    std::vector<Predicate*> facts = database.GetFacts();

    for (unsigned int i = 0; i < schemes.size(); i++) {
        Header header;
        for (int j = 0; j < schemes.at(i)->GetSize(); j++) {
            header.AddAttribute(schemes.at(i)->GetParam(j));
        }
        Relation* newRelation =new Relation(schemes.at(i)->GetId(), header);
        data.AddTable(schemes.at(i)->GetId(), newRelation);
    }

    for (unsigned int i = 0; i < facts.size(); i++) {
        Tuple tuple;
        for (int j = 0; j < facts.at(i)->GetSize(); j++) {
            tuple.AddValue(facts.at(i)->GetParam(j));
        }
        data.SetTable(facts.at(i)->GetId(), tuple);
    }
}

int Interpreter::FindVariable(std::string variable, std::vector<std::string> variables) {
    for (unsigned int i = 0; i < variables.size(); i++) {
        if (variables.at(i) == variable) {
            return i;
        }
    }
    return -1;
}

void Interpreter::CreateGraph() {
    std::vector<Rule*> rules = database.GetRules();
    std::vector<std::string> ruleOrder;

    std::cout << "Dependency Graph" << std::endl;
    for (unsigned int i = 0; i < rules.size(); i++) {
        dependency.AddNode(i, {});
        ruleOrder.push_back(rules.at(i)->GetHead()->GetId());
        postorder.push_back(i);
    }
    for (unsigned int i = 0; i < rules.size(); i++) {
        std::vector<Predicate*> body = rules.at(i)->GetRuleBody();

        for (unsigned int j = 0; j < body.size(); j++) {
            for (unsigned int k = 0; k < ruleOrder.size(); k++) {
                if (body.at(j)->GetId() == ruleOrder.at(k)) {
                    int index = dependency.FindNode(k);
                    dependency.AddAdjacentNode(i, index);
                }
            }
        }
    }

    dependency.To_String();
}

void Interpreter::EvaluateRules() {
    int passes = 0;
    std::vector<Rule*> rules = database.GetRules();

    CreateGraph();
    Graph reverse = dependency.ReverseGraph();
    postorder = reverse.DFSForestPostorder(postorder);

    std::vector<int> reverseOrder;
    for (int i = (postorder.size() - 1); i >= 0; i--) {
        reverseOrder.push_back(postorder.at(i));
    }
    std::vector<std::set<int>> forest = dependency.DFSForestSCCs(reverseOrder);

    std::cout << "Rule Evaluation";
    for (unsigned int i = 0; i < forest.size(); i++) {
        passes = 0;
        std::cout << std::endl << "SCC: ";

        std::string tostring = "";
        for (int j : forest.at(i)) {
            tostring += "R" + std::to_string(j) + "," ;
        }
        tostring.pop_back();
        std::cout << tostring << std::endl;
        if (forest.at(i).size() == 1) {
            bool dependent = false;
            for (int j : forest.at(i)) {
                if (dependency.CheckDependency(j)) {
                    dependent = true;
                }
            }
            if (dependent) {
                do {
                    addedTuple = false;
                    for (int j : forest.at(i)) {
                        EvaluateRule(rules.at(j));
                    }
                    passes++;
                } while(addedTuple);
            }
            else {
                addedTuple = false;
                for (int j : forest.at(i)) {
                    EvaluateRule(rules.at(j));
                }
                passes++;
            }
        }
        else {
            do {
                addedTuple = false;
                for (int j : forest.at(i)) {
                    EvaluateRule(rules.at(j));
                }
                passes++;
            } while(addedTuple);
        }
        std::cout << passes << " passes: " << tostring;
    }
    std::cout << std::endl;
}

void Interpreter::EvaluateRule(Rule* rule) {
    std::vector<Predicate*> body = rule->GetRuleBody();
    Relation* ruleBody = EvaluatePredicate(body.at(0));
    for (unsigned int i = 1; i < body.size(); i++) {
        Relation* newRelation = EvaluatePredicate(body.at(i));
        ruleBody = ruleBody->NaturalJoin(newRelation);
    }

    Predicate* head = rule->GetHead();
    std::vector<unsigned int> projectHead;
    Header header = ruleBody->GetHeader();
    for (int i = 0; i < head->GetSize(); i++) {
        int attribute = header.FindAttribute(head->GetParam(i));
        projectHead.push_back(attribute);
    }
    ruleBody = ruleBody->Project(projectHead);

    Relation* table = data.GetTable(head->GetId());
    header = table->GetHeader();
    ruleBody->SetHeader(header);

    std::cout << rule->to_String() << std::endl;
    table->Union(ruleBody);
    if (table->AddedTuple()) {
        addedTuple = table->AddedTuple();
    }
}

void Interpreter::EvaluateQueries() {
    std::vector<Predicate*> queries = database.GetQueries();

    std::cout << std::endl << "Query Evaluation" << std::endl;
    for (unsigned int i = 0; i < queries.size(); i++) {
        std::vector<std::string> variables;
        std::vector<unsigned int> paramIndex;
        Relation* table = data.GetTable(queries.at(i)->GetId());
        for (int j = 0; j < queries.at(i)->GetSize(); j++) {
            std::string parameter = queries.at(i)->GetParam(j);
            if (parameter[0] == '\'' ) {
                table = table->Select(j, parameter);
            }
            else {
                int index = FindVariable(parameter, variables);
                if (index == -1) {
                    variables.push_back(parameter);
                    paramIndex.push_back(j);
                }
                else {
                    table = table->SelectTwo(paramIndex.at(index), j);
                }
            }
        }
        if (!paramIndex.empty()) {
            table = table->Project(paramIndex);
        }
        for (unsigned int k = 0; k < variables.size(); k++) {
            table = table->Rename(k, variables.at(k));
        }
        std::cout << queries.at(i)->toString() << "? ";
        if (table->GetTupleSize() != 0) {
            if (paramIndex.empty()) {
                std::cout << "Yes(" << table->GetTupleSize() << ")" << std::endl;
            }
            else {
                std::cout << "Yes(" << table->GetTupleSize() << ")" << std::endl;
                std::cout << table->To_String();
            }
        }
        else {
            std::cout << "No" << std::endl;
        }
    }
}

Relation* Interpreter::EvaluatePredicate(Predicate* p) {
    Relation* table = data.GetTable(p->GetId());
    std::vector<std::string> variables;
    std::vector<unsigned int> paramIndex;
    for (int j = 0; j < p->GetSize(); j++) {
        std::string parameter = p->GetParam(j);
        if (parameter[0] == '\'' ) {
            table = table->Select(j, parameter);
        }
        else {
            int index = FindVariable(parameter, variables);
            if (index == -1) {
                variables.push_back(parameter);
                paramIndex.push_back(j);
            }
            else {
                table = table->SelectTwo(paramIndex.at(index), j);
            }
        }
    }
    if (!paramIndex.empty()) {
        table = table->Project(paramIndex);
    }
    for (unsigned int k = 0; k < variables.size(); k++) {
        table = table->Rename(k, variables.at(k));
    }
    return table;
}

