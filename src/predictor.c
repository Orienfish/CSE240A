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
#define MAX_BYTE 13 * (1 << 10) // 13kB
struct gshare {
  int ghistory_reg;
  int mask;
  int index; // last access index
  char pred; // last prediction result
  char gshare_BHT[MAX_BYTE];
} gshare;



//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  // Make initialization based on the bpType
  switch (bpType) {
    case STATIC:
      break;
    case GSHARE:
      gshare.mask = (1 << ghistoryBits) - 1;
      gshare.ghistory_reg = 0; // init to NOTTAKEN
      // init gshare BHT to WN
      for (int i = 0; i < MAX_BYTE; ++i)
        gshare.gshare_BHT[i] = 0x55;
      break;
    case TOURNAMENT:
    case CUSTOM:
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
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      // predict
      gshare.index = (gshare.ghistory_reg ^ pc) & gshare.mask;
      gshare.pred = read_BHT(gshare.gshare_BHT, gshare.index);
      // update ghistory register
      gshare.ghistory_reg = gshare.ghistory_reg << 1 | gshare.pred;
      return gshare.pred >= WT;
    case TOURNAMENT:
    case CUSTOM:
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
  //
  //TODO: Implement Predictor training
  //
  switch (bpType) {
    case STATIC:
      break;
    case GSHARE:
      // taken and BHT does not reach ST
      if (outcome == TAKEN && gshare.pred != ST)
        write_BHT(gshare.gshare_BHT, gshare.index, TAKEN);
      // not taken and BHT does not reach SN
      else if (outcome == NOTTAKEN && gshare.pred != SN)
        write_BHT(gshare.gshare_BHT, gshare.index, NOTTAKEN);
      break;
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }

}

// Read the result of the 2-bit predictor from byte-based BHT
//
char read_BHT(char *BHT, int index) {
  int pos = index >> 2; // divide by 4
  int offset = index & 0x3; // get the last two bit
  offset <<= 1; // time 2, get the offset in one byte
  int pred = (int)((BHT[pos] >> offset) & 0x3);
  printf("read byte: %2x, pred: %1x\r\n", BHT[pos], pred);
  return pred;
}

// Update the element in BHT to a certain direction
// If dir == TAKEN, ++. If dir == NOTTAKEN, --.
//
void write_BHT(char *BHT, int index, int dir) {
  int pos = index >> 2; // divide by 4
  int offset = index & 0x3; // get the last two bit
  offset <<= 1; // time 2, get the offset in one byte
  printf("before write byte: %2x\r\n", BHT[pos]);
  if (dir == TAKEN)
    BHT[pos] += 1 << offset;
  else
    BHT[pos] -= 1 << offset;
  printf("after write byte: %2x\r\n", BHT[pos]);
  return;
}
