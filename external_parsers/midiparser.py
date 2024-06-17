from MIDI import MIDIFile
from sys import argv

def parse(file):
    c=MIDIFile(file)
    c.parse()
    #print(str(c))
    parse = ""
    lastTick = 0
    noteList=[]
    for idx, track in enumerate(c):
        track.parse()
        print(f'Track {idx}:')
        parse= str(track)
        print(parse)
        for line in parse.splitlines():
            if (idx==2):
                if("NOTE_ON" in line):
                    currentTick = int(line.split(" ")[0].split("@")[1]) 
                    if (currentTick != lastTick):
                        noteList.append(0)
                        noteList.append(max(tickToLen(currentTick-lastTick),1))
                        lastTick = currentTick
                        noteList.append(noteToFreq(line.split(" ")[2]))
                    else:
                        lastTick = currentTick
                        noteList.append(noteToFreq(line.split(" ")[2]))
                elif ("NOTE_OFF" in line):
                    currentTick = int(line.split(" ")[0].split("@")[1]) 
                    noteList.append(max(tickToLen(currentTick - lastTick), 1))
                    lastTick = currentTick
    print(noteList)
    noteDump = str(noteList)
    noteDump = noteDump.replace("[", "{").replace("]", "};")
    print(noteDump)
    dumpFile = open(argv[2],"w")
    dumpFile.write("#ifndef "+str(argv[2]).upper().replace('.', '_')+"\n#define "+str(argv[2]).upper().replace('.', '_')+"\n#include <stdint.h>\n\nuint32_t "+argv[1].split(".")[0]+"length = "+str(len(noteList))+ ";\nuint8_t "+argv[1].split(".")[0]+"[] = ")
    dumpFile.write(noteDump)
    dumpFile.write("\n\n#endif")
    dumpFile.close()
    #dump = bytearray(noteList)
    #print(len(dump))
    #dumplen = len(dump)
    #dump.insert(0, dumplen % 256)
    #dump.insert(1, int((dumplen/256)) % 256)
    #dump.insert(2, int((dumplen/(256**2))) % 256)
    #dump.insert(3, int((dumplen/(256**3))) % 256)

    #dumpFile = open(argv[2], "wb")
    #dumpFile.write(bytes(dump))
    #dumpFile.close()
    #print(dump)

    




def tickToLen(tick):
    return int(tick/4)
    #return int(tick/6)

def noteToFreq(note):
    freq = 0
    coef = 0
    if(len(note)==2):
        if(note[0] == "C"):
            freq = 523.25
        elif(note[0] == "D"):
            freq = 587.33
        elif(note[0] == "E"):
            freq = 659.25
        elif(note[0] == "F"):
            freq = 698.46
        elif(note[0] == "G"):
            freq = 784
        elif (note[0] == "A"):
            freq = 880
        elif(note[0] == "B"):
            freq = 987.77
        coef = int(note[1])
    elif(len(note) == 3):
        if(note[0] == "C"):
            freq = 554.37
        elif(note[0] == "D"):
            freq = 622.25
        elif(note[0] == "F"):
            freq = 740
        elif(note[0] == "G"):
            freq = 830.61
        elif (note[0] == "A"):
            freq = 932.33
        coef = int(note[2])
    freq = freq * (2**(coef - 5))
    return int(freq / 16)

parse(argv[1])