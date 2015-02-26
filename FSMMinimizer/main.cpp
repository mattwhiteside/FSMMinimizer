//
//  main.cpp
//  wx-test1
//
//  Created by Matthew Whiteside on 5/3/14.
//  Copyright (c) 2014 mattwhiteside. All rights reserved.
//

#include <iostream>
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/grid.h>
#include <sstream>

#include "FiniteStateMachine.hpp"
typedef FiniteStateMachine<6, 1, 2> MyFSM;
// typedef FiniteStateMachine<6, 2, 1> MyFSM;
//typedef FiniteStateMachine<6, 1, 1> MyFSM;
class MyApp : public wxApp {
  bool OnInit();

  wxFrame *frame;
  wxGrid *grid;
  MyFSM *fsm;

public:
  DECLARE_EVENT_TABLE()

  void OnAbout(wxCommandEvent &evt);
  void OnPrefs(wxCommandEvent &evt);

private:
  void DrawTableHeaderAndGutter(unsigned row);
};

IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE(MyApp, wxApp)
EVT_MENU(wxID_ABOUT, MyApp::OnAbout)
EVT_MENU(wxID_PREFERENCES, MyApp::OnPrefs)
END_EVENT_TABLE()

bool MyApp::OnInit() {
  // wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  frame = new wxFrame((wxFrame *)NULL, -1, wxT("FSM Minimizer"),
                      wxPoint(50, 50), wxSize(800, 600));

  wxMenuBar *menubar = new wxMenuBar();
  wxMenu *testmenu = new wxMenu();

  fsm = new MyFSM({{"1", {"2/-0", "1/1-"}},
                   {"2", {"-/--", "-/1-"}},
                   {"3", {"4/0-", "-/-1"}},
                   {"4", {"-/10", "-/1-"}},
                   {"5", {"5/--", "1/11"}},
                   {"6", {"-/-0", "-/1-"}}},
                  {"a"});

  //  fsm = new MyFSM({{"A", {"A/0", "B/0", "C/0", "D/1"}},
  //                   {"B", {"A/1", "B/1", "C/0", "D/1"}},
  //                   {"C", {"C/0", "-/-", "C/0", "-/-"}},
  //                   {"D", {"-/-", "A/0", "-/-", "D/-"}},
  //                   {"E", {"D/1", "B/0", "-/-", "-/-"}},
  //                   {"F", {"-/-", "-/-", "F/0", "D/-"}}},
  //                  {"X", "Y"});

