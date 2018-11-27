#define LED_COUNT 20


/*
   If you want to save bytes in SRAM, this could be a collection of bytes for each DDRx and PORTx
   on the AVR chip (up to 8 bits or lines) (8 lines would allow for 56 LEDs)
*/
int matrix[LED_COUNT][2][PIN_COUNT] = {
  /*
      Each row in this matrix respresents the pin modes and pin states for a single LED
  */
  //              PIN_CONFIG                          PIN_STATE
  //    A       B       C      D      E         A     B    C    D    E
  { {  INPUT, OUTPUT, INPUT, INPUT, OUTPUT }, { LOW, LOW, LOW, LOW, HIGH } }, // EA 19
  { { OUTPUT, INPUT, INPUT, INPUT, OUTPUT }, { LOW, LOW, LOW, LOW, HIGH } }, // EA 19
  { { INPUT, INPUT, OUTPUT, INPUT, OUTPUT},  { LOW, LOW, LOW, LOW , HIGH  } }, // DC 5
  { { INPUT, INPUT, INPUT, OUTPUT,  OUTPUT}, { LOW, LOW, LOW, LOW, HIGH } }, // BC 2

  { { OUTPUT, OUTPUT, INPUT, INPUT, INPUT }, { LOW, HIGH, LOW, LOW, LOW } },  // DB 13
  { { INPUT, OUTPUT, INPUT, OUTPUT, INPUT }, { LOW, HIGH, LOW, LOW, LOW } },  // DB 14
  { { INPUT, OUTPUT, INPUT, INPUT,  OUTPUT }, { LOW, HIGH, LOW, LOW, LOW } }, // ED 6
  { { INPUT, OUTPUT, OUTPUT, INPUT, INPUT  }, { LOW, HIGH, LOW, LOW, LOW } }, // ED 6

  { { OUTPUT, INPUT, INPUT, INPUT,  OUTPUT }, { HIGH, LOW, LOW, LOW, LOW } }, // AC 8
  { { OUTPUT, INPUT, OUTPUT, INPUT, INPUT }, { HIGH, LOW, LOW, LOW, LOW } }, // CA 9
  { { OUTPUT, OUTPUT, INPUT, INPUT, INPUT }, { HIGH, LOW, LOW, LOW, LOW } }, // CA 9
  { { OUTPUT, INPUT, INPUT, OUTPUT,  INPUT }, { HIGH, LOW, LOW, LOW, LOW } }, // AC 8

  { { INPUT, INPUT, OUTPUT, OUTPUT, INPUT },  { LOW, LOW, LOW, HIGH, LOW } }, // CB 3
  { { INPUT, INPUT, INPUT, OUTPUT,  OUTPUT},  { LOW, LOW, LOW, HIGH, LOW  } },  // BA 1
  { { INPUT, OUTPUT, INPUT, OUTPUT, INPUT }, { LOW, LOW, LOW, HIGH, LOW } },  // DA 15
  { { OUTPUT, INPUT, INPUT, OUTPUT, INPUT }, { LOW, LOW, LOW, HIGH, LOW } },  // BE 16

  { { OUTPUT, INPUT, OUTPUT, INPUT, INPUT }, { LOW, LOW, HIGH, LOW, LOW } },  // EB 17
  { { INPUT, OUTPUT, OUTPUT, INPUT, INPUT }, { LOW, LOW, HIGH, LOW, LOW } },  // AE 18
  { { INPUT, INPUT, OUTPUT, OUTPUT,  INPUT}, { LOW, LOW, HIGH, LOW, LOW } }, // AB 0
  { { INPUT, INPUT, OUTPUT, INPUT,  OUTPUT}, { LOW, LOW, HIGH, LOW,  LOW } } // CD 4

};

void turnOn( int led ) {
  // set all the pin modes
  pinMode( A, matrix[led][PIN_CONFIG][0] );
  pinMode( B, matrix[led][PIN_CONFIG][1] );
  pinMode( C, matrix[led][PIN_CONFIG][2] );
  pinMode( D, matrix[led][PIN_CONFIG][3] );
  pinMode( E, matrix[led][PIN_CONFIG][4] );
  // set all the pin states
  digitalWrite( A, matrix[led][PIN_STATE][0] );
  digitalWrite( B, matrix[led][PIN_STATE][1] );
  digitalWrite( C, matrix[led][PIN_STATE][2] );
  digitalWrite( D, matrix[led][PIN_STATE][3] );
  digitalWrite( E, matrix[led][PIN_STATE][4] );
}

