/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qmake application of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "option.h"
#include "cachekeys.h"
#include <qdir.h>
#include <qregexp.h>
#include <qhash.h>
#include <qdebug.h>
#include <qsettings.h>
#include <stdlib.h>
#include <stdarg.h>

QT_BEGIN_NAMESPACE

//convenience
const char *Option::application_argv0 = 0;
QString Option::prf_ext;
QString Option::js_ext;
QString Option::prl_ext;
QString Option::libtool_ext;
QString Option::pkgcfg_ext;
QString Option::ui_ext;
QStringList Option::h_ext;
QString Option::cpp_moc_ext;
QString Option::h_moc_ext;
QStringList Option::cpp_ext;
QStringList Option::c_ext;
QString Option::obj_ext;
QString Option::lex_ext;
QString Option::yacc_ext;
QString Option::pro_ext;
QString Option::dir_sep;
QString Option::dirlist_sep;
QString Option::h_moc_mod;
QString Option::cpp_moc_mod;
QString Option::yacc_mod;
QString Option::lex_mod;
QString Option::sysenv_mod;
QString Option::res_ext;
char Option::field_sep;

//mode
Option::QMAKE_MODE Option::qmake_mode = Option::QMAKE_GENERATE_NOTHING;

//all modes
QString Option::qmake_abslocation;
int Option::warn_level = WarnLogic;
int Option::debug_level = 0;
QFile Option::output;
QString Option::output_dir;
bool Option::recursive = false;
QStringList Option::before_user_vars;
QStringList Option::after_user_vars;
QStringList Option::user_configs;
QStringList Option::after_user_configs;
QString Option::user_template;
QString Option::user_template_prefix;
QStringList Option::shellPath;
#if defined(Q_OS_WIN32)
Option::TARG_MODE Option::target_mode = Option::TARG_WIN_MODE;
#elif defined(Q_OS_MAC)
Option::TARG_MODE Option::target_mode = Option::TARG_MACX_MODE;
#elif defined(Q_OS_QNX6)
Option::TARG_MODE Option::target_mode = Option::TARG_QNX6_MODE;
#else
Option::TARG_MODE Option::target_mode = Option::TARG_UNIX_MODE;
#endif

//QMAKE_*_PROPERTY stuff
QStringList Option::prop::properties;

//QMAKE_GENERATE_PROJECT stuff
bool Option::projfile::do_pwd = true;
QStringList Option::projfile::project_dirs;

//QMAKE_GENERATE_MAKEFILE stuff
QString Option::mkfile::qmakespec;
int Option::mkfile::cachefile_depth = -1;
bool Option::mkfile::do_deps = true;
bool Option::mkfile::do_mocs = true;
bool Option::mkfile::do_dep_heuristics = true;
bool Option::mkfile::do_preprocess = false;
bool Option::mkfile::do_stub_makefile = false;
bool Option::mkfile::do_cache = true;
QString Option::mkfile::cachefile;
QStringList Option::mkfile::project_files;
QString Option::mkfile::qmakespec_commandline;

static Option::QMAKE_MODE default_mode(QString progname)
{
    int s = progname.lastIndexOf(Option::dir_sep);
    if(s != -1)
        progname = progname.right(progname.length() - (s + 1));
    if(progname == "qmakegen")
        return Option::QMAKE_GENERATE_PROJECT;
    else if(progname == "qt-config")
        return Option::QMAKE_QUERY_PROPERTY;
    return Option::QMAKE_GENERATE_MAKEFILE;
}

