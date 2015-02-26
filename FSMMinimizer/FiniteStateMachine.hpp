//
//  FiniteStateMachine.h
//  FSMMinimizer
//
//  Created by Matthew Whiteside on 5/3/14.
//  Copyright (c) 2014 mattwhiteside. All rights reserved.
//

#include <map>
#include <string>
#include <array>
#include <vector>
#include <tuple>
#include <regex>
#include <iostream>
#include <functional>
#include "clique.h"

using namespace std;
string implode(const vector<pair<string, string>> &pairs) {
  string imploded;
  for (unsigned i = 0; i < pairs.size(); i++) {
    imploded += pairs[i].first + " = " + pairs[i].second;
    if (i < pairs.size() - 1) {
      imploded += ", ";
    }
  }

  return imploded;
}

template <class T>
inline constexpr T power(const T base, unsigned const exponent) {
  // (parentheses not required in next line)
  return (exponent == 0) ? 1 : (base * power(base, exponent - 1));
}

enum class LogicState { _0, _1, DC };

template <size_t _nStates, size_t _nInputs, size_t _nOutputs> class AsyncFSM {
	static const size_t nStates = _nStates;
	static const size_t nOutputs = _nOutputs;
	static const size_t nInputs = _nInputs;
	
  typedef pair<string, vector<LogicState>> StateEncoding;
	map<string, StateEncoding> FSMencoding;
	typedef array<LogicState,_nOutputs> FSMoutput;
	map<string, array<pair<StateEncoding,FSMoutput>, power(2,_nOutputs)>> stateTable;

	array<string, nStates> states;
	array<array<string, power(2,nInputs)>, nStates> transitions;
	array<array<array<LogicState, nOutputs>,power(2,nInputs)>, nStates> outputs;
	


  // std::map<StateEncoding, array<FSMStateEncoding, power(2, _nInputs)>>
  // stateTable;

public:
  //AsyncFSM(FSMencoding,)
};

template <size_t _nStates, size_t _nInputs, size_t _nOutputs>
class FiniteStateMachine {

public:
  enum class Compatibility { OK, MAYBE_OK, NO_GOOD };

  static const unsigned nStates = _nStates;
  static const unsigned nInputs = _nInputs;
  static const unsigned nOutputs = _nOutputs;
  static const unsigned nCols = power(2, _nInputs);

  map<string, array<string, nCols>> _stateTable;
  const array<string, nInputs> _inputs;
  typedef map<pair<string, string>,
              pair<Compatibility, vector<pair<string, string>>>> ImplicantTable;
  ImplicantTable _implicantTable;
  vector<string> cliques;
  vector<string> _states;
  map<string, array<string, nCols>> _minimizedStateTable;

  typedef typename map<
      pair<string, string>,
      pair<Compatibility, vector<pair<string, string>>>>::iterator StateCombo;
  FiniteStateMachine(map<string, array<string, nCols>> stateTable,
                     const array<string, nInputs> inputs)
      : _stateTable(stateTable), _inputs(inputs) {

    for (auto &keyval : stateTable) {
      _states.push_back(keyval.first);
    }
    processStatesInternal([](StateCombo sc) {}, true);
  }

  void processStates(std::function<void(StateCombo sc)> func) {
    processStatesInternal(func, false);
  }

  pair<Compatibility, vector<pair<string, string>>>
  operator[](pair<string, string> key) {
    StateCombo iter = _implicantTable.find(key);
    if (iter == _implicantTable.end()) {
      iter = _implicantTable.find({key.second, key.first});
    }
    return iter->second;
  }

