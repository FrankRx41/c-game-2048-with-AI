enum{
    // normal rule
    WEIGHT_BLANK,
    WEIGHT_NEIGHBOR,
    WEIGHT_MAXNUM,
    WEIGHT_MERGE,
    WEIGHT_BLANK_NEAR_2_OR_4,
    WEIGHT_WAVE,        // smooth, I not sure it is useful and I have to FIX IT

    WEIGHT_CORNER_VALUE,
    WEIGHT_ALL_INSIDE,
    WEIGHT_ALL_AROUND,
    WEIGHT_ALL_NEAR,

    WEIGHT_BIG_AROUND,
    WEIGHT_BIG_INCORNER,
    WEIGHT_BIG_INSIDE,
    WEIGHT_BIG_NEAR,

    WEIGHT_DEPTH,

    WEIGHT_END
};

char *WeightTableName[] = {
    "BLANK",
    "NEIGHBOR",
    "MAXNUM",
    "MERGE",
    "BLANK_NEAR_2",
    "WAVE",
    "CORNER_VALUE",
    "ALL_INSIDE",
    "ALL_AROUND",
    "ALL_NEAR",
    "BIG_AROUND",
    "BIG_INCORNER",
    "BIG_INSIDE",
    "BIG_NEAR",
    "WEIGHT_DEPTH",
};

float WeightTable[][13] = { 
    //{   2.0,    2.0,    2.0,    2.0,    1.5,    1.2,    1.0,    .75,    0.5,    0.5,    0.5,    0.5,    0.5,    },  // NEARBY 
    //{   1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    },  // ALL_AROUND
    //{   0.5,    0.5,    1.0,    1.0,    1.0,    1.2,    0.8,    0.5,    0.2,    0.2,    0.2,    1.0,    0.5,    },  // BIG_NEAR
    //{   0.5,    0.5,    1.0,    1.0,    1.0,    1.2,    0.8,    0.5,    0.2,    0.2,    0.2,    1.0,    0.5,    },  // BIG_NEAR

    //  0       2       4       8       16      32      64      128     256     512     1024    2048    4096
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BLANK
    {   2.0,    2.0,    2.0,    2.0,    1.5,    1.2,    1.0,    .75,    0.5,    0.5,    0.5,    0.5,    0.5,    },  // NEIGHBOR 
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // MAXNUMBER
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    0.5,    1.0,    1.5,    1.5,    2.5,    3.0,    },  // MERGE 
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BLANK_NEAR_2_OR_4 
    {   .25,    .25,    .25,    .25,    .25,    .20,    .15,    .10,    .10,    .05,    .05,    .01,    .25,    },  // WAVE
                                   
    {   0.0,    1.0,    2.0,    3.0,    4.0,    6.0,    8.0,    10.0,   14.,    15.,    16.,    21.,    25.,    },  // CORNER_VALUE
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    },  // ALL_INSIDE
    {   .25,    .25,    .25,    .25,    .25,    .30,    .35,    .40,    .45,    .50,    0.45,   .25,    .25,    },  // ALL_AROUND
    {   .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .10,    },  // ALL_NEAR
                       
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BIG_AROUND
    {   2,      4,      4,      3,      3,      3,      3,      3,      3,      2.5,    2.5,    2,      2,      },  // BIG_INCORNER
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BIG_INSIDE
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BIG_NEAR

    {   1,      .05,    .05,    .05,    .05,    .05,    .05,    .05,    1,      1,      1,      1,      1,      },  // DEPTH
};


/*******************************************************************************
Scoring rules:
Every Blank, Neighbor, Merger will get point

*******************************************************************************/

