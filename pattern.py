class Pattern:
	def __init__(self,oa,length=4,events=[],name='unnamed'):
		self.length=length
		self.events=events
		self.oa=oa
		self.name=name
		self.active_events=[]
		self.last_tick=0.

	def remove(self):
		for event_stop,res in self.active_events:
			self.oa.remove_effect(res)
		self.active_events=[]

	def get_events(self,begin,end):
		prev_iter=int(begin / self.length)
		cur_iter=int(end / self.length)
		return [(timestamp+self.length*i,duration,eff) for i in range(prev_iter,cur_iter+1) for timestamp,duration,eff in self.events]

	def process(self,tick):
		if tick<=self.last_tick:
			return
		prev_iter=int(self.last_tick / self.length)
		cur_iter=int(tick / self.length)

		for event_stop,res in self.active_events:
			if tick > event_stop:
				self.oa.remove_effect(res)
		self.active_events=[(event_stop,res) for event_stop,res in self.active_events if tick <= event_stop]

		#for i in range(prev_iter,cur_iter+1):
		#	for timestamp,duration,eff in self.events:
		#		start=timestamp+self.length*i
		#		stop=timestamp+self.length*i+duration
		for start,duration,eff in self.get_events(self.last_tick,tick):
				stop=start+duration
				if start >= self.last_tick and start < tick and stop > tick:
					if eff.persistent:
						res=self.oa.add_persistent_effect(eff.cmd,eff.data)
						self.active_events.append((stop,res))
					else:
						res=self.oa.add_transient_effect(eff.cmd,eff.data)
		self.last_tick=tick

