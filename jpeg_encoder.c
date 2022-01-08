/***************************************************************************/
/*                                                                         */
/*  File: savejpg.h                                                        */
/*  Author: bkenwright@xbdev.net                                           */
/*  URL: www.xbdev.net                                                     */
/*  Date: 19-01-06                                                         */
/*                                                                         */
/***************************************************************************/
/*
	Tiny Simplified C source of a JPEG encoder.
	A BMP truecolor to JPEG encoder

	*.bmp -> *.jpg
*/
/***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "main.h"
#include "jpeg_encoder.h"

// #include <math.h> 

/***************************************************************************/

// #define PI 3.14159265358979323846 

// double my_sin( double );

static BYTE bytenew=0; // The byte that will be written in the JPG file
static SBYTE bytepos=7; //bit position in the byte we write (bytenew)
			//should be<=7 and >=0
const uint16_t mask[16]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};

// The Huffman tables we'll use:
const bitstring YDC_HT[12] = {
	{0x02, 0x0000}, {0x03, 0x0002}, {0x03, 0x0003}, {0x03, 0x0004}, {0x03, 0x0005}, {0x03, 0x0006}, 
	{0x04, 0x000e}, {0x05, 0x001e}, {0x06, 0x003e}, {0x07, 0x007e}, {0x08, 0x00fe}, {0x09, 0x01fe}
};
const bitstring CbDC_HT[12] = {
	{0x02, 0x0000}, {0x02, 0x0001}, {0x02, 0x0002}, {0x03, 0x0006}, {0x04, 0x000e}, {0x05, 0x001e}, 
	{0x06, 0x003e}, {0x07, 0x007e}, {0x08, 0x00fe}, {0x09, 0x01fe}, {0x0a, 0x03fe}, {0x0b, 0x07fe}
};

const bitstring YAC_HT[256] = {
{0x04, 0x000a}, {0x02, 0x0000}, {0x02, 0x0001}, {0x03, 0x0004}, {0x04, 0x000b}, {0x05, 0x001a}, 
{0x07, 0x0078}, {0x08, 0x00f8}, {0x0a, 0x03f6}, {0x10, 0xff82}, {0x10, 0xff83}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x04, 0x000c}, 
{0x05, 0x001b}, {0x07, 0x0079}, {0x09, 0x01f6}, {0x0b, 0x07f6}, {0x10, 0xff84}, {0x10, 0xff85}, 
{0x10, 0xff86}, {0x10, 0xff87}, {0x10, 0xff88}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x05, 0x001c}, {0x08, 0x00f9}, {0x0a, 0x03f7}, 
{0x0c, 0x0ff4}, {0x10, 0xff89}, {0x10, 0xff8a}, {0x10, 0xff8b}, {0x10, 0xff8c}, {0x10, 0xff8d}, 
{0x10, 0xff8e}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x06, 0x003a}, {0x09, 0x01f7}, {0x0c, 0x0ff5}, {0x10, 0xff8f}, {0x10, 0xff90}, 
{0x10, 0xff91}, {0x10, 0xff92}, {0x10, 0xff93}, {0x10, 0xff94}, {0x10, 0xff95}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x06, 0x003b}, 
{0x0a, 0x03f8}, {0x10, 0xff96}, {0x10, 0xff97}, {0x10, 0xff98}, {0x10, 0xff99}, {0x10, 0xff9a}, 
{0x10, 0xff9b}, {0x10, 0xff9c}, {0x10, 0xff9d}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x07, 0x007a}, {0x0b, 0x07f7}, {0x10, 0xff9e}, 
{0x10, 0xff9f}, {0x10, 0xffa0}, {0x10, 0xffa1}, {0x10, 0xffa2}, {0x10, 0xffa3}, {0x10, 0xffa4}, 
{0x10, 0xffa5}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x07, 0x007b}, {0x0c, 0x0ff6}, {0x10, 0xffa6}, {0x10, 0xffa7}, {0x10, 0xffa8}, 
{0x10, 0xffa9}, {0x10, 0xffaa}, {0x10, 0xffab}, {0x10, 0xffac}, {0x10, 0xffad}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x08, 0x00fa}, 
{0x0c, 0x0ff7}, {0x10, 0xffae}, {0x10, 0xffaf}, {0x10, 0xffb0}, {0x10, 0xffb1}, {0x10, 0xffb2}, 
{0x10, 0xffb3}, {0x10, 0xffb4}, {0x10, 0xffb5}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x09, 0x01f8}, {0x0f, 0x7fc0}, {0x10, 0xffb6}, 
{0x10, 0xffb7}, {0x10, 0xffb8}, {0x10, 0xffb9}, {0x10, 0xffba}, {0x10, 0xffbb}, {0x10, 0xffbc}, 
{0x10, 0xffbd}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x09, 0x01f9}, {0x10, 0xffbe}, {0x10, 0xffbf}, {0x10, 0xffc0}, {0x10, 0xffc1}, 
{0x10, 0xffc2}, {0x10, 0xffc3}, {0x10, 0xffc4}, {0x10, 0xffc5}, {0x10, 0xffc6}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x09, 0x01fa}, 
{0x10, 0xffc7}, {0x10, 0xffc8}, {0x10, 0xffc9}, {0x10, 0xffca}, {0x10, 0xffcb}, {0x10, 0xffcc}, 
{0x10, 0xffcd}, {0x10, 0xffce}, {0x10, 0xffcf}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x0a, 0x03f9}, {0x10, 0xffd0}, {0x10, 0xffd1}, 
{0x10, 0xffd2}, {0x10, 0xffd3}, {0x10, 0xffd4}, {0x10, 0xffd5}, {0x10, 0xffd6}, {0x10, 0xffd7}, 
{0x10, 0xffd8}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x0a, 0x03fa}, {0x10, 0xffd9}, {0x10, 0xffda}, {0x10, 0xffdb}, {0x10, 0xffdc}, 
{0x10, 0xffdd}, {0x10, 0xffde}, {0x10, 0xffdf}, {0x10, 0xffe0}, {0x10, 0xffe1}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x0b, 0x07f8}, 
{0x10, 0xffe2}, {0x10, 0xffe3}, {0x10, 0xffe4}, {0x10, 0xffe5}, {0x10, 0xffe6}, {0x10, 0xffe7}, 
{0x10, 0xffe8}, {0x10, 0xffe9}, {0x10, 0xffea}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x10, 0xffeb}, {0x10, 0xffec}, {0x10, 0xffed}, 
{0x10, 0xffee}, {0x10, 0xffef}, {0x10, 0xfff0}, {0x10, 0xfff1}, {0x10, 0xfff2}, {0x10, 0xfff3}, 
{0x10, 0xfff4}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x0b, 0x07f9}, {0x10, 0xfff5}, {0x10, 0xfff6}, {0x10, 0xfff7}, {0x10, 0xfff8}, {0x10, 0xfff9}, 
{0x10, 0xfffa}, {0x10, 0xfffb}, {0x10, 0xfffc}, {0x10, 0xfffd}, {0x10, 0xfffe}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}};