  void resolveDependencies(unsigned depth = 0) {
    for (auto &keyval : _implicantTable) {

      if (keyval.second.first == Compatibility::MAYBE_OK) {
        auto &dependencies = keyval.second.second;
        for (auto &key : dependencies) {
          const auto &dependency = (*this)[key];
          if (dependency.first == Compatibility::NO_GOOD) {
            keyval.second.first = Compatibility::NO_GOOD;
            goto nextIteration; // break out of this second level nested loop
          }
        }
        auto predicate = [&](pair<string, string> key)->bool {
          const auto &dependency = (*this)[key];
          return dependency.first == Compatibility::OK;
        };

        auto remover =
            std::remove_if(dependencies.begin(), dependencies.end(), predicate);
        dependencies.erase(remover, dependencies.end());
        if (dependencies.empty()) {
          keyval.second.first = Compatibility::OK;
        } else {
          int originals = (int)dependencies.size();
          while (originals > 0) {
            auto oldDep = dependencies[0];
            auto newDeps = (*this)[oldDep].second;
            for (auto newDep : newDeps) {
              if (newDep == keyval.first) {
                // cout << "doh" << endl;
                goto nextIteration;
              } else if (newDep.second == keyval.first.first &&
                         newDep.first == keyval.first.second) {
                // cout << "super doh" << endl;
                goto nextIteration;
                // not sure if this branch is necessary
              } else if (std::find(dependencies.begin(), dependencies.end(),
                                   newDep) != dependencies.end()) {
                // cout << "hyper doh" << endl;
                goto nextIteration;

              } else {
                dependencies.push_back(newDep);
              }
            }
            dependencies.erase(dependencies.begin());
            originals--;
          }
          resolveDependencies(depth + 1);
        }
      }
    nextIteration:
      cout << "";
    }
    generateCliques();
    checkCoverage();
    genMinimizedStateTable();
  }

  void generateCliques() {
    //ifstream infile("graph.txt");
    // ofstream outfile("cliques.txt");
    cout << "Clique Algorithm." << endl;
    int n, i, j, k, K, p, q, r, s, min, counter = 0;
    n = (int)_states.size();
    vector<vector<int>> graph;

    for (auto &row : _stateTable) {
      vector<int> _row;
      for (auto &col : _stateTable) {
        pair<string, string> key(row.first, col.first);
        if (key.first == key.second) {
          _row.push_back(1);

        } else {
          const auto &implicant = (*this)[key];
          if (implicant.first == Compatibility::OK ||
              implicant.first == Compatibility::MAYBE_OK)
            _row.push_back(0);
          else
            _row.push_back(1);
        }
      }
      graph.push_back(_row);
    }

    // Find Neighbors
    vector<vector<int>> neighbors;
    for (i = 0; i < graph.size(); i++) {
      vector<int> neighbor;
      for (j = 0; j < graph[i].size(); j++)
        if (graph[i][j] == 1)
          neighbor.push_back(j);
      neighbors.push_back(neighbor);
    }
    // cout << "Graph has n = " << n << " vertices." << endl;
    // Read maximum size of Clique wanted
    K = 2;
    k = n - K;
    // Find Cliques
    bool found = false;
    // cout << "Finding Cliques..." << endl;
    min = n + 1;
    vector<vector<int>> covers;
    vector<int> allcover;
    for (i = 0; i < graph.size(); i++)
      allcover.push_back(1);
    for (i = 0; i < allcover.size(); i++) {
      if (found)
        break;
      counter++;
      // cout << counter << ". ";

      vector<int> cover = allcover;
      cover[i] = 0;
      cover = procedure_1(neighbors, cover);
      s = cover_size(cover);
      if (s < min)
        // min = s;
        if (s <= k) {
          // cout << "Clique (" << n - s << "): ";
          string clique;
          for (j = 0; j < cover.size(); j++)
            if (cover[j] == 0)
              clique += _states[j];
          if (std::find(cliques.begin(), cliques.end(), clique) ==
              cliques.end())
            cliques.push_back(clique);
          // cout << "Clique Size: " << n - s << endl;
          covers.push_back(cover);
          //        found = true;
          //        break;
        }
      for (j = 0; j < n - k; j++)
        cover = procedure_2(neighbors, cover, j);
      s = cover_size(cover);
      // if (s < min)
      // min = s;
      // cout << "Clique (" << n - s << "): ";
      string clique;
      for (j = 0; j < cover.size(); j++)
        if (cover[j] == 0)
          clique += _states[j];
      ;
      if (std::find(cliques.begin(), cliques.end(), clique) == cliques.end())
        cliques.push_back(clique);
      //      cout << "Clique Size: " << n - s << endl;
      covers.push_back(cover);
      if (s <= k) {
        //        found = true;
        //        break;
      }
    }
    // Pairwise Intersections
    for (p = 0; p < covers.size(); p++) {
      if (found)
        break;
      for (q = p + 1; q < covers.size(); q++) {
        if (found)
          break;
        counter++;
        vector<int> cover = allcover;
        for (r = 0; r < cover.size(); r++)
          if (covers[p][r] == 0 && covers[q][r] == 0)
            cover[r] = 0;
        cover = procedure_1(neighbors, cover);
        s = cover_size(cover);
        if (s < min)
          // min = s;
          if (s <= k) {

            string clique;
            for (j = 0; j < cover.size(); j++)
              if (cover[j] == 0)
                clique += _states[j];
            ;
            if (std::find(cliques.begin(), cliques.end(), clique) ==
                cliques.end())
              cliques.push_back(clique);

            //          found = true;
            //          break;
          }
        for (j = 0; j < k; j++)
          cover = procedure_2(neighbors, cover, j);
        s = cover_size(cover);

        string clique;
        for (j = 0; j < cover.size(); j++)
          if (cover[j] == 0)
            clique += _states[j];
        if (std::find(cliques.begin(), cliques.end(), clique) == cliques.end())
          cliques.push_back(clique);

        if (s <= k) {
          //          found = true;
          //          break;
        }
      }
    }
  }

private:
  void processStatesInternal(std::function<void(StateCombo sc)> func,
                             bool initial) {
    unsigned r;
    r = 2;

    vector<bool> vec(nStates);
    fill(vec.begin() + r, vec.end(), true);
    vector<string> _states;
    for (auto &keyval : _stateTable) {
      _states.push_back(keyval.first);
    }

    do {

      pair<string, string> key;
      for (int i = 0; i < nStates; ++i) {
        if (!vec[i]) {
          auto state = _states[i];
          if (key.first.length() == 0) {
            key.first = state;
          } else {
            key.second = state;
          }
        }
      }

      if (initial) {
        genImplicant(key);
      } else {
        StateCombo sc = _implicantTable.find(key);
        func(sc);
      }

    } while (std::next_permutation(vec.begin(), vec.end()));
  }

