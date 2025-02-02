HEADERS         = highlighter.h \
                  mainwindow.h
SOURCES         = highlighter.cpp \
                  mainwindow.cpp \
                  main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/richtext/syntaxhighlighter
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS syntaxhighlighter.pro examples
sources.path = $$[QT_INSTALL_EXAMPLES]/richtext/syntaxhighlighter
INSTALLS += target sources

wince*: {
   addFiles.sources = main.cpp mainwindow.cpp
   addFiles.path = .
   DEPLOYMENT += addFiles
}