const bitstring CbAC_HT[256] = {
{0x02, 0x0000}, {0x02, 0x0001}, {0x03, 0x0004}, {0x04, 0x000a}, {0x05, 0x0018}, {0x05, 0x0019}, 
{0x06, 0x0038}, {0x07, 0x0078}, {0x09, 0x01f4}, {0x0a, 0x03f6}, {0x0c, 0x0ff4}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x04, 0x000b}, 
{0x06, 0x0039}, {0x08, 0x00f6}, {0x09, 0x01f5}, {0x0b, 0x07f6}, {0x0c, 0x0ff5}, {0x10, 0xff88}, 
{0x10, 0xff89}, {0x10, 0xff8a}, {0x10, 0xff8b}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x05, 0x001a}, {0x08, 0x00f7}, {0x0a, 0x03f7}, 
{0x0c, 0x0ff6}, {0x0f, 0x7fc2}, {0x10, 0xff8c}, {0x10, 0xff8d}, {0x10, 0xff8e}, {0x10, 0xff8f}, 
{0x10, 0xff90}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x05, 0x001b}, {0x08, 0x00f8}, {0x0a, 0x03f8}, {0x0c, 0x0ff7}, {0x10, 0xff91}, 
{0x10, 0xff92}, {0x10, 0xff93}, {0x10, 0xff94}, {0x10, 0xff95}, {0x10, 0xff96}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x06, 0x003a}, 
{0x09, 0x01f6}, {0x10, 0xff97}, {0x10, 0xff98}, {0x10, 0xff99}, {0x10, 0xff9a}, {0x10, 0xff9b}, 
{0x10, 0xff9c}, {0x10, 0xff9d}, {0x10, 0xff9e}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x06, 0x003b}, {0x0a, 0x03f9}, {0x10, 0xff9f}, 
{0x10, 0xffa0}, {0x10, 0xffa1}, {0x10, 0xffa2}, {0x10, 0xffa3}, {0x10, 0xffa4}, {0x10, 0xffa5}, 
{0x10, 0xffa6}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x07, 0x0079}, {0x0b, 0x07f7}, {0x10, 0xffa7}, {0x10, 0xffa8}, {0x10, 0xffa9}, 
{0x10, 0xffaa}, {0x10, 0xffab}, {0x10, 0xffac}, {0x10, 0xffad}, {0x10, 0xffae}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x07, 0x007a}, 
{0x0b, 0x07f8}, {0x10, 0xffaf}, {0x10, 0xffb0}, {0x10, 0xffb1}, {0x10, 0xffb2}, {0x10, 0xffb3}, 
{0x10, 0xffb4}, {0x10, 0xffb5}, {0x10, 0xffb6}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x08, 0x00f9}, {0x10, 0xffb7}, {0x10, 0xffb8}, 
{0x10, 0xffb9}, {0x10, 0xffba}, {0x10, 0xffbb}, {0x10, 0xffbc}, {0x10, 0xffbd}, {0x10, 0xffbe}, 
{0x10, 0xffbf}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x09, 0x01f7}, {0x10, 0xffc0}, {0x10, 0xffc1}, {0x10, 0xffc2}, {0x10, 0xffc3}, 
{0x10, 0xffc4}, {0x10, 0xffc5}, {0x10, 0xffc6}, {0x10, 0xffc7}, {0x10, 0xffc8}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x09, 0x01f8}, 
{0x10, 0xffc9}, {0x10, 0xffca}, {0x10, 0xffcb}, {0x10, 0xffcc}, {0x10, 0xffcd}, {0x10, 0xffce}, 
{0x10, 0xffcf}, {0x10, 0xffd0}, {0x10, 0xffd1}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x09, 0x01f9}, {0x10, 0xffd2}, {0x10, 0xffd3}, 
{0x10, 0xffd4}, {0x10, 0xffd5}, {0x10, 0xffd6}, {0x10, 0xffd7}, {0x10, 0xffd8}, {0x10, 0xffd9}, 
{0x10, 0xffda}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x09, 0x01fa}, {0x10, 0xffdb}, {0x10, 0xffdc}, {0x10, 0xffdd}, {0x10, 0xffde}, 
{0x10, 0xffdf}, {0x10, 0xffe0}, {0x10, 0xffe1}, {0x10, 0xffe2}, {0x10, 0xffe3}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x0b, 0x07f9}, 
{0x10, 0xffe4}, {0x10, 0xffe5}, {0x10, 0xffe6}, {0x10, 0xffe7}, {0x10, 0xffe8}, {0x10, 0xffe9}, 
{0x10, 0xffea}, {0x10, 0xffeb}, {0x10, 0xffec}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x0e, 0x3fe0}, {0x10, 0xffed}, {0x10, 0xffee}, 
{0x10, 0xffef}, {0x10, 0xfff0}, {0x10, 0xfff1}, {0x10, 0xfff2}, {0x10, 0xfff3}, {0x10, 0xfff4}, 
{0x10, 0xfff5}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 
{0x0a, 0x03fa}, {0x0f, 0x7fc3}, {0x10, 0xfff6}, {0x10, 0xfff7}, {0x10, 0xfff8}, {0x10, 0xfff9}, 
{0x10, 0xfffa}, {0x10, 0xfffb}, {0x10, 0xfffc}, {0x10, 0xfffd}, {0x10, 0xfffe}, {0x00, 0x00}, 
{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}};

