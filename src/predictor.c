//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Xiaofan Yu";
const char *studentID   = "A53276743";
const char *email       = "x1yu@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
struct gshare {
  uint32_t ghistory_reg;
  uint32_t mask;
  uint32_t index; // last access index
  uint8_t pred; // last 2-bit prediction result
  uint8_t BHT[GSHARE_BHT_SIZE_BYTE];
} gshare;

struct chooser {
  uint32_t index; // last access index
  uint32_t pred; // last 2-bit prediction result
  uint8_t BHT[CHOOSER_SIZE_BYTE];
};

struct global_pred {
  uint32_t index; // last addcess index
  uint8_t pred; // last 2-bit prediction result
  uint8_t BHT[GLOBAL_SIZE_BYTE];
};

struct local_pred {
  uint32_t pred_index; // last access index of local predictor table
  uint8_t pred; // last 2-bit prediction result
  uint16_t hisTable[LOCAL_HISTORY_SIZE_WORD];
  uint8_t preTable[LOCAL_PREDICTOR_SIZE_BYTE];
};

struct tournament {
  uint32_t ghistory_reg;
  uint32_t gmask;
  uint32_t lmask;
  uint32_t pcmask;
  struct chooser chooser;
  struct global_pred gPred;
  struct local_pred lPred;
} trn;

struct perceptron {
  uint32_t ghistory_reg;
  // uint32_t gmask; // 32 bit history
  uint32_t pcmask;
  // add one for the intercept
  int8_t pctTable[PERCEPTRON_PC_INDEX_SIZE][PERCEPTRON_BHR_BITS + 1];
  int res;
} pct;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  // Make initialization based on the bpType
  switch (bpType) {
    case STATIC:
      break;
    case GSHARE:
      gshare.mask = (1 << ghistoryBits) - 1;
      gshare.ghistory_reg = 0; // init to NOTTAKEN
      // init gshare BHT to WN
      for (int i = 0; i < GSHARE_BHT_SIZE_BYTE; ++i)
        gshare.BHT[i] = 0x55;
      break;
    case TOURNAMENT:
      trn.gmask = (1 << ghistoryBits) - 1;
      trn.lmask = (1 << lhistoryBits) - 1;
      trn.pcmask =  (1 << pcIndexBits) - 1;
      trn.ghistory_reg = 0; // init to NOTTAKEN
      // init chooser to weakly global
      for (int i = 0; i < CHOOSER_SIZE_BYTE; ++i)
        trn.chooser.BHT[i] = 0x55;
      // init global predictor to WN
      for (int i = 0; i < GLOBAL_SIZE_BYTE; ++i)
        trn.gPred.BHT[i] = 0x55;
      // init local history table to NT
      for (int i = 0; i < LOCAL_HISTORY_SIZE_WORD; ++i)
        trn.lPred.hisTable[i] = 0;
      // init local predictor table to WN
      for (int i = 0; i < LOCAL_PREDICTOR_SIZE_BYTE; ++i)
        trn.lPred.preTable[i] = 0x55;
      break;
    case CUSTOM:
      pct.pcmask = (1 << PERCEPTRON_PC_BITS) - 1;
      pct.ghistory_reg = 0;
      for (int i = 0; i < PERCEPTRON_PC_INDEX_SIZE; ++i)
      	for (int j = 0; j < PERCEPTRON_BHR_BITS + 1; ++j)
      		pct.pctTable[i][j] = 0;
    default:
      break;
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      // predict
      gshare.index = (gshare.ghistory_reg ^ pc) & gshare.mask;
      if (verbose)
        printf("pc: %x, history: %x, index: %d\r\n", pc, 
	        gshare.ghistory_reg, gshare.index);
      gshare.pred = read_BHT(gshare.BHT, gshare.index);
      return gshare.pred >> 1; // prediction is uppermost bit
    case TOURNAMENT:;
      // Use local predictor
      uint16_t hPattern = trn.lPred.hisTable[pc & trn.pcmask];
      trn.lPred.pred_index = hPattern & trn.lmask;
      if (verbose)
        printf("local pc: %x, history: %02x, index: %d\r\n", pc, 
          hPattern, trn.lPred.pred_index);
      trn.lPred.pred = read_BHT(trn.lPred.preTable, 
        trn.lPred.pred_index);

      // Use global predictor
      trn.gPred.index = trn.ghistory_reg & trn.gmask;
      if (verbose)
        printf("global pc: %x, history: %x, index: %d\r\n", pc, 
          trn.ghistory_reg, trn.gPred.index);
      trn.gPred.pred = read_BHT(trn.gPred.BHT, trn.gPred.index);

      // chooser
      trn.chooser.index = trn.ghistory_reg & trn.gmask;
      if (verbose)
      	printf("chooser pc: %x, history: %x, index: %d\r\n", pc, 
          trn.ghistory_reg, trn.chooser.index);
      trn.chooser.pred = read_BHT(trn.chooser.BHT, 
        trn.chooser.index);
      // choose predictor according to chooser
      if (trn.chooser.pred >> 1) // choose local
        return trn.lPred.pred >> 1;
      else // choose global
        return trn.gPred.pred >> 1;
    case CUSTOM:;
      pct.res = dot(pct.ghistory_reg, 
      	pct.pctTable[pc & pct.pcmask]);
      return (pct.res > INFER_THRESHOLD);
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  switch (bpType) {
    case STATIC:
      break;
    case GSHARE:
      // taken and BHT does not reach ST
      if (outcome == TAKEN && gshare.pred != ST)
        write_BHT(gshare.BHT, gshare.index, TAKEN);
      // not taken and BHT does not reach SN
      else if (outcome == NOTTAKEN && gshare.pred != SN)
        write_BHT(gshare.BHT, gshare.index, NOTTAKEN);
      // update ghistory register
      gshare.ghistory_reg = gshare.ghistory_reg << 1 | outcome;
      if (verbose)
        printf("after shift history: %x\r\n", 
          gshare.ghistory_reg);
      break;
    case TOURNAMENT:
      // train local predictor
      // update local history table
      if (verbose) printf("Train local\r\n");  
      trn.lPred.hisTable[pc & trn.pcmask] = 
        trn.lPred.hisTable[pc & trn.pcmask] << 1 | outcome;
      // update local predictor table
      if (outcome == TAKEN && trn.lPred.pred != ST)
        write_BHT(trn.lPred.preTable, trn.lPred.pred_index, 
          TAKEN);
      else if (outcome == NOTTAKEN && trn.lPred.pred != SN)
        write_BHT(trn.lPred.preTable, trn.lPred.pred_index, 
          NOTTAKEN);
           
      // update global predictor table
      if (verbose) printf("Train global\r\n");
      trn.ghistory_reg = trn.ghistory_reg << 1 | outcome;
      if (outcome == TAKEN && trn.gPred.pred != ST)
        write_BHT(trn.gPred.BHT, trn.gPred.index, 
	        TAKEN);
      else if (outcome == NOTTAKEN && trn.gPred.pred != SN)
        write_BHT(trn.gPred.BHT, trn.gPred.index, 
          NOTTAKEN);
      
      // update chooser
      if (verbose) printf("Train chooser\r\n");
      uint8_t local_correct = ((trn.lPred.pred >> 1) == outcome);
      uint8_t global_correct = ((trn.gPred.pred >> 1) == outcome);
      // if local is correct and chooser pred is not SLC
      if (local_correct > global_correct && 
        trn.chooser.pred != SLC)
        write_BHT(trn.chooser.BHT, trn.chooser.index,
          SLC >> 1);
      // else if global is correct and chooser pred is not SGB
      else if (global_correct > local_correct &&
        trn.chooser.pred != SGB)
        write_BHT(trn.chooser.BHT, trn.chooser.index,
          SGB >> 1);
      break;
    case CUSTOM:
      if (pct.pred != outcome && abs(res) < TRAIN_THRESHOLD)
        train_pct(pct.ghistory_reg, pct.pctTable[pc & pct.pcmask], 
      	  outcome);
      pct.ghistory_reg = pct.ghistory_reg << 1 | outcome;
    default:
      break;
  }
  return;
}

