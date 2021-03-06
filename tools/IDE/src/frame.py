#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013 OSW. All rights reserved.
# Copyright (c) 2008-2012 the MansOS team. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#  * Redistributions of source code must retain the above copyright notice,
#    this list of  conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import os
import wx
from stat import S_ISDIR
from wx.lib.agw import aui
from time import time
from subprocess import Popen
import webbrowser

from src.upload_module import UploadModule
from src.globals import * #@UnusedWildImport
from src.Translater import localize, Translater
from src.Settings import Settings

class Frame(wx.Frame):
    def __init__(self, parent, title, size, pos, API):
        super(Frame, self).__init__(parent, wx.ID_ANY, title, size = size, pos = pos)
        # Get path, here must use only file name, __file__ sometimes contains more than that
        self.path = os.path.dirname(os.path.realpath(os.path.split(__file__)[1]))
        self.API = API
        self.API.path = self.path
        self.lastPanel = None
        self.auiManager = aui.AuiManager()
        self.loadPositioning()
        self.exitCalled = False
        self.blocklyToolVisible = False

        self.toolbar = None
        self.menubar = None
        self.examples = dict()
        self.initUI()
        self.SetBackgroundColour("white")
        flag = aui.AUI_MGR_ALLOW_ACTIVE_PANE | \
        aui.AUI_MGR_AUTONB_NO_CAPTION | aui.AUI_MGR_SMOOTH_DOCKING | \
        aui.AUI_MGR_TRANSPARENT_HINT | aui.AUI_NB_CLOSE_ON_TAB_LEFT | \
        aui.AUI_MGR_AERO_DOCKING_GUIDES | aui.AUI_MGR_TRANSPARENT_DRAG
        self.auiManager.SetAGWFlags(self.auiManager.GetAGWFlags() ^ flag)
        self.auiManager.SetAutoNotebookStyle(aui.AUI_NB_TOP | aui.AUI_NB_SMART_TABS | aui.AUI_NB_CLOSE_ON_ALL_TABS)
        self.auiManager.SetManagedWindow(self)

        self.API.onExit.append(self.Close)

        self.API.tabManager.Reparent(self)
        self.API.editPanel.Reparent(self)
        if self.API.foundBlockly:
            self.API.blockly.Reparent(self)
        self.API.listenModules[0].Reparent(self)
        self.API.infoArea.Reparent(self)

        mainPane = (aui.AuiPaneInfo().CenterPane().Name("mainPane").
                Caption(localize("Editors")).CaptionVisible(False).
                BestSize(wx.Size(600, 400)))
        self.auiManager.AddPane(self.API.tabManager, mainPane)

        self.infoPane = (aui.AuiPaneInfo().CaptionVisible(True).
                BestSize(wx.Size(500, 150)).Bottom().
                CloseButton(False).MaximizeButton(True).MinimizeButton(True))

        self.auiManager.AddPane(self.API.infoArea, self.infoPane.Caption(localize("Info")).Name("infoPane"))

        self.layoutListenPane(self.API.listenModules[0], localize("Listen module") + " 1", False)
        self.layoutEditPane()
        self.layoutBlocklyPane()

        self.auiManager.Update()
        self.Bind(wx.EVT_CLOSE, self.OnQuit)
        self.auiManager.ShowPane(self.API.infoArea, True)

    def initUI(self):
        self.generateMenu()
        # Check if we need to update existing toolbar(for rerun)
        if self.toolbar == None:
            self.toolbar = self.CreateToolBar()
            self.toolbar.SetToolBitmapSize((32, 32))
        else:
            self.toolbar.ClearTools()

        # Note that all icons must be 32x32, so they look good :)
        self.toolbar.AddLabelTool(wx.ID_NEW, localize('New'),
                                wx.Bitmap(self.path + '/src/Icons/new.png'),
                                shortHelp = localize('New'))
        self.toolbar.AddSeparator()
        self.toolbar.AddLabelTool(wx.ID_OPEN, localize('Open'),
                                wx.Bitmap(self.path + '/src/Icons/open.png'),
                                shortHelp = localize('Open'))
        self.toolbar.AddLabelTool(wx.ID_SAVE, localize('Save'),
                                wx.Bitmap(self.path + '/src/Icons/save.png'),
                                shortHelp = localize('Save'))
        self.toolbar.AddSeparator()
        addStatementTool = self.toolbar.AddLabelTool(wx.ID_ADD, localize('Add statement'),
                                wx.Bitmap(self.path + '/src/Icons/add_statement.png'),
                                shortHelp = localize('Add statement'))
        # Used ID_APPLY for identification, hope nothing else uses it
        addConditionTool = self.toolbar.AddLabelTool(wx.ID_APPLY, localize('Add condition'),
                                wx.Bitmap(self.path + '/src/Icons/add_condition.png'),
                                shortHelp = localize('Add condition'))
        #self.toolbar.AddSeparator()
        #addBlocklyTool = self.toolbar.AddLabelTool(wx.ID_MORE, localize('Open Seal-Blockly editor'),
        #                        wx.Bitmap(self.path + '/src/Icons/Seal_blockly.png'),
        #                        shortHelp = localize('Open Seal-Blockly editor'))
        self.toolbar.AddSeparator()
        compileTool = self.toolbar.AddLabelTool(wx.ID_PREVIEW, localize('Compile'),
                                wx.Bitmap(self.path + '/src/Icons/compile.png'),
                                shortHelp = localize('Compile'))
        uplTool = self.toolbar.AddLabelTool(wx.ID_PREVIEW_GOTO, localize('Upload'),
                                wx.Bitmap(self.path + '/src/Icons/upload.png'),
                                shortHelp = localize('Upload'))
        outputTool = self.toolbar.AddLabelTool(wx.ID_PREVIEW_ZOOM, localize('Configure upload and compile'),
                                wx.Bitmap(self.path + '/src/Icons/read.png'),
                                shortHelp = localize('Configure upload and compile'))
        listenTool = self.toolbar.AddCheckLabelTool(wx.ID_HOME, localize('Listen node\'s output'),
                                wx.Bitmap(self.path + '/src/Icons/listen.png'),
                                shortHelp = localize('Listen node\'s output'))
        self.toolbar.AddSeparator()
        self.toolbar.AddLabelTool(wx.ID_EXIT, localize('Exit'),
                                wx.Bitmap(self.path + '/src/Icons/exit.png'),
                                shortHelp = localize('Exit'))
        self.toolbar.Realize()

        # Second bind to toolbar, but only items with ID_ANY, because all
        # defined ID already binded from menu, weird side effect.
        self.Bind(wx.EVT_TOOL, self.OnCompile, compileTool)
        self.Bind(wx.EVT_TOOL, self.OnUpload, uplTool)
        self.Bind(wx.EVT_TOOL, self.OnOutput, outputTool)
        self.Bind(wx.EVT_TOOL, self.OnAddStatement, addStatementTool)
        self.Bind(wx.EVT_TOOL, self.OnAddCondition, addConditionTool)
        self.Bind(wx.EVT_TOOL, self.OnStartListening, listenTool)

    def addExampleMenuItems(self, subMenu, path, maxDepth):
        dirList = os.listdir(path)
        dirList.sort()
        hasAny = False
        for dirName in dirList:
            if dirName[0] == '.': continue
            pathName = os.path.join(path, dirName)
            mode = os.stat(pathName).st_mode
            if not S_ISDIR(mode): continue
            # it's a directory; process it
            dirMenu = wx.Menu()
            # test whether it has a Makefile in it
            if os.path.isfile(os.path.join(pathName, 'Makefile')):
                # append the name of this example
                ex = subMenu.Append(wx.ID_ANY, dirName, pathName)
                # save this id/path combination, to be used in callback code
                self.examples[ex.GetId()] = pathName
                self.Bind(wx.EVT_MENU, self.OnOpenExample, ex)
                hasAny = True
            elif maxDepth:
                # recursive call
                if self.addExampleMenuItems(dirMenu, pathName, maxDepth - 1):
                    hasAny = True
                    subMenu.AppendMenu(wx.ID_ANY, dirName, dirMenu)
        return hasAny

    def generateMenu(self):
        fileMenu = wx.Menu()
        new = fileMenu.Append(wx.ID_NEW, '&' + localize('New') + '\tCtrl+N',
                              localize('Create empty document'))
        open_ = fileMenu.Append(wx.ID_OPEN, '&' + localize('Open') + '\tCtrl+O',
                              localize('Open document'))
        save = fileMenu.Append(wx.ID_SAVE, '&' + localize('Save') + '\tCtrl+S',
                              localize('Save document'))
        saveAs = fileMenu.Append(wx.ID_SAVEAS, '&' + localize('Save as') + '\t',
                              localize('Save document as'))
        upload = fileMenu.Append(wx.ID_ANY, '&' + localize('Upload') + '\tCtrl+U',
                              localize('Open upload window'))
        recent = wx.Menu()
        fileMenu.AppendMenu(wx.ID_ANY, '&' + localize('Recently used files'),
                               recent, localize('Recently used files'))
        close = fileMenu.Append(wx.ID_EXIT, '&' + localize('Exit') + '\tCtrl+Q',
                              localize('Exit application'))

        self.fileHistory = wx.FileHistory(int(Settings.get('recently_opened_count')))
        self.fileHistory.Load(self.API.config)
        self.fileHistory.UseMenu(recent)
        self.fileHistory.AddFilesToMenu()
        self.Bind(wx.EVT_MENU_RANGE, self.on_file_history, id = wx.ID_FILE1, id2 = wx.ID_FILE9)

        # show menu with osw demo & test applications
        exampleMenu = wx.Menu()
        pathToOSWApps = self.API.path + os.path.normcase("/../../apps/")
        self.addExampleMenuItems(exampleMenu, pathToOSWApps, maxDepth = 2)

        optionMenu = wx.Menu()
        language = wx.Menu()
        self.langs = []
        for i in Translater.translations.keys():
            self.langs.append(language.Append(wx.ID_ANY,
                    Translater.translations[i]['langName'], i, kind = wx.ITEM_RADIO))
            if i == Settings.get("active_language"):
                language.Check(self.langs[-1].GetId(), True)
            self.Bind(wx.EVT_MENU, self.changeLanguage, self.langs[-1])

        optionMenu.AppendMenu(wx.ID_ANY, localize('Change language'), language)

        output = optionMenu.Append(wx.ID_ANY, '&' + localize('Configure upload and compile') + '\tCtrl+R',
                              localize('Open read output window'))
        windowMenu = wx.Menu()
        addMenu = wx.Menu()
        showMenu = wx.Menu()
        windowMenu.AppendMenu(wx.ID_ANY, localize('Add window'), addMenu)
        #windowMenu.AppendMenu(wx.ID_ANY, localize('Show window'), showMenu)
        listen = addMenu.Append(wx.ID_ANY, '&' + localize('Add listen window'),
                              localize('Add listen window'))
        self.blocklyCheck = showMenu.AppendCheckItem(wx.ID_ANY, '&' + localize('Show blockly window'),
                              localize('Show blockly window'))
        self.editCheck = showMenu.AppendCheckItem(wx.ID_ANY, '&' + localize('Show edit window'),
                              localize('Show edit window'))

        helpMenu = wx.Menu()
        sealHelp = helpMenu.Append(wx.ID_ANY, '&' + localize('Seal documentation'),
                              localize('About'))
        oswHelp = helpMenu.Append(wx.ID_ANY, '&' + localize('Osw documentation'),
                              localize('About'))
        about = helpMenu.Append(wx.ID_ABOUT, '&' + localize('About') + '\tCtrl+H',
                              localize('About'))
        # Check if we need to update existing menubar(for translate)
        if self.menubar == None:
            self.menubar = wx.MenuBar()
        else:
            for i in range(0, self.menubar.GetMenuCount()):
                self.menubar.Remove(0)

        self.menubar.Append(fileMenu, '&' + localize('File'))
        self.menubar.Append(exampleMenu, '&' + localize('Examples'))
        self.menubar.Append(optionMenu, '&' + localize('Options'))
        self.menubar.Append(windowMenu, '&' + localize('Windows'))
        self.menubar.Append(helpMenu, '&' + localize('Help'))
        self.SetMenuBar(self.menubar)

        # First bind to menu
        self.Bind(wx.EVT_MENU, self.OnQuit, close)
        self.Bind(wx.EVT_MENU, self.OnOpen, open_)
        self.Bind(wx.EVT_MENU, self.OnSave, save)
        self.Bind(wx.EVT_MENU, self.OnSaveAs, saveAs)
        self.Bind(wx.EVT_MENU, self.OnUpload, upload)
        self.Bind(wx.EVT_MENU, self.OnOutput, output)
        self.Bind(wx.EVT_MENU, self.OnNew, new)
        self.Bind(wx.EVT_MENU, self.OnAbout, about)
        self.Bind(wx.EVT_MENU, self.OnSealHelp, sealHelp)
        self.Bind(wx.EVT_MENU, self.OnOSWHelp, oswHelp)

        self.Bind(wx.EVT_MENU, self.API.addListenWindow, listen)
        self.Bind(wx.EVT_MENU, self.API.showBlocklyWindow, self.blocklyCheck)
        self.Bind(wx.EVT_MENU, self.API.showEditWindow, self.editCheck)

    def on_file_history(self, event):
            fileNum = event.GetId() - wx.ID_FILE1
            path = self.fileHistory.GetHistoryFile(fileNum)
            self.fileHistory.AddFileToHistory(path)  # move up the list
            self.API.tabManager.addPage(path)

    def OnQuit(self, event):
        # Workaround, because wx.exit calls wx.ON_CLOSE, which is binded to this 
        # function, result is recursive calling of this function.
        # Time used, because cancel on file save dialog makes exit unresponsive.
        if self.exitCalled + 0.5 > time():
            return
        self.API.tabManager.rememberOpenedTabs()
        self.rememberPositioning()
        if self.API.tabManager.onQuitCheck() == True:
            self.exitCalled = time()
            self.API.performExit()
            wx.Exit()

    def OnSave(self, event):
        self.API.tabManager.doPopupSave(None)

    def OnSaveAs(self, event):
        self.API.tabManager.doPopupSaveAs(None)

    def OnCompile(self, event):
        if self.API.tabManager.doPopupSave(None) == True:
            self.API.doCompile()

    def OnUpload(self, event):
        if self.API.tabManager.doPopupSave(None) == True:
            self.API.doUpload()

    def OnOutput(self, event):
        if self.API.tabManager.doPopupSave(None) == True:

            dialog = wx.Dialog(self, wx.ID_ANY, localize('Configure upload and compile'),
                style = wx.DEFAULT_DIALOG_STYLE | wx.RESIZE_BORDER)

            UploadModule(dialog, self.API)
            dialog.Fit()
            dialog.ShowModal()
            dialog.Destroy()

    def OnStartListening(self, event):
        if not self.API.listenModules[0].listening:
            self.auiManager.ShowPane(self.API.listenModules[0], True)
            self.API.listenModules[0].doClear()
        else:
            self.API.listenModules[0].doClear("")

    def checkToggleState(self):
        if self.API.listenModules[0].listening:
            self.toolbar.ToggleTool(wx.ID_HOME, True)
        else:
            self.toolbar.ToggleTool(wx.ID_HOME, False)
        wx.CallLater(100, self.checkToggleState)

    def OnNew(self, event):
        self.API.tabManager.addPage()
        self.API.editWindow.update()

    def OnOpen(self, event):
        open_ = wx.FileDialog(self,
            localize("Open new document"),
            wildcard = "SEAL {} Osw {} (*.sl, *.c, *.h)|*.sl;*.c;*.h;config|{} (.*)|*".format(
                                                            *localize(["or", "files", "All files"])),

            style = wx.FD_OPEN | wx.FD_MULTIPLE)
        if open_.ShowModal() == wx.ID_OK:
            for x in open_.GetPaths():
                if os.path.exists(x) and os.path.isfile(x):
                    self.API.tabManager.addPage(x)
        open_.Destroy()

    def findFirstSourceFile(self, path):
        # look for files in this order: first main.sl
        filename = os.path.join(path, "main.sl")
        if os.path.isfile(filename): return filename
        # then main.c
        filename = os.path.join(path, "main.c")
        if os.path.isfile(filename): return filename
        # then any other .sl file
        for _ in os.listdir(path):
            filename = os.path.join(path, filename)
            if filename[len(filename) - 3:] == '.sl': return filename
        # then any other .c file
        for _ in os.listdir(path):
            filename = os.path.join(path, filename)
            if filename[len(filename) - 2:] == '.c': return filename
        # then give up
        return None

    def OnOpenExample(self, event):
        path = self.examples.get(event.GetId())
        filename = self.findFirstSourceFile(path)
        if filename:
            self.API.tabManager.addPage(filename)
        else:
            self.API.logMsg(LOG_WARNING, "No source files in {}".format(path))

    def OnAddStatement(self, event):
        self.API.tabManager.getPageObject().code.addStatement()

    def OnAddCondition(self, event):
        self.API.tabManager.getPageObject().code.addCondition()

    def OnAbout(self, event):
        versionFile = os.path.join("../..", "doc/VERSION")
        f = open(versionFile, "r")
        lines = f.readlines()
        f.close()
        release = "Unknown"
        date = "Unknown"
        if len(lines) > 0:
            release = lines[0].strip()
        if len(lines) > 1:
            date = lines[1].strip()
        text = localize("__ABOUT__").format(release, date)
        wx.MessageBox(text, 'Info',
            wx.OK | wx.ICON_INFORMATION)

    def OnSealHelp(self, event):
        filename = "http://open-sci.net/wiki/seal"
        # Damn linux
        if os.name == 'posix':
            Popen(['xdg-open', filename])
        # other OS
        else:
            webbrowser.open_new_tab(filename)

    def OnOSWHelp(self, event):
        filename = "http://open-scri.net/wiki"
        # Damn linux
        if os.name == 'posix':
            Popen(['xdg-open', filename])
        # other OS
        else:
            webbrowser.open_new_tab(filename)

    def changeLanguage(self, event):
        for i in self.langs:
            if i.IsChecked() == True:
                Settings.set("active_language", i.GetHelp())
                self.initUI()

    def disableAdders(self):
        self.toolbar.EnableTool(wx.ID_ADD, False)
        self.toolbar.EnableTool(wx.ID_APPLY, False)
        if self.API.loaded:
            self.API.frame.auiManager.ShowPane(self.API.editPanel, False)

    def enableAdders(self):
        self.toolbar.EnableTool(wx.ID_ADD, True)
        self.toolbar.EnableTool(wx.ID_APPLY, True)
        if self.API.loaded:
            self.API.frame.auiManager.ShowPane(self.API.editPanel, True)

    def rememberPositioning(self):
        # This approach have a small bug.
        #    1. Maximize window
        #    2. Exit
        #    3. Reopen
        #    4. Re-maximize
        # Result is that window stays with maximized height and width, 
        # but is not maximized.
        # TODO: FIX: cache all sizes when resizing and not in maximized mode.
        Settings.set("Width", self.GetSize()[0])
        Settings.set("Height", self.GetSize()[1])
        Settings.set("LocX", self.GetScreenPositionTuple()[0])
        Settings.set("LocY", self.GetScreenPositionTuple()[1])
        Settings.set("Maximized", self.IsMaximized())

    def loadPositioning(self):
        width = Settings.get("Width")
        if width == '':
            width = 800
        else:
            width = int(width)

        height = Settings.get("Height")
        if height == '':
            height = 600
        else:
            height = int(height)

        self.SetSize((width, height))

        locX = Settings.get("LocX")
        if locX == '':
            # Center
            locX = (wx.GetDisplaySize()[0] - width) / 2
        else:
            locX = int(locX)

        locY = Settings.get("LocY")
        if locY == '':
            # Center
            locY = (wx.GetDisplaySize()[1] - height) / 2
        else:
            locY = int(locY)

        self.SetPosition((locX, locY))

        maximized = Settings.get("Maximized")
        if maximized == '':
            maximized = False
        else:
            maximized = bool(maximized == "True")

        self.Maximize(maximized)

    def deactivateNoEditorMode(self):
        self.toolbar.EnableTool(wx.ID_SAVE, True)
        self.toolbar.EnableTool(wx.ID_ADD, True)
        self.toolbar.EnableTool(wx.ID_APPLY, True)
        self.toolbar.EnableTool(wx.ID_PREVIEW, True)
        self.toolbar.EnableTool(wx.ID_PREVIEW_GOTO, True)
        self.toolbar.EnableTool(wx.ID_PREVIEW_ZOOM, True)

    def activateNoEditorMode(self):
        self.toolbar.EnableTool(wx.ID_SAVE, False)
        self.toolbar.EnableTool(wx.ID_ADD, False)
        self.toolbar.EnableTool(wx.ID_APPLY, False)
        self.toolbar.EnableTool(wx.ID_PREVIEW, False)
        self.toolbar.EnableTool(wx.ID_PREVIEW_GOTO, False)
        self.toolbar.EnableTool(wx.ID_PREVIEW_ZOOM, False)
        if self.API.loaded:
            self.API.frame.auiManager.ShowPane(self.API.editPanel, False)

    #def OnBlocklyTool(self, event):
    #    wx.YieldIfNeeded()
    #    self.auiManager.RestorePane(self.blocklyPane)
    #    self.blocklyToolVisible = not self.blocklyToolVisible

    def layoutBlocklyPane(self):
        if not self.API.foundBlockly:
            return;
        blocklyPane = (aui.AuiPaneInfo().Caption(localize("Seal-Blockly handler")).
                Name("blocklyPane").BestSize(wx.Size(500, 150)))
        self.auiManager.AddPane(self.API.blockly, blocklyPane,
                                target = self.infoPane)

    def layoutListenPane(self, listenModule, name, floating = True):
        listenPane = (aui.AuiPaneInfo().BestSize(wx.Size(500, 150)).
                Caption(name).Name(name).MinimizeButton(True).
                MaximizeButton(True).CloseButton(True))
        if floating:
            listenPane.Float()
        self.auiManager.AddPane(listenModule, listenPane, target = self.infoPane)

    def layoutEditPane(self):
        self.rightPane = (aui.AuiPaneInfo().Floatable(True).
                Right().Name("editPane").MinimizeButton(True).
                MaximizeButton(True).CloseButton(True).Caption(localize("Visual edit")).
                BestSize(wx.Size(320, 400)))
        self.auiManager.AddPane(self.API.editPanel, self.rightPane)