#if 0
static BYTE *category_alloc;
static BYTE *category; //Here we'll keep the category of the numbers in range: -32767..32767
static bitstring *bitcode_alloc;
static bitstring *bitcode; // their bitcoded representation
#endif

#if 0
uint8_t 	trx_buf[2][TRX_SIZE];
uint8_t		pkt_cs;
uint8_t		pkt_cs0;
uint8_t		file_cs;
uint8_t		pkt_cnt;
uint16_t	trx_cnt;
#endif
extern uint8_t 	trx_buf[2][TRX_SIZE];
extern uint8_t		pkt_cs;
extern uint8_t		pkt_cs0;
extern uint8_t		file_cs;
extern uint8_t		pkt_cnt;
extern uint16_t	trx_cnt;

//Precalculated tables for a faster YCbCr->RGB transformation
// We use a SDWORD table because we'll scale values by 2^16 and work with integers
// static SDWORD YRtab[256],YGtab[256],YBtab[256];
// static SDWORD CbRtab[256],CbGtab[256],CbBtab[256];
// static SDWORD CrRtab[256],CrGtab[256],CrBtab[256];
float fdtbl_Y[64];
float fdtbl_Cb[64]; //the same with the fdtbl_Cr[64]

// static colorRGB *RGB_buffer; //image to be encoded
// static WORD Ximage,Yimage;// image dimensions divisible by 8

