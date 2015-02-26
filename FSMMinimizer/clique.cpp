//
//  clique.cpp
//  wx-test1
//
//  Created by Matthew Whiteside on 5/6/14.
//  Copyright (c) 2014 mattwhiteside. All rights reserved.
//

#include "clique.h"

bool removable(vector<int> neighbor, vector<int> cover) {
  bool check = true;
	for (int i = 0; i < neighbor.size(); i++){
    if (cover[neighbor[i]] == 0) {
      check = false;
      break;
    }
	}
  return check;
}

int max_removable(vector<vector<int>> neighbors, vector<int> cover) {
  int r = -1, max = -1;
  for (int i = 0; i < cover.size(); i++) {
    if (cover[i] == 1 && removable(neighbors[i], cover) == true) {
      vector<int> temp_cover = cover;
      temp_cover[i] = 0;
      int sum = 0;
			for (int j = 0; j < temp_cover.size(); j++){
				if (temp_cover[j] == 1 && removable(neighbors[j], temp_cover) == true){
          sum++;
				}
			}
      if (sum > max) {
        if (r == -1) {
          max = sum;
          r = i;
        } else if (neighbors[r].size() >= neighbors[i].size()) {
          max = sum;
          r = i;
        }
      }
    }
  }
  return r;
}

vector<int> procedure_1(vector<vector<int>> neighbors, vector<int> cover) {
  vector<int> temp_cover = cover;
  int r = 0;
  while (r != -1) {
    r = max_removable(neighbors, temp_cover);
    if (r != -1)
      temp_cover[r] = 0;
  }
  return temp_cover;
}

vector<int> procedure_2(vector<vector<int>> neighbors, vector<int> cover,
                        int k) {
  int count = 0;
  vector<int> temp_cover = cover;

  for (int i = 0; i < temp_cover.size(); i++) {
    if (temp_cover[i] == 1) {
      int sum = 0, index = 0;
			for (int j = 0; j < neighbors[i].size(); j++){
        if (temp_cover[neighbors[i][j]] == 0) {
          index = j;
          sum++;
        }
			}
      if (sum == 1 && cover[neighbors[i][index]] == 0) {
        temp_cover[neighbors[i][index]] = 1;
        temp_cover[i] = 0;
        temp_cover = procedure_1(neighbors, temp_cover);
        count++;
      }
      if (count > k)
        break;
    }
  }
  return temp_cover;
}

int cover_size(vector<int> cover) {
  int count = 0;
	for (int i = 0; i < cover.size(); i++){
    if (cover[i] == 1)
      count++;
	}
  return count;
}