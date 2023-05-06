#include <cstring>
#include <stdio.h>
#include <assert.h>

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

#define ASIZE 1024
#define XSIZE 1024

void preBmBc(unsigned char *x, int m, int bmBc[]) {
   int i;
 
   for (i = 0; i < ASIZE; ++i)
      bmBc[i] = m;
   for (i = 0; i < m - 1; ++i)
      bmBc[x[i]] = m - i - 1;
}
 
 
void suffixes(unsigned char *x, int m, int *suff) {
   int f, g, i;
 
   suff[m - 1] = m;
   g = m - 1;
   for (i = m - 2; i >= 0; --i) {
      if (i > g && suff[i + m - 1 - f] < i - g)
         suff[i] = suff[i + m - 1 - f];
      else {
         if (i < g)
            g = i;
         f = i;
         while (g >= 0 && x[g] == x[g + m - 1 - f])
            --g;
         suff[i] = f - g;
      }
   }
}
 
void preBmGs(unsigned char *x, int m, int bmGs[]) {
   int i, j, suff[XSIZE];
 
   suffixes(x, m, suff);
 
   for (i = 0; i < m; ++i)
      bmGs[i] = m;
   j = 0;
   for (i = m - 1; i >= 0; --i)
      if (suff[i] == i + 1)
         for (; j < m - 1 - i; ++j)
            if (bmGs[j] == m)
               bmGs[j] = m - 1 - i;
   for (i = 0; i <= m - 2; ++i)
      bmGs[m - 1 - suff[i]] = m - 1 - i;
}
 
 
int BM(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, bmGs[XSIZE], bmBc[ASIZE];
 
   /* Preprocessing */
   preBmGs(x, m, bmGs);
   preBmBc(x, m, bmBc);
 
   /* Searching */
   j = 0;
   while (j <= n - m) {
      for (i = m - 1; i >= 0 && x[i] == y[i + j]; --i);
      if (i < 0) {
         return (j);
         j += bmGs[0];
      }
      else
         j += max(bmGs[i], bmBc[y[i + j]] - m + 1 + i);
   }
   return m + 1;
}

int TBM(unsigned char *x, int m, unsigned char *y, int n) {
   int bcShift, i, j, shift, u, v, turboShift,
       bmGs[XSIZE], bmBc[ASIZE];

   /* Preprocessing */
   preBmGs(x, m, bmGs);
   preBmBc(x, m, bmBc);

   /* Searching */
   j = u = 0;
   shift = m;
   while (j <= n - m) {
      i = m - 1;
      while (i >= 0 && x[i] == y[i + j]) {
         --i;
         if (u != 0 && i == m - 1 - shift)
            i -= u;
      }
      if (i < 0) {
         return(j);
         shift = bmGs[0];
         u = m - shift;
      }
      else {
         v = m - 1 - i;
         turboShift = u - v;
         bcShift = bmBc[y[i + j]] - m + 1 + i;
         shift = max(turboShift, bcShift);
         shift = max(shift, bmGs[i]);
         if (shift == bmGs[i])
            u = min(m - shift, v);
         else {
           if (turboShift < bcShift)
              shift = max(shift, u + 1);
           u = 0;
         }
      }
      j += shift;
   }
   return m + 1;
}

