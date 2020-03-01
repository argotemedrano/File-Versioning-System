#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include "gitint.h"

// My includes

//using namespace std;

/*********************** Messages to use for errors ***************************/
const std::string INVALID_COMMAND = "Invalid command";
const std::string INVALID_OPTION = "Invalid option";
const std::string INVALID_COMMIT_NUMBER = "Invalid commit number";
const std::string LOG_COMMIT_STARTER = "Commit: ";

// Class implementation
GitInt::GitInt() : head_(0) {
    // Dummy Entry
    commits_.emplace_back("", std::map<std::string, int>(), -1);
}

void GitInt::create(const std::string& filename, int value) {
    // Check that file doesn't already exist
    if (current_files.find(filename) != current_files.end()) {
        throw std::invalid_argument("File already exists.");
    }

    // Create file
    current_files.insert(make_pair(filename, value));
}

void GitInt::edit(const std::string& filename, int value) {
    // Check that file already exists
    if (current_files.find(filename) == current_files.end()) {
        throw std::invalid_argument("File does not exist.");
    }

    // Edit file
    current_files[filename] = value;
}

void GitInt::display_all() const {
    display_helper(current_files);
}

void GitInt::display(const std::string& filename) const {
    // If filename is given, check that it exists
    std::map<std::string, int>::const_iterator file_to_print = current_files.find(filename);
    if (file_to_print == current_files.end()) {
        throw std::invalid_argument("File does not exist.");
    }
    std::cout << file_to_print->first << " : " << file_to_print->second << std::endl;
}

void GitInt::display_commit(CommitIdx commit) const
{
    if (false == valid_commit(commit)) {
        throw std::invalid_argument(INVALID_COMMIT_NUMBER);
    }
    display_helper(commits_[(unsigned int)commit].diffs_);
}

void GitInt::add(std::string filename) {
    std::map<std::string, int>::iterator file = 
        current_files.find(filename); 

    // File not found: Throws exception
    if (file == current_files.end()) {
        throw std::invalid_argument("File does not exist.");
    }
    else {
        // Add file to stage it
        staged_.insert(filename);
    }
}

void GitInt::commit(std::string message) {
    // IS IT GUARANTEED THAT THERE IS THERE IS AT LEAST ONE CHARACTER?
    // Check for quotations "[message]"

    // No files are staged
    if (staged_.empty()) {
        throw std::runtime_error("No files are staged.");
    }

    // Start committing files

    std::set<std::string>::iterator staged_itr; 
    std::map<std::string, int> differences;

    for (staged_itr = staged_.begin(); staged_itr != staged_.end(); ++staged_itr) {
        std::map<std::string, int>::const_iterator current_file_itr = current_files.find(*staged_itr);

        // Go through previous commits
        int summed_diff = current_file_itr->second;
        CommitIdx i = head_;

        while (i != 0) {
            std::map<std::string, int>::const_iterator diff_itr = commits_[static_cast<unsigned int>(i)].diffs_.find(*staged_itr);
            if (diff_itr != commits_[(unsigned int)i].diffs_.end()) {
                summed_diff -= diff_itr->second;
            }
            i = commits_[(unsigned int)i].parent_;
        }
       
        differences.insert(std::pair<std::string, int>(*staged_itr, summed_diff));
    }

    // No more staged files
    staged_.clear();

    // Place new commit
    commits_.emplace_back(message, differences, head_);
      
    // Change checked out commit
    head_ = commits_.size() - 1;
}

void GitInt::create_tag(const std::string& tagname, CommitIdx commit) {
    // Tag name already exists
    if (tags_.find(tagname) != tags_.end()) {
        throw std::invalid_argument("tag-name already exists.");
    }

    // Create tag
    tags_.insert(std::pair<std::string, int>(tagname, commit));
    ordered_tags_.emplace_back(tagname);
}

void GitInt::tags() const {
    // Print tags in reverse order of being added
    for (int i = (int)ordered_tags_.size() - 1; i >= 0; --i) {
        std::cout << ordered_tags_[(unsigned int)i] << std::endl;
    }
}

