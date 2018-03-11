#-------------------------------------------------
#
# Project created by QtCreator 2016-08-31T18:09:05
#
#-------------------------------------------------

COMP_CONFIG = target_predeps no_link combine

MKDIR = mkdir
DIR_EXISTS = if exist
DIR_NOT_EXISTS = if not exist
win32: {
    EOFF = @echo off &
    COPY_FILE = copy /y
    COPY_DIR = xcopy /s /q /y /i
    DEL_FILE = del /s /q
    DEL_DIR = rd /s /q
} else:unix {
    EOFF =
    COPY_FILE = cp /y
    COPY_DIR = \"$$shell_path($$this_dir/tools/xcopy.py)\"
    DEL_FILE = rm -rf
    DEL_DIR = rm -rf
} else {
    error(unknown platform!!!)
}

INPUT = $$_PRO_FILE_

##
win32:CONFIG(debug, debug|release):debug_suffix=d
else:win32:CONFIG(release, debug|release):debug_suffix=
else:unix:debug_suffix=

## execsuffix
defineReplace(execsuffix) {
    win32:return (.exe)
}

## dylib
defineReplace(dylibprefix) {
    unix:return (lib)
}

## dylibprefix
defineReplace(dylibprefix) {
    unix:return (lib)
}

## dylibsuffix
defineReplace(dylibsuffix) {
    win32:return (.dll)
    else:unix:return (.so*)
}

## copyfile
defineReplace(copyfile) {
    srcfile = $$shell_path($$1)
    dstfile = $$shell_path($$2)
    dstdir = $$section(dstfile, $$shell_path('/'), 0, -2)
    commands =
    commands += ($$DIR_NOT_EXISTS \"$$dstdir\" $$MKDIR \"$$dstdir\") &
    commands += ($$DIR_EXISTS \"$$srcfile\" $$COPY_FILE \"$$srcfile\" \"$$dstfile\") &
    return ($$commands)
}

## copydir
defineReplace(copydir) {
    srcfile = $$shell_path($$1)
    dstdir = $$shell_path($$2)
    excludes = $$3
    !exists($$srcfile):return ()
    srcdir = $$section(srcfile, $$shell_path('/'), 0, -2)
    win32:commands = $$COPY_DIR \"$$srcfile\" \"$$dstdir\"
    unix:commands = \"$$COPY_DIR\" \"$$srcdir\" \"$$dstdir\" \"$$srcfile\"
    !isEmpty(excludes):commands += /exclude:$$join(excludes, '+')
    commands += &
    return ($$commands)
}

## removefile
defineReplace(removefile) {
    file = $$shell_path($$1)
    return (($$DIR_EXISTS \"$${file}\" $$DEL_FILE \"$${file}\") &)
}

## removedir
defineReplace(removedir) {
    dir = $$shell_path($$1)
    return (($$DIR_EXISTS \"$${dir}\" $$DEL_DIR \"$${dir}\") &)
}

## copylib
defineReplace(copylib) {
    srcdir = $$shell_path($$1)
    name = $$2
    dstdir = $$shell_path($$3)
    includelib = $$4
    commands =
    # - dll
    suffix=
    contains(DEFINES, DEVELOP):suffix=?
    commands += $$copyfile($$srcdir/$$dylibprefix()$${name}$${suffix}$$dylibsuffix()?, $$dstdir)
    # - lib
    equals(includelib, "true") {
        commands += $$copyfile($$srcdir/$$dylibprefix()$${name}$${suffix}.lib?, $$dstdir)
    }
#    contains(DEFINES, DEVELOP) {
#        commands += $$copyfile($$srcdir/$$dylibprefix()$${name}$${suffix}.lib?, $$dstdir)
#        commands += $$copyfile($$srcdir/$$dylibprefix()$${name}$${suffix}.pdb?, $$dstdir)
#    }
    return ($$commands)
}

## copydevlib
defineReplace(copydevlib) {
    srcdir = $$shell_path($$1)
    name = $$2
    dstdir = $$shell_path($$3)
    commands =
    # - dll
    suffix=
    contains(DEFINES, DEVELOP):suffix=?
    # - lib
    contains(DEFINES, DEVELOP) {
        commands += $$copyfile($$srcdir/$$dylibprefix()$${name}$${suffix}.lib?, $$dstdir)
#        commands += $$copyfile($$srcdir/$$dylibprefix()$${name}$${suffix}.pdb?, $$dstdir)
    }
    return ($$commands)
}

## copydll
defineReplace(copydll) {
    srcdir = $$shell_path($$1)
    name = $$2
    dstdir = $$shell_path($$3)
    commands =
    # - dll
    suffix=
    contains(DEFINES, DEVELOP):suffix=?
    commands += $$copyfile($$srcdir/$$dylibprefix()$${name}$${suffix}$$dylibsuffix()?, $$dstdir)
    return ($$commands)
}

## copyComponent
defineReplace(copyComponent) {
    srcdir = $$shell_path($$1)
    name = $$2
    dstdir = $$shell_path($$3)
    commands =
    # - dll
    suffix=
    contains(DEFINES, DEVELOP):suffix=?
    commands += $$copyfile($$srcdir/$$dylibprefix()$${name}$${suffix}$$dylibsuffix()?, $$dstdir)
    # - xml
    commands += $$copyfile($$srcdir/$${name}.xml?, $$dstdir)
    return ($$commands)
}

## group
defineReplace(group) {
    return($$1)
}

## compiler
defineReplace(compiler) {
    name = $$1
    commands = $$2
    depends = $$3

    $${name}.input = INPUT
    $${name}.output = $$name
    $${name}.CONFIG = $${COMP_CONFIG}
    $${name}.commands = $${EOFF} $$commands
    isEmpty(depends) {
        $${name}.depends = $$last(QMAKE_EXTRA_COMPILERS)
    } else {
        $${name}.depends = $$depends
    }

    QMAKE_EXTRA_COMPILERS += $$name

    export(name)
    export($${name}.depends)
    export($${name}.input)
    export($${name}.output)
    export($${name}.CONFIG)
    export($${name}.commands)
    export($${name}.depends)
    export(QMAKE_EXTRA_COMPILERS)

    return($$name)
}

## package_init
defineTest(package_init) {
    ROOT = $$section(PWD, '/', -1)
    exists($$PWD/*.ignore) {
        DISTFILES += $$PWD/*.ignore
        export(DISTFILES)
    }

    $$compiler($$group($${ROOT}.header), $$group(echo --- $$PWD))
    $$compiler($$group($${ROOT}.clean), $$removedir($$PWD/data/))

    export(ROOT)

    return(true)
}

##
defineReplace(solutiondir) {
    this_dir = $$1
    exists($$this_dir/.rootdir) {
        this_name = $$section(this_dir, /, -1)
        proj_name = $$fromfile($$this_dir/.rootdir, proj_name)
        !isEmpty(proj_name) {
            _this_dir = $$section(OUT_PWD, /build-, 0, -2)/$$proj_name
            exists($$_this_dir): this_dir = _this_dir
        }
    }

    return ($$this_dir)
}