QString project_builtin_regx();
bool usage(const char *a0)
{
    fprintf(stdout, "Usage: %s [mode] [options] [files]\n"
            "\n"
            "QMake has two modes, one mode for generating project files based on\n"
            "some heuristics, and the other for generating makefiles. Normally you\n"
            "shouldn't need to specify a mode, as makefile generation is the default\n"
            "mode for qmake, but you may use this to test qmake on an existing project\n"
            "\n"
            "Mode:\n"
            "  -project       Put qmake into project file generation mode%s\n"
            "                 In this mode qmake interprets files as files to\n"
            "                 be built,\n"
            "                 defaults to %s\n"
            "  -makefile      Put qmake into makefile generation mode%s\n"
            "                 In this mode qmake interprets files as project files to\n"
            "                 be processed, if skipped qmake will try to find a project\n"
            "                 file in your current working directory\n"
            "\n"
            "Warnings Options:\n"
            "  -Wnone         Turn off all warnings\n"
            "  -Wall          Turn on all warnings\n"
            "  -Wparser       Turn on parser warnings\n"
            "  -Wlogic        Turn on logic warnings\n"
            "\n"
            "Options:\n"
            "   * You can place any variable assignment in options and it will be     *\n"
            "   * processed as if it was in [files]. These assignments will be parsed *\n"
            "   * before [files].                                                     *\n"
            "  -o file        Write output to file\n"
            "  -unix          Run in unix mode\n"
            "  -win32         Run in win32 mode\n"
            "  -macx          Run in Mac OS X mode\n"
            "  -d             Increase debug level\n"
            "  -t templ       Overrides TEMPLATE as templ\n"
            "  -tp prefix     Overrides TEMPLATE so that prefix is prefixed into the value\n"
            "  -help          This help\n"
            "  -v             Version information\n"
            "  -after         All variable assignments after this will be\n"
            "                 parsed after [files]\n"
            "  -norecursive   Don't do a recursive search\n"
            "  -recursive     Do a recursive search\n"
            "  -cache file    Use file as cache           [makefile mode only]\n"
            "  -spec spec     Use spec as QMAKESPEC       [makefile mode only]\n"
            "  -nocache       Don't use a cache file      [makefile mode only]\n"
            "  -nodepend      Don't generate dependencies [makefile mode only]\n"
            "  -nomoc         Don't generate moc targets  [makefile mode only]\n"
            "  -nopwd         Don't look for files in pwd [project mode only]\n"
            ,a0,
            default_mode(a0) == Option::QMAKE_GENERATE_PROJECT  ? " (default)" : "", project_builtin_regx().toLatin1().constData(),
            default_mode(a0) == Option::QMAKE_GENERATE_MAKEFILE ? " (default)" : ""
        );
    return false;
}