bool GitInt::checkout(CommitIdx commitIndex) {
    // Invalid commit number. 0 is a valid commit #.
    if (!valid_commit(commitIndex)) {
        throw std::invalid_argument("Invalid commit number.");
    }

    // Clear files and staged files
    current_files.clear();
    staged_.clear();

    // New checkoued commit
    head_ = commitIndex;

    // Iterate through each parent
    CommitIdx i = commitIndex;
    while (i != 0) {
        // At each parent, add up differences for all files
        for (std::map<std::string, int>::iterator itr = commits_[(unsigned int)i].diffs_.begin(); 
            itr != commits_[(unsigned int)i].diffs_.end(); ++itr) {
                
            // Add to current_files
            if (current_files.find(itr->first) == current_files.end()) {
                current_files.insert(*itr);
            } 
            // Add difference
            else {
                current_files[itr->first] += itr->second;
            }

        }
        i = commits_[(unsigned int)i].parent_;
    }
    return true;
}

bool GitInt::checkout(std::string tag) {
    // Tag does not exist
    std::map<std::string, int>::iterator tag_itr;
    tag_itr = tags_.find(tag);
    if (tag_itr == tags_.end()) {
        throw std::invalid_argument("Invalid tag.");
    }

    // Checking out to existing tag
    CommitIdx commit_index = tag_itr->second;
    checkout(commit_index);
    return true;
}

void GitInt::log() const {
    // Iterate through commits backwards
    CommitIdx i = head_;
    while (i != 0) {
        log_helper(i, commits_[(unsigned int)i].msg_);
        i = commits_[(unsigned int)i].parent_;
    }

}

void GitInt::diff(CommitIdx to) const {
    // Not a valid commit number, 0 is considered valid
    if (!valid_commit(to)) {
        throw std::invalid_argument("Invalid commit number.");
    }

    // Task 1: Get original file states for commit comparing file state to
    std::map<std::string, int> commit_file_values = buildState(to, 0);

    // Task 2: Calculate difference between current file states and commit file state
    std::map<std::string,int>::const_iterator current_files_itr = current_files.begin();
    for (; current_files_itr != current_files.end(); ++current_files_itr) {
        std::map<std::string, int>::iterator commit_file_itr = commit_file_values.find(current_files_itr->first);

        // File not in previous commit
        if (commit_file_itr == commit_file_values.end()) {
            std::cout << current_files_itr->first << " : " << current_files_itr->second << std::endl;
        }
        // File exists in previous commit
        // File has different value
        else if (commit_file_itr->second != current_files_itr->second) {
            // Print difference
            int difference = current_files_itr->second - commit_file_itr->second;
            std::cout << current_files_itr->first << " : " << difference << std::endl;
        }
        // File values are the same, no nothing
    }
}

void GitInt::diff(CommitIdx from, CommitIdx to) const {
    // Check correct commit numbers provided
    if (!valid_commit(from) || !valid_commit(to) || from < to) {
        throw std::invalid_argument("Invalid commit number(s).");
    }

    // Task 1: Original file states for Commit from (most recent)
    std::map<std::string, int> recent_commit =  buildState(from);

    // Task 2: Original file states for Commit to (earlier)
    std::map<std::string, int> earlier_commit = buildState(to);  

    // Task 3: Calculate difference between from and to
    std::map<std::string,int>::const_iterator recent_itr = recent_commit.begin();
    for (; recent_itr != recent_commit.end(); ++recent_itr) {
        std::map<std::string, int>::iterator commit_file_itr = earlier_commit.find(recent_itr->first);

        // File not in previous commit
        if (commit_file_itr == earlier_commit.end()) {
            std::cout << recent_itr->first << " : " << recent_itr->second << std::endl;
        }
        // File exists in previous commit
        // File has different value
        else if (commit_file_itr->second != recent_itr->second) {
            // Print difference
            int difference = recent_itr->second - commit_file_itr->second;
            std::cout << recent_itr->first << " : " << difference << std::endl;
        }
        // File values are the same
        
    }

}

