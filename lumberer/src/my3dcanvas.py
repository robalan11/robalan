import wx
from wx import glcanvas

from OpenGL.GL import *
from OpenGL.GLUT import *

class My3DCanvas(glcanvas.GLCanvas):
    def __init__(self, parent):
        glcanvas.GLCanvas.__init__(self, parent, -1)
        self.init = False
        # initial mouse position
        self.lastx = self.x = 30
        self.lasty = self.y = 30
        self.size = None
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnMouseDown)
        self.Bind(wx.EVT_LEFT_UP, self.OnMouseUp)
        self.Bind(wx.EVT_MOTION, self.OnMouseMotion)
    
    
    def InitGL(self):
        # set viewing projection
        glMatrixMode(GL_PROJECTION)
        glFrustum(-0.3, 0.3, -0.3, 0.3, 1.0, 5.0)

        # position viewer
        glMatrixMode(GL_MODELVIEW)
        glTranslatef(0.0, 0.0, -3.0)

        # position object
        glRotatef(self.y, 1.0, 0.0, 0.0)
        glRotatef(self.x, 0.0, 1.0, 0.0)

        glClearColor(1.0, 1.0, 1.0, 1.0)
        
        #glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE)
        #glEnable (GL_COLOR_MATERIAL)
        glEnable(GL_DEPTH_TEST)
        #glEnable(GL_LIGHTING)
        #glEnable(GL_LIGHT0)
        glDisable(GL_LIGHTING)
        glLineWidth(2)


    def OnEraseBackground(self, event):
        pass # Do nothing, to avoid flashing on MSW.


    def OnSize(self, event):
        size = self.size = self.GetClientSize()
        if self.GetContext():
            self.SetCurrent()
            glViewport(0, 0, size.width, size.height)
        event.Skip()


    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        self.SetCurrent()
        if not self.init:
            self.InitGL()
            self.init = True
        self.OnDraw()


    def OnDraw(self):
        # clear color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        self.DrawBoard((-0.5, -0.5, -0.5), (0.5, 0.5, 0.5))
        
        if self.size is None:
            self.size = self.GetClientSize()
        w, h = self.size
        w = max(w, 1.0)
        h = max(h, 1.0)
        xScale = 180.0 / w
        yScale = 180.0 / h
        glRotatef((self.y - self.lasty) * yScale, 1.0, 0.0, 0.0);
        glRotatef((self.x - self.lastx) * xScale, 0.0, 1.0, 0.0);

        self.SwapBuffers()


    def DrawBoard(self, corner1, corner2):
        self.DrawRectPrism(corner1, corner2)
        

    def DrawRectPrism(self, corner1, corner2):
        #glColor3f(1.0, 1.0, 1.0)
        glColor3f(0.97,0.97,0.97)
        glBegin(GL_QUADS)
        glVertex3f( corner2[0], corner2[1], corner2[2])
        glVertex3f( corner1[0], corner2[1], corner2[2])
        glVertex3f( corner1[0], corner1[1], corner2[2])
        glVertex3f( corner2[0], corner1[1], corner2[2])

        glVertex3f( corner1[0], corner1[1], corner1[2])
        glVertex3f( corner1[0], corner2[1], corner1[2])
        glVertex3f( corner2[0], corner2[1], corner1[2])
        glVertex3f( corner2[0], corner1[1], corner1[2])

        glVertex3f( corner2[0], corner2[1], corner2[2])
        glVertex3f( corner2[0], corner2[1], corner1[2])
        glVertex3f( corner1[0], corner2[1], corner1[2])
        glVertex3f( corner1[0], corner2[1], corner2[2])

        glVertex3f( corner1[0], corner1[1], corner1[2])
        glVertex3f( corner2[0], corner1[1], corner1[2])
        glVertex3f( corner2[0], corner1[1], corner2[2])
        glVertex3f( corner1[0], corner1[1], corner2[2])

        glVertex3f( corner2[0], corner2[1], corner2[2])
        glVertex3f( corner2[0], corner1[1], corner2[2])
        glVertex3f( corner2[0], corner1[1], corner1[2])
        glVertex3f( corner2[0], corner2[1], corner1[2])

        glVertex3f( corner1[0], corner1[1], corner1[2])
        glVertex3f( corner1[0], corner1[1], corner2[2])
        glVertex3f( corner1[0], corner2[1], corner2[2])
        glVertex3f( corner1[0], corner2[1], corner1[2])
        glEnd()
        
        glColor3f(0.0,0.0,0.0)
        glBegin(GL_LINES)
        glVertex3f( corner2[0], corner2[1], corner2[2])
        glVertex3f( corner1[0], corner2[1], corner2[2])
        
        glVertex3f( corner1[0], corner2[1], corner2[2])
        glVertex3f( corner1[0], corner1[1], corner2[2])
        
        glVertex3f( corner1[0], corner1[1], corner2[2])
        glVertex3f( corner2[0], corner1[1], corner2[2])
        
        glVertex3f( corner2[0], corner1[1], corner2[2])
        glVertex3f( corner2[0], corner2[1], corner2[2])
        

        glVertex3f( corner1[0], corner1[1], corner1[2])
        glVertex3f( corner1[0], corner2[1], corner1[2])
        
        glVertex3f( corner1[0], corner2[1], corner1[2])
        glVertex3f( corner2[0], corner2[1], corner1[2])
        
        glVertex3f( corner2[0], corner2[1], corner1[2])
        glVertex3f( corner2[0], corner1[1], corner1[2])
        
        glVertex3f( corner2[0], corner1[1], corner1[2])
        glVertex3f( corner1[0], corner1[1], corner1[2])
        
        
        glVertex3f( corner2[0], corner2[1], corner2[2])
        glVertex3f( corner2[0], corner2[1], corner1[2])
        
        glVertex3f( corner1[0], corner2[1], corner2[2])
        glVertex3f( corner1[0], corner2[1], corner1[2])
        
        glVertex3f( corner1[0], corner1[1], corner2[2])
        glVertex3f( corner1[0], corner1[1], corner1[2])
        
        glVertex3f( corner2[0], corner1[1], corner2[2])
        glVertex3f( corner2[0], corner1[1], corner1[2])
        glEnd()


    def OnMouseDown(self, evt):
        self.CaptureMouse()
        self.x, self.y = self.lastx, self.lasty = evt.GetPosition()


    def OnMouseUp(self, evt):
        self.ReleaseMouse()


    def OnMouseMotion(self, evt):
        if evt.Dragging() and evt.LeftIsDown():
            self.lastx, self.lasty = self.x, self.y
            self.x, self.y = evt.GetPosition()
            self.Refresh(False)