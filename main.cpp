#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

class State{
    char leftNonTerminal_;
    std::string rightSideOfState_;
    int origin_;
public:
    State(char left, std::string right, int org = 0) : leftNonTerminal_(left), rightSideOfState_(std::move(right)), origin_(org) {}
    void Print() { std::cout << leftNonTerminal_ << "->" << rightSideOfState_ << "\n"; }
    char GetLeftNonTerminal() const { return leftNonTerminal_; }
    std::string GetRightSideOfState() const { return rightSideOfState_; }
    friend bool operator==(State& rhs, State&lhs);
    ~State(){ rightSideOfState_.clear(); }
};

bool operator==(State &rhs, State &lhs) {
    return rhs.leftNonTerminal_ == lhs.leftNonTerminal_ &&
           rhs.rightSideOfState_ == lhs.rightSideOfState_;
}

void Predictor(std::vector<std::vector<State*>>& SetOfStates, State* state, int nrOfSet, const std::vector<State*>& grammar){
}


void Scanner(std::vector<std::vector<State*>>& SetOfStates, State* state, int nrOfSet, const std::string& word){
}

void Completer(std::vector<std::vector<State*>>& SetOfStates, State* state, int nrOfSet){
}

bool IsNextElementOfStateTerminal(State* state){
    return isupper(state->GetRightSideOfState()[state->GetRightSideOfState().find('.') + 1]);
}

bool IsStateFinished(State* state){
    return state->GetRightSideOfState().find('.') == state->GetRightSideOfState().size() - 1;
}

bool TestWord(const std::string& input, const std::vector<State*>& grammar){
    std::vector<std::vector<State*>> SetOfStates(input.length()+1);
    std::string seed; seed += grammar[0]->GetLeftNonTerminal(); seed +=".";
    SetOfStates[0].push_back(new State('P', seed)); // insert the seed state

    for(int wordIndex = 0; wordIndex <= input.length(); ++wordIndex)
        for(int stateIndex = 0; stateIndex < SetOfStates[wordIndex].size(); ++stateIndex) // indexing over because push breaks iterators
            if(!IsStateFinished(SetOfStates[wordIndex][stateIndex])){
                if(!IsNextElementOfStateTerminal(SetOfStates[wordIndex][stateIndex]))
                    Predictor(SetOfStates, SetOfStates[wordIndex][stateIndex], wordIndex, grammar);
                else Scanner(SetOfStates, SetOfStates[wordIndex][stateIndex], wordIndex, input);
            } else Completer(SetOfStates, SetOfStates[wordIndex][stateIndex], wordIndex);

    return false;
}

void ReadInputAndGrammar(std::string& input, std::vector<State*>& grammar){
    std::ifstream fin("data.txt");
    getline(fin, input);

    std::string line;
    while(getline(fin, line)) {
        grammar.push_back(new State(static_cast<char>(line[0]), line.substr(3)));
    }
}

int main(){
    std::string input_;
    std::vector<State*> grammar_;
    ReadInputAndGrammar(input_, grammar_);
    //TestWord(input_, grammar_);
    return 0;
}
