#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int days_at_month(int x);

int main(int argc, char **argv) {
    
    int day1=0, mon1=0, year1=0, day2, mon2, year2;
    int ref, dd1, dd2, i;

   // printf("Enter first day, month, year: ");
   // scanf("%02d%02d%04d", &day1, &mon1, &year1);
//    printf("Enter second day,month,year: ");
//    scanf("%02d%02d%04d", &day2, &mon2, &year2);
    for (i=0; argv[1][i] != '\0'; i++) {
        day1 = 10*day1 + (char)argv[1][i] - '0';      
    }
    // printf("%d\n", day1);
    for (i=0; argv[2][i] != '\0'; i++) {        
        mon1 = 10*mon1 + (char)argv[2][i] - '0';
    }
    // printf("%d\n", mon1);
    for (i=0; i<4; i++)
        year1 = 10*year1 + (char)argv[3][i] - '0';
    // printf("%d\n", year1);    

    time_t t = time(0);
    struct tm tm = *localtime(&t);
    
    day2 = tm.tm_mday;
    mon2 = tm.tm_mon+1;
    year2 = tm.tm_year+1900;
    
    //printf("\n");
    
    ref = year1;
    
    // if (year2 < year1)
    //     ref = year2;
    
    dd1 = 0;
    dd1 = days_at_month(mon1);
    
    for (i = ref; i < year1; i++) {
        if (i % 4 == 0)
            dd1 += 1;
    }
    
    dd1 = dd1 + day1 + (year1 - ref)*365;
    // printf("No. of days of first date from the Jan 1 %d= %d\n", year1, dd1);
    
    /* Count for additional days due to leap years*/
    dd2 = 0;
    for (i = ref; i < year2; i++) {
        if (i % 4 == 0)
            dd2 += 1;
    }
    
    dd2 = days_at_month(mon2) + dd2 + day2 + ((year2 - ref)*365);
    
    //printf("No. of days from the reference year's first Jan = %d\n", dd2);
    //printf("Therefore, diff between the two dates is %d\n", dd2 - dd1);
    if (dd2 - dd1 > 30)
        printf("Yes\n", dd2-dd1);

    else
        printf("No\n", dd2-dd1);
}

int days_at_month(int x) //x for month y for dd
{
    int y = 0;
    switch (x) {
        case 1: y = 0;
            break;
        case 2: y = 31;
            break;
        case 3: y = 59;
            break;
        case 4: y = 90;
            break;
        case 5: y = 120;
            break;
        case 6: y = 151;
            break;
        case 7: y = 181;
            break;
        case 8: y = 212;
            break;
        case 9: y = 243;
            break;
        case 10:y = 273;
            break;
        case 11:y = 304;
            break;
        case 12:y = 334;
            break;
        default: printf("Error encountered");
            exit(1);
    }
    
    return y;
}