#if COLOR
static float YDU[64];
static float CbDU[64];
static float CrDU[64];
#else
static float YDU[64];
static float CbDU[64];
//static float CrDU[64];
#endif
static SWORD DU_DCT[64]; // Current DU (after DCT and quantization) which we'll zigzag
static SWORD DU[64]; //zigzag reordered DU which will be Huffman coded

//static FILE *fp_jpeg_stream;
uint8_t jpeg_buf[V_SIZE][H_SIZE];
extern uint16_t rx_cnt;
extern uint8_t rsp_flag;
extern uint8_t jpeg_pkt_cnt, jpeg_tx_pnt;
extern uint8_t tx_pkt_cnt;

extern uint8_t rsp2_flag;

extern uint16_t	tx1_ppnt;
extern uint16_t	tx1_gpnt;
extern uint16_t	tx2_ppnt;
extern uint16_t	tx2_gpnt;

void tx_to_bluetooth(char);
void encoding_8line();
void pre_encode();
void post_encode();
void init_all();
void writebyte(uint8_t);

extern void chk_rx(void);
extern void writebyte(uint8_t);
extern uint8_t rx_data_proc(void);
extern void Delay(__IO uint32_t);
extern void Set_Delay2(__IO uint32_t nTime);

/***************************************************************************/

void write_APP0info()
//Nothing to overwrite for APP0info
{
 writeword(APP0info.marker);
 writeword(APP0info.length);
 writebyte('J');writebyte('F');writebyte('I');writebyte('F');writebyte(0);
 writebyte(APP0info.versionhi);writebyte(APP0info.versionlo);
 writebyte(APP0info.xyunits);
 writeword(APP0info.xdensity);writeword(APP0info.ydensity);
 writebyte(APP0info.thumbnwidth);writebyte(APP0info.thumbnheight);
}

void write_SOF0info()
// We should overwrite width and height
{
 writeword(SOF0info.marker);
 writeword(SOF0info.length);
 writebyte(SOF0info.precision);
 writeword(SOF0info.height);writeword(SOF0info.width);
 writebyte(SOF0info.nrofcomponents);
 writebyte(SOF0info.IdY);writebyte(SOF0info.HVY);writebyte(SOF0info.QTY);
 writebyte(SOF0info.IdCb);writebyte(SOF0info.HVCb);writebyte(SOF0info.QTCb);
 writebyte(SOF0info.IdCr);writebyte(SOF0info.HVCr);writebyte(SOF0info.QTCr);
}

void write_DQTinfo()
{
 BYTE i;
 writeword(DQTinfo.marker);
 writeword(DQTinfo.length);
 writebyte(DQTinfo.QTYinfo);for (i=0;i<64;i++) writebyte(DQTinfo.Ytable[i]);
 writebyte(DQTinfo.QTCbinfo);for (i=0;i<64;i++) writebyte(DQTinfo.Cbtable[i]);
}

void set_quant_table(const BYTE *basic_table,BYTE scale_factor,BYTE *newtable)
// Set quantization table and zigzag reorder it
{
 BYTE i;
 long temp;
 for (i = 0; i < 64; i++) {
      temp = ((long) basic_table[i] * scale_factor + 50L) / 100L;
	//limit the values to the valid range
    if (temp <= 0L) temp = 1L;
    if (temp > 255L) temp = 255L; //limit to baseline range if requested
    newtable[zigzag[i]] = (BYTE) temp;
			  }
}

void set_DQTinfo()
{
// BYTE scalefactor=100;// scalefactor controls the visual quality of the image
 BYTE scalefactor=90;// scalefactor controls the visual quality of the image
			// the smaller is, the better image we'll get, and the smaller
			// compression we'll achieve
 DQTinfo.marker=0xFFDB;
 DQTinfo.length=132;
 DQTinfo.QTYinfo=0;
 DQTinfo.QTCbinfo=1;
 set_quant_table(std_luminance_qt,scalefactor,DQTinfo.Ytable);
 set_quant_table(std_chrominance_qt,scalefactor,DQTinfo.Cbtable);
}

