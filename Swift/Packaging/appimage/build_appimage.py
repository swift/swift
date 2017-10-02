#!/usr/bin/env python2
#
# This Python script builds self-contained Linux packages, i.e. AppImages ( http://appimage.org/ ), of Swift.
# It downloads required appimage tool that is executed at the end and an exculde list from the internet.
#
# Non-standard dependencies:
# * plumbum ( https://plumbum.readthedocs.io/en/latest/ )
# * click ( http://click.pocoo.org/5/ )
#
# They can be installed via `pip install plumbum click`.
#
# The script requires git, wget, readelf and objcopy to work and will fail if
# they are not present.
#
# Just run ./Swift/Packaging/appimage/build_appimage.py and after it executed
# there should be .appimage and .debug files in Packages/Swift.
#
# The appimage will be for the same architecture as the host architecture.
# Pass `--qt5=False` to the tool to build against legacy Qt4.
#
# To include newer libstdc++.so or similar libs in an AppImage, use -li parameters.

from plumbum import local, FG, BG, RETCODE, colors, commands
import click
import os
import re
import time
import sys

git = local['git']

def git_working_dir():
    working_dir = git('rev-parse', '--show-toplevel').strip()
    if not os.path.exists(working_dir):
        working_dir = os.path.dirname(os.path.abspath(git('rev-parse', '--git-dir').strip()))
    return working_dir

git_working_dir = git_working_dir()
resources_dir = os.path.join(git_working_dir, "Swift/resources/")

