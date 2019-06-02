//========================================================//
//  predictor.h                                           //
//  Header file for the Branch Predictor                  //
//                                                        //
//  Includes function prototypes and global predictor     //
//  variables and defines                                 //
//========================================================//

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <stdint.h>
#include <stdlib.h>

//
// Student Information
//
extern const char *studentName;
extern const char *studentID;
extern const char *email;

//------------------------------------//
//      Global Predictor Defines      //
//------------------------------------//
#define NOTTAKEN  0
#define TAKEN     1
// #define CUSTOM_NOTTAKEN -1
// #define CUSTOM_TAKEN 1

// The Different Predictor Types
#define STATIC      0
#define GSHARE      1
#define TOURNAMENT  2
#define CUSTOM      3
extern const char *bpName[];

// Definitions for 2-bit counters
#define SN  0			// predict NT, strong not taken
#define WN  1			// predict NT, weak not taken
#define WT  2			// predict T, weak taken
#define ST  3			// predict T, strong taken

// Definitions for the max size of Branch History Tables
#define GSHARE_BHT_SIZE_BYTE (1 << 11) // 2^11kB->2^14kbit->2bit*2^13 entries
#define CHOOSER_SIZE_BYTE (1 << 7) // 128Byte->1kbit->2bit*512entries
#define GLOBAL_SIZE_BYTE (1 << 7) // 128Byte->1kbit->2bit*512entries
#define LOCAL_HISTORY_SIZE_WORD (1 << 10) // 2^11kB->2^14kbit->
										  // ($lhistoryBits)bit*2^10entries
#define LOCAL_PREDICTOR_SIZE_BYTE (1 << 8) // 256Byte->2kbit->2bit*2^10entries
#define PERCEPTRON_PC_BITS 7
#define PERCEPTRON_PC_INDEX_SIZE (1 << 7)
#define PERCEPTRON_BHR_BITS 35 // 15 BHT and 1 for intercept
#define INFER_THRESHOLD 0
#define TRAIN_THRESHOLD 20
#define MAX_FP 127 // max for one signed byte
#define MIN_FP -127 // min for one signed byte
#define CUSTOM_GSHARE_SIZE_BYTE (1 << 8) // 2^8B->2^11bit->2bit*2^10entries
#define CUSTOM_GSHARE_BITS 10
#define CUSTOM_CHOOSER_SIZE_BYTE (1 << 8) // 2^8B->2^11bit->2bit*w^10entries
#define CUSTOM_CHOOSER_BITS 10
// Definitions for 2-bit chooser
#define SGB  0			// predict GLOBAL, strong global
#define WGB  1			// predict GLOBAL, weak global
#define WLC  2			// predict LOCAL, weak local
#define SLC  3			// predict LOCAL, strong local
// Definitions for 2-bit chooser for custom predictor
#define SGS  0			// predict GSHARE, strong gshare
#define WGS  1			// predict GSHARE, weak gshare
#define WPC  2			// predict PERCEPTRON, weak perceptron
#define SPC  3			// predict PERCEPTRON, strong perceptron
//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//
extern int ghistoryBits; // Number of bits used for Global History
extern int lhistoryBits; // Number of bits used for Local History
extern int pcIndexBits;  // Number of bits used for PC index
extern int bpType;       // Branch Prediction Type
extern int verbose;

//------------------------------------//
//    Predictor Function Prototypes   //
//------------------------------------//

// Initialize the predictor
//
void init_predictor();

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc);

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome);

uint8_t read_BHT(uint8_t *BHT, uint32_t index);
void write_BHT(uint8_t *BHT, uint32_t index, uint8_t dir);
int dot(uint32_t hisReg, int8_t * fp);
void train_pct(uint32_t hisReg, int8_t * fp, 
	uint8_t outcome);
int abs(int res);
#endif
