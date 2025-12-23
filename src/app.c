/******************************************************************************
 
 Copyright (c) 2015, Focusrite Audio Engineering Ltd.
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 * Neither the name of Focusrite Audio Engineering Ltd., nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 *****************************************************************************/

//______________________________________________________________________________
//
// Headers
//______________________________________________________________________________

#include "app.h"
#include "pyr.h"


//______________________________________________________________________________
//
// This is where the fun is!  Add your code to the callbacks below to define how
// your app behaves.
//
// In this example, we either render the raw ADC data as LED rainbows or store
// and recall the pad state from flash.
//______________________________________________________________________________

// store ADC frame pointer
// static const u16 *g_ADC = 0;

// buffer to store pad states for flash save
#define BUTTON_COUNT 100


const u8 pads[64] = {
81,82,83,84,85,86,87,88,
71,72,73,74,75,76,77,78,
61,62,63,64,65,66,67,68,
51,52,53,54,55,56,57,58,
41,42,43,44,45,46,47,48,
31,32,33,34,35,36,37,38,
21,22,23,24,25,26,27,28,
11,12,13,14,15,16,17,18};



u8 track_mute_colors[64] = {0};
u8 macro_colors[64] = {0};
u8 defmacro_colors[64] = {0};
u8 track_select_colors[64] = {0};
u8 seq_colors[32] = {0};


u8 *current_colors;

u8 g_macros[64][8] = {0};
u8 g_defmacro = 0;
u8 g_seqBank = 1;
u8 g_colorpicker = 0;
u8 g_colorpicker_seq = 0;
u8 g_colorpicker_index = 0;


const u8 palette_colors[16][3] = {
	{0x00, 0x00, 0x00},
	{0x03, 0x03, 0x03},
	{0x00, 0x10, 0x00},
	{0x00, 0x00, 0x10},
	{0x10, 0x10, 0x00},
	{0x00, 0x10, 0x10},
	{0x10, 0x00, 0x10},
	{0x20, 0x00, 0x00},
	{0x00, 0x20, 0x00},
	{0x00, 0x00, 0x20},
	{0x20, 0x20, 0x00},
	{0x00, 0x20, 0x20},
	{0x30, 0x00, 0x00},
	{0x00, 0x30, 0x00},
	{0x00, 0x30, 0x30},
	{0x3F, 0x3F, 0x3F}
};

const u8 muted_palette_colors[16][3] = {
	{0x00, 0x00, 0x00},
	{0x0A, 0x00, 0x00},
	{0x00, 0x0A, 0x00},
	{0x00, 0x00, 0x0A},
	{0x0A, 0x0A, 0x00},
	{0x00, 0x0A, 0x0A},
	{0x0A, 0x00, 0x0A},
	{0x1A, 0x00, 0x00},
	{0x00, 0x1A, 0x00},
	{0x00, 0x00, 0x1A},
	{0x1A, 0x1A, 0x00},
	{0x00, 0x1A, 0x1A},
	{0x2A, 0x00, 0x00},
	{0x00, 0x2A, 0x00},
	{0x00, 0x2A, 0x2A},
	{0x3F, 0x3F, 0x3F}
};



const u8 mute_row_colors[4] = {1,4,8,12};



u8 g_Channel = 13;
u8 g_Buttons[BUTTON_COUNT] = {0};
u8 g_Setup = 0;
u8 g_Pressed[BUTTON_COUNT] = {0};
u8 g_Mode = MODE_TRACKMUTE;
u8 g_x = 0;
u8 g_y = 0;
u8 g_Dirty = 0;

KeyInfo g_Key;

//______________________________________________________________________________
u8 getBit(u8 byte, u8 bitNum)
{
    return (byte & (0x1 << (bitNum - 1)));
}


void init_track_mute_colors() {
	for (int i=0; i < 64; ++i) {
		switch (i / 16) {
			case 0: track_mute_colors[i] = 0; break;
			case 1: track_mute_colors[i] = 1; break;
			case 2: track_mute_colors[i] = 0; break;
			case 3: track_mute_colors[i] = 1; break;
		}
	}
}

