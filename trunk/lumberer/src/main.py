#!/usr/bin/env python
import wx
import os

class MainWindow(wx.Frame):
    """ We simply derive a new class of Frame. """
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title)
        self.control = wx.TextCtrl(self, style=wx.TE_MULTILINE)
        self.mainDisplay = wx.Panel(self)
        self.mainDisplay.Bind(wx.EVT_PAINT, self.OnPaint) 
        
        self.CreateMenu()
        self.CreateToolBar()
        self.CreateStatusBar()
        
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.control, 0, wx.EXPAND)
        self.sizer.Add(self.mainDisplay, 1, wx.EXPAND)
        
        self.SetSizer(self.sizer)
        self.SetAutoLayout(True)
        self.sizer.Fit(self)
        
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
        
    def OnPaint(self, event):
        # establish the painting surface
        dc = wx.PaintDC(self.mainDisplay)
        dc.SetPen(wx.WHITE_PEN)
        dc.SetBrush(wx.WHITE_BRUSH)
        size = self.mainDisplay.GetSize()
        dc.DrawRectangle(0, 0, size[0], size[1])
        dc.SetPen(wx.Pen('blue', 4))
        # draw a blue line (thickness = 4)
        dc.DrawLine(50, 20, 300, 20)
        dc.SetPen(wx.Pen('red', 1))
        # draw a red rounded-rectangle
        rect = wx.Rect(50, 50, 100, 100)
        dc.DrawRoundedRectangleRect(rect, 8)
        # draw a red circle with yellow fill
        dc.SetBrush(wx.Brush('yellow'))
        x = 250
        y = 100
        r = 50
        dc.DrawCircle(x, y, r)

app = wx.App(False)
frame = MainWindow(None, 'Lumberer')
app.MainLoop()