void write_DHTinfo()
{
 BYTE i;
 writeword(DHTinfo.marker);
 writeword(DHTinfo.length);
 writebyte(DHTinfo.HTYDCinfo);
 for (i=0;i<16;i++)  writebyte(DHTinfo.YDC_nrcodes[i]);
 for (i=0;i<=11;i++) writebyte(DHTinfo.YDC_values[i]);
 writebyte(DHTinfo.HTYACinfo);
 for (i=0;i<16;i++)  writebyte(DHTinfo.YAC_nrcodes[i]);
 for (i=0;i<=161;i++) writebyte(DHTinfo.YAC_values[i]);
 writebyte(DHTinfo.HTCbDCinfo);
 for (i=0;i<16;i++)  writebyte(DHTinfo.CbDC_nrcodes[i]);
 for (i=0;i<=11;i++)  writebyte(DHTinfo.CbDC_values[i]);
 writebyte(DHTinfo.HTCbACinfo);
 for (i=0;i<16;i++)  writebyte(DHTinfo.CbAC_nrcodes[i]);
 for (i=0;i<=161;i++) writebyte(DHTinfo.CbAC_values[i]);
}

void set_DHTinfo()
{
 BYTE i;
 DHTinfo.marker=0xFFC4;
 DHTinfo.length=0x01A2;
 DHTinfo.HTYDCinfo=0;
 for (i=0;i<16;i++)  DHTinfo.YDC_nrcodes[i]=std_dc_luminance_nrcodes[i+1];
 for (i=0;i<=11;i++)  DHTinfo.YDC_values[i]=std_dc_luminance_values[i];
 DHTinfo.HTYACinfo=0x10;
 for (i=0;i<16;i++)  DHTinfo.YAC_nrcodes[i]=std_ac_luminance_nrcodes[i+1];
 for (i=0;i<=161;i++) DHTinfo.YAC_values[i]=std_ac_luminance_values[i];
 DHTinfo.HTCbDCinfo=1;
 for (i=0;i<16;i++)  DHTinfo.CbDC_nrcodes[i]=std_dc_chrominance_nrcodes[i+1];
 for (i=0;i<=11;i++)  DHTinfo.CbDC_values[i]=std_dc_chrominance_values[i];
 DHTinfo.HTCbACinfo=0x11;
 for (i=0;i<16;i++)  DHTinfo.CbAC_nrcodes[i]=std_ac_chrominance_nrcodes[i+1];
 for (i=0;i<=161;i++) DHTinfo.CbAC_values[i]=std_ac_chrominance_values[i];
}

void write_SOSinfo()
//Nothing to overwrite for SOSinfo
{
 writeword(SOSinfo.marker);
 writeword(SOSinfo.length);
 writebyte(SOSinfo.nrofcomponents);
 writebyte(SOSinfo.IdY);writebyte(SOSinfo.HTY);
 writebyte(SOSinfo.IdCb);writebyte(SOSinfo.HTCb);
 writebyte(SOSinfo.IdCr);writebyte(SOSinfo.HTCr);
 writebyte(SOSinfo.Ss);writebyte(SOSinfo.Se);writebyte(SOSinfo.Bf);
}

void write_comment(BYTE *comment)
{
 WORD i,length;
 writeword(0xFFFE); //The COM marker
 length=(WORD)strlen((const char *)comment);
 writeword(length+2);
 for (i=0;i<length;i++) writebyte(comment[i]);
}

void writebits(bitstring bs)
// A portable version; it should be done in assembler
{
 WORD value;
 SBYTE posval;//bit position in the bitstring we read, should be<=15 and >=0
 value=bs.value;
 posval=bs.length-1;
 while (posval>=0)
  {
   if (value & mask[posval]) bytenew|=mask[bytepos];
   posval--;bytepos--;
   if (bytepos<0) { if (bytenew==0xFF) {writebyte(0xFF);writebyte(0);}
		     else {writebyte(bytenew);}
		    bytepos=7;bytenew=0;
		  }
  }
}

void compute_Huffman_table(const BYTE *nrcodes,const BYTE *std_table,bitstring *HT)
{
 BYTE k,j;
 BYTE pos_in_table;
 WORD codevalue;
 codevalue=0; pos_in_table=0;
 for (k=1;k<=16;k++)
   {
     for (j=1;j<=nrcodes[k];j++) {HT[std_table[pos_in_table]].value=codevalue;
				  HT[std_table[pos_in_table]].length=k;
				  pos_in_table++;
				  codevalue++;
				 }
     codevalue*=2;
   }
}

void exitmessage(char *error_message)
{
 printf("%s\n",error_message);exit(EXIT_FAILURE);
}

// Using a bit modified form of the FDCT routine from IJG's C source:
// Forward DCT routine idea taken from Independent JPEG Group's C source for
// JPEG encoders/decoders