@click.command()
@click.option('--qt5', default=True, type=bool, help='Build with Qt5.')
@click.option('--includelib', '-il', type=click.Path(), multiple=True, help='Copy extra library into AppImage.')
def build_appimage(qt5, includelib):
    print(colors.bold & colors.info | "Switch to git working directory root " + git_working_dir)
    with local.cwd(git_working_dir):

        def copy_dependencies_into_appdir(excludelist, binary_path, lib_path):
            chain = local['ldd'][binary_path] | local['grep']["=> /"] | local['awk']['{print $3}']
            for dynamic_lib in chain().strip().split('\n'):
                if os.path.basename(dynamic_lib) in excludelist:
                    #print(colors.info | "Not including " + dynamic_lib + " as it is blacklisted.")
                    pass
                else:
                    local['cp']('-v', '-L', dynamic_lib, lib_path)

        scons = local['./scons']
        print(colors.bold & colors.info | "Building Swift")
        scons['qt5={0}'.format("1" if qt5 else "0"), 'Swift'] & FG

        swift = local['./Swift/QtUI/swift-im']
        swift_version = swift('--version').strip()
        print(colors.bold & colors.info | "Successfully built " + swift_version)

        swift_architecture_string = ""
        try:
            readelf_on_swift_im_output = local['readelf']('-h', './Swift/QtUI/swift-im').strip()
            swift_architecture = re.search(r'Class:\s+([^\s]+)', readelf_on_swift_im_output, flags=re.I).group(1)
            if swift_architecture == "ELF32":
                swift_architecture_string = ".i386"
            elif swift_architecture == "ELF64":
                swift_architecture_string = ".amd64"
        except:
            pass

        appdir_path = os.path.join(git_working_dir, 'Swift/Packaging/AppImage/Swift.AppDir')
        print(colors.bold & colors.info | "Prepare AppDir structure at " + appdir_path)
        local['mkdir']('-p', appdir_path)

        swift_install_dir = os.path.join(appdir_path, 'usr')
        print(colors.bold & colors.info | "Install Swift to AppDir")
        scons['qt5={0}'.format("1" if qt5 else "0"), 'Swift', 'SWIFT_INSTALLDIR=' + swift_install_dir , swift_install_dir] & FG

        print(colors.bold & colors.info | "Download dynamic lib exclude list from https://raw.githubusercontent.com/AppImage/AppImages/master/excludelist")
        local['wget']['--no-check-certificate', '-O', '/tmp/excludelist', 'https://raw.githubusercontent.com/AppImage/AppImages/master/excludelist'] & FG

        print(colors.bold & colors.info | "Load dynamic library exclude list")
        excludelist = set()
        with open('/tmp/excludelist') as f:
            for line in f:
                if line and "#" not in line:
                    excludelist.add(line.strip())

        print(colors.bold & colors.info | "Copy dynamic library dependencies into AppDir")
        local['mkdir']('-p', os.path.join(appdir_path, 'usr/lib'))
        appdir_swift_im_binary = os.path.join(swift_install_dir, 'bin/swift-im')
        copy_dependencies_into_appdir(excludelist, appdir_swift_im_binary, os.path.join(swift_install_dir, "lib"))

        if qt5:
            print(colors.bold & colors.info | "Analyze binary for Qt plugin paths")
            # Run Swift binary and parse debug output to find plugin paths to copy.
            swift_plugin_debug_output = ""
            try:
                process = local["env"]["QT_DEBUG_PLUGINS=1", './Swift/QtUI/swift-im'].popen()
                time.sleep(2)
                process.kill()
                stdout, stderr = process.communicate()
                swift_plugin_debug_output = stderr
            except:
                pass
            if swift_plugin_debug_output:
                matches = re.search(r"/.*/qt5/plugins", swift_plugin_debug_output)
                if matches:
                    qt5_plugin_path = matches.group(0)
                    appdir_qt5_plugin_path = os.path.join(appdir_path, 'usr/lib/qt5')
                    local['mkdir']('-p', appdir_qt5_plugin_path)
                    local['cp']('-r', '-L', qt5_plugin_path, appdir_qt5_plugin_path)

                    print(colors.bold & colors.info | "Copy plugin dependencies into AppDir")
                    for plugin_path in local.path(appdir_qt5_plugin_path) // "plugins/*/*.so":
                        copy_dependencies_into_appdir(excludelist, plugin_path, os.path.join(swift_install_dir, "lib"))

        if includelib:
            for includelib_path in includelib:
                print(colors.bold & colors.info | "Copy " + includelib_path + " to AppDir.")
                local['cp']('-v', '-L', includelib_path, os.path.join(swift_install_dir, "lib"))

        print(colors.bold & colors.info | "Download https://github.com/AppImage/AppImageKit/raw/appimagetool/master/resources/AppRun to " + os.path.join(appdir_path, 'AppRun'))
        local['wget']('--no-check-certificate', '-O', os.path.join(appdir_path, 'AppRun'), "https://github.com/AppImage/AppImageKit/raw/appimagetool/master/resources/AppRun")
        local['chmod']('+x', os.path.join(appdir_path, 'AppRun'))

        print(colors.bold & colors.info | "Copy swift.desktop file")
        source_desktop_file = os.path.join(resources_dir, "swift.desktop")
        target_desktop_file = os.path.join(appdir_path, 'swift.desktop')
        local['cp']('-v', '-L', source_desktop_file, target_desktop_file)

        print(colors.bold & colors.info | "Copy logos to Swift.AppDir")
        local['cp']('-v', '-L', os.path.join(resources_dir, "logo/logo-icon-512.png"), os.path.join(appdir_path, "swift.png"))
        local['cp']('-v', '-L', os.path.join(resources_dir, "logo/logo-icon.svg"), os.path.join(appdir_path, "swift.svg"))
        local['cp']('-v', '-L', os.path.join(resources_dir, "logo/logo-icon-32.xpm"), os.path.join(appdir_path, "swift.xpm"))

        print(colors.bold & colors.info | "Download appimagetool to /tmp/appimagetool and make it executable")
        appimage_url_suffix = ""
        if swift_architecture_string == ".amd64":
            appimage_url_suffix = "x86_64.AppImage"
        else:
            appimage_url_suffix = "i686.AppImage"
        local['wget']['-O', '/tmp/appimagetool', 'https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-' + appimage_url_suffix] & FG
        local['chmod']['a+x', '/tmp/appimagetool']()

        local['mkdir']('-p', os.path.join(git_working_dir, "Packages/Swift"))
        appimage_path = os.path.join(git_working_dir, "Packages/Swift/" + swift_version.replace(" ", "-") + swift_architecture_string + ".appimage")
        debug_symbol_path = os.path.join(git_working_dir, "Packages/Swift/" + swift_version.replace(" ", "-") + swift_architecture_string + ".debug")

        print(colors.bold & colors.info | "Extract debug information from swift-im")
        local['objcopy']('--only-keep-debug', appdir_swift_im_binary, debug_symbol_path)
        local['strip']('-g', appdir_swift_im_binary)
        debuglink_retcode = local['objcopy']['--add-gnu-debuglink', debug_symbol_path, os.path.join(swift_install_dir, 'bin/swift-im')] & RETCODE
        if debuglink_retcode != 0:
            print(colors.bold & colors.warn | "Failed to create debuglink in binary.")

        print(colors.bold & colors.info | "Generate AppImage from Swift.AppDir")
        local['/tmp/appimagetool'][appdir_path, appimage_path] & FG


if __name__ == '__main__':
    build_appimage()