int main() {

  int needle_length = 17;
  int haystack_length = 256;

  unsigned char needle[1024];
  unsigned char haystack[1024];
  
  needle[0] = 255;
  needle[1] = 255;
  needle[2] = 16;
  needle[3] = 16;
  needle[4] = 16;
  needle[5] = 16;
  needle[6] = 16;
  needle[7] = 16;
  needle[8] = 16;
  needle[9] = 64;
  needle[10] = 16;
  needle[11] = 16;
  needle[12] = 255;
  needle[13] = 255;
  needle[14] = 16;
  needle[15] = 16;
  needle[16] = 16;
  needle[17] = 0;
  haystack[0] = 1;
  haystack[1] = 1;
  haystack[2] = 1;
  haystack[3] = 1;
  haystack[4] = 1;
  haystack[5] = 1;
  haystack[6] = 1;
  haystack[7] = 1;
  haystack[8] = 1;
  haystack[9] = 1;
  haystack[10] = 1;
  haystack[11] = 1;
  haystack[12] = 255;
  haystack[13] = 255;
  haystack[14] = 16;
  haystack[15] = 16;
  haystack[16] = 16;
  haystack[17] = 255;
  haystack[18] = 255;
  haystack[19] = 16;
  haystack[20] = 16;
  haystack[21] = 16;
  haystack[22] = 16;
  haystack[23] = 16;
  haystack[24] = 16;
  haystack[25] = 16;
  haystack[26] = 64;
  haystack[27] = 16;
  haystack[28] = 16;
  haystack[29] = 255;
  haystack[30] = 255;
  haystack[31] = 16;
  haystack[32] = 16;
  haystack[33] = 16;
  haystack[34] = 1;
  haystack[35] = 1;
  haystack[36] = 1;
  haystack[37] = 1;
  haystack[38] = 1;
  haystack[39] = 1;
  haystack[40] = 1;
  haystack[41] = 1;
  haystack[42] = 1;
  haystack[43] = 1;
  haystack[44] = 1;
  haystack[45] = 1;
  haystack[46] = 1;
  haystack[47] = 1;
  haystack[48] = 1;
  haystack[49] = 1;
  haystack[50] = 1;
  haystack[51] = 1;
  haystack[52] = 1;
  haystack[53] = 1;
  haystack[54] = 1;
  haystack[55] = 1;
  haystack[56] = 1;
  haystack[57] = 1;
  haystack[58] = 1;
  haystack[59] = 1;
  haystack[60] = 1;
  haystack[61] = 1;
  haystack[62] = 1;
  haystack[63] = 1;
  haystack[64] = 1;
  haystack[65] = 1;
  haystack[66] = 1;
  haystack[67] = 1;
  haystack[68] = 1;
  haystack[69] = 1;
  haystack[70] = 1;
  haystack[71] = 1;
  haystack[72] = 1;
  haystack[73] = 1;
  haystack[74] = 1;
  haystack[75] = 1;
  haystack[76] = 1;
  haystack[77] = 1;
  haystack[78] = 1;
  haystack[79] = 1;
  haystack[80] = 1;
  haystack[81] = 1;
  haystack[82] = 1;
  haystack[83] = 1;
  haystack[84] = 1;
  haystack[85] = 1;
  haystack[86] = 1;
  haystack[87] = 1;
  haystack[88] = 1;
  haystack[89] = 1;
  haystack[90] = 1;
  haystack[91] = 1;
  haystack[92] = 1;
  haystack[93] = 1;
  haystack[94] = 1;
  haystack[95] = 1;
  haystack[96] = 1;
  haystack[97] = 1;
  haystack[98] = 1;
  haystack[99] = 1;
  haystack[100] = 1;
  haystack[101] = 1;
  haystack[102] = 1;
  haystack[103] = 1;
  haystack[104] = 1;
  haystack[105] = 1;
  haystack[106] = 1;
  haystack[107] = 1;
  haystack[108] = 1;
  haystack[109] = 1;
  haystack[110] = 1;
  haystack[111] = 1;
  haystack[112] = 1;
  haystack[113] = 1;
  haystack[114] = 1;
  haystack[115] = 1;
  haystack[116] = 1;
  haystack[117] = 1;
  haystack[118] = 1;
  haystack[119] = 1;
  haystack[120] = 1;
  haystack[121] = 1;
  haystack[122] = 1;
  haystack[123] = 1;
  haystack[124] = 1;
  haystack[125] = 1;
  haystack[126] = 1;
  haystack[127] = 1;
  haystack[128] = 1;
  haystack[129] = 1;
  haystack[130] = 1;
  haystack[131] = 1;
  haystack[132] = 1;
  haystack[133] = 1;
  haystack[134] = 1;
  haystack[135] = 1;
  haystack[136] = 1;
  haystack[137] = 1;
  haystack[138] = 1;
  haystack[139] = 1;
  haystack[140] = 1;
  haystack[141] = 1;
  haystack[142] = 1;
  haystack[143] = 1;
  haystack[144] = 1;
  haystack[145] = 1;
  haystack[146] = 1;
  haystack[147] = 1;
  haystack[148] = 1;
  haystack[149] = 1;
  haystack[150] = 1;
  haystack[151] = 1;
  haystack[152] = 1;
  haystack[153] = 1;
  haystack[154] = 1;
  haystack[155] = 1;
  haystack[156] = 1;
  haystack[157] = 1;
  haystack[158] = 1;
  haystack[159] = 1;
  haystack[160] = 1;
  haystack[161] = 1;
  haystack[162] = 1;
  haystack[163] = 1;
  haystack[164] = 1;
  haystack[165] = 1;
  haystack[166] = 1;
  haystack[167] = 1;
  haystack[168] = 1;
  haystack[169] = 1;
  haystack[170] = 1;
  haystack[171] = 1;
  haystack[172] = 1;
  haystack[173] = 1;
  haystack[174] = 1;
  haystack[175] = 1;
  haystack[176] = 1;
  haystack[177] = 1;
  haystack[178] = 1;
  haystack[179] = 1;
  haystack[180] = 1;
  haystack[181] = 1;
  haystack[182] = 1;
  haystack[183] = 1;
  haystack[184] = 1;
  haystack[185] = 1;
  haystack[186] = 1;
  haystack[187] = 1;
  haystack[188] = 1;
  haystack[189] = 1;
  haystack[190] = 1;
  haystack[191] = 1;
  haystack[192] = 1;
  haystack[193] = 1;
  haystack[194] = 1;
  haystack[195] = 1;
  haystack[196] = 1;
  haystack[197] = 1;
  haystack[198] = 1;
  haystack[199] = 1;
  haystack[200] = 1;
  haystack[201] = 1;
  haystack[202] = 1;
  haystack[203] = 1;
  haystack[204] = 1;
  haystack[205] = 1;
  haystack[206] = 1;
  haystack[207] = 1;
  haystack[208] = 1;
  haystack[209] = 1;
  haystack[210] = 1;
  haystack[211] = 1;
  haystack[212] = 1;
  haystack[213] = 1;
  haystack[214] = 1;
  haystack[215] = 1;
  haystack[216] = 1;
  haystack[217] = 1;
  haystack[218] = 1;
  haystack[219] = 1;
  haystack[220] = 1;
  haystack[221] = 1;
  haystack[222] = 1;
  haystack[223] = 1;
  haystack[224] = 1;
  haystack[225] = 1;
  haystack[226] = 1;
  haystack[227] = 1;
  haystack[228] = 1;
  haystack[229] = 1;
  haystack[230] = 1;
  haystack[231] = 1;
  haystack[232] = 1;
  haystack[233] = 1;
  haystack[234] = 1;
  haystack[235] = 1;
  haystack[236] = 1;
  haystack[237] = 1;
  haystack[238] = 1;
  haystack[239] = 1;
  haystack[240] = 1;
  haystack[241] = 1;
  haystack[242] = 1;
  haystack[243] = 1;
  haystack[244] = 1;
  haystack[245] = 1;
  haystack[246] = 1;
  haystack[247] = 1;
  haystack[248] = 1;
  haystack[249] = 1;
  haystack[250] = 1;
  haystack[251] = 1;
  haystack[252] = 1;
  haystack[253] = 1;
  haystack[254] = 1;
  haystack[255] = 1;
  haystack[256] = 0;

  int BM_result = BM(haystack, haystack_length, needle, needle_length);
  int Turbo_result = TBM(haystack, haystack_length, needle, needle_length);

  printf("BMH: %d\n", BM_result);
  printf("Turbo: %d\n", Turbo_result);
  
  assert(BM_result == Turbo_result);
}