int
Option::parseCommandLine(int argc, char **argv, int skip)
{
    bool before = true;
    for(int x = skip; x < argc; x++) {
        if(*argv[x] == '-' && strlen(argv[x]) > 1) { /* options */
            QString opt = argv[x] + 1;

            //first param is a mode, or we default
            if(x == 1) {
                bool specified = true;
                if(opt == "project") {
                    Option::recursive = true;
                    Option::qmake_mode = Option::QMAKE_GENERATE_PROJECT;
                } else if(opt == "prl") {
                    Option::mkfile::do_deps = false;
                    Option::mkfile::do_mocs = false;
                    Option::qmake_mode = Option::QMAKE_GENERATE_PRL;
                } else if(opt == "set") {
                    Option::qmake_mode = Option::QMAKE_SET_PROPERTY;
                } else if(opt == "query") {
                    Option::qmake_mode = Option::QMAKE_QUERY_PROPERTY;
                } else if(opt == "makefile") {
                    Option::qmake_mode = Option::QMAKE_GENERATE_MAKEFILE;
                } else {
                    specified = false;
                }
                if(specified)
                    continue;
            }
            //all modes
            if(opt == "o" || opt == "output") {
                Option::output.setFileName(argv[++x]);
            } else if(opt == "after") {
                before = false;
            } else if(opt == "t" || opt == "template") {
                Option::user_template = argv[++x];
            } else if(opt == "tp" || opt == "template_prefix") {
                Option::user_template_prefix = argv[++x];
            } else if(opt == "mac9") {
                Option::target_mode = TARG_MAC9_MODE;
            } else if(opt == "macx") {
                Option::target_mode = TARG_MACX_MODE;
            } else if(opt == "unix") {
                Option::target_mode = TARG_UNIX_MODE;
            } else if(opt == "win32") {
                Option::target_mode = TARG_WIN_MODE;
            } else if(opt == "d") {
                Option::debug_level++;
            } else if(opt == "version" || opt == "v" || opt == "-version") {
                fprintf(stdout,
                        "QMake version %s\n"
                        "Using Qt version %s in %s\n",
                        qmake_version(), QT_VERSION_STR,
                        QLibraryInfo::location(QLibraryInfo::LibrariesPath).toLatin1().constData());
#ifdef QMAKE_OPENSOURCE_VERSION
                fprintf(stdout, "QMake is Open Source software from Nokia Corporation and/or its subsidiary(-ies).\n");
#endif
                return Option::QMAKE_CMDLINE_BAIL;
            } else if(opt == "h" || opt == "help") {
                return Option::QMAKE_CMDLINE_SHOW_USAGE;
            } else if(opt == "Wall") {
                Option::warn_level |= WarnAll;
            } else if(opt == "Wparser") {
                Option::warn_level |= WarnParser;
            } else if(opt == "Wlogic") {
                Option::warn_level |= WarnLogic;
            } else if(opt == "Wnone") {
                Option::warn_level = WarnNone;
            } else if(opt == "r" || opt == "recursive") {
                Option::recursive = true;
            } else if(opt == "norecursive") {
                Option::recursive = false;
            } else if(opt == "config") {
                Option::user_configs += argv[++x];
            } else {
                if(Option::qmake_mode == Option::QMAKE_GENERATE_MAKEFILE ||
                   Option::qmake_mode == Option::QMAKE_GENERATE_PRL) {
                    if(opt == "nodepend" || opt == "nodepends") {
                        Option::mkfile::do_deps = false;
                    } else if(opt == "nomoc") {
                        Option::mkfile::do_mocs = false;
                    } else if(opt == "nocache") {
                        Option::mkfile::do_cache = false;
                    } else if(opt == "createstub") {
                        Option::mkfile::do_stub_makefile = true;
                    } else if(opt == "nodependheuristics") {
                        Option::mkfile::do_dep_heuristics = false;
                    } else if(opt == "E") {
                        Option::mkfile::do_preprocess = true;
                    } else if(opt == "cache") {
                        Option::mkfile::cachefile = argv[++x];
                    } else if(opt == "platform" || opt == "spec") {
                        Option::mkfile::qmakespec = argv[++x];
                        Option::mkfile::qmakespec_commandline = argv[x];
                    } else {
                        fprintf(stderr, "***Unknown option -%s\n", opt.toLatin1().constData());
                        return Option::QMAKE_CMDLINE_SHOW_USAGE | Option::QMAKE_CMDLINE_ERROR;
                    }
                } else if(Option::qmake_mode == Option::QMAKE_GENERATE_PROJECT) {
                    if(opt == "nopwd") {
                        Option::projfile::do_pwd = false;
                    } else {
                        fprintf(stderr, "***Unknown option -%s\n", opt.toLatin1().constData());
                        return Option::QMAKE_CMDLINE_SHOW_USAGE | Option::QMAKE_CMDLINE_ERROR;
                    }
                }
            }
        } else {
            QString arg = argv[x];
            if(arg.indexOf('=') != -1) {
                if(before)
                    Option::before_user_vars.append(arg);
                else
                    Option::after_user_vars.append(arg);
            } else {
                bool handled = true;
                if(Option::qmake_mode == Option::QMAKE_QUERY_PROPERTY ||
                    Option::qmake_mode == Option::QMAKE_SET_PROPERTY) {
                    Option::prop::properties.append(arg);
                } else {
                    QFileInfo fi(arg);
                    if(!fi.makeAbsolute()) //strange
                        arg = fi.filePath();
                    if(Option::qmake_mode == Option::QMAKE_GENERATE_MAKEFILE ||
                       Option::qmake_mode == Option::QMAKE_GENERATE_PRL)
                        Option::mkfile::project_files.append(arg);
                    else if(Option::qmake_mode == Option::QMAKE_GENERATE_PROJECT)
                        Option::projfile::project_dirs.append(arg);
                    else
                        handled = false;
                }
                if(!handled) {
                    return Option::QMAKE_CMDLINE_SHOW_USAGE | Option::QMAKE_CMDLINE_ERROR;
                }
            }
        }
    }

    return Option::QMAKE_CMDLINE_SUCCESS;
}

#ifdef Q_OS_WIN
static QStringList detectShellPath()
{
    QStringList paths;
    QString path = qgetenv("PATH");
    QStringList pathlist = path.toLower().split(";");
    for (int i = 0; i < pathlist.count(); i++) {
        QString maybeSh = pathlist.at(i) + "/sh.exe";
        if (QFile::exists(maybeSh)) {
            paths.append(maybeSh);
        }
    }
    return paths;
}
#endif

