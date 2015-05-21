#include <boost/format.hpp>
#include "cppformat/format.h"
#include "cppformat/format.cc"


void get_config(git_repository *repo, GitStatus *status) {
    git_config *config;
    const char *text_fmt, *stash_fmt, *new_fmt, *working_fmt, *index_fmt, *ahead_fmt, *behind_fmt, *state_fmt;
    string text_text, stash_text, new_text, working_text, index_text, ahead_text, behind_text, state_text;
    int error;

    error = git_repository_config(&config, repo);
    if (git_config_get_string(&text_fmt, config, "libgitprompt.text"))
        text_fmt = "{0}{1}{2}{3}{4}{5}{6}{7}";

    if (git_config_get_string(&working_fmt, config, "libgitprompt.working"))
        working_fmt = "*{}";
    if (working_fmt)
        working_text = status->working_files ? fmt::format(working_fmt, status->working_files) : "";

    if (git_config_get_string(&index_fmt, config, "libgitprompt.index"))
        index_fmt = "+{}";
    if (index_fmt)
        index_text = status->index_files ? fmt::format(index_fmt, status->index_files) : "";

    if (git_config_get_string(&new_fmt, config, "libgitprompt.new"))
        new_fmt = "%{}";
    if (new_fmt)
        new_text = status->new_files ? fmt::format(new_fmt, status->new_files) : "";

    if (git_config_get_string(&state_fmt, config, "libgitprompt.state"))
        state_fmt = "|{}";
    if (state_fmt)
        state_text = status->state ? fmt::format(state_fmt, status->state) : "";

    if (git_config_get_string(&stash_fmt, config, "libgitprompt.stash"))
        stash_fmt = "\\{}";
    if (stash_fmt)
        stash_text = status->stash ? fmt::format(stash_fmt, status->stash) : "";

    if (git_config_get_string(&ahead_fmt, config, "libgitprompt.ahead"))
        ahead_fmt = "▲{}";
    if (ahead_fmt)
        ahead_text = status->ahead ? fmt::format(ahead_fmt, status->ahead) : "";

    if (git_config_get_string(&behind_fmt, config, "libgitprompt.behind"))
        behind_fmt = "▼{}";
    if (behind_fmt)
        behind_text = status->behind ? fmt::format(behind_fmt, status->behind) : "";

    cout << fmt::format(text_fmt, status->branch, working_text, index_text, new_text, state_text, stash_text, ahead_text, behind_text);
}