void init_macro_colors() {
	for (int i=0; i < 64; ++i) {
		macro_colors[i] = COLOR_UNSETMACRO;
	}
}	

void init_track_select_colors() {
	for (int i=0; i < 64; ++i) {
		switch (i / 16) {
			case 0: track_select_colors[i] = 0; break;
			case 1: track_select_colors[i] = 1; break;
			case 2: track_select_colors[i] = 0; break;
			case 3: track_select_colors[i] = 1; break;
		}
	}
}

// We have macro_colors but it might be missing a new macro or have one defined that no longer has any values

void update_macro_colors() {
	u8 have_macro = 0;
	for (int i=0; i < 64; ++i) {  // for each macro
		for (int j=0; j < 8; ++j) {  // need to look at each row to figure out if it's active
			if (g_macros[i][j]) {   // if we have a macro
				have_macro = 1; 
				if (macro_colors[i] == COLOR_UNSETMACRO) { 	// see if we already have a color
					macro_colors[i] = COLOR_SETMACRO;	// if not, set it our default
				}
				break;  // if we know ww have a macro here, we can bail early.
			}
		} // back to check the next row
		if (!have_macro) { 				// been through all the rows, still no macro so we can set the color to empty
			macro_colors[i] = COLOR_UNSETMACRO;
		}
		have_macro = 0;  // reload for the next one
	}
}


// a bit simplified color model because we're not offering color changes for different steps in one macro. 
void update_defmacro_colors(u8 number) {
	for (int i=0; i < 64; ++i) {
		switch (i / 16) {
			case 0: defmacro_colors[i] = 0; break;
			case 1: defmacro_colors[i] = 1; break;
			case 2: defmacro_colors[i] = 0; break;
			case 3: defmacro_colors[i] = 1; break;
		}
	}
	for (int i=0; i < 8; ++i) {
		for (int j=0; j < 8; ++j) {
			if (getBit(g_macros[number][i],8-j)) {
				defmacro_colors[(i*8)+j] = COLOR_DEFMACROFG;
			}
		}
	}
}

void pad_light(u8 index, u8 ix) {
	hal_plot_led(TYPEPAD,index, palette_colors[ix][0], palette_colors[ix][1], palette_colors[ix][2]);
}

void pad_light_muted(u8 index, u8 ix) {
	hal_plot_led(TYPEPAD,index, muted_palette_colors[ix][0], muted_palette_colors[ix][1], muted_palette_colors[ix][2]);
}






// set all pad lights to current_colors

void update_pad_lights() {
	for (int i=0; i < 64; ++i)
	{
		pad_light(pads[i],current_colors[i]);
	}
}

// set all the side and top buttons to what they should be.
// also apparently sets the current_colors pointer to the appropriate array based on the current mode. not sure this should be here.

void update_control_lights() {
	switch (g_Mode) {
			case MODE_TRACKMUTE:
				pad_light(91,1); pad_light(92,0); pad_light(93,0); pad_light(94,0);
//				current_colors = track_mute_colors;
				break;
			case MODE_MACRO:
				pad_light(91,0); pad_light(92,1); pad_light(93,0); pad_light(94,0);
//				current_colors = macro_colors;
				break;
			case MODE_TRACKSELECT:
				pad_light(91,0); pad_light(92,0); pad_light(93,1); pad_light(94,0);
//				current_colors = track_select_colors;
				break;			
	}
	
	// sequence banks
	pad_light(95,0); pad_light(96,0); pad_light(97,0); pad_light(98,0);
	pad_light(94 + g_seqBank, 2);

	// left lights (do nothing now)
	pad_light(80,0);
	pad_light(70,0);
	pad_light(60,0);
	pad_light(50,0);
	pad_light(40,0);
	pad_light(30,0);
	pad_light(20,0);
	pad_light(10,0);
	
	// sequence lights
	pad_light(89,seq_colors[((g_seqBank - 1) * 8) + 0]);
	pad_light(79,seq_colors[((g_seqBank - 1) * 8) + 1]);
	pad_light(69,seq_colors[((g_seqBank - 1) * 8) + 2]);
	pad_light(59,seq_colors[((g_seqBank - 1) * 8) + 3]);
	pad_light(49,seq_colors[((g_seqBank - 1) * 8) + 4]);
	pad_light(39,seq_colors[((g_seqBank - 1) * 8) + 5]);
	pad_light(29,seq_colors[((g_seqBank - 1) * 8) + 6]);
	pad_light(19,seq_colors[((g_seqBank - 1) * 8) + 7]);
}