int
Option::init(int argc, char **argv)
{
    Option::application_argv0 = 0;
    Option::cpp_moc_mod = "";
    Option::h_moc_mod = "moc_";
    Option::lex_mod = "_lex";
    Option::yacc_mod = "_yacc";
    Option::prl_ext = ".prl";
    Option::libtool_ext = ".la";
    Option::pkgcfg_ext = ".pc";
    Option::prf_ext = ".prf";
    Option::js_ext = ".js";
    Option::ui_ext = ".ui";
    Option::h_ext << ".h" << ".hpp" << ".hh" << ".hxx";
    Option::c_ext << ".c";
#ifndef Q_OS_WIN
    Option::h_ext << ".H";
#endif
    Option::cpp_moc_ext = ".moc";
    Option::h_moc_ext = ".cpp";
    Option::cpp_ext << ".cpp" << ".cc" << ".cxx";
#ifndef Q_OS_WIN
    Option::cpp_ext << ".C";
#endif
    Option::lex_ext = ".l";
    Option::yacc_ext = ".y";
    Option::pro_ext = ".pro";
#ifdef Q_OS_WIN
    Option::dirlist_sep = ";";
    Option::shellPath = detectShellPath();
#else
    Option::dirlist_sep = ":";
#endif
    Option::sysenv_mod = "QMAKE_ENV_";
    Option::field_sep = ' ';

    if(argc && argv) {
        Option::application_argv0 = argv[0];
        QString argv0 = argv[0];
        if(Option::qmake_mode == Option::QMAKE_GENERATE_NOTHING)
            Option::qmake_mode = default_mode(argv0);
        if(!argv0.isEmpty() && !QFileInfo(argv0).isRelative()) {
            Option::qmake_abslocation = argv0;
        } else if (argv0.contains(QLatin1Char('/'))
#ifdef Q_OS_WIN
		   || argv0.contains(QLatin1Char('\\'))
#endif
	    ) { //relative PWD
            Option::qmake_abslocation = QDir::current().absoluteFilePath(argv0);
        } else { //in the PATH
            QByteArray pEnv = qgetenv("PATH");
            QDir currentDir = QDir::current();
#ifdef Q_OS_WIN
            QStringList paths = QString::fromLocal8Bit(pEnv).split(QLatin1String(";"));
#else
            QStringList paths = QString::fromLocal8Bit(pEnv).split(QLatin1String(":"));
#endif
            for (QStringList::const_iterator p = paths.constBegin(); p != paths.constEnd(); ++p) {
                if ((*p).isEmpty())
                    continue;
                QString candidate = currentDir.absoluteFilePath(*p + QLatin1Char('/') + argv0);
#ifdef Q_OS_WIN
                candidate += ".exe";
#endif
                if (QFile::exists(candidate)) {
                    Option::qmake_abslocation = candidate;
                    break;
                }
            }
        }
        if(!Option::qmake_abslocation.isNull())
            Option::qmake_abslocation = QDir::cleanPath(Option::qmake_abslocation);
    } else {
        Option::qmake_mode = Option::QMAKE_GENERATE_MAKEFILE;
    }

    const QByteArray envflags = qgetenv("QMAKEFLAGS");
    if (!envflags.isNull()) {
        int env_argc = 0, env_size = 0, currlen=0;
        char quote = 0, **env_argv = NULL;
        for (int i = 0; i < envflags.size(); ++i) {
            if (!quote && (envflags.at(i) == '\'' || envflags.at(i) == '"')) {
                quote = envflags.at(i);
            } else if (envflags.at(i) == quote) {
                quote = 0;
            } else if (!quote && envflags.at(i) == ' ') {
                if (currlen && env_argv && env_argv[env_argc]) {
                    env_argv[env_argc][currlen] = '\0';
                    currlen = 0;
                    env_argc++;
                }
            } else {
                if(!env_argv || env_argc > env_size) {
                    env_argv = (char **)realloc(env_argv, sizeof(char *)*(env_size+=10));
                    for(int i2 = env_argc; i2 < env_size; i2++)
                        env_argv[i2] = NULL;
                }
                if(!env_argv[env_argc]) {
                    currlen = 0;
                    env_argv[env_argc] = (char*)malloc(255);
                }
                if(currlen < 255)
                    env_argv[env_argc][currlen++] = envflags.at(i);
            }
        }
        if(env_argv) {
            if(env_argv[env_argc]) {
                env_argv[env_argc][currlen] = '\0';
                currlen = 0;
                env_argc++;
            }
            parseCommandLine(env_argc, env_argv);
            for(int i2 = 0; i2 < env_size; i2++) {
                if(env_argv[i2])
                    free(env_argv[i2]);
            }
            free(env_argv);
        }
    }
    if(argc && argv) {
        int ret = parseCommandLine(argc, argv, 1);
        if(ret != Option::QMAKE_CMDLINE_SUCCESS) {
            if ((ret & Option::QMAKE_CMDLINE_SHOW_USAGE) != 0)
                usage(argv[0]);
            return ret;
            //return ret == QMAKE_CMDLINE_SHOW_USAGE ? usage(argv[0]) : false;
        }
    }

    //last chance for defaults
    if(Option::qmake_mode == Option::QMAKE_GENERATE_MAKEFILE ||
        Option::qmake_mode == Option::QMAKE_GENERATE_PRL) {
        if(Option::mkfile::qmakespec.isNull() || Option::mkfile::qmakespec.isEmpty())
            Option::mkfile::qmakespec = QString::fromLocal8Bit(qgetenv("QMAKESPEC").constData());

        //try REALLY hard to do it for them, lazy..
        if(Option::mkfile::project_files.isEmpty()) {
            QString pwd = qmake_getpwd(),
                   proj = pwd + "/" + pwd.right(pwd.length() - (pwd.lastIndexOf('/') + 1)) + Option::pro_ext;
            if(QFile::exists(proj)) {
                Option::mkfile::project_files.append(proj);
            } else { //last try..
                QStringList profiles = QDir(pwd).entryList(QStringList("*" + Option::pro_ext));
                if(profiles.count() == 1)
                    Option::mkfile::project_files.append(pwd + "/" + profiles[0]);
            }
#ifndef QT_BUILD_QMAKE_LIBRARY
            if(Option::mkfile::project_files.isEmpty()) {
                usage(argv[0]);
                return Option::QMAKE_CMDLINE_ERROR;
            }
#endif
        }
    }

    //defaults for globals
    if(Option::target_mode == Option::TARG_WIN_MODE) {
        Option::dir_sep = "\\";
        Option::obj_ext = ".obj";
        Option::res_ext = ".res";
    } else {
        if(Option::target_mode == Option::TARG_MAC9_MODE)
            Option::dir_sep = ":";
        else
            Option::dir_sep = "/";
        Option::obj_ext = ".o";
    }
    Option::qmake_abslocation = Option::fixPathToTargetOS(Option::qmake_abslocation);
    return QMAKE_CMDLINE_SUCCESS;
}

