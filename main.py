import pygame
import patterns
import device
import compositor
import lightstrip
import ui
import time
import nanokontrol
import audio

pygame.init()
pygame.midi.init()

screen=ui.init()

framerate = pygame.time.Clock()
go = True

nk2=nanokontrol.NanoKontrol2()
nk_knob_map={'Knob0':0,'Knob1':1,'Knob2':2,'Knob3':3,'Knob4':4,'Knob5':5,'Knob6':6,'Knob7':7}
nk_slider_map={'Slider0':0,'Slider1':1,'Slider2':2,'Slider3':3,'Slider4':4,'Slider5':5,'Slider6':6,'Slider7':7}

pattern_bank=[patterns.Full,patterns.Segment,patterns.Wave,patterns.Strobe]

ah=audio.AudioHandler()

class Control:
    def __init__(self,name,value=0.0):
        self.name=name
        self.value=value
        self.knob=None

    def to_pair(self):
        return (self.name,self.value)

    def set_knob(self,knob):
        if self.knob is not None:
            self.knob.del_control(self)
        if knob is not None:
            knob.add_control(self)
        self.knob=knob

class Knob:
    def __init__(self,name):
        self.name=name
        self.controls=set()

    def add_control(self,control):
        self.controls.add(control)

    def del_control(self,control):
        self.controls.remove(control)

    def update(self,value):
        for c in self.controls:
            c.value=value

class Slot:
    def __init__(self,pattern):
        self.pat = pattern
        self.controls = [Control(c) for c in pattern.controls]
        self.alpha_control = Control('alpha')
        self.ui = ui.Pattern(self)

    def update_pattern(self,t):
        self.frame=self.pat.render(t,**dict([c.to_pair() for c in self.controls]))

    def render(self,surf,pos):
        self.ui.render(surf,pos)

class BankPattern:
    def __init__(self,pattern_cls):
        self.pat=pattern_cls
        self.ui=ui.BankPattern(self)

    def render(self,surf,pos):
        self.ui.render(surf,pos)

    def make(self):
        return Slot(self.pat())

bank = [BankPattern(p) for p in pattern_bank]
slots = [None]*8

lo=Knob('lo')
mid=Knob('mid')
hi=Knob('hi')
glob=[lo,mid,hi]
nk_knobs=[Knob('NK '+str(i)) for i in range(len(slots))]
nk_sliders=[Knob('NK S '+str(i)) for i in range(len(slots))]

def knobs(slot):
    return [None]+[nk_knobs[slot]]+glob

def alpha_knobs(slot):
    return [nk_sliders[slot]]

d = device.PrintDevice()

interface=ui.UI(screen,bank,slots,knobs,alpha_knobs)

ah.start()

while go:
    framerate.tick(60)

    t=time.time()

    frames=[]
    for slot in slots:
        if slot is not None:
            slot.update_pattern(t)
            frames.append((slot.frame,slot.alpha_control.value))

    frame = compositor.composite(frames)

    interface.master.frame=frame

    d.render(frame)

    interface.render()

    pygame.display.update()

    for (k,v) in nk2.get_events():
        if k in nk_knob_map:
            nk_knobs[nk_knob_map[k]].update(v)
        if k in nk_slider_map:
            nk_sliders[nk_slider_map[k]].update(v)

    lo.update(ah.lo)
    mid.update(ah.mid)
    hi.update(ah.hi)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            go = False
        if event.type in [pygame.MOUSEBUTTONUP,pygame.MOUSEBUTTONDOWN,pygame.MOUSEMOTION]:
            relpos=event.pos
            interface.mouse(relpos,event)

ah.stop()
