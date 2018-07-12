#!/usr/bin/env python

import subprocess

def print_log(full_log):
    full_log_lines = full_log.split("\n")

    commits = []

    commit_bit = "commit "
    author_bit = "Author: "
    date_bit = "Date: "

    commit = None
    for line in full_log_lines:

        if line[0:len(commit_bit)] == commit_bit:
            if commit:
                commits.append(commit)
            commit = {'text':''}
        handled = False
        for bit in [commit_bit, author_bit, date_bit]:
            if line[0:len(bit)] == bit:
                commit[bit] = line
                handled = True
        if not handled:
            commit['text'] += line

    commits.append(commit)

    contributions = []

    for commit in commits:
        if not "git@kismith.co.uk" in commit[author_bit] and not "git@el-tramo.be" in commit[author_bit]:
            contributions.append(commit)

    #print contributions
    contributors = {}
    for commit in contributions:
        if not commit[author_bit] in contributors:
            contributors[commit[author_bit]] = []
        contributors[commit[author_bit]].append(commit[commit_bit])

    for contributor in contributors:
        print contributor + " has contributed patches " + ", ".join([commit[len(commit_bit):] for commit in contributors[contributor]])

full_swiften_log = subprocess.check_output(["git", "log", "--", "Swiften"])

print("Contributors for Swiften/ subtree:\n")
print_log(full_swiften_log)

full_all_log = subprocess.check_output(["git", "log"])

print("\n\n\n\n")

print("Contributors for full tree:\n")
print_log(full_all_log)
