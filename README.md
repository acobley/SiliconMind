View this project on [CADLAB.io](https://cadlab.io/node/794). 

# SiliconMind
PCB3 is a test release.  
**Do not use** . Unwired airwire on PCB.
Use the hardware files

Pure Keys aec-pcb3.sch
Pure Keys aec-pcb3.brd

PCB2 is a working release.  Use the hardware files

Pure Keys aec-pcb2.sch
Pure Keys aec-pcb2.brd

**make sure that the 20 way header(keyboard connector) is soldered on so the connector is on the otherside of the board to the components. **

**Button 1** controls the mode. Starts in Polyphonc mode.  
Press once (2 flashes) for split mode.  CV1/Trig1 is lower half of keyboard. CV2/Trig2 is upper
Press again (3 flashes for mono mode.  CV1/Trig 1 only
Press again (1 flash) to return to poly mode.

**Button 2* is record/play mode
Press and hold (button flashes when ready) to enter record mode.  The note on each trig in will be recorded (up to 64), the note length is **not** recorded.  When finished press once to exit record mode.   Press once (quickly) to enter play mode.

For step type recording use a manual trigger press.

**Note** Will only record the bottom note, best used in Mono mode for recording.   In playback mode will output on CV1/Trig1 and other notes are shifted up one.  In Split mode upper half of keyboard is available.

**Glide** 
Controls the Portamento rate.  Works in Mono/Poly mode.  In Split mode only upper half of keyboard is glided.

-------------------------------------------------------------------

This is a project to create a voltage control keyboard for Eurorack Synths.  The aim is to create a fourvoice polyphtonic keyboard, with no MIDI at all.

The hardware is based around the ATMEGA.

See alos Changelog.txt in Hardware

Software 

SiliconMind.ion is latest for aec-pcb2 First PCB hardware

                  Work in progress, in test !

Hardware Designs:

Pure Keys aec Original Design

Pure Keys aec-pcb1 First PCB  do not use

      	       Needs pull down resistors on	MK lines
                   5V Ref needs	 replacing with 3v version
                   Switch needs	 pull up, not down.
Pure Keys aec-pcb2. 

                   Changes in pcb1 are rolled into this
                   Pure Keys aec-pcb2 In Progress

Pure Keys aec-pcb3-inProgress (obsolete)

	Next design in progress
	     	    Changes from pcb2 are rolled into this 
		    
Pure Keys aec-pcb3-smd-inProgress 

	SMD version.   Next design in progress
	     	    Changes from pcb2 are rolled into this 
Pure Keys aec-pcb3 

	SMD version.   **Do not use** Unwired airwire on PCB.
	
	     	    Changes from pcb2 are rolled into this 
		    
