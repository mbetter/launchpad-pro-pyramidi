#ifndef PYR_H
#define PYR_H

#define MODE_TRACKMUTE			(0)
#define MODE_MACRO				(1)
#define MODE_TRACKSELECT		(2)
#define MODE_CHANNEL			(3)
#define MODE_DEFMACRO			(4)


#define KEYTYPE_MODE 			(0)
#define KEYTYPE_SEQBANK			(1)
#define KEYTYPE_SEQ				(2)
#define KEYTYPE_BOTTOM			(3)
#define KEYTYPE_LEFT			(4)
#define KEYTYPE_PAD				(5)
#define KEYTYPE_SETUP			(6)
#define KEYTYPE_CHANNEL			(7)

#define COLOR_SETMACRO 			(5)
#define COLOR_UNSETMACRO		(6)
#define COLOR_DEFMACROBG1       (7)
#define COLOR_DEFMACROBG2       (8)
#define COLOR_DEFMACROFG		(9)

typedef struct
{
	u8 type;
	u8 hal_index;
	u8 index;
} KeyInfo;

#endif
