<?php

class testCCN {

  // should net a ccn of 1
  public function noBody() {
    return;
  }

  // should net a ccn of 2
  public function hasIf() {
    if ( 1 == 3 ) {
      return;
    }
  }

  // public function ccn of 8
  public function ifOnIf() {
    // ccn: 1
    $i = false;
    // ccn: 2
    if ( $i == 2 ) {
      // ccn: 3
      if ( $i == 3 ) {
        // ccn: 4
        if ( $i == 4) {
          // cnn: 5
          if ( $i == 5 ) {
            // ccn: 6
            if ( $i == 6 ) {
              // ccn: 7
              if ( $i == 7 ) {
                // cnn: 8
                if ( $i == 8 ) {

                }
              }
            }
          }
        }
      }
    }
  }
}