//  fsm = new MyFSM({{"S1", {"S2/1", "S4/1","-/-", "S3/1", "-/-","-/-","-/-","-/-"}},
//                   {"S2", {"-/-","-/-", "S4/0", "-/-","S4/0","S4/0","S4/0","S4/0"}},
//                   {"S3", {"-/-","-/-", "S6/0", "S6/-","S6/0","S6/0","S6/0","S6/0",}},
//                   {"S4", {"S5/1", "S1/0", "-/-", "S6/0","-/-", "-/-", "-/-", "-/-"}},
//                   {"S5", {"-/-", "S2/-", "-/-", "-/-", "-/-","-/-","-/-", "-/-"}},
//                   {"S6", {"S3/-", "S2/1", "S3/0", "S2/-","S3/0","S3/0","S3/0","S3/0"}},
//                   {"S7", {"S3/1", "S2/1", "-/-", "-/-","-/-","-/-","-/-","-/-"}}
//		
//	},
//                  {"A", "B", "C"});

  //	fsm = new MyFSM({{"A", {"B/-","D/-"}},
  //									{"B",
  //{"F/-","E/-"}},
  //									{"C",
  //{"F/-","B/-"}},
  //									{"D",
  //{"A/-","E/-"}},
  //  								{"E",
  // {"C/-","B/-"}},
  //									{"F",
  //{"E/-","C/-"}}},
  //									{"X"}
  //	);

  grid = new wxGrid(frame, -1, wxPoint(0, 0), wxSize(400, 300));
  // Then we call CreateGrid to set the dimensions of the grid
  // (100 rows and 10 columns in this example)
  grid->CreateGrid(100, 10);
  // We can set the sizes of individual rows and columns
  // in pixels
  // grid->SetRowSize(0, 60);
  grid->SetColSize(0, 120);
  // Colours can be specified for grid cell contents
  int row = 0;
  int col;
  for (unsigned i = 0; i < power(2,MyFSM::nInputs); i++) {
      std::ostringstream o;
			o << std::bitset<MyFSM::nInputs>(i);
      grid->SetCellValue(row, 1 + i, o.str());
      grid->SetReadOnly(row, 1 + i);
      grid->SetCellTextColour(row, 1 + i, *wxRED);
      grid->SetCellBackgroundColour(row, 1 + i, *wxLIGHT_GREY);
  }
  row++;
  for (auto &keyval : fsm->_stateTable) {
    col = 0;
    grid->SetCellValue(row, col, keyval.first);
    grid->SetReadOnly(row, col);
    grid->SetCellTextColour(row, col, *wxRED);
    grid->SetCellBackgroundColour(row, col, *wxLIGHT_GREY);

    col++;
    for (auto &cell : keyval.second) {
      grid->SetCellValue(row, col, cell);
      grid->SetReadOnly(row, col);
      col++;
    }
    row++;
  }

  unsigned rowOffset = MyFSM::nStates + 6;

  row = rowOffset - 1;

  DrawTableHeaderAndGutter(row);
  row++;

  unsigned colShift = 1;

  col = MyFSM::nStates;

  auto state = fsm->_stateTable.begin()->first;
  auto func = [&](MyFSM::StateCombo sc) {

    if (sc->first.first != state) {
      state = sc->first.first;
      row++;
      colShift++;
      col = MyFSM::nStates - colShift;
    } else {
      col--;
    }

    const auto val = sc->second.second;

    string s;
    if (sc->second.first == MyFSM::Compatibility::OK) {
      s = "V";
      cout << "v mama" << endl;
    } else if (sc->second.first == MyFSM::Compatibility::MAYBE_OK) {
      s = implode(val);
    } else {
      s = "X";
      cout << "x mama" << endl;
    }
    grid->SetCellValue(row, col, s);
    grid->SetReadOnly(row, col);
  };

  fsm->processStates(func);

  fsm->resolveDependencies();
  row += 3;
  DrawTableHeaderAndGutter(row);
  col = MyFSM::nStates;
  colShift = 1;
  row++;
  state = fsm->_stateTable.begin()->first;
  fsm->processStates(func);
  testmenu->Append(wxID_ANY, _("New\tCtrl-N"));
  testmenu->Append(wxID_ANY, _("Open\tCtrl-O"));

  testmenu->Append(wxID_ABOUT, _("About"));
  testmenu->Append(wxID_HELP, _("Help"));
  testmenu->Append(wxID_PREFERENCES, _("Preferences"));
  testmenu->Append(wxID_EXIT, _("Exit"));

  menubar->Append(testmenu, _("File"));

  frame->SetMenuBar(menubar);
  frame->Show();

  return true;
}

void MyApp::OnAbout(wxCommandEvent &evt) {
  wxMessageBox(_("About this application... not much to say for such a simple "
                 "example, really"));
}

void MyApp::OnPrefs(wxCommandEvent &evt) {
  wxMessageBox(_(
      "Here are preferences. Not much to configure in this simple example =)"));
}

void MyApp::DrawTableHeaderAndGutter(unsigned int row) {
  auto iter = fsm->_stateTable.begin();
  iter++;
  unsigned colOffset = 1;
  int col;
  for (col = MyFSM::nStates - 1; col >= colOffset; col--) {
    grid->SetCellValue(row, col, iter->first);
    grid->SetCellTextColour(row, col, *wxRED);
    grid->SetCellBackgroundColour(row, col, *wxLIGHT_GREY);
    iter++;
  }
  row++;
  iter = fsm->_stateTable.begin();
  col = 0;
  for (unsigned i = 0; i < MyFSM::nStates - 1; i++) {
    grid->SetCellValue(row + i, col, iter->first);
    grid->SetCellTextColour(row + i, col, *wxRED);
    grid->SetCellBackgroundColour(row + i, col, *wxLIGHT_GREY);
    iter++;
  }
}
