#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int Cur_Vertical_Sep;
int High_Confidence;
int Two_of_Three_Reports_Valid;
int Own_Tracked_Alt;
int Own_Tracked_Alt_Rate;
int Other_Tracked_Alt;
int Alt_Layer_Value;
int Up_Separation;
int Down_Separation;
int Other_RAC;
int Other_Capability;
int Climb_Inhibit;

int Positive_RA_Alt_Thresh_0;
int Positive_RA_Alt_Thresh_1;
int Positive_RA_Alt_Thresh_2;
int Positive_RA_Alt_Thresh_3;

int Own_Below_Threat()
{
    return (Own_Tracked_Alt < Other_Tracked_Alt);
}

int Own_Above_Threat()
{
    return (Other_Tracked_Alt < Own_Tracked_Alt);
}

void initialize()
{
    High_Confidence = 32767;
    Other_Capability = 0;
    
    Climb_Inhibit = 1;
    Cur_Vertical_Sep = 16684;
    Own_Tracked_Alt_Rate = 450;
    
    Positive_RA_Alt_Thresh_0 = 16434;
    Positive_RA_Alt_Thresh_1 = 0;
    Positive_RA_Alt_Thresh_2 = 0;
    Positive_RA_Alt_Thresh_3 = 0;
}

int Positive_RA_Alt_Thresh(int Alt)
{
    int res = 0;

    if(Alt == 0) {
        res = Positive_RA_Alt_Thresh_0;
    }
    else if(Alt == 1) {
        res = Positive_RA_Alt_Thresh_1;
    }
    else if( Alt == 2) {
        res = Positive_RA_Alt_Thresh_2;
    }
    else if(Alt == 3) {
        res = Positive_RA_Alt_Thresh_3;
    }
    return res;
}

int ALIM ()
{
    return Positive_RA_Alt_Thresh(Alt_Layer_Value);
}

int Inhibit_Biased_Climb ()
{
    return Climb_Inhibit ? Up_Separation + 100 : Up_Separation;
}

int Non_Crossing_Biased_Climb()
{
    int upward_preferred;
    int upward_crossing_situation;
    int result;
    
    upward_preferred = Inhibit_Biased_Climb() > Down_Separation;
    
    if (upward_preferred)
    {
        result = !(Own_Below_Threat()) || ((Own_Below_Threat()) && (!(Down_Separation >= ALIM())));
    }
    else
    {
        result = Own_Above_Threat() && (Cur_Vertical_Sep >= 300 ) && (Up_Separation >= ALIM());
    }
    
    return result;
}

int Non_Crossing_Biased_Descend()
{
    int upward_preferred;
    int upward_crossing_situation;
    int result;
    
    upward_preferred = Inhibit_Biased_Climb() > Down_Separation;
    
    if (upward_preferred)
    {
        result = Own_Below_Threat() && (Cur_Vertical_Sep >= 300) && (Down_Separation >= ALIM());
    }
    else
    {
        result = !(Own_Above_Threat()) || ((Own_Above_Threat()) && (Up_Separation >= ALIM()));
    }
    
    return result;
}

int alt_sep_test()
{
    int enabled, tcas_equipped, intent_not_known;
    int need_upward_RA = 0;
    int need_downward_RA = 0;
    
    int alt_sep;
    
    enabled = High_Confidence && (Own_Tracked_Alt_Rate <= 600) && (Cur_Vertical_Sep > 600);
    tcas_equipped = Other_Capability == 1 ;
    intent_not_known = Two_of_Three_Reports_Valid && Other_RAC == 0 ;
    
    alt_sep = 0 ;
    
    if (enabled && ((tcas_equipped && intent_not_known) || !tcas_equipped))
    {
        need_upward_RA = Non_Crossing_Biased_Climb() && Own_Below_Threat() ;
        need_downward_RA = Non_Crossing_Biased_Descend() && Own_Above_Threat();
        
        if(need_upward_RA && need_downward_RA) {
            alt_sep = 0;
        }
        else {
            if (need_upward_RA) {;
                alt_sep = 1;
            }
            else if (need_downward_RA) {
                alt_sep = 2;
            }
            else {
                alt_sep = 0 ;
            }
        }
    }
    
    return alt_sep;
}

