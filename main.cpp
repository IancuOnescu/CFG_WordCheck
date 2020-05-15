#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <utility>
#include <vector>
#include <algorithm>
#include <set>

class State{
    char leftNonTerminal_;
    std::string rightSideOfState_;
    int origin_;
public:
    State(char left, std::string right, int org = 0) : leftNonTerminal_(left), rightSideOfState_(std::move(right)), origin_(org) {}
    void Print() { std::cout << leftNonTerminal_ << "->" << rightSideOfState_ << ", " << origin_ << "\n"; }
    char GetLeftNonTerminal() const { return leftNonTerminal_; }
    std::string GetRightSideOfState() const { return rightSideOfState_; }
    int GetOrigin() const { return origin_; }
    friend bool operator==(State& rhs, State&lhs);
    ~State(){ rightSideOfState_.clear(); }
};

bool operator==(State &rhs, State &lhs) {
    return rhs.leftNonTerminal_ == lhs.leftNonTerminal_ &&
           rhs.rightSideOfState_ == lhs.rightSideOfState_ &&
           rhs.origin_ == lhs.origin_;
}

bool IsNextElementOfStateTerminal(State* state){
    return !isupper(state->GetRightSideOfState()[state->GetRightSideOfState().find('.') + 1]);
}

bool IsStateFinished(State* state){
    return state->GetRightSideOfState().find('.') == state->GetRightSideOfState().size() - 1;
}

bool wasStateAlreadyAdded(const std::vector<std::vector<State*>>& SetOfStates, State* state, int nrOfSet){
    for (auto setsIt : SetOfStates[nrOfSet])
        if(*setsIt == *state)
            return true;
    return false;
}

void Predictor(std::vector<std::vector<State*>>& SetOfStates, State* state, int nrOfSet, const std::vector<State*>& grammar, const std::set<char>& nullable){
    char st = state->GetRightSideOfState()[state->GetRightSideOfState().find('.') + 1];
    for (auto grammarIt : grammar) {
        if(grammarIt->GetLeftNonTerminal() == st){
            std::string right = ".";
            right += grammarIt->GetRightSideOfState();
            auto stateToAdd = new State(st, right, nrOfSet);
            if(!wasStateAlreadyAdded(SetOfStates, stateToAdd, nrOfSet))
                SetOfStates[nrOfSet].push_back(stateToAdd);
        }
    }
    auto nullPos = nullable.find(st);
    if(nullPos != nullable.end()) {
        std::string right = state->GetRightSideOfState();
        std::string newRight = right.substr(0, right.find('.')); newRight += st; newRight += '.';
        newRight += right.substr(right.find('.') + 2);
        auto stateToAdd = new State(state->GetLeftNonTerminal(), newRight, state->GetOrigin());
        if(!wasStateAlreadyAdded(SetOfStates, stateToAdd, nrOfSet))
            SetOfStates[nrOfSet].push_back(stateToAdd);
    }
}

void Scanner(std::vector<std::vector<State*>>& SetOfStates, State* state, int nrOfSet, const std::string& word){
    auto dotPosition = state->GetRightSideOfState().find('.');
    char charAfterDot = state->GetRightSideOfState()[dotPosition+1];
    char excl = '.';
    if(charAfterDot == word[nrOfSet]){
        std::string right = state->GetRightSideOfState().substr(0, dotPosition);
        right += charAfterDot; right += "."; right += state->GetRightSideOfState().substr(dotPosition+2);
        auto stateToAdd = new State(state->GetLeftNonTerminal(), right, state->GetOrigin());
        if(!wasStateAlreadyAdded(SetOfStates, stateToAdd, nrOfSet+1))
            SetOfStates[nrOfSet+1].push_back(stateToAdd);
    }
}

void Completer(std::vector<std::vector<State*>>& SetOfStates, State* state, int nrOfSet){
    char st = state->GetLeftNonTerminal();
    int index = state->GetOrigin();
    for(auto stateIt: SetOfStates[index]){
        std::string right = stateIt->GetRightSideOfState();
        auto dotPosition = right.find('.');
        if(right[dotPosition+1] == st) {
            std::string newRight = right.substr(0, dotPosition);
            newRight += st; newRight += "."; newRight += right.substr(dotPosition + 2);
            auto stateToAdd = new State(stateIt->GetLeftNonTerminal(), newRight, stateIt->GetOrigin());
            if(!wasStateAlreadyAdded(SetOfStates, stateToAdd, nrOfSet))
                SetOfStates[nrOfSet].push_back(stateToAdd);
        }
    }
}

bool TestWord(const std::string& input, const std::vector<State*>& grammar, const std::set<char>& nullable){
    std::vector<std::vector<State*>> SetOfStates(input.length()+1);
    std::string seed="."; seed += grammar[0]->GetLeftNonTerminal();
    SetOfStates[0].push_back(new State('P', seed)); // insert the seed state

    for(int wordIndex = 0; wordIndex <= input.length(); ++wordIndex)
        for(int stateIndex = 0; stateIndex < SetOfStates[wordIndex].size(); ++stateIndex) // indexing over because push breaks iterators
            if(!IsStateFinished(SetOfStates[wordIndex][stateIndex])){
                if(!IsNextElementOfStateTerminal(SetOfStates[wordIndex][stateIndex]))
                    Predictor(SetOfStates, SetOfStates[wordIndex][stateIndex], wordIndex, grammar, nullable);
                else Scanner(SetOfStates, SetOfStates[wordIndex][stateIndex], wordIndex, input);
            } else Completer(SetOfStates, SetOfStates[wordIndex][stateIndex], wordIndex);

    auto passedChecker= new State('P', std::string(1, grammar[0]->GetLeftNonTerminal()) + '.');
    return wasStateAlreadyAdded(SetOfStates, passedChecker, input.length());
}

void ReadInputAndGrammar(std::string& input, std::vector<State*>& grammar){
    std::ifstream fin("data.txt");
    getline(fin, input);

    std::string line;
    while(getline(fin, line)) {
        grammar.push_back(new State(static_cast<char>(line[0]), line.substr(3)));
    }
}

void Nullables(const std::vector<State*>& grammar, std::set<char>& nullables){ //can be optimized
    bool flag = true;
    while(flag){
        flag = false;
        for(auto it : grammar){
            auto alreadyAdded = nullables.find(it->GetLeftNonTerminal());
            if(alreadyAdded == nullables.end()){
                if(it->GetRightSideOfState() == "!")
                    nullables.insert(it->GetLeftNonTerminal());
                else{//Check if right side contains only nullables
                    std::string right = it->GetRightSideOfState();
                    bool secondFlag = true;
                    for(auto item : right){
                        auto find = nullables.find(item);
                        if(find == nullables.end())
                            secondFlag = false;
                    }
                    if(secondFlag)
                        nullables.insert(it->GetLeftNonTerminal());
                }
            }
        }
    }
}

int main(){
    std::string input_;
    std::vector<State*> grammar_;
    std::set<char> nullable_;
    ReadInputAndGrammar(input_, grammar_);
    Nullables(grammar_, nullable_);
    std::cout << TestWord(input_, grammar_, nullable_);
    return 0;
}
