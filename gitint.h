#ifndef GITINT_H
#define GITINT_H
#include <iostream>
#include <map>
#include <string>
// Add headers below
#include <utility>
#include <vector>
#include <set>

/**
 * Integral type used to index Commits
 */
typedef int CommitIdx;

/**
 * Container for data pertaining to each commit
 */
struct CommitObj {
    std::string msg_;
    std::map<std::string, int> diffs_;
    CommitIdx parent_;

    /**
     * Completed constructor
     *
     *  @param[in] msg
     *     Log message for this commit.
     *  @param[in] diffs
     *     Map containing the filename (key) and integer *difference*
     *     of the file from the parent commit.
     *  @param[in] parent
     *     Index of the parent commit.
     */
    CommitObj(
        const std::string& msg,
        std::map<std::string, int> diffs,
        CommitIdx parent) :
        msg_(msg), diffs_(diffs), parent_(parent)
    {}
};

/**
 * @brief Class to model 'git' for files containing integers
 */
class GitInt {
public:
    /**
     * Default constructor
     * [TO BE WRITTEN]
     */
    GitInt();

    /**
     * Prints the menu of command options.
     * [COMPLETED]
     */
    void print_menu() const;

    /**
     * Processes/executes a line of input (i.e. one command with its options)
     * [TO BE WRITTEN]
     *
     * @param[in] cmd_line
     *    String containing the entire command line to be processed
     * @returns 'true' if the "quit" command was entered, 'false' otherwise
     * @throws std::invalid_argument or std::runtime_error
     */
    bool process_command(std::string cmd_line);

    /**
     * Creates a new file with the given value
     * [TO BE WRITTEN]
     *
     * @param[in] filename
     *    Name of the file to be created
     * @param[in] value
     *    Integer content of the file
     * @throws std::invalid_argument or std::runtime_error -
     *    See homework writeup for details on error cases
     */
    void create(const std::string& filename, int value);

    /**
     * Modifies the given file to a new value
     * [TO BE WRITTEN]
     *
     * @param[in] filename
     *    Name of the file to be edited
     * @param[in] value
     *    New integer content of the file
     * @throws std::invalid_argument or std::runtime_error -
     *    See homework writeup for details on error cases
     */
    void edit(const std::string& filename, int value);

    /**
     * Displays the current contents of the given file
     * [TO BE WRITTEN]
     *
     * @param[in] filename
     *    Name of the file to be edited
     * @throws std::invalid_argument -
     *    See homework writeup for details on error cases
     */
    void display(const std::string& filename) const;

    /**
     * Displays the current content of all files
     * [TO BE WRITTEN]
     *
     */
    void display_all() const;

    /**
     * Displays the contents of the given commit's diff map
     * [COMPLETED]
     *
     * @param[in] commit
     *    Index of the commit to display
     * @throws std::invalid_argument -
     *    See homework writeup for details on error cases
     */
    void display_commit(CommitIdx commit) const;

    /**
     * Stages the given file for commit
     * [TO BE WRITTEN]
     *
     * @param[in] filename
     *    File to be staged and committed on the next commit command
     * @throws std::invalid_argument or std::runtime_error -
     *    See homework writeup for details on error cases
     */
    void add(std::string filename);


    /**
     * Commits all files staged since the last commit
     * [TO BE WRITTEN]
     *
     * @param[in] message
     *    Log message for this commit
     * @throws std::runtime_error -
     *    See homework writeup for details on error cases
     */
    void commit(std::string message);

    /**
     * Associates a new tag name to the currently checked-out commit
     * [TO BE WRITTEN]
     *
     * @param[in] tagname
     *    Name for this tag
     * @param[in] commit
     *    Commit number/index to associate with the given tagname
     * @throws std::invalid_argument or std::runtime_error -
     *    See homework writeup for details on error cases
     */
    void create_tag(const std::string& tagname, CommitIdx commit);

    /**
     * Displays all tag names in the order they were created from most
     * to least recent.
     * [TO BE WRITTEN]
     *
     */
    void tags() const;

    /**
     * Updates the files and content to match the state of the given commit
     * [TO BE WRITTEN]
     *
     * @param[in] commit
     *    Index of the commit to checkout
     * @throws std::invalid_argument or std::runtime_error -
     *    See homework writeup for details on error cases
     */
    bool checkout(CommitIdx commitIndex);

    /**
     * Updates the files and content to match the state of the given commit
     * [TO BE WRITTEN]
     *
     * @param[in] tag
     *    Tag name associated with the commit to checkout
     * @throws std::invalid_argument or std::runtime_error -
     *    See homework writeup for details on error cases
     */
    bool checkout(std::string tag);

    /**
     * Displays the commit numbers and log message in order from the current
     * checked-out commit back through all parent/ancestor commits.
     * [TO BE WRITTEN]
     *
     * @throws std::invalid_argument or std::runtime_error -
     *    See homework writeup for details on error cases
     */
    void log() const;

    /**
     * Display the file content differences between the current state back
     * through all parent/ancestor commits until the `to` commit
     * [TO BE WRITTEN]
     *
     * @param[in] to
     *    Index of the commit to end the diff process
     * @throws std::invalid_argument -
     *    See homework writeup for details on error cases
     */
    void diff(CommitIdx to) const;

    /**
     * Display the file content differences between commit `from` back
     * through all parent/ancestor commits until the `to` commit
     * [TO BE WRITTEN]
     *
     * @param[in] from
     *    Index of the commit to start the diff process
     * @param[in] to
     *    Index of the commit to end the diff process
     * @throws std::invalid_argument -
     *    See homework writeup for details on error cases
     */
    void diff(CommitIdx from, CommitIdx to) const;

    /**
     *  Returns true if the given input is a valid commit number
     *  [TO BE WRITTEN]
     * @param[in] commit
     */
    bool valid_commit(CommitIdx commit) const;
    
private:

    /**
     * Builds the file state between commit 'from' until 'to' (exclusive)
     * [OPTIONAL - TO BE WRITTEN]
     *
     * @param[in] from
     *    Index of the commit to start the accrual of diffs
     * @param[in] to
     *    Index of the commit to end the accrual of diffs
     * @throws may vary depending on implementation
     */
    std::map<std::string, int> buildState(CommitIdx from, CommitIdx to = 0) const;

    /**
     * Displays a map containing filenames and contents in the desired format
     * [COMPLETED]
     *
     * @param[in] dat
     *    Filename and values map to print
     */
    void display_helper(const std::map<std::string, int>& dat) const;

    /**
     * Displays a single commit in the desired log format
     * [COMPLETED]
     *
     * @param[in] commit_num
     *    Index of the commit to be printed
     * @param[in] msg
     *    Log message of the commit to be printed
     */
    void log_helper(CommitIdx commit_num, const std::string& log_message) const;

    // Current state files
    std::map<std::string, int> current_files;

    // Commits
    std::vector<CommitObj> commits_;

    // Current Checkedout commit
    CommitIdx head_;

    // Staged files
    std::set<std::string> staged_;

    // Tags
    std::map<std::string, int> tags_;
    std::vector<std::string> ordered_tags_;
};

#endif