// Read the result of the 2-bit predictor from byte-based BHT
uint8_t read_BHT(uint8_t *BHT, uint32_t index) {
  uint32_t pos = index >> 2; // divide by 4
  uint32_t offset = index & 0x3; // get the last two bit
  offset <<= 1; // time 2, get the offset in one byte
  uint8_t pred = (uint8_t)((BHT[pos] >> offset) & 0x3);
  if (verbose)
    printf("read byte: 0x%02x, offset: %d, pred: 0x%1x\r\n", 
      BHT[pos], offset, pred);
  return pred;
}

// Update the element in BHT to a certain direction
// If dir == TAKEN, ++. If dir == NOTTAKEN, --.
//
void write_BHT(uint8_t *BHT, uint32_t index, uint8_t dir) {
  uint32_t pos = index >> 2; // divide by 4
  uint32_t offset = index & 0x3; // get the last two bit
  offset <<= 1; // time 2, get the offset in one byte
  if (verbose)
    printf("offset: %d, before write byte: 0x%02x\r\n", 
      offset, BHT[pos]);
  if (dir == TAKEN)
    BHT[pos] += 1 << offset;
  else
    BHT[pos] -= 1 << offset;
  if (verbose)
    printf("offset: %d, after write byte: 0x%02x\r\n", 
      offset, BHT[pos]);
  return;
}

// Calculate the dot product
//
int dot(uint32_t hisReg, int8_t * fp) {
  int res = 0;
  if (verbose)
    printf("dot: hisReg: 0x%x\r\n", hisReg);

  for (int i = 0; i < PERCEPTRON_BHR_BITS; ++i) {
  	uint8_t bit = hisReg & 0x1; // get LSB
  	if (verbose)
      printf("hisReg bit: %d, fp: %d\r\n", bit, fp[i]);
  	
    // CUSTOM_TAKEN = 1, CUSTOM_NOTTAKEN = -1
    if (bit)
  	  res += fp[i];
    else
      res -= fp[i];
  	hisReg >>= 1;
  }
  res += fp[PERCEPTRON_BHR_BITS]; // add intercept
  if (verbose) printf("res: %d\r\n", res);
  return res;
}

// Train perceptron
//
void train_pct(uint32_t hisReg, int8_t * fp, 
	uint8_t outcome) {
  // t: custom taken or not taken
  int t = outcome? 1: -1;
  for (int i = 0; i < PERCEPTRON_BHR_BITS; ++i) {
  	uint8_t bit = hisReg & 0x01;
  	// if (verbose)
    //  printf("hisReg bit: %d, fp: %d\r\n", bit, fp[i]);

    // x: custom taken or not taken
    int x = bit? 1: -1;
    fp[i] += t * x;
    hisReg >>= 1;

    // if (verbose)
    //  printf("After fp: %d\r\n", fp[i]);
  }
  fp[PERCEPTRON_BHR_BITS] += t;
  return;
}

int abs(int res) {
  if (res > 0)
    return res;
  else
    return -res;
}