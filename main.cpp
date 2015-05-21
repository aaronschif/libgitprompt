#include <git2.h>
#include <iostream>
#include <boost/filesystem.hpp>

#define ICON_NEW "%"
#define ICON_INDEX "+"
#define ICON_WORKING "*"
#define ICON_AHEAD "▲"
#define ICON_BEHIND "▼"
#define ICON_STASH "/"
#define ICON_STATE "|"
#define ICON_NOBRANCH "#"

using namespace boost::filesystem;
using namespace std;

struct GitStatus{
    size_t ahead = 0;
    size_t behind = 0;
    int new_files = 0;
    int working_files = 0;
    int index_files = 0;
    int stash = 0;
    int state = 0;

    bool empty = 0; // TODO Is this diffrent from null?

    const char *branch = NULL;
};

#include "format.cpp"

void print_debug(GitStatus *status) {
    cout << "Index files:   " << status->index_files << '\n';
    cout << "New files:     " << status->new_files << '\n';
    cout << "Working Files: " << status->working_files << '\n';
    cout << "Branch:        " << status->branch << '\n';
    cout << "Ahead:         " << status->ahead << '\n';
    cout << "Behind:        " << status->behind << '\n';
    cout << "Stash:         " << status->stash << '\n';
    cout << "State:         " << status->state <<'\n';
}

void get_branch(git_repository *repo, GitStatus *status) {
    if (git_repository_is_empty(repo)) {
        status->empty = true;
        return;
    }

    git_reference *head = NULL;
    int error = git_repository_head(&head, repo);
    status->branch = git_reference_shorthand(head);
}

void get_repo_state(git_repository *repo, GitStatus *status) {
    status->state = git_repository_state(repo);
}

int _stash_cb(size_t index, const char *message, const git_oid *stash_id, void *payload) {
    (*((int*) payload)) ++;
    return 0;
}

void get_stash_state(git_repository *repo, GitStatus *status) {
    int payload = 0;
    git_stash_foreach(repo, _stash_cb, &payload);
    status->stash = payload;
}

void get_remote_diffs(git_repository *repo, GitStatus *status) {
    const git_oid *local_oid, *remote_oid;
    int error;
    git_reference *head, *remote;

    if (git_repository_head(&head, repo)) return;
    if (git_branch_upstream(&remote, head)) return;
    local_oid = git_reference_target(head);
    remote_oid = git_reference_target(remote);

    git_graph_ahead_behind(&status->ahead, &status->behind, repo, local_oid, remote_oid);
}

void get_local_changes(git_repository *repo, GitStatus *status) {
      git_status_options opts = GIT_STATUS_OPTIONS_INIT;
      opts.show  = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
      opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED |
        GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX |
        GIT_STATUS_OPT_SORT_CASE_SENSITIVELY;

      git_status_list *status_list;
      git_status_list_new(&status_list, repo, &opts);

      for (int i = 0; i < git_status_list_entrycount(status_list); ++i) {
          const git_status_entry *s = git_status_byindex(status_list, i);
          if (s->status == GIT_STATUS_CURRENT)
              continue;
          if (s->status & (GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_INDEX_DELETED | GIT_STATUS_INDEX_RENAMED | GIT_STATUS_INDEX_TYPECHANGE))
              status->index_files++;
          if (s->status & (GIT_STATUS_WT_MODIFIED | GIT_STATUS_WT_DELETED | GIT_STATUS_WT_TYPECHANGE | GIT_STATUS_WT_RENAMED))
              status->working_files++;
          if (s->status & (GIT_STATUS_WT_NEW))
              status->new_files++;
      }
}

int main(int argc, char *argv[]) {
    GitStatus status;
    int error = 0;

    path cwd = current_path();

    do {
        if(is_directory(cwd/".git"))
            break;
        cwd = cwd.parent_path();
    } while (!cwd.empty());

    git_repository *repo = NULL;
    error = git_repository_open(&repo, cwd.c_str());
    if (error)
        return 0;
    get_branch(repo, &status);
    get_local_changes(repo, &status);
    get_repo_state(repo, &status);
    get_remote_diffs(repo, &status);
    get_stash_state(repo, &status);

    get_config(repo, &status);
}