  void genImplicant(pair<string, string> &key) {

    const string currentState = key.first;
    const string stateBeingCompared = key.second;
    for (unsigned k = 0; k < nCols; k++) {

      string outputBitString1 = _stateTable[currentState][k].substr(
          2, _stateTable[currentState][k].length() - 1);
      string outputBitString2 = _stateTable[stateBeingCompared][k].substr(
          2, _stateTable[stateBeingCompared][k].length() - 1);

      for (unsigned m = 0; m < outputBitString1.length(); m++) {
        char bit1 = outputBitString1[m];
        char bit2 = outputBitString2[m];
        bool dontCare = bit1 == '-' || bit2 == '-';
        bool outputsEqual = bit1 == bit2;
        if (dontCare || outputsEqual) {
          // keep looking
        } else {
          _implicantTable[key].first = Compatibility::NO_GOOD;
          return;
        }
      }
      string nextState1 = _stateTable[currentState][k].substr(0, 1);
      string nextState2 = _stateTable[stateBeingCompared][k].substr(0, 1);
      bool dontCare = nextState1 == "-" || nextState2 == "-";
      if (nextState1 == nextState2 || dontCare) {
        if (_implicantTable.find(key) == _implicantTable.end()) {
          _implicantTable[key].first = Compatibility::OK;
        }
      } else {
        _implicantTable[key].first = Compatibility::MAYBE_OK;
        _implicantTable[key].second.push_back({nextState1, nextState2});
      }
    }
  }

  bool checkCoverage() {
    map<char, char> coverage;
    for (auto &clique : cliques) {
      for (auto member : clique) {
        coverage[member] = member;
        if (coverage.size() == _states.size()) {
          return true;
        }
      }
    }
    return false;
  }
	
	bool checkClosure(){
		
	}

  void genMinimizedStateTable() {
    //for (auto &clique : cliques) {
    //< #statements # >
		//}
  }
};

