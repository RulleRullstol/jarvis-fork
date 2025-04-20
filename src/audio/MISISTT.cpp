
#include "audio.h"
#include "../globals.h"
#include <cmath>
 
short misistt(vector<deque<int16_t>> &buffers) {
  short returnId;
  double currentHighest = 0.0;

  for (int i = 0; i < globalCH.getESPCount(); i++) {
    double sumSquares = 0.0;
    for (int j = 0; j < 24; j++) {
      sumSquares += static_cast<double>(buffers[i][j] * buffers[i][j]);
       double comp = sqrt(sumSquares / 24);
      if (comp > currentHighest) {
        currentHighest = comp;
        returnId = i;
      }
    }
  }

  return returnId;
}
