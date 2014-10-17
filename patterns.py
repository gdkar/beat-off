import lightstrip
import colorsys
import math

def mkcolor(h,s=1.,v=1.):
    return colorsys.hsv_to_rgb(h,s,v)

class Full(object):
    name='full'
    controls=[
        'color', 
    ]

    def render(self,t,color):
        (r,g,b)=mkcolor(color)
        strip=[(r,g,b,1)]*lightstrip.STRIP_LENGTH
        return strip

class Segment(object):
    name='segment'
    controls=[
        'color',
        'start',
        'stop'
    ]

    def render(self,t,color,start,stop):
        (r,g,b)=mkcolor(color)
        strip=[(0,0,0,0)]*lightstrip.STRIP_LENGTH
        ends=(int(start*lightstrip.STRIP_LENGTH),int((1.-stop)*lightstrip.STRIP_LENGTH))
        for i in range(min(ends),max(ends)):
            strip[i]=(r,g,b,1)
        return strip

class Bounce(object):
    name='bounce'
    controls=[
        'color',
        'size',
        'velocity'
    ]

    def __init__(self):
        self.lt=0
        self.x=0

    initial_gamma = 0.4

    def map_gamma(self, g):
        return 0.01 + 6 * g

    def render(self, t, color, size, velocity):
        def f(x):
            return (math.sin(x*2*math.pi)+1)/2

        (r,g,b)=mkcolor(color)
        strip=[]
        dt=t-self.lt
        self.lt=t
        self.x+=dt*velocity
        
        pos = f(self.x) * (1.0 - size) 
        strip=[(0,0,0,0)]*lightstrip.STRIP_LENGTH
        start = pos 
        stop = pos + size 
        
        ends=(start*lightstrip.STRIP_LENGTH,stop*lightstrip.STRIP_LENGTH)
        low, high = min(ends), max(ends)
        for i in range(int(low),int(high)):
            strip[i]=(r,g,b,1)
        strip[int(low)] = (r, g, b, 1.0 - (low - int(low)))
        strip[int(high)] = (r, g, b, high - int(high))
        return strip


class Wave(object):
    name='wave'
    controls=[
        'color',
        'frequency',
        'velocity',
        'gamma'
    ]

    def __init__(self):
        self.lt=0
        self.x=0

    initial_gamma = 0.4

    def map_gamma(self, g):
        return 0.01 + 6 * g

    def render(self,t,color,frequency,velocity, gamma):
        def f(x):
            return (math.sin(x*2*math.pi)+1)/2

        (r,g,b)=mkcolor(color)
        strip=[]
        dt=t-self.lt
        self.lt=t
        fr=frequency*10
        self.x+=dt*velocity*4*fr
        for i in range(lightstrip.STRIP_LENGTH):
            d=float(i)/lightstrip.STRIP_LENGTH
            a=f(d*fr+self.x) ** gamma
            strip.append((r,g,b,a))
        return strip

class Strobe(object):
    name='strobe'
    controls=[
        'color',
        'frequency',
        'up',
        'down',
    ]

    def __init__(self):
        self.lt=0
        self.lx=0
        self.x=0

    def render(self,t,color,frequency,up,down):
        dt=t-self.lt
        self.lt=t
        x=self.x+dt*frequency*4
        period=1
        self.x=x
        #print x

        (r,g,b)=mkcolor(color)
        if x>=self.lx+period:
            strip=[(r,g,b,1)]*lightstrip.STRIP_LENGTH
            self.lx=math.floor(x/period)*period
        else:
            nx=(math.ceil(x/period)*period-x)/period
            px=(x-math.floor(x/period)*period)/period
            a=0
            if up > 0 and nx < up:
                a+=1-nx/up
            if down > 0 and px < down:
                a+=1-px/down
            strip=[(r,g,b,a)]*lightstrip.STRIP_LENGTH
        return strip