// For float AA&N IDCT method, divisors are equal to quantization
//   coefficients scaled by scalefactor[row]*scalefactor[col], where
//   scalefactor[0] = 1
//   scalefactor[k] = cos(k*PI/16) * sqrt(2)    for k=1..7
//   We apply a further scale factor of 8.
//   What's actually stored is 1/divisor so that the inner loop can
//   use a multiplication rather than a division.
void prepare_quant_tables()
{
 double aanscalefactor[8] = {1.0, 1.387039845, 1.306562965, 1.175875602,
			   1.0, 0.785694958, 0.541196100, 0.275899379};
 BYTE row, col;
 BYTE i = 0;
 for (row = 0; row < 8; row++)
 {
   for (col = 0; col < 8; col++)
     {
       fdtbl_Y[i] = (float) (1.0 / ((double) DQTinfo.Ytable[zigzag[i]] *
			  aanscalefactor[row] * aanscalefactor[col] * 8.0));
       fdtbl_Cb[i] = (float) (1.0 / ((double) DQTinfo.Cbtable[zigzag[i]] *
			  aanscalefactor[row] * aanscalefactor[col] * 8.0));

	   i++;
     }
 }
}

void fdct_and_quantization(float *data,float *fdtbl,SWORD *outdata)
{
  float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  float tmp10, tmp11, tmp12, tmp13;
  float z1, z2, z3, z4, z5, z11, z13;
  float *dataptr;
  float datafloat[64];
  float temp;
  SBYTE ctr;
  BYTE i;
  for (i=0;i<64;i++) datafloat[i]=data[i];		// 64 * 5 = 320 CLK

  // Pass 1: process rows.
  dataptr=datafloat;
  for (ctr = 7; ctr >= 0; ctr--) {				// 8 * (31*3+5) = 800 CLK
	tmp0 = dataptr[0] + dataptr[7];
    tmp7 = dataptr[0] - dataptr[7];
    tmp1 = dataptr[1] + dataptr[6];
    tmp6 = dataptr[1] - dataptr[6];
    tmp2 = dataptr[2] + dataptr[5];
    tmp5 = dataptr[2] - dataptr[5];
    tmp3 = dataptr[3] + dataptr[4];
    tmp4 = dataptr[3] - dataptr[4];

	// Even part

    tmp10 = tmp0 + tmp3;	// phase 2
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    dataptr[0] = tmp10 + tmp11; // phase 3
    dataptr[4] = tmp10 - tmp11;

    z1 = (tmp12 + tmp13) * ((float) 0.707106781); // c4
	dataptr[2] = tmp13 + z1;	// phase 5
    dataptr[6] = tmp13 - z1;

    // Odd part

    tmp10 = tmp4 + tmp5;	// phase 2
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

	// The rotator is modified from fig 4-8 to avoid extra negations
    z5 = (tmp10 - tmp12) * ((float) 0.382683433); // c6
    z2 = ((float) 0.541196100) * tmp10 + z5; // c2-c6
    z4 = ((float) 1.306562965) * tmp12 + z5; // c2+c6
    z3 = tmp11 * ((float) 0.707106781); // c4

    z11 = tmp7 + z3;		// phase 5
    z13 = tmp7 - z3;

    dataptr[5] = z13 + z2;	// phase 6
    dataptr[3] = z13 - z2;
	dataptr[1] = z11 + z4;
    dataptr[7] = z11 - z4;

    dataptr += 8;		//advance pointer to next row
  }

  // Pass 2: process columns

  dataptr = datafloat;
  for (ctr = 7; ctr >= 0; ctr--) {	// 8 * (31*3+5) = 800 CLK
    tmp0 = dataptr[0] + dataptr[56];
    tmp7 = dataptr[0] - dataptr[56];
    tmp1 = dataptr[8] + dataptr[48];
    tmp6 = dataptr[8] - dataptr[48];
    tmp2 = dataptr[16] + dataptr[40];
    tmp5 = dataptr[16] - dataptr[40];
    tmp3 = dataptr[24] + dataptr[32];
    tmp4 = dataptr[24] - dataptr[32];

    //Even part/

    tmp10 = tmp0 + tmp3;	//phase 2
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    dataptr[0] = tmp10 + tmp11; // phase 3
    dataptr[32] = tmp10 - tmp11;

	z1 = (tmp12 + tmp13) * ((float) 0.707106781); // c4
    dataptr[16] = tmp13 + z1; // phase 5
    dataptr[48] = tmp13 - z1;

    // Odd part

    tmp10 = tmp4 + tmp5;	// phase 2
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

    // The rotator is modified from fig 4-8 to avoid extra negations.
	z5 = (tmp10 - tmp12) * ((float) 0.382683433); // c6
    z2 = ((float) 0.541196100) * tmp10 + z5; // c2-c6
    z4 = ((float) 1.306562965) * tmp12 + z5; // c2+c6
    z3 = tmp11 * ((float) 0.707106781); // c4

    z11 = tmp7 + z3;		// phase 5
    z13 = tmp7 - z3;
    dataptr[40] = z13 + z2; // phase 6
	dataptr[24] = z13 - z2;
    dataptr[8] = z11 + z4;
    dataptr[56] = z11 - z4;

    dataptr++;			// advance pointer to next column
  }

// Quantize/descale the coefficients, and store into output array
 for (i = 0; i < 64; i++) {						// 64 * (5+2*3) = 704 CLK
 // Apply the quantization and scaling factor
			   temp = datafloat[i] * fdtbl[i];

   //Round to nearest integer.
   //Since C does not specify the direction of rounding for negative
   //quotients, we have to force the dividend positive for portability.
   //The maximum coefficient size is +-16K (for 12-bit data), so this
   //code should work for either 16-bit or 32-bit ints.

	   outdata[i] = (SWORD) ((SWORD)(temp + 16384.5) - 16384);
   }
}

