cmake_minimum_required(VERSION 3.16)
project(QScintilla VERSION 15.2.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt components
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)
if(NOT IOS)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS PrintSupport)
endif()

# Set Qt automoc properties globally
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Library name based on Qt version and build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(APPLE)
        set(QSCINTILLA_TARGET_NAME qscintilla2_qt${QT_VERSION_MAJOR}_debug)
    elseif(WIN32)
        set(QSCINTILLA_TARGET_NAME qscintilla2_qt${QT_VERSION_MAJOR}d)
    else()
        set(QSCINTILLA_TARGET_NAME qscintilla2_qt${QT_VERSION_MAJOR})
    endif()
else()
    set(QSCINTILLA_TARGET_NAME qscintilla2_qt${QT_VERSION_MAJOR})
endif()

# Create library
add_library(${QSCINTILLA_TARGET_NAME} SHARED)

# Set library properties
set_target_properties(${QSCINTILLA_TARGET_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# Include directories
target_include_directories(${QSCINTILLA_TARGET_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Qsci>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scintilla/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scintilla/lexlib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scintilla/src>
    $<INSTALL_INTERFACE:include>
)

# Compile definitions
target_compile_definitions(${QSCINTILLA_TARGET_NAME} PRIVATE
    QSCINTILLA_MAKE_DLL
    SCINTILLA_QT
    SCI_LEXER
    INCLUDE_DEPRECATED_FEATURES
)

# Optional: Enable SCI_NAMESPACE
# target_compile_definitions(${QSCINTILLA_TARGET_NAME} PRIVATE SCI_NAMESPACE)

# Platform-specific settings
if(APPLE AND NOT BUILD_SHARED_LIBS)
    set_target_properties(${QSCINTILLA_TARGET_NAME} PROPERTIES
        INSTALL_RPATH "@rpath"
    )
endif()

# Sources
set(SOURCES
    src/qsciscintilla.cpp
    src/qsciscintillabase.cpp
    src/qsciabstractapis.cpp
    src/qsciapis.cpp
    src/qscicommand.cpp
    src/qscicommandset.cpp
    src/qscidocument.cpp
    src/qscilexer.cpp
    src/qscilexerasm.cpp
    src/qscilexeravs.cpp
    src/qscilexerbash.cpp
    src/qscilexerbatch.cpp
    src/qscilexercmake.cpp
    src/qscilexercoffeescript.cpp
    src/qscilexercpp.cpp
    src/qscilexercsharp.cpp
    src/qscilexercss.cpp
    src/qscilexercustom.cpp
    src/qscilexerd.cpp
    src/qscilexerdiff.cpp
    src/qscilexeredifact.cpp
    src/qscilexerfortran.cpp
    src/qscilexerfortran77.cpp
    src/qscilexerhex.cpp
    src/qscilexerhtml.cpp
    src/qscilexeridl.cpp
    src/qscilexerintelhex.cpp
    src/qscilexerjava.cpp
    src/qscilexerjavascript.cpp
    src/qscilexerjson.cpp
    src/qscilexerlua.cpp
    src/qscilexermakefile.cpp
    src/qscilexermarkdown.cpp
    src/qscilexermasm.cpp
    src/qscilexermatlab.cpp
    src/qscilexernasm.cpp
    src/qscilexeroctave.cpp
    src/qscilexerpascal.cpp
    src/qscilexerperl.cpp
    src/qscilexerpostscript.cpp
    src/qscilexerpo.cpp
    src/qscilexerpov.cpp
    src/qscilexerproperties.cpp
    src/qscilexerpython.cpp
    src/qscilexerruby.cpp
    src/qscilexerspice.cpp
    src/qscilexersql.cpp
    src/qscilexersrec.cpp
    src/qscilexertcl.cpp
    src/qscilexertekhex.cpp
    src/qscilexertex.cpp
    src/qscilexerverilog.cpp
    src/qscilexervhdl.cpp
    src/qscilexerxml.cpp
    src/qscilexeryaml.cpp
    src/qscimacro.cpp
    src/qscistyle.cpp
    src/qscistyledtext.cpp
    src/InputMethod.cpp
    src/ListBoxQt.cpp
    src/MacPasteboardMime.cpp
    src/PlatQt.cpp
    src/SciAccessibility.cpp
    src/SciClasses.cpp
    src/ScintillaQt.cpp
)

# Add printer source for non-iOS builds
if(NOT IOS)
    list(APPEND SOURCES src/qsciprinter.cpp)
endif()

# Scintilla lexer sources
set(SCINTILLA_LEXER_SOURCES
    scintilla/lexers/LexA68k.cpp
    scintilla/lexers/LexAPDL.cpp
    scintilla/lexers/LexASY.cpp
    scintilla/lexers/LexAU3.cpp
    scintilla/lexers/LexAVE.cpp
    scintilla/lexers/LexAVS.cpp
    scintilla/lexers/LexAbaqus.cpp
    scintilla/lexers/LexAda.cpp
    scintilla/lexers/LexAsm.cpp
    scintilla/lexers/LexAsn1.cpp
    scintilla/lexers/LexBaan.cpp
    scintilla/lexers/LexBash.cpp
    scintilla/lexers/LexBasic.cpp
    scintilla/lexers/LexBatch.cpp
    scintilla/lexers/LexBibTeX.cpp
    scintilla/lexers/LexBullant.cpp
    scintilla/lexers/LexCLW.cpp
    scintilla/lexers/LexCOBOL.cpp
    scintilla/lexers/LexCPP.cpp
    scintilla/lexers/LexCSS.cpp
    scintilla/lexers/LexCaml.cpp
    scintilla/lexers/LexCmake.cpp
    scintilla/lexers/LexCoffeeScript.cpp
    scintilla/lexers/LexConf.cpp
    scintilla/lexers/LexCrontab.cpp
    scintilla/lexers/LexCsound.cpp
    scintilla/lexers/LexD.cpp
    scintilla/lexers/LexDMAP.cpp
    scintilla/lexers/LexDMIS.cpp
    scintilla/lexers/LexDiff.cpp
    scintilla/lexers/LexECL.cpp
    scintilla/lexers/LexEDIFACT.cpp
    scintilla/lexers/LexEScript.cpp
    scintilla/lexers/LexEiffel.cpp
    scintilla/lexers/LexErlang.cpp
    scintilla/lexers/LexErrorList.cpp
    scintilla/lexers/LexFlagship.cpp
    scintilla/lexers/LexForth.cpp
    scintilla/lexers/LexFortran.cpp
    scintilla/lexers/LexGAP.cpp
    scintilla/lexers/LexGui4Cli.cpp
    scintilla/lexers/LexHTML.cpp
    scintilla/lexers/LexHaskell.cpp
    scintilla/lexers/LexHex.cpp
    scintilla/lexers/LexIndent.cpp
    scintilla/lexers/LexInno.cpp
    scintilla/lexers/LexJSON.cpp
    scintilla/lexers/LexKVIrc.cpp
    scintilla/lexers/LexKix.cpp
    scintilla/lexers/LexLaTeX.cpp
    scintilla/lexers/LexLisp.cpp
    scintilla/lexers/LexLout.cpp
    scintilla/lexers/LexLua.cpp
    scintilla/lexers/LexMMIXAL.cpp
    scintilla/lexers/LexMPT.cpp
    scintilla/lexers/LexMSSQL.cpp
    scintilla/lexers/LexMagik.cpp
    scintilla/lexers/LexMake.cpp
    scintilla/lexers/LexMarkdown.cpp
    scintilla/lexers/LexMatlab.cpp
    scintilla/lexers/LexMaxima.cpp
    scintilla/lexers/LexMetapost.cpp
    scintilla/lexers/LexModula.cpp
    scintilla/lexers/LexMySQL.cpp
    scintilla/lexers/LexNimrod.cpp
    scintilla/lexers/LexNsis.cpp
    scintilla/lexers/LexNull.cpp
    scintilla/lexers/LexOScript.cpp
    scintilla/lexers/LexOpal.cpp
    scintilla/lexers/LexPB.cpp
    scintilla/lexers/LexPLM.cpp
    scintilla/lexers/LexPO.cpp
    scintilla/lexers/LexPOV.cpp
    scintilla/lexers/LexPS.cpp
    scintilla/lexers/LexPascal.cpp
    scintilla/lexers/LexPerl.cpp
    scintilla/lexers/LexPowerPro.cpp
    scintilla/lexers/LexPowerShell.cpp
    scintilla/lexers/LexProgress.cpp
    scintilla/lexers/LexProps.cpp
    scintilla/lexers/LexPython.cpp
    scintilla/lexers/LexR.cpp
    scintilla/lexers/LexRebol.cpp
    scintilla/lexers/LexRegistry.cpp
    scintilla/lexers/LexRuby.cpp
    scintilla/lexers/LexRust.cpp
    scintilla/lexers/LexSAS.cpp
    scintilla/lexers/LexSML.cpp
    scintilla/lexers/LexSQL.cpp
    scintilla/lexers/LexSTTXT.cpp
    scintilla/lexers/LexScriptol.cpp
    scintilla/lexers/LexSmalltalk.cpp
    scintilla/lexers/LexSorcus.cpp
    scintilla/lexers/LexSpecman.cpp
    scintilla/lexers/LexSpice.cpp
    scintilla/lexers/LexStata.cpp
    scintilla/lexers/LexTACL.cpp
    scintilla/lexers/LexTADS3.cpp
    scintilla/lexers/LexTAL.cpp
    scintilla/lexers/LexTCL.cpp
    scintilla/lexers/LexTCMD.cpp
    scintilla/lexers/LexTeX.cpp
    scintilla/lexers/LexTxt2tags.cpp
    scintilla/lexers/LexVB.cpp
    scintilla/lexers/LexVHDL.cpp
    scintilla/lexers/LexVerilog.cpp
    scintilla/lexers/LexVisualProlog.cpp
    scintilla/lexers/LexYAML.cpp
)

# Scintilla lexlib sources
set(SCINTILLA_LEXLIB_SOURCES
    scintilla/lexlib/Accessor.cpp
    scintilla/lexlib/CharacterCategory.cpp
    scintilla/lexlib/CharacterSet.cpp
    scintilla/lexlib/DefaultLexer.cpp
    scintilla/lexlib/LexerBase.cpp
    scintilla/lexlib/LexerModule.cpp
    scintilla/lexlib/LexerNoExceptions.cpp
    scintilla/lexlib/LexerSimple.cpp
    scintilla/lexlib/PropSetSimple.cpp
    scintilla/lexlib/StyleContext.cpp
    scintilla/lexlib/WordList.cpp
)

# Scintilla src sources
set(SCINTILLA_SRC_SOURCES
    scintilla/src/AutoComplete.cpp
    scintilla/src/CallTip.cpp
    scintilla/src/CaseConvert.cpp
    scintilla/src/CaseFolder.cpp
    scintilla/src/Catalogue.cpp
    scintilla/src/CellBuffer.cpp
    scintilla/src/CharClassify.cpp
    scintilla/src/ContractionState.cpp
    scintilla/src/DBCS.cpp
    scintilla/src/Decoration.cpp
    scintilla/src/Document.cpp
    scintilla/src/EditModel.cpp
    scintilla/src/Editor.cpp
    scintilla/src/EditView.cpp
    scintilla/src/ExternalLexer.cpp
    scintilla/src/Indicator.cpp
    scintilla/src/KeyMap.cpp
    scintilla/src/LineMarker.cpp
    scintilla/src/MarginView.cpp
    scintilla/src/PerLine.cpp
    scintilla/src/PositionCache.cpp
    scintilla/src/RESearch.cpp
    scintilla/src/RunStyles.cpp
    scintilla/src/ScintillaBase.cpp
    scintilla/src/Selection.cpp
    scintilla/src/Style.cpp
    scintilla/src/UniConversion.cpp
    scintilla/src/ViewStyle.cpp
    scintilla/src/XPM.cpp
)

# Add all sources to target
target_sources(${QSCINTILLA_TARGET_NAME} PRIVATE
    ${SOURCES}
    ${SCINTILLA_LEXER_SOURCES}
    ${SCINTILLA_LEXLIB_SOURCES}
    ${SCINTILLA_SRC_SOURCES}
)

# Explicitly include headers for MOC processing
set(MOC_HEADERS
    src/Qsci/qsciglobal.h
    src/Qsci/qsciscintilla.h
    src/Qsci/qsciscintillabase.h
    src/Qsci/qsciabstractapis.h
    src/Qsci/qsciapis.h
    src/Qsci/qscicommand.h
    src/Qsci/qscicommandset.h
    src/Qsci/qscidocument.h
    src/Qsci/qscilexer.h
    src/Qsci/qscilexerasm.h
    src/Qsci/qscilexeravs.h
    src/Qsci/qscilexerbash.h
    src/Qsci/qscilexerbatch.h
    src/Qsci/qscilexercmake.h
    src/Qsci/qscilexercoffeescript.h
    src/Qsci/qscilexercpp.h
    src/Qsci/qscilexercsharp.h
    src/Qsci/qscilexercss.h
    src/Qsci/qscilexercustom.h
    src/Qsci/qscilexerd.h
    src/Qsci/qscilexerdiff.h
    src/Qsci/qscilexeredifact.h
    src/Qsci/qscilexerfortran.h
    src/Qsci/qscilexerfortran77.h
    src/Qsci/qscilexerhex.h
    src/Qsci/qscilexerhtml.h
    src/Qsci/qscilexeridl.h
    src/Qsci/qscilexerintelhex.h
    src/Qsci/qscilexerjava.h
    src/Qsci/qscilexerjavascript.h
    src/Qsci/qscilexerjson.h
    src/Qsci/qscilexerlua.h
    src/Qsci/qscilexermakefile.h
    src/Qsci/qscilexermarkdown.h
    src/Qsci/qscilexermasm.h
    src/Qsci/qscilexermatlab.h
    src/Qsci/qscilexernasm.h
    src/Qsci/qscilexeroctave.h
    src/Qsci/qscilexerpascal.h
    src/Qsci/qscilexerperl.h
    src/Qsci/qscilexerpostscript.h
    src/Qsci/qscilexerpo.h
    src/Qsci/qscilexerpov.h
    src/Qsci/qscilexerproperties.h
    src/Qsci/qscilexerpython.h
    src/Qsci/qscilexerruby.h
    src/Qsci/qscilexerspice.h
    src/Qsci/qscilexersql.h
    src/Qsci/qscilexersrec.h
    src/Qsci/qscilexertcl.h
    src/Qsci/qscilexertekhex.h
    src/Qsci/qscilexertex.h
    src/Qsci/qscilexerverilog.h
    src/Qsci/qscilexervhdl.h
    src/Qsci/qscilexerxml.h
    src/Qsci/qscilexeryaml.h
    src/Qsci/qscimacro.h
    src/Qsci/qscistyle.h
    src/Qsci/qscistyledtext.h
    src/ListBoxQt.h
    src/SciAccessibility.h
    src/SciClasses.h
    src/ScintillaQt.h
)

if(NOT IOS)
    list(APPEND MOC_HEADERS src/Qsci/qsciprinter.h)
endif()

# Add headers to target sources for MOC processing
target_sources(${QSCINTILLA_TARGET_NAME} PRIVATE ${MOC_HEADERS})

# Headers for installation
set(PUBLIC_HEADERS
    src/Qsci/qsciglobal.h
    src/Qsci/qsciscintilla.h
    src/Qsci/qsciscintillabase.h
    src/Qsci/qsciabstractapis.h
    src/Qsci/qsciapis.h
    src/Qsci/qscicommand.h
    src/Qsci/qscicommandset.h
    src/Qsci/qscidocument.h
    src/Qsci/qscilexer.h
    src/Qsci/qscilexerasm.h
    src/Qsci/qscilexeravs.h
    src/Qsci/qscilexerbash.h
    src/Qsci/qscilexerbatch.h
    src/Qsci/qscilexercmake.h
    src/Qsci/qscilexercoffeescript.h
    src/Qsci/qscilexercpp.h
    src/Qsci/qscilexercsharp.h
    src/Qsci/qscilexercss.h
    src/Qsci/qscilexercustom.h
    src/Qsci/qscilexerd.h
    src/Qsci/qscilexerdiff.h
    src/Qsci/qscilexeredifact.h
    src/Qsci/qscilexerfortran.h
    src/Qsci/qscilexerfortran77.h
    src/Qsci/qscilexerhex.h
    src/Qsci/qscilexerhtml.h
    src/Qsci/qscilexeridl.h
    src/Qsci/qscilexerintelhex.h
    src/Qsci/qscilexerjava.h
    src/Qsci/qscilexerjavascript.h
    src/Qsci/qscilexerjson.h
    src/Qsci/qscilexerlua.h
    src/Qsci/qscilexermakefile.h
    src/Qsci/qscilexermarkdown.h
    src/Qsci/qscilexermasm.h
    src/Qsci/qscilexermatlab.h
    src/Qsci/qscilexernasm.h
    src/Qsci/qscilexeroctave.h
    src/Qsci/qscilexerpascal.h
    src/Qsci/qscilexerperl.h
    src/Qsci/qscilexerpostscript.h
    src/Qsci/qscilexerpo.h
    src/Qsci/qscilexerpov.h
    src/Qsci/qscilexerproperties.h
    src/Qsci/qscilexerpython.h
    src/Qsci/qscilexerruby.h
    src/Qsci/qscilexerspice.h
    src/Qsci/qscilexersql.h
    src/Qsci/qscilexersrec.h
    src/Qsci/qscilexertcl.h
    src/Qsci/qscilexertekhex.h
    src/Qsci/qscilexertex.h
    src/Qsci/qscilexerverilog.h
    src/Qsci/qscilexervhdl.h
    src/Qsci/qscilexerxml.h
    src/Qsci/qscilexeryaml.h
    src/Qsci/qscimacro.h
    src/Qsci/qscistyle.h
    src/Qsci/qscistyledtext.h
)

if(NOT IOS)
    list(APPEND PUBLIC_HEADERS src/Qsci/qsciprinter.h)
endif()

# Link libraries
target_link_libraries(${QSCINTILLA_TARGET_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)

if(NOT IOS)
    target_link_libraries(${QSCINTILLA_TARGET_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::PrintSupport)
endif()

# macOS specific
if(APPLE AND QT_VERSION_MAJOR LESS 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS MacExtras)
    target_link_libraries(${QSCINTILLA_TARGET_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::MacExtras)
endif()

# Disable warnings
if(MSVC)
    target_compile_options(${QSCINTILLA_TARGET_NAME} PRIVATE /W0)
else()
    target_compile_options(${QSCINTILLA_TARGET_NAME} PRIVATE -w)
endif()

# Installation
include(GNUInstallDirs)

# Install library
install(TARGETS ${QSCINTILLA_TARGET_NAME}
    EXPORT QScintillaTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Qsci
)

# Install the Qsci directory itself
install(DIRECTORY src/Qsci
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# Translation files
set(TRANSLATION_FILES
    src/qscintilla_cs.ts
    src/qscintilla_de.ts
    src/qscintilla_es.ts
    src/qscintilla_fr.ts
    src/qscintilla_pt_br.ts
)

# Find Qt translation tools
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS LinguistTools)

# Process translation files
qt_add_translation(QM_FILES ${TRANSLATION_FILES})
add_custom_target(translations ALL DEPENDS ${QM_FILES})

# Install translation files
install(FILES ${QM_FILES}
    DESTINATION ${CMAKE_INSTALL_DATADIR}/qt${QT_VERSION_MAJOR}/translations
)

# Install qsci data directory
install(DIRECTORY qsci
    DESTINATION ${CMAKE_INSTALL_DATADIR}/qt${QT_VERSION_MAJOR}
)

# Create and install CMake config files
include(CMakePackageConfigHelpers)

# Generate the version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QScintillaConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Create a simple config file
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/QScintillaConfig.cmake"
"# QScintilla CMake Config File
include(\"\${CMAKE_CURRENT_LIST_DIR}/QScintillaTargets.cmake\")
")

# Install the config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/QScintillaConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/QScintillaConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QScintilla
)

# Export targets
install(EXPORT QScintillaTargets
    FILE QScintillaTargets.cmake
    NAMESPACE QScintilla::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QScintilla
)

# Create a simple pkg-config file
set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix \${prefix})
set(libdir \${exec_prefix}/${CMAKE_INSTALL_LIBDIR})
set(includedir \${prefix}/${CMAKE_INSTALL_INCLUDEDIR})

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/qscintilla2_qt${QT_VERSION_MAJOR}.pc"
"prefix=${prefix}
exec_prefix=\${prefix}
libdir=\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}
includedir=\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}

Name: QScintilla2
Description: Qt port of the Scintilla source code editing widget
Version: ${PROJECT_VERSION}
Requires: Qt${QT_VERSION_MAJOR}Core Qt${QT_VERSION_MAJOR}Widgets Qt${QT_VERSION_MAJOR}PrintSupport
Libs: -L\${libdir} -l${QSCINTILLA_TARGET_NAME}
Cflags: -I\${includedir}
")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qscintilla2_qt${QT_VERSION_MAJOR}.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# Feature summary
include(FeatureSummary)
feature_summary(WHAT ALL)

# Build examples option
option(BUILD_EXAMPLES "Build example applications" ON)

# Export the target name to parent scope
set(QSCINTILLA_TARGET_NAME ${QSCINTILLA_TARGET_NAME} PARENT_SCOPE)
set(QSCINTILLA_TARGET_NAME ${QSCINTILLA_TARGET_NAME} CACHE INTERNAL "QScintilla target name")

# Create an alias for easier usage
add_library(QScintilla::QScintilla ALIAS ${QSCINTILLA_TARGET_NAME})