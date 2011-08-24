#!/usr/bin/env python
import wx
import os
   
import my3dcanvas

class MainWindow(wx.Frame):
    """ We simply derive a new class of Frame. """
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title)
        
        self.mainSplitter = wx.SplitterWindow(self, wx.ID_DEFAULT)
        self.mainSplitter.SetMinimumPaneSize(40)
        self.infoSplitter = wx.SplitterWindow(self.mainSplitter, wx.ID_DEFAULT)
        self.infoSplitter.SetMinimumPaneSize(40)
        
        self.control = wx.TextCtrl(self.infoSplitter, style=wx.TE_MULTILINE)
        self.otherControl = wx.TextCtrl(self.infoSplitter, style=wx.TE_MULTILINE)
        
        self.mainDisplay = my3dcanvas.My3DCanvas(self.mainSplitter)
                    
        self.infoSplitter.SplitHorizontally(self.control, self.otherControl)        
        self.mainSplitter.SplitVertically(self.mainDisplay, self.infoSplitter)
        
        self.CreateMenu()
        self.CreateToolBar()
        self.CreateStatusBar()
        
        self.Show(True)

    def CreateMenu(self):
        fileMenu = wx.Menu()
        openItem = fileMenu.Append(wx.ID_OPEN, "&Open...", "Open a project")
        self.Bind(wx.EVT_MENU, self.OnOpen, openItem)
        saveAsItem = fileMenu.Append(wx.ID_SAVE, "&Save as...", "Save the current project")
        self.Bind(wx.EVT_MENU, self.OnSaveAs, saveAsItem)
        quitItem = fileMenu.Append(wx.ID_EXIT, "&Quit", "Quit the application")
        self.Bind(wx.EVT_MENU, self.OnExit, quitItem)
        
        helpMenu = wx.Menu()
        helpItem = helpMenu.Append(wx.ID_ABOUT, "&About", "Information about this application")
        self.Bind(wx.EVT_MENU, self.OnAbout, helpItem)
        
        menuBar = wx.MenuBar()
        menuBar.Append(fileMenu, "&File")
        menuBar.Append(helpMenu, "&Help")
        
        self.SetMenuBar(menuBar)

    def OnExit(self, event):
        """Exit the application"""
        self.Close(True)
        
    def OnAbout(self, event):
        """Display the About dialog"""
        dlg = wx.MessageDialog( self, "A small application, currently a text editor", "About Lumberer", wx.OK)
        dlg.ShowModal() # Show it
        dlg.Destroy() # finally destroy it when finished.
        
    def OnOpen(self, event):
        """Open a project"""
        self.dirname = ''
        dlg = wx.FileDialog(self, "Choose a file", self.dirname, "", "*.*", wx.OPEN)
        if dlg.ShowModal() == wx.ID_OK:
            self.filename = dlg.GetFilename()
            self.dirname = dlg.GetDirectory()
            f = open(os.path.join(self.dirname, self.filename), 'r')
            self.control.SetValue(f.read())
            f.close()
        dlg.Destroy()
        
    def OnSaveAs(self, event):
        """Save a project"""
        self.dirname = ''
        dlg = wx.FileDialog(self, "Choose a file", self.dirname, "", "*.*", wx.SAVE)
        if dlg.ShowModal() == wx.ID_OK:
            self.filename = dlg.GetFilename()
            self.dirname = dlg.GetDirectory()
            f = open(os.path.join(self.dirname, self.filename), 'w')
            f.write(self.control.GetValue())
            f.close()
        dlg.Destroy()

app = wx.App(False)
frame = MainWindow(None, 'Lumberer')
app.MainLoop()