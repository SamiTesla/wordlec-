#ifndef APPLICATION_H
#define APPLICATION_H
#include <bobcat_ui/bobcat_ui.h>
#include <bobcat_ui/window.h>
#include <bobcat_ui/button.h>
#include <bobcat_ui/textbox.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include<random>

namespace bobcat {
class Application : public Application_ {
Window* window;
 Button* gamebutton; 
 Button* instructionsbutton;
 Button* statisticsbutton;
 Button* MainMenuPageButton; 
 Button* resetStatsButton;
 TextBox* heading;
 TextBox* statText = NULL;
 TextBox* instructionsTextBox; 
 std::vector<Button*> gameKeyboard;
 std::vector<std::vector<TextBox*>> wordleTiles;
 Button* deleteButton; 
 Button* enterButton; 
 int userTry; 
 std::string userGuess; 
 std::string selectedWord;
 std::string filename; 
 std::string currentMenu; 
 std::string winLossStatus; 

 void initializeScreens() {
     heading = new TextBox(10, 40, 380, 25, "Wordle");
     heading->align(FL_ALIGN_CENTER);
     heading->labelsize(20);
     heading->labelfont(FL_BOLD);
     gamebutton = new Button(20, 100, 360, 30, "Play Game");
     instructionsbutton = new Button(20,200, 360, 30, "View Instructions");
     statisticsbutton = new Button(20, 150, 360, 30, "View Statistics");
     ON_CLICK(gamebutton, navigationClickControl);
     ON_CLICK(instructionsbutton, navigationClickControl);
     ON_CLICK(statisticsbutton, navigationClickControl);
     instructionsTextBox = new TextBox(20, 100, 360, 200, "The Wordle Player Has 5 Tries To Guess The Correct Word\nEach Guess Must Be A Valid 5-Letter Word.\nThe Color of the Tiles Change To Show How Close You Are.\n\nGreen: Letter Is In The Correct spot.\nYellow: Letter Is In The Word But In The Wrong spot.\nRed: Letter Is Not In Word.\n\n Once You Fill In The Slots For Words\n The Game Will Tell You What The Word Is \n Along With If You Are Right Or Wrong.\n\n Have Fun! \n :)\n\n\n HINT: This Is Computer Science/Engineering Themed!");
     instructionsTextBox->align(FL_ALIGN_CENTER | FL_ALIGN_TOP);
     instructionsTextBox->labelsize(12);
     resetStatsButton = new Button(280, 10, 100, 30, "Reset Stats");
     ON_CLICK(resetStatsButton, handleResetStatsClick);
 }

 void initializeWordleGame() {
 int gridSize = 35, spacing = 5;
 int totalGridHeight = 5 * (gridSize + spacing);
 int offsetY = (window->h() - totalGridHeight) / 2 - 50;
 for (int i = 0; i < 5; i++) {
     std::vector<TextBox*> row;
     for (int j = 0; j < 5; j++) {
           int x = (window->w() - (5 * (gridSize + spacing))) / 2 + j * (gridSize + spacing);
           int y = (offsetY + i * (gridSize + spacing)) -20;
            auto* box = new TextBox(x, y, gridSize, gridSize, "_");
           box->labelsize(18);
           box->align(FL_ALIGN_CENTER);
            box->box(FL_FLAT_BOX);
           row.push_back(box);
     }
       wordleTiles.push_back(row);
 }
 int yOffset = 300;
 intializeWordleKeyboard(yOffset);
 int buttonWidth = 100;
 int buttonHeight = 30;
 int yButtons = yOffset - 40;
 int xBackspace = (window->w() / 2) - 70 - 60;
 int xEnter = (window->w() / 2) + 70;         
 int xMainMenu = (xBackspace + xEnter + 60) / 2 - (buttonWidth / 2);
 MainMenuPageButton = new Button(xMainMenu, yButtons, buttonWidth, buttonHeight, "Main Menu");
 ON_CLICK(MainMenuPageButton, navigationClickControl);
}