/* 	our color picker. this will set a global flag to indicate that it's active. don't want to use mode because we want to preserve
	the mode so we know where to go back to when we're done and where to put the color we're picking.
	
	this should be for all uses of the color picker. the one for picking a seq color gets a wrapper because sequences are always active
	regardless of mode.

*/



void show_channel_lights() {
		for (int i=0; i < 8; ++i) {
			pad_light(10 * (i+1), 0);
			pad_light((10 * (i+1)) +9 , 0);
		}
		if (g_Channel <= 8) {
			pad_light(10 * (9 - g_Channel), 3);
		} else {
			pad_light((10 * (9 - (g_Channel - 8))) + 9, 3);
		}
}


		
void play_macro(u8 number) {
	for (int i=0; i < 8; ++i) {
		if (g_macros[number][i]) {
			for (int j=0; j < 8; ++j) {
				if (getBit(g_macros[number][i],8-j)) {
					hal_send_midi(USBSTANDALONE, NOTEON | g_Channel , (i * 8) + j + 36, 1);
					hal_send_midi(USBSTANDALONE, NOTEOFF | g_Channel , (i * 8) + j + 36, 1);
				}
			}
		}
	}
}

void toggle_macro_track(u8 index) {
	u8 row = index / 8;
	u8 col = index % 8;
	g_macros[g_defmacro][row] ^= (0x1 << (8-col-1));
}

/****************/
/* COLOR PICKER */
/****************/

void show_color_picker(u8 current) {
		g_colorpicker = 1;
		pad_light(80, 0);
		pad_light(70, 1);
		pad_light(60, 2);
		pad_light(50, 3);
		pad_light(40, 4);
		pad_light(30, 5);
		pad_light(20, 6);
		pad_light(10, 7);
		pad_light(89, 8);
		pad_light(79, 9);
		pad_light(69,10);
		pad_light(59,11);
		pad_light(49,12);
		pad_light(39,13);
		pad_light(29,14);
		pad_light(19,15);
}

void show_seq_color_picker(u8 current) {
		g_colorpicker_seq = 1;
		show_color_picker(current);
}

void handle_color_picker_event() {
	u8 offset = 0;
	u8 value = 0;
	switch (g_Key.type) {
		case KEYTYPE_SETUP:
		case KEYTYPE_CHANNEL:
		case KEYTYPE_MODE:
		case KEYTYPE_SEQBANK:
		case KEYTYPE_BOTTOM:
			g_colorpicker = 0;
			update_control_lights();
			break;
		case KEYTYPE_SEQ:
			offset = 8;
		case KEYTYPE_LEFT:
			value = g_Key.index + offset;
			if (g_colorpicker_seq) {
				seq_colors[g_colorpicker_index] = value - 1;
			} else { 
				switch (g_Mode) {
					case MODE_TRACKMUTE:
						track_mute_colors[g_colorpicker_index] = value - 1;
						update_pad_lights();
						break;
					case MODE_MACRO:
					case MODE_DEFMACRO:
						macro_colors[g_colorpicker_index] = value - 1;
						break;
					case MODE_TRACKSELECT:
						track_select_colors[g_colorpicker_index] = value - 1;
						update_pad_lights();
						break;
				}
				update_control_lights();
			}
			break;
		case KEYTYPE_PAD:	
			g_colorpicker_seq = 0;
			g_colorpicker_index = g_Key.index - 1;
			show_color_picker(current_colors[g_colorpicker_index -1]);
			break;
	}
}



