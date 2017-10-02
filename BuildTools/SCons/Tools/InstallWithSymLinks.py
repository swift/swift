"""SCons.Tool.install

Tool-specific initialization for the install tool.

There normally shouldn't be any need to import this module directly.
It will usually be imported through the generic SCons.Tool.Tool()
selection method.
"""

#
# Copyright (c) 2001 - 2015 The SCons Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

from SCons.Script import Action, Builder
from SCons.Node import FS
import shutil
import stat
import os
from os import path

class CopytreeError(EnvironmentError):
    pass

# This is a patched version of shutil.copytree from python 2.5.  It
# doesn't fail if the dir exists, which regular copytree does
# (annoyingly).  Note the XXX comment in the docstring.
def scons_copytree(src, dst, symlinks=False):
    """Recursively copy a directory tree using copy2().

    The destination directory must not already exist.
    If exception(s) occur, an CopytreeError is raised with a list of reasons.

    If the optional symlinks flag is true, symbolic links in the
    source tree result in symbolic links in the destination tree; if
    it is false, the contents of the files pointed to by symbolic
    links are copied.

    XXX Consider this example code rather than the ultimate tool.

    """
    names = os.listdir(src)
    # garyo@genarts.com fix: check for dir before making dirs.
    if not os.path.exists(dst):
        os.makedirs(dst)
    errors = []
    for name in names:
        srcname = os.path.join(src, name)
        dstname = os.path.join(dst, name)
        try:
            if symlinks and os.path.islink(srcname):
                linkto = os.readlink(srcname)
                os.symlink(linkto, dstname)
            elif os.path.isdir(srcname):
                scons_copytree(srcname, dstname, symlinks)
            else:
                shutil.copy2(srcname, dstname)
            # XXX What about devices, sockets etc.?
        except (IOError, os.error), why:
            errors.append((srcname, dstname, str(why)))
        # catch the CopytreeError from the recursive copytree so that we can
        # continue with other files
        except CopytreeError, err:
            errors.extend(err.args[0])
    try:
        shutil.copystat(src, dst)
    except WindowsError:
        # can't copy file access times on Windows
        pass
    except OSError, why:
        errors.extend((src, dst, str(why)))
    if errors:
        raise CopytreeError, errors


def symlinkBuilderImpl(target, source, env):
    lnk = target[0].abspath
    src = source[0].abspath
    lnkdir,lnkname = path.split(lnk)
    srcdir,srcname = path.split(src)

    scons_copytree(src, os.path.join(lnk, srcname), True)

    return None

def symlinkBuilderPrinter(target, source, env):
    lnk = path.basename(target[0].abspath)
    src = path.basename(source[0].abspath)
    return 'INSTALL PRESERVING SYMLINKS ' + target[0].get_internal_path()

def generate(env) :
    symlinkBuilder = Builder(action = Action(symlinkBuilderImpl, symlinkBuilderPrinter), target_factory = FS.Entry, source_factory = FS.Entry)
    env.Append(BUILDERS = {'InstallWithSymLinks' : symlinkBuilder})

def exists(env) :
    return True