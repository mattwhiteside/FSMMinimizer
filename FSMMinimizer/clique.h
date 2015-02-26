//
//  clique.h
//  wx-test1
//
//  Created by Matthew Whiteside on 5/6/14.
//  Copyright (c) 2014 mattwhiteside. All rights reserved.
//

#ifndef __wx_test1__clique__
#define __wx_test1__clique__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

bool removable(vector<int> neighbor, vector<int> cover);
int max_removable(vector<vector<int>> neighbors, vector<int> cover);
vector<int> procedure_1(vector<vector<int>> neighbors, vector<int> cover);
vector<int> procedure_2(vector<vector<int>> neighbors, vector<int> cover,
                        int k);
int cover_size(vector<int> cover);

#endif /* defined(__wx_test1__clique__) */