void change_mode(u8 mode) {
	switch (mode) {
		case MODE_TRACKMUTE:
			g_Mode = MODE_TRACKMUTE;
			current_colors = track_mute_colors;
			update_control_lights();
			update_pad_lights();
			break;
		case MODE_MACRO:
			g_Mode = MODE_MACRO;
			update_macro_colors();
			current_colors = macro_colors;
			update_control_lights();
			update_pad_lights();
			break;
        case MODE_TRACKSELECT:
			g_Mode = MODE_TRACKSELECT;
			current_colors = track_select_colors;
			update_control_lights();
			update_pad_lights();
			break;
		case MODE_DEFMACRO:
			g_Mode = MODE_DEFMACRO;
			update_defmacro_colors(g_defmacro);
			current_colors = defmacro_colors;
			update_pad_lights();
			show_color_picker(current_colors[g_defmacro]);
	}
}


//  take what's given to us and break out the things we need to know about the current event into a global struct

void parse_key(u8 index)
{
	u8 x = index % 10; 
	u8 y = index / 10;
	
	g_Key.hal_index = index;

	if (index == 2) {
		g_Key.type = KEYTYPE_CHANNEL;
		g_Key.index = 0;
	} else if (y == 9) {
		if (x >= 1 && x <= 4) {
			g_Key.type = KEYTYPE_MODE;
			g_Key.index = index - 90;
		} else {
			g_Key.type = KEYTYPE_SEQBANK;
			g_Key.index = index - 94;
		}
	} else if (y == 0) {
		g_Key.type = KEYTYPE_BOTTOM;
		g_Key.index = x;
	} else if (x == 0) {
		g_Key.type = KEYTYPE_LEFT;
		g_Key.index = 9 - y;
	} else if (x == 9) {
		g_Key.type = KEYTYPE_SEQ;
		g_Key.index = 9 - y;
	} else {
		g_Key.type = KEYTYPE_PAD;
		g_Key.index = ((8 - y) * 8) + x;
	}
}

// main event for ui ops


