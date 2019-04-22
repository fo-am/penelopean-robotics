import osc,time

bpm=43
#bpm=60
beat=0

#osc.Message("/socketio",[100.0,100.0]).sendlocal(9998)

while 1:
     print("sync")
     osc.Message("/sync",[beat,bpm]).sendlocal(8000)
     delay=(1/(bpm/60.0))
     print(delay)
     beat+=1
     time.sleep(delay)