bool Option::postProcessProject(QMakeProject *project)
{
    Option::cpp_ext = project->variables()["QMAKE_EXT_CPP"];
    if(cpp_ext.isEmpty())
        cpp_ext << ".cpp"; //something must be there
    Option::h_ext = project->variables()["QMAKE_EXT_H"];
    if(h_ext.isEmpty())
        h_ext << ".h";
    Option::c_ext = project->variables()["QMAKE_EXT_C"];
    if(c_ext.isEmpty())
        c_ext << ".c"; //something must be there

    if(!project->isEmpty("QMAKE_EXT_RES"))
        Option::res_ext = project->first("QMAKE_EXT_RES");
    if(!project->isEmpty("QMAKE_EXT_PKGCONFIG"))
        Option::pkgcfg_ext = project->first("QMAKE_EXT_PKGCONFIG");
    if(!project->isEmpty("QMAKE_EXT_LIBTOOL"))
        Option::libtool_ext = project->first("QMAKE_EXT_LIBTOOL");
    if(!project->isEmpty("QMAKE_EXT_PRL"))
        Option::prl_ext = project->first("QMAKE_EXT_PRL");
    if(!project->isEmpty("QMAKE_EXT_PRF"))
        Option::prf_ext = project->first("QMAKE_EXT_PRF");
    if(!project->isEmpty("QMAKE_EXT_JS"))
        Option::prf_ext = project->first("QMAKE_EXT_JS");
    if(!project->isEmpty("QMAKE_EXT_UI"))
        Option::ui_ext = project->first("QMAKE_EXT_UI");
    if(!project->isEmpty("QMAKE_EXT_CPP_MOC"))
        Option::cpp_moc_ext = project->first("QMAKE_EXT_CPP_MOC");
    if(!project->isEmpty("QMAKE_EXT_H_MOC"))
        Option::h_moc_ext = project->first("QMAKE_EXT_H_MOC");
    if(!project->isEmpty("QMAKE_EXT_LEX"))
        Option::lex_ext = project->first("QMAKE_EXT_LEX");
    if(!project->isEmpty("QMAKE_EXT_YACC"))
        Option::yacc_ext = project->first("QMAKE_EXT_YACC");
    if(!project->isEmpty("QMAKE_EXT_OBJ"))
        Option::obj_ext = project->first("QMAKE_EXT_OBJ");
    if(!project->isEmpty("QMAKE_H_MOD_MOC"))
        Option::h_moc_mod = project->first("QMAKE_H_MOD_MOC");
    if(!project->isEmpty("QMAKE_CPP_MOD_MOC"))
        Option::cpp_moc_mod = project->first("QMAKE_CPP_MOD_MOC");
    if(!project->isEmpty("QMAKE_MOD_LEX"))
        Option::lex_mod = project->first("QMAKE_MOD_LEX");
    if(!project->isEmpty("QMAKE_MOD_YACC"))
        Option::yacc_mod = project->first("QMAKE_MOD_YACC");
    if(!project->isEmpty("QMAKE_DIR_SEP"))
        Option::dir_sep = project->first("QMAKE_DIR_SEP");
    if(!project->isEmpty("QMAKE_DIRLIST_SEP"))
        Option::dirlist_sep = project->first("QMAKE_DIRLIST_SEP");
    if(!project->isEmpty("QMAKE_MOD_SYSTEM_ENV"))
        Option::sysenv_mod = project->first("QMAKE_MOD_SYSTEM_ENV");
    return true;
}