void app_surface_event(u8 type, u8 index, u8 value)
{

	parse_key(index);
	
	if (value) { 								// key down
		if (type == TYPESETUP) {
			g_Key.type = KEYTYPE_SETUP;
			g_Key.index = 0;
			g_Setup = 1;
		} else { 
			parse_key(index);
			g_Pressed[g_Key.hal_index] = 1;
		}

		switch (g_Key.type) {
				case KEYTYPE_MODE:
					if (g_colorpicker) {
						g_colorpicker = 0;
						g_colorpicker_seq = 0;
					}
					switch (g_Key.index) {
						case 1:
							change_mode(MODE_TRACKMUTE);
							break;
						case 2:
							change_mode(MODE_MACRO);
							break;
						case 3:
							change_mode(MODE_TRACKSELECT);
							break;
						case 4:
							break;
					}
					break;
				
				case KEYTYPE_SEQBANK:
					if (g_colorpicker) {
						g_colorpicker = 0;
						g_colorpicker_seq = 0;
					}
					g_seqBank = g_Key.index;
					update_control_lights();
					break;
				
				case KEYTYPE_SEQ:	
					if (g_Pressed[2]) {   // if the channel ket is down, that means this is a channel set event
						g_Channel = g_Key.index + 8;
						show_channel_lights();
					} else if (g_colorpicker) {   // we've got the color picker active, so it's a color selection event
						handle_color_picker_event();
					} else if (g_Setup) {         // seq color picker! idk about ordering of this and the channel one.
						g_colorpicker_index = g_Key.index - 1;
						show_seq_color_picker(seq_colors[g_Key.index - 1]);
					} else {			  // if none of those, just a plain seq send change event
						// send cc 69, value 01 - 32
						hal_send_midi(USBSTANDALONE, CC | g_Channel, 69, ((g_seqBank - 1) * 8) + g_Key.index);
					}
					break;
				case KEYTYPE_CHANNEL:
					if (g_colorpicker) {
						g_colorpicker = 0;
						g_colorpicker_seq = 0;
					}
					show_channel_lights();
					break;
				case KEYTYPE_BOTTOM:
					break;
				case KEYTYPE_LEFT:	
					if (g_Pressed[2]) {		// if the channel ket is down, that means this is a channel set event
						g_Channel = g_Key.index;
						show_channel_lights();
					} else if (g_colorpicker) {   // we've got the color picker active, so it's a color selection event
						handle_color_picker_event();
					}
					break;
				case KEYTYPE_SETUP:
					if (g_colorpicker) {	// setup always bails out of color picker if active
						g_colorpicker = 0;
						g_colorpicker_seq = 0;
						update_control_lights();
					}
					switch (g_Mode) {		// this and above should never overlap so this is ok?
						case MODE_DEFMACRO:
							change_mode(MODE_MACRO);
							break;
					}
					break;
				case KEYTYPE_PAD:
					switch (g_Mode) {
						case MODE_TRACKMUTE:
							if (g_Setup) {
								g_colorpicker_index = g_Key.index - 1;
								show_color_picker(track_mute_colors[g_Key.index - 1]);
							} else { 
								pad_light( g_Key.hal_index, 15);
								hal_send_midi(USBSTANDALONE, NOTEON | g_Channel , g_Key.index + 35, value);
								hal_send_midi(USBSTANDALONE, NOTEOFF | g_Channel ,g_Key.index + 35, value);
							}
							break;
						case MODE_TRACKSELECT:
							if (g_Setup) {
								g_colorpicker_index = g_Key.index - 1;
								show_color_picker(track_select_colors[g_Key.index - 1]);
							} else { 
								pad_light( g_Key.hal_index, 15);
								hal_send_midi(USBSTANDALONE, CC | g_Channel , 0, g_Key.index);
							}
							break;
						case MODE_MACRO:
							if (g_Setup) {
								g_colorpicker_index = g_Key.index - 1;
								g_defmacro = g_Key.index - 1;
								change_mode(MODE_DEFMACRO);
							} else {
								pad_light( g_Key.hal_index, 15);
								play_macro(g_Key.index - 1);
							}
							break;
						case MODE_DEFMACRO:
							toggle_macro_track(g_Key.index - 1);
							change_mode(MODE_DEFMACRO);
							break;


					}
				break;
		}
	/********************

		Key release

	*********************/
	} else {  
		if (type == TYPESETUP) {				
			g_Key.type = KEYTYPE_SETUP;
			g_Key.index = 0;
			g_Setup = 0;
		} else { 
			parse_key(index);
			g_Pressed[g_Key.hal_index] = 0;
		}

		switch (g_Key.type) {
				case KEYTYPE_MODE:
				case KEYTYPE_SEQBANK:
				case KEYTYPE_BOTTOM:
				case KEYTYPE_CHANNEL:
					update_control_lights();
					break;
				case KEYTYPE_SEQ:	
				case KEYTYPE_LEFT:	
					break;
				case KEYTYPE_SETUP:
					break;
				case KEYTYPE_PAD:
					switch (g_Mode) {
						case MODE_TRACKMUTE:
							pad_light(g_Key.hal_index, current_colors[g_Key.index - 1]);
							break;
						case MODE_MACRO:
							pad_light(g_Key.hal_index, current_colors[g_Key.index - 1]);
							break;
						case MODE_TRACKSELECT:
							pad_light(g_Key.hal_index, current_colors[g_Key.index - 1]);
							break;							
					}
				break;
		}
	}	
}

//______________________________________________________________________________

void app_midi_event(u8 port, u8 status, u8 d1, u8 d2)
{
}

//______________________________________________________________________________

void app_sysex_event(u8 port, u8 * data, u16 count)
{
}

//______________________________________________________________________________

void app_aftertouch_event(u8 index, u8 value)
{
}

//______________________________________________________________________________

void app_cable_event(u8 type, u8 value)
{
}

//______________________________________________________________________________

void app_timer_event()
{
}

//______________________________________________________________________________

void app_init(const u16 *adc_raw)

{	

	init_track_mute_colors();
	init_macro_colors();
	init_track_select_colors();
	
	change_mode(MODE_TRACKMUTE);
	
	// 1 4 7 12
	// g_macros[0][0] = 146;
	
	// store off the raw ADC frame pointer for later use
	// g_ADC = adc_raw;
}