void process_DU(float *ComponentDU,float *fdtbl,SWORD *DC,
		const bitstring *HTDC, const bitstring *HTAC)
//__ramfunc void process_DU(uint8_t *ComponentDU,float *fdtbl,SWORD *DC,
//			const bitstring *HTDC, const bitstring *HTAC)
{
 bitstring EOB=HTAC[0x00];
 bitstring M16zeroes=HTAC[0xF0];
 BYTE i;
 BYTE startpos;
 BYTE end0pos;
 BYTE nrzeroes;
 BYTE nrmarker;
 SWORD Diff;

 BYTE categor;
 bitstring bit_cod;

 fdct_and_quantization(ComponentDU,fdtbl,DU_DCT);		// 2624 CLK,  FDCT + 양자화 
 //zigzag reorder
 for (i=0;i<=63;i++) DU[zigzag[i]]=DU_DCT[i];			// Zigzag Scanning
 Diff=DU[0]-*DC;
 *DC=DU[0];
 
 // lji 2014/06/27 category 및 bitcode 구하는루틴 수정
 extract_parameter(Diff, &categor, &bit_cod.length, &bit_cod.value);	// 허프만 coding
 
 //Encode DC
 if (Diff==0) writebits(HTDC[0]); //Diff might be 0
// lji 2014/06/27 DC게수 구하는루틴 수정
//  else {writebits(HTDC[category[Diff]]);
//	writebits(bitcode[Diff]);
  else {
  	writebits(HTDC[categor]);
	writebits(bit_cod);
  }
  
 //Encode ACs
 for (end0pos=63;(end0pos>0)&&(DU[end0pos]==0);end0pos--) ;
 //end0pos = first element in reverse order !=0
 if (end0pos==0) {writebits(EOB);return;}

 i=1;
 while (i<=end0pos)
  {
   startpos=i;
   for (; (DU[i]==0)&&(i<=end0pos);i++) ;
   nrzeroes=i-startpos;
   if (nrzeroes>=16) {
      for (nrmarker=1;nrmarker<=nrzeroes/16;nrmarker++) writebits(M16zeroes);
      nrzeroes=nrzeroes%16;
		     }
   // lji 2014/06/27 category 및 bitcode 구하는루틴 수정
   extract_parameter(DU[i], &categor, &bit_cod.length, &bit_cod.value);
//   writebits(HTAC[nrzeroes*16+category[DU[i]]]);writebits(bitcode[DU[i]]);
   writebits(HTAC[nrzeroes*16+categor]);	writebits(bit_cod);
   i++;
  }
 if (end0pos!=63) writebits(EOB);
}

void pre_encode()
{

	WORD Ximage_original = 640;
    WORD Yimage_original = V_SIZE;	//the original image dimensions,
											// before we made them divisible by 8

//	bitstring fillbits; //filling bitstring for the bit alignment of the EOI marker

	init_all();
	SOF0info.width=Ximage_original;
	SOF0info.height=Yimage_original;
	writeword(0xFFD8); //SOI
	write_APP0info();	// "IFIF"+..

	write_DQTinfo();
	write_SOF0info();
	write_DHTinfo();
	write_SOSinfo();
}

