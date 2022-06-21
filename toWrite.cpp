#include "toWrite.h"



String linkedEvent(String lu){
    if (lu=="petit") return "drink";
    if (lu=="eat") return "grand";
    if (lu=="gauche") return "mauvais";
    if (lu=="droite") return "bon";

}

int linkedBlock(String lu){
    if (lu=="messieurs") return 4;
    if (lu=="eat") return 13;
    if (lu=="gauche") return 14;
    if (lu=="droite") return 16;
}
