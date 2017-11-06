#define WEIGHT_LEN  13

enum{
    // normal rule
    WEIGHT_BLANK,
    WEIGHT_NEIGHBOR,
    WEIGHT_MAXNUM,
    WEIGHT_MERGE,
    WEIGHT_BLANK_NEAR_2_OR_4,
    WEIGHT_WAVE,        // smooth, I not sure it is useful and I have to FIX IT

    WEIGHT_CORNER_NEIGHBOR,
    WEIGHT_CORNER_VALUE,

    WEIGHT_SIDE_LT_CORNER,
    WEIGHT_SIDE_GT_MID,

    WEIGHT_ALL_INSIDE,
    WEIGHT_ALL_AROUND,
    WEIGHT_ALL_NEAR,
    WEIGHT_ALL_TRIANGLE,

    WEIGHT_BIG_AROUND,
    WEIGHT_BIG_INCORNER,
    WEIGHT_BIG_INSIDE,
    WEIGHT_BIG_NEAR,
    WEIGHT_BIG_LINE_NEIGHBOR,
    WEIGHT_BIG_WAVE,

    WEIGHT_IN_BIG_SIDE,

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

    "CORNER_NEIGHBOR",
    "CORNER_VALUE",

    "SIDE < CORNER",
    "SIDE > MID",

    "ALL_INSIDE",
    "ALL_AROUND",
    "ALL_NEAR",
    "ALL_TRIANGLE",

    "BIG_AROUND",
    "BIG_INCORNER",
    "BIG_INSIDE",
    "BIG_NEAR",
    "BLINE_NEIGHBOR",
    "BIG_WAVE",
    "INBIGSIDE",

    "WEIGHT_DEPTH",
};

float WeightTable[][WEIGHT_LEN] = { 
    //{   2.0,    2.0,    2.0,    2.0,    1.5,    1.2,    1.0,    .75,    0.5,    0.5,    0.5,    0.5,    0.5,    },  // NEARBY 
    //{   1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    },  // ALL_AROUND
    //{   0.5,    0.5,    1.0,    1.0,    1.0,    1.2,    0.8,    0.5,    0.2,    0.2,    0.2,    1.0,    0.5,    },  // BIG_NEAR
    //{   0.5,    0.5,    1.0,    1.0,    1.0,    1.2,    0.8,    0.5,    0.2,    0.2,    0.2,    1.0,    0.5,    },  // BIG_NEAR

    //  0       2       4       8       16      32      64      128     256     512     1024    2048    4096
    {   1,      1,      1,      1,      1,      1,      1,      1,      2,      2,      3,      3,      1,      },  // BLANK
    {   2.0,    2.0,    2.0,    2.0,    1.5,    1.2,    1.0,    0.8,    0.6,    0.4,    0.3,    0.2,    0.1,    },  // NEIGHBOR 
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // MAXNUMBER
    {   1,      1,      1,      1,      1,      1,      1,      1,      1.5,    2.0,    2.5,    3.0,    3.5,      },  // MERGE 
    //{   .25,    .25,    .25,    .25,    .25,    .25,    .25,    0.5,    1.0,    1.5,    2.0,    2.5,    3.0,    },  // MERGE 
    {   0.0,    0.0,    0.0,    .25,    .25,    .25,    .25,    .25,    .25,    .50,    .75,    .85,    .95,    },  // BLANK_NEAR_2_OR_4 
    {   .25,    .25,    .25,    .25,    .25,    .20,    .15,    .10,    .10,    .05,    .05,    .01,    .25,    },  // WAVE

    {   0,      0,      0,      0,      0,      0,      0,      1,      1,      1,      1,      1,      1,      },  // CORNER_NEIGHBOR
    {   0.0,    1.0,    2.0,    3.0,    4.0,    6.0,    8.0,    10.0,   14.,    15.,    16.,    21.,    25.,    },  // CORNER_VALUE
                                   
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // SIDE_<_CORNER
    {   0,      0,      0,      0,      0,      0.1,    0.2,    0.3,    0.4,    .95,    1.1,    1.2,     .1,    },  // SIDE_>_MID
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    },  // ALL_INSIDE
    {   .25,    .25,    .25,    .25,    .25,    .30,    .35,    .40,    .45,    .50,    0.45,   .25,    .25,    },  // ALL_AROUND
    {   .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .10,    },  // ALL_NEAR
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    },  // ALL_TRIANGLE

    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1.5,    1.5,    1.5,    1.5,    },  // BIG_AROUND
    {   2,      4,      4,      3,      3,      3,      3,      3,      3,      2.5,    2.5,    2.5,    2.5,    },  // BIG_INCORNER
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BIG_INSIDE
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1.2,    1.5,    1,      1,      },  // BIG_NEAR
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    },  // BIG_LINE_NEIGHBOR
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .35,    .45,    .55,    .25,    },  // BIG_WAVE
    {   .25,    .25,    .25,    .25,    .55,    .55,    .55,    .55,    .55,    .55,    .55,    .55,    .55,    },  // INBIGSIDE

    {   1,      .05,    .05,    .05,    .05,    .05,    .05,    .05,    1,      1,      1,      1,      1,      },  // DEPTH
};


/*******************************************************************************
Scoring rules:
Every Blank, Neighbor, Merger will get point

*******************************************************************************/

