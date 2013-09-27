"""
    This experimental module provides classes and functions used to download
    and manage debuginfos via libzypp and zypper.
"""

import os
import subprocess

from reportclient import (_, RETURN_OK, RETURN_FAILURE, ask_yes_no)


def clean_up(*args):
    pass


def build_ids_to_path(pfx, build_ids):
    """
    Transforms build ids into a path.

    build_id1=${build_id:0:2}
    build_id2=${build_id:2}
    file="usr/lib/debug/.build-id/$build_id1/$build_id2.debug"
    """

    return ["%s/usr/lib/debug/.build-id/%s/%s.debug" % (pfx, b_id[:2], b_id[2:]) for b_id in build_ids]


def filter_installed_debuginfos(build_ids, cache_dirs):
    """
    Checks for installed debuginfos.

    Arguments:
        build_ids - string containing build ids
        cache_dirs - list of cache directories

    Returns:
        List of missing debuginfo files.
    """

    def exists_in_system(debuginfo_path):
        return os.path.exists(debuginfo_path)

    def exists_in_cache(debuginfo_path):
        for cache_dir in cache_dirs:
            cache_debuginfo_path = cache_dir + debuginfo_path
            return os.path.exists(cache_debuginfo_path)

    files = build_ids_to_path("", build_ids)
    return [build_ids[i] for i in xrange(0, len(files))
            if not exists_in_system(files[i]) and not exists_in_cache(files[i])]


class DebugInfoDownload(object):
    """
    This class is used to manage download of debuginfos.
    """

    def __init__(self, cache, tmp, repo_pattern="*debug*", keep_rpms=False,
                 noninteractive=True):
        pass

    # return value will be used as exitcode. So 0 = ok, !0 - error
    def download(self, files, download_exact_files=False):
        """
        Downloads rpms into a temporary directory

        Arguments:
            package_files_dict - a dict containing {pkg: file list} entries
            total_pkgs - total number of packages to download
            download_exact_files - extract only specified files

        Returns:
            RETURN_OK if all goes well.
            RETURN_FAILURE in case it cannot set up either of the directories.
        """

        if not download_exact_files:
            ins_cmds = ('-C "debuginfo(build-id)={0}"'.format(bid)
                        for bid in files)
        else:
           ins_cmds = files

        # insert empty line
        print ""

        print _("Please run the following commands as root user to install"
                " all necessary debug info packages:")

        for cmd in ins_cmds:
            print "zypper install {0}".format(cmd)

        # insert empty line
        print ""

        if ask_yes_no(_("See the log and install necessary debuginfo packages. "
                     "Once you are done answer 'Yes' or if you do not want to "
                     "download debuginfo answer 'No'")):
            return RETURN_OK

        return RETURN_FAILURE