QString
Option::fixString(QString string, uchar flags)
{
    const QString orig_string = string;
    static QHash<FixStringCacheKey, QString> *cache = 0;
    if(!cache) {
        cache = new QHash<FixStringCacheKey, QString>;
        qmakeAddCacheClear(qmakeDeleteCacheClear_QHashFixStringCacheKeyQString, (void**)&cache);
    }
    FixStringCacheKey cacheKey(string, flags);
    if(cache->contains(cacheKey)) {
	const QString ret = cache->value(cacheKey);
	//qDebug() << "Fix (cached) " << orig_string << "->" << ret;
        return ret;
    }

    //fix the environment variables
    if(flags & Option::FixEnvVars) {
        int rep;
        QRegExp reg_var("\\$\\(.*\\)");
        reg_var.setMinimal(true);
        while((rep = reg_var.indexIn(string)) != -1)
            string.replace(rep, reg_var.matchedLength(),
                           QString::fromLocal8Bit(qgetenv(string.mid(rep + 2, reg_var.matchedLength() - 3).toLatin1().constData()).constData()));
    }

    //canonicalize it (and treat as a path)
    if(flags & Option::FixPathCanonicalize) {
#if 0
        string = QFileInfo(string).canonicalFilePath();
#endif
        string = QDir::cleanPath(string);
    }

    if(string.length() > 2 && string[0].isLetter() && string[1] == QLatin1Char(':'))
        string[0] = string[0].toLower();

    //fix separators
    Q_ASSERT(!((flags & Option::FixPathToLocalSeparators) && (flags & Option::FixPathToTargetSeparators)));
    if(flags & Option::FixPathToLocalSeparators) {
#if defined(Q_OS_WIN32)
        string = string.replace('/', '\\');
#else
        string = string.replace('\\', '/');
#endif
    } else if(flags & Option::FixPathToTargetSeparators) {
        string = string.replace('/', Option::dir_sep).replace('\\', Option::dir_sep);
    }

    if (string.startsWith("\"") && string.endsWith("\"") ||
        string.startsWith("\'") && string.endsWith("\'"))
        string = string.mid(1, string.length()-2);

    //cache
    //qDebug() << "Fix" << orig_string << "->" << string;
    cache->insert(cacheKey, string);
    return string;
}

