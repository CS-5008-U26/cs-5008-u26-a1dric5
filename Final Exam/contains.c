#include <stdio.h>
#include <tg.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <geom-a> <geom-b> <geom-c>\n", argv[0]);
        return 1;
    }

    // Parse the input geometries and check for errors.
    struct tg_geom *a = tg_parse_wkt(argv[1]);
    if (tg_geom_error(a)) {
        fprintf(stderr, "%s\n", tg_geom_error(a));
        return 1;
    }
    struct tg_geom *b = tg_parse_wkt(argv[2]);
    if (tg_geom_error(b)) {
        fprintf(stderr, "%s\n", tg_geom_error(b));
        return 1;
    }
   struct tg_geom *c = tg_parse_wkt(argv[3]); 
   if (tg_geom_error(c)) {
       fprintf(stderr, "%s\n", tg_geom_error(c));
       tg_geom_free(a);
       tg_geom_free(b); 
      tg_geom_free(c); 
      return 1;
  }

    // Execute the "contains" predicate twice: b must contain a, and c must must 
    // contain b. The && short-circuits, so the second test is skipped if the first 
    // one fails. 
    if (tg_geom_intersects(b, a) && tg_geom_contains(c, b)) {
        printf("yes\n");
    } else {
        printf("no\n");
    }

   // USED RESOURCE: https://github.com/tidwall/tg/blob/main/docs/API.md#group___geometry_accessors_1ga0435b45df2158200f3b527d1adb11a62
   // Task 2 written here: reports if the 3rd geometry is a Point. tg_geom_typeof()
   // returns an enum (an integer), so this is an int comparison.
   if (tg_geom_typeof(c) == TG_POINT) {
       printf("POINT\n");
   }

    // Free geometries when done.
    tg_geom_free(a);
    tg_geom_free(b);
    tg_geom_free(c); 
    return 0;
}