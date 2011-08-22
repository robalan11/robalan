#!/usr/bin/env python
import wx
import os

class MainWindow(wx.Frame):
    """ We simply derive a new class of Frame. """
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title, size=(800,600))
        self.control = wx.TextCtrl(self, style=wx.TE_MULTILINE)
        self.CreateStatusBar()
        self.CreateToolBar()
        
        self.initMenu()
        
        self.Show(True)

    def initMenu(self):
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
        dlg = wx.MessageDialog( self, "A small application, currently a text editor", "About Simple App", wx.OK)
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
frame = MainWindow(None, 'Small editor')
app.MainLoop()