// Valid commit range: [0, # of commits]
bool GitInt::valid_commit(CommitIdx commit) const {
    if (commit >= (int)commits_.size() || commit < 0) {
        return false;
    }

    return true;
}

// Rebuilds file states at a particular commit
std::map<std::string, int> GitInt::buildState(CommitIdx from, CommitIdx to /*= 0*/) const {
    // Check correct commit numbers provided
    if (from >= (int)commits_.size() || to >= (int)commits_.size()
            || from < to || from < 0 || to < 0) {
        throw std::invalid_argument("Invalid commit number(s).");
    }

    std::map<std::string, int> new_state;
    CommitIdx i = from;
    
    while (i != to) {
        // At each parent, add up differences for all files
        for (std::map<std::string, int>::const_iterator itr = commits_[(unsigned int)i].diffs_.begin(); 
            itr != commits_[(unsigned int)i].diffs_.end(); ++itr) {
                
            // Add to current_files
            if (new_state.find(itr->first) == new_state.end()) {
                new_state.insert(*itr);
            } 
            // Add difference
            else {
                new_state[itr->first] += itr->second;
            }

        }
        i = commits_[(unsigned int)i].parent_;
    }

    return new_state;
}


void GitInt::print_menu() const
{
    std::cout << "Menu:                          " << std::endl;
    std::cout << "===============================" << std::endl;
    std::cout << "create   filename int-value    " << std::endl;
    std::cout << "edit     filename int-value    " << std::endl;
    std::cout << "display  (filename)            " << std::endl;
    std::cout << "display  commit-num            " << std::endl;
    std::cout << "add      file1 (file2 ...)     " << std::endl;
    std::cout << "commit   \"log-message\"       " << std::endl;
    std::cout << "tag      (-a tag-name)         " << std::endl;
    std::cout << "log                            " << std::endl;
    std::cout << "checkout commit-num/tag-name   " << std::endl;
    std::cout << "diff                           " << std::endl;
    std::cout << "diff     commit                " << std::endl;
    std::cout << "diff     commit-n commit-m     " << std::endl;
}