#pragma optimize=s low no_code_motion
void post_encode()
{ int i;
	bitstring fillbits; //filling bitstring for the bit alignment of the EOI marker

	//Do the bit alignment of the EOI marker
	if (bytepos>=0) 
	{
		fillbits.length=bytepos+1;
		fillbits.value=(1<<(bytepos+1))-1;
		writebits(fillbits);
	}
//	writeword(0xFFD9); //EOI
	writebyte(0xFF); //EOI
#if 1
	trx_buf[jpeg_pkt_cnt][A_STX] = STX;
	trx_buf[jpeg_pkt_cnt][A_CMD] = 'A';

	trx_buf[jpeg_pkt_cnt][TRX_HDR+trx_cnt] = 0xD9;
	pkt_cs += 0xD9;
	file_cs += 0xD9;
	trx_cnt++;

	trx_buf[jpeg_pkt_cnt][A_P_NO] = pkt_cnt | 0x80;
//	마지막 패킷의 나머지는 0으로 채움
	for(i=trx_cnt;i<P_SIZE;i++)
		trx_buf[jpeg_pkt_cnt][TRX_HDR+i] = 0x00;
		
//		FILE CS
	trx_buf[jpeg_pkt_cnt][A_F_CS] = ~file_cs+1;
				
//		PACKET CS
	pkt_cs += trx_buf[jpeg_pkt_cnt][1]+trx_buf[jpeg_pkt_cnt][2]+trx_buf[jpeg_pkt_cnt][A_F_CS];		// CMD+P_NO+file_cs
	trx_buf[jpeg_pkt_cnt][A_P_CS] = ~pkt_cs+1;
	
	if(jpeg_pkt_cnt<(JPEG_PKT-1))
		jpeg_pkt_cnt++;
	tx_pkt_cnt++;

	trx_cnt = pkt_cnt = file_cs = pkt_cs = 0;
#endif
}

void encoding_8line()
{ int j, k, xpos, ypos;
 SWORD DCY=0,DCCb=0,DCCr=0; //DC coefficients used for differential encoding

  bytenew=0; bytepos=7;

  for (ypos=0;ypos<V_SIZE;ypos+=8) {
#if COLOR
	  for (xpos=0;xpos<H_SIZE;xpos+=16) {
		for(j=0;j<8;j++) {
		  for(k=0;k<4;k++) {
			YDU[j*8+2*k] = (float)(jpeg_buf[ypos+j][xpos+k*4]-128);
			CbDU[j*8+2*k] = CbDU[j*8+2*k+1] = (float)(jpeg_buf[ypos+j][xpos+k*4+1]-128);
			YDU[j*8+2*k+1] = (float)(jpeg_buf[ypos+j][xpos+k*4+2]-128);
			CrDU[j*8+2*k] = CrDU[j*8+2*k+1] = (float)(jpeg_buf[ypos+j][xpos+k*4+3]-128);
		  }
		}
#else
	for (xpos=0;xpos<H_SIZE;xpos+=8) {
		for(j=0;j<8;j++) {
		  for(k=0;k<8;k++) {
			YDU[j*8+k] = (float)jpeg_buf[ypos+j][xpos+k]-128;
			CbDU[j*8+k] = 0;
		  }
		}
#endif
	    process_DU(YDU,fdtbl_Y,&DCY,YDC_HT,YAC_HT);
	    process_DU(CbDU,fdtbl_Cb,&DCCb,CbDC_HT,CbAC_HT);
#if COLOR
	    process_DU(CrDU,fdtbl_Cb,&DCCr,CbDC_HT,CbAC_HT);
#else
	    process_DU(CbDU,fdtbl_Cb,&DCCr,CbDC_HT,CbAC_HT);
#endif
	   }
   }
}

void init_all()
{
 set_DQTinfo();
 set_DHTinfo();
// init_Huffman_tables();
// set_numbers_category_and_bitcode(); // lji 2014/06/27 table 처리대신 직접 게산처리 
// precalculate_YCbCr_tables();				// lji 2014/06/30 사용안함(YUV 방식으로 처리)
 prepare_quant_tables();
}

// lji 2014/06/27 
// category 및 bitcode의 값을 게산식에 의하여 얻어냄
// original source의 nr에서 nrlower, nrupper, cat를 구하여 category와 bitcode 값을 추출한다
void extract_parameter(SWORD nr, BYTE *catego, BYTE *bit_code_length, WORD *bit_code_value)
{ char i, cat;
  SWORD nrlower, nrupper;
  WORD unsign_nr;

	if(nr<0)	unsign_nr = ~nr + 1;
	else		unsign_nr = nr;
	
	for(i=15; i>=1; i--) {
		if((mask[i-1] & unsign_nr)) break;
	}
	cat =i;
	nrlower = mask[i-1];	nrupper = nrlower << 1;
	*catego = cat;
	*bit_code_length = cat;
	if(nr > 0) 	*bit_code_value = nr;
	else		*bit_code_value = nrupper -1+nr;
}

