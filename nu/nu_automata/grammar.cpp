#include "grammar.hpp"
using namespace automate;
using namespace std;

State::State(string identifiant) : id(identifiant){ }
State::State() = default;
State::~State() = default;

bool isFresh(vector<pair<bool,unordered_set<string>>> const& memory, string const& event){
  for(auto const& variable : memory){
    if(variable.second.find(event) != variable.second.end()){
      return false;
    }
  }
  return true;
}

bool use(int ressource,
          vector<pair<bool,unordered_set<string>>> & memory,
          string const& event){
  pair<bool,unordered_set<string>> & variable = memory[ressource];
  if(variable.first){
    if(!isFresh(memory, event)){
      return false;
    }
    variable.first = false;
    variable.second.insert(event);
    return true;
  }
  return variable.second.find(event) != variable.second.end();
}

void alloc(vector<int> ressources, vector<pair<bool,unordered_set<string>>> & memory){
  for(int ressource : ressources){
    memory[ressource].first = true;
  }
}

void desalloc(vector<int> ressources, vector<pair<bool,unordered_set<string>>> & memory){
  for(int ressource : ressources){
    memory[ressource].first = false;
    memory[ressource].second.clear();
  }
}

Transition::Transition(State* const& ori, State* const& dest, vector<int> allocs,
                       vector<int> freez) :
         origine(ori), destination(dest), allocations(allocs), frees(freez){}
Transition::Transition(State* const& ori, State* const& dest):
           origine(ori), destination(dest){}
Transition::~Transition() = default;
vector<pair<bool,unordered_set<string>>> Transition::accept_epsilon(
       vector<pair<bool,unordered_set<string>>> memory){
  return {};
}vector<pair<bool,unordered_set<string>>> Transition::accept_event(
       vector<pair<bool,unordered_set<string>>> memory, string p_event){
  return {};
}
vector<pair<bool,unordered_set<string>>> Transition::accept_constant(
       vector<pair<bool,unordered_set<string>>> memory, string p_constant){
  return {};
}

Epsilon_Transition::Epsilon_Transition(State* const& ori, State* const& dest, vector<int> allocs,
           vector<int> freez): Transition(ori,dest,allocs,freez){}
Epsilon_Transition::Epsilon_Transition(State* const& ori, State* const& dest): Transition(ori,dest){}
Epsilon_Transition::~Epsilon_Transition() = default;
vector<pair<bool,unordered_set<string>>> Epsilon_Transition::accept_epsilon(
       vector<pair<bool,unordered_set<string>>> memory){
  alloc(this->allocations, memory);
  desalloc(this->frees, memory);

  return memory;
}
string Epsilon_Transition::to_string(){
  return "epsi";
}

Event_Transition::Event_Transition(State* const& ori, State* const& dest, vector<int> allocs,
           vector<int> freez, int p_variable): Transition(ori,dest,allocs,freez),
                                               variable(p_variable){}
Event_Transition::Event_Transition(State* const& ori, State* const& dest, int p_variable):
                                    Transition(ori,dest), variable(p_variable){}
Event_Transition::~Event_Transition() = default;
vector<pair<bool,unordered_set<string>>> Event_Transition::accept_event(
       vector<pair<bool,unordered_set<string>>> memory, string p_event){
  alloc(this->allocations, memory);
  if(!use(this->variable, memory, p_event)){
    return {};
  }
  desalloc(this->frees, memory);

  return memory;
}
string Event_Transition::to_string(){
  return "{" + std::to_string(variable) + "}";
}

Constant_Transition::Constant_Transition(State* const& ori, State* const& dest, vector<int> allocs,
           vector<int> freez, string p_constant): Transition(ori,dest,allocs,freez),
                                               constant(p_constant){}
Constant_Transition::Constant_Transition(State* const& ori, State* const& dest, string p_constant):
                                    Transition(ori,dest), constant(p_constant){}
Constant_Transition::~Constant_Transition() = default;
vector<pair<bool,unordered_set<string>>> Constant_Transition::accept_constant(
       vector<pair<bool,unordered_set<string>>> memory, string p_constant){
   if(p_constant != constant){
     return {};
   }
  alloc(this->allocations, memory);
  desalloc(this->frees, memory);

  return memory;
}
string Constant_Transition::to_string(){
  return constant;
}

Automate::Automate() = default;

Automate::Automate(int p_ressources, unordered_set<string> p_constants,
            list<State> p_states, map<State*, vector<Transition*>> p_transitions,
            State* p_start, vector<State*> p_endStates) :
         ressources(p_ressources), constants(p_constants),
         states(p_states), transitions(p_transitions),
         start(p_start), endStates(p_endStates) {}
Automate::~Automate() {
  for(auto & pair : transitions){
    for(Transition * trans : pair.second){
      delete trans;
    }
  }
}

State* Automate::getState(string state_name){
  for(State& state : this->states){
    if(state.id == state_name){
      return &state;
    }
  }
  return nullptr;
}
