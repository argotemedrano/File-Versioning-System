#include <iostream>
#include "gitint.h"
// Add any necessary headers
#include <exception>
using namespace std;

void print_exception_message(const std::string& what_msg);

int main()
{

    GitInt g;
    string cmd_line;
    bool quit = false;
    const string PROMPT_STARTER = "$ ";

    g.print_menu();

    // Process command
    while (!quit) {
      cout << PROMPT_STARTER;
      getline(cin, cmd_line);

      try {
        quit = g.process_command(cmd_line);
      } catch (exception& e) {
        print_exception_message(e.what());
      }
    }

    return 0;
}

void print_exception_message(const std::string& what_msg)
{
  cout << "Error - " << what_msg << endl;
}
