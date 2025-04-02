
import threading
import time

class TimeHandler:
    def __init__(self):
        self.timers = []
        
    def start_timer(self, name: str, duration: int, callback=None):
        """Starts a new timer that runs for duration seconds"""
        stop_event = threading.Event()
    
        
        def timer_thread():
            """Creates a timer for a thread"""
            if not stop_event.wait(duration):
                if callback:
                    callback()
                _timer = self.remove_timer(name)
                del _timer
            
            
                    
        # Creating and starts the thread and adds it to self.timers
        thread = threading.Thread(target=timer_thread, daemon=True)
        self.timers.append([name, thread, stop_event])
        thread.start()
        
    def cancel_timer(self, name: str):
        """Cancels the timer with the name name if it exists"""
        _timer = self.remove_timer(name)
        if _timer:
            _timer[2].set()
            del _timer
            
    def cancel_timers(self):
        """Cancels all the timers"""
        if len(self.timers) != 0:
            _timer = self.timers.pop(0)
            _timer[2].set()
            del _timer
            self.cancel_timers()
        return
    
    def remove_timer(self, name: str):
        """Returns and removes a timer with the name name from self.timers"""
        for _timer in self.timers:
            if _timer[0] == name:
                self.timers.remove(_timer)
                return _timer
        return None
    
    def list_timers(self):
        """Returns a list of strings name from every active timer"""
        timers = []
        for _timer in self.timers:
            timers.append(_timer[0])
            
        return timers
    
    def get_timer_threads(self):
        """Returns a list of treads for every active timer"""
        timers = []
        for _timer in self.timers:
            timers.append(_timer[1])
            
        return timers
    
    def get_timers(self):
        """Returns a list of every active timer"""
        timers = []
        for _timer in self.timers:
            timers.append(_timer)
            
        return timers
    
    def get_timer(self, name: str):
        """Returns the timer with the name name if it exists"""
        for _timer in self.timers:
            if _timer[0] == name:
                return _timer
    
    @staticmethod
    def toSeconds(sec=0, mins=0, hours=0, days=0):
        """Returns the total amount of time given into seconds"""
        t = 0
        t += sec
        t += mins * 60
        t += hours * 3600
        t += days * 86400
        return t
            
        