bool GitInt::process_command(std::string cmd_line)
{
    bool quit = false;
    std::stringstream ss(cmd_line);
    std::string cmd;
    ss >> cmd;
    if (ss.fail()) throw std::runtime_error(INVALID_COMMAND);

    if (cmd == "quit") {
        quit = true;
    }

    // Continue checking/processing commands
    else if (cmd == "create") {
        // Check that filename and int is given
        std::string filename;
        int integer;

        ss >> filename >> integer;
        
        // No file name or value provided
        if (ss.fail()) {
            throw std::runtime_error("Invalid amount of arguments: filename and value must be provided.");
        }

        // Create file
        create(filename, integer);
    }
    else if (cmd == "edit") {
        // Check that filename and int is given
        std::string filename;
        int integer;

        ss >> filename >> integer;
        
        // No file name or value provided
        if (ss.fail()) {
            throw std::runtime_error("Invalid amount of arguments: filename and value must be provided.");
        }

        // Edit prospective file
        edit(filename, integer);
    }
    else if (cmd == "display") {
        std::string filename;
        int commit_number;

        std::stringstream second_ss(cmd_line);
        second_ss >> filename; // Dummy read
        
        // Parse through display command parameters
        ss >> filename;
        second_ss >> commit_number;

        // Neither file name nor commit number is provided
        if (ss.fail() && second_ss.fail()) { 
            display_all();
        }

        // Only filename is provided
        else if (!ss.fail() && second_ss.fail()) {
            display(filename);
        }
        // Only commit number is provided
        else if (!ss.fail() && !second_ss.fail()) {
            display_commit(commit_number);
        }        
    }
    else if (cmd == "add") {
        std::string file_name;

        while (ss >> file_name) {
            add(file_name);
        }
    }
    else if (cmd == "commit") {
        char c;
        ss >> c;

        if (ss.fail()) {
            throw std::runtime_error("No message provided");
        }

        std::string commit_message;
        getline(ss, commit_message);

        if (ss.fail()) {
            throw std::runtime_error("No message provided");
        }

        commit_message = c + commit_message;
        if (commit_message.size() <= 2) {
            throw std::runtime_error("No message provided.");
        }

        bool has_begin_quotation = (c == '"');
        bool has_end_quotation = false;


        // CHECK -- Search for second end quotation
        for (int i = 1; i < (int)commit_message.size(); ++i) {
            // First occurance of potential end quotation found
            if (commit_message[(unsigned int)i] == '"') {
                has_end_quotation = true;
                
                // Create substring of message without quotations
                commit_message = commit_message.substr(1,(unsigned int)i-1);
                break;
            }
        }

        // Correct message format
        if (has_begin_quotation && has_end_quotation) {
            commit(commit_message);
        }
        else {
            throw std::runtime_error("Improper message format.");
        }
    }
    else if (cmd == "tag") {
        std::string new_tag;
        std::string tag_flag;

        ss >> tag_flag;

        // No other parameters provided: Print all tags
        if (ss.fail()) {
            tags();
        } 
        // Other parameters are provided
        else {
            // Incorrect flag
            if (tag_flag != "-a") {
                throw std::runtime_error("-a flag not provided.");
            }

            ss >> new_tag;

            // No tag-name provided
            if (ss.fail()) {
                throw std::runtime_error("No tag name provided.");
            }
            
            // Create new tag
            create_tag(new_tag, head_);
        }
    }
    else if (cmd == "log") {
        log();
    }
    else if (cmd == "checkout") {
        int checkout_id;
        std::string checkout_tag;

        std::stringstream second_ss(cmd_line);
        second_ss >> checkout_tag; // Dummy read

        ss >> checkout_id;
        second_ss >> checkout_tag;

        // No commit number of tag provided
        // MAKE SURE TO CHECK NO TAG IS PROVIDED EITHER
        if (second_ss.fail() && ss.fail()) {
            throw std::runtime_error("No commit number or tag provided.");
        }

        // Commit # given priority
        if (!ss.fail()) {
            checkout(checkout_id);
        } 
        // Tag provided
        else {
            checkout(checkout_tag);
        }
    
    }
    else if (cmd == "diff") {
        CommitIdx commit_n, commit_m;
        ss >> commit_n;

        // Print any differences between the currently modified 
        // file state and the HEAD (last checked-out) commit
        if (ss.fail()) {
            diff(head_);
            return quit;
        }

        ss >> commit_m;

        // Differences between the currently modified file state and 
        // the given commit-num walking the parent/ancestor chain to determine those differences.
        if (ss.fail()) {
            // Not a valid commit number - what about 0? negative?
            if (commit_n >= (int)commits_.size() || commit_n < 0) {
                throw std::invalid_argument("Invalid commit number.");
            }
            diff(commit_n);
            return quit;
        }

        // Not valid commit numbers or n < m
        if (commit_n >= (int)commits_.size() || commit_m >= (int)commits_.size()
            || commit_n < commit_m || commit_n < 0 || commit_m < 0) {
                throw std::invalid_argument("Invalid commit number(s).");
            }

        // Differences between two different commits
        diff(commit_n, commit_m);
    } 
    else {
        throw std::runtime_error(INVALID_COMMAND);
    }

    return quit;
}

// ============== GIVEN ===============

void GitInt::display_helper(const std::map<std::string, int>& dat) const
{
    for (std::map<std::string, int>::const_iterator cit = dat.begin();
            cit != dat.end();
            ++cit) {
        std::cout << cit->first << " : " << cit->second << std::endl;
    }
}


void GitInt::log_helper(CommitIdx commit_num, const std::string& log_message) const
{
    std::cout << LOG_COMMIT_STARTER << commit_num << std::endl;
    std::cout << log_message << std::endl << std::endl;
}