int main(int argc, char *argv[1])
{
    if(argc < 13)
    {
        fprintf(stderr, "Error: Command line arguments are\n");
        fprintf(stderr, "Cur_Vertical_Sep, High_Confidence, Two_of_Three_Reports_Valid\n");
        fprintf(stderr, "Own_Tracked_Alt, Own_Tracked_Alt_Rate, Other_Tracked_Alt\n");
        fprintf(stderr, "Alt_Layer_Value, Up_Separation, Down_Separation\n");
        fprintf(stderr, "Other_RAC, Other_Capability, Climb_Inhibit\n");
        exit(1);
    }
    initialize();

    Cur_Vertical_Sep = atoi(argv[1]);
    High_Confidence = atoi(argv[2]);
    Two_of_Three_Reports_Valid = atoi(argv[3]);
    Own_Tracked_Alt = atoi(argv[4]);
    Own_Tracked_Alt_Rate = atoi(argv[5]);
    Other_Tracked_Alt = atoi(argv[6]);
    Alt_Layer_Value = atoi(argv[7]);
    Up_Separation = atoi(argv[8]);
    Down_Separation = atoi(argv[9]);
    Other_RAC = atoi(argv[10]);
    Other_Capability = atoi(argv[11]);
    Climb_Inhibit = atoi(argv[12]);



    fprintf(stderr, "%d\n", alt_sep_test());

    // printf("Inhibit_Biased_Climb: %d\n", Inhibit_Biased_Climb());

    
    // Clause Coverage
    printf("\nClause Coverage\n");
    printf("True = 1, False = 0\n");
    bool c1 = Inhibit_Biased_Climb() > Down_Separation;
    bool c2 = High_Confidence && (Own_Tracked_Alt_Rate <= 600) && (Cur_Vertical_Sep > 600);
    bool c3 = Other_Capability == 1;
    bool c4 = (Two_of_Three_Reports_Valid && Other_RAC == 0);
    bool c5 = !(Other_Capability == 1);
    bool c6 = Non_Crossing_Biased_Climb() && Own_Below_Threat();
    bool c7 = Non_Crossing_Biased_Descend() && Own_Above_Threat();
    printf("\nClause1: upward_preferred = %d\n", c1);
    printf("Clause2: enabled = %d\n", c2);
    printf("Clause3: tcas_equipped = %d\n", c3);
    printf("Clause4: intent_not_known = %d\n", c4);
    printf("Clause5: !tcas_equipped = %d\n", c5);
    printf("Clause6: need_upward_RA = %d\n", c6);
    printf("Clause7: need_downward_RA = %d\n", c7);


    // Predicate Coverage
    printf("\nPredicate Coverage\n");
    printf("True = 1, False = 0\n");
    bool p1 = Climb_Inhibit;
    bool p2 = c1;
    bool p3 = (c2 && ((c3 && c4) || c5));
    bool p4 = c6 && c7;
    bool p5 = c6 && !c7;
    bool p6 = !c6 && c7;
    bool p7 = !c6 && !c7;
    printf("\nPredicate 1: Climb_Inhibit = %d\n", p1);
    printf("Predicate 2: upward_preferred = %d\n", p2);
    printf("Predicate 3: (enabled && ((tcas_equipped && intent_not_known) || !tcas_equipped)) = %d\n", p3);
    printf("Predicate 4: (need_upward_RA) && (need_downward_RA) = %d\n", p4);
    printf("Predicate 5: (need_upward_RA) && !(need_downward_RA) = %d\n", p5);
    printf("Predicate 6: !(need_upward_RA) && (need_downward_RA) = %d\n", p6);
    printf("Predicate 7: !(need_upward_RA) && !(need_downward_RA) = %d\n", p7);
}