 void intializeWordleKeyboard(int y) {
 std::vector<std::string> row1 = {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"};
 std::vector<std::string> row2 = {"A", "S", "D", "F", "G", "H", "J", "K", "L"};
 std::vector<std::string> row3 = {"Z", "X", "C", "V", "B", "N", "M"};
 intializeWordleKeyboardRow(row1, y);
 intializeWordleKeyboardRow(row2, y + 40);
 intializeWordleKeyboardRow(row3, y + 80);
 int buttonWidth = 60;
 int buttonHeight = 30;
 int spacing = 10;
 int yButtons = y-40;
 int xBackspace = (window->w() / 2) - 70 - buttonWidth;
 int xEnter = (window->w() / 2) + 70;                
 deleteButton = new Button(xBackspace, yButtons -20, buttonWidth, buttonHeight, "⌦");
 enterButton = new Button(xEnter, yButtons - 20, buttonWidth, buttonHeight, "OK");
 ON_CLICK(deleteButton, backspaceControl);
 ON_CLICK(enterButton, enterControl);
}

 void intializeWordleKeyboardRow(const std::vector<std::string>& keys, int y) {
   int buttonSize = 28;
   int spacing = 4;    
   int startX = (window->w() - (keys.size() * (buttonSize + spacing))) / 2;
   for (const auto& key : keys) {
       auto* button = new Button(startX, y - 20, buttonSize, buttonSize, key);
       gameKeyboard.push_back(button);
       ON_CLICK(button, keyboardClickControl);
       startX += buttonSize + spacing;
   }
}


void navigationClickControl(Widget* sender) {
 if (sender == gamebutton) {
     displayCurrentMenu("game");
 } else if (sender == instructionsbutton) {
     displayCurrentMenu("instructions");
 } else if (sender == statisticsbutton) {
     displayCurrentMenu("stats");
 } else if (sender == MainMenuPageButton) {
     resetWordle();
     displayCurrentMenu("menu");
 }
}


 void keyboardClickControl(Widget* sender) {
     if (userGuess.size() < 5) {
         userGuess += sender->label();
         int pos = userGuess.size() - 1;
         wordleTiles[userTry][pos]->label(sender->label());
     }
 }

 void backspaceControl(Widget* sender) {
     if (!userGuess.empty()) {
         int pos = userGuess.size() - 1;
         wordleTiles[userTry][pos]->label("_");
         userGuess.pop_back();
     }
 }


void enterControl(Widget* sender) {
   if (userGuess.size() == 5) {
       if(checkIfWordAllowed()){
       std::vector<char> targetWordLetters(selectedWord.begin(), selectedWord.end());
       std::vector<bool> correctFlags(5, false);


       for (int i = 0; i < 5; i++) {
           if (userGuess[i] == selectedWord[i]) {
               wordleTiles[userTry][i]->color(FL_GREEN);
               correctFlags[i] = true;
               targetWordLetters[i] = '_';
           } else {
               wordleTiles[userTry][i]->color(FL_BACKGROUND_COLOR);
           }
       }


       for (int i = 0; i < 5; i++) {
           if (!correctFlags[i] && std::find(targetWordLetters.begin(), targetWordLetters.end(), userGuess[i]) != targetWordLetters.end()) {
               wordleTiles[userTry][i]->color(FL_YELLOW);  // Color yellow for misplaced
               targetWordLetters[std::find(targetWordLetters.begin(), targetWordLetters.end(), userGuess[i]) - targetWordLetters.begin()] = '_';
           }
       }


       for (int i = 0; i < 5; i++) {
           if (!correctFlags[i] && wordleTiles[userTry][i]->color() == FL_BACKGROUND_COLOR) {
               wordleTiles[userTry][i]->color(FL_RED);
           }
       }


       window->redraw();
       Fl::wait(1);
       if (userGuess == selectedWord) {
           displayEnd("Congratulations! You've guessed the correct word");
       } else if (++userTry == 5) {
           displayEnd("Game Over! The word was: " + selectedWord);
       } else {
           userGuess.clear();
       }
      window->redraw();
       }
       else{
           showMessage("This word: " + userGuess + " is not real or is not in the wordbank");
           for(int i = 0; i < 5; i++){
               wordleTiles[userTry][i]->label("_");
           }
           userGuess.clear();
       }
     
   }
}
 void displayCurrentMenu(const std::string& view) {
     currentMenu = view;
     updateMenu();
 }
void updateMenu() {
   hideElements();
   if (currentMenu == "menu") {
       heading->label("WORDLE");
       heading->show();
       gamebutton->show();
       instructionsbutton->show();
       statisticsbutton->show();
   } else if (currentMenu == "instructions") {
       heading->label("Instructions");
       heading->show();
       instructionsTextBox->show();
       MainMenuPageButton->show();
       MainMenuPageButton->position(150, 360);
   } else if (currentMenu == "game") {
       for (auto& row : wordleTiles) {
           for (auto* box : row) box->show();
       }
       for (auto* button : gameKeyboard) button->show();
       deleteButton->show();
       enterButton->show();
       MainMenuPageButton->show();
       selectedWord = generateWord();
       MainMenuPageButton->position((((window->w() / 2) - 70 - 60) + ((window->w() / 2) + 70) + 60) / 2 - (100 / 2), 240);
   } else if (currentMenu == "stats") {
       heading->label("Statistics");
       heading->show();
       displayStatistics();
       MainMenuPageButton->position(10, 10);
       MainMenuPageButton->show();
       resetStatsButton->show();
   }
}
 void hideElements() {
     heading->hide();
     gamebutton->hide();
     instructionsbutton->hide();
     statisticsbutton->hide();
     instructionsTextBox->hide();
     for (auto& row : wordleTiles) {
         for (auto* box : row) box->hide();
     }
     for (auto* button : gameKeyboard) button->hide();
     deleteButton->hide();
     enterButton->hide();
     MainMenuPageButton->hide();
     if (statText != NULL) {
       statText->hide();
     }
     resetStatsButton->hide();
 }

 void displayEnd(const std::string& message) {
   std::string attemptMessage = "You finished this round in " + std::to_string(userTry+1) + " tries.";
   if (userGuess == selectedWord) {
           winLossStatus = "Win";
       } else {
           winLossStatus = "Loss";
           attemptMessage+="\nThe Correct Word Was:" +selectedWord;
       }
   fl_message("%s\n%s", message.c_str(), attemptMessage.c_str());


     recordStatistics();
   userTry = 0;
   userGuess.clear();
   for (auto& row : wordleTiles) {
       for (auto* box : row) {
           box->label("_");
           box->color(FL_BACKGROUND_COLOR); 
       }
   }
   displayCurrentMenu("menu");
}
void resetWordle() {
 userTry = 0;
 userGuess.clear();
  for (auto& row : wordleTiles) {
     for (auto* box : row) {
         box->label("_");
         box->color(FL_BACKGROUND_COLOR);
     }
 }
 for (auto* button : gameKeyboard) {
     button->color(FL_BACKGROUND_COLOR);
 }
 userGuess.clear();
}

bool checkIfWordAllowed(){
       std::fstream file(filename);
       std::string line;


       std::string lowerCaseCurrentGuess;
       for(int i = 0; i < 5;i++){
           lowerCaseCurrentGuess += tolower(userGuess[i]);
       }


       while(getline(file, line)){
           if(line == lowerCaseCurrentGuess){
               return true;
           };
       }


       return false;
   };
   std::string generateWord(){
       std::fstream file(filename);
       std::vector<std::string> words;
       std::string line;
       while(getline(file,line)){
        words.push_back(line);
       }
       std::random_device rd;
       std::mt19937 gen(rd());
       std::uniform_int_distribution<> distrib(0,words.size()-1);
       std::string newTargetWord=words[distrib(gen)];
       std::string fixedTargetWord;


       for(char c:newTargetWord){
           fixedTargetWord += toupper(c);
       }
     
    
       return fixedTargetWord;
   }
   void recordStatistics() {
       std::fstream file("stats.csv", std::ios::app);
       if (file.is_open()) {
           file << selectedWord << "," << (userTry) << "," << winLossStatus << "\n";
           file.close();
       } else {
           fl_alert("Error: Could not open stats.csv for writing.");
       }
   }
   void displayStatistics() {
   std::ifstream csvFile("stats.csv");
   if (!csvFile.is_open()) {
       fl_message("Error: Unable to open statistics file.");
       return;
   }


   std::string line;
   int yOffset = 120;
   int maxLines = 10; 


   while (std::getline(csvFile, line) && maxLines-- > 0) {
       size_t pos1 = line.find(',');
       size_t pos2 = line.find(',', pos1 + 1);


       std::string word = line.substr(0, pos1);
       int attempts = std::stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
       std::string result = line.substr(pos2 + 1);
       std::string statLine = "Word: " + word + ", Attempts: " + std::to_string(attempts) + ", Result: " + result;
       statText = new TextBox(20, yOffset, 360, 25, statLine.c_str());
       statText->labelsize(12);
       statText->align(FL_ALIGN_LEFT);
       window->add(statText);
       statText->show();
       yOffset += 30;
   }


   csvFile.close();
}

void resetStatistics() {
   std::ofstream file("stats.csv", std::ios::trunc);
   if (file.is_open()) {
       file.close();
   } else {
       fl_alert("Error: Could not reset stats.csv.");
   }
}

void handleResetStatsClick(Widget* sender) {
   resetStatistics();
   displayCurrentMenu("stats");
}


public:
 Application() {
   resetStatistics();
     theme();
     window = new Window(100, 100, 400, 400, "Wordle!");
     initializeScreens();
     initializeWordleGame();
     filename = "./wordbank.txt";
     userTry = 0;
     userGuess = "";
     displayCurrentMenu("menu");
     window->show();
 }
};
}
#endif