const char *qmake_version()
{
    static char *ret = NULL;
    if(ret)
        return ret;
    ret = (char *)malloc(15);
    qmakeAddCacheClear(qmakeFreeCacheClear, (void**)&ret);
#if defined(_MSC_VER) && _MSC_VER >= 1400
    sprintf_s(ret, 15, "%d.%02d%c", QMAKE_VERSION_MAJOR, QMAKE_VERSION_MINOR, 'a' + QMAKE_VERSION_PATCH);
#else
    sprintf(ret, "%d.%02d%c", QMAKE_VERSION_MAJOR, QMAKE_VERSION_MINOR, 'a' + QMAKE_VERSION_PATCH);
#endif
    return ret;
}

void debug_msg_internal(int level, const char *fmt, ...)
{
    if(Option::debug_level < level)
        return;
    fprintf(stderr, "DEBUG %d: ", level);
    {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }
    fprintf(stderr, "\n");
}

void warn_msg(QMakeWarn type, const char *fmt, ...)
{
    if(!(Option::warn_level & type))
        return;
    fprintf(stderr, "WARNING: ");
    {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }
    fprintf(stderr, "\n");
}

class QMakeCacheClearItem {
private:
    qmakeCacheClearFunc func;
    void **data;
public:
    QMakeCacheClearItem(qmakeCacheClearFunc f, void **d) : func(f), data(d) { }
    ~QMakeCacheClearItem() {
        (*func)(*data);
        *data = 0;
    }
};
static QList<QMakeCacheClearItem*> cache_items;

void
qmakeClearCaches()
{
    qDeleteAll(cache_items);
    cache_items.clear();
}

void
qmakeAddCacheClear(qmakeCacheClearFunc func, void **data)
{
    cache_items.append(new QMakeCacheClearItem(func, data));
}

#ifdef Q_OS_WIN
# include <windows.h>

QT_USE_NAMESPACE
#endif

QString qmake_libraryInfoFile()
{
    QString ret;
#if defined( Q_OS_WIN )
    QFileInfo filePath;
    QT_WA({
        unsigned short module_name[256];
        GetModuleFileNameW(0, reinterpret_cast<wchar_t *>(module_name), sizeof(module_name));
        filePath = QString::fromUtf16(module_name);
    }, {
        char module_name[256];
        GetModuleFileNameA(0, module_name, sizeof(module_name));
        filePath = QString::fromLocal8Bit(module_name);
    });
    ret = filePath.filePath();
#else
    QString argv0 = QFile::decodeName(QByteArray(Option::application_argv0));
    QString absPath;

    if (!argv0.isEmpty() && argv0.at(0) == QLatin1Char('/')) {
        /*
          If argv0 starts with a slash, it is already an absolute
          file path.
        */
        absPath = argv0;
    } else if (argv0.contains(QLatin1Char('/'))) {
        /*
          If argv0 contains one or more slashes, it is a file path
          relative to the current directory.
        */
        absPath = QDir::current().absoluteFilePath(argv0);
    } else {
        /*
          Otherwise, the file path has to be determined using the
          PATH environment variable.
        */
        QByteArray pEnv = qgetenv("PATH");
        QDir currentDir = QDir::current();
        QStringList paths = QString::fromLocal8Bit(pEnv.constData()).split(QLatin1String(":"));
        for (QStringList::const_iterator p = paths.constBegin(); p != paths.constEnd(); ++p) {
            if ((*p).isEmpty())
                continue;
            QString candidate = currentDir.absoluteFilePath(*p + QLatin1Char('/') + argv0);
            QFileInfo candidate_fi(candidate);
            if (candidate_fi.exists() && !candidate_fi.isDir()) {
                absPath = candidate;
                break;
            }
        }
    }

    absPath = QDir::cleanPath(absPath);

    QFileInfo fi(absPath);
    ret = fi.exists() ? fi.canonicalFilePath() : QString();
#endif
    if(!ret.isEmpty())
        ret = QDir(QFileInfo(ret).absolutePath()).filePath("qt.conf");
    return ret;
}

QT_END_NAMESPACE
