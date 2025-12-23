# Launchpad Pro + Squarp Pyramid
I got annoyed with not being able to mute two tracks on in separate banks simultaneously, this is what I came up with. The Squarp Pyramid is a pretty cool and powerful MIDI sequencer that is going relativly cheaply these days. My main gripe with it (and most stand-alone sequencers so far) is that fleshing loops out to bigger arrangements is tedious at best. I have used some of the MIDI interface offered by the Pyramid to add additional tools for live playing and arrangement.

This replaces the stock firmware on the device. I *think* that a USB MIDI connection to your Squarp Pyramid is assumed and DIN MIDI won't work. As with most things in this, if you care about any of this stuff, just ask me and I'd likely be happy to add whatever's missing for your use case. This was forked from https://github.com/dvhdr/launchpad-pro/tree/master, look there for all of the install / build instructions. 

Enable "PyraMIDI" on your Pyramid, set the channel to the MIDI channel you want to recieve on. Once you connect and power on your Lauchpad Pro, hold down the "Track Select" button on the bottom row and select the MIDI channel you want the Launchpad Pro to transmit on. The left buttons are channels 1-8, counting from top to bottom, the right butons are channels 9-16. (Note: there is a current bug where the channel actually used is one more than the channel you pick. So pick lower than the channel you want for now.)

There are currently three modes of operation, selected with the first three buttons in the top row:
  1) Track mute toggle. Tracks are represented as the four banks of 16, right over each other. Press a pad to send a message to toggle mute state on that track. This follows the track mute timing selected on the Pyramid. Hold setup and press a pad to change its color.
  2) Track mute macros. Hold setup and press a pad to create (or edit) a macro (a group of track mute toggles that will be sent at one time) at that location. A new page will pop up, toggle which tracks are to be included in that macro. If you want, select a color for the macro using the side buttons. Press setup to exit. Now you can press that button to toggle all of those tracks at once.
  3) Track selction. Pick a track and it will be selected on the Pyramid. Hold setup and press a pad to change a color.

In addition, in all modes, the buttons on the right will launch sequences. There are four banks of eight sequences, the bank is selected with the four rightmost buttons on the top row. Hold setup and press a sequence button to change its color.
Open source firmware for the Novation Launchpad Pro grid controller!  By customising this code, you